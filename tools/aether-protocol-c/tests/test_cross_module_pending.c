

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

#include "adsl_project_internal.h"


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

/*
 * Read a generated file and check for a substring.
 *
 * Generated-file assertions deliberately test textual presence rather
 * than compiling the output: at D6.5 cross-module #include is not yet
 * generated, so the inter-module output is not yet compilable. The
 * check still catches the two D6 regressions we care about: a module
 * silently emitting a foreign declaration, or dropping an inherited
 * method from the effective set.
 */
static bool file_contains(const char *path, const char *needle) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return false;
    }
    char buffer[65536];
    size_t n = fread(buffer, 1, sizeof(buffer) - 1u, f);
    fclose(f);
    buffer[n] = '\0';
    return strstr(buffer, needle) != NULL;
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



/*
 * PENDING_API: include-only declarations must enter the global
 * namespace of the single root that pulled them in.
 *
 * The project has exactly one root (ClientServerApi.adsl.yaml). Its
 * include chain reaches common.adsl.yaml, which declares Key. Key is
 * used by ClientHello without a local declaration, so validation must
 * resolve it through the root's closure. Under policy B ownership of
 * Key belongs to that single root.
 */
static void pending_include_only_declaration_resolves(void) {
    char dir[1024];
    char root_path[1024];

    fixture_path("registry_project_include", dir, sizeof(dir));
    fixture_path(
        "registry_project_include/ClientServerApi.adsl.yaml",
        root_path, sizeof(root_path));

    const char *include_paths[] = { dir };
    const char *paths[] = { root_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 1u, include_paths, 1u,
        &project, error, sizeof(error)));

    assert(adsl_project_validate(&project, error, sizeof(error)));

    adsl_project_free(&project);
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
     * A valid plan must pass every structural check and actually emit
     * the four generated files for the selected module. Document index 0
     * is A_uses_item, whose base_name matches the plan entry, and only
     * that document's own declarations must land in the generated
     * module (cross-module references are resolved by the resolver but
     * never duplicated into another module's output).
     */
    error[0] = '\0';
    bool emitted = adsl_emit_c_project(
        &project, &plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "A_uses_item",
        error, sizeof(error));
    assert(emitted);
    assert(error[0] == '\0');


{
        char path[1024];

        /*
         * The API header and source must reference the method name and
         * the cross-document parameter type by name, and must use a
         * contextual wire id of 3 for the single method.
         */
        (void)snprintf(path, sizeof(path),
            "%s/a_uses_item_api.h", ADSL_TEST_OUTPUT_DIR);
        assert(file_contains(path, "api_a_use_item("));
        assert(file_contains(path, "item_t"));

        (void)snprintf(path, sizeof(path),
            "%s/a_uses_item_api.c", ADSL_TEST_OUTPUT_DIR);
        assert(file_contains(path, "api_a_use_item("));
        assert(file_contains(path, "= 3u;"));
        assert(file_contains(path, "a_uses_item_serialize_item("));

        /*
         * A owns only its own ApiA declaration. Types declared in B
         * (Item) must not be redefined here, and B's serializers must
         * not be duplicated into A's module.
         */
        (void)snprintf(path, sizeof(path),
            "%s/a_uses_item_types.h", ADSL_TEST_OUTPUT_DIR);
        assert(!file_contains(path, "item_t"));
        assert(!file_contains(path, "b_defines_item"));

        (void)snprintf(path, sizeof(path),
            "%s/a_uses_item_types.c", ADSL_TEST_OUTPUT_DIR);
        assert(!file_contains(path, "b_defines_item"));
    }



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




/* PENDING_API: cross-file inheritance in the global hierarchy view. */
static void pending_cross_file_hierarchy_resolves(void) {
    char base_path[1024];
    char child_path[1024];

    fixture_path(
        "cross_module/G_defines_base.adsl.yaml",
        base_path, sizeof(base_path));
    fixture_path(
        "cross_module/H_extends_base.adsl.yaml",
        child_path, sizeof(child_path));

    const char *paths[] = { base_path, child_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, NULL, 0u,
        &project, error, sizeof(error)));

    /*
     * Base is declared in G, Child extends it in H, without an include
     * directive. Project validation must succeed: both documents
     * contribute to the same global namespace, and the parent chain is
     * resolved through the project-global type index.
     */
    assert(adsl_project_validate(&project, error, sizeof(error)));

    /*
     * The project resolver exposes both physical declarations through
     * the global view, so a hierarchy walk initiated from either module
     * sees both Base and Child.
     */
    adsl_project_resolver_t *resolver = NULL;
    assert(adsl_project_resolver_build(
        &project, &resolver, error, sizeof(error)));

    const adsl_type_t *base =
        adsl_project_resolver_type(resolver, "Base");
    const adsl_type_t *child =
        adsl_project_resolver_type(resolver, "Child");

    assert(base != NULL);
    assert(child != NULL);
    assert(child->parent != NULL);
    assert(strcmp(child->parent, "Base") == 0);

    size_t hierarchy_count = adsl_project_resolver_type_count(resolver);
    assert(hierarchy_count >= 2u);

    bool saw_base = false;
    bool saw_child = false;

    for (size_t i = 0u; i < hierarchy_count; ++i) {
        const adsl_type_t *entry =
            adsl_project_resolver_type_at(resolver, i);

        assert(entry != NULL);
        assert(entry->name != NULL);

        if (strcmp(entry->name, "Base") == 0) {
            saw_base = true;
        }
        if (strcmp(entry->name, "Child") == 0) {
            saw_child = true;
        }
    }

    assert(saw_base);
    assert(saw_child);

    adsl_project_resolver_release(resolver);
    adsl_project_free(&project);
}

