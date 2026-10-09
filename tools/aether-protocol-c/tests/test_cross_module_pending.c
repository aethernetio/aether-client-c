

/*
 * Cross-module project validation tests (ТЗ №4-Б1).
 *
 * These tests exercise the project-level global type namespace defined
 * in adsl_project_validate(). They cover:
 *
 *   - cross-file type references without an include directive;
 *   - load-order independence;
 *   - global collisions (same name and case-insensitive name clashes);
 *   - include-only declarations entering the global namespace;
 *   - arrays and nullable cross-file types;
 *   - stream key types resolved across documents;
 *   - the validator being const with respect to the project;
 *   - unknown references rejected at project level.
 *
 * The file is compiled into aether_adslc_cross_module_pending_test
 * (see CMakeLists.txt). adsl_emit_c_project() belongs to ТЗ №4-Б2 and
 * is not exercised here yet.
 */


/* Compiled unconditionally now; see CMakeLists.txt target aether_adslc_cross_module_pending_test. */

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




    /*
     * Project-level generation (adsl_emit_c_project) is not exercised
     * here yet: cross-file serializer references require the emitter to
     * know the owning module of every referenced type and to add an
     * #include for that module's generated _types.h. That is a
     * generated-file layout change and is tracked under ТЗ №4-Б2 with
     * its own architectural review. Until then, this test covers
     * project validation only.
     */


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


/* PENDING_API: include-only declarations must enter the global namespace. */
static void pending_include_only_declaration_resolves(void) {
    char dir[1024];
    fixture_path("registry_project_include", dir, sizeof(dir));

    const char *include_paths[] = { dir };

    char **files = NULL;
    size_t file_count = 0u;

    char error[512] = {0};

    assert(adsl_find_files(
        include_paths, 1u, &files, &file_count, error, sizeof(error)));
    assert(file_count == 2u);

    adsl_project_t project;
    adsl_project_init(&project);

    assert(adsl_project_load_files(
        (const char *const *)files, file_count,
        include_paths, 1u, &project, error, sizeof(error)));

    /*
     * Key is declared only in common.adsl.yaml, which is loaded through
     * ClientServerApi.adsl.yaml's include. ClientHello references it
     * cross-file. Validation must resolve this through the global
     * namespace and not through ClientServerApi's own include scope.
     */
    assert(adsl_project_validate(&project, error, sizeof(error)));

    adsl_project_free(&project);

    for (size_t i = 0u; i < file_count; ++i) {
        free(files[i]);
    }
    free(files);
}

/* PENDING_API: arrays and nullable with cross-file types. */
static void pending_cross_file_array_and_nullable(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/A_uses_item_array_nullable.adsl.yaml",
        a_path, sizeof(a_path));
    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        b_path, sizeof(b_path));

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

    assert(adsl_project_validate(&project, error, sizeof(error)));

    adsl_project_free(&project);
}

/* PENDING_API: stream key type must resolve cross-file. */
static void pending_cross_file_stream_key(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/B_defines_item_stream.adsl.yaml",
        a_path, sizeof(a_path));
    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        b_path, sizeof(b_path));

    const char *paths[] = { a_path, b_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, NULL, 0u,
        &project, error, sizeof(error)));

    /*
     * ItemStream.keys[].type references Item, which is declared in a
     * separate project document. The standalone validator would reject
     * this; the project validator must accept it.
     */
    assert(adsl_project_validate(&project, error, sizeof(error)));

    adsl_project_free(&project);
}

