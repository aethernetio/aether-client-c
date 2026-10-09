
#include "adslc.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Cross-module type resolution tests (ТЗ №4-А).
 *
 * Two categories are covered here:
 *
 *   CONTRACT   -- behaviour that already works and must keep working.
 *   REGRESSION -- diagnostics that freeze the *current* (wrong) behaviour
 *                 of the standalone C generator relative to Java. These
 *                 tests pass today because they assert exactly what the
 *                 current code does; they document the defect that
 *                 ТЗ №4-Б must fix.
 *
 * Tests that require the not-yet-designed project API
 * (adsl_project_validate(), adsl_emit_c_project()) are deliberately kept
 * in a separate translation unit, test_cross_module_pending.c, so the
 * ordinary build neither compiles nor runs them. See that file for the
 * expected future contract.
 *
 * Contract frozen here (Scenario 1, Java-compatible):
 *   - All types of a loaded project share one global namespace of simple
 *     names (case-insensitive).
 *   - `include` does NOT define type visibility.
 *   - Two conflicting declarations of the same simple name are an error.
 *   - Module.Type is not supported syntax.
 */

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

static bool load_single(
    const char *name,
    adsl_document_t *document,
    char *error,
    size_t error_capacity) {

    char path[1024];
    fixture_path(name, path, sizeof(path));

    adsl_document_init(document);

    return adsl_load_file(path, document, error, error_capacity);
}


/*
 * RES-002, CONTRACT.
 *
 * A reference to a type declared in another file resolves when the
 * referencing file carries `include: B`. This is the include-closure
 * behaviour the loader and emitter implement today and must be preserved
 * after ТЗ №4-Б.
 */
static void test_res_002_cross_file_with_include(void) {
    adsl_document_t document;
    char error[512] = {0};

    assert(load_single(
        "cross_module/A_includes_b.adsl.yaml",
        &document,
        error,
        sizeof(error)));

    assert(adsl_validate(&document, error, sizeof(error)));

    error[0] = '\0';

    assert(adsl_emit_c(
        &document,
        ADSL_TEST_OUTPUT_DIR,
        "res_002",
        error,
        sizeof(error)));

    adsl_document_free(&document);
}


/*
 * RES-003 (loading only), CONTRACT.
 *
 * Loading order must not change the set of documents that ends up in the
 * project. This test says nothing about type resolution — that is the
 * concern of the PENDING_API tests.
 */
static void test_res_003_load_order_stability(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/A_includes_b.adsl.yaml",
        a_path,
        sizeof(a_path));

    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        b_path,
        sizeof(b_path));

    const char *include_paths[] = {
        ADSL_FIXTURE_DIR "/cross_module"
    };

    char error[512] = {0};

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

        assert(adsl_project_load_files(
            paths,
            2u,
            include_paths,
            1u,
            &project,
            error,
            sizeof(error)));

        assert(project.document_count == 2u);

        adsl_project_free(&project);
    }
}


/*
 * RES-010, CONTRACT.
 *
 * A reference to a type that does not exist anywhere must be rejected by
 * standalone validation with a non-empty diagnostic that names the type.
 */
static void test_res_010_unknown_type(void) {
    adsl_document_t document;
    char error[512] = {0};

    assert(load_single(
        "cross_module/A_uses_item.adsl.yaml",
        &document,
        error,
        sizeof(error)));

    error[0] = '\0';

    assert(!adsl_validate(&document, error, sizeof(error)));
    assert(error[0] != '\0');
    assert(strstr(error, "Item") != NULL);

    adsl_document_free(&document);
}


/*
 * RES-011, CONTRACT.
 *
 * Registry never owns declarations: freeing the registry must leave the
 * project's declarations usable, and freeing the project afterwards must
 * not double-free.
 */
static void test_res_011_registry_ownership(void) {
    char path[1024];
    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        path,
        sizeof(path));

    const char *paths[] = { path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths,
        1u,
        NULL,
        0u,
        &project,
        error,
        sizeof(error)));

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);

    assert(adsl_type_registry_build(
        &project,
        &registry,
        error,
        sizeof(error)));

    const adsl_registry_entry_t *item =
        adsl_type_registry_find(&registry, "B_defines_item", "Item");

    assert(item != NULL);
    assert(item->declaration != NULL);

    const adsl_type_t *declaration = item->declaration;

    adsl_type_registry_free(&registry);

    assert(declaration->name != NULL);
    assert(strcmp(declaration->name, "Item") == 0);

    adsl_project_free(&project);
}


