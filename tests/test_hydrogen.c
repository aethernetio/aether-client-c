
#include "aether_hydrogen.h"
#include "hydrogen.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_symmetric_round_trip(void) {
    uint8_t key[AETHER_HYDROGEN_KEY_BYTES];
    uint8_t cipher[256];
    uint8_t plain[256];
    size_t cipher_len = 0u;
    size_t plain_len = 0u;

    static const uint8_t message[] = {
        'a', 'e', 't', 'h', 'e', 'r', '-', 'h', 'y', 'd', 'r', 'o', 'g', 'e', 'n'
    };

    aether_hydrogen_random_symmetric_key(key);

    assert(aether_hydrogen_symmetric_encrypt(
        key,
        message,
        sizeof(message),
        cipher,
        sizeof(cipher),
        &cipher_len) == 0);

    assert(cipher_len ==
        sizeof(message) +
        AETHER_HYDROGEN_MSG_ID_BYTES +
        AETHER_HYDROGEN_SECRETBOX_HEADER_BYTES);

    assert(aether_hydrogen_symmetric_decrypt(
        key,
        cipher,
        cipher_len,
        plain,
        sizeof(plain),
        &plain_len) == 0);

    assert(plain_len == sizeof(message));
    assert(memcmp(plain, message, sizeof(message)) == 0);

    cipher[cipher_len - 1u] ^= 1u;
    assert(aether_hydrogen_symmetric_decrypt(
        key,
        cipher,
        cipher_len,
        plain,
        sizeof(plain),
        &plain_len) != 0);
}

static void test_asymmetric_round_trip(void) {
    hydro_kx_keypair server;
    uint8_t cipher[256];
    uint8_t plain[256];
    size_t cipher_len = 0u;
    size_t plain_len = 0u;

    static const uint8_t message[] = {
        'r', 'e', 'g', 'i', 's', 't', 'r', 'a', 't', 'i', 'o', 'n'
    };

    hydro_kx_keygen(&server);

    assert(aether_hydrogen_asymmetric_encrypt(
        server.pk,
        message,
        sizeof(message),
        cipher,
        sizeof(cipher),
        &cipher_len) == 0);

    assert(cipher_len ==
        hydro_kx_N_PACKET1BYTES +
        AETHER_HYDROGEN_MSG_ID_BYTES +
        AETHER_HYDROGEN_SECRETBOX_HEADER_BYTES +
        sizeof(message));

    assert(aether_hydrogen_asymmetric_decrypt(
        server.pk,
        server.sk,
        cipher,
        cipher_len,
        plain,
        sizeof(plain),
        &plain_len) == 0);

    assert(plain_len == sizeof(message));
    assert(memcmp(plain, message, sizeof(message)) == 0);
}

static void test_kdf(void) {
    uint8_t master[AETHER_HYDROGEN_KEY_BYTES];
    uint8_t tx1[AETHER_HYDROGEN_KEY_BYTES];
    uint8_t rx1[AETHER_HYDROGEN_KEY_BYTES];
    uint8_t tx2[AETHER_HYDROGEN_KEY_BYTES];
    uint8_t rx2[AETHER_HYDROGEN_KEY_BYTES];

    aether_hydrogen_random_symmetric_key(master);

    assert(aether_hydrogen_derive_server_keys(
        master, 7, 0u, tx1, rx1) == 0);
    assert(aether_hydrogen_derive_server_keys(
        master, 7, 0u, tx2, rx2) == 0);

    assert(memcmp(tx1, tx2, sizeof(tx1)) == 0);
    assert(memcmp(rx1, rx2, sizeof(rx1)) == 0);
    assert(memcmp(tx1, rx1, sizeof(tx1)) != 0);

    assert(aether_hydrogen_derive_server_keys(
        master, 8, 0u, tx2, rx2) == 0);
    assert(memcmp(tx1, tx2, sizeof(tx1)) != 0);
}

static void test_signature(void) {
    hydro_sign_keypair signer;
    uint8_t signature[hydro_sign_BYTES];

    static const uint8_t data[] = {
        0x01u, 0x02u, 0x03u, 0x04u, 0x05u
    };
    static const uint8_t other[] = {
        0x01u, 0x02u, 0x03u, 0x04u, 0x06u
    };
    static const char ctx[hydro_sign_CONTEXTBYTES] = {
        'a', 'e', 't', 'h', 'e', 'r', 'i', 'o'
    };

    hydro_sign_keygen(&signer);

    assert(hydro_sign_create(
        signature,
        data,
        sizeof(data),
        ctx,
        signer.sk) == 0);

    assert(aether_hydrogen_verify_signature(
        signer.pk,
        signature,
        data,
        sizeof(data)) == 0);

    assert(aether_hydrogen_verify_signature(
        signer.pk,
        signature,
        other,
        sizeof(other)) != 0);
}

int main(void) {
    assert(aether_hydrogen_init() == 0);

    test_symmetric_round_trip();
    test_asymmetric_round_trip();
    test_kdf();
    test_signature();

    puts("aether_hydrogen_test: OK");
    return 0;
}
