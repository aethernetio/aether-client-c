
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

static void load_registry_project(
    adsl_project_t *project) {

    char dir[1024];
    fixture_path("registry_project", dir, sizeof(dir));

    const char *include_paths[] = { dir };

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

    adsl_project_init(project);

    assert(adsl_project_load_files(
        (const char *const *)files,
        file_count,
        include_paths,
        1u,
        project,
        error,
        sizeof(error)));

    for (size_t i = 0u; i < file_count; ++i) {
        free(files[i]);
    }

    free(files);
}

static void test_declared_lookup(void) {
    adsl_project_t project;
    load_registry_project(&project);

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);

    char error[512] = {0};

    assert(adsl_type_registry_build(
        &project,
        &registry,
        error,
        sizeof(error)));

    const adsl_registry_entry_t *key =
        adsl_type_registry_find(&registry, "common", "Key");

    assert(key != NULL);
    assert(key->kind == ADSL_TYPE_DECLARED);
    assert(key->declaration != NULL);
    assert(strcmp(key->module, "common") == 0);
    assert(strcmp(key->name, "Key") == 0);

    const adsl_registry_entry_t *hello =
        adsl_type_registry_find(
            &registry, "ClientServerApi", "ClientHello");

    assert(hello != NULL);
    assert(hello->kind == ADSL_TYPE_DECLARED);
    assert(hello->declaration != NULL);

    assert(adsl_type_registry_find(
        &registry, "common", "ClientHello") == NULL);

    adsl_type_registry_free(&registry);
    adsl_project_free(&project);
}

static void test_primitive_lookup(void) {
    adsl_project_t project;
    load_registry_project(&project);

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);

    char error[512] = {0};

    assert(adsl_type_registry_build(
        &project,
        &registry,
        error,
        sizeof(error)));

    const adsl_registry_entry_t *int_entry =
        adsl_type_registry_find(&registry, NULL, "int");

    assert(int_entry != NULL);
    assert(int_entry->kind == ADSL_TYPE_PRIMITIVE);
    assert(int_entry->declaration == NULL);
    assert(int_entry->module == NULL);

    /* Aliases are registered independently, not normalized. */
    assert(adsl_type_registry_find(&registry, NULL, "bool") != NULL);
    assert(adsl_type_registry_find(&registry, NULL, "boolean") != NULL);

    assert(adsl_type_registry_find(
        &registry, NULL, "definitely_not_a_type") == NULL);

    /* Primitives are not addressable via a module namespace. */
    assert(adsl_type_registry_find(&registry, "common", "int") == NULL);

    adsl_type_registry_free(&registry);
    adsl_project_free(&project);
}

static void test_entry_count(void) {
    adsl_project_t project;
    load_registry_project(&project);

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);

    char error[512] = {0};

    assert(adsl_type_registry_build(
        &project,
        &registry,
        error,
        sizeof(error)));

    /* 2 declared types + 13 builtin scalars. */
    assert(registry.entry_count == 15u);

    adsl_type_registry_free(&registry);
    adsl_project_free(&project);
}

static void test_duplicate_module(void) {
    char path_a[1024];
    char path_b[1024];

    fixture_path("registry_dup_a/common.adsl.yaml", path_a, sizeof(path_a));
    fixture_path("registry_dup_b/common.adsl.yaml", path_b, sizeof(path_b));

    char dir_a[1024];
    char dir_b[1024];

    fixture_path("registry_dup_a", dir_a, sizeof(dir_a));
    fixture_path("registry_dup_b", dir_b, sizeof(dir_b));

    const char *paths[] = { path_a, path_b };
    const char *include_paths[] = { dir_a, dir_b };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths,
        2u,
        include_paths,
        2u,
        &project,
        error,
        sizeof(error)));

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);

    assert(!adsl_type_registry_build(
        &project,
        &registry,
        error,
        sizeof(error)));

    assert(strstr(error, "duplicate module") != NULL);
    assert(registry.entry_count == 0u);
    assert(registry.entries == NULL);

    adsl_type_registry_free(&registry);
    adsl_project_free(&project);
}

static void test_free_safety(void) {
    adsl_type_registry_t empty;
    adsl_type_registry_init(&empty);

    adsl_type_registry_free(&empty);
    assert(empty.entries == NULL);
    assert(empty.entry_count == 0u);

    adsl_project_t project;
    load_registry_project(&project);

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);

    char error[512] = {0};

    assert(adsl_type_registry_build(
        &project,
        &registry,
        error,
        sizeof(error)));

    assert(registry.entry_count > 0u);
    assert(registry.entries != NULL);

    adsl_type_registry_free(&registry);

    assert(registry.entries == NULL);
    assert(registry.entry_count == 0u);

    /* Second free must be a no-op. */
    adsl_type_registry_free(&registry);

    adsl_project_free(&project);
}


static void test_registry_include_not_attributed_to_root(void) {
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

    adsl_type_registry_t registry;
    adsl_type_registry_init(&registry);
    assert(adsl_type_registry_build(
        &project, &registry, error, sizeof(error)));

    /* Key is declared only in common; include must not reassign ownership. */
    assert(adsl_type_registry_find(&registry, "common", "Key") != NULL);
    assert(adsl_type_registry_find(
        &registry, "ClientServerApi", "Key") == NULL);
    assert(adsl_type_registry_find(
        &registry, "ClientServerApi", "ClientHello") != NULL);

    adsl_type_registry_free(&registry);
    adsl_project_free(&project);
    for (size_t i = 0u; i < file_count; ++i) free(files[i]);
    free(files);
}



int main(void) {
    test_declared_lookup();
    test_primitive_lookup();
    test_entry_count();
    test_duplicate_module();

    test_free_safety();
    test_registry_include_not_attributed_to_root();


    return 0;
}