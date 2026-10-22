
#include "aether_reg_root_key.h"

#include "aether_api.h"


static aether_status_t aether_reg_root_key_commit(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    (void)data;

    size_t *length_out =
        ctx;

    if (length_out == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    *length_out =
        length;

    return AETHER_OK;
}


aether_status_t aether_generated_reg_root_build_get_key(
    uint8_t *data,
    size_t capacity,
    size_t *length,
    uint8_t crypto_lib) {

    if (length == NULL ||
        (capacity > 0u &&
         data == NULL)) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_data_out_t out;
    aether_data_out_init(&out, data, capacity);

    size_t produced =
        0u;

    csa_registration_root_api_remote_t remote = {
        .send_ctx = &produced,
        .send = aether_reg_root_key_commit,
        .out = &out
    };

    aether_status_t status =
        csa_registration_root_api_get_asymmetric_public_key(
            &remote,
            (common_crypto_lib_t)crypto_lib);

    if (status != AETHER_OK) {
        return status;
    }

    *length =
        produced;

    return AETHER_OK;
}