/* PENDING_API: by-value field of a type declared in another document. */
static void pending_cross_file_by_value_field(void) {
    char holder_path[1024];
    char item_path[1024];

    fixture_path(
        "cross_module/I_holds_external.adsl.yaml",
        holder_path, sizeof(holder_path));
    fixture_path(
        "cross_module/B_defines_item.adsl.yaml",
        item_path, sizeof(item_path));

    const char *paths[] = { holder_path, item_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, NULL, 0u,
        &project, error, sizeof(error)));

    /*
     * Holder.item references Item, declared in a separate project
     * document without an include directive. Global namespace
     * resolution must accept this at the project validation stage.
     *
     * Generating the corresponding C code requires the owning module's
     * full definition of Item to be visible before Holder is emitted;
     * that is the subject of ТЗ №4-Б2.4 and is not exercised here yet.
     */
    assert(adsl_project_validate(&project, error, sizeof(error)));

    adsl_project_free(&project);

}


/*
 * Cross-document API inheritance must produce a child module whose
 * effective method set flattens its parent API's methods.
 *
 * Both documents are top-level project roots; the child document does
 * not include the parent document. The project resolver must therefore
 * be the source of truth for parent lookup, and the effective method
 * set must contain both the inherited and the own method.
 */
static void pending_cross_file_inheritance_emission(void) {
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

    assert(adsl_project_validate(&project, error, sizeof(error)));

    adsl_emission_module_t modules[2];
    modules[0].source_path = project.documents[0].root_source_path;
    modules[0].base_name = "D_defines_base_api";
    modules[1].source_path = project.documents[1].root_source_path;
    modules[1].base_name = "E_inherits_base_api";

    adsl_emission_plan_t plan;
    plan.modules = modules;
    plan.module_count = 2u;

    error[0] = '\0';
    assert(adsl_emit_c_project(
        &project, &plan, 0u,
        ADSL_TEST_OUTPUT_DIR, "D_defines_base_api",
        error, sizeof(error)));

    error[0] = '\0';
    assert(adsl_emit_c_project(
        &project, &plan, 1u,
        ADSL_TEST_OUTPUT_DIR, "E_inherits_base_api",
        error, sizeof(error)));

    char path[1024];

    /*
     * D owns only its own API. It must emit basePing and must not emit
     * any symbol from E's ChildApi.
     */
    (void)snprintf(path, sizeof(path),
        "%s/d_defines_base_api_api.h", ADSL_TEST_OUTPUT_DIR);
    assert(file_contains(path, "base_api_base_ping("));
    assert(!file_contains(path, "child_api_"));

    (void)snprintf(path, sizeof(path),
        "%s/d_defines_base_api_api.c", ADSL_TEST_OUTPUT_DIR);
    assert(file_contains(path, "base_api_base_ping("));
    assert(!file_contains(path, "child_api_"));

    /*
     * E flattens the inherited method set. Its effective API name is
     * ChildApi, so both the inherited basePing and its own childPing
     * appear under the child_api_ prefix; BaseApi's own module prefix
     * must not leak into E's output.
     */
    (void)snprintf(path, sizeof(path),
        "%s/e_inherits_base_api_api.h", ADSL_TEST_OUTPUT_DIR);
    assert(file_contains(path, "child_api_base_ping("));
    assert(file_contains(path, "child_api_child_ping("));

    (void)snprintf(path, sizeof(path),
        "%s/e_inherits_base_api_api.c", ADSL_TEST_OUTPUT_DIR);
    assert(file_contains(path, "child_api_base_ping("));
    assert(file_contains(path, "child_api_child_ping("));

    adsl_project_free(&project);

}


/*
 * B2.4.1: type_owner_base_snake must map a cross-document type to the
 * owning module, and must fail loudly when the owner cannot be
 * determined. Standalone mode keeps the current module as owner.
 */
