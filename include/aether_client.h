
#ifndef AETHER_CLIENT_H
#define AETHER_CLIENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef AETHER_MAX_PACKET
#define AETHER_MAX_PACKET 1536u
#endif

#ifndef AETHER_MAX_SERVERS
#define AETHER_MAX_SERVERS 8u
#endif

#ifndef AETHER_MAX_ADDRESS_BYTES
#define AETHER_MAX_ADDRESS_BYTES 16u
#endif

#ifndef AETHER_MAX_POW_TEXT
#define AETHER_MAX_POW_TEXT 64u
#endif

#ifndef AETHER_MAX_POW_PASSWORDS
#define AETHER_MAX_POW_PASSWORDS 32u
#endif




#define AETHER_SYMMETRIC_KEY_BYTES 32u
#define AETHER_KEY_BYTES AETHER_SYMMETRIC_KEY_BYTES

#ifndef AETHER_MAX_CRYPTO_PUBLIC_KEY_BYTES
#define AETHER_MAX_CRYPTO_PUBLIC_KEY_BYTES 64u
#endif

#ifndef AETHER_MAX_CRYPTO_SIGNATURE_BYTES
#define AETHER_MAX_CRYPTO_SIGNATURE_BYTES 64u
#endif

#define AETHER_SIGNATURE_BYTES AETHER_MAX_CRYPTO_SIGNATURE_BYTES
#define AETHER_UUID_BYTES 16u


typedef enum {
    AETHER_CODEC_TCP = 0,
    AETHER_CODEC_UDP = 1
} aether_codec_t;


typedef enum {
    AETHER_ADDR_IPV4 = 1,
    AETHER_ADDR_IPV6 = 2
} aether_address_kind_t;


typedef enum {
    AETHER_CHANNEL_REGISTRATION = 0,
    AETHER_CHANNEL_WORK = 1
} aether_channel_t;


typedef enum {
    AETHER_STATE_STOPPED = 0,
    AETHER_STATE_REG_CONNECTING,
    AETHER_STATE_REG_WAIT_SERVER_KEY,
    AETHER_STATE_REG_WAIT_POW,
    AETHER_STATE_REG_WAIT_FINISH,
    AETHER_STATE_REG_WAIT_SERVERS,
    AETHER_STATE_WORK_CONNECTING,
    AETHER_STATE_READY,
    AETHER_STATE_ERROR
} aether_state_t;



typedef enum {
    AETHER_OK = 0,
    AETHER_ERR_ARGUMENT = -1,
    AETHER_ERR_STATE = -2,
    AETHER_ERR_OVERFLOW = -3,
    AETHER_ERR_PROTOCOL = -4,
    AETHER_ERR_CRYPTO = -5,
    AETHER_ERR_TRANSPORT = -6,
    AETHER_ERR_STORAGE = -7,
    AETHER_ERR_UNSUPPORTED = -8,
    AETHER_ERR_BUSY = -9,
    AETHER_ERR_REMOTE = -10,
    AETHER_ERR_TIMEOUT = -11
} aether_status_t;



typedef enum {
    AETHER_ERROR_ORIGIN_CORE = 0,
    AETHER_ERROR_ORIGIN_PROTOCOL,
    AETHER_ERROR_ORIGIN_CRYPTO,
    AETHER_ERROR_ORIGIN_TRUST,
    AETHER_ERROR_ORIGIN_TRANSPORT,
    AETHER_ERROR_ORIGIN_STORAGE,
    AETHER_ERROR_ORIGIN_POW,
    AETHER_ERROR_ORIGIN_REMOTE
} aether_error_origin_t;


typedef struct {
    uint64_t msb;
    uint64_t lsb;
} aether_uuid_t;


typedef struct {
    aether_address_kind_t kind;
    uint8_t length;
    uint8_t bytes[AETHER_MAX_ADDRESS_BYTES];
} aether_address_t;


typedef struct {
    aether_codec_t codec;
    aether_address_t address;
    uint16_t port;
} aether_endpoint_t;


typedef struct {
    int16_t sid;
    bool valid;
    aether_endpoint_t endpoint;
} aether_server_t;


typedef struct {
    uint8_t key_type;
    uint8_t key[AETHER_MAX_CRYPTO_PUBLIC_KEY_BYTES];
    uint8_t sign_type;
    uint8_t signature[AETHER_MAX_CRYPTO_SIGNATURE_BYTES];
} aether_signed_key_t;


typedef struct {
    const uint8_t *data;
    size_t length;
} aether_bytes_view_t;



/*
 * Legacy core events.
 *
 * State/error events remain the compatibility callback path.
 * MESSAGE/REQUEST_DONE/REQUEST_TIMEOUT enum values stay defined so old source
 * can still compile, but application work items are no longer pushed through
 * on_event.
 *
 * Messages and work request results are exposed through
 * aether_client_ingress().
 */

