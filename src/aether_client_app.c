

/*
 * Advanced application facade.
 *
 * This layer owns URI/DNS/platform scheduling glue and converts low-level
 * protocol state into the pull-ingress application model. It copies the supplied
 * config/platform mechanism tables into caller-owned aether_client_app_t and
 * performs no heap allocation.
 */
#include "aether_client_app.h"

#include <string.h>



const aether_uuid_t AETHER_ANONYMOUS_UID = {
    UINT64_C(0x237e2dc021a44e83),
    UINT64_C(0x8184c43052f93b79)
};


static bool uuid_is_zero(
    aether_uuid_t uuid) {

    return
        uuid.msb == UINT64_C(0) &&
        uuid.lsb == UINT64_C(0);
}


bool aether_uuid_equal(
    aether_uuid_t left,
    aether_uuid_t right) {

    return
        left.msb == right.msb &&
        left.lsb == right.lsb;
}


static int hex_value(
    char value) {

    if (value >= '0' &&
        value <= '9') {

        return value - '0';
    }

    if (value >= 'a' &&
        value <= 'f') {

        return value - 'a' + 10;
    }

    if (value >= 'A' &&
        value <= 'F') {

        return value - 'A' + 10;
    }

    return -1;
}


aether_status_t aether_uuid_parse(
    const char *text,
    aether_uuid_t *uuid) {

    if (text == NULL ||
        uuid == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    uint8_t bytes[AETHER_UUID_BYTES];
    size_t nibble_count = 0u;

    memset(
        bytes,
        0,
        sizeof(bytes));

    for (size_t i = 0u;
         text[i] != '\0';
         ++i) {

        if (text[i] == '-') {
            continue;
        }

        int nibble =
            hex_value(
                text[i]);

        if (nibble < 0 ||
            nibble_count >=
                AETHER_UUID_BYTES * 2u) {

            return AETHER_ERR_ARGUMENT;
        }

        size_t byte_index =
            nibble_count / 2u;

        if ((nibble_count & 1u) == 0u) {
            bytes[byte_index] =
                (uint8_t)(
                    (unsigned)nibble << 4u);

        } else {
            bytes[byte_index] |=
                (uint8_t)(unsigned)nibble;
        }

        ++nibble_count;
    }

    if (nibble_count !=
        AETHER_UUID_BYTES * 2u) {

        return AETHER_ERR_ARGUMENT;
    }

    uint64_t msb = UINT64_C(0);
    uint64_t lsb = UINT64_C(0);

    for (size_t i = 0u;
         i < 8u;
         ++i) {

        msb =
            (msb << 8u) |
            (uint64_t)bytes[i];

        lsb =
            (lsb << 8u) |
            (uint64_t)bytes[i + 8u];
    }

    uuid->msb = msb;
    uuid->lsb = lsb;

    return AETHER_OK;
}


void aether_client_app_config_default(
    aether_client_app_config_t *config) {

    if (config == NULL) {
        return;
    }

    memset(
        config,
        0,
        sizeof(*config));

    config->registration_uri =
        AETHER_DEFAULT_REGISTRATION_URI;

    config->parent_uid =
        AETHER_ANONYMOUS_UID;

    config->ping_interval_ms =
        6000u;

    config->rx_window_ms =
        5000u;
}


void aether_client_app_init(
    aether_client_app_t *client,
    const aether_client_app_config_t *config,
    const aether_client_platform_t *platform) {

    if (client == NULL) {
        return;
    }

    memset(
        client,
        0,
        sizeof(*client));

    aether_client_app_config_default(
        &client->config);

    if (config != NULL) {
        client->config =
            *config;
    }

    if (platform != NULL) {
        client->platform =
            *platform;
    }
}


static bool parse_port(
    const char *text,
    uint16_t *port) {

    if (text == NULL ||
        port == NULL ||
        *text == '\0') {

        return false;
    }

    uint32_t value = 0u;

    for (size_t i = 0u;
         text[i] != '\0';
         ++i) {

        if (text[i] < '0' ||
            text[i] > '9') {

            return false;
        }

        value =
            value * 10u +
            (uint32_t)(
                text[i] - '0');

        if (value > UINT16_MAX) {
            return false;
        }
    }

    if (value == 0u) {
        return false;
    }

    *port =
        (uint16_t)value;

    return true;
}


static bool parse_ipv4(
    const char *text,
    aether_address_t *address) {

    if (text == NULL ||
        address == NULL) {

        return false;
    }

    uint8_t bytes[4];
    size_t part = 0u;
    uint32_t value = 0u;
    bool have_digit = false;

    for (size_t i = 0u;; ++i) {
        char ch =
            text[i];

        if (ch >= '0' &&
            ch <= '9') {

            have_digit =
                true;

            value =
                value * 10u +
                (uint32_t)(
                    ch - '0');

            if (value > 255u) {
                return false;
            }

            continue;
        }

        if (ch != '.' &&
            ch != '\0') {

            return false;
        }

        if (!have_digit ||
            part >= 4u) {

            return false;
        }

        bytes[part++] =
            (uint8_t)value;

        value = 0u;
        have_digit = false;

        if (ch == '\0') {
            break;
        }
    }

    if (part != 4u) {
        return false;
    }

    memset(
        address,
        0,
        sizeof(*address));

    address->kind =
        AETHER_ADDR_IPV4;

    address->length =
        4u;

    memcpy(
        address->bytes,
        bytes,
        sizeof(bytes));

    return true;
}


static aether_status_t parse_registration_uri(
    const char *uri,
    aether_client_platform_t *platform,
    aether_endpoint_t *endpoint) {

    if (uri == NULL ||
        platform == NULL ||
        endpoint == NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    const char *host_start = NULL;
    aether_codec_t codec;

    if (strncmp(
            uri,
            "tcp://",
            6u) == 0) {

        codec =
            AETHER_CODEC_TCP;

        host_start =
            uri + 6u;

    } else if (
        strncmp(
            uri,
            "udp://",
            6u) == 0) {

        codec =
            AETHER_CODEC_UDP;

        host_start =
            uri + 6u;

    } else {
        return AETHER_ERR_ARGUMENT;
    }

    const char *host_end = NULL;
    const char *port_text = NULL;

    if (*host_start == '[') {
        ++host_start;

        host_end =
            strchr(
                host_start,
                ']');

        if (host_end == NULL ||
            host_end[1] != ':') {

            return AETHER_ERR_ARGUMENT;
        }

        port_text =
            host_end + 2;

    } else {
        const char *colon =
            strrchr(
                host_start,
                ':');

        if (colon == NULL) {
            return AETHER_ERR_ARGUMENT;
        }

        host_end =
            colon;

        port_text =
            colon + 1;
    }

    size_t host_length =
        (size_t)(
            host_end -
            host_start);

    if (host_length == 0u ||
        host_length > AETHER_MAX_HOSTNAME) {

        return AETHER_ERR_ARGUMENT;
    }

    char hostname[
        AETHER_MAX_HOSTNAME + 1u
    ];

    memcpy(
        hostname,
        host_start,
        host_length);

    hostname[host_length] =
        '\0';

    uint16_t port = 0u;

    if (!parse_port(
            port_text,
            &port)) {

        return AETHER_ERR_ARGUMENT;
    }

    aether_address_t address;

    if (!parse_ipv4(
            hostname,
            &address)) {

        if (platform->resolve == NULL) {
            return AETHER_ERR_UNSUPPORTED;
        }

        memset(
            &address,
            0,
            sizeof(address));

        aether_status_t status =
            platform->resolve(
                platform->ctx,
                hostname,
                &address);

        if (status != AETHER_OK) {
            return status;
        }
    }

    bool valid_address =
        (address.kind ==
             AETHER_ADDR_IPV4 &&
         address.length == 4u) ||
        (address.kind ==
             AETHER_ADDR_IPV6 &&
         address.length == 16u);

    if (!valid_address) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(
        endpoint,
        0,
        sizeof(*endpoint));

    endpoint->codec =
        codec;

    endpoint->address =
        address;

    endpoint->port =
        port;

    return AETHER_OK;
}


/*
 * Legacy core-event bridge for mandatory lifecycle only.
 *
 * Application MESSAGE ingress never arrives here.
 */
static void app_on_event(
    void *ctx,
    const aether_event_t *event) {

    aether_client_app_t *client =
        ctx;

    if (client == NULL ||
        event == NULL) {

        return;
    }

    switch (event->type) {
    case AETHER_EVENT_STATE:
        if (event->as.state.state ==
                AETHER_STATE_READY &&
            client->config.on_ready != NULL) {

            client->config.on_ready(
                client->config.callback_ctx,
                client->core.uid);
        }
        break;

    case AETHER_EVENT_ERROR:
        if (client->config.on_error != NULL) {
            client->config.on_error(
                client->config.callback_ctx,
                event->as.error.code,
                event->as.error.origin);
        }
        break;

    default:
        break;
    }
}


static void fill_core_config(
    aether_client_app_t *client,
    const aether_endpoint_t *registration_endpoint,
    aether_client_config_t *core_config) {

    memset(
        core_config,
        0,
        sizeof(*core_config));

    core_config->parent_uid =
        uuid_is_zero(
            client->config.parent_uid)
            ? AETHER_ANONYMOUS_UID
            : client->config.parent_uid;

    core_config->registration_endpoint =
        *registration_endpoint;

    core_config->ping_interval_ms =
        client->config.ping_interval_ms;

    core_config->rx_window_ms =
        client->config.rx_window_ms;

    core_config->request_timeout_ms =
        client->config.request_timeout_ms;

    core_config->transport =
        client->platform.transport;

    core_config->flash =
        client->platform.flash;

    core_config->crypto =
        client->platform.crypto;

    core_config->crypto_ctx =
        client->platform.crypto_ctx;

    core_config->pow =
        client->platform.pow;

    core_config->trusted_sign_keys =
        client->platform.trusted_sign_keys;

    core_config->trusted_sign_key_count =
        client->platform.trusted_sign_key_count;

    core_config->on_event =
        app_on_event;

    core_config->event_ctx =
        client;
}


static bool pull_rx_pair_is_valid(
    const aether_client_app_t *client) {

    bool have_receive =
        client->platform.receive != NULL;

    bool have_consume =
        client->platform.consume != NULL;

    return
        have_receive ==
        have_consume;
}


aether_status_t aether_client_app_start(
    aether_client_app_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (client->started) {
        return AETHER_ERR_STATE;
    }

    if (client->platform.now_ms == NULL ||
        !pull_rx_pair_is_valid(client)) {

        return AETHER_ERR_ARGUMENT;
    }

    const char *registration_uri =
        client->config.registration_uri != NULL
            ? client->config.registration_uri
            : AETHER_DEFAULT_REGISTRATION_URI;

    aether_endpoint_t registration_endpoint;

    aether_status_t status =
        parse_registration_uri(
            registration_uri,
            &client->platform,
            &registration_endpoint);

    if (status != AETHER_OK) {
        return status;
    }

    aether_client_config_t core_config;

    fill_core_config(
        client,
        &registration_endpoint,
        &core_config);

    aether_client_init(
        &client->core,
        &core_config);

    if (client->platform.bind != NULL) {
        status =
            client->platform.bind(
                client->platform.ctx,
                &client->core);

        if (status != AETHER_OK) {
            return status;
        }
    }

    status =
        aether_client_start(
            &client->core);

    if (status == AETHER_OK) {
        client->started =
            true;
    }

    return status;
}


void aether_client_app_stop(
    aether_client_app_t *client) {

    if (client == NULL) {
        return;
    }

    if (client->started) {
        aether_client_stop(
            &client->core);
    }

    client->started =
        false;
}


/*
 * Process one bounded RX parser step.
 *
 * A yielded ingress remains pending. This layer does not choose receive policy
 * and does not invoke an application message callback.
 */
static aether_status_t receive_one(
    aether_client_app_t *client) {

    if (client->platform.receive == NULL) {
        return AETHER_OK;
    }

    aether_channel_t channels[2];
    size_t channel_count;

    if (aether_client_rx_active(
            &client->core)) {

        channels[0] =
            aether_client_rx_channel(
                &client->core);

        channel_count =
            1u;

    } else {
        channels[0] =
            AETHER_CHANNEL_REGISTRATION;

        channels[1] =
            AETHER_CHANNEL_WORK;

        channel_count =
            2u;
    }

    for (size_t i = 0u;
         i < channel_count;
         ++i) {

        aether_bytes_view_t frame = {
            NULL,
            0u
        };

        aether_status_t status =
            client->platform.receive(
                client->platform.ctx,
                channels[i],
                &frame);

        if (status != AETHER_OK) {
            return status;
        }

        if (frame.data == NULL) {
            continue;
        }

        status =
            aether_client_on_rx(
                &client->core,
                channels[i],
                frame.data,
                frame.length);

        if (status != AETHER_OK) {
            /*
             * Parser failure abandons its internal RX state.
             * Release the corresponding borrowed transport frame as well.
             */
            if (!aether_client_rx_active(
                    &client->core)) {

                client->platform.consume(
                    client->platform.ctx,
                    channels[i],
                    frame.data,
                    frame.length);
            }

            return status;
        }

        /*
         * A yielded ingress pins the frame until explicit caller consume.
         */
        if (aether_client_ingress(
                &client->core) != NULL) {

            return AETHER_OK;
        }

        if (!aether_client_rx_active(
                &client->core)) {

            client->platform.consume(
                client->platform.ctx,
                channels[i],
                frame.data,
                frame.length);
        }

        return AETHER_OK;
    }

    return AETHER_OK;
}


aether_status_t aether_client_app_tick(
    aether_client_app_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (!client->started) {
        return AETHER_ERR_STATE;
    }

    /*
     * Policy belongs to the caller. Do not advance or overwrite borrowed
     * ingress memory until the caller explicitly consumes it.
     */
    if (aether_client_ingress(
            &client->core) != NULL) {

        return AETHER_OK;
    }

    uint64_t now_ms =
        client->platform.now_ms(
            client->platform.ctx);

    bool frame_pinned =
        aether_client_rx_active(
            &client->core);

    /*
     * A pinned frame must stay stable. Therefore do not call platform.poll()
     * until parser progress releases that frame.
     */
    if (!frame_pinned &&
        client->platform.poll != NULL) {

        aether_status_t status =
            client->platform.poll(
                client->platform.ctx);

        if (status != AETHER_OK) {
            return status;
        }
    }

    aether_status_t status =
        receive_one(
            client);

    if (status != AETHER_OK) {
        return status;
    }

    /*
     * Timers/reconnect/ping may reuse crypto buffers or transport state.
     * Run them only after no borrowed RX frame remains pinned.
     */
    if (!aether_client_rx_active(
            &client->core)) {

        aether_client_poll(
            &client->core,
            now_ms);
    }

    return AETHER_OK;
}


aether_status_t aether_client_app_poll(
    aether_client_app_t *client) {

    return
        aether_client_app_tick(
            client);
}


const aether_ingress_t *aether_client_app_ingress(
    const aether_client_app_t *client) {

    if (client == NULL) {
        return NULL;
    }

    return
        aether_client_ingress(
            &client->core);
}


aether_status_t aether_client_app_consume_ingress(
    aether_client_app_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    const aether_ingress_t *ingress =
        aether_client_ingress(
            &client->core);

    if (ingress == NULL) {
        return AETHER_OK;
    }

    if (!aether_client_rx_active(
            &client->core)) {

        aether_client_consume_ingress(
            &client->core);

        return AETHER_OK;
    }

    if (client->platform.receive == NULL ||
        client->platform.consume == NULL) {

        return AETHER_ERR_STATE;
    }

    aether_channel_t channel =
        aether_client_rx_channel(
            &client->core);

    aether_bytes_view_t frame = {
        NULL,
        0u
    };

    aether_status_t status =
        client->platform.receive(
            client->platform.ctx,
            channel,
            &frame);

    if (status != AETHER_OK) {
        return status;
    }

    if (frame.data == NULL) {
        return AETHER_ERR_STATE;
    }

    aether_client_consume_ingress(
        &client->core);

    if (!aether_client_rx_active(
            &client->core)) {

        client->platform.consume(
            client->platform.ctx,
            channel,
            frame.data,
            frame.length);
    }

    return AETHER_OK;
}


aether_status_t aether_client_app_send(
    aether_client_app_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t length) {

    return
        aether_client_app_send_with_id(
            client,
            destination,
            data,
            length,
            NULL);
}


aether_status_t aether_client_app_send_with_id(
    aether_client_app_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t length,
    uint32_t *request_id_out) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    return
        aether_client_send_message(
            &client->core,
            destination,
            data,
            length,
            request_id_out);
}


bool aether_client_app_is_ready(
    const aether_client_app_t *client) {

    return
        client != NULL &&
        client->started &&
        aether_client_state(
            &client->core) ==
            AETHER_STATE_READY;
}


bool aether_client_app_is_registered(
    const aether_client_app_t *client) {

    return
        client != NULL &&
        aether_client_is_registered(
            &client->core);
}


aether_state_t aether_client_app_state(
    const aether_client_app_t *client) {

    if (client == NULL) {
        return AETHER_STATE_STOPPED;
    }

    return
        aether_client_state(
            &client->core);
}


aether_uuid_t aether_client_app_uid(
    const aether_client_app_t *client) {

    if (client == NULL) {
        return
            (aether_uuid_t){
                UINT64_C(0),
                UINT64_C(0)
            };
    }

    return
        client->core.uid;
}