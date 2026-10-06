
#include "adslc.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static bool validate_tiered_config(
    uint8_t min_tier_bytes,
    const uint32_t *limits,
    size_t limit_count) {

    assert(limit_count <= 3u);

    adsl_type_t type = {
        .name = "Tiered",
        .has_tiered_int = true,
        .tiered_int_min_bytes = min_tier_bytes,
        .tiered_int_limit_count = limit_count
    };

    for (size_t i = 0u;
         i < limit_count;
         ++i) {

        type.tiered_int_limits[i] =
            limits[i];
    }

    adsl_document_t document = {0};
    document.types = &type;
    document.type_count = 1u;

    char error[512] = {0};

    return adsl_validate(
        &document,
        error,
        sizeof(error));
}


static void test_tiered_int_validation(void) {
    static const uint32_t canonical[] = {
        UINT32_C(250),
        UINT32_C(1514),
        UINT32_C(1049834)
    };

    static const uint32_t wide[] = {
        UINT32_C(4000),
        UINT32_C(100000000)
    };

    static const uint32_t repeated[] = {
        UINT32_C(10),
        UINT32_C(10)
    };

    static const uint32_t first_header_full[] = {
        UINT32_C(255)
    };

    static const uint32_t unreachable_second[] = {
        UINT32_C(250),
        UINT32_C(1531)
    };

    static const uint32_t too_many_for_base_two[] = {
        UINT32_C(4000),
        UINT32_C(100000000),
        UINT32_C(100000001)
    };

    assert(
        validate_tiered_config(
            1u,
            canonical,
            3u));

    /*
     * This is the larger configuration used by the Java TieredInt generator
     * regression. It proves that validation is not accidentally specialized
     * to the canonical 1-byte Aether thresholds.
     */
    assert(
        validate_tiered_config(
            2u,
            wide,
            2u));

    assert(
        !validate_tiered_config(
            3u,
            canonical,
            3u));

    assert(
        !validate_tiered_config(
            1u,
            canonical,
            0u));

    assert(
        !validate_tiered_config(
            1u,
            repeated,
            2u));

    assert(
        !validate_tiered_config(
            1u,
            first_header_full,
            1u));

    /*
     * With base=1 and first limit 250 the previous tier can represent at most
     * 1530. A next threshold of 1531 therefore cannot be reached canonically.
     */
    assert(
        !validate_tiered_config(
            1u,
            unreachable_second,
            2u));

    assert(
        !validate_tiered_config(
            2u,
            too_many_for_base_two,
            3u));

    adsl_type_t types[2] = {0};

    types[0].name = "ScalarBase";
    types[0].has_tiered_int = true;
    types[0].tiered_int_min_bytes = 1u;
    types[0].tiered_int_limits[0] = UINT32_C(250);
    types[0].tiered_int_limit_count = 1u;

    types[1].name = "IllegalChild";
    types[1].parent = "ScalarBase";

    adsl_document_t inheritance = {0};
    inheritance.types = types;
    inheritance.type_count = 2u;

    char error[512] = {0};

    assert(
        !adsl_validate(
            &inheritance,
            error,
            sizeof(error)));

    assert(
        strstr(
            error,
            "cannot inherit from scalar tieredInt") != NULL);
}




static void assert_emit_rejects_by_value_cycle(
    const char *fixture_name,
    const char *base_name) {

    char path[1024];
    (void)snprintf(
        path,
        sizeof(path),
        "%s/%s",
        ADSL_FIXTURE_DIR,
        fixture_name);

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_parse_file(
            path,
            &document,
            error,
            sizeof(error)));

    /*
     * These schemas are legal ADSL graphs but cannot be represented as finite
     * inline C layouts. Nullable references and dynamic arrays are pointer/view
     * shaped and remain valid recursive termination edges.
     */
    assert(
        adsl_validate(
            &document,
            error,
            sizeof(error)));

    error[0] = '\0';

    assert(
        !adsl_emit_c(
            &document,
            ADSL_TEST_OUTPUT_DIR,
            base_name,
            error,
            sizeof(error)));

    assert(
        strstr(
            error,
            "non-nullable by-value type cycle cannot be represented in generated C") != NULL);

    adsl_document_free(&document);
}


