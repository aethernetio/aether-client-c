
#include "aether_hydrogen.h"

#include "hydrogen.h"

#include <stdbool.h>
#include <string.h>

static const char AETHER_CRYPTO_CONTEXT[hydro_secretbox_CONTEXTBYTES] = {
    'a', 'e', 't', 'h', 'e', 'r', 'i', 'o'
};

static const char AETHER_KDF_CONTEXT[hydro_kdf_CONTEXTBYTES] = {
    '_', 'a', 'e', 't', 'h', 'e', 'r', '_'
};

static void write_u64le(uint8_t out[8], uint64_t value) {
    for (unsigned i = 0; i < 8u; ++i) {
        out[i] = (uint8_t)(value >> (i * 8u));
    }
}

static uint64_t read_u64le(const uint8_t in[8]) {
    uint64_t value = 0u;
    for (unsigned i = 0; i < 8u; ++i) {
        value |= (uint64_t)in[i] << (i * 8u);
    }
    return value;
}

static uint64_t random_msg_id(void) {
    uint64_t value;
    hydro_random_buf(&value, sizeof(value));
    return value;
}

int aether_hydrogen_init(void) {
    return hydro_init();
}

void aether_hydrogen_random_symmetric_key(
    uint8_t out[AETHER_HYDROGEN_KEY_BYTES]) {
    hydro_secretbox_keygen(out);
}

int aether_hydrogen_symmetric_encrypt(
    const uint8_t key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    if (key == NULL || out == NULL || out_len == NULL ||
        (plain == NULL && plain_len != 0u)) {
        return -1;
    }

    const size_t required =
        AETHER_HYDROGEN_MSG_ID_BYTES +
        hydro_secretbox_HEADERBYTES +
        plain_len;

    if (required < plain_len || out_capacity < required) {
        return -1;
    }

    uint64_t msg_id = random_msg_id();
    write_u64le(out, msg_id);

    if (hydro_secretbox_encrypt(
            out + AETHER_HYDROGEN_MSG_ID_BYTES,
            plain,
            plain_len,
            msg_id,
            AETHER_CRYPTO_CONTEXT,
            key) != 0) {
        return -1;
    }

    *out_len = required;
    return 0;
}

int aether_hydrogen_symmetric_decrypt(
    const uint8_t key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t *cipher,
    size_t cipher_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    if (key == NULL || cipher == NULL || out == NULL || out_len == NULL) {
        return -1;
    }

    const size_t overhead =
        AETHER_HYDROGEN_MSG_ID_BYTES +
        hydro_secretbox_HEADERBYTES;

    if (cipher_len < overhead) {
        return -1;
    }

    const size_t plain_len = cipher_len - overhead;
    if (out_capacity < plain_len) {
        return -1;
    }

    const uint64_t msg_id = read_u64le(cipher);

    if (hydro_secretbox_decrypt(
            out,
            cipher + AETHER_HYDROGEN_MSG_ID_BYTES,
            cipher_len - AETHER_HYDROGEN_MSG_ID_BYTES,
            msg_id,
            AETHER_CRYPTO_CONTEXT,
            key) != 0) {
        return -1;
    }

    *out_len = plain_len;
    return 0;
}

int aether_hydrogen_asymmetric_encrypt(
    const uint8_t public_key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t *plain,
    size_t plain_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    if (public_key == NULL || out == NULL || out_len == NULL ||
        (plain == NULL && plain_len != 0u)) {
        return -1;
    }

    const size_t symmetric_len =
        AETHER_HYDROGEN_MSG_ID_BYTES +
        hydro_secretbox_HEADERBYTES +
        plain_len;

    const size_t required =
        hydro_kx_N_PACKET1BYTES +
        symmetric_len;

    if (required < plain_len || out_capacity < required) {
        return -1;
    }

    hydro_kx_session_keypair session;

    if (hydro_kx_n_1(
            &session,
            out,
            NULL,
            public_key) != 0) {
        hydro_memzero(&session, sizeof(session));
        return -1;
    }

    size_t encrypted_len = 0u;
    int result = aether_hydrogen_symmetric_encrypt(
        session.tx,
        plain,
        plain_len,
        out + hydro_kx_N_PACKET1BYTES,
        out_capacity - hydro_kx_N_PACKET1BYTES,
        &encrypted_len);

    hydro_memzero(&session, sizeof(session));

    if (result != 0) {
        return -1;
    }

    *out_len = hydro_kx_N_PACKET1BYTES + encrypted_len;
    return 0;
}

