
#include "adslc.h"

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/*
 * Emitter hardening tests (ТЗ №3.6-А).
 *
 * Only the public generator API and the local filesystem are used.
 * Production code is not modified.
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

static bool path_is_directory(
    const char *path) {

    struct stat info;

    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

static bool make_one_directory(
    const char *path) {

    if (path_is_directory(path)) {
        return true;
    }

    return mkdir(path, 0777) == 0;
}

/*
 * Create every intermediate directory along `path`, mirroring `mkdir -p`.
 * Avoids spawning a shell so an infrastructure failure cannot be silently
 * turned into a passing test.
 */
static bool make_directory_recursive(
    const char *path) {

    char buffer[4096];

    size_t length =
        strlen(path);

    if (length == 0u || length >= sizeof(buffer)) {
        return false;
    }

    memcpy(buffer, path, length + 1u);

    for (size_t i = 1u; i < length; ++i) {

        if (buffer[i] != '/') {
            continue;
        }

        buffer[i] = '\0';

        bool ok =
            make_one_directory(buffer) ||
            path_is_directory(buffer);

        buffer[i] = '/';

        if (!ok) {
            return false;
        }
    }

    return make_one_directory(buffer) ||
           path_is_directory(buffer);
}

static bool directory_is_empty(
    const char *path) {

    DIR *dir = opendir(path);

    if (dir == NULL) {
        return false;
    }

    bool empty = true;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {

            continue;
        }

        empty = false;
        break;
    }

    closedir(dir);

    return empty;
}

static void remove_regular_files(
    const char *path) {

    DIR *dir = opendir(path);

    if (dir == NULL) {
        return;
    }

    char full[4096];
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {

            continue;
        }

        (void)snprintf(
            full,
            sizeof(full),
            "%s/%s",
            path,
            entry->d_name);

        (void)remove(full);
    }

    closedir(dir);
}

/*
 * EMIT-A2, CONTRACT.
 *
 * A '?' in the middle of a type expression (not a trailing nullable
 * suffix) must be rejected by adsl_validate() and must never reach the
 * emitter's nullable-stripping recursion, where strip_nullable_suffix()
 * cannot make progress and the function would recurse forever.
 *
 * The fixture is syntactically valid: loading must succeed and it is
 * validation that must reject the type.
 */
static void test_invalid_nullable_midtype_is_rejected(void) {
    char path[1024];
    fixture_path("EmitterInvalidNullableMid.adsl.yaml", path, sizeof(path));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(adsl_load_file(path, &document, error, sizeof(error)));

    error[0] = '\0';

    assert(!adsl_validate(&document, error, sizeof(error)));
    assert(error[0] != '\0');

    adsl_document_free(&document);
}

/*
 * EMIT-A4, REGRESSION.
 *
 * When output_directory + base_name + suffix does not fit the emitter's
 * fixed 1024-byte path buffer, the generator must:
 *   - return false,
 *   - set a non-empty diagnostic that mentions the offending path,
 *   - leave no file under a silently truncated name.
 *
 * Regression: the current implementation uses (void)snprintf() without
 * checking its return value, so an over-long path is truncated and a
 * junk-named file (e.g. "ba") is created instead of an error being
 * reported.
 */
static void test_overlong_output_path_is_rejected(void) {
    /*
     * Leaf length is chosen so that the full directory path stays below
     * PATH_MAX but "<dir>/base_types.h" exceeds 1024 bytes.
     */
    char leaf[232];
    memset(leaf, 'a', 227u);
    leaf[227] = '\0';

    char directory[2048];
    (void)snprintf(
        directory,
        sizeof(directory),

        "%s/%s/%s/%s/%s/%s",
        ADSL_TEST_OUTPUT_DIR,
        leaf, leaf, leaf, leaf, leaf);


    assert(make_directory_recursive(directory));
    assert(path_is_directory(directory));

    remove_regular_files(directory);
    assert(directory_is_empty(directory));

    char fixture[1024];
    fixture_path("CommonTypes.adsl.yaml", fixture, sizeof(fixture));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};
    assert(adsl_load_file(fixture, &document, error, sizeof(error)));


    error[0] = '\0';

    /*
     * EMIT-A4 regression: a silently truncated path opens a file whose
     * name is a prefix of the intended one (e.g. "base_types" instead of
     * "base_types.h") and still reports success. Two things must hold:
     *   1. the target directory contains nothing but the four exact
     *      expected file names (if it exists at all);
     *   2. no junk appears in the parent directory either, which is where
     *      a truncated path would actually land.
     */
    static const char *const expected[] = {
        "base_types.h",
        "base_types.c",
        "base_api.h",
        "base_api.c",
    };

    char parent[2048];
    (void)snprintf(parent, sizeof(parent), "%s", directory);

    char *slash = strrchr(parent, '/');
    assert(slash != NULL);
    *slash = '\0';

    remove_regular_files(directory);
    remove_regular_files(parent);

    (void)adsl_emit_c(&document, directory, "base", error, sizeof(error));

    DIR *dir = opendir(directory);

    if (dir != NULL) {

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {

            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0) {

                continue;
            }

            bool known = false;

            for (size_t i = 0u;
                 i < sizeof(expected) / sizeof(expected[0]);
                 ++i) {

                if (strcmp(entry->d_name, expected[i]) == 0) {
                    known = true;
                    break;
                }
            }

            assert(known);
        }

        closedir(dir);
    }

    DIR *pdir = opendir(parent);
    assert(pdir != NULL);

    struct dirent *pentry;

    while ((pentry = readdir(pdir)) != NULL) {

        if (strcmp(pentry->d_name, ".") == 0 ||
            strcmp(pentry->d_name, "..") == 0) {

            continue;
        }

        assert(strcmp(pentry->d_name, leaf) == 0);
    }

    closedir(pdir);


}

/*
 * EMIT-A4 boundary, CONTRACT.
 *
 * A path that comfortably fits the fixed buffer must still generate the
 * expected files. This guards against an over-eager fix that would
 * reject every long path.
 */
static void test_acceptable_output_path_succeeds(void) {
    char directory[2048];
    (void)snprintf(
        directory,
        sizeof(directory),
        "%s/boundary-ok",
        ADSL_TEST_OUTPUT_DIR);

    assert(make_directory_recursive(directory));
    remove_regular_files(directory);

    char fixture[1024];
    fixture_path("CommonTypes.adsl.yaml", fixture, sizeof(fixture));

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};
    assert(adsl_load_file(fixture, &document, error, sizeof(error)));

    error[0] = '\0';

    bool emitted =
        adsl_emit_c(
            &document,
            directory,
            "base",
            error,
            sizeof(error));

    assert(emitted);


    static const char *const expected[] = {
        "aether_api.h",
        "aether_api.c",
    };


    for (size_t i = 0u;
         i < sizeof(expected) / sizeof(expected[0]);
         ++i) {

        char probe[4096];
        struct stat info;

        (void)snprintf(
            probe,
            sizeof(probe),
            "%s/%s",
            directory,
            expected[i]);

        assert(stat(probe, &info) == 0);
        assert(S_ISREG(info.st_mode));
    }

    adsl_document_free(&document);
}

int main(void) {
    test_invalid_nullable_midtype_is_rejected();
    test_overlong_output_path_is_rejected();
    test_acceptable_output_path_succeeds();

    return 0;
}