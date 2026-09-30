
#include "adslc.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char path[1024];

    (void)snprintf(
        path,
        sizeof(path),
        "%s/TemperatureSensor.adsl.yaml",
        ADSL_FIXTURE_DIR);

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    if (!adsl_parse_file(
            path,
            &document,
            error,
            sizeof(error))) {

        fprintf(
            stderr,
            "parse failed: %s\n",
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
            "validation failed: %s\n",
            error);

        adsl_document_free(&document);
        return 1;
    }

    if (document.api_count != 1u) {
        fprintf(
            stderr,
            "unexpected API count: %zu\n",
            document.api_count);

        adsl_document_free(&document);
        return 1;
    }

    const adsl_api_t *api =
        &document.apis[0];

    if (strcmp(
            api->name,
            "TemperatureSensorApi") != 0) {

        fprintf(
            stderr,
            "unexpected API name: %s\n",
            api->name);

        adsl_document_free(&document);
        return 1;
    }

    if (api->method_count != 1u) {
        fprintf(
            stderr,
            "unexpected method count: %zu\n",
            api->method_count);

        adsl_document_free(&document);
        return 1;
    }

    const adsl_method_t *method =
        &api->methods[0];

    if (strcmp(
            method->name,
            "reportTemperature") != 0) {

        fprintf(
            stderr,
            "unexpected method name: %s\n",
            method->name);

        adsl_document_free(&document);
        return 1;
    }

    if (!method->has_id ||
        method->id != 3u) {

        fprintf(
            stderr,
            "unexpected method id: has_id=%d id=%u\n",
            method->has_id ? 1 : 0,
            (unsigned int)method->id);

        adsl_document_free(&document);
        return 1;
    }

    if (method->param_count != 1u) {
        fprintf(
            stderr,
            "unexpected parameter count: %zu\n",
            method->param_count);

        adsl_document_free(&document);
        return 1;
    }

    if (strcmp(
            method->params[0].name,
            "temperature") != 0) {

        fprintf(
            stderr,
            "unexpected parameter name: %s\n",
            method->params[0].name);

        adsl_document_free(&document);
        return 1;
    }

    if (strcmp(
            method->params[0].type,
            "float") != 0) {

        fprintf(
            stderr,
            "unexpected parameter type: %s\n",
            method->params[0].type);

        adsl_document_free(&document);
        return 1;
    }

    adsl_document_free(&document);

    return 0;
}