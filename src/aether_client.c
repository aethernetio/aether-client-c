





/*
 * Low-level Aether protocol state machine.
 *
 * This monolithic embedded core owns protocol sequencing, registration/work
 * state, persistent Aether identity serialization, request tracking, crypto
 * orchestration, bounded parsing and message transmission.
 *
 * It deliberately does not own physical sockets, platform persistence,
 * cryptographic primitive implementations, application scheduling or
 * heap-backed queues.
 *
 * Registration wire construction/decoding is delegated to protocol bindings and
 * maintained registration adapters under src/generated/.
 *
 * Work RX can pin a borrowed platform frame while one application ingress item
 * is exposed. The platform must preserve that frame until the ingress is
 * consumed and aether_client_rx_active() becomes false.
 */
#include "aether_client.h"
#include "generated/client_server_reg_direct_response_response.h"
#include "generated/aether_reg_global.h"
#include "generated/aether_reg_root_enter.h"
#include "generated/aether_reg_root_key.h"
#include "generated/aether_reg_safe.h"
#include "generated/aether_reg_unsafe_local.h"
#include "generated/client_server_api_api.h"

#include <string.h>




#define AETHER_STATE_MAGIC0 ((uint8_t)'A')
#define AETHER_STATE_MAGIC1 ((uint8_t)'E')
#define AETHER_STATE_MAGIC2 ((uint8_t)'C')
#define AETHER_STATE_MAGIC3 ((uint8_t)'1')
#define AETHER_STATE_VERSION 1u


#define CMD_RESULT 0u
#define CMD_ERROR 1u







#define LOGIN_BY_ALIAS 5u
#define CLIENT_UNSAFE_SAFE_DATA_MULTI 3u
#define CLIENT_UNSAFE_SAFE_DATA 4u


#define AUTH_PING 4u
#define AUTH_SET_RECEIVE_WINDOW 45u


#define CLIENT_SAFE_SEND_MESSAGES 6u
#define CLIENT_SAFE_SEND_MESSAGE 20u




/*
 * Wire values understood only so unsupported server endpoints can be skipped.
 */
#define AETHER_CODEC_WS_WIRE 2u
#define AETHER_CODEC_WSS_WIRE 3u
#define AETHER_ADDR_WEB_WIRE 3u


typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t pos;
    bool failed;
} writer_t;


typedef struct {
    const uint8_t *data;
    size_t length;
    size_t pos;
    bool failed;
} reader_t;


static void emit_event(
    aether_client_t *client,
    const aether_event_t *event) {

    if (client->config.on_event == NULL) {
        return;
    }

    client->callback_active =
        true;

    client->config.on_event(
        client->config.event_ctx,
        event);

    client->callback_active =
        false;
}


static void set_state(
    aether_client_t *client,
    aether_state_t state) {

    if (client->state == state) {
        return;
    }

    client->state =
        state;

    aether_event_t event;

    memset(
        &event,
        0,
        sizeof(event));

    event.type =
        AETHER_EVENT_STATE;

    event.as.state.state =
        state;

    emit_event(
        client,
        &event);
}


static aether_error_origin_t error_origin_for_status(
    aether_status_t code) {

    switch (code) {
    case AETHER_ERR_PROTOCOL:
    case AETHER_ERR_UNSUPPORTED:
        return AETHER_ERROR_ORIGIN_PROTOCOL;

    case AETHER_ERR_CRYPTO:
        return AETHER_ERROR_ORIGIN_CRYPTO;

    case AETHER_ERR_TRANSPORT:
        return AETHER_ERROR_ORIGIN_TRANSPORT;

    case AETHER_ERR_STORAGE:
        return AETHER_ERROR_ORIGIN_STORAGE;

    case AETHER_ERR_REMOTE:
        return AETHER_ERROR_ORIGIN_REMOTE;

    default:
        return AETHER_ERROR_ORIGIN_CORE;
    }
}


static void emit_error(
    aether_client_t *client,
    aether_status_t code,
    aether_error_origin_t origin,
    aether_state_t state,
    uint32_t request_id) {

    aether_event_t event;

    memset(
        &event,
        0,
        sizeof(event));

    event.type =
        AETHER_EVENT_ERROR;

    event.as.error.code =
        code;

    event.as.error.origin =
        origin;

    event.as.error.state =
        state;

    event.as.error.request_id =
        request_id;

    emit_event(
        client,
        &event);
}


static void clear_ingress(
    aether_client_t *client) {

    memset(
        &client->ingress,
        0,
        sizeof(client->ingress));

    client->ingress.kind =
        AETHER_INGRESS_NONE;
}


static void clear_rx_state(
    aether_client_t *client) {

    client->rx_frame_data =
        NULL;

    client->rx_frame_length =
        0u;

    client->rx_outer_pos =
        0u;

    client->rx_plain_length =
        0u;

    client->rx_plain_pos =
        0u;

    client->rx_messages_remaining =
        0u;

    client->rx_channel =
        AETHER_CHANNEL_WORK;

    client->rx_active =
        false;

    client->rx_plain_active =
        false;

    clear_ingress(
        client);
}


static aether_status_t fail_with_origin(
    aether_client_t *client,
    aether_status_t code,
    aether_error_origin_t origin) {

    aether_state_t source_state =
        client->state;

    set_state(
        client,
        AETHER_STATE_ERROR);

    emit_error(
        client,
        code,
        origin,
        source_state,
        0u);

    return code;
}


static aether_status_t fail_remote(
    aether_client_t *client,
    uint32_t request_id) {

    aether_state_t source_state =
        client->state;

    set_state(
        client,
        AETHER_STATE_ERROR);

    emit_error(
        client,
        AETHER_ERR_REMOTE,
        AETHER_ERROR_ORIGIN_REMOTE,
        source_state,
        request_id);

    return AETHER_ERR_REMOTE;
}


static aether_status_t fail(
    aether_client_t *client,
    aether_status_t code) {

    return
        fail_with_origin(
            client,
            code,
            error_origin_for_status(code));
}


static void w_bytes(
    writer_t *writer,
    const uint8_t *data,
    size_t size) {

    if (writer->failed ||
        size >
            writer->capacity -
                writer->pos) {

        writer->failed =
            true;

        return;
    }

    if (size != 0u) {
        memcpy(
            writer->data +
                writer->pos,
            data,
            size);
    }

    writer->pos +=
        size;
}


static void w_u8(
    writer_t *writer,
    uint8_t value) {

    w_bytes(
        writer,
        &value,
        1u);
}


static void w_u16le(
    writer_t *writer,
    uint16_t value) {

    uint8_t bytes[2] = {
        (uint8_t)value,
        (uint8_t)(value >> 8u)
    };

    w_bytes(
        writer,
        bytes,
        sizeof(bytes));
}


static void w_u32le(
    writer_t *writer,
    uint32_t value) {

    uint8_t bytes[4] = {
        (uint8_t)value,
        (uint8_t)(value >> 8u),
        (uint8_t)(value >> 16u),
        (uint8_t)(value >> 24u)
    };

    w_bytes(
        writer,
        bytes,
        sizeof(bytes));
}


static void w_u64le(
    writer_t *writer,
    uint64_t value) {

    uint8_t bytes[8];

    for (unsigned i = 0u;
         i < 8u;
         ++i) {

        bytes[i] =
            (uint8_t)(
                value >>
                (i * 8u));
    }

    w_bytes(
        writer,
        bytes,
        sizeof(bytes));
}


static void w_i16le(
    writer_t *writer,
    int16_t value) {

    w_u16le(
        writer,
        (uint16_t)value);
}


static void w_i64le(
    writer_t *writer,
    int64_t value) {

    w_u64le(
        writer,
        (uint64_t)value);
}


/*
 * Exact SerializerPackNumber wire format from fast-meta.
 */
static void w_pack(
    writer_t *writer,
    uint64_t value) {

    const uint64_t u8 = 251u;
    const uint64_t u16 = 1515u;
    const uint64_t u32 = 1049835u;

    const uint64_t u64 =
        u32 +
        (UINT64_C(4294967296) *
         256u);

    if (value < u8) {
        w_u8(
            writer,
            (uint8_t)value);

        return;
    }

    if (value < u16) {
        uint64_t x =
            value - u8;

        w_u8(
            writer,
            (uint8_t)(
                ((x >> 8u) &
                 0xffu) +
                u8));

        w_u8(
            writer,
            (uint8_t)(
                x &
                0xffu));

        return;
    }

    if (value < u32) {
        uint64_t x =
            value - u16;

        w_u8(
            writer,
            255u);

        w_u8(
            writer,
            (uint8_t)(
                ((x >> 16u) -
                 u8) +
                u16));

        w_u16le(
            writer,
            (uint16_t)x);

        return;
    }

    if (value < u64) {
        uint64_t x =
            value - u32;

        w_u8(
            writer,
            255u);

        w_u8(
            writer,
            255u);

        w_u16le(
            writer,
            (uint16_t)(
                ((x >> 32u) -
                 u16) +
                u32));

        w_u32le(
            writer,
            (uint32_t)x);

        return;
    }

    writer->failed =
        true;
}