static void pending_type_owner_lookup(void) {
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

    assert(adsl_project_validate(&project, error, sizeof(error)));

    adsl_project_resolver_t *resolver = NULL;
    assert(adsl_project_resolver_build(
        &project, &resolver, error, sizeof(error)));

    adsl_emission_module_t modules[2];
    modules[0].source_path = project.documents[0].root_source_path;
    modules[0].base_name = "A_uses_item";
    modules[1].source_path = project.documents[1].root_source_path;
    modules[1].base_name = "B_defines_item";

    adsl_emission_plan_t plan;
    plan.modules = modules;
    plan.module_count = 2u;

    const adsl_type_t *item =
        adsl_project_resolver_type(resolver, "Item");
    assert(item != NULL);

    char owner[256];

    /* Project mode: Item is owned by module B. */
    emitter_ctx_t project_ctx;
    memset(&project_ctx, 0, sizeof(project_ctx));
    project_ctx.document = &project.documents[0];
    project_ctx.resolver = resolver;
    project_ctx.project = &project;
    project_ctx.plan = &plan;
    (void)snprintf(project_ctx.base_snake,
        sizeof(project_ctx.base_snake), "a_uses_item");

    error[0] = '\0';
    assert(type_owner_base_snake(
        &project_ctx, item, owner, sizeof(owner),
        error, sizeof(error)));
    assert(strcmp(owner, "b_defines_item") == 0);

    /* Standalone mode: always the current module. */
    emitter_ctx_t standalone_ctx;
    memset(&standalone_ctx, 0, sizeof(standalone_ctx));
    standalone_ctx.document = &project.documents[0];
    (void)snprintf(standalone_ctx.base_snake,
        sizeof(standalone_ctx.base_snake), "a_uses_item");

    error[0] = '\0';
    assert(type_owner_base_snake(
        &standalone_ctx, item, owner, sizeof(owner),
        error, sizeof(error)));
    assert(strcmp(owner, "a_uses_item") == 0);

    /* Project mode without an owner entry in the plan must fail. */
    adsl_emission_module_t broken[1];
    broken[0].source_path = project.documents[0].root_source_path;
    broken[0].base_name = "A_uses_item";

    adsl_emission_plan_t broken_plan;
    broken_plan.modules = broken;
    broken_plan.module_count = 1u;

    emitter_ctx_t broken_ctx;
    memset(&broken_ctx, 0, sizeof(broken_ctx));
    broken_ctx.document = &project.documents[0];
    broken_ctx.resolver = resolver;
    broken_ctx.project = &project;
    broken_ctx.plan = &broken_plan;
    (void)snprintf(broken_ctx.base_snake,
        sizeof(broken_ctx.base_snake), "a_uses_item");

    error[0] = '\0';
    assert(!type_owner_base_snake(
        &broken_ctx, item, owner, sizeof(owner),
        error, sizeof(error)));
    assert(error[0] != '\0');

    adsl_project_resolver_release(resolver);
    adsl_project_free(&project);
}



/*
 * B2.4.1: two roots whose closures both contain the same physical
 * declaration must be rejected. The owner of the module-prefixed C
 * symbols would be ambiguous: both roots would want to emit
 * "<owner>_serialize_Key" and friends.
 */
static void pending_two_roots_common_include_rejected(void) {
    char dir[1024];
    char a_path[1024];
    char b_path[1024];

    fixture_path("registry_project_include", dir, sizeof(dir));
    fixture_path(
        "registry_project_include/ClientServerApi.adsl.yaml",
        a_path, sizeof(a_path));
    fixture_path(
        "registry_project_include/common.adsl.yaml",
        b_path, sizeof(b_path));

    const char *include_paths[] = { dir };
    const char *paths[] = { a_path, b_path };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, include_paths, 1u,
        &project, error, sizeof(error)));

    error[0] = '\0';
    assert(!adsl_project_validate(&project, error, sizeof(error)));
    assert(error[0] != '\0');
    assert(strstr(error, "ambiguous C module ownership") != NULL);
    assert(strstr(error, "Key") != NULL);

    adsl_project_free(&project);
}


int main(void) {



    pending_res_001_cross_file_without_include();
    pending_res_003_resolution_order_independent();
    pending_res_004_project_validate_rejects_collision();
    pending_res_005_project_validate_rejects_case_collision();


    pending_include_only_declaration_resolves();
    pending_two_roots_common_include_rejected();

    pending_cross_file_array_and_nullable();
    pending_cross_file_stream_key();
    pending_project_validate_is_const();
    pending_over_long_reference_rejected();

    pending_cross_file_api_inheritance();
    pending_global_api_collision_rejected();

    pending_emission_plan_validation();

    pending_cross_file_hierarchy_resolves();

    pending_cross_file_by_value_field();

    pending_cross_file_inheritance_emission();
    pending_type_owner_lookup();






    return 0;
}


/* end */