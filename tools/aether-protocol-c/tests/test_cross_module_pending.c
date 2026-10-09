
/*
 * PENDING_API contract tests (ТЗ №4-А).
 *
 * These tests express the *target* behaviour of cross-module type
 * resolution that ТЗ №4-Б must implement. They reference two public APIs
 * that do not exist yet:
 *
 *     bool adsl_project_validate(
 *         const adsl_project_t *project,
 *         char *error,
 *         size_t error_capacity);
 *
 *     bool adsl_emit_c_project(
 *         const adsl_project_t *project,
 *         size_t document_index,
 *         const char *output_directory,
 *         const char *base_name,
 *         char *error,
 *         size_t error_capacity);
 *
 * The exact signatures are still subject to review in №4-Б. This file is
 * compiled only when AETHER_ENABLE_PENDING_API_TESTS is defined and is
 * NOT registered as a running CTest test. The ordinary build does not
 * compile it, so it never references the missing symbols and never
 * masquerades as a passing test.
 */

#if defined(AETHER_ENABLE_PENDING_API_TESTS)

#include "adslc.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fixture_path(
    const char *name,
    char *path,
    size_t capacity) {

    (void)snprintf(
        path,
        capacity,
        "%s/%s",
        ADSL_FIXTURE_DIR,
        name);
}

/* PENDING_API RES-001: cross-file reference without include. */
static void pending_res_001_cross_file_without_include(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/A_uses_item.adsl.yaml",
        a_path,
        sizeof(a_path));

    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        b_path,
        sizeof(b_path));

    const char *paths[] = { a_path, b_path };
    const char *include_paths[] = {
        ADSL_FIXTURE_DIR "/cross_module"
    };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, include_paths, 1u,
        &project, error, sizeof(error)));

    /* Project-level validation must accept this project. */
    assert(adsl_project_validate(&project, error, sizeof(error)));

    /* Project-level generation of document 0 (A) must succeed. */
    assert(adsl_emit_c_project(
        &project, 0u,
        ADSL_TEST_OUTPUT_DIR, "pending_res_001",
        error, sizeof(error)));

    adsl_project_free(&project);
}

/* PENDING_API RES-004: global collision must be rejected. */
static void pending_res_004_project_validate_rejects_collision(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        a_path, sizeof(a_path));

    fixture_path(
        "cross_module/C_defines_item_too.adsl.yaml",
        b_path, sizeof(b_path));

    const char *paths[] = { a_path, b_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, NULL, 0u,
        &project, error, sizeof(error)));

    error[0] = '\0';

    assert(!adsl_project_validate(&project, error, sizeof(error)));
    assert(error[0] != '\0');
    assert(strstr(error, "Item") != NULL);

    adsl_project_free(&project);
}

/* PENDING_API RES-005: case-insensitive collision must be rejected. */
static void pending_res_005_project_validate_rejects_case_collision(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/case_upper.adsl.yaml",
        a_path, sizeof(a_path));

    fixture_path(
        "cross_module/case_lower.adsl.yaml",
        b_path, sizeof(b_path));

    const char *paths[] = { a_path, b_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, NULL, 0u,
        &project, error, sizeof(error)));

    error[0] = '\0';

    assert(!adsl_project_validate(&project, error, sizeof(error)));
    assert(error[0] != '\0');

    adsl_project_free(&project);
}

/* PENDING_API RES-003: resolution is independent of load order. */
static void pending_res_003_resolution_order_independent(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/A_uses_item.adsl.yaml",
        a_path, sizeof(a_path));

    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        b_path, sizeof(b_path));

    const char *include_paths[] = {
        ADSL_FIXTURE_DIR "/cross_module"
    };

    for (int order = 0; order < 2; ++order) {

        const char *paths[2];

        if (order == 0) {
            paths[0] = a_path;
            paths[1] = b_path;
        } else {
            paths[0] = b_path;
            paths[1] = a_path;
        }

        adsl_project_t project;
        adsl_project_init(&project);

        char error[512] = {0};

        assert(adsl_project_load_files(
            paths, 2u, include_paths, 1u,
            &project, error, sizeof(error)));

        assert(adsl_project_validate(&project, error, sizeof(error)));

        adsl_project_free(&project);
    }
}

int main(void) {
    pending_res_001_cross_file_without_include();
    pending_res_003_resolution_order_independent();
    pending_res_004_project_validate_rejects_collision();
    pending_res_005_project_validate_rejects_case_collision();

    return 0;
}

#endif /* AETHER_ENABLE_PENDING_API_TESTS */
