/*
 * Stage 1 regression: unified merge of equal ADSL input files.
 *
 * Every input file is a peer. A physical declaration is identified by
 * (source_path, name); reaching the same declaration through the include
 * closures of several documents must yield exactly one merged declaration
 * and must not raise an ownership error.
 */

#include "adslc.h"

#include <assert.h>
#include <stdio.h>
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

static const adsl_api_t *find_api_name(
    const adsl_document_t *document,
    const char *name) {

    for (size_t i = 0u; i < document->api_count; ++i) {
        if (strcmp(document->apis[i].name, name) == 0) {
            return &document->apis[i];
        }
    }

    return NULL;
}

static size_t count_type_name(
    const adsl_document_t *document,
    const char *name) {

    size_t count = 0u;

    for (size_t i = 0u; i < document->type_count; ++i) {
        if (strcmp(document->types[i].name, name) == 0) {
            count++;
        }
    }

    return count;
}

static void load_and_merge(
    const char *const *file_names,
    size_t file_count,
    adsl_document_t *merged,
    char *error,
    size_t error_capacity) {

    char paths[8][1024];

    assert(file_count <= 8u);

    const char *path_ptrs[8];

    for (size_t i = 0u; i < file_count; ++i) {
        fixture_path(file_names[i], paths[i], sizeof(paths[i]));
        path_ptrs[i] = paths[i];
    }

    adsl_project_t project;
    adsl_project_init(&project);


    assert(adsl_project_load_files(
        path_ptrs,
        file_count,
        &project,
        error,
        error_capacity));


    assert(adsl_project_merge_into_document(
        &project,
        merged,
        error,
        error_capacity));

    adsl_project_free(&project);
}

static void test_common_include_merges_once(void) {

    const char *files[] = {
        "unified_merge/Common.adsl.yaml",
        "unified_merge/FirstApi.adsl.yaml",
        "unified_merge/SecondApi.adsl.yaml",
    };

    adsl_document_t merged;
    adsl_document_init(&merged);

    char error[512] = {0};

    load_and_merge(files, 3u, &merged, error, sizeof(error));

    assert(count_type_name(&merged, "Item") == 1u);
    assert(find_api_name(&merged, "FirstApi") != NULL);
    assert(find_api_name(&merged, "SecondApi") != NULL);

    adsl_document_free(&merged);
}

static void test_input_order_is_irrelevant(void) {

    const char *forward[] = {
        "unified_merge/Common.adsl.yaml",
        "unified_merge/FirstApi.adsl.yaml",
        "unified_merge/SecondApi.adsl.yaml",
    };

    const char *reversed[] = {
        "unified_merge/SecondApi.adsl.yaml",
        "unified_merge/FirstApi.adsl.yaml",
        "unified_merge/Common.adsl.yaml",
    };

    adsl_document_t merged_forward;
    adsl_document_init(&merged_forward);

    adsl_document_t merged_reversed;
    adsl_document_init(&merged_reversed);

    char error[512] = {0};

    load_and_merge(
        forward, 3u, &merged_forward, error, sizeof(error));

    load_and_merge(
        reversed, 3u, &merged_reversed, error, sizeof(error));

    assert(merged_forward.type_count == merged_reversed.type_count);
    assert(merged_forward.api_count == merged_reversed.api_count);

    assert(count_type_name(&merged_forward, "Item") == 1u);
    assert(count_type_name(&merged_reversed, "Item") == 1u);

    assert(find_api_name(&merged_forward, "FirstApi") != NULL);
    assert(find_api_name(&merged_forward, "SecondApi") != NULL);
    assert(find_api_name(&merged_reversed, "FirstApi") != NULL);
    assert(find_api_name(&merged_reversed, "SecondApi") != NULL);

    adsl_document_free(&merged_forward);
    adsl_document_free(&merged_reversed);
}

int main(void) {
    test_common_include_merges_once();
    test_input_order_is_irrelevant();

    return 0;
}