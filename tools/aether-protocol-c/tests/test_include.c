
#include "adslc.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static void test_adjacent_include(void) {
    char path[1024];

    fixture_path(
        "IncludeRoot.adsl.yaml",
        path,
        sizeof(path));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_load_file(
            path,
            &document,
            error,
            sizeof(error)));


    /* IncludeRoot declares the API itself but no types. */
    assert(document.type_count == 0u);
    assert(document.api_count == 1u);

    /* Everything arrives through the include closure. */
    assert(document.resolved_type_count == 1u);
    assert(document.resolved_api_count == 1u);

    assert(
        strcmp(
            document.resolved_types[0].name,
            "SensorReading") == 0);

    assert(
        strcmp(
            document.resolved_apis[0].name,
            "SensorApi") == 0);


    assert(
        adsl_validate(
            &document,
            error,
            sizeof(error)));


    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    assert(
        adsl_collect_effective_methods(
            &document,
            &document.resolved_apis[0],
            &effective,
            &effective_count,
            error,
            sizeof(error)));

    assert(effective_count == 1u);
    assert(effective[0].id == 3u);

    free(effective);


    adsl_document_free(&document);
}

static void test_include_cycle(void) {
    char path[1024];

    fixture_path(
        "CycleA.adsl.yaml",
        path,
        sizeof(path));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_load_file(
            path,
            &document,
            error,
            sizeof(error)));

    assert(document.resolved_type_count == 2u);

    assert(
        adsl_validate(
            &document,
            error,
            sizeof(error)));

    adsl_document_free(&document);
}

static void test_sibling_include(void) {
    char path[1024];

    fixture_path(
        "SiblingRoot/Root.adsl.yaml",
        path,
        sizeof(path));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_load_file(
            path,
            &document,
            error,
            sizeof(error)));


    /* SiblingRoot declares the API itself but no types. */
    assert(document.type_count == 0u);
    assert(document.api_count == 1u);
    assert(document.resolved_type_count == 1u);
    assert(document.resolved_api_count == 1u);


    assert(
        strcmp(
            document.resolved_types[0].name,
            "SiblingValue") == 0);

    assert(
        adsl_validate(
            &document,
            error,
            sizeof(error)));

    adsl_document_free(&document);
}


static void test_explicit_include_search_path(void) {
    char root_path[1024];
    fixture_path(
        "SearchRoot/Root.adsl.yaml",
        root_path,
        sizeof(root_path));

    char include_path[1024];
    fixture_path(
        "SearchPath",
        include_path,
        sizeof(include_path));

    const char *include_paths[] = {
        include_path
    };

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_load_file_with_include_paths(
            root_path,
            include_paths,
            1u,
            &document,
            error,
            sizeof(error)));


    /* SearchRoot declares the API itself but no types. */
    assert(document.type_count == 0u);
    assert(document.api_count == 1u);
    assert(document.resolved_type_count == 1u);
    assert(document.resolved_api_count == 1u);


    assert(
        strcmp(
            document.resolved_types[0].name,
            "SharedValue") == 0);

    assert(
        adsl_validate(
            &document,
            error,
            sizeof(error)));

    adsl_document_free(&document);
}


static void test_duplicate_include(void) {
    char path[1024];

    fixture_path(
        "DuplicateIncludeRoot.adsl.yaml",
        path,
        sizeof(path));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_load_file(
            path,
            &document,
            error,
            sizeof(error)));


    /* DuplicateIncludeRoot declares nothing itself. */
    assert(document.type_count == 0u);
    assert(document.api_count == 0u);
    assert(document.resolved_type_count == 2u);
    assert(document.resolved_api_count == 1u);


    assert(
        strcmp(
            document.resolved_types[0].name,
            "SharedType") == 0 ||
        strcmp(
            document.resolved_types[1].name,
            "SharedType") == 0);

    assert(
        strcmp(
            document.resolved_apis[0].name,
            "SharedApi") == 0);

    adsl_document_free(&document);
}




static void test_root_types_are_root_owned(void) {
    char path[1024];
    fixture_path("CommonTypes.adsl.yaml", path, sizeof(path));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_load_file(
            path,
            &document,
            error,
            sizeof(error)));

    /* No includes: root-owned and resolved closures coincide. */
    assert(document.type_count == 1u);
    assert(document.resolved_type_count == 1u);

    assert(
        strcmp(
            document.types[0].name,
            "SensorReading") == 0);

    assert(
        strcmp(
            document.resolved_types[0].name,
            "SensorReading") == 0);

    /*
     * Deep-copy ownership: resolved entries are independent allocations,
     * not aliases of the root declarations array.
     */
    assert(document.resolved_types != document.types);

    adsl_document_free(&document);
}




static void test_duplicate_api_in_includes_is_merged(void) {
    char path[1024];
    fixture_path("IncludeApiRoot.adsl.yaml", path, sizeof(path));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_load_file(
            path,
            &document,
            error,
            sizeof(error)));

    /* Root declares nothing itself. */
    assert(document.api_count == 0u);

    /* Both includes declare the same API; normalization keeps one. */
    assert(document.resolved_api_count == 1u);
    assert(
        strcmp(
            document.resolved_apis[0].name,
            "SharedApi") == 0);

    adsl_document_free(&document);
}


static void test_conflicting_api_in_includes_fails(void) {
    char path[1024];
    fixture_path("IncludeApiConflictRoot.adsl.yaml", path, sizeof(path));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        !adsl_load_file(
            path,
            &document,
            error,
            sizeof(error)));

    assert(strstr(error, "conflicting api declaration") != NULL);

    adsl_document_free(&document);
}



int main(void) {
    test_adjacent_include();
    test_include_cycle();
    test_sibling_include();
    test_duplicate_include();

    test_explicit_include_search_path();

    test_root_types_are_root_owned();
    test_duplicate_api_in_includes_is_merged();
    test_conflicting_api_in_includes_fails();



    return 0;
}