typedef enum {
    AETHER_EVENT_STATE = 0,
    AETHER_EVENT_REGISTERED,
    AETHER_EVENT_MESSAGE,
    AETHER_EVENT_REQUEST_DONE,
    AETHER_EVENT_ERROR,
    AETHER_EVENT_REQUEST_TIMEOUT
} aether_event_type_t;


typedef struct {
    aether_event_type_t type;

    union {
        struct {
            aether_state_t state;
        } state;

        struct {
            aether_uuid_t uid;
            aether_uuid_t alias;
        } registered;

        struct {
            aether_uuid_t from;
            aether_bytes_view_t payload;
        } message;

        struct {
            uint32_t request_id;
        } request_done;

        struct {
            aether_status_t code;
            aether_error_origin_t origin;
            aether_state_t state;
            uint32_t request_id;
        } error;

        struct {
            uint32_t request_id;
        } request_timeout;
    } as;
} aether_event_t;



/*
 * One application-visible item yielded by the protocol parser.
 *
 * MESSAGE payload memory is borrowed from aether_client_t and remains valid
 * until aether_client_consume_ingress().
 *
 * REQUEST_RESULT contains only value fields and follows the same bounded
 * one-ingress scheduling rule.
 *
 * No queue is allocated. The parser stops after one application item.
 */
typedef enum {
    AETHER_INGRESS_NONE = 0,
    AETHER_INGRESS_MESSAGE,
    AETHER_INGRESS_REQUEST_RESULT
} aether_ingress_kind_t;


typedef struct {
    aether_ingress_kind_t kind;

    union {
        struct {
            aether_uuid_t from;
            aether_bytes_view_t payload;
        } message;

        struct {
            uint32_t request_id;
            aether_status_t status;
        } request_result;
    } as;
} aether_ingress_t;



typedef struct {
    void *ctx;

    /*
     * open() starts platform transport asynchronously.
     * The platform reports writability through
     * aether_client_on_transport_state().
     */
    aether_status_t (*open)(
        void *ctx,
        aether_channel_t channel,
        const aether_endpoint_t *endpoint);

    void (*close)(
        void *ctx,
        aether_channel_t channel);

    /*
     * send() must consume or copy data before returning.
     * The core immediately reuses its TX buffers.
     */
    aether_status_t (*send)(
        void *ctx,
        aether_channel_t channel,
        const uint8_t *data,
        size_t length);
} aether_transport_vtable_t;


typedef struct {
    void *ctx;

    aether_status_t (*load)(
        void *ctx,
        uint8_t *dst,
        size_t capacity,
        size_t *length);

    aether_status_t (*save)(
        void *ctx,
        const uint8_t *src,
        size_t length);
} aether_flash_vtable_t;


/*
 * Wire profile implemented by a crypto backend.
 */
typedef struct {
    uint8_t crypto_lib;
    uint8_t symmetric_key_type;
    uint8_t asymmetric_public_key_type;
    uint8_t sign_type;

    uint8_t symmetric_key_bytes;
    uint8_t asymmetric_public_key_bytes;
    uint8_t sign_public_key_bytes;
    uint8_t signature_bytes;
} aether_crypto_profile_t;


typedef struct {
    aether_crypto_profile_t profile;

    aether_status_t (*initialize)(
        void *ctx);

    aether_status_t (*random_symmetric_key)(
        void *ctx,
        uint8_t *out_key,
        size_t key_len);

    aether_status_t (*verify_signature)(
        void *ctx,
        const uint8_t *public_key,
        size_t public_key_len,
        const uint8_t *signature,
        size_t signature_len,
        const uint8_t *data,
        size_t data_len);

    aether_status_t (*asymmetric_encrypt)(
        void *ctx,
        const uint8_t *public_key,
        size_t public_key_len,
        const uint8_t *plain,
        size_t plain_len,
        uint8_t *out,
        size_t out_capacity,
        size_t *out_len);

    aether_status_t (*symmetric_encrypt)(
        void *ctx,
        const uint8_t *key,
        size_t key_len,
        const uint8_t *plain,
        size_t plain_len,
        uint8_t *out,
        size_t out_capacity,
        size_t *out_len);

    aether_status_t (*symmetric_decrypt)(
        void *ctx,
        const uint8_t *key,
        size_t key_len,
        const uint8_t *cipher,
        size_t cipher_len,
        uint8_t *out,
        size_t out_capacity,
        size_t *out_len);

    aether_status_t (*derive_server_keys)(
        void *ctx,
        const uint8_t *master_key,
        size_t master_key_len,
        int32_t server_id,
        uint32_t key_number,
        uint8_t *client_to_server,
        uint8_t *server_to_client,
        size_t derived_key_len);
} aether_crypto_vtable_t;


typedef struct {
    void *ctx;

    aether_status_t (*generate)(
        void *ctx,
        const uint8_t *salt,
        size_t salt_len,
        const uint8_t *suffix,
        size_t suffix_len,
        uint8_t pool_size,
        int32_t max_hash_value,
        int32_t *passwords,
        size_t capacity,
        size_t *count);
} aether_pow_vtable_t;


