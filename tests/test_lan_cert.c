
/*
 * Certificate-chain regression test with real ECDSA P-256.
 *
 * Uses OpenSSL to:
 *   - generate test root and authority P-256 keypairs;
 *   - sign authority cert with root key;
 *   - sign peer cert with authority key;
 * and drives aether_lan_verify_authority_cert / aether_lan_verify_peer_cert
 * through a crypto vtable whose verify_signature matches the ESP32 PSA
 * semantics: SHA-256 of data, ECDSA over the digest, raw 64-byte r||s
 * signature, 64-byte raw X||Y public key.
 */
#include "aether.h"
#include "aether_lan.h"
#include "aether_lan_cert.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include <openssl/ecdsa.h>
#include <openssl/bn.h>

#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/obj_mac.h>

/* ---------- OpenSSL helpers ---------- */

static EVP_PKEY *gen_p256(void) {
    EVP_PKEY *k = EVP_PKEY_Q_keygen(NULL, NULL, "EC", "P-256");
    assert(k != NULL);
    return k;
}

static void pkey_raw_pub(const EVP_PKEY *k, uint8_t out64[64]) {
    uint8_t buf[133];
    size_t len = 0;
    assert(EVP_PKEY_get_octet_string_param(
        k, OSSL_PKEY_PARAM_PUB_KEY, buf, sizeof(buf), &len) == 1);
    /* buf[0] == 0x04 prefix, then 64 bytes X||Y */
    assert(len == 65u && buf[0] == 0x04u);
    memcpy(out64, buf + 1, 64u);
}

/* Sign `data` with ECDSA-SHA256. Returns raw r||s 64 bytes. */
static void sign_raw(
    EVP_PKEY *k, const uint8_t *data, size_t data_len,
    uint8_t out64[64]) {

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    assert(ctx != NULL);
    assert(EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, k) == 1);

    size_t der_len = 0u;
    assert(EVP_DigestSign(ctx, NULL, &der_len, data, data_len) == 1);
    uint8_t der[128];
    assert(der_len <= sizeof(der));
    assert(EVP_DigestSign(ctx, der, &der_len, data, data_len) == 1);

    const uint8_t *der_p = der;

    EVP_MD_CTX_free(ctx);


    ECDSA_SIG *sig = d2i_ECDSA_SIG(NULL, &der_p, (long)der_len);
    assert(sig != NULL);
    const BIGNUM *r = NULL;
    const BIGNUM *s = NULL;
    ECDSA_SIG_get0(sig, &r, &s);
    assert(r != NULL && s != NULL);
    assert(BN_bn2binpad(r, out64, 32) == 32);
    assert(BN_bn2binpad(s, out64 + 32, 32) == 32);
    ECDSA_SIG_free(sig);

}

