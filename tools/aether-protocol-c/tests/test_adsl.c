
#include "adslc.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    /*
     * Effective-method collector: LocalApi inherits enter/enterGlobal from
     * BaseLocalApi and declares its own methods. Verify parent-first order
     * and inherited presence.
     */
    (void)snprintf(
        path,
        sizeof(path),
        "%s/LocalStreamDispatch.adsl.yaml",
        ADSL_FIXTURE_DIR);

    adsl_document_init(&document);

    if (!adsl_parse_file(
            path,
            &document,
            error,
            sizeof(error)) ||
        !adsl_validate(
            &document,
            error,
            sizeof(error))) {

        fprintf(stderr, "fixture load failed: %s\n", error);
        adsl_document_free(&document);
        return 1;
    }

    const adsl_api_t *local_api = NULL;
    for (size_t i = 0u; i < document.api_count; ++i) {
        if (strcmp(document.apis[i].name, "LocalApi") == 0) {
            local_api = &document.apis[i];
            break;
        }
    }

    if (local_api == NULL) {
        fprintf(stderr, "LocalApi not found in fixture\n");
        adsl_document_free(&document);
        return 1;
    }

    adsl_effective_method_t *effective = NULL;
    size_t effective_count = 0u;

    if (!adsl_collect_effective_methods(
            &document,
            local_api,
            &effective,
            &effective_count,
            error,
            sizeof(error))) {

        fprintf(stderr, "effective collect failed: %s\n", error);
        adsl_document_free(&document);
        return 1;
    }

    /*
     * Parent-first ordering: enter (3), enterGlobal (4) from BaseLocalApi,
     * then local sendMessage (5) onward.
     */

    if (effective_count != 11u ||

        strcmp(effective[0].method->name, "enter") != 0 ||
        strcmp(effective[1].method->name, "enterGlobal") != 0 ||
        strcmp(effective[0].owner->name, "BaseLocalApi") != 0) {

        fprintf(stderr, "unexpected effective ordering\n");
        free(effective);
        adsl_document_free(&document);
        return 1;
    }

    free(effective);
    adsl_document_free(&document);

    /*
     * Effective collision: construct a document where a child inherits a
     * method whose command id clashes with a locally declared method.
     * adsl_collect_effective_methods must reject this.
     */
    adsl_document_init(&document);

    document.name = strdup("Collision");

    document.apis = (adsl_api_t *)calloc(2u, sizeof(adsl_api_t));
    document.api_count = 2u;

    document.apis[0].name = strdup("BaseCollisionApi");
    document.apis[0].methods =
        (adsl_method_t *)calloc(1u, sizeof(adsl_method_t));
    document.apis[0].method_count = 1u;
    document.apis[0].methods[0].name = strdup("baseOp");
    document.apis[0].methods[0].has_id = true;
    document.apis[0].methods[0].id = 3u;

    document.apis[1].name = strdup("ChildCollisionApi");
    document.apis[1].parents =
        (char **)calloc(1u, sizeof(char *));
    document.apis[1].parent_count = 1u;
    document.apis[1].parents[0] = strdup("BaseCollisionApi");
    document.apis[1].methods =
        (adsl_method_t *)calloc(1u, sizeof(adsl_method_t));
    document.apis[1].method_count = 1u;
    document.apis[1].methods[0].name = strdup("childOp");
    document.apis[1].methods[0].has_id = true;
    document.apis[1].methods[0].id = 3u;

    adsl_effective_method_t *collision = NULL;
    size_t collision_count = 0u;

    if (adsl_collect_effective_methods(
            &document,
            &document.apis[1],
            &collision,
            &collision_count,
            error,
            sizeof(error))) {

        fprintf(stderr, "expected collision error but collector succeeded\n");
        free(collision);
        adsl_document_free(&document);
        return 1;
    }

    adsl_document_free(&document);

    return 0;
}