/* PENDING_API: project validator must not modify the project. */
static void pending_project_validate_is_const(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/A_uses_item.adsl.yaml",
        a_path, sizeof(a_path));
    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        b_path, sizeof(b_path));

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



    assert(project.document_count == 2u);

    size_t doc0_types = project.documents[0].type_count;
    size_t doc1_types = project.documents[1].type_count;
    size_t doc0_apis = project.documents[0].api_count;
    size_t doc1_apis = project.documents[1].api_count;

    assert(doc0_types == 0u);
    assert(doc0_apis == 1u);
    assert(doc1_types == 1u);
    assert(doc1_apis == 0u);

    assert(strcmp(project.documents[0].apis[0].name, "ApiA") == 0);
    assert(strcmp(project.documents[1].types[0].name, "Item") == 0);
    assert(strcmp(project.documents[1].types[0].source_path, b_path) == 0);
    assert(strcmp(
        project.documents[0].apis[0].methods[0].params[0].type,
        "Item") == 0);

    const char *doc0_api_name = project.documents[0].apis[0].name;
    const char *doc0_method_param_type =
        project.documents[0].apis[0].methods[0].params[0].type;
    const char *doc1_type_name = project.documents[1].types[0].name;
    const char *doc1_type_source = project.documents[1].types[0].source_path;

    assert(adsl_project_validate(&project, error, sizeof(error)));
    assert(adsl_project_validate(&project, error, sizeof(error)));

    assert(project.document_count == 2u);
    assert(project.documents[0].type_count == doc0_types);
    assert(project.documents[1].type_count == doc1_types);
    assert(project.documents[0].api_count == doc0_apis);
    assert(project.documents[1].api_count == doc1_apis);
    assert(project.documents[0].apis[0].name == doc0_api_name);
    assert(project.documents[0].apis[0].methods[0].params[0].type ==
           doc0_method_param_type);
    assert(project.documents[1].types[0].name == doc1_type_name);
    assert(project.documents[1].types[0].source_path == doc1_type_source);

}


/* PENDING_API: over-long type reference must be rejected by the project validator. */
static void pending_over_long_reference_rejected(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/A_long_reference.adsl.yaml",
        a_path, sizeof(a_path));
    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        b_path, sizeof(b_path));

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

    error[0] = '\0';
    assert(!adsl_project_validate(&project, error, sizeof(error)));
    assert(error[0] != '\0');
    assert(strstr(error, "too long") != NULL);

    adsl_project_free(&project);
}



/* PENDING_API: cross-file API inheritance without include. */
static void pending_cross_file_api_inheritance(void) {
    char base_path[1024];
    char child_path[1024];

    fixture_path(
        "cross_module/D_defines_base_api.adsl.yaml",
        base_path, sizeof(base_path));
    fixture_path(
        "cross_module/E_inherits_base_api.adsl.yaml",
        child_path, sizeof(child_path));

    const char *paths[] = { base_path, child_path };
    const char *include_paths[] = {
        ADSL_FIXTURE_DIR "/cross_module"
    };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, include_paths, 1u,
        &project, error, sizeof(error)));

    /*
     * ChildApi inherits from BaseApi, which is declared in a separate
     * project document. Standalone adsl_collect_effective_methods()
     * resolves parents through document->resolved_apis[] only and would
     * reject this. Project validation must resolve through the
     * project-global API index (Java ApiGenerator.getAllMethods()
     * semantics).
     */
    assert(adsl_project_validate(&project, error, sizeof(error)));

    adsl_project_free(&project);
}

/* PENDING_API: global API name collision must be rejected. */
static void pending_global_api_collision_rejected(void) {
    char e_path[1024];
    char f_path[1024];

    fixture_path(
        "cross_module/E_inherits_base_api.adsl.yaml",
        e_path, sizeof(e_path));
    fixture_path(
        "cross_module/F_duplicate_api_name.adsl.yaml",
        f_path, sizeof(f_path));

    const char *paths[] = { e_path, f_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, NULL, 0u,
        &project, error, sizeof(error)));

    error[0] = '\0';
    assert(!adsl_project_validate(&project, error, sizeof(error)));
    assert(error[0] != '\0');
    assert(strstr(error, "ChildApi") != NULL);

    adsl_project_free(&project);
}