static void w_byte_array(
    writer_t *writer,
    const uint8_t *data,
    size_t length) {

    w_pack(
        writer,
        length);

    w_bytes(
        writer,
        data,
        length);
}


static void w_uuid(
    writer_t *writer,
    aether_uuid_t uuid) {

    /*
     * FastMeta UUID is intentionally big-endian.
     */
    w_u8(writer, (uint8_t)(uuid.msb >> 56u));
    w_u8(writer, (uint8_t)(uuid.msb >> 48u));
    w_u8(writer, (uint8_t)(uuid.msb >> 40u));
    w_u8(writer, (uint8_t)(uuid.msb >> 32u));
    w_u8(writer, (uint8_t)(uuid.msb >> 24u));
    w_u8(writer, (uint8_t)(uuid.msb >> 16u));
    w_u8(writer, (uint8_t)(uuid.msb >> 8u));
    w_u8(writer, (uint8_t)uuid.msb);

    w_u8(writer, (uint8_t)(uuid.lsb >> 56u));
    w_u8(writer, (uint8_t)(uuid.lsb >> 48u));
    w_u8(writer, (uint8_t)(uuid.lsb >> 40u));
    w_u8(writer, (uint8_t)(uuid.lsb >> 32u));
    w_u8(writer, (uint8_t)(uuid.lsb >> 24u));
    w_u8(writer, (uint8_t)(uuid.lsb >> 16u));
    w_u8(writer, (uint8_t)(uuid.lsb >> 8u));
    w_u8(writer, (uint8_t)uuid.lsb);
}


static uint8_t r_u8(
    reader_t *reader) {

    if (reader->failed ||
        reader->pos >=
            reader->length) {

        reader->failed =
            true;

        return 0u;
    }

    return
        reader->data[
            reader->pos++
        ];
}


static uint16_t r_u16le(
    reader_t *reader) {

    uint16_t value =
        r_u8(reader);

    value |=
        (uint16_t)(
            (uint16_t)r_u8(reader) <<
            8u);

    return value;
}


static uint32_t r_u32le(
    reader_t *reader) {

    uint32_t value =
        r_u8(reader);

    value |=
        (uint32_t)r_u8(reader) <<
        8u;

    value |=
        (uint32_t)r_u8(reader) <<
        16u;

    value |=
        (uint32_t)r_u8(reader) <<
        24u;

    return value;
}


static int16_t r_i16le(
    reader_t *reader) {

    return
        (int16_t)r_u16le(
            reader);
}


static int32_t r_i32le(
    reader_t *reader) {

    return
        (int32_t)r_u32le(
            reader);
}


static uint64_t r_pack(
    reader_t *reader) {

    const uint64_t u8 = 251u;
    const uint64_t u16 = 1515u;
    const uint64_t u32 = 1049835u;

    const uint64_t u64 =
        u32 +
        (UINT64_C(4294967296) *
         256u);

    uint64_t value =
        r_u8(reader);

    if (reader->failed ||
        value < u8) {

        return value;
    }

    uint64_t v =
        r_u8(reader);

    value =
        ((value - u8) <<
         8u) +
        u8 +
        v;

    if (reader->failed ||
        value < u16) {

        return value;
    }

    uint64_t f =
        r_u16le(reader);

    value =
        ((value - u16) <<
         16u) +
        u16 +
        f;

    if (reader->failed ||
        value < u32) {

        return value;
    }

    uint64_t f1 =
        r_u32le(reader);

    value =
        ((value - u32) <<
         32u) +
        u32 +
        f1;

    if (reader->failed ||
        value < u64) {

        return value;
    }

    reader->failed =
        true;

    return 0u;
}


static bool r_bytes(
    reader_t *reader,
    uint8_t *destination,
    size_t size) {

    if (reader->failed ||
        size >
            reader->length -
                reader->pos) {

        reader->failed =
            true;

        return false;
    }

    if (destination != NULL &&
        size != 0u) {

        memcpy(
            destination,
            reader->data +
                reader->pos,
            size);
    }

    reader->pos +=
        size;

    return true;
}


static bool r_view(
    reader_t *reader,
    const uint8_t **data,
    size_t *length) {

    uint64_t size =
        r_pack(reader);

    if (reader->failed ||
        size > SIZE_MAX ||
        (size_t)size >
            reader->length -
                reader->pos) {

        reader->failed =
            true;

        return false;
    }

    *data =
        reader->data +
        reader->pos;

    *length =
        (size_t)size;

    reader->pos +=
        (size_t)size;

    return true;
}


static aether_uuid_t r_uuid(
    reader_t *reader) {

    aether_uuid_t uuid = {
        0u,
        0u
    };

    for (unsigned i = 0u;
         i < 8u;
         ++i) {

        uuid.msb =
            (uuid.msb << 8u) |
            (uint64_t)r_u8(reader);
    }

    for (unsigned i = 0u;
         i < 8u;
         ++i) {

        uuid.lsb =
            (uuid.lsb << 8u) |
            (uint64_t)r_u8(reader);
    }

    return uuid;
}


static uint32_t next_request_id(
    aether_client_t *client) {

    /*
     * Bit 31 is permanently reserved for mandatory internal work requests.
     *
     * Registration and application requests therefore use only:
     *
     *   1 .. 0x7fffffff
     */
    const uint32_t normal_max =
        UINT32_C(0x7fffffff);

    uint32_t current =
        client->next_request_id &
        normal_max;

    if (current >=
        normal_max) {

        current =
            1u;

    } else {
        ++current;

        if (current == 0u) {
            current =
                1u;
        }
    }

    client->next_request_id =
        current;

    return current;
}