/*
 * RES-001, REGRESSION (diagnostic of the current limitation).
 *
 * Java resolves a reference to a type declared in another project file
 * even when the referencing file does not `include` it, because Java uses
 * a single global type table. In the current C generator the emitter only
 * sees a single document's resolved_types[], which is that document's own
 * include closure. A cross-file reference without include cannot be
 * resolved.
 *
 * This test freezes the current, wrong behaviour: adsl_emit_c() on A must
 * fail with an "unknown" type diagnostic while B is loaded in the same
 * project. ТЗ №4-Б must change this; the future contract is expressed in
 * test_cross_module_pending.c.
 */
static void test_res_001_current_limitation(void) {
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
        paths,
        2u,
        include_paths,
        1u,
        &project,
        error,
        sizeof(error)));

    adsl_document_t *a = NULL;

    for (size_t i = 0u; i < project.document_count; ++i) {
        if (strstr(
                project.documents[i].root_source_path,
                "A_uses_item") != NULL) {

            a = &project.documents[i];
            break;
        }
    }

    assert(a != NULL);

    error[0] = '\0';

    bool emitted =
        adsl_emit_c(
            a,
            ADSL_TEST_OUTPUT_DIR,
            "res_001",
            error,
            sizeof(error));

    /*
     * Current behaviour: emitting A fails because the standalone emitter
     * does not see the project-wide type table. When this assertion
     * starts failing, it means the fix landed; remove this test at that
     * point and rely on the PENDING_API contract instead.
     */
    assert(!emitted);
    assert(error[0] != '\0');
    assert(strstr(error, "unknown") != NULL ||
           strstr(error, "Item") != NULL);

    adsl_project_free(&project);
}


/*
 * RES-004, REGRESSION (diagnostic of the current limitation).
 *
 * Java rejects a project in which two different files declare the same
 * simple type name: Validator.addCanonicalName() raises an "Ambiguous Type
 * declaration" error.
 *
 * Current C behaviour: adsl_project_load_files() accepts both documents
 * and adsl_type_registry_build() accepts both declarations, because the
 * registry key is (module, name) and modules differ. No global uniqueness
 * check exists yet.
 *
 * This test freezes the current, wrong acceptance. ТЗ №4-Б adds
 * adsl_project_validate() (see test_cross_module_pending.c) which must
 * reject this project.
 */
static void test_res_004_current_accepts_global_collision(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        a_path,
        sizeof(a_path));

    fixture_path(
        "cross_module/C_defines_item_too.adsl.yaml",
        b_path,
        sizeof(b_path));

    const char *paths[] = { a_path, b_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    bool loaded =
        adsl_project_load_files(
            paths,
            2u,
            NULL,
            0u,
            &project,
            error,
            sizeof(error));

    assert(loaded);

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);

    error[0] = '\0';

    bool built =
        adsl_type_registry_build(
            &project,
            &registry,
            error,
            sizeof(error));

    /* Two modules, same simple name: both declarations are accepted. */
    assert(built);
    assert(adsl_type_registry_find(
        &registry, "B_defines_item", "Item") != NULL);
    assert(adsl_type_registry_find(
        &registry, "C_defines_item_too", "Item") != NULL);

    adsl_type_registry_free(&registry);
    adsl_project_free(&project);
}


/*
 * RES-005, REGRESSION (diagnostic of the current limitation).
 *
 * Name comparison must be case-insensitive, as in Java: `Widget` and
 * `widget` are the same logical type and must collide.
 *
 * Current C behaviour: modules differ, so the registry accepts both. No
 * case-insensitive global check exists yet.
 */
static void test_res_005_current_accepts_case_collision(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/case_upper.adsl.yaml",
        a_path,
        sizeof(a_path));

    fixture_path(
        "cross_module/case_lower.adsl.yaml",
        b_path,
        sizeof(b_path));

    const char *paths[] = { a_path, b_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    bool loaded =
        adsl_project_load_files(
            paths,
            2u,
            NULL,
            0u,
            &project,
            error,
            sizeof(error));

    assert(loaded);

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);

    error[0] = '\0';

    bool built =
        adsl_type_registry_build(
            &project,
            &registry,
            error,
            sizeof(error));

    assert(built);

    adsl_type_registry_free(&registry);
    adsl_project_free(&project);
}


int main(void) {
    test_res_002_cross_file_with_include();
    test_res_003_load_order_stability();
    test_res_010_unknown_type();
    test_res_011_registry_ownership();

    /* REGRESSION diagnostics — see file header. */
    test_res_001_current_limitation();
    test_res_004_current_accepts_global_collision();
    test_res_005_current_accepts_case_collision();

    return 0;
}
