/*
 * Stage 2 regression: cross-file type references in unified generation.
 *
 * File A declares Holder { Item single; Item[] many; }. File B declares
 * Item. The generator emits one aether_api.h/.c where Holder symbols carry
 * A's prefix and Item symbols carry B's prefix; every reference from Holder
 * to Item (field, array alias, serializer/deserializer calls) must use B's
 * prefix, not A's.
 *
 * The test does not stop at textual assertions: it compiles the generated
 * aether_api.c together with a consumer and the meta runtime, and links
 * them, so that a declaration/definition/call mismatch fails the test.
 */

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

static char *read_file(const char *path) {

    FILE *f = fopen(path, "rb");

    assert(f != NULL);

    (void)fseek(f, 0L, SEEK_END);
    long size = ftell(f);
    assert(size >= 0);
    (void)fseek(f, 0L, SEEK_SET);

    char *buffer = malloc((size_t)size + 1u);
    assert(buffer != NULL);

    size_t read = fread(buffer, 1u, (size_t)size, f);
    fclose(f);

    buffer[read] = '\0';

    return buffer;
}

static void assert_contains(
    const char *text,
    const char *needle,
    const char *what) {

    if (strstr(text, needle) == NULL) {
        fprintf(
            stderr,
            "missing %s: expected substring '%s'\n",
            what,
            needle);
        assert(0);
    }
}