static aether_status_t transport_send(
    aether_client_t *client,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {

    if (client->config.transport.send ==
        NULL) {

        return AETHER_ERR_TRANSPORT;
    }

    return
        client->config.transport.send(
            client->config.transport.ctx,
            channel,
            data,
            length);
}


static aether_status_t transport_open(
    aether_client_t *client,
    aether_channel_t channel,
    const aether_endpoint_t *endpoint) {

    if (client->config.transport.open ==
        NULL) {

        return AETHER_ERR_TRANSPORT;
    }

    return
        client->config.transport.open(
            client->config.transport.ctx,
            channel,
            endpoint);
}


static void transport_close(
    aether_client_t *client,
    aether_channel_t channel) {

    if (client->config.transport.close !=
        NULL) {

        client->config.transport.close(
            client->config.transport.ctx,
            channel);
    }
}


static bool parse_signed_key(
    reader_t *reader,
    const aether_client_t *client,
    aether_signed_key_t *key) {

    const aether_crypto_profile_t *profile =
        &client->config.crypto->profile;

    memset(
        key,
        0,
        sizeof(*key));

    key->key_type =
        r_u8(reader);

    if (key->key_type !=
        profile->asymmetric_public_key_type) {

        reader->failed =
            true;

        return false;
    }

    if (!r_bytes(
            reader,
            key->key,
            profile->asymmetric_public_key_bytes)) {

        return false;
    }

    key->sign_type =
        r_u8(reader);

    if (key->sign_type !=
        profile->sign_type) {

        reader->failed =
            true;

        return false;
    }

    return
        r_bytes(
            reader,
            key->signature,
            profile->signature_bytes);
}


static bool verify_signed_key(
    const aether_client_t *client,
    const aether_signed_key_t *key) {

    const aether_crypto_vtable_t *crypto =
        client->config.crypto;

    const aether_crypto_profile_t *profile =
        &crypto->profile;

    if (key->key_type !=
            profile->asymmetric_public_key_type ||
        key->sign_type !=
            profile->sign_type) {

        return false;
    }

    for (size_t i = 0u;
         i <
             client->config.trusted_sign_key_count;
         ++i) {

        const uint8_t *root =
            client->config.trusted_sign_keys +
            i *
                profile->sign_public_key_bytes;

        if (crypto->verify_signature(
                client->config.crypto_ctx,
                root,
                profile->sign_public_key_bytes,
                key->signature,
                profile->signature_bytes,
                key->key,
                profile->asymmetric_public_key_bytes) ==
            AETHER_OK) {

            return true;
        }
    }

    return false;
}


static aether_status_t crypto_asymmetric_encrypt(
    aether_client_t *client,
    const uint8_t *public_key,
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    const aether_crypto_vtable_t *crypto =
        client->config.crypto;

    return
        crypto->asymmetric_encrypt(
            client->config.crypto_ctx,
            public_key,
            crypto->profile.asymmetric_public_key_bytes,
            plain,
            plain_len,
            out,
            out_capacity,
            out_len);
}


static aether_status_t crypto_symmetric_encrypt(
    aether_client_t *client,
    const uint8_t key[AETHER_KEY_BYTES],
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    const aether_crypto_vtable_t *crypto =
        client->config.crypto;

    return
        crypto->symmetric_encrypt(
            client->config.crypto_ctx,
            key,
            crypto->profile.symmetric_key_bytes,
            plain,
            plain_len,
            out,
            out_capacity,
            out_len);
}


static aether_status_t crypto_symmetric_decrypt(
    aether_client_t *client,
    const uint8_t key[AETHER_KEY_BYTES],
    const uint8_t *cipher,
    size_t cipher_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    const aether_crypto_vtable_t *crypto =
        client->config.crypto;

    return
        crypto->symmetric_decrypt(
            client->config.crypto_ctx,
            key,
            crypto->profile.symmetric_key_bytes,
            cipher,
            cipher_len,
            out,
            out_capacity,
            out_len);
}


static aether_status_t crypto_derive_server_keys(
    aether_client_t *client,
    const uint8_t master_key[AETHER_KEY_BYTES],
    int32_t server_id,
    uint32_t key_number,
    uint8_t client_to_server[AETHER_KEY_BYTES],
    uint8_t server_to_client[AETHER_KEY_BYTES]) {

    const aether_crypto_vtable_t *crypto =
        client->config.crypto;

    return
        crypto->derive_server_keys(
            client->config.crypto_ctx,
            master_key,
            crypto->profile.symmetric_key_bytes,
            server_id,
            key_number,
            client_to_server,
            server_to_client,
            crypto->profile.symmetric_key_bytes);
}




static bool cloud_contains(
    const aether_client_t *client,
    int16_t sid) {

    for (uint8_t i = 0u;
         i <
             client->cloud_count;
         ++i) {

        if (client->cloud_sids[i] ==
            sid) {

            return true;
        }
    }

    return false;
}


static bool skip_address(
    reader_t *reader,
    aether_address_t *out) {

    uint8_t type =
        r_u8(reader);

    size_t size = 0u;

    if (out != NULL) {
        memset(
            out,
            0,
            sizeof(*out));
    }

    if (type ==
        AETHER_ADDR_IPV4) {

        size = 4u;

    } else if (
        type ==
        AETHER_ADDR_IPV6) {

        size = 16u;

    } else if (
        type ==
        AETHER_ADDR_WEB_WIRE) {

        uint64_t size64 =
            r_pack(reader);

        if (reader->failed ||
            size64 > SIZE_MAX) {

            return false;
        }

        return
            r_bytes(
                reader,
                NULL,
                (size_t)size64);

    } else {
        reader->failed =
            true;

        return false;
    }

    if (out != NULL) {
        out->kind =
            (aether_address_kind_t)type;

        out->length =
            (uint8_t)size;

        return
            r_bytes(
                reader,
                out->bytes,
                size);
    }

    return
        r_bytes(
            reader,
            NULL,
            size);
}


static bool parse_server_descriptor(
    reader_t *reader,
    aether_server_t *server) {

    if (server != NULL) {
        memset(
            server,
            0,
            sizeof(*server));
    }

    int16_t sid =
        r_i16le(reader);

    if (server != NULL) {
        server->sid =
            sid;
    }

    uint64_t address_count =
        r_pack(reader);

    if (reader->failed) {
        return false;
    }

    bool have_endpoint =
        false;

    bool have_udp =
        false;

    for (uint64_t ai = 0u;
         ai < address_count;
         ++ai) {

        aether_address_t address;

        if (!skip_address(
                reader,
                server != NULL
                    ? &address
                    : NULL)) {

            return false;
        }

        bool address_supported =
            server != NULL &&
            address.length != 0u;

        uint64_t codec_count =
            r_pack(reader);

        if (reader->failed) {
            return false;
        }

        for (uint64_t ci = 0u;
             ci < codec_count;
             ++ci) {

            uint8_t codec =
                r_u8(reader);

            uint16_t port =
                r_u16le(reader);

            if (reader->failed) {
                return false;
            }

            if (!address_supported) {
                continue;
            }

            if (codec ==
                    AETHER_CODEC_UDP &&
                !have_udp) {

                server->endpoint.codec =
                    AETHER_CODEC_UDP;

                server->endpoint.address =
                    address;

                server->endpoint.port =
                    port;

                have_endpoint =
                    true;

                have_udp =
                    true;

            } else if (
                codec ==
                    AETHER_CODEC_TCP &&
                !have_endpoint) {

                server->endpoint.codec =
                    AETHER_CODEC_TCP;

                server->endpoint.address =
                    address;

                server->endpoint.port =
                    port;

                have_endpoint =
                    true;
            }
        }
    }

    if (server != NULL) {
        server->valid =
            have_endpoint;
    }

    return
        !reader->failed;
}


static bool parse_server_array(
    reader_t *reader,
    aether_client_t *client) {

    uint64_t count =
        r_pack(reader);

    if (reader->failed) {
        return false;
    }

    client->server_count =
        0u;

    for (uint64_t i = 0u;
         i < count;
         ++i) {

        aether_server_t *server =
            NULL;

        if (client->server_count <
            AETHER_MAX_SERVERS) {

            server =
                &client->servers[
                    client->server_count++
                ];
        }

        if (!parse_server_descriptor(
                reader,
                server)) {

            return false;
        }
    }

    return
        !reader->failed;
}


static int find_active_server(
    aether_client_t *client) {

    for (uint8_t i = 0u;
         i <
             client->server_count;
         ++i) {

        if (client->servers[i].valid &&
            cloud_contains(
                client,
                client->servers[i].sid)) {

            return
                (int)i;
        }
    }

    for (uint8_t i = 0u;
         i <
             client->server_count;
         ++i) {

        if (client->servers[i].valid) {
            return
                (int)i;
        }
    }

    return -1;
}


static uint32_t crc32_bytes(
    const uint8_t *data,
    size_t length) {

    uint32_t crc =
        UINT32_C(0xffffffff);

    for (size_t i = 0u;
         i < length;
         ++i) {

        crc ^=
            data[i];

        for (unsigned j = 0u;
             j < 8u;
             ++j) {

            uint32_t mask =
                (uint32_t)-
                    (int32_t)(
                        crc &
                        1u);

            crc =
                (crc >> 1u) ^
                (UINT32_C(0xedb88320) &
                 mask);
        }
    }

    return
        ~crc;
}


static aether_status_t save_state(
    aether_client_t *client) {

    if (client->config.flash.save ==
        NULL) {

        return AETHER_OK;
    }

    writer_t writer = {
        client->tx_plain,
        sizeof(client->tx_plain),
        0u,
        false
    };

    w_u8(&writer, AETHER_STATE_MAGIC0);
    w_u8(&writer, AETHER_STATE_MAGIC1);
    w_u8(&writer, AETHER_STATE_MAGIC2);
    w_u8(&writer, AETHER_STATE_MAGIC3);
    w_u8(&writer, AETHER_STATE_VERSION);

    w_u8(
        &writer,
        client->config.crypto->profile.crypto_lib);

    w_u8(
        &writer,
        client->registered
            ? 1u
            : 0u);

    w_u8(
        &writer,
        0u);

    w_uuid(
        &writer,
        client->uid);

    w_uuid(
        &writer,
        client->alias);

    w_bytes(
        &writer,
        client->master_key,
        AETHER_KEY_BYTES);

    w_u8(
        &writer,
        client->cloud_count);

    for (uint8_t i = 0u;
         i <
             client->cloud_count;
         ++i) {

        w_i16le(
            &writer,
            client->cloud_sids[i]);
    }

    w_u8(
        &writer,
        client->server_count);

    for (uint8_t i = 0u;
         i <
             client->server_count;
         ++i) {

        const aether_server_t *server =
            &client->servers[i];

        w_i16le(
            &writer,
            server->sid);

        w_u8(
            &writer,
            server->valid
                ? 1u
                : 0u);

        w_u8(
            &writer,
            (uint8_t)
                server->endpoint.codec);

        w_u8(
            &writer,
            (uint8_t)
                server->endpoint.address.kind);

        w_u8(
            &writer,
            server->endpoint.address.length);

        w_bytes(
            &writer,
            server->endpoint.address.bytes,
            server->endpoint.address.length);

        w_u16le(
            &writer,
            server->endpoint.port);
    }

    if (writer.failed ||
        writer.capacity -
            writer.pos <
        4u) {

        return AETHER_ERR_OVERFLOW;
    }

    uint32_t crc =
        crc32_bytes(
            writer.data,
            writer.pos);

    w_u32le(
        &writer,
        crc);

    if (writer.failed) {
        return AETHER_ERR_OVERFLOW;
    }

    return
        client->config.flash.save(
            client->config.flash.ctx,
            writer.data,
            writer.pos);
}


static bool load_state(
    aether_client_t *client) {

    if (client->config.flash.load ==
        NULL) {

        return false;
    }

    size_t length = 0u;

    if (client->config.flash.load(
            client->config.flash.ctx,
            client->tx_plain,
            sizeof(client->tx_plain),
            &length) !=
        AETHER_OK) {

        return false;
    }

    if (length <
        4u + 4u +
        16u + 16u +
        32u +
        1u + 1u +
        4u) {

        return false;
    }

    uint32_t stored_crc =
        (uint32_t)
            client->tx_plain[
                length - 4u
            ] |
        ((uint32_t)
            client->tx_plain[
                length - 3u
            ] <<
         8u) |
        ((uint32_t)
            client->tx_plain[
                length - 2u
            ] <<
         16u) |
        ((uint32_t)
            client->tx_plain[
                length - 1u
            ] <<
         24u);

    if (crc32_bytes(
            client->tx_plain,
            length - 4u) !=
        stored_crc) {

        return false;
    }

    reader_t reader = {
        client->tx_plain,
        length - 4u,
        0u,
        false
    };

    if (r_u8(&reader) != AETHER_STATE_MAGIC0 ||
        r_u8(&reader) != AETHER_STATE_MAGIC1 ||
        r_u8(&reader) != AETHER_STATE_MAGIC2 ||
        r_u8(&reader) != AETHER_STATE_MAGIC3 ||
        r_u8(&reader) != AETHER_STATE_VERSION) {

        return false;
    }

    uint8_t provider =
        r_u8(&reader);

    if (provider !=
        client->config.crypto->profile.crypto_lib) {

        return false;
    }

    client->registered =
        r_u8(&reader) != 0u;

    (void)r_u8(
        &reader);

    client->uid =
        r_uuid(&reader);

    client->alias =
        r_uuid(&reader);

    if (!r_bytes(
            &reader,
            client->master_key,
            AETHER_KEY_BYTES)) {

        return false;
    }

    uint8_t cloud_count =
        r_u8(&reader);

    if (cloud_count >
        AETHER_MAX_SERVERS) {

        return false;
    }

    client->cloud_count =
        cloud_count;

    for (uint8_t i = 0u;
         i < cloud_count;
         ++i) {

        client->cloud_sids[i] =
            r_i16le(&reader);
    }

    uint8_t server_count =
        r_u8(&reader);

    if (server_count >
        AETHER_MAX_SERVERS) {

        return false;
    }

    client->server_count =
        server_count;

    for (uint8_t i = 0u;
         i < server_count;
         ++i) {

        aether_server_t *server =
            &client->servers[i];

        memset(
            server,
            0,
            sizeof(*server));

        server->sid =
            r_i16le(&reader);

        bool stored_valid =
            r_u8(&reader) != 0u;

        uint8_t codec =
            r_u8(&reader);

        uint8_t kind =
            r_u8(&reader);

        uint8_t address_length =
            r_u8(&reader);

        if (reader.failed) {
            return false;
        }

        bool supported_codec =
            codec ==
                (uint8_t)AETHER_CODEC_TCP ||
            codec ==
                (uint8_t)AETHER_CODEC_UDP;

        bool supported_address =
            (kind ==
                 (uint8_t)AETHER_ADDR_IPV4 &&
             address_length == 4u) ||
            (kind ==
                 (uint8_t)AETHER_ADDR_IPV6 &&
             address_length == 16u);

        if (supported_codec &&
            supported_address) {

            server->endpoint.codec =
                (aether_codec_t)codec;

            server->endpoint.address.kind =
                (aether_address_kind_t)kind;

            server->endpoint.address.length =
                address_length;

            if (!r_bytes(
                    &reader,
                    server->endpoint.address.bytes,
                    address_length)) {

                return false;
            }

        } else {
            if (!r_bytes(
                    &reader,
                    NULL,
                    address_length)) {

                return false;
            }
        }

        server->endpoint.port =
            r_u16le(&reader);

        if (reader.failed) {
            return false;
        }

        server->valid =
            stored_valid &&
            supported_codec &&
            supported_address;
    }

    return
        !reader.failed &&
        reader.pos ==
            reader.length &&
        client->registered;
}


static aether_status_t ensure_required_callbacks(
    aether_client_t *client) {

    if (client->config.transport.open ==
            NULL ||
        client->config.transport.send ==
            NULL ||
        client->config.pow.generate ==
            NULL ||
        client->config.crypto ==
            NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    const aether_crypto_vtable_t *crypto =
        client->config.crypto;

    const aether_crypto_profile_t *profile =
        &crypto->profile;

    if (crypto->random_symmetric_key ==
            NULL ||
        crypto->verify_signature ==
            NULL ||
        crypto->asymmetric_encrypt ==
            NULL ||
        crypto->symmetric_encrypt ==
            NULL ||
        crypto->symmetric_decrypt ==
            NULL ||
        crypto->derive_server_keys ==
            NULL) {

        return AETHER_ERR_ARGUMENT;
    }

    if (profile->symmetric_key_bytes !=
            AETHER_KEY_BYTES ||
        profile->asymmetric_public_key_bytes ==
            0u ||
        profile->asymmetric_public_key_bytes >
            AETHER_MAX_CRYPTO_PUBLIC_KEY_BYTES ||
        profile->sign_public_key_bytes ==
            0u ||
        profile->sign_public_key_bytes >
            AETHER_MAX_CRYPTO_PUBLIC_KEY_BYTES ||
        profile->signature_bytes ==
            0u ||
        profile->signature_bytes >
            AETHER_MAX_CRYPTO_SIGNATURE_BYTES) {

        return AETHER_ERR_UNSUPPORTED;
    }

    if (client->config.trusted_sign_keys ==
            NULL ||
        client->config.trusted_sign_key_count ==
            0u) {

        return AETHER_ERR_ARGUMENT;
    }

    if (crypto->initialize !=
        NULL) {

        aether_status_t status =
            crypto->initialize(
                client->config.crypto_ctx);

        if (status != AETHER_OK) {
            return status;
        }
    }

    return AETHER_OK;
}


static bool registration_in_progress(
    aether_state_t state) {

    return
        state ==
            AETHER_STATE_REG_CONNECTING ||
        state ==
            AETHER_STATE_REG_WAIT_SERVER_KEY ||
        state ==
            AETHER_STATE_REG_WAIT_POW ||
        state ==
            AETHER_STATE_REG_WAIT_FINISH ||
        state ==
            AETHER_STATE_REG_WAIT_SERVERS;
}


static void set_registration_state(
    aether_client_t *client,
    aether_state_t state) {

    client->state_started_at_ms =
        (uint32_t)
            client->now_ms;

    set_state(
        client,
        state);
}


static aether_status_t begin_registration(
    aether_client_t *client) {

    client->registered =
        false;

    set_registration_state(
        client,
        AETHER_STATE_REG_CONNECTING);

    aether_status_t status =
        transport_open(
            client,
            AETHER_CHANNEL_REGISTRATION,
            &client->config.registration_endpoint);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    return AETHER_OK;
}


static aether_status_t restart_registration(
    aether_client_t *client) {

    transport_close(
        client,
        AETHER_CHANNEL_REGISTRATION);

    return
        begin_registration(
            client);
}


static aether_status_t connect_next_work_server(
    aether_client_t *client,
    int previous_index) {

    if (client->server_count ==
        0u) {

        return
            fail(
                client,
                AETHER_ERR_PROTOCOL);
    }

    uint8_t start = 0u;

    if (previous_index >= 0) {
        start =
            (uint8_t)(
                ((uint8_t)previous_index +
                 1u) %
                client->server_count);
    }

    bool found_valid =
        false;

    aether_status_t last_open_status =
        AETHER_ERR_TRANSPORT;

    for (uint8_t offset = 0u;
         offset <
             client->server_count;
         ++offset) {

        uint8_t index =
            (uint8_t)(
                (start + offset) %
                client->server_count);

        const aether_server_t *server =
            &client->servers[index];

        if (!server->valid) {
            continue;
        }

        found_valid =
            true;

        if (crypto_derive_server_keys(
                client,
                client->master_key,
                (int32_t)server->sid,
                0u,
                client->work_tx_key,
                client->work_rx_key) !=
            AETHER_OK) {

            return
                fail(
                    client,
                    AETHER_ERR_CRYPTO);
        }

        client->active_server_index =
            (int8_t)index;

        client->state_started_at_ms =
            (uint32_t)
                client->now_ms;

        set_state(
            client,
            AETHER_STATE_WORK_CONNECTING);

        aether_status_t status =
            transport_open(
                client,
                AETHER_CHANNEL_WORK,
                &server->endpoint);

        if (status == AETHER_OK) {
            return AETHER_OK;
        }

        last_open_status =
            status;
    }

    client->active_server_index =
        -1;

    if (!found_valid) {
        return
            fail(
                client,
                AETHER_ERR_PROTOCOL);
    }

    return
        fail(
            client,
            last_open_status);
}


static aether_status_t begin_work(
    aether_client_t *client) {

    int preferred =
        find_active_server(
            client);

    if (preferred < 0) {
        return
            fail(
                client,
                AETHER_ERR_PROTOCOL);
    }

    client->active_server_index =
        -1;

    int previous =
        preferred == 0
            ? -1
            : preferred - 1;

    return
        connect_next_work_server(
            client,
            previous);
}


static aether_status_t send_get_server_key(
    aether_client_t *client) {

    client->req_server_key =
        next_request_id(
            client);

    size_t packet_length =
        0u;

    aether_status_t status =
        aether_generated_reg_root_build_get_key(
            client->tx_plain,
            sizeof(client->tx_plain),
            &packet_length,
            client->req_server_key,
            client->config.crypto
                ->profile.crypto_lib);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    status =
        transport_send(
            client,
            AETHER_CHANNEL_REGISTRATION,
            client->tx_plain,
            packet_length);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    set_registration_state(
        client,
        AETHER_STATE_REG_WAIT_SERVER_KEY);

    return AETHER_OK;
}


static aether_status_t send_reg_enter(
    aether_client_t *client,
    const uint8_t *nested,
    size_t nested_len) {

    size_t cipher_len =
        0u;

    if (crypto_asymmetric_encrypt(
            client,
            client->registration_server_key.key,
            nested,
            nested_len,
            client->tx_crypto,
            sizeof(client->tx_crypto),
            &cipher_len) !=
        AETHER_OK) {

        return
            fail(
                client,
                AETHER_ERR_CRYPTO);
    }

    size_t packet_length =
        0u;

    aether_status_t status =
        aether_generated_reg_root_build_enter(
            client->tx_plain,
            sizeof(client->tx_plain),
            &packet_length,
            client->config.crypto
                ->profile.crypto_lib,
            client->tx_crypto,
            cipher_len);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    status =
        transport_send(
            client,
            AETHER_CHANNEL_REGISTRATION,
            client->tx_plain,
            packet_length);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    return AETHER_OK;
}


static aether_status_t send_pow_request(
    aether_client_t *client) {

    client->req_pow =
        next_request_id(
            client);

    size_t nested_len =
        0u;

    aether_status_t status =
        aether_generated_reg_safe_build_pow(
            client->tx_plain,
            sizeof(client->tx_plain),
            &nested_len,
            client->config.crypto
                ->profile.symmetric_key_type,
            client->temp_key,
            client->config.crypto
                ->profile.symmetric_key_bytes,
            client->req_pow,
            client->config.parent_uid);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    status =
        send_reg_enter(
            client,
            client->tx_plain,
            nested_len);

    if (status == AETHER_OK) {
        set_registration_state(
            client,
            AETHER_STATE_REG_WAIT_POW);
    }

    return status;
}


static aether_status_t send_registration_payload(
    aether_client_t *client) {

    client->req_finish =
        next_request_id(
            client);

    size_t nested_len =
        0u;

    aether_status_t status =
        aether_generated_reg_safe_build_registration_direct(
            client->tx_plain,
            sizeof(client->tx_plain),
            &nested_len,
            client->config.crypto
                ->profile.symmetric_key_type,
            client->temp_key,
            client->config.crypto
                ->profile.symmetric_key_bytes,
            client->master_key,
            client->config.crypto
                ->profile.symmetric_key_bytes,
            client->req_finish,
            client->pow_salt,
            client->pow_salt_len,
            client->pow_suffix,
            client->pow_suffix_len,
            client->pow_passwords,
            client->pow_password_count,
            client->config.parent_uid);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    status =
        send_reg_enter(
            client,
            client->tx_plain,
            nested_len);

    if (status == AETHER_OK) {
        set_registration_state(
            client,
            AETHER_STATE_REG_WAIT_FINISH);
    }

    return status;
}


static aether_status_t send_resolve_servers(
    aether_client_t *client) {

    client->req_resolve =
        next_request_id(
            client);

    size_t nested_len =
        0u;

    aether_status_t status =
        aether_generated_reg_safe_build_resolve(
            client->tx_plain,
            sizeof(client->tx_plain),
            &nested_len,
            client->req_resolve,
            client->cloud_sids,
            client->cloud_count);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    status =
        send_reg_enter(
            client,
            client->tx_plain,
            nested_len);

    if (status == AETHER_OK) {
        set_registration_state(
            client,
            AETHER_STATE_REG_WAIT_SERVERS);
    }

    return status;
}


static bool parse_work_proof(
    reader_t *reader,
    aether_client_t *client) {

    const uint8_t *salt;
    size_t salt_len;

    const uint8_t *suffix;
    size_t suffix_len;

    if (!r_view(
            reader,
            &salt,
            &salt_len) ||
        !r_view(
            reader,
            &suffix,
            &suffix_len)) {

        return false;
    }

    if (salt_len >
            sizeof(client->pow_salt) ||
        suffix_len >
            sizeof(client->pow_suffix)) {

        return false;
    }

    memcpy(
        client->pow_salt,
        salt,
        salt_len);

    memcpy(
        client->pow_suffix,
        suffix,
        suffix_len);

    client->pow_salt_len =
        salt_len;

    client->pow_suffix_len =
        suffix_len;

    client->pow_pool_size =
        r_u8(reader);

    client->pow_max_hash =
        r_i32le(reader);

    return
        parse_signed_key(
            reader,
            client,
            &client->global_key) &&
        !reader->failed;
}


static aether_status_t handle_pow_response(
    aether_client_t *client,
    reader_t *reader) {

    if (!parse_work_proof(
            reader,
            client)) {

        return
            fail(
                client,
                AETHER_ERR_PROTOCOL);
    }

    if (!verify_signed_key(
            client,
            &client->global_key)) {

        return
            fail_with_origin(
                client,
                AETHER_ERR_CRYPTO,
                AETHER_ERROR_ORIGIN_TRUST);
    }

    size_t count =
        0u;

    aether_status_t status =
        client->config.pow.generate(
            client->config.pow.ctx,
            client->pow_salt,
            client->pow_salt_len,
            client->pow_suffix,
            client->pow_suffix_len,
            client->pow_pool_size,
            client->pow_max_hash,
            client->pow_passwords,
            AETHER_MAX_POW_PASSWORDS,
            &count);

    if (status != AETHER_OK) {
        return
            fail_with_origin(
                client,
                status,
                AETHER_ERROR_ORIGIN_POW);
    }

    if (count >
        AETHER_MAX_POW_PASSWORDS) {

        return
            fail(
                client,
                AETHER_ERR_OVERFLOW);
    }

    client->pow_password_count =
        count;

    return
        send_registration_payload(
            client);
}


static aether_status_t handle_reg_safe_plain(
    aether_client_t *client,
    const uint8_t *data,
    size_t length) {

    reader_t reader = {
        data,
        length,
        0u,
        false
    };

    while (!reader.failed &&
           reader.pos <
               reader.length) {


        size_t frame_start =
            reader.pos;


        uint8_t command =
            r_u8(&reader);

        if (command ==
            CMD_RESULT) {

            uint32_t request_id =
                r_u32le(&reader);

            if (request_id ==
                    client->req_pow &&
                client->state ==
                    AETHER_STATE_REG_WAIT_POW) {

                aether_status_t status =
                    handle_pow_response(
                        client,
                        &reader);

                if (status != AETHER_OK) {
                    return status;
                }


            } else if (
                request_id ==
                    client->req_finish &&
                client->state ==
                    AETHER_STATE_REG_WAIT_FINISH) {

                server_registration_api_registration_direct_response_storage_t storage = {
                    .cloud_data_storage =
                        client->cloud_sids,
                    .cloud_data_capacity =
                        sizeof(client->cloud_sids) /
                        sizeof(client->cloud_sids[0])
                };

                server_registration_api_registration_direct_response_frame_t frame = {
                    0
                };

                size_t consumed =
                    0u;

                aether_status_t status =
                    server_registration_api_registration_direct_response_decode(
                        &storage,
                        data + frame_start,
                        length - frame_start,
                        &frame,
                        &consumed);

                if (status != AETHER_OK ||
                    consumed == 0u ||
                    consumed > length - frame_start ||
                    frame.kind !=
                        SERVER_REGISTRATION_API_REGISTRATION_DIRECT_RESPONSE_RESULT ||
                    frame.request_id !=
                        request_id) {

                    return
                        fail(
                            client,
                            status != AETHER_OK
                                ? status
                                : AETHER_ERR_PROTOCOL);
                }

                client->alias =
                    frame.result.alias;

                client->uid =
                    frame.result.uid;

                client->cloud_count =
                    frame.result.cloud.data.length;

                aether_event_t event;

                memset(
                    &event,
                    0,
                    sizeof(event));

                event.type =
                    AETHER_EVENT_REGISTERED;

                event.as.registered.uid =
                    client->uid;

                event.as.registered.alias =
                    client->alias;

                emit_event(
                    client,
                    &event);

                return
                    send_resolve_servers(
                        client);


            } else if (
                request_id ==
                    client->req_resolve &&
                client->state ==
                    AETHER_STATE_REG_WAIT_SERVERS) {

                if (!parse_server_array(
                        &reader,
                        client)) {

                    return
                        fail(
                            client,
                            AETHER_ERR_PROTOCOL);
                }

                client->registered =
                    true;

                if (save_state(client) !=
                    AETHER_OK) {

                    return
                        fail(
                            client,
                            AETHER_ERR_STORAGE);
                }

                transport_close(
                    client,
                    AETHER_CHANNEL_REGISTRATION);

                return
                    begin_work(
                        client);

            } else {
                return
                    fail(
                        client,
                        AETHER_ERR_PROTOCOL);
            }

            continue;
        }

        if (command ==
            CMD_ERROR) {

            uint32_t request_id =
                r_u32le(&reader);

            if (reader.failed) {
                return
                    fail(
                        client,
                        AETHER_ERR_PROTOCOL);
            }

            return
                fail_remote(
                    client,
                    request_id);
        }

        return
            fail(
                client,
                AETHER_ERR_PROTOCOL);
    }

    return
        reader.failed
            ? fail(
                  client,
                  AETHER_ERR_PROTOCOL)
            : AETHER_OK;
}


static aether_status_t handle_global_plain(
    aether_client_t *client,
    const uint8_t *data,
    size_t length) {

    (void)data;
    (void)length;

    return
        fail(
            client,
            AETHER_ERR_PROTOCOL);
}


static aether_status_t handle_registration_rx(
    aether_client_t *client,
    const uint8_t *data,
    size_t length) {

    reader_t reader = {
        data,
        length,
        0u,
        false
    };

    while (!reader.failed &&
           reader.pos <
               reader.length) {


size_t frame_start =
            reader.pos;

        uint8_t command =
            r_u8(&reader);


        if (command ==
            CMD_RESULT) {

            uint32_t request_id =
                r_u32le(&reader);

            if (request_id !=
                    client->req_server_key ||
                client->state !=
                    AETHER_STATE_REG_WAIT_SERVER_KEY) {

                return
                    fail(
                        client,
                        AETHER_ERR_PROTOCOL);
            }

            if (!parse_signed_key(
                    &reader,
                    client,
                    &client->registration_server_key)) {

                return
                    fail(
                        client,
                        AETHER_ERR_PROTOCOL);
            }

            if (!verify_signed_key(
                    client,
                    &client->registration_server_key)) {

                return
                    fail_with_origin(
                        client,
                        AETHER_ERR_CRYPTO,
                        AETHER_ERROR_ORIGIN_TRUST);
            }

            if (client->config.crypto
                        ->random_symmetric_key(
                    client->config.crypto_ctx,
                    client->temp_key,
                    client->config.crypto
                        ->profile.symmetric_key_bytes) !=
                    AETHER_OK ||
                client->config.crypto
                        ->random_symmetric_key(
                    client->config.crypto_ctx,
                    client->master_key,
                    client->config.crypto
                        ->profile.symmetric_key_bytes) !=
                    AETHER_OK) {

                return
                    fail(
                        client,
                        AETHER_ERR_CRYPTO);
            }

            return
                send_pow_request(
                    client);
        }

        if (command ==
            CMD_ERROR) {

            uint32_t request_id =
                r_u32le(&reader);

            if (reader.failed) {
                return
                    fail(
                        client,
                        AETHER_ERR_PROTOCOL);
            }

            return
                fail_remote(
                    client,
                    request_id);
        }


aether_reg_unsafe_frame_t frame = {
            0
        };

        size_t consumed =
            0u;

        aether_status_t decode_status =
            aether_generated_reg_unsafe_decode(
                &frame,
                reader.data +
                    frame_start,
                reader.length -
                    frame_start,
                &consumed);

        if (decode_status != AETHER_OK ||
            consumed == 0u ||
            consumed >
                reader.length -
                    frame_start) {

            return
                fail(
                    client,
                    AETHER_ERR_PROTOCOL);
        }

        const uint8_t *key;

        if (frame.kind ==
            AETHER_REG_UNSAFE_FRAME_ENTER) {

            key =
                client->temp_key;

        } else if (
            frame.kind ==
            AETHER_REG_UNSAFE_FRAME_ENTER_GLOBAL) {

            key =
                client->master_key;

        } else {
            return
                fail(
                    client,
                    AETHER_ERR_PROTOCOL);
        }

        size_t plain_len =
            0u;

        if (crypto_symmetric_decrypt(
                client,
                key,
                frame.data,
                frame.length,
                client->tx_crypto,
                sizeof(client->tx_crypto),
                &plain_len) !=
            AETHER_OK) {

            return
                fail(
                    client,
                    AETHER_ERR_CRYPTO);
        }

        aether_status_t status =
            frame.kind ==
                AETHER_REG_UNSAFE_FRAME_ENTER
                ? handle_reg_safe_plain(
                      client,
                      client->tx_crypto,
                      plain_len)
                : handle_global_plain(
                      client,
                      client->tx_crypto,
                      plain_len);

        if (status != AETHER_OK) {
            return status;
        }

        reader.pos =
            frame_start +
            consumed;

        continue;


        return
            fail(
                client,
                AETHER_ERR_PROTOCOL);
    }

    return
        reader.failed
            ? fail(
                  client,
                  AETHER_ERR_PROTOCOL)
            : AETHER_OK;
}



static uint32_t operation_timeout_ms(
    const aether_client_t *client) {

    if (client->config.request_timeout_ms !=
        0u) {

        return
            client->config.request_timeout_ms;
    }

    uint32_t base =
        client->config.ping_interval_ms;

    if (client->config.rx_window_ms >
        base) {

        base =
            client->config.rx_window_ms;
    }

    if (base == 0u) {
        return 10000u;
    }

    if (base >
        UINT32_MAX /
            2u) {

        return UINT32_MAX;
    }

    return
        base * 2u;
}



static aether_status_t send_authorized_plain(
    aether_client_t *client,
    const uint8_t *plain,
    size_t plain_len) {

    size_t cipher_len =
        0u;

    if (crypto_symmetric_encrypt(
            client,
            client->work_tx_key,
            plain,
            plain_len,
            client->tx_crypto,
            sizeof(client->tx_crypto),
            &cipher_len) !=
        AETHER_OK) {

        return AETHER_ERR_CRYPTO;
    }

    writer_t outer = {
        client->tx_plain,
        sizeof(client->tx_plain),
        0u,
        false
    };

    w_u8(
        &outer,
        LOGIN_BY_ALIAS);

    w_uuid(
        &outer,
        client->alias);

    w_byte_array(
        &outer,
        client->tx_crypto,
        cipher_len);

    if (outer.failed) {
        return AETHER_ERR_OVERFLOW;
    }


    return
        transport_send(
            client,
            AETHER_CHANNEL_WORK,
            outer.data,
            outer.pos);
}


static aether_status_t generated_authorized_send(
    void *ctx,
    const uint8_t *data,
    size_t length) {

    if (ctx == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    return send_authorized_plain(
        (aether_client_t *)ctx,
        data,
        length);
}



static aether_status_t send_ping(
    aether_client_t *client) {

    if (client->state !=
        AETHER_STATE_READY) {

        return AETHER_ERR_STATE;
    }

    /*
     * tx_crypto owns the decrypted RX block while a frame is pinned.
     */
    if (client->rx_active) {
        return AETHER_ERR_BUSY;
    }

    /*
     * Mandatory internal work requests live in the high-bit namespace.
     *
     * The low 31 bits still come from the shared monotonic sequence, so
     * consecutive pings do not reuse one fixed wire request id.
     */
    uint32_t request_id =
        next_request_id(
            client) |
        UINT32_C(0x80000000);

    writer_t inner = {
        client->tx_plain,
        sizeof(client->tx_plain),
        0u,
        false
    };

    w_u8(
        &inner,
        AUTH_PING);

    w_u32le(
        &inner,
        request_id);

    w_i64le(
        &inner,
        (int64_t)
            client->config.ping_interval_ms);

    w_i64le(
        &inner,
        (int64_t)
            client->config.rx_window_ms);

    w_u8(
        &inner,
        AUTH_SET_RECEIVE_WINDOW);

    w_i64le(
        &inner,
        (int64_t)
            client->config.ping_interval_ms);

    w_i64le(
        &inner,
        (int64_t)
            client->config.rx_window_ms);

    if (inner.failed) {
        return AETHER_ERR_OVERFLOW;
    }

    aether_status_t status =
        send_authorized_plain(
            client,
            inner.data,
            inner.pos);

    client->next_ping_at_ms =
        client->now_ms +
        client->config.ping_interval_ms;

    return status;
}


/*
 * Yield one message and stop parser progress.
 *
 * payload points into tx_crypto and therefore remains stable while the
 * ingress is pending and while rx_active pins the parser state.
 */
static aether_status_t yield_message(
    aether_client_t *client,
    reader_t *reader) {

    if (client->ingress.kind !=
        AETHER_INGRESS_NONE) {

        return AETHER_ERR_BUSY;
    }

    aether_uuid_t from =
        r_uuid(reader);

    const uint8_t *payload;
    size_t length;

    if (!r_view(
            reader,
            &payload,
            &length) ||
        reader->failed) {

        return AETHER_ERR_PROTOCOL;
    }

    client->ingress.kind =
        AETHER_INGRESS_MESSAGE;

    client->ingress.as.message.from =
        from;

    client->ingress.as.message.payload.data =
        payload;

    client->ingress.as.message.payload.length =
        length;

    return AETHER_OK;
}


/*
 * Resume one decrypted ClientApiSafe stream from tx_crypto.
 *
 * The function stops immediately after yielding one message.
 */
static aether_status_t resume_client_safe_plain(
    aether_client_t *client) {

    reader_t reader = {
        client->tx_crypto,
        client->rx_plain_length,
        client->rx_plain_pos,
        false
    };

    if (client->rx_messages_remaining !=
        0u) {

        aether_status_t status =
            yield_message(
                client,
                &reader);

        if (status != AETHER_OK) {
            return status;
        }

        --client->rx_messages_remaining;

        client->rx_plain_pos =
            reader.pos;

        return AETHER_OK;
    }

    while (!reader.failed &&
           reader.pos <
               reader.length) {

        uint8_t command =
            r_u8(&reader);

        if (command ==
            CMD_RESULT) {

            uint32_t request_id =
                r_u32le(&reader);

            if (reader.failed) {
                return AETHER_ERR_PROTOCOL;
            }

            /*
             * High-bit request ids are mandatory internal work traffic.
             * A delayed ping acknowledgement is therefore harmless even after
             * many later application requests have been issued.
             */
            if ((request_id &
                 UINT32_C(0x80000000)) !=
                0u) {

                continue;
            }

            if (client->ingress.kind !=
                AETHER_INGRESS_NONE) {

                return AETHER_ERR_BUSY;
            }

            client->ingress.kind =
                AETHER_INGRESS_REQUEST_RESULT;

            client->ingress.as.request_result.request_id =
                request_id;

            client->ingress.as.request_result.status =
                AETHER_OK;

            client->rx_plain_pos =
                reader.pos;

            return AETHER_OK;
        }

        if (command ==
            CMD_ERROR) {

            uint32_t request_id =
                r_u32le(&reader);

            if (reader.failed) {
                return AETHER_ERR_PROTOCOL;
            }

            if ((request_id &
                 UINT32_C(0x80000000)) !=
                0u) {

                /*
                 * Preserve the compatibility diagnostic for mandatory
                 * internal traffic, but do not publish a send-future result.
                 */
                emit_error(
                    client,
                    AETHER_ERR_REMOTE,
                    AETHER_ERROR_ORIGIN_REMOTE,
                    client->state,
                    request_id);

                continue;
            }

            if (client->ingress.kind !=
                AETHER_INGRESS_NONE) {

                return AETHER_ERR_BUSY;
            }

            client->ingress.kind =
                AETHER_INGRESS_REQUEST_RESULT;

            client->ingress.as.request_result.request_id =
                request_id;

            client->ingress.as.request_result.status =
                AETHER_ERR_REMOTE;

            emit_error(
                client,
                AETHER_ERR_REMOTE,
                AETHER_ERROR_ORIGIN_REMOTE,
                client->state,
                request_id);

            client->rx_plain_pos =
                reader.pos;

            return AETHER_OK;
        }

        if (command ==
            CLIENT_SAFE_SEND_MESSAGE) {

            aether_status_t status =
                yield_message(
                    client,
                    &reader);

            if (status != AETHER_OK) {
                return status;
            }

            client->rx_plain_pos =
                reader.pos;

            return AETHER_OK;
        }

        if (command ==
            CLIENT_SAFE_SEND_MESSAGES) {

            uint64_t count =
                r_pack(&reader);

            if (reader.failed) {
                return AETHER_ERR_PROTOCOL;
            }

            if (count == 0u) {
                continue;
            }

            client->rx_messages_remaining =
                count;

            aether_status_t status =
                yield_message(
                    client,
                    &reader);

            if (status != AETHER_OK) {
                return status;
            }

            --client->rx_messages_remaining;

            client->rx_plain_pos =
                reader.pos;

            return AETHER_OK;
        }

        return AETHER_ERR_UNSUPPORTED;
    }

    if (reader.failed) {
        return AETHER_ERR_PROTOCOL;
    }

    client->rx_plain_pos =
        reader.pos;

    client->rx_plain_length =
        0u;

    client->rx_plain_pos =
        0u;

    client->rx_messages_remaining =
        0u;

    client->rx_plain_active =
        false;

    return AETHER_OK;
}


/*
 * Resume processing the current borrowed work frame.
 *
 * Outer packet bytes remain in platform-owned storage.
 * Decrypted safe bytes remain in tx_crypto.
 */
static aether_status_t handle_work_rx(
    aether_client_t *client) {

    while (client->rx_active) {
        if (client->ingress.kind !=
            AETHER_INGRESS_NONE) {

            return AETHER_OK;
        }

        if (client->rx_plain_active) {
            aether_status_t status =
                resume_client_safe_plain(
                    client);

            if (status != AETHER_OK) {
                return status;
            }

            if (client->ingress.kind !=
                AETHER_INGRESS_NONE) {

                return AETHER_OK;
            }

            continue;
        }

        if (client->rx_outer_pos >=
            client->rx_frame_length) {

            client->rx_active =
                false;

            client->rx_frame_data =
                NULL;

            client->rx_frame_length =
                0u;

            client->rx_outer_pos =
                0u;

            return AETHER_OK;
        }

        reader_t outer = {
            client->rx_frame_data,
            client->rx_frame_length,
            client->rx_outer_pos,
            false
        };

        uint8_t command =
            r_u8(&outer);

        if (command ==
            CLIENT_UNSAFE_SAFE_DATA_MULTI) {

            (void)r_u8(
                &outer);

        } else if (
            command !=
            CLIENT_UNSAFE_SAFE_DATA) {

            return AETHER_ERR_UNSUPPORTED;
        }

        const uint8_t *cipher;
        size_t cipher_len;

        if (!r_view(
                &outer,
                &cipher,
                &cipher_len)) {

            return AETHER_ERR_PROTOCOL;
        }

        client->rx_outer_pos =
            outer.pos;

        size_t plain_len =
            0u;

        if (crypto_symmetric_decrypt(
                client,
                client->work_rx_key,
                cipher,
                cipher_len,
                client->tx_crypto,
                sizeof(client->tx_crypto),
                &plain_len) !=
            AETHER_OK) {

            return AETHER_ERR_CRYPTO;
        }

        client->rx_plain_length =
            plain_len;

        client->rx_plain_pos =
            0u;

        client->rx_messages_remaining =
            0u;

        client->rx_plain_active =
            true;
    }

    return AETHER_OK;
}


void aether_client_init(
    aether_client_t *client,
    const aether_client_config_t *config) {

    if (client == NULL) {
        return;
    }

    memset(
        client,
        0,
        sizeof(*client));

    client->state =
        AETHER_STATE_STOPPED;

    client->active_server_index =
        -1;

    client->rx_channel =
        AETHER_CHANNEL_WORK;

    client->ingress.kind =
        AETHER_INGRESS_NONE;

    if (config != NULL) {
        client->config =
            *config;
    }

    if (client->config.ping_interval_ms ==
        0u) {

        client->config.ping_interval_ms =
            6000u;
    }

    if (client->config.rx_window_ms ==
        0u) {

        client->config.rx_window_ms =
            5000u;
    }
}


aether_status_t aether_client_start(
    aether_client_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (client->state !=
        AETHER_STATE_STOPPED) {

        return AETHER_ERR_STATE;
    }

    if (client->callback_active) {
        return AETHER_ERR_BUSY;
    }

    clear_rx_state(
        client);

    aether_status_t status =
        ensure_required_callbacks(
            client);

    if (status != AETHER_OK) {
        return
            fail(
                client,
                status);
    }

    if (load_state(client)) {
        return
            begin_work(
                client);
    }

    return
        begin_registration(
            client);
}


aether_status_t aether_client_retry(
    aether_client_t *client) {

    if (client == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    if (client->callback_active) {
        return AETHER_ERR_BUSY;
    }

    if (client->state !=
        AETHER_STATE_ERROR) {

        return AETHER_ERR_STATE;
    }

    transport_close(
        client,
        AETHER_CHANNEL_REGISTRATION);

    transport_close(
        client,
        AETHER_CHANNEL_WORK);

    clear_rx_state(
        client);

    client->next_ping_at_ms =
        0u;

    client->state_started_at_ms =
        (uint32_t)
            client->now_ms;

    if (client->registered) {
        return
            begin_work(
                client);
    }

    return
        begin_registration(
            client);
}


void aether_client_stop(
    aether_client_t *client) {

    if (client == NULL ||
        client->callback_active) {

        return;
    }

    transport_close(
        client,
        AETHER_CHANNEL_REGISTRATION);

    transport_close(
        client,
        AETHER_CHANNEL_WORK);

    clear_rx_state(
        client);

    client->time_initialized =
        false;

    client->now_ms =
        0u;

    client->next_ping_at_ms =
        0u;

    client->state_started_at_ms =
        0u;

    set_state(
        client,
        AETHER_STATE_STOPPED);
}


void aether_client_poll(
    aether_client_t *client,
    uint64_t now_ms) {

    if (client == NULL ||
        client->callback_active) {

        return;
    }

    if (!client->time_initialized) {
        client->time_initialized =
            true;

        client->now_ms =
            now_ms;

        uint32_t now32 =
            (uint32_t)now_ms;

        if (registration_in_progress(
                client->state) ||
            client->state ==
                AETHER_STATE_WORK_CONNECTING) {

            client->state_started_at_ms =
                now32;
        }

        if (client->state ==
                AETHER_STATE_READY &&
            client->next_ping_at_ms !=
                0u) {

            client->next_ping_at_ms =
                now_ms +
                client->config.ping_interval_ms;
        }

    } else {
        client->now_ms =
            now_ms;
    }

    /*
     * A pinned RX frame owns tx_crypto and may still reference the platform
     * receive buffer. Do not run timeout/reconnect/send transitions until
     * bounded parser work is drained.
     *
     * Application request timeout is caller-owned future policy now.
     */
    if (client->rx_active) {
        return;
    }

    if (registration_in_progress(
            client->state)) {

        uint32_t elapsed =
            (uint32_t)now_ms -
            client->state_started_at_ms;

        if (elapsed >=
            operation_timeout_ms(
                client)) {

            (void)restart_registration(
                client);

            return;
        }
    }

    if (client->state ==
        AETHER_STATE_WORK_CONNECTING) {

        uint32_t elapsed =
            (uint32_t)now_ms -
            client->state_started_at_ms;

        if (elapsed >=
            operation_timeout_ms(
                client)) {

            int previous_index =
                client->active_server_index;

            transport_close(
                client,
                AETHER_CHANNEL_WORK);

            (void)connect_next_work_server(
                client,
                previous_index);

            return;
        }
    }

    if (client->state ==
            AETHER_STATE_READY &&
        (client->next_ping_at_ms ==
             0u ||
         now_ms >=
             client->next_ping_at_ms)) {

        aether_status_t status =
            send_ping(
                client);

        if (status != AETHER_OK &&
            status != AETHER_ERR_BUSY) {

            emit_error(
                client,
                status,
                error_origin_for_status(
                    status),
                client->state,
                0u);
        }
    }
}


void aether_client_on_transport_state(
    aether_client_t *client,
    aether_channel_t channel,
    bool writable) {

    if (client == NULL ||
        client->callback_active) {

        return;
    }

    /*
     * Current pull adapters call transport-state notifications only from
     * platform.poll(). app_tick() pauses platform.poll() while rx_active, so a
     * pinned borrowed frame cannot be invalidated by reconnect.
     */
    if (client->rx_active &&
        channel ==
            AETHER_CHANNEL_WORK) {

        return;
    }

    if (!writable) {
        if (channel ==
            AETHER_CHANNEL_REGISTRATION) {

            if (registration_in_progress(
                    client->state)) {

                (void)begin_registration(
                    client);
            }

        } else if (
            channel ==
                AETHER_CHANNEL_WORK &&
            client->registered) {

            (void)connect_next_work_server(
                client,
                client->active_server_index);
        }

        return;
    }

    if (channel ==
            AETHER_CHANNEL_REGISTRATION &&
        client->state ==
            AETHER_STATE_REG_CONNECTING) {

        (void)send_get_server_key(
            client);

    } else if (
        channel ==
            AETHER_CHANNEL_WORK &&
        client->state ==
            AETHER_STATE_WORK_CONNECTING) {

        set_state(
            client,
            AETHER_STATE_READY);

        client->next_ping_at_ms =
            0u;

        aether_status_t status =
            send_ping(
                client);

        if (status != AETHER_OK &&
            status != AETHER_ERR_BUSY) {

            emit_error(
                client,
                status,
                error_origin_for_status(
                    status),
                client->state,
                0u);
        }
    }
}


aether_status_t aether_client_on_rx(
    aether_client_t *client,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {

    if (client == NULL ||
        (data == NULL &&
         length != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    if (client->callback_active) {
        return AETHER_ERR_BUSY;
    }

    if (length == 0u) {
        return AETHER_OK;
    }

    if (channel ==
        AETHER_CHANNEL_REGISTRATION) {

        if (client->rx_active) {
            return AETHER_ERR_BUSY;
        }

        if (client->state ==
            AETHER_STATE_REG_CONNECTING) {

            return AETHER_OK;
        }

        return
            handle_registration_rx(
                client,
                data,
                length);
    }

    if (channel !=
        AETHER_CHANNEL_WORK) {

        return AETHER_ERR_ARGUMENT;
    }

    if (client->ingress.kind !=
        AETHER_INGRESS_NONE) {

        return AETHER_ERR_BUSY;
    }

    if (!client->rx_active) {
        client->rx_frame_data =
            data;

        client->rx_frame_length =
            length;

        client->rx_outer_pos =
            0u;

        client->rx_plain_length =
            0u;

        client->rx_plain_pos =
            0u;

        client->rx_messages_remaining =
            0u;

        client->rx_channel =
            channel;

        client->rx_active =
            true;

        client->rx_plain_active =
            false;

    } else if (
        client->rx_channel != channel ||
        client->rx_frame_data != data ||
        client->rx_frame_length !=
            length) {

        return AETHER_ERR_BUSY;
    }

    aether_status_t status =
        handle_work_rx(
            client);

    if (status != AETHER_OK) {
        clear_rx_state(
            client);

        return
            fail(
                client,
                status);
    }

    return AETHER_OK;
}


const aether_ingress_t *aether_client_ingress(
    const aether_client_t *client) {

    if (client == NULL ||
        client->ingress.kind ==
            AETHER_INGRESS_NONE) {

        return NULL;
    }

    return
        &client->ingress;
}


void aether_client_consume_ingress(
    aether_client_t *client) {

    if (client == NULL ||
        client->ingress.kind ==
            AETHER_INGRESS_NONE) {

        return;
    }

    clear_ingress(
        client);

    /*
     * The yielded item may have been the final object in both:
     *
     *   - the current decrypted ClientApiSafe block;
     *   - the current borrowed transport frame.
     *
     * In that case there is nothing left for a future parser step to do.
     * Finish the frame immediately after the application has consumed the
     * ingress.
     *
     * This is important for component scheduling: business logic regains
     * control after one ingress item and may transmit immediately afterwards.
     * Requiring one artificial extra tick here would keep tx_crypto owned by
     * RX and make an otherwise independent send return AETHER_ERR_BUSY.
     *
     * We do NOT parse any following command here. If another message, control
     * command or encrypted outer block exists, rx_active remains true and the
     * parser resumes on a later tick.
     */
    if (!client->rx_active ||
        !client->rx_plain_active ||
        client->rx_messages_remaining !=
            0u ||
        client->rx_plain_pos !=
            client->rx_plain_length ||
        client->rx_outer_pos !=
            client->rx_frame_length) {

        return;
    }

    clear_rx_state(
        client);
}


bool aether_client_rx_active(
    const aether_client_t *client) {

    return
        client != NULL &&
        client->rx_active;
}


aether_channel_t aether_client_rx_channel(
    const aether_client_t *client) {

    if (client == NULL) {
        return
            AETHER_CHANNEL_WORK;
    }

    return
        client->rx_channel;
}


aether_status_t aether_client_send_message(
    aether_client_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t length,
    uint32_t *request_id_out) {

    if (client == NULL ||
        (data == NULL &&
         length != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }

    if (client->callback_active ||
        client->rx_active) {

        return AETHER_ERR_BUSY;
    }

    if (client->state !=
        AETHER_STATE_READY) {

        return AETHER_ERR_STATE;
    }

    uint32_t request_id =
        next_request_id(
            client);

    message_t message = {
        .uid = destination,
        .data = {
            .data = data,
            .length = length
        }
    };

    authorized_api_remote_t authorized = {
        .send_ctx = client,
        .send = generated_authorized_send,
        .tx = client->tx_plain,
        .tx_capacity =
            sizeof(client->tx_plain)
    };

    aether_status_t status =
        authorized_api_send_message_with_result(
            &authorized,
            request_id,
            &message);

    if (status != AETHER_OK) {
        return status;
    }

    if (request_id_out != NULL) {
        *request_id_out =
            request_id;
    }

    return AETHER_OK;
}


aether_state_t aether_client_state(
    const aether_client_t *client) {

    return
        client == NULL
            ? AETHER_STATE_ERROR
            : client->state;
}


bool aether_client_is_registered(
    const aether_client_t *client) {

    return
        client != NULL &&
        client->registered;
}