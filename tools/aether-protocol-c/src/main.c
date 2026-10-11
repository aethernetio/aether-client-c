

/*
 * Thin CLI front end for the development-time ADSL C compiler.
 *
 * Argument/selection handling belongs here. Parsing/loading belongs in adsl.c;
 * generated C structure and wire policy belongs in emitter.c.
 *
 * The compiler executable is tooling and is never linked into client firmware.
 */
#include "adslc.h"

#include <stdio.h>
#include <string.h>

#include <stdlib.h>





int main(
    int argc,
    char **argv) {

    /*
     * Unified generation mode. The CLI accepts a list of files and
     * directories via repeated --input (a bare positional input list is
     * also accepted for convenience). Directories are scanned recursively
     * for *.adsl.yaml; every document is loaded into a flat pool keyed by
     * basename, and includes resolve by name inside that pool. The single
     * positional argument is the output directory.
     */
    const char *inputs[128];
    size_t input_count = 0u;

    int argument = 1;

    while (argument < argc &&
           argv[argument][0] == '-') {

        if (strcmp(
                argv[argument],
                "--input") == 0) {

            if (argument + 1 >= argc ||
                input_count >=
                    sizeof(inputs) / sizeof(inputs[0])) {

                fprintf(
                    stderr,
                    "usage: aether-adslc --input <dir-or-file>... <output-dir>\n");

                return 2;
            }

            inputs[input_count++] =
                argv[argument + 1];

            argument += 2;
            continue;
        }

        fprintf(
            stderr,
            "unknown option: %s\n",
            argv[argument]);

        return 2;
    }

    /*
     * Trailing positional inputs (up to the last one) are treated as
     * additional --input entries. The final positional is the output
     * directory.
     */
    if (argc - argument < 1) {
        fprintf(
            stderr,
            "usage: aether-adslc --input <dir-or-file>... <output-dir>\n");
        return 2;
    }

    for (int i = argument; i < argc - 1; ++i) {
        if (input_count >= sizeof(inputs) / sizeof(inputs[0])) {
            fprintf(stderr, "too many inputs\n");
            return 2;
        }
        inputs[input_count++] = argv[i];
    }

    if (input_count == 0u) {
        fprintf(
            stderr,
            "usage: aether-adslc --input <dir-or-file>... <output-dir>\n");
        return 2;
    }

    const char *output_directory = argv[argc - 1];

    char error[1024] = {0};

    adsl_project_t project;
    adsl_project_init(&project);

    if (!adsl_project_load_files(
            inputs,
            input_count,
            &project,
            error,
            sizeof(error))) {

        fprintf(stderr, "ADSL load error: %s\n", error);
        adsl_project_free(&project);
        return 1;
    }

    adsl_document_t merged;
    adsl_document_init(&merged);

    if (!adsl_project_merge_into_document(
            &project,
            &merged,
            error,
            sizeof(error))) {

        fprintf(stderr, "ADSL merge error: %s\n", error);
        adsl_project_free(&project);
        adsl_document_free(&merged);
        return 1;
    }

    if (!adsl_validate(
            &merged,
            error,
            sizeof(error))) {

        fprintf(stderr, "ADSL validation error: %s\n", error);
        adsl_project_free(&project);
        adsl_document_free(&merged);
        return 1;
    }

    bool emitted = adsl_emit_c_method(
        &merged,
        output_directory,
        "aether_api",
        NULL,
        NULL,
        error,
        sizeof(error));

    adsl_project_free(&project);
    adsl_document_free(&merged);

    if (!emitted) {
        fprintf(stderr, "ADSL generation error: %s\n", error);
        return 1;
    }

    fprintf(
        stderr,
        "generated aether_api.h/.c from %zu documents\n",
        input_count);

    return 0;
}