static void test_generation_and_compile(void) {

    char a_path[1024];
    char b_path[1024];

    fixture_path("unified_types/A_defines_holder.adsl.yaml", a_path, sizeof(a_path));
    fixture_path("unified_types/B_defines_item.adsl.yaml", b_path, sizeof(b_path));

    const char *paths[] = { a_path, b_path };
    const char *include_paths[] = {
        ADSL_FIXTURE_DIR "/unified_types"
    };

    adsl_project_t project;
    adsl_project_init(&project);

    char error[512] = {0};

    assert(adsl_project_load_files(
        paths, 2u, include_paths, 1u, &project, error, sizeof(error)));

    adsl_document_t merged;
    adsl_document_init(&merged);

    assert(adsl_project_merge_into_document(
        &project, &merged, error, sizeof(error)));

    assert(adsl_validate(&merged, error, sizeof(error)));

    assert(adsl_emit_c_method(
        &merged,
        ADSL_TEST_OUTPUT_DIR,
        "aether_api",
        NULL,
        NULL,
        error,
        sizeof(error)));

    adsl_document_free(&merged);
    adsl_project_free(&project);

    char header_path[1200];
    char source_path[1200];

    (void)snprintf(header_path, sizeof(header_path),
        "%s/aether_api.h", ADSL_TEST_OUTPUT_DIR);
    (void)snprintf(source_path, sizeof(source_path),
        "%s/aether_api.c", ADSL_TEST_OUTPUT_DIR);

    char *header = read_file(header_path);
    char *source = read_file(source_path);

    /* Holder is declared in A -> adh_ prefix. */
    assert_contains(header, "adh_holder_t", "holder type name");
    assert_contains(header, "adh_serialize_holder", "holder serializer");

    /* Item is declared in B -> bdi_ prefix. */
    assert_contains(header, "bdi_item_t", "item type name");
    assert_contains(header, "bdi_serialize_item", "item serializer");

    /* Cross-file: Holder's field and array alias use B's prefix. */
    assert_contains(header, "bdi_item_t single", "holder single field");
    assert_contains(header, "bdi_item_array_view_t", "item array alias");
    assert_contains(header, "const bdi_item_t *data", "array alias element");

    /* Holder's impl calls B's item serializer/deserializer. */
    assert_contains(source, "bdi_serialize_item", "cross-file serialize call");
    assert_contains(source, "bdi_deserialize_item", "cross-file deserialize call");

    /* Unified mode is one translation unit: no cross-module _with_depth. */
    assert(strstr(source, "_with_depth") == NULL);

    /* No phantom A-prefixed Item symbols. */
    assert(strstr(source, "adh_serialize_item") == NULL);
    assert(strstr(header, "adh_item_t") == NULL);


    /*
     * A type-only unified document has no API, so no transport
     * declarations must be generated: no send-fn typedef and no remote
     * transport struct. Emitting them with an empty prefix would be dead
     * and could collide across unrelated outputs.
     */
    assert(strstr(header, "_send_fn") == NULL);
    assert(strstr(header, "_remote_t") == NULL);

    free(header);
    free(source);

    /*
     * Compile the generated .c with the runtime and link a consumer. This
     * catches any declaration/definition/call mismatch that textual checks
     * cannot.
     */
    char command[4096];

    (void)snprintf(
        command,
        sizeof(command),
        "%s -std=c11 -Wall -Wextra -Werror "
        "-I%s -I%s "
        "-c %s/aether_api.c -o %s/aether_api.o",
        ADSL_CC,
        ADSL_TEST_OUTPUT_DIR,
        AETHER_RUNTIME_INC,
        ADSL_TEST_OUTPUT_DIR,
        ADSL_TEST_OUTPUT_DIR);

    int rc = system(command);
    assert(rc == 0);

    (void)snprintf(
        command,
        sizeof(command),
        "%s -std=c11 -Wall -Wextra -Werror -I%s -c %s -o %s/aether_meta_runtime.o",
        ADSL_CC,
        AETHER_RUNTIME_INC,
        AETHER_RUNTIME_SRC,
        ADSL_TEST_OUTPUT_DIR);

    rc = system(command);
    assert(rc == 0);

    char consumer_path[1200];
    (void)snprintf(consumer_path, sizeof(consumer_path),
        "%s/consumer.c", ADSL_TEST_OUTPUT_DIR);

    FILE *consumer = fopen(consumer_path, "wb");
    assert(consumer != NULL);

    fputs(
        "#include \"aether_api.h\"\n"
        "#include <stddef.h>\n"
        "#include <string.h>\n"
        "int main(void) {\n"
        "    adh_holder_t in;\n"
        "    memset(&in, 0, sizeof(in));\n"
        "    in.single.value = 7;\n"
        "    bdi_item_t items[2];\n"
        "    memset(items, 0, sizeof(items));\n"
        "    items[0].value = 11;\n"
        "    items[1].value = 22;\n"
        "    in.many.data = items;\n"
        "    in.many.length = 2u;\n"
        "    uint8_t tx[256];\n"
        "    size_t pos = 0u;\n"
        "    if (adh_serialize_holder(tx, sizeof(tx), &pos, &in) != AETHER_OK) return 1;\n"
        "    uint8_t scratch[256];\n"
        "    aether_meta_read_context_t ctx = {0};\n"
        "    ctx.reader.data = tx;\n"
        "    ctx.reader.length = pos;\n"
        "    ctx.scratch = scratch;\n"
        "    ctx.scratch_capacity = sizeof(scratch);\n"
        "    adh_holder_t out;\n"
        "    memset(&out, 0, sizeof(out));\n"
        "    if (adh_deserialize_holder(&ctx, &out) != AETHER_OK) return 2;\n"
        "    if (out.single.value != 7) return 3;\n"
        "    if (out.many.length != 2u) return 4;\n"
        "    if (out.many.data == NULL) return 5;\n"
        "    if (out.many.data[0].value != 11) return 6;\n"
        "    if (out.many.data[1].value != 22) return 7;\n"
        "    return 0;\n"
        "}\n",
        consumer);

    fclose(consumer);

    (void)snprintf(
        command,
        sizeof(command),
        "%s -std=c11 -Wall -Wextra -Werror -I%s -I%s -c %s -o %s/consumer.o",
        ADSL_CC,
        ADSL_TEST_OUTPUT_DIR,
        AETHER_RUNTIME_INC,
        consumer_path,
        ADSL_TEST_OUTPUT_DIR);

    rc = system(command);
    assert(rc == 0);

    (void)snprintf(
        command,
        sizeof(command),
        "%s %s/aether_api.o %s/consumer.o %s/aether_meta_runtime.o "
        "-o %s/unified_types_demo",
        ADSL_CC,
        ADSL_TEST_OUTPUT_DIR,
        ADSL_TEST_OUTPUT_DIR,
        ADSL_TEST_OUTPUT_DIR,
        ADSL_TEST_OUTPUT_DIR);

    rc = system(command);
    assert(rc == 0);



    /*
     * Run the linked consumer. It performs a real serialize/deserialize
     * round-trip (single + non-empty Item[]), so a wire-level defect or a
     * silent symbol mismatch that still links cleanly fails here.
     */
    (void)snprintf(
        command,
        sizeof(command),
        "%s/unified_types_demo",
        ADSL_TEST_OUTPUT_DIR);

    rc = system(command);
    assert(rc == 0);

}

int main(void) {
    test_generation_and_compile();

    return 0;
}