/* Verify raw r||s 64-byte signature over SHA-256(data). */
static bool verify_raw(
    const uint8_t pub64[64],
    const uint8_t sig64[64],
    const uint8_t *data, size_t data_len) {

    /* Wrap pub64 into 65-byte uncompressed form. */
    uint8_t unc[65];
    unc[0] = 0x04u;
    memcpy(unc + 1, pub64, 64u);

    OSSL_PARAM params[3];
    params[0] = OSSL_PARAM_construct_utf8_string(
        OSSL_PKEY_PARAM_GROUP_NAME, "P-256", 0);
    params[1] = OSSL_PARAM_construct_octet_string(
        OSSL_PKEY_PARAM_PUB_KEY, unc, sizeof(unc));
    params[2] = OSSL_PARAM_construct_end();

    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    assert(pctx != NULL);
    assert(EVP_PKEY_fromdata_init(pctx) == 1);
    EVP_PKEY *pk = NULL;
    assert(EVP_PKEY_fromdata(pctx, &pk, EVP_PKEY_PUBLIC_KEY, params) == 1);
    EVP_PKEY_CTX_free(pctx);

    /* Raw r||s -> DER ECDSA-Sig-Value. */
    uint8_t der[80];
    size_t der_len = 0u;
    {
        size_t p = 0u;
        der[p++] = 0x30u; /* SEQUENCE */
        size_t seq_len_pos = p++;
        /* r INTEGER */
        der[p++] = 0x02u;
        const uint8_t *r = sig64;
        size_t rl = 32u;
        while (rl > 0 && r[0] == 0u && rl > 1u) { r += 1; rl -= 1; }
        size_t r_enc = rl;
        if (r[0] & 0x80u) r_enc += 1u;
        der[p++] = (uint8_t)r_enc;
        if (r[0] & 0x80u) der[p++] = 0x00u;
        memcpy(der + p, r, rl); p += rl;
        /* s INTEGER */
        der[p++] = 0x02u;
        const uint8_t *s = sig64 + 32;
        size_t sl = 32u;
        while (sl > 0 && s[0] == 0u && sl > 1u) { s += 1; sl -= 1; }
        size_t s_enc = sl;
        if (s[0] & 0x80u) s_enc += 1u;
        der[p++] = (uint8_t)s_enc;
        if (s[0] & 0x80u) der[p++] = 0x00u;
        memcpy(der + p, s, sl); p += sl;
        der[seq_len_pos] = (uint8_t)(p - seq_len_pos - 1u);
        der_len = p;
    }

    EVP_MD_CTX *mctx = EVP_MD_CTX_new();
    assert(mctx != NULL);
    assert(EVP_DigestVerifyInit(mctx, NULL, EVP_sha256(), NULL, pk) == 1);
    int ok = EVP_DigestVerify(mctx, der, der_len, data, data_len);
    EVP_MD_CTX_free(mctx);
    EVP_PKEY_free(pk);
    return ok == 1;
}

/* ---------- crypto vtable ---------- */

static aether_status_t crypto_init(void *c) { (void)c; return AETHER_OK; }
static aether_status_t crypto_rand(void *c, uint8_t *o, size_t n) {
    (void)c; memset(o, 0x11, n); return AETHER_OK;
}

static aether_status_t crypto_verify(
    void *c, const uint8_t *pk, size_t pkl,
    const uint8_t *sig, size_t sigl,
    const uint8_t *data, size_t dl) {
    (void)c;
    if (pkl != 64u || sigl != 64u) return AETHER_ERR_ARGUMENT;
    return verify_raw(pk, sig, data, dl) ? AETHER_OK : AETHER_ERR_CRYPTO;
}

static aether_status_t crypto_asym(
    void *c, const uint8_t *pk, size_t pkl,
    const uint8_t *pl, size_t pll,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)pk; (void)pkl;
    if (oc < pll) return AETHER_ERR_OVERFLOW;
    memcpy(o, pl, pll); *ol = pll; return AETHER_OK;
}
static aether_status_t crypto_se(
    void *c, const uint8_t *k, size_t kl,
    const uint8_t *pl, size_t pll,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)k; (void)kl;
    if (oc < pll) return AETHER_ERR_OVERFLOW;
    memcpy(o, pl, pll); *ol = pll; return AETHER_OK;
}
static aether_status_t crypto_sd(
    void *c, const uint8_t *k, size_t kl,
    const uint8_t *ci, size_t cl,
    uint8_t *o, size_t oc, size_t *ol) {
    (void)c; (void)k; (void)kl;
    if (oc < cl) return AETHER_ERR_OVERFLOW;
    memcpy(o, ci, cl); *ol = cl; return AETHER_OK;
}
static aether_status_t crypto_derive(
    void *c, const uint8_t *mk, size_t mkl,
    int32_t sid, uint32_t kn,
    uint8_t *a, uint8_t *b, size_t dl) {
    (void)c; (void)mk; (void)mkl; (void)sid; (void)kn;
    memset(a, 0, dl); memset(b, 0, dl); return AETHER_OK;
}

static const aether_crypto_vtable_t vt = {
    .profile = { 2,11,13,3, 32,64,64,64 },
    .initialize = crypto_init,
    .random_symmetric_key = crypto_rand,
    .verify_signature = crypto_verify,
    .asymmetric_encrypt = crypto_asym,
    .symmetric_encrypt = crypto_se,
    .symmetric_decrypt = crypto_sd,
    .derive_server_keys = crypto_derive
};

/* ---------- platform stub ---------- */

static uint8_t trusted_root[64];
static aether_t client;

