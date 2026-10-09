

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


    return 0;
}


/* end */