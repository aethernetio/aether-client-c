
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

#endif
