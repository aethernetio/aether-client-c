
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

static void free_files(
    char **files,
    size_t file_count) {

    for (size_t i = 0u; i < file_count; ++i) {
        free(files[i]);
    }

    free(files);
}

static void test_scan_returns_two_files(void) {
    char directory[1024];

    fixture_path("project_scan", directory, sizeof(directory));

    const char *include_paths[] = { directory };

    char **files = NULL;
    size_t file_count = 0u;

    char error[512] = {0};

    assert(adsl_find_files(
        include_paths,
        1u,
        &files,
        &file_count,
        error,
        sizeof(error)));

    assert(file_count == 2u);
    assert(files != NULL);

    for (size_t i = 0u; i < file_count; ++i) {
        assert(strstr(files[i], ".adsl.yaml") != NULL);
    }

    free_files(files, file_count);
}

static void test_scan_deduplicates_directories(void) {
    char directory[1024];

    fixture_path("project_scan", directory, sizeof(directory));

    const char *include_paths[] = {
        directory,
        directory,
    };

    char **files = NULL;
    size_t file_count = 0u;

    char error[512] = {0};

    assert(adsl_find_files(
        include_paths,
        2u,
        &files,
        &file_count,
        error,
        sizeof(error)));

    assert(file_count == 2u);

    free_files(files, file_count);
}

static void test_scan_loads_project(void) {
    char directory[1024];

    fixture_path("project_scan", directory, sizeof(directory));

    const char *include_paths[] = { directory };

    char **files = NULL;
    size_t file_count = 0u;

    char error[512] = {0};

    assert(adsl_find_files(
        include_paths,
        1u,
        &files,
        &file_count,
        error,
        sizeof(error)));

    assert(file_count == 2u);

    adsl_project_t project;
    adsl_project_init(&project);

    assert(adsl_project_load_files(
        (const char *const *)files,
        file_count,
        include_paths,
        1u,
        &project,
        error,
        sizeof(error)));

    assert(project.document_count == 2u);
    assert(project.documents != NULL);

    for (size_t i = 0u; i < project.document_count; ++i) {
        assert(project.documents[i].root_source_path != NULL);
        assert(project.documents[i].type_count == 1u);
    }

    adsl_project_free(&project);

    free_files(files, file_count);
}

int main(void) {
    test_scan_returns_two_files();
    test_scan_deduplicates_directories();
    test_scan_loads_project();

    return 0;
}
