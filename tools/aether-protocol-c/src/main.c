

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


static const char *file_base_name(
    const char *path) {

    const char *base =
        path;

    for (const char *p = path;
         *p != '\0';
         ++p) {

        if (*p == '/' ||
            *p == '\\') {

            base =
                p + 1;
        }
    }

    return base;
}

static void derive_base_name(
    const char *path,
    char *output,
    size_t capacity) {

    const char *base =
        file_base_name(path);

    (void)snprintf(
        output,
        capacity,
        "%s",
        base);

    char *suffix =
        strstr(
            output,
            ".adsl.yaml");

    if (suffix == NULL) {
        suffix =
            strstr(
                output,
                ".adsl.yml");
    }

    if (suffix != NULL) {
        *suffix =
            '\0';
    }
}

int main(
    int argc,
    char **argv) {

    const char *include_paths[64];
    size_t include_path_count = 0u;

    const char *api_selection = NULL;
    const char *method_selection = NULL;

    const char *local_api_selection = NULL;
    const char *response_method_selection = NULL;


    int argument = 1;

    while (argument < argc &&
           argv[argument][0] == '-') {

        if (strcmp(
                argv[argument],
                "-I") == 0) {

            if (argument + 1 >= argc ||
                include_path_count >=
                    sizeof(include_paths) /
                        sizeof(include_paths[0])) {

                fprintf(
                    stderr,
                    "usage: aether-adslc [-I <include-dir>]... [--api ApiName | --method Api.method | --local-api ApiName] <input.adsl.yaml> <output-dir> [base-name]\n");

                return 2;
            }

            include_paths[include_path_count++] =
                argv[argument + 1];

            argument += 2;
            continue;
        }

        if (strcmp(
                argv[argument],
                "--api") == 0) {


            if (argument + 1 >= argc ||
                api_selection != NULL ||
                method_selection != NULL ||
                local_api_selection != NULL ||
                response_method_selection != NULL) {


                fprintf(
                    stderr,
                    "usage: aether-adslc [-I <include-dir>]... [--api ApiName | --method Api.method | --local-api ApiName] <input.adsl.yaml> <output-dir> [base-name]\n");

                return 2;
            }

            api_selection =
                argv[argument + 1];

            if (api_selection[0] == '\0' ||
                strchr(
                    api_selection,
                    '.') != NULL) {

                fprintf(
                    stderr,
                    "--api must be a single API name\n");

                return 2;
            }

            argument += 2;
            continue;
        }

        if (strcmp(
                argv[argument],
                "--local-api") == 0) {


            if (argument + 1 >= argc ||
                api_selection != NULL ||
                method_selection != NULL ||
                local_api_selection != NULL ||
                response_method_selection != NULL) {


                fprintf(
                    stderr,
                    "usage: aether-adslc [-I <include-dir>]... [--api ApiName | --method Api.method | --local-api ApiName] <input.adsl.yaml> <output-dir> [base-name]\n");

                return 2;
            }

            local_api_selection =
                argv[argument + 1];

            if (local_api_selection[0] == '\0' ||
                strchr(
                    local_api_selection,
                    '.') != NULL) {

                fprintf(
                    stderr,
                    "--local-api must be a single API name\n");

                return 2;
            }

            argument += 2;
            continue;
        }

        if (strcmp(
                argv[argument],
                "--method") == 0) {


            if (argument + 1 >= argc ||
                method_selection != NULL ||
                api_selection != NULL ||
                local_api_selection != NULL ||
                response_method_selection != NULL) {


                fprintf(
                    stderr,
                    "usage: aether-adslc [-I <include-dir>]... [--api ApiName | --method Api.method | --local-api ApiName] <input.adsl.yaml> <output-dir> [base-name]\n");

                return 2;
            }

            method_selection =
                argv[argument + 1];

            argument += 2;
            continue;
        }


        if (strcmp(
                argv[argument],
                "--response-method") == 0) {

            if (argument + 1 >= argc ||
                response_method_selection != NULL ||
                method_selection != NULL ||
                api_selection != NULL ||
                local_api_selection != NULL) {

                fprintf(
                    stderr,
                    "usage: aether-adslc [-I <include-dir>]... [--api ApiName | --method Api.method | --local-api ApiName | --response-method Api.method] <input.adsl.yaml> <output-dir> [base-name]\n");

                return 2;
            }

            response_method_selection =
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

    int positional_count =
        argc - argument;

    if (positional_count < 2 ||
        positional_count > 3) {

        fprintf(
            stderr,
            "usage: aether-adslc [-I <include-dir>]... [--api ApiName | --method Api.method | --local-api ApiName] <input.adsl.yaml> <output-dir> [base-name]\n");

        return 2;
    }

    const char *input_path =
        argv[argument];

    const char *output_directory =
        argv[argument + 1];

    char base_name[256];

    if (positional_count == 3) {
        (void)snprintf(
            base_name,
            sizeof(base_name),
            "%s",
            argv[argument + 2]);
    } else {
        derive_base_name(
            input_path,
            base_name,
            sizeof(base_name));
    }


    char selected_api_name[256] = {0};
    char selected_method_name[256] = {0};

    const char *qualified_method_selection =
        method_selection != NULL
            ? method_selection
            : response_method_selection;

    if (qualified_method_selection != NULL) {
        const char *separator =
            strchr(
                qualified_method_selection,
                '.');


        if (separator == NULL ||
            separator == qualified_method_selection ||
            separator[1] == '\0' ||
            strchr(
                separator + 1,
                '.') != NULL) {

            fprintf(
                stderr,
                "method selection must be Api.method\n");

            return 2;
        }

        size_t api_length =
            (size_t)(
                separator -
                qualified_method_selection);

        size_t method_length =
            strlen(
                separator + 1);

        if (api_length >=
                sizeof(selected_api_name) ||
            method_length >=
                sizeof(selected_method_name)) {

            fprintf(
                stderr,
                "--method selection is too long\n");

            return 2;
        }

        memcpy(
            selected_api_name,
            qualified_method_selection,
            api_length);

        selected_api_name[api_length] =
            '\0';

        memcpy(
            selected_method_name,
            separator + 1,
            method_length + 1u);
    }

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    if (!adsl_load_file_with_include_paths(
            input_path,
            include_paths,
            include_path_count,
            &document,
            error,
            sizeof(error))) {

        fprintf(
            stderr,
            "ADSL load error: %s\n",
            error);

        adsl_document_free(&document);
        return 1;
    }

    if (!adsl_validate(
            &document,
            error,
            sizeof(error))) {

        fprintf(
            stderr,
            "ADSL validation error: %s\n",
            error);

        adsl_document_free(&document);
        return 1;
    }

    bool emitted;

    if (method_selection != NULL) {
        emitted =
            adsl_emit_c_method(
                &document,
                output_directory,
                base_name,
                selected_api_name,
                selected_method_name,
                error,
                sizeof(error));
    } else if (api_selection != NULL) {
        emitted =
            adsl_emit_c_method(
                &document,
                output_directory,
                base_name,
                api_selection,
                NULL,
                error,
                sizeof(error));
    } else if (local_api_selection != NULL) {
        emitted =
            adsl_emit_c_local_api(
                &document,
                output_directory,
                base_name,
                local_api_selection,
                error,
                sizeof(error));

    } else if (response_method_selection != NULL) {
        emitted =
            adsl_emit_c_response_method(
                &document,
                output_directory,
                base_name,
                selected_api_name,
                selected_method_name,
                error,
                sizeof(error));
    } else {

        emitted =
            adsl_emit_c(
                &document,
                output_directory,
                base_name,
                error,
                sizeof(error));
    }

    if (!emitted) {
        fprintf(
            stderr,
            "ADSL generation error: %s\n",
            error);

        adsl_document_free(&document);
        return 1;
    }

    adsl_document_free(&document);

    return 0;
}