static void test_by_value_cycle_rejection(void) {
    assert_emit_rejects_by_value_cycle(
        "InvalidDirectByValueCycle.adsl.yaml",
        "invalid_direct_by_value_cycle");

    assert_emit_rejects_by_value_cycle(
        "InvalidMutualByValueCycle.adsl.yaml",
        "invalid_mutual_by_value_cycle");

    assert_emit_rejects_by_value_cycle(
        "InvalidFixedArrayByValueCycle.adsl.yaml",
        "invalid_fixed_array_by_value_cycle");
}




static void assert_validation_rejects_fixture(
    const char *fixture_name,
    const char *expected_error) {

    char path[1024];

    (void)snprintf(
        path,
        sizeof(path),
        "%s/%s",
        ADSL_FIXTURE_DIR,
        fixture_name);

    adsl_document_t document;
    adsl_document_init(&document);

    char error[512] = {0};

    assert(
        adsl_parse_file(
            path,
            &document,
            error,
            sizeof(error)));

    assert(
        !adsl_validate(
            &document,
            error,
            sizeof(error)));

    assert(
        strstr(
            error,
            expected_error) != NULL);

    adsl_document_free(&document);
}


static void test_collapsible_method_validation(void) {
    assert_validation_rejects_fixture(
        "InvalidCollapsibleResponse.adsl.yaml",
        "cannot declare returns or throws");

    assert_validation_rejects_fixture(
        "InvalidAllCollapsibleMethod.adsl.yaml",
        "requires at least one non-collapsible key parameter");
    assert_validation_rejects_fixture(
        "InvalidCollapsibleUriKey.adsl.yaml",
        "URI key cannot reproduce java.net.URI.equals semantics");

    assert_validation_rejects_fixture(
        "InvalidCollapsibleArrayKey.adsl.yaml",
        "array key cannot reproduce Java array identity semantics");

    assert_validation_rejects_fixture(
        "InvalidCollapsibleStructKey.adsl.yaml",
        "complex key type 'Key' cannot reproduce Java Objects.equals semantics");

}



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


    adsl_effective_method_t *temperature_effective = NULL;
    size_t temperature_effective_count = 0u;

    if (!adsl_collect_effective_methods(
            &document,
            api,
            &temperature_effective,
            &temperature_effective_count,
            error,
            sizeof(error))) {

        fprintf(stderr, "effective collect failed: %s\n", error);
        adsl_document_free(&document);
        return 1;
    }

    if (temperature_effective_count != 1u ||
        temperature_effective[0].id != 3u) {

        fprintf(
            stderr,
            "unexpected method id: count=%zu id=%u\n",
            temperature_effective_count,
            temperature_effective_count > 0u
                ? (unsigned int)temperature_effective[0].id
                : 0u);

        free(temperature_effective);
        adsl_document_free(&document);
        return 1;
    }

    free(temperature_effective);


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


    test_tiered_int_validation();

    /*
     * Parse the real TieredInt fixture as well as validating hand-built
     * semantic models. This protects the indentation/YAML-list parser path and
     * confirms the Java regression configuration survives parsing unchanged.
     */
    (void)snprintf(
        path,
        sizeof(path),
        "%s/TieredInt.adsl.yaml",
        ADSL_FIXTURE_DIR);

    adsl_document_init(&document);

    assert(
        adsl_parse_file(
            path,
            &document,
            error,
            sizeof(error)));

    assert(
        adsl_validate(
            &document,
            error,
            sizeof(error)));

    const adsl_type_t *wide_counter = NULL;

    for (size_t i = 0u;
         i < document.type_count;
         ++i) {

        if (strcmp(
                document.types[i].name,
                "WideCounter") == 0) {

            wide_counter =
                &document.types[i];

            break;
        }
    }

    assert(wide_counter != NULL);
    assert(wide_counter->has_tiered_int);
    assert(wide_counter->tiered_int_min_bytes == 2u);
    assert(wide_counter->tiered_int_limit_count == 2u);
    assert(
        wide_counter->tiered_int_limits[0] ==
        UINT32_C(4000));
    assert(
        wide_counter->tiered_int_limits[1] ==
        UINT32_C(100000000));

    adsl_document_free(&document);




    test_collapsible_method_validation();


    test_by_value_cycle_rejection();


    return 0;
}