static uint64_t fake_now(void *c) { (void)c; return 0u; }
static aether_status_t fake_resolve(
    void *c, const char *h, aether_address_t *a) {
    (void)c; (void)h;
    memset(a, 0, sizeof(*a));
    a->kind = AETHER_ADDR_IPV4; a->length = 4u;
    a->bytes[0] = 127; a->bytes[3] = 1;
    return AETHER_OK;
}
static aether_status_t f_open(
    void *c, aether_channel_t ch, const aether_endpoint_t *e) {
    (void)c; (void)ch; (void)e; return AETHER_OK;
}
static void f_close(void *c, aether_channel_t ch) { (void)c; (void)ch; }
static aether_status_t f_send(
    void *c, aether_channel_t ch, const uint8_t *d, size_t n) {
    (void)c; (void)ch; (void)d; (void)n; return AETHER_OK;
}
static aether_status_t f_poll(void *c) { (void)c; return AETHER_OK; }
static aether_status_t f_recv(
    void *c, aether_channel_t ch, aether_bytes_view_t *f) {
    (void)c; (void)ch;
    if (f) { f->data = NULL; f->length = 0u; }
    return AETHER_OK;
}
static void f_consume(
    void *c, aether_channel_t ch, const uint8_t *d, size_t n) {
    (void)c; (void)ch; (void)d; (void)n;
}
static aether_status_t f_fload(
    void *c, uint8_t *d, size_t cap, size_t *len) {
    (void)c; (void)d; (void)cap;
    if (len) *len = 0u;
    return AETHER_ERR_STORAGE;
}
static aether_status_t f_fsave(
    void *c, const uint8_t *s, size_t n) {
    (void)c; (void)s; (void)n; return AETHER_OK;
}
static aether_status_t f_pow(
    void *c, const uint8_t *sa, size_t sl,
    const uint8_t *su, size_t sul,
    uint8_t p, int32_t mh, int32_t *ws, size_t cap, size_t *ct) {
    (void)c; (void)sa; (void)sl; (void)su; (void)sul;
    (void)p; (void)mh; (void)ws; (void)cap;
    if (ct) *ct = 0u;
    return AETHER_OK;
}

aether_status_t aether_platform_init(
    void *ctx, size_t sz, uint32_t slot,
    aether_client_platform_t *p) {
    (void)ctx; (void)sz; (void)slot;
    if (p == NULL) return AETHER_ERR_ARGUMENT;
    memset(p, 0, sizeof(*p));
    p->ctx = (void *)1;
    p->now_ms = fake_now;
    p->resolve = fake_resolve;
    p->poll = f_poll;
    p->receive = f_recv;
    p->consume = f_consume;
    p->transport = (aether_transport_vtable_t){
        (void *)1, f_open, f_close, f_send };
    p->flash = (aether_flash_vtable_t){
        (void *)1, f_fload, f_fsave };
    p->crypto = &vt;
    p->trusted_sign_keys = trusted_root;
    p->trusted_sign_key_count = 1u;
    p->pow = (aether_pow_vtable_t){ (void *)1, f_pow };
    return AETHER_OK;
}
void aether_platform_deinit(void *ctx) { (void)ctx; }

/* ---------- cert builders ---------- */

static const uint8_t AUTH_PREFIX[8] = { 'A','E','T','H','E','R','I','A' };
static const uint8_t PEER_PREFIX[8] = { 'A','E','T','H','E','R','P','E' };

static void build_authority_cert(
    EVP_PKEY *root, EVP_PKEY *auth, uint8_t purpose, uint8_t out[134]) {

    memset(out, 0, 134);
    out[0] = 0x01u;
    out[1] = purpose;
    out[2] = 0; out[3] = 0; out[4] = 0; out[5] = 1;
    pkey_raw_pub(auth, out + 6);

    uint8_t input[8u + 70u];
    memcpy(input, AUTH_PREFIX, 8u);
    memcpy(input + 8u, out, 70u);

    uint8_t sig[64];
    sign_raw(root, input, sizeof(input), sig);
    memcpy(out + 70u, sig, 64u);
}

