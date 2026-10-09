
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

#endif
