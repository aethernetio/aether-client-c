
#include "aether_client_app.h"

#include <assert.h>
#include <string.h>


int main(void) {
    aether_client_app_config_t config;

    aether_client_app_config_default(
        &config);

    assert(
        strcmp(
            config.registration_uri,
            "tcp://registration.aethernet.io:9010") == 0);

    assert(
        config.parent_uid.msb ==
            UINT64_C(0x237e2dc021a44e83));

    assert(
        config.parent_uid.lsb ==
            UINT64_C(0x8184c43052f93b79));

    assert(
        config.ping_interval_ms == 6000u);

    assert(
        config.rx_window_ms == 5000u);

    aether_uuid_t parsed;

    assert(
        aether_uuid_parse(
            "B1AC52C8-8D94-BD39-4C01-A631AC594165",
            &parsed) == AETHER_OK);

    assert(
        parsed.msb ==
            UINT64_C(0xB1AC52C88D94BD39));

    assert(
        parsed.lsb ==
            UINT64_C(0x4C01A631AC594165));

    assert(
        aether_uuid_equal(
            parsed,
            (aether_uuid_t){
                UINT64_C(0xB1AC52C88D94BD39),
                UINT64_C(0x4C01A631AC594165)
            }));

    assert(
        aether_uuid_parse(
            "not-a-uuid",
            &parsed) ==
            AETHER_ERR_ARGUMENT);

    return 0;
}
