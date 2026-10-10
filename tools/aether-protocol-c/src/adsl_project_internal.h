
/*
 * Private interface between adsl.c and emitter.c.
 *
 * This header is not part of the public API. It exposes an opaque
 * project resolver so that adsl_project_validate() and the project
 * emitter share a single semantic for cross-document type and API
 * resolution. Without this shared interface, validator and generator
 * could diverge on which names are visible, which would produce
 * generated C that does not match the schema that was accepted.
 *
 * Ownership:
 *   - adsl_project_resolver_build() allocates the resolver on the heap
 *     and populates it from the given project. On success the caller
 *     owns the returned pointer and must release it with
 *     adsl_project_resolver_release().
 *   - The resolver does not own the project. The project must remain
 *     alive for the resolver's lifetime.
 *   - The returned declaration pointers point into the project's
 *     documents and are valid for as long as the project is.
 */

#ifndef AETHER_ADSL_PROJECT_INTERNAL_H
#define AETHER_ADSL_PROJECT_INTERNAL_H

#include "adslc.h"

typedef struct adsl_project_resolver adsl_project_resolver_t;

bool adsl_project_resolver_build(
    const adsl_project_t *project,
    adsl_project_resolver_t **out_resolver,
    char *error,
    size_t error_capacity);

void adsl_project_resolver_release(
    adsl_project_resolver_t *resolver);

const adsl_type_t *adsl_project_resolver_type(
    const adsl_project_resolver_t *resolver,
    const char *name);

const adsl_api_t *adsl_project_resolver_api(
    const adsl_project_resolver_t *resolver,
    const char *name);

bool adsl_project_resolver_collect_effective(
    const adsl_project_resolver_t *resolver,
    const adsl_api_t *api,
    adsl_effective_method_t **out_methods,
    size_t *out_count,
    char *error,

    size_t error_capacity);

/*
 * Enumerate every distinct physical user type known to the project,
 * deduplicated by (source_path, name).
 *
 * This is the enumeration that hierarchy walks must use in project
 * mode: unlike ctx_types()/ctx_apis() in emitter.c, which return the
 * current document's own declarations for generation, this view exposes
 * the global type space so that descendants declared in other documents
 * are visible when computing polymorphism, hierarchy refs and
 * serialization switch tables.
 *
 * Entries are stable for the resolver's lifetime. The caller must not
 * mutate or free the returned pointers.
 */
size_t adsl_project_resolver_type_count(
    const adsl_project_resolver_t *resolver);

const adsl_type_t *adsl_project_resolver_type_at(
    const adsl_project_resolver_t *resolver,

    size_t index);

/*
 * Root source path of the document that owns the given type declaration
 * for symbol naming purposes.
 *
 * The project resolver registers each declaration together with the
 * root_source_path of the root document whose closure contains it. In
 * project mode this is the module that will own "<owner>_serialize_X",
 * "<owner>_deserialize_X", "<owner>_tiered_int_limits" and similar
 * module-prefixed symbols.
 *
 * Returns NULL if the declaration is not registered in the resolver.
 */
const char *adsl_project_resolver_type_source(
    const adsl_project_resolver_t *resolver,
    const adsl_type_t *type);



/*
 * Emitter context shared between adsl.c and emitter.c.
 *
 * Fields:
 *   - document: the document currently being emitted. Enumeration of
 *     what to generate uses this document's own types[]/apis[]
 *     (project mode) or resolved_types[]/resolved_apis[] (standalone).
 *   - resolver: NULL in standalone mode; in project mode, the
 *     project-global resolver shared with adsl_project_validate().
 *   - project / plan: NULL in standalone mode; in project mode, the
 *     project and its emission plan. These allow the emitter to
 *     identify the owner module of a declaration for cross-module C
 *     symbol naming.
 *   - base_snake: normalized base_name of the module currently being
 *     emitted ("a_uses_item"). Filled by the top-level entrypoints.
 *
 * No global mutable state: contexts are created on the stack and
 * passed by pointer.
 */
typedef struct {
    const adsl_document_t *document;
    const adsl_project_resolver_t *resolver;
    const adsl_project_t *project;
    const adsl_emission_plan_t *plan;

    char module_abbr[32];

    char base_snake[256];
} emitter_ctx_t;

/*
 * Resolve the module prefix (base_snake) of the module that owns the
 * generated C symbols for the given type declaration.
 *
 * Standalone mode: every type in the include closure belongs to the
 * single generated module; returns ctx->base_snake.
 *
 * Project mode: finds the root document whose closure contains the
 * declaration and maps it to the emission plan's base_name. Returns
 * an error if the owner cannot be determined unambiguously. Never
 * falls back to the current module: two modules referencing the same
 * ADSL type must agree on who owns "<owner>_serialize_<type>" and
 * friends. The C type name itself (e.g. "item_t") is not
 * module-prefixed and is not affected.
 */
bool type_owner_base_snake(
    const emitter_ctx_t *ctx,
    const adsl_type_t *type,
    char *out,
    size_t capacity,
    char *error,
    size_t error_capacity);

#endif