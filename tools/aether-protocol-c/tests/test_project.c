
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

static void test_project_two_documents(void) {
    char a_path[1024];
    char b_path[1024];

    fixture_path("A.adsl.yaml", a_path, sizeof(a_path));
    fixture_path("B.adsl.yaml", b_path, sizeof(b_path));

    const char *paths[] = {
        a_path,
        b_path
    };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths,
        2u,
        &project,
        error,
        sizeof(error)));

    assert(project.document_count == 2u);
    assert(project.documents != NULL);

    assert(project.documents[0].root_source_path != NULL);
    assert(strstr(
        project.documents[0].root_source_path,
        "A.adsl.yaml") != NULL);

    assert(project.documents[1].root_source_path != NULL);
    assert(strstr(
        project.documents[1].root_source_path,
        "B.adsl.yaml") != NULL);

    adsl_project_free(&project);

    assert(project.document_count == 0u);
    assert(project.documents == NULL);
}

int main(void) {
    test_project_two_documents();

    return 0;
}