static void build_peer_cert(
    EVP_PKEY *auth, aether_uuid_t uid,
    const uint8_t sign_pk[32], uint8_t out[113]) {

    memset(out, 0, 113);
    out[0] = 0x01u;
    for (unsigned i = 0; i < 8u; ++i) {
        out[1 + i] = (uint8_t)(uid.msb >> (56u - 8u*i));
        out[9 + i] = (uint8_t)(uid.lsb >> (56u - 8u*i));
    }
    memcpy(out + 17, sign_pk, 32);

    uint8_t input[8u + 49u];
    memcpy(input, PEER_PREFIX, 8u);
    memcpy(input + 8u, out, 49u);

    uint8_t sig[64];
    sign_raw(auth, input, sizeof(input), sig);
    memcpy(out + 49, sig, 64u);
}

int main(void) {
    EVP_PKEY *root = gen_p256();
    EVP_PKEY *auth = gen_p256();
    EVP_PKEY *rogue_root = gen_p256();
    EVP_PKEY *rogue_auth = gen_p256();

    uint8_t root_pub[64];
    pkey_raw_pub(root, root_pub);
    memcpy(trusted_root, root_pub, 64u);

    assert(aether_init(&client, (aether_uuid_t){ 0, 0 }) == AETHER_OK);

    aether_uuid_t uid = { 0xCAFEBABE12345678ull, 0x1122334455667788ull };
    uint8_t peer_sign_pk[32];
    for (unsigned i = 0; i < 32u; ++i) peer_sign_pk[i] = (uint8_t)(0x40u + i);

    uint8_t auth_cert[134];
    uint8_t peer_cert[113];
    build_authority_cert(root, auth, 0x01u, auth_cert);
    build_peer_cert(auth, uid, peer_sign_pk, peer_cert);

    /* 1. Positive */
    assert(aether_lan_verify_authority_cert(&client, auth_cert));

    /* 2. Positive peer cert, sign_pk returned */
    uint8_t out_pk[32];
    assert(aether_lan_verify_peer_cert(&client, auth_cert, uid, peer_cert, out_pk));
    assert(memcmp(out_pk, peer_sign_pk, 32u) == 0);

    /* 3. Negative: authority signed by rogue root */
    uint8_t bad_auth[134];
    build_authority_cert(rogue_root, auth, 0x01u, bad_auth);
    assert(!aether_lan_verify_authority_cert(&client, bad_auth));

    /* 4. Negative: wrong purpose */
    uint8_t wrong_purpose[134];
    build_authority_cert(root, auth, 0x02u, wrong_purpose);
    assert(!aether_lan_verify_authority_cert(&client, wrong_purpose));

    /* 5. Negative: authority pk tampered */
    uint8_t tampered_auth[134];
    memcpy(tampered_auth, auth_cert, 134);
    tampered_auth[10] ^= 0x01u;
    assert(!aether_lan_verify_authority_cert(&client, tampered_auth));

    /* 6. Negative: peer cert from rogue authority */
    uint8_t bad_peer[113];
    build_peer_cert(rogue_auth, uid, peer_sign_pk, bad_peer);
    assert(!aether_lan_verify_peer_cert(&client, auth_cert, uid, bad_peer, out_pk));

    /* 7. Negative: peer cert UID mismatch */
    aether_uuid_t other = { 0x1111u, 0x2222u };
    assert(!aether_lan_verify_peer_cert(&client, auth_cert, other, peer_cert, out_pk));

    /* 8. Negative: peer sign_pk tampered */
    uint8_t tampered_peer[113];
    memcpy(tampered_peer, peer_cert, 113);
    tampered_peer[20] ^= 0x01u;
    assert(!aether_lan_verify_peer_cert(&client, auth_cert, uid, tampered_peer, out_pk));

    /* 9. Negative: authority signature over peer tampered */
    uint8_t tampered_sig[113];
    memcpy(tampered_sig, peer_cert, 113);
    tampered_sig[60] ^= 0x01u;
    assert(!aether_lan_verify_peer_cert(&client, auth_cert, uid, tampered_sig, out_pk));

    EVP_PKEY_free(rogue_auth);
    EVP_PKEY_free(rogue_root);
    EVP_PKEY_free(auth);
    EVP_PKEY_free(root);

    printf("LAN cert chain: OK\n");
    return 0;
}