int aether_hydrogen_asymmetric_decrypt(
    const uint8_t public_key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t private_key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t *cipher,
    size_t cipher_len,
    uint8_t *out,
    size_t out_capacity,
    size_t *out_len) {

    if (public_key == NULL || private_key == NULL ||
        cipher == NULL || out == NULL || out_len == NULL ||
        cipher_len < hydro_kx_N_PACKET1BYTES) {
        return -1;
    }

    hydro_kx_keypair keypair;
    hydro_kx_session_keypair session;

    memcpy(keypair.pk, public_key, sizeof(keypair.pk));
    memcpy(keypair.sk, private_key, sizeof(keypair.sk));

    if (hydro_kx_n_2(
            &session,
            cipher,
            NULL,
            &keypair) != 0) {
        hydro_memzero(&keypair, sizeof(keypair));
        hydro_memzero(&session, sizeof(session));
        return -1;
    }

    int result = aether_hydrogen_symmetric_decrypt(
        session.rx,
        cipher + hydro_kx_N_PACKET1BYTES,
        cipher_len - hydro_kx_N_PACKET1BYTES,
        out,
        out_capacity,
        out_len);

    hydro_memzero(&keypair, sizeof(keypair));
    hydro_memzero(&session, sizeof(session));

    return result;
}

int aether_hydrogen_derive_server_keys(
    const uint8_t master_key[AETHER_HYDROGEN_KEY_BYTES],
    int32_t server_id,
    uint32_t key_number,
    uint8_t client_to_server[AETHER_HYDROGEN_KEY_BYTES],
    uint8_t server_to_client[AETHER_HYDROGEN_KEY_BYTES]) {

    if (master_key == NULL ||
        client_to_server == NULL ||
        server_to_client == NULL) {
        return -1;
    }

    uint8_t derived[hydro_secretbox_KEYBYTES * 2u];

    /*
     * Match Java exactly:
     *   (((long) serverId) << 32) | (keyNumber & 0xffffffffL)
     *
     * Convert the signed 32-bit value to uint64_t before shifting so negative
     * server ids are represented with the same sign-extended high 32 bits
     * without invoking signed-left-shift undefined behaviour in C.
     */
    uint64_t subkey_id =
        ((uint64_t)(int64_t)server_id << 32u) |
        (uint64_t)key_number;

    if (hydro_kdf_derive_from_key(
            derived,
            sizeof(derived),
            subkey_id,
            AETHER_KDF_CONTEXT,
            master_key) != 0) {
        hydro_memzero(derived, sizeof(derived));
        return -1;
    }

    memcpy(client_to_server, derived, AETHER_HYDROGEN_KEY_BYTES);
    memcpy(
        server_to_client,
        derived + AETHER_HYDROGEN_KEY_BYTES,
        AETHER_HYDROGEN_KEY_BYTES);

    hydro_memzero(derived, sizeof(derived));
    return 0;
}

int aether_hydrogen_verify_signature(
    const uint8_t public_key[AETHER_HYDROGEN_KEY_BYTES],
    const uint8_t signature[AETHER_HYDROGEN_SIGNATURE_BYTES],
    const uint8_t *data,
    size_t data_len) {

    if (public_key == NULL || signature == NULL ||
        (data == NULL && data_len != 0u)) {
        return -1;
    }

    return hydro_sign_verify(
               signature,
               data,
               data_len,
               AETHER_CRYPTO_CONTEXT,
               public_key) == 0
        ? 0
        : -1;
}