typedef void (*aether_event_callback_t)(
    void *ctx,
    const aether_event_t *event);


typedef struct {
    aether_uuid_t parent_uid;
    aether_endpoint_t registration_endpoint;

    uint32_t ping_interval_ms;
    uint32_t rx_window_ms;
    uint32_t request_timeout_ms;

    aether_transport_vtable_t transport;
    aether_flash_vtable_t flash;

    const aether_crypto_vtable_t *crypto;
    void *crypto_ctx;

    aether_pow_vtable_t pow;

    const uint8_t *trusted_sign_keys;
    size_t trusted_sign_key_count;

    /*
     * Legacy state/error/request compatibility callback.
     * Messages are not delivered through this callback anymore.
     */
    aether_event_callback_t on_event;
    void *event_ctx;
} aether_client_config_t;





typedef struct aether_client {
    aether_client_config_t config;

    aether_state_t state;
    bool callback_active;

    uint32_t next_request_id;

    uint64_t now_ms;
    uint64_t next_ping_at_ms;
    uint32_t state_started_at_ms;
    bool time_initialized;

    bool registered;
    aether_uuid_t uid;
    aether_uuid_t alias;

    uint8_t master_key[AETHER_KEY_BYTES];
    uint8_t temp_key[AETHER_KEY_BYTES];
    uint8_t work_tx_key[AETHER_KEY_BYTES];
    uint8_t work_rx_key[AETHER_KEY_BYTES];

    aether_signed_key_t registration_server_key;
    aether_signed_key_t global_key;

    uint8_t cloud_count;
    int16_t cloud_sids[AETHER_MAX_SERVERS];

    uint8_t server_count;
    aether_server_t servers[AETHER_MAX_SERVERS];
    int8_t active_server_index;

    uint32_t req_server_key;
    uint32_t req_pow;
    uint32_t req_finish;
    uint32_t req_resolve;

    uint8_t pow_salt[AETHER_MAX_POW_TEXT];
    size_t pow_salt_len;

    uint8_t pow_suffix[AETHER_MAX_POW_TEXT];
    size_t pow_suffix_len;

    uint8_t pow_pool_size;
    int32_t pow_max_hash;

    int32_t pow_passwords[AETHER_MAX_POW_PASSWORDS];
    size_t pow_password_count;



    /*
     * Mandatory parser mechanism.
     *
     * rx_frame_* references a borrowed frame owned by the platform transport.
     * The platform must keep that frame alive while rx_active is true.
     *
     * tx_crypto owns the currently decrypted safe payload while rx_plain_active
     * or ingress.kind != NONE.
     */
    const uint8_t *rx_frame_data;
    size_t rx_frame_length;
    size_t rx_outer_pos;

    size_t rx_plain_length;
    size_t rx_plain_pos;

    uint64_t rx_messages_remaining;

    aether_channel_t rx_channel;

    bool rx_active;
    bool rx_plain_active;

    aether_ingress_t ingress;

    /*
     * Caller-visible object owns all runtime RAM.
     * No malloc/free are used.
     */
    uint8_t tx_plain[AETHER_MAX_PACKET];
    uint8_t tx_crypto[AETHER_MAX_PACKET];
} aether_client_t;


void aether_client_init(
    aether_client_t *client,
    const aether_client_config_t *config);

aether_status_t aether_client_start(
    aether_client_t *client);

aether_status_t aether_client_retry(
    aether_client_t *client);

void aether_client_stop(
    aether_client_t *client);

void aether_client_poll(
    aether_client_t *client,
    uint64_t now_ms);

void aether_client_on_transport_state(
    aether_client_t *client,
    aether_channel_t channel,
    bool writable);


/*
 * Process/resume one borrowed transport frame.
 *
 * For work traffic this function may stop after producing one ingress item.
 * In that case aether_client_rx_active() stays true and the caller must NOT
 * release or mutate data.
 *
 * After the ingress has been consumed, call this function again with the same
 * channel/data/length to resume parsing.
 *
 * The frame is completely consumed only when aether_client_rx_active() becomes
 * false.
 */
aether_status_t aether_client_on_rx(
    aether_client_t *client,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length);


/*
 * Inspect/consume the single yielded application item.
 */
const aether_ingress_t *aether_client_ingress(
    const aether_client_t *client);

void aether_client_consume_ingress(
    aether_client_t *client);


/*
 * True while a borrowed work frame remains pinned by the parser.
 */
bool aether_client_rx_active(
    const aether_client_t *client);

aether_channel_t aether_client_rx_channel(
    const aether_client_t *client);


aether_status_t aether_client_send_message(
    aether_client_t *client,
    aether_uuid_t destination,
    const uint8_t *data,
    size_t length,
    uint32_t *request_id_out);

aether_state_t aether_client_state(
    const aether_client_t *client);

bool aether_client_is_registered(
    const aether_client_t *client);


#ifdef __cplusplus
}
#endif

#endif