/* PENDING_API: emission plan validation. */
static void pending_emission_plan_validation(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path(
        "cross_module/A_uses_item.adsl.yaml",
        a_path, sizeof(a_path));
    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        b_path, sizeof(b_path));

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

    adsl_emission_module_t modules[2];
    modules[0].source_path = project.documents[0].root_source_path;
    modules[0].base_name = "A_uses_item";
    modules[1].source_path = project.documents[1].root_source_path;
    modules[1].base_name = "B_defines_item";

    adsl_emission_plan_t plan;
    plan.modules = modules;
    plan.module_count = 2u;

    /* Empty plan is rejected as empty. */
    adsl_emission_plan_t empty_plan = {0};
    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &empty_plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "A_uses_item",
        error, sizeof(error)));
    assert(strstr(error, "empty") != NULL);

    /* base_name mismatch for the current document is rejected. */
    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "wrong_name",
        error, sizeof(error)));
    assert(strstr(error, "does not match emission plan") != NULL);

    /* Document index out of range is rejected. */
    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &plan, 99u,
        ADSL_TEST_OUTPUT_DIR, "A_uses_item",
        error, sizeof(error)));
    assert(strstr(error, "out of range") != NULL);

    /* Empty base_name argument is rejected. */
    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "",
        error, sizeof(error)));
    assert(strstr(error, "invalid project emitter arguments") != NULL);

    /*
     * A valid plan must pass every structural check and reach the
     * temporary "not yet wired" step. Asserting the specific diagnostic
     * prevents false positives from unrelated structural failures.
     */
    error[0] = '\0';
    bool emitted = adsl_emit_c_project(
        &project, &plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "A_uses_item",
        error, sizeof(error));
    assert(!emitted);
    assert(strstr(error, "not yet wired") != NULL);

    /* Plan with a missing entry for one document is rejected. */
    adsl_emission_module_t incomplete[1];
    incomplete[0].source_path = project.documents[0].root_source_path;
    incomplete[0].base_name = "A_uses_item";
    adsl_emission_plan_t incomplete_plan;
    incomplete_plan.modules = incomplete;
    incomplete_plan.module_count = 1u;

    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &incomplete_plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "A_uses_item",
        error, sizeof(error)));
    assert(strstr(error, "entries") != NULL);

    /* Plan with an unknown source_path is rejected. */
    adsl_emission_module_t unknown[2];
    unknown[0].source_path = project.documents[0].root_source_path;
    unknown[0].base_name = "A_uses_item";
    unknown[1].source_path = "/nonexistent/path.adsl.yaml";
    unknown[1].base_name = "B_defines_item";
    adsl_emission_plan_t unknown_plan;
    unknown_plan.modules = unknown;
    unknown_plan.module_count = 2u;

    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &unknown_plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "A_uses_item",
        error, sizeof(error)));
    assert(strstr(error, "does not match any project document") != NULL);

    /* Plan entry with empty source_path is rejected. */
    adsl_emission_module_t blank[2];
    blank[0].source_path = project.documents[0].root_source_path;
    blank[0].base_name = "A_uses_item";
    blank[1].source_path = "";
    blank[1].base_name = "B_defines_item";
    adsl_emission_plan_t blank_plan;
    blank_plan.modules = blank;
    blank_plan.module_count = 2u;

    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &blank_plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "A_uses_item",
        error, sizeof(error)));
    assert(strstr(error, "empty source_path or base_name") != NULL);

    /* Colliding module names after snake_name are rejected. */
    adsl_emission_module_t colliding[2];
    colliding[0].source_path = project.documents[0].root_source_path;
    colliding[0].base_name = "SameName";
    colliding[1].source_path = project.documents[1].root_source_path;
    colliding[1].base_name = "same_name";
    adsl_emission_plan_t colliding_plan;
    colliding_plan.modules = colliding;
    colliding_plan.module_count = 2u;

    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &colliding_plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "SameName",
        error, sizeof(error)));
    assert(strstr(error, "colliding module names") != NULL);

    /*
     * Two base_names long enough to overflow the snake_name buffer are
     * rejected before any collision check, so a truncated filename
     * cannot silently pass validation.
     */
    adsl_emission_module_t too_long[2];
    too_long[0].source_path = project.documents[0].root_source_path;
    too_long[0].base_name =
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    too_long[1].source_path = project.documents[1].root_source_path;
    too_long[1].base_name = "B_defines_item";
    adsl_emission_plan_t too_long_plan;
    too_long_plan.modules = too_long;
    too_long_plan.module_count = 2u;

    error[0] = '\0';
    assert(!adsl_emit_c_project(
        &project, &too_long_plan, 0u,
        ADSL_TEST_OUTPUT_DIR, too_long[0].base_name,
        error, sizeof(error)));
    assert(strstr(error, "cannot be normalized") != NULL);

    adsl_project_free(&project);
}



int main(void) {
    pending_res_001_cross_file_without_include();
    pending_res_003_resolution_order_independent();
    pending_res_004_project_validate_rejects_collision();
    pending_res_005_project_validate_rejects_case_collision();

    pending_include_only_declaration_resolves();
    pending_cross_file_array_and_nullable();
    pending_cross_file_stream_key();
    pending_project_validate_is_const();
    pending_over_long_reference_rejected();

    pending_cross_file_api_inheritance();
    pending_global_api_collision_rejected();

    pending_emission_plan_validation();



    return 0;
}


/* end */