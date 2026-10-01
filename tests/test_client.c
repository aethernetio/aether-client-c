
#include "aether_credentials.h"

#include "aether_client.h"
#include "aether_registration.h"

#include "aether_hydrogen.h"
#include "aether_test_crypto_hydrogen.h"
#include "hydrogen.h"




#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    uint8_t packet[4096];
    size_t packet_len;
    aether_channel_t packet_channel;
    unsigned sends;
    unsigned opens;
    unsigned closes;
    aether_channel_t last_open_channel;
    uint16_t fail_open_port;
    unsigned fail_sends_remaining;
    aether_endpoint_t last_endpoint;

    uint8_t flash[4096];
    size_t flash_len;
    unsigned fail_flash_saves;

    unsigned messages;
    aether_uuid_t last_from;
    uint8_t last_message[128];
    size_t last_message_len;
    unsigned registered_events;
    unsigned done_events;
    unsigned errors;

    aether_status_t last_error_code;
    aether_error_origin_t last_error_origin;
    aether_state_t last_error_state;
    uint32_t last_error_request_id;


    hydro_sign_keypair root_signer;
    hydro_kx_keypair registration_kx;
    hydro_kx_keypair global_kx;


    unsigned timeout_events;
    uint32_t last_timeout_request;

} fixture_t;

static void put8(uint8_t *b, size_t *p, uint8_t v) { b[(*p)++] = v; }
static void put16(uint8_t *b, size_t *p, uint16_t v) {
    put8(b,p,(uint8_t)v); put8(b,p,(uint8_t)(v>>8));
}
static void put32(uint8_t *b, size_t *p, uint32_t v) {
    put8(b,p,(uint8_t)v); put8(b,p,(uint8_t)(v>>8)); put8(b,p,(uint8_t)(v>>16)); put8(b,p,(uint8_t)(v>>24));
}

static void put_uuid(
    uint8_t *b,
    size_t *p,
    aether_uuid_t u) {

    for (int shift = 56; shift >= 0; shift -= 8) {
        put8(
            b,
            p,
            (uint8_t)(u.msb >> (unsigned)shift)
        );
    }

    for (int shift = 56; shift >= 0; shift -= 8) {
        put8(
            b,
            p,
            (uint8_t)(u.lsb >> (unsigned)shift)
        );
    }
}

static void put_pack(uint8_t *b, size_t *p, size_t v) {
    assert(v < 251u);
    put8(b,p,(uint8_t)v);
}
static void put_array(uint8_t *b, size_t *p, const uint8_t *d, size_t n) {
    put_pack(b,p,n); memcpy(b+*p,d,n); *p += n;
}

static void put_signed_hydrogen_key(
    uint8_t *b,
    size_t *p,
    const uint8_t key[32],
    const hydro_sign_keypair *signer) {

    static const char ctx[hydro_sign_CONTEXTBYTES] = {
        'a','e','t','h','e','r','i','o'
    };
    uint8_t signature[hydro_sign_BYTES];

    assert(hydro_sign_create(
        signature,
        key,
        32u,
        ctx,
        signer->sk) == 0);

    put8(b,p,2); /* HydrogenCurvePublic */
    memcpy(b+*p,key,32u);
    *p += 32u;

    put8(b,p,2); /* SignHYDROGEN */
    memcpy(b+*p,signature,sizeof(signature));
    *p += sizeof(signature);
}

static void put_encrypted_packet(
    uint8_t *packet,
    size_t *p,
    uint8_t command,
    const uint8_t key[32],
    const uint8_t *plain,
    size_t plain_len) {

    uint8_t cipher[2048];
    size_t cipher_len=0u;

    assert(aether_hydrogen_symmetric_encrypt(
        key,
        plain,
        plain_len,
        cipher,
        sizeof(cipher),
        &cipher_len) == 0);

    *p=0u;
    put8(packet,p,command);
    put_array(packet,p,cipher,cipher_len);
}


static uint8_t take8(
    const uint8_t *data,
    size_t length,
    size_t *pos) {

    assert(*pos < length);
    return data[(*pos)++];
}

static uint32_t take32(
    const uint8_t *data,
    size_t length,
    size_t *pos) {

    uint32_t value=0u;
    for (unsigned i=0u;i<4u;i++) {
        value |= (uint32_t)take8(data,length,pos) << (i*8u);
    }
    return value;
}


static uint64_t take64(
    const uint8_t *data,
    size_t length,
    size_t *pos) {

    uint64_t value=0u;
    for (unsigned i=0u;i<8u;i++) {
        value =
            (value << 8u) |
            (uint64_t)take8(data,length,pos);
    }
    return value;
}


static void expect_uuid(
    const uint8_t *data,
    size_t length,
    size_t *pos,
    aether_uuid_t expected) {

    assert(take64(data,length,pos)==expected.msb);
    assert(take64(data,length,pos)==expected.lsb);
}

static void expect_bytes(
    const uint8_t *data,
    size_t length,
    size_t *pos,
    const uint8_t *expected,
    size_t expected_len) {

    assert(*pos <= length);
    assert(expected_len <= length-*pos);
    assert(memcmp(data+*pos,expected,expected_len)==0);
    *pos += expected_len;
}


static uint64_t take_pack(
    const uint8_t *data,
    size_t length,
    size_t *pos) {

    const uint64_t u8=251u;
    const uint64_t u16=1515u;
    const uint64_t u32=1049835u;
    const uint64_t u64=
        u32 + (UINT64_C(4294967296) * 256u);

    uint64_t val=take8(data,length,pos);
    if (val < u8) return val;

    uint64_t v=take8(data,length,pos);
    val=((val-u8) << 8u) + u8 + v;
    if (val < u16) return val;

    uint64_t f=0u;
    f |= (uint64_t)take8(data,length,pos);
    f |= (uint64_t)take8(data,length,pos) << 8u;

    val=((val-u16) << 16u) + u16 + f;
    if (val < u32) return val;

    uint64_t f1=0u;
    for (unsigned i=0u;i<4u;i++) {
        f1 |= (uint64_t)take8(data,length,pos) << (i*8u);
    }

    val=((val-u32) << 32u) + u32 + f1;
    assert(val < u64);
    return val;
}

static const uint8_t *take_array(
    const uint8_t *data,
    size_t length,
    size_t *pos,
    size_t *array_len) {

    uint64_t packed=take_pack(data,length,pos);
    assert(packed <= (uint64_t)SIZE_MAX);

    *array_len=(size_t)packed;

    assert(*pos <= length);
    assert(*array_len <= length-*pos);

    const uint8_t *result=data+*pos;
    *pos += *array_len;
    return result;
}


static size_t decrypt_registration_enter(
    const uint8_t *packet,
    size_t packet_len,
    const hydro_kx_keypair *server_key,
    uint8_t *plain,
    size_t plain_capacity) {

    size_t pos=0u;

    assert(take8(packet,packet_len,&pos)==4u);
    assert(take8(packet,packet_len,&pos)==1u); /* HYDROGEN */

    size_t cipher_len=0u;
    const uint8_t *cipher=
        take_array(packet,packet_len,&pos,&cipher_len);

    assert(pos==packet_len);

    size_t plain_len=0u;
    assert(aether_hydrogen_asymmetric_decrypt(
        server_key->pk,
        server_key->sk,
        cipher,
        cipher_len,
        plain,
        plain_capacity,
        &plain_len)==0);

    return plain_len;
}



static uint32_t test_crc32(const uint8_t *data, size_t length) {
    uint32_t crc = UINT32_C(0xffffffff);

    for (size_t i = 0u; i < length; ++i) {
        crc ^= data[i];

        for (unsigned j = 0u; j < 8u; ++j) {
            uint32_t mask =
                (uint32_t)-(int32_t)(crc & 1u);

            crc = (crc >> 1u) ^
                (UINT32_C(0xedb88320) & mask);
        }
    }

    return ~crc;
}





static aether_status_t t_open(void *ctx, aether_channel_t channel, const aether_endpoint_t *ep) {
    fixture_t *f = ctx;
    f->opens++;
    f->last_open_channel = channel;
    f->last_endpoint = *ep;

    if (f->fail_open_port != 0u &&
        ep->port == f->fail_open_port) {

        return AETHER_ERR_TRANSPORT;
    }
    return AETHER_OK;
}

static void t_close(void *ctx, aether_channel_t channel) {
    fixture_t *f = ctx; (void)channel; f->closes++;
}
static aether_status_t t_send(
    void *ctx,
    aether_channel_t channel,
    const uint8_t *data,
    size_t length) {

    fixture_t *f =
        ctx;

    assert(
        length <=
        sizeof(f->packet));

    memcpy(
        f->packet,
        data,
        length);

    f->packet_len =
        length;

    f->packet_channel =
        channel;

    f->sends++;

    if (f->fail_sends_remaining !=
        0u) {

        f->fail_sends_remaining--;

        return AETHER_ERR_TRANSPORT;
    }

    return AETHER_OK;
}
static aether_status_t flash_load(void *ctx, uint8_t *dst, size_t capacity, size_t *length) {
    fixture_t *f = ctx;
    if (f->flash_len == 0) return AETHER_ERR_STORAGE;
    assert(f->flash_len <= capacity);
    memcpy(dst, f->flash, f->flash_len);
    *length = f->flash_len;
    return AETHER_OK;
}

static aether_status_t flash_save(void *ctx, const uint8_t *src, size_t length) {
    fixture_t *f = ctx;
    if (f->fail_flash_saves != 0u) {
        f->fail_flash_saves--;
        return AETHER_ERR_STORAGE;
    }
    assert(length <= sizeof(f->flash));
    memcpy(f->flash, src, length);
    f->flash_len = length;
    return AETHER_OK;
}

static aether_status_t pow_cb(void *ctx, const uint8_t *salt, size_t salt_len,
                              const uint8_t *suffix, size_t suffix_len,
                              uint8_t pool_size, int32_t max_hash,
                              int32_t *passwords, size_t capacity, size_t *count) {
    (void)ctx; (void)salt; (void)salt_len; (void)suffix; (void)suffix_len; (void)pool_size; (void)max_hash;
    if (capacity < 2) return AETHER_ERR_OVERFLOW;
    passwords[0]=123; passwords[1]=456; *count=2; return AETHER_OK;
}
static void on_event(void *ctx, const aether_event_t *e) {
    fixture_t *f=ctx;
    if (e->type==AETHER_EVENT_MESSAGE) {
        f->messages++;
        f->last_from=e->as.message.from;
        assert(e->as.message.payload.length <= sizeof(f->last_message));
        memcpy(f->last_message,e->as.message.payload.data,e->as.message.payload.length);
        f->last_message_len=e->as.message.payload.length;

    } else if (e->type==AETHER_EVENT_REGISTERED) {
        f->registered_events++;
    } else if (e->type==AETHER_EVENT_REQUEST_DONE) {
        f->done_events++;
    } else if (e->type==AETHER_EVENT_REQUEST_TIMEOUT) {
        f->timeout_events++;
        f->last_timeout_request=e->as.request_timeout.request_id;

    } else if (e->type==AETHER_EVENT_ERROR) {
        f->errors++;
        f->last_error_code=e->as.error.code;
        f->last_error_origin=e->as.error.origin;
        f->last_error_state=e->as.error.state;
        f->last_error_request_id=e->as.error.request_id;
    }
}




static aether_client_config_t config_for(fixture_t *f) {
    aether_client_config_t c;
    memset(&c,0,sizeof(c));

    c.parent_uid=(aether_uuid_t){
        0x1122334455667788ULL,
        0x99aabbccddeeff00ULL
    };

    c.registration_endpoint.codec=AETHER_CODEC_TCP;
    c.registration_endpoint.address.kind=AETHER_ADDR_IPV4;
    c.registration_endpoint.address.length=4;
    c.registration_endpoint.address.bytes[0]=127;
    c.registration_endpoint.address.bytes[1]=0;
    c.registration_endpoint.address.bytes[2]=0;
    c.registration_endpoint.address.bytes[3]=1;
    c.registration_endpoint.port=9010;

    c.ping_interval_ms=6000;
    c.rx_window_ms=5000;

    c.transport=(aether_transport_vtable_t){
        f,t_open,t_close,t_send
    };
    c.flash=(aether_flash_vtable_t){
        f,flash_load,flash_save
    };
    c.pow=(aether_pow_vtable_t){
        f,pow_cb
    };

    c.trusted_sign_keys=f->root_signer.pk;
    c.trusted_sign_key_count=1;

    c.crypto=&AETHER_TEST_HYDROGEN_CRYPTO;

    c.crypto_ctx=f;

    c.trusted_sign_keys=f->root_signer.pk;

    c.trusted_sign_key_count=1u;


    c.on_event=on_event;
    c.event_ctx=f;
    return c;
}





static void start_registration_component(
    aether_registration_t *registration,
    aether_client_t *client) {

    aether_registration_init(
        registration,
        client);

    assert(
        aether_registration_poll(
            registration) ==
        AETHER_OK);
}


static void start_self_registration(
    aether_registration_t *registration,
    aether_client_t *client) {

    assert(
        aether_client_start(client) ==
        AETHER_OK);

    assert(
        client->state ==
        AETHER_STATE_NO_IDENTITY);

    start_registration_component(
        registration,
        client);
}




static void test_provisioned_identity(void) {
    fixture_t f;
    memset(
        &f,
        0,
        sizeof(f));

    hydro_sign_keygen(
        &f.root_signer);

    aether_client_config_t cfg =
        config_for(
            &f);

    cfg.pow.generate =
        NULL;

    aether_client_t client;

    aether_client_init(
        &client,
        &cfg);

    assert(
        aether_client_start(
            &client) ==
        AETHER_OK);

    assert(
        client.state ==
        AETHER_STATE_NO_IDENTITY);

    assert(
        !client.registered);

    assert(
        f.flash_len ==
        0u);

    uint8_t master_key[AETHER_KEY_BYTES];

    for (size_t i = 0u;
         i < sizeof(master_key);
         ++i) {

        master_key[i] =
            (uint8_t)(0x40u + i);
    }

    const int16_t cloud[] = {
        7,
        11
    };

    const aether_uuid_t uid = {
        UINT64_C(0x1122334455667788),
        UINT64_C(0x99aabbccddeeff00)
    };

    const aether_uuid_t alias = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    assert(
        aether_client_provision_identity(
            &client,
            uid,
            alias,
            NULL,
            cloud,
            2u) ==
        AETHER_ERR_ARGUMENT);

    assert(
        aether_client_provision_identity(
            &client,
            uid,
            alias,
            master_key,
            NULL,
            2u) ==
        AETHER_ERR_ARGUMENT);

    assert(
        aether_client_provision_identity(
            &client,
            uid,
            alias,
            master_key,
            cloud,
            0u) ==
        AETHER_ERR_ARGUMENT);

    assert(
        aether_client_provision_identity(
            &client,
            uid,
            alias,
            master_key,
            cloud,
            AETHER_MAX_SERVERS + 1u) ==
        AETHER_ERR_OVERFLOW);

    assert(
        client.state ==
        AETHER_STATE_NO_IDENTITY);

    assert(
        !client.registered);

    assert(
        f.flash_len ==
        0u);

    assert(
        f.registered_events ==
        0u);

    assert(
        aether_client_provision_identity(
            &client,
            uid,
            alias,
            master_key,
            cloud,
            2u) ==
        AETHER_OK);

    assert(
        client.registered);

    assert(
        client.state ==
        AETHER_STATE_SERVER_RESOLVING);

    assert(
        client.uid.msb ==
            uid.msb &&
        client.uid.lsb ==
            uid.lsb);

    assert(
        client.alias.msb ==
            alias.msb &&
        client.alias.lsb ==
            alias.lsb);

    assert(
        memcmp(
            client.master_key,
            master_key,
            sizeof(master_key)) ==
        0);

    assert(
        client.cloud_count ==
        2u);

    assert(
        client.cloud_sids[0] ==
        7);

    assert(
        client.cloud_sids[1] ==
        11);

    assert(
        client.server_count ==
        0u);

    assert(
        client.active_server_index ==
        -1);

    assert(
        f.registered_events ==
        1u);

    assert(
        f.flash_len >
        0u);

    assert(
        f.opens ==
        1u);

    assert(
        f.last_open_channel ==
        AETHER_CHANNEL_REGISTRATION);

    assert(
        aether_client_provision_identity(
            &client,
            uid,
            alias,
            master_key,
            cloud,
            2u) ==
        AETHER_ERR_STATE);

    assert(
        f.registered_events ==
        1u);

    /*
     * Simulate a fresh process/MCU boot. Only the ordinary persisted client
     * state is copied; there is deliberately no registration component.
     */
    fixture_t restored;
    memset(
        &restored,
        0,
        sizeof(restored));

    memcpy(
        restored.flash,
        f.flash,
        f.flash_len);

    restored.flash_len =
        f.flash_len;

    restored.root_signer =
        f.root_signer;

    aether_client_config_t restored_cfg =
        config_for(
            &restored);

    restored_cfg.pow.generate =
        NULL;

    aether_client_t restored_client;

    aether_client_init(
        &restored_client,
        &restored_cfg);

    assert(
        aether_client_start(
            &restored_client) ==
        AETHER_OK);

    assert(
        restored_client.registered);

    assert(
        restored_client.state ==
        AETHER_STATE_SERVER_RESOLVING);

    assert(
        restored_client.uid.msb ==
            uid.msb &&
        restored_client.uid.lsb ==
            uid.lsb);

    assert(
        restored_client.alias.msb ==
            alias.msb &&
        restored_client.alias.lsb ==
            alias.lsb);

    assert(
        memcmp(
            restored_client.master_key,
            master_key,
            sizeof(master_key)) ==
        0);

    assert(
        restored_client.cloud_count ==
        2u);

    assert(
        restored_client.cloud_sids[0] ==
        7);

    assert(
        restored_client.cloud_sids[1] ==
        11);

    assert(
        restored_client.server_count ==
        0u);

    assert(
        restored.opens ==
        1u);

    assert(
        restored.last_open_channel ==
        AETHER_CHANNEL_REGISTRATION);
}



static void test_rejects_untrusted_registration_key(void) {
    fixture_t f;
    memset(&f,0,sizeof(f));

    assert(aether_hydrogen_init()==0);

    test_provisioned_identity();


    /*
     * config_for() trusts only f.root_signer. The attacker produces a
     * cryptographically valid Hydrogen signature, but its public key is not
     * in the client's trust set.
     */
    hydro_sign_keygen(&f.root_signer);

    hydro_sign_keypair attacker;
    hydro_sign_keygen(&attacker);

    hydro_kx_keygen(&f.registration_kx);

    aether_client_t c;
    aether_registration_t registration;
    aether_client_config_t cfg=config_for(&f);

    aether_client_init(&c,&cfg);

    start_self_registration(&registration,&c);
    assert(c.state==AETHER_STATE_REG_CONNECTING);

    aether_client_on_transport_state(
        &c,
        AETHER_CHANNEL_REGISTRATION,
        true);

    assert(c.state==AETHER_STATE_REG_WAIT_SERVER_KEY);

    unsigned sends_before=f.sends;

    uint8_t packet[256];
    size_t p=0u;

    put8(packet,&p,0); /* CMD_RESULT */
    put32(packet,&p,c.req_server_key);

    put_signed_hydrogen_key(
        packet,
        &p,
        f.registration_kx.pk,
        &attacker);

    assert(aether_client_on_rx(
        &c,
        AETHER_CHANNEL_REGISTRATION,
        packet,
        p)==AETHER_ERR_CRYPTO);

    assert(c.state==AETHER_STATE_ERROR);
    assert(!c.registered);
    assert(f.errors==1u);

    assert(f.last_error_code==AETHER_ERR_CRYPTO);
    assert(f.last_error_origin==AETHER_ERROR_ORIGIN_TRUST);
    assert(f.last_error_state==AETHER_STATE_REG_WAIT_SERVER_KEY);
    assert(f.last_error_request_id==0u);


    /*
     * Verification happens before random session-key generation and before
     * requestWorkProofData. An untrusted signer must therefore have no
     * observable handshake side effects beyond the error.
     */
    assert(f.sends==sends_before);


    for (size_t i=0u;i<AETHER_KEY_BYTES;++i) {
        assert(c.temp_key[i]==0u);
        assert(c.master_key[i]==0u);
    }

    unsigned opens_after_security_error=f.opens;
    unsigned sends_after_security_error=f.sends;

    /*
     * ERROR is a stable hand-off point to business logic. Time passing alone
     * must not restart a security-failed handshake.
     */
    aether_client_poll(
        &c,
        c.now_ms+60000u);

    assert(c.state==AETHER_STATE_ERROR);
    assert(f.opens==opens_after_security_error);
    assert(f.sends==sends_after_security_error);

    /*
     * Recovery happens only after the application explicitly requests it.
     */

    assert(aether_client_retry(&c)==AETHER_OK);
    assert(c.state==AETHER_STATE_NO_IDENTITY);
    assert(f.opens==opens_after_security_error);

    start_registration_component(&registration,&c);

    assert(c.state==AETHER_STATE_REG_CONNECTING);
    assert(f.opens==opens_after_security_error+1u);
    assert(f.last_open_channel==AETHER_CHANNEL_REGISTRATION);


    /*
     * retry() is valid only at the ERROR hand-off boundary.
     */
    assert(aether_client_retry(&c)==AETHER_ERR_STATE);
}



static void test_work_rx_crypto_error_hands_off_to_business_logic(void) {
    fixture_t f;
    memset(&f,0,sizeof(f));

    assert(aether_hydrogen_init()==0);
    hydro_sign_keygen(&f.root_signer);

    aether_client_config_t cfg=config_for(&f);
    aether_client_t c;
    aether_client_init(&c,&cfg);

    /*
     * Build the minimum valid registered topology needed for explicit
     * work retry. This test is about the WORK receive/error boundary, not
     * registration itself.
     */
    c.registered=true;
    c.state=AETHER_STATE_READY;
    c.server_count=1u;
    c.active_server_index=0;

    c.servers[0].valid=true;
    c.servers[0].sid=7;
    c.servers[0].endpoint.codec=AETHER_CODEC_UDP;
    c.servers[0].endpoint.address.kind=AETHER_ADDR_IPV4;
    c.servers[0].endpoint.address.length=4u;
    c.servers[0].endpoint.address.bytes[0]=127u;
    c.servers[0].endpoint.address.bytes[1]=0u;
    c.servers[0].endpoint.address.bytes[2]=0u;
    c.servers[0].endpoint.address.bytes[3]=1u;
    c.servers[0].endpoint.port=9011u;

    /*
     * ClientApiUnsafe.sendSafeApiData command 4 followed by a byte-array
     * of length 1. One byte cannot possibly contain Hydrogen secretbox
     * overhead, therefore decrypt must fail authentication.
     */
    const uint8_t invalid_work_packet[]={
        4u,
        1u,
        0u
    };

    unsigned errors_before=f.errors;
    unsigned opens_before=f.opens;
    unsigned sends_before=f.sends;

    assert(aether_client_on_rx(
        &c,
        AETHER_CHANNEL_WORK,
        invalid_work_packet,
        sizeof(invalid_work_packet))==AETHER_ERR_CRYPTO);

    assert(c.state==AETHER_STATE_ERROR);
    assert(c.registered);
    assert(f.errors==errors_before+1u);

    assert(f.last_error_code==AETHER_ERR_CRYPTO);
    assert(f.last_error_origin==AETHER_ERROR_ORIGIN_CRYPTO);
    assert(f.last_error_state==AETHER_STATE_READY);
    assert(f.last_error_request_id==0u);

    assert(f.opens==opens_before);
    assert(f.sends==sends_before);

    /*
     * ERROR is the business hand-off boundary. Merely advancing time must
     * never select a recovery policy on behalf of the application.
     */
    aether_client_poll(&c,60000u);

    assert(c.state==AETHER_STATE_ERROR);
    assert(f.opens==opens_before);
    assert(f.sends==sends_before);

    /*
     * Application explicitly chooses retry. Since registration remains
     * valid, retry reconnects WORK rather than entering registration.
     */
    assert(aether_client_retry(&c)==AETHER_OK);

    assert(c.state==AETHER_STATE_WORK_CONNECTING);
    assert(c.registered);
    assert(f.opens==opens_before+1u);
    assert(f.last_open_channel==AETHER_CHANNEL_WORK);
    assert(f.last_endpoint.port==9011u);
}





static void test_server_request_error_context(void) {
    fixture_t f;
    memset(&f,0,sizeof(f));

    assert(aether_hydrogen_init()==0);
    hydro_sign_keygen(&f.root_signer);

    aether_client_config_t cfg=config_for(&f);
    aether_client_t c;
    aether_client_init(&c,&cfg);

    c.registered=true;
    c.state=AETHER_STATE_READY;

    const uint8_t payload[]={'x'};
    aether_uuid_t destination={
        0x0102030405060708ULL,
        0x1112131415161718ULL
    };
    uint32_t req=0u;

    assert(aether_client_send_message(
        &c,
        destination,
        payload,
        sizeof(payload),
        &req)==AETHER_OK);

    assert(req!=0u);

    uint8_t inner[16];
    size_t q=0u;
    put8(inner,&q,1u); /* CMD_ERROR */
    put32(inner,&q,req);

    uint8_t packet[128];
    size_t p=0u;

    put_encrypted_packet(
        packet,
        &p,
        4u,
        c.work_rx_key,
        inner,
        q);

    unsigned errors_before=f.errors;



    assert(aether_client_on_rx(
        &c,
        AETHER_CHANNEL_WORK,
        packet,
        p)==AETHER_OK);



    assert(c.state==AETHER_STATE_READY);
    assert(f.errors==errors_before+1u);
    assert(f.last_error_code==AETHER_ERR_REMOTE);
    assert(f.last_error_origin==AETHER_ERROR_ORIGIN_REMOTE);
    assert(f.last_error_state==AETHER_STATE_READY);
    assert(f.last_error_request_id==req);
}


static void test_full_registration_and_message(void) {

    test_rejects_untrusted_registration_key();
    test_work_rx_crypto_error_hands_off_to_business_logic();
    test_server_request_error_context();

    fixture_t f;
    memset(&f,0,sizeof(f));

    assert(aether_hydrogen_init()==0);
    hydro_sign_keygen(&f.root_signer);
    hydro_kx_keygen(&f.registration_kx);
    hydro_kx_keygen(&f.global_kx);

    aether_client_t c;
    aether_registration_t registration;
    aether_client_config_t cfg=config_for(&f);
    aether_client_init(&c,&cfg);

    start_self_registration(&registration,&c);
    assert(c.state==AETHER_STATE_REG_CONNECTING && f.last_open_channel==AETHER_CHANNEL_REGISTRATION);

    aether_client_on_transport_state(&c,AETHER_CHANNEL_REGISTRATION,true);
    assert(c.state==AETHER_STATE_REG_WAIT_SERVER_KEY);
    assert(f.packet_len==6 && f.packet[0]==3 && f.packet[5]==1);


    uint8_t packet[2048]; size_t p=0;
    put8(packet,&p,0);
    put32(packet,&p,c.req_server_key);
    put_signed_hydrogen_key(
        packet,&p,f.registration_kx.pk,&f.root_signer);
    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_REGISTRATION,packet,p)==AETHER_OK);

    assert(c.state==AETHER_STATE_REG_WAIT_POW);
    assert(f.packet_channel==AETHER_CHANNEL_REGISTRATION && f.packet[0]==4 && f.packet[1]==1);


    /*
     * Server-side decrypt the client's real requestWorkProofData packet.
     */
    uint8_t request_pow_plain[512];
    size_t request_pow_plain_len=
        decrypt_registration_enter(
            f.packet,
            f.packet_len,
            &f.registration_kx,
            request_pow_plain,
            sizeof(request_pow_plain));

    size_t request_pow_pos=0u;

    assert(take8(
        request_pow_plain,
        request_pow_plain_len,
        &request_pow_pos)==6u); /* setReturnKey */

    assert(take8(
        request_pow_plain,
        request_pow_plain_len,
        &request_pow_pos)==3u); /* HydrogenSecretBox */

    expect_bytes(
        request_pow_plain,
        request_pow_plain_len,
        &request_pow_pos,
        c.temp_key,
        32u);

    assert(take8(
        request_pow_plain,
        request_pow_plain_len,
        &request_pow_pos)==4u); /* requestWorkProofData */

    assert(take32(
        request_pow_plain,
        request_pow_plain_len,
        &request_pow_pos)==registration.req_pow);

    expect_uuid(
        request_pow_plain,
        request_pow_plain_len,
        &request_pow_pos,
        cfg.parent_uid);

    assert(take8(
        request_pow_plain,
        request_pow_plain_len,
        &request_pow_pos)==0u); /* AE_BCRYPT_CRC32 */

    assert(request_pow_pos==request_pow_plain_len);



    /*
     * A lost registration response must not leave a writable transport stuck
     * forever in REG_WAIT_POW.
     *
     * start()/transport callbacks can run before the application's first
     * poll(). The first poll may use a large absolute monotonic uptime, so it
     * establishes the client's timebase rather than treating started_at=0 as
     * an ancient request.
     */
    uint32_t timed_out_pow_req=registration.req_pow;
    unsigned timeout_opens_before=f.opens;
    unsigned timeout_closes_before=f.closes;
    uint64_t first_monotonic_now=UINT64_C(987654321000);

    assert(!c.time_initialized);

    aether_client_poll(&c,first_monotonic_now);

    assert(c.time_initialized);
    assert(c.now_ms==first_monotonic_now);
    assert(c.state==AETHER_STATE_REG_WAIT_POW);
    assert(f.opens==timeout_opens_before);
    assert(f.closes==timeout_closes_before);
    assert(c.state_started_at_ms==
           (uint32_t)first_monotonic_now);

    aether_client_poll(
        &c,
        first_monotonic_now+12001u);

    assert(c.state==AETHER_STATE_REG_CONNECTING);
    assert(f.opens==timeout_opens_before+1);
    assert(f.closes==timeout_closes_before+1);
    assert(f.last_open_channel==AETHER_CHANNEL_REGISTRATION);

    aether_client_on_transport_state(
        &c,AETHER_CHANNEL_REGISTRATION,true);
    assert(c.state==AETHER_STATE_REG_WAIT_SERVER_KEY);

    p=0;
    put8(packet,&p,0);
    put32(packet,&p,c.req_server_key);
    put_signed_hydrogen_key(
        packet,&p,f.registration_kx.pk,&f.root_signer);
    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_REGISTRATION,packet,p)==AETHER_OK);

    assert(c.state==AETHER_STATE_REG_WAIT_POW);
    assert(registration.req_pow!=timed_out_pow_req);





    /*
     * Registration transport loss restarts the handshake from the root API.
     */
    uint32_t old_server_key_req=c.req_server_key;
    unsigned registration_opens_before=f.opens;

    aether_client_on_transport_state(&c,AETHER_CHANNEL_REGISTRATION,false);
    assert(c.state==AETHER_STATE_REG_CONNECTING);
    assert(f.opens==registration_opens_before+1);
    assert(f.last_open_channel==AETHER_CHANNEL_REGISTRATION);

    /*
     * A delayed response from the old transport must not poison the restarted
     * handshake while the replacement socket is still connecting.
     */

    p=0;
    put8(packet,&p,0);
    put32(packet,&p,old_server_key_req);
    put_signed_hydrogen_key(
        packet,&p,f.registration_kx.pk,&f.root_signer);
    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_REGISTRATION,packet,p)==AETHER_OK);

    assert(c.state==AETHER_STATE_REG_CONNECTING);

    aether_client_on_transport_state(&c,AETHER_CHANNEL_REGISTRATION,true);
    assert(c.state==AETHER_STATE_REG_WAIT_SERVER_KEY);
    assert(c.req_server_key!=old_server_key_req);

    /*
     * Continue the restarted registration flow with the fresh request id.
     */

    p=0;
    put8(packet,&p,0);
    put32(packet,&p,c.req_server_key);
    put_signed_hydrogen_key(
        packet,&p,f.registration_kx.pk,&f.root_signer);
    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_REGISTRATION,packet,p)==AETHER_OK);

    assert(c.state==AETHER_STATE_REG_WAIT_POW);
    assert(f.packet_channel==AETHER_CHANNEL_REGISTRATION);
    assert(f.packet[0]==4 && f.packet[1]==1);



    p=0;
    uint8_t pow_plain[512]; size_t q=0;
    put8(pow_plain,&q,0);
    put32(pow_plain,&q,registration.req_pow);
    put_array(pow_plain,&q,(const uint8_t*)"salt",4);
    put_array(pow_plain,&q,(const uint8_t*)"suffix",6);
    put8(pow_plain,&q,2);
    put32(pow_plain,&q,9999);
    put_signed_hydrogen_key(
        pow_plain,&q,f.global_kx.pk,&f.root_signer);

    put_encrypted_packet(
        packet,&p,3,c.temp_key,pow_plain,q);

    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_REGISTRATION,packet,p)==AETHER_OK);

    assert(c.state==AETHER_STATE_REG_WAIT_FINISH);
    assert(registration.pow_password_count==2 && registration.pow_passwords[0]==123);


    /*
     * Verify outbound registration payload against the two real server
     * private keys.
     */
    uint8_t registration_plain[2048];
    size_t registration_plain_len=
        decrypt_registration_enter(
            f.packet,
            f.packet_len,
            &f.registration_kx,
            registration_plain,
            sizeof(registration_plain));

    size_t registration_pos=0u;

    assert(take8(
        registration_plain,
        registration_plain_len,
        &registration_pos)==6u); /* setReturnKey */

    assert(take8(
        registration_plain,
        registration_plain_len,
        &registration_pos)==3u); /* HydrogenSecretBox */

    expect_bytes(
        registration_plain,
        registration_plain_len,
        &registration_pos,
        c.temp_key,
        32u);


    assert(take8(
        registration_plain,
        registration_plain_len,
        &registration_pos)==7u); /* registrationDirect */

    assert(take32(
        registration_plain,
        registration_plain_len,
        &registration_pos)==registration.req_finish);

    size_t sent_salt_len=0u;
    const uint8_t *sent_salt=
        take_array(
            registration_plain,
            registration_plain_len,
            &registration_pos,
            &sent_salt_len);
    assert(sent_salt_len==4u);
    assert(memcmp(sent_salt,"salt",4u)==0);

    size_t sent_suffix_len=0u;
    const uint8_t *sent_suffix=
        take_array(
            registration_plain,
            registration_plain_len,
            &registration_pos,
            &sent_suffix_len);
    assert(sent_suffix_len==6u);
    assert(memcmp(sent_suffix,"suffix",6u)==0);

    assert(take8(
        registration_plain,
        registration_plain_len,
        &registration_pos)==2u);

    assert((int32_t)take32(
        registration_plain,
        registration_plain_len,
        &registration_pos)==123);

    assert((int32_t)take32(
        registration_plain,
        registration_plain_len,
        &registration_pos)==456);

    expect_uuid(
        registration_plain,
        registration_plain_len,
        &registration_pos,
        cfg.parent_uid);

    assert(take8(
        registration_plain,
        registration_plain_len,
        &registration_pos)==3u); /* HydrogenSecretBox masterKey */

    expect_bytes(
        registration_plain,
        registration_plain_len,
        &registration_pos,
        c.master_key,
        32u);

    assert(registration_pos==registration_plain_len);



    aether_uuid_t alias={0x0102030405060708ULL,0x1112131415161718ULL};
    aether_uuid_t uid={0x2122232425262728ULL,0x3132333435363738ULL};

    p=0; q=0;
    put8(pow_plain,&q,0);
    put32(pow_plain,&q,registration.req_finish);
    put_uuid(pow_plain,&q,alias);
    put_uuid(pow_plain,&q,uid);
    put_pack(pow_plain,&q,1);
    put16(pow_plain,&q,7);


    put_encrypted_packet(
        packet,&p,3,c.temp_key,pow_plain,q);

    unsigned resolve_send_errors_before =
        f.errors;

    f.fail_sends_remaining =
        1u;



    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_REGISTRATION,packet,p)==AETHER_OK);

    assert(
        f.fail_sends_remaining ==
        0u);

    assert(
        f.errors ==
        resolve_send_errors_before);


    assert(c.state==AETHER_STATE_SERVER_RESOLVING);
    assert(f.registered_events==1);


    /*
     * Crash/restart exactly between registrationDirect and resolveServers.
     *
     * Identity + Cloud are durable already, while ServerDescriptor[] is still
     * an empty disposable cache. A restored mandatory client must recover the
     * descriptors without invoking the optional PoW/registration policy.
     */
    fixture_t recovered;
    memset(&recovered,0,sizeof(recovered));

    memcpy(
        recovered.flash,
        f.flash,
        f.flash_len);

    recovered.flash_len =
        f.flash_len;

    recovered.root_signer =
        f.root_signer;

    recovered.registration_kx =
        f.registration_kx;

    recovered.global_kx =
        f.global_kx;

    recovered.fail_open_port =
        9010u;


    aether_client_config_t recovered_cfg =
        config_for(&recovered);

    recovered_cfg.pow.generate =
        NULL;

    aether_client_t recovered_client;

    aether_client_init(
        &recovered_client,
        &recovered_cfg);

    assert(
        aether_client_start(
            &recovered_client) ==
        AETHER_OK);

    assert(
        recovered_client.registered);

    assert(
        recovered_client.uid.msb ==
            uid.msb &&
        recovered_client.uid.lsb ==
            uid.lsb);

    assert(
        recovered_client.alias.msb ==
            alias.msb &&
        recovered_client.alias.lsb ==
            alias.lsb);

    assert(
        recovered_client.cloud_count ==
        1u);

    assert(
        recovered_client.cloud_sids[0] ==
        7);

    assert(
        recovered_client.server_count ==
        0u);

    assert(
        recovered_client.state ==
        AETHER_STATE_SERVER_RESOLVING);

    assert(
        recovered.opens ==
        1u);

    assert(
        recovered.last_open_channel ==
        AETHER_CHANNEL_REGISTRATION);

    /*
     * A missing bootstrap endpoint is a normal SERVER_RESOLVING condition.
     * Startup succeeds, no ERROR event is emitted, and poll retries later.
     */
    assert(
        recovered.errors ==
        0u);

    recovered.fail_open_port =
        0u;

    aether_client_poll(
        &recovered_client,
        UINT64_C(1000));

    assert(
        recovered.opens ==
        1u);

    aether_client_poll(
        &recovered_client,
        UINT64_C(1000000));

    assert(
        recovered.opens ==
        2u);

    assert(
        recovered.last_open_channel ==
        AETHER_CHANNEL_REGISTRATION);

    assert(
        recovered_client.state ==
        AETHER_STATE_SERVER_RESOLVING);

    /*
     * A synchronous getServerKey send failure is equally transient.
     * The failed send consumes no state-machine identity and emits no error.
     */
    recovered.fail_sends_remaining =
        1u;

    unsigned bootstrap_sends_before =
        recovered.sends;

    aether_client_on_transport_state(
        &recovered_client,
        AETHER_CHANNEL_REGISTRATION,
        true);

    assert(
        recovered.sends ==
        bootstrap_sends_before +
        1u);

    assert(
        recovered.fail_sends_remaining ==
        0u);

    assert(
        recovered_client.state ==
        AETHER_STATE_SERVER_RESOLVING);

    assert(
        recovered.errors ==
        0u);

    aether_client_poll(
        &recovered_client,
        UINT64_C(2000000));

    assert(
        recovered.opens ==
        3u);

    assert(
        recovered.last_open_channel ==
        AETHER_CHANNEL_REGISTRATION);

    assert(
        recovered_client.state ==
        AETHER_STATE_SERVER_RESOLVING);


    /*
     * A writable bootstrap transport first requests the signed registration
     * server key.
     */
    aether_client_on_transport_state(
        &recovered_client,
        AETHER_CHANNEL_REGISTRATION,
        true);

    assert(
        recovered_client.state ==
        AETHER_STATE_SERVER_RESOLVING);

    assert(
        recovered.packet_channel ==
        AETHER_CHANNEL_REGISTRATION);

    assert(
        recovered.packet_len ==
        6u);

    assert(
        recovered.packet[0] ==
        3u);

    /*
     * Feed the trusted registration-server public key. SERVER_RESOLVING must
     * now create only a temporary return key and immediately issue
     * setReturnKey + resolveServers. No PoW callback exists in recovered_cfg.
     */
    size_t recovered_packet_pos =
        0u;

    put8(
        packet,
        &recovered_packet_pos,
        0u);

    put32(
        packet,
        &recovered_packet_pos,
        recovered_client.req_server_key);

    put_signed_hydrogen_key(
        packet,
        &recovered_packet_pos,
        recovered.registration_kx.pk,
        &recovered.root_signer);

    assert(
        aether_client_on_rx(
            &recovered_client,
            AETHER_CHANNEL_REGISTRATION,
            packet,
            recovered_packet_pos) ==
        AETHER_OK);

    assert(
        recovered_client.state ==
        AETHER_STATE_SERVER_RESOLVING);

    assert(
        recovered.packet_channel ==
        AETHER_CHANNEL_REGISTRATION);

    uint8_t recovered_resolve_plain[512];

    size_t recovered_resolve_plain_len =
        decrypt_registration_enter(
            recovered.packet,
            recovered.packet_len,
            &recovered.registration_kx,
            recovered_resolve_plain,
            sizeof(recovered_resolve_plain));

    size_t recovered_resolve_pos =
        0u;

    assert(
        take8(
            recovered_resolve_plain,
            recovered_resolve_plain_len,
            &recovered_resolve_pos) ==
        6u); /* setReturnKey */

    assert(
        take8(
            recovered_resolve_plain,
            recovered_resolve_plain_len,
            &recovered_resolve_pos) ==
        3u); /* HydrogenSecretBox */

    expect_bytes(
        recovered_resolve_plain,
        recovered_resolve_plain_len,
        &recovered_resolve_pos,
        recovered_client.temp_key,
        32u);

    assert(
        take8(
            recovered_resolve_plain,
            recovered_resolve_plain_len,
            &recovered_resolve_pos) ==
        5u); /* resolveServers */

    assert(
        take32(
            recovered_resolve_plain,
            recovered_resolve_plain_len,
            &recovered_resolve_pos) ==
        recovered_client.req_resolve);

    assert(
        take_pack(
            recovered_resolve_plain,
            recovered_resolve_plain_len,
            &recovered_resolve_pos) ==
        1u);

    assert(
        take8(
            recovered_resolve_plain,
            recovered_resolve_plain_len,
            &recovered_resolve_pos) ==
        7u);

    assert(
        take8(
            recovered_resolve_plain,
            recovered_resolve_plain_len,
            &recovered_resolve_pos) ==
        0u);

    assert(
        recovered_resolve_pos ==
        recovered_resolve_plain_len);



    p=0; q=0;
    put8(pow_plain,&q,0); put32(pow_plain,&q,c.req_resolve);
    put_pack(pow_plain,&q,2);          /* ServerDescriptor[] */

    put16(pow_plain,&q,7);             /* server 0 sid */
    put_pack(pow_plain,&q,1);
    put8(pow_plain,&q,1);              /* IPv4 */
    put8(pow_plain,&q,127); put8(pow_plain,&q,0); put8(pow_plain,&q,0); put8(pow_plain,&q,1);
    put_pack(pow_plain,&q,1);
    put8(pow_plain,&q,1);              /* UDP */
    put16(pow_plain,&q,9011);

    put16(pow_plain,&q,8);             /* server 1 sid */
    put_pack(pow_plain,&q,1);
    put8(pow_plain,&q,1);              /* IPv4 */
    put8(pow_plain,&q,127); put8(pow_plain,&q,0); put8(pow_plain,&q,0); put8(pow_plain,&q,2);
    put_pack(pow_plain,&q,1);
    put8(pow_plain,&q,1);              /* UDP */
    put16(pow_plain,&q,9012);


    put_encrypted_packet(
        packet,&p,3,c.temp_key,pow_plain,q);

    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_REGISTRATION,packet,p)==AETHER_OK);

    assert(c.state==AETHER_STATE_WORK_CONNECTING);
    assert(c.registered && f.flash_len>0 && f.last_open_channel==AETHER_CHANNEL_WORK);
    assert(f.last_endpoint.codec==AETHER_CODEC_UDP && f.last_endpoint.port==9011);


    /*
     * transport.open() returning OK is only the beginning of an async connect.
     * If no writable callback ever arrives, poll timeout must close that stale
     * attempt and rotate to the next resolved server.
     */
    unsigned work_timeout_opens_before=f.opens;
    unsigned work_timeout_closes_before=f.closes;
    uint64_t work_timeout_now=c.now_ms+12001u;

    assert(c.active_server_index==0);

    aether_client_poll(&c,work_timeout_now);

    assert(c.state==AETHER_STATE_WORK_CONNECTING);
    assert(f.opens==work_timeout_opens_before+1);
    assert(f.closes==work_timeout_closes_before+1);
    assert(f.last_open_channel==AETHER_CHANNEL_WORK);
    assert(f.last_endpoint.codec==AETHER_CODEC_UDP);
    assert(f.last_endpoint.port==9012);
    assert(c.active_server_index==1);

    /*
     * Preserve the rest of this end-to-end scenario on server 0: server 1
     * reports connect failure, which must rotate back immediately.
     */
    aether_client_on_transport_state(
        &c,AETHER_CHANNEL_WORK,false);

    assert(c.state==AETHER_STATE_WORK_CONNECTING);
    assert(f.last_endpoint.port==9011);
    assert(c.active_server_index==0);


    aether_client_on_transport_state(&c,AETHER_CHANNEL_WORK,true);
    assert(c.state==AETHER_STATE_READY);

    assert(f.packet_channel==AETHER_CHANNEL_WORK); /* immediate ping */


    /*
     * High-bit request ids belong to mandatory internal work traffic.
     * Even a delayed ping result must never become application ingress.
     *
     * Keep this regression self-contained; the normal message test declares
     * its own `inner` buffer later in this function.
     */
    {
        uint8_t ping_inner[16];
        size_t ping_q=0u;
        uint8_t ping_packet[64];
        size_t ping_p=0u;

        put8(
            ping_inner,
            &ping_q,
            0);

        put32(
            ping_inner,
            &ping_q,
            UINT32_C(0x80000001));

        put_encrypted_packet(
            ping_packet,
            &ping_p,
            4,
            c.work_rx_key,
            ping_inner,
            ping_q);

        assert(aether_client_on_rx(
            &c,
            AETHER_CHANNEL_WORK,
            ping_packet,
            ping_p)==AETHER_OK);

        assert(aether_client_ingress(&c)==NULL);
        assert(!aether_client_rx_active(&c));
    }



    uint32_t send_req=0;
    const uint8_t hello[]={'h','i'};
    aether_uuid_t dest={0x4142434445464748ULL,0x5152535455565758ULL};
    assert(aether_client_send_message(&c,dest,hello,sizeof(hello),&send_req)==AETHER_OK);
    assert(send_req!=0 && f.packet[0]==5); /* LoginApi.loginByAlias */


    /*
     * Verify the actual outbound client packet, not a helper round-trip.
     * Server derives the same client->server key and must be able to decrypt
     * AuthorizedApi.sendMessageWithResult.
     */
    size_t outbound_pos=0u;
    assert(take8(f.packet,f.packet_len,&outbound_pos)==5u);
    expect_uuid(f.packet,f.packet_len,&outbound_pos,c.alias);

    size_t outbound_cipher_len=0u;
    const uint8_t *outbound_cipher=
        take_array(f.packet,f.packet_len,&outbound_pos,&outbound_cipher_len);
    assert(outbound_pos==f.packet_len);

    uint8_t outbound_plain[2048];
    size_t outbound_plain_len=0u;
    assert(aether_hydrogen_symmetric_decrypt(
        c.work_tx_key,
        outbound_cipher,
        outbound_cipher_len,
        outbound_plain,
        sizeof(outbound_plain),
        &outbound_plain_len)==0);

    size_t authorized_pos=0u;
    assert(take8(outbound_plain,outbound_plain_len,&authorized_pos)==39u);
    assert(take32(outbound_plain,outbound_plain_len,&authorized_pos)==send_req);
    expect_uuid(outbound_plain,outbound_plain_len,&authorized_pos,dest);

    size_t sent_message_len=0u;
    const uint8_t *sent_message=
        take_array(
            outbound_plain,
            outbound_plain_len,
            &authorized_pos,
            &sent_message_len);

    assert(sent_message_len==sizeof(hello));
    assert(memcmp(sent_message,hello,sizeof(hello))==0);
    assert(authorized_pos==outbound_plain_len);




    /*
     * Server -> ClientApiUnsafe.sendSafeApiData -> ClientApiSafe.sendMessage.
     *
     * Work RX now yields one caller-visible ingress item and pins the borrowed
     * transport frame until the parser is resumed.
     */
    uint8_t inner[128]; q=0;
    aether_uuid_t from={0xa1a2a3a4a5a6a7a8ULL,0xb1b2b3b4b5b6b7b8ULL};
    put8(inner,&q,20);
    put_uuid(inner,&q,from);
    put_array(inner,&q,(const uint8_t*)"abc",3);

    put_encrypted_packet(
        packet,&p,4,c.work_rx_key,inner,q);

    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_WORK,packet,p)==AETHER_OK);

    assert(aether_client_rx_active(&c));

    const aether_ingress_t *ingress=
        aether_client_ingress(&c);

    assert(ingress!=NULL);
    assert(ingress->kind==AETHER_INGRESS_MESSAGE);
    assert(ingress->as.message.from.msb==from.msb);
    assert(ingress->as.message.from.lsb==from.lsb);
    assert(ingress->as.message.payload.length==3u);
    assert(memcmp(
        ingress->as.message.payload.data,
        "abc",
        3u)==0);

    /*
     * Core must not have pushed the message through legacy on_event.
     */
    assert(f.messages==0u);


    aether_client_consume_ingress(&c);
    assert(aether_client_ingress(&c)==NULL);

    /*
     * The single yielded message was also the final item in this frame.
     * Consuming it must release RX ownership immediately; requiring another
     * artificial parser call here would make an immediate reply BUSY.
     */
    assert(!aether_client_rx_active(&c));



    /* Acknowledge sendMessageWithResult over the real work receive key. */
    q=0;
    put8(inner,&q,0);
    put32(inner,&q,send_req);
    put_encrypted_packet(
        packet,&p,4,c.work_rx_key,inner,q);
    assert(aether_client_on_rx(
        &c,AETHER_CHANNEL_WORK,packet,p)==AETHER_OK);

    const aether_ingress_t *send_result=
        aether_client_ingress(&c);

    assert(send_result!=NULL);
    assert(send_result->kind==AETHER_INGRESS_REQUEST_RESULT);
    assert(send_result->as.request_result.request_id==send_req);
    assert(send_result->as.request_result.status==AETHER_OK);
    assert(aether_client_rx_active(&c));
    assert(f.done_events==0u);
    assert(f.errors==0u);

    aether_client_consume_ingress(&c);
    assert(aether_client_ingress(&c)==NULL);
    assert(!aether_client_rx_active(&c));




    /* Losing the active work transport rotates to the next resolved server. */
    unsigned failover_opens_before=f.opens;
    assert(c.active_server_index==0);
    aether_client_on_transport_state(&c,AETHER_CHANNEL_WORK,false);
    assert(c.state==AETHER_STATE_WORK_CONNECTING);
    assert(f.opens==failover_opens_before+1);
    assert(f.last_open_channel==AETHER_CHANNEL_WORK);
    assert(f.last_endpoint.codec==AETHER_CODEC_UDP);
    assert(f.last_endpoint.port==9012);
    assert(c.active_server_index==1);

    aether_client_on_transport_state(&c,AETHER_CHANNEL_WORK,true);
    assert(c.state==AETHER_STATE_READY);

    /*
     * If the next endpoint fails synchronously in transport.open(), the core
     * must continue immediately to the following valid server.
     */
    f.fail_open_port=9011;
    failover_opens_before=f.opens;
    aether_client_on_transport_state(&c,AETHER_CHANNEL_WORK,false);
    assert(c.state==AETHER_STATE_WORK_CONNECTING);
    assert(f.opens==failover_opens_before+2);
    assert(f.last_open_channel==AETHER_CHANNEL_WORK);
    assert(f.last_endpoint.port==9012);
    assert(c.active_server_index==1);
    f.fail_open_port=0;

    aether_client_on_transport_state(&c,AETHER_CHANNEL_WORK,true);
    assert(c.state==AETHER_STATE_READY);



    /*
     * Lost application results no longer consume mandatory core RAM.
     * Timeout belongs to caller-owned future state. The core must remain READY
     * and continue transmitting regardless of how many old results are lost.
     */
    uint32_t lost_req=0;
    assert(aether_client_send_message(&c,dest,hello,sizeof(hello),&lost_req)==AETHER_OK);
    assert(lost_req!=0);

    uint64_t timeout_step=12001u;

    for (unsigned i=0;i<11u;i++) {
        aether_client_poll(&c,c.now_ms+timeout_step);
        assert(c.state==AETHER_STATE_READY);
    }

    assert(f.timeout_events==0u);

    uint32_t after_loss_req=0;
    assert(aether_client_send_message(&c,dest,hello,sizeof(hello),&after_loss_req)==AETHER_OK);
    assert(after_loss_req!=0);





    /* Persisted state must bypass registration on a fresh client object. */
    aether_client_t c2;
    cfg=config_for(&f);
    unsigned opens_before=f.opens;
    aether_client_init(&c2,&cfg);
    assert(aether_client_start(&c2)==AETHER_OK);
    assert(c2.state==AETHER_STATE_WORK_CONNECTING);
    assert(f.opens==opens_before+1 && f.last_open_channel==AETHER_CHANNEL_WORK);
    assert(c2.registered && c2.uid.msb==uid.msb && c2.alias.lsb==alias.lsb);
}



static void test_work_frame_yields_messages_one_by_one(void) {
    fixture_t f;
    memset(&f,0,sizeof(f));

    assert(aether_hydrogen_init()==0);
    hydro_sign_keygen(&f.root_signer);
    hydro_kx_keygen(&f.registration_kx);
    hydro_kx_keygen(&f.global_kx);

    aether_client_t c;
    aether_client_config_t cfg=config_for(&f);
    aether_client_init(&c,&cfg);

    c.state=AETHER_STATE_READY;

    for (size_t i=0u;i<AETHER_KEY_BYTES;++i) {
        c.work_rx_key[i]=(uint8_t)(i+1u);
        c.work_tx_key[i]=(uint8_t)(0x80u+i);
    }

    uint8_t plain[256];
    size_t q=0u;

    put8(plain,&q,6u); /* ClientApiSafe.sendMessages */
    put_pack(plain,&q,2u);

    aether_uuid_t from1={
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    aether_uuid_t from2={
        UINT64_C(0x2122232425262728),
        UINT64_C(0x3132333435363738)
    };

    put_uuid(plain,&q,from1);
    put_array(plain,&q,(const uint8_t*)"one",3u);

    put_uuid(plain,&q,from2);
    put_array(plain,&q,(const uint8_t*)"two",3u);

    uint8_t packet[2048];
    size_t p=0u;

    put_encrypted_packet(
        packet,
        &p,
        4u,
        c.work_rx_key,
        plain,
        q);

    /*
     * First scheduling step yields exactly the first message.
     */
    assert(aether_client_on_rx(
        &c,
        AETHER_CHANNEL_WORK,
        packet,
        p)==AETHER_OK);

    assert(aether_client_rx_active(&c));

    const aether_ingress_t *first=
        aether_client_ingress(&c);

    assert(first!=NULL);
    assert(first->kind==AETHER_INGRESS_MESSAGE);
    assert(first->as.message.from.msb==from1.msb);
    assert(first->as.message.from.lsb==from1.lsb);
    assert(first->as.message.payload.length==3u);
    assert(memcmp(
        first->as.message.payload.data,
        "one",
        3u)==0);

    /*
     * Parser cannot advance while the caller still owns the ingress view.
     */
    assert(aether_client_on_rx(
        &c,
        AETHER_CHANNEL_WORK,
        packet,
        p)==AETHER_ERR_BUSY);

    aether_client_consume_ingress(&c);

    /*
     * A second message still exists, so the borrowed frame must remain pinned.
     */
    assert(aether_client_ingress(&c)==NULL);
    assert(aether_client_rx_active(&c));

    /*
     * Next scheduling step resumes the same borrowed frame and yields only the
     * second message.
     */
    assert(aether_client_on_rx(
        &c,
        AETHER_CHANNEL_WORK,
        packet,
        p)==AETHER_OK);

    const aether_ingress_t *second=
        aether_client_ingress(&c);

    assert(second!=NULL);
    assert(second->kind==AETHER_INGRESS_MESSAGE);
    assert(second->as.message.from.msb==from2.msb);
    assert(second->as.message.from.lsb==from2.lsb);
    assert(second->as.message.payload.length==3u);
    assert(memcmp(
        second->as.message.payload.data,
        "two",
        3u)==0);

    assert(aether_client_rx_active(&c));

    aether_client_consume_ingress(&c);

    /*
     * The second message is the final safe item and the final outer block.
     * No extra parser tick is necessary solely to release RX ownership.
     */
    assert(aether_client_ingress(&c)==NULL);
    assert(!aether_client_rx_active(&c));

    /*
     * Regression for the real public-P2P failure:
     * application code may send immediately after consuming the last message.
     */
    static const uint8_t reply[]={
        9u,8u,7u
    };

    uint32_t reply_request=0u;

    assert(aether_client_send_message(
        &c,
        from2,
        reply,
        sizeof(reply),
        &reply_request)==AETHER_OK);

    assert(reply_request!=0u);
}




static void make_persisted_identity(
    fixture_t *f,
    aether_uuid_t uid,
    aether_uuid_t alias,
    const uint8_t key[AETHER_KEY_BYTES]) {
    uint8_t blob[256];
    size_t p = 0u;

    put8(blob, &p, 'A'); put8(blob, &p, 'E');
    put8(blob, &p, 'C'); put8(blob, &p, '1');
    put8(blob, &p, 1u);                 /* version */
    put8(blob, &p, 1u);                 /* crypto_lib = Hydrogen */
    put8(blob, &p, 1u);                 /* registered */
    put8(blob, &p, 0u);                 /* reserved */

    put_uuid(blob, &p, uid);
    put_uuid(blob, &p, alias);

    for (size_t i = 0u; i < AETHER_KEY_BYTES; ++i) {
        put8(blob, &p, key[i]);
    }

    put8(blob, &p, 0u);                 /* cloud_count = 0 */
    put8(blob, &p, 0u);                 /* server_count = 0 */

    uint32_t crc = test_crc32(blob, p);
    put32(blob, &p, crc);

    memcpy(f->flash, blob, p);
    f->flash_len = p;
}


static void recovery_exchange_server_key(
    aether_client_t *c,
    fixture_t *f) {
    aether_client_on_transport_state(
        c, AETHER_CHANNEL_REGISTRATION, true);
    assert(c->state == AETHER_STATE_RECOVERY_WAIT_SERVER_KEY);

    uint8_t packet[2048];
    size_t p = 0u;
    put8(packet, &p, 0);
    put32(packet, &p, c->req_server_key);
    put_signed_hydrogen_key(
        packet, &p, f->registration_kx.pk, &f->root_signer);

    assert(aether_client_on_rx(
        c, AETHER_CHANNEL_REGISTRATION, packet, p) == AETHER_OK);
    assert(c->state == AETHER_STATE_RECOVERY_WAIT_RESULT);
}


static aether_status_t recovery_exchange_result(
    aether_client_t *c,
    aether_uuid_t new_alias) {
    uint8_t plain[1024];
    size_t q = 0u;
    put8(plain, &q, 0);                       /* CMD_RESULT */
    put32(plain, &q, c->req_resolve);         /* request id */
    put_uuid(plain, &q, new_alias);
    put_pack(plain, &q, 1);                   /* cloud count */
    put16(plain, &q, 9);                      /* cloud sid */

    put_pack(plain, &q, 1);                   /* ServerDescriptor[] */
    put16(plain, &q, 9);                      /* sid */
    put_pack(plain, &q, 1);                   /* addresses */
    put8(plain, &q, 1);                       /* IPv4 */
    put8(plain, &q, 127); put8(plain, &q, 0); put8(plain, &q, 0); put8(plain, &q, 3);
    put_pack(plain, &q, 1);                   /* codecs */
    put8(plain, &q, 1);                       /* UDP */
    put16(plain, &q, 9500);

    uint8_t enc[2048];
    size_t enc_len = 0u;
    put_encrypted_packet(
        enc, &enc_len, 3, c->temp_key, plain, q);

    return aether_client_on_rx(
        c, AETHER_CHANNEL_REGISTRATION, enc, enc_len);
}


static void test_recovery_response_updates_routing_not_identity(void) {
    fixture_t f;
    memset(&f, 0, sizeof(f));
    assert(aether_hydrogen_init() == 0);
    hydro_sign_keygen(&f.root_signer);
    hydro_kx_keygen(&f.registration_kx);

    aether_uuid_t uid = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };
    aether_uuid_t old_alias = {
        UINT64_C(0x3132333435363738),
        UINT64_C(0x4142434445464748)
    };

    uint8_t key[AETHER_KEY_BYTES];
    for (size_t i = 0u; i < sizeof(key); ++i) {
        key[i] = (uint8_t)(0x40u + i);
    }

    /*
     * Start from an ALREADY REGISTERED persisted identity. Recovery here is a
     * topology refresh of an existing client, not initial credential
     * provisioning.
     */
    make_persisted_identity(&f, uid, old_alias, key);

    aether_client_config_t cfg = config_for(&f);
    cfg.pow.generate = NULL;

    aether_client_t c;
    aether_client_init(&c, &cfg);


    assert(aether_client_start(&c) == AETHER_OK);
    assert(c.registered);
    assert(c.uid.msb == uid.msb && c.uid.lsb == uid.lsb);
    assert(c.state == AETHER_STATE_RECOVERY_CONNECTING);


    /* No Cloud in the persisted state: escalate to recovery immediately. */
    assert(c.state == AETHER_STATE_RECOVERY_CONNECTING);
    assert(f.registered_events == 0u);

    recovery_exchange_server_key(&c, &f);

    aether_uuid_t new_alias = {
        UINT64_C(0x2122232425262728),
        UINT64_C(0x5152535455565758)
    };

    assert(recovery_exchange_result(&c, new_alias) == AETHER_OK);

    /* identity must be preserved */
    assert(c.uid.msb == uid.msb && c.uid.lsb == uid.lsb);
    assert(memcmp(c.master_key, key, sizeof(key)) == 0);

    /* routing must be updated */
    assert(c.alias.msb == new_alias.msb && c.alias.lsb == new_alias.lsb);
    assert(c.cloud_count == 1u);
    assert(c.cloud_sids[0] == 9);
    assert(c.server_count == 1u);
    assert(c.servers[0].sid == 9);
    assert(f.flash_len > 0u);
    assert(c.state == AETHER_STATE_WORK_CONNECTING);

    /* Recovery never creates an identity: no REGISTERED event. */
    assert(f.registered_events == 0u);
}





static void test_recovery_save_failure_is_not_committed(void) {
    fixture_t f;
    memset(&f, 0, sizeof(f));
    assert(aether_hydrogen_init() == 0);
    hydro_sign_keygen(&f.root_signer);
    hydro_kx_keygen(&f.registration_kx);

    aether_client_config_t cfg = config_for(&f);
    cfg.pow.generate = NULL;

    aether_client_t c;
    aether_client_init(&c, &cfg);

    aether_uuid_t uid = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    uint8_t key[AETHER_KEY_BYTES];
    for (size_t i = 0u; i < sizeof(key); ++i) {
        key[i] = (uint8_t)(0x40u + i);
    }

    assert(aether_set_credentials(&c, uid, key) == AETHER_OK);
    assert(aether_client_start(&c) == AETHER_OK);
    assert(c.state == AETHER_STATE_RECOVERY_CONNECTING);

    recovery_exchange_server_key(&c, &f);

    aether_uuid_t new_alias = {
        UINT64_C(0x2122232425262728),
        UINT64_C(0x3132333435363738)
    };

    unsigned errors_before = f.errors;

    /*
     * Force persistence to fail at recovery commit time.
     */
    f.fail_flash_saves = 1u;

    assert(recovery_exchange_result(&c, new_alias) == AETHER_ERR_STORAGE);

    assert(c.state == AETHER_STATE_ERROR);
    assert(f.errors == errors_before + 1u);
    assert(f.last_error_code == AETHER_ERR_STORAGE);

    /* identity must be preserved and not marked committed */
    assert(c.uid.msb == uid.msb && c.uid.lsb == uid.lsb);
    assert(memcmp(c.master_key, key, sizeof(key)) == 0);
    assert(!c.registered);

    /*
     * Explicit retry must re-run authoritative recovery, not jump to WORK.
     */
    assert(aether_client_retry(&c) == AETHER_OK);
    assert(c.state == AETHER_STATE_RECOVERY_CONNECTING);
    assert(f.last_open_channel == AETHER_CHANNEL_REGISTRATION);

    assert(f.registered_events == 0u);

    /*
     * Complete the second recovery handshake. Persistence now succeeds.
     */
    recovery_exchange_server_key(&c, &f);

    assert(recovery_exchange_result(&c, new_alias) == AETHER_OK);

    assert(c.registered);
    assert(c.uid.msb == uid.msb && c.uid.lsb == uid.lsb);
    assert(memcmp(c.master_key, key, sizeof(key)) == 0);
    assert(c.cloud_count == 1u);
    assert(c.cloud_sids[0] == 9);
    assert(c.server_count == 1u);
    assert(c.state == AETHER_STATE_WORK_CONNECTING);

    /* Recovery never creates an identity: no REGISTERED event. */
    assert(f.registered_events == 0u);
}






static void test_set_credentials_first_time(void) {
    aether_client_t c;
    memset(&c, 0, sizeof(c));
    c.state = AETHER_STATE_STOPPED;

    aether_uuid_t uid = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    uint8_t key[AETHER_KEY_BYTES];
    for (size_t i = 0u; i < sizeof(key); ++i) {
        key[i] = (uint8_t)(0x40u + i);
    }

    assert(aether_set_credentials(&c, uid, key) == AETHER_OK);
    assert(c.uid.msb == uid.msb && c.uid.lsb == uid.lsb);
    assert(memcmp(c.master_key, key, sizeof(key)) == 0);
}



static void test_persisted_state_wins_over_caller_candidate(void) {
    fixture_t f;
    memset(&f, 0, sizeof(f));
    assert(aether_hydrogen_init() == 0);
    hydro_sign_keygen(&f.root_signer);

    aether_client_config_t cfg = config_for(&f);
    cfg.pow.generate = NULL;

    /*
     * Produce a valid persisted identity whose master key is B.
     */
    aether_client_t writer;
    aether_client_init(&writer, &cfg);

    assert(aether_client_start(&writer) == AETHER_OK);
    assert(writer.state == AETHER_STATE_NO_IDENTITY);


    aether_uuid_t uid = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    aether_uuid_t alias = {
        UINT64_C(0x2122232425262728),
        UINT64_C(0x3132333435363738)
    };

    uint8_t key_b[AETHER_KEY_BYTES];
    for (size_t i = 0u; i < sizeof(key_b); ++i) {
        key_b[i] = (uint8_t)(0x80u + i);
    }

    const int16_t cloud[] = {7};

    assert(aether_client_provision_identity(
        &writer, uid, alias, key_b, cloud, 1u) == AETHER_OK);
    assert(f.flash_len > 0u);

    /*
     * Fresh boot: caller supplies a different candidate key A before start.
     * load_state() must let persisted key B win.
     */
    aether_client_t c;
    aether_client_init(&c, &cfg);

    uint8_t key_a[AETHER_KEY_BYTES];
    for (size_t i = 0u; i < sizeof(key_a); ++i) {
        key_a[i] = (uint8_t)(0x10u + i);
    }

    assert(aether_set_credentials(&c, uid, key_a) == AETHER_OK);

    assert(aether_client_start(&c) == AETHER_OK);
    assert(c.registered);
    assert(memcmp(c.master_key, key_b, sizeof(key_b)) == 0);
    assert(c.uid.msb == uid.msb && c.uid.lsb == uid.lsb);

    /*
     * A repeated call after identity load must not roll the key back.
     */
    assert(aether_set_credentials(&c, uid, key_a) == AETHER_OK);
    assert(memcmp(c.master_key, key_b, sizeof(key_b)) == 0);
}



static void test_set_credentials_does_not_overwrite_existing(void) {
    aether_client_t c;
    memset(&c, 0, sizeof(c));
    c.state = AETHER_STATE_STOPPED;

    aether_uuid_t uid_a = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    uint8_t key_a[AETHER_KEY_BYTES];
    uint8_t key_b[AETHER_KEY_BYTES];
    for (size_t i = 0u; i < AETHER_KEY_BYTES; ++i) {
        key_a[i] = (uint8_t)(0x10u + i);
        key_b[i] = (uint8_t)(0x80u + i);
    }

    assert(aether_set_credentials(&c, uid_a, key_a) == AETHER_OK);

    aether_uuid_t uid_b = {
        UINT64_C(0xa1a2a3a4a5a6a7a8),
        UINT64_C(0xb1b2b3b4b5b6b7b8)
    };

    assert(aether_set_credentials(&c, uid_b, key_b) == AETHER_OK);
    assert(c.uid.msb == uid_a.msb && c.uid.lsb == uid_a.lsb);
    assert(memcmp(c.master_key, key_a, sizeof(key_a)) == 0);
}


static void test_caller_credentials_start_enters_recovery(void) {
    fixture_t f;
    memset(&f, 0, sizeof(f));
    assert(aether_hydrogen_init() == 0);
    hydro_sign_keygen(&f.root_signer);

    aether_client_config_t cfg = config_for(&f);
    cfg.pow.generate = NULL;

    aether_client_t c;
    aether_client_init(&c, &cfg);

    aether_uuid_t uid = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };

    uint8_t key[AETHER_KEY_BYTES];
    for (size_t i = 0u; i < sizeof(key); ++i) {
        key[i] = (uint8_t)(0x40u + i);
    }

    assert(aether_set_credentials(&c, uid, key) == AETHER_OK);
    assert(!c.registered);

    assert(aether_client_start(&c) == AETHER_OK);
    assert(c.state == AETHER_STATE_RECOVERY_CONNECTING);
    assert(f.opens == 1u);
    assert(f.last_open_channel == AETHER_CHANNEL_REGISTRATION);
    assert(f.errors == 0u);
    assert(c.uid.msb == uid.msb && c.uid.lsb == uid.lsb);
    assert(memcmp(c.master_key, key, sizeof(key)) == 0);
}



static void test_cached_servers_exhausted_escalates_to_recovery(void) {
    fixture_t f;
    memset(&f, 0, sizeof(f));
    assert(aether_hydrogen_init() == 0);
    hydro_sign_keygen(&f.root_signer);
    hydro_kx_keygen(&f.registration_kx);

    aether_client_config_t cfg = config_for(&f);
    cfg.pow.generate = NULL;

    aether_client_t c;
    aether_client_init(&c, &cfg);

    c.registered = true;
    c.state = AETHER_STATE_READY;
    c.cloud_count = 1u;
    c.cloud_sids[0] = 7;

    /*
     * One stale cached descriptor for the known Cloud. It is valid, so the
     * first work connection attempt targets it, but its endpoint is refused by
     * the platform transport.
     */
    c.server_count = 1u;
    c.active_server_index = -1;
    c.servers[0].valid = true;
    c.servers[0].sid = 7;
    c.servers[0].endpoint.codec = AETHER_CODEC_UDP;
    c.servers[0].endpoint.address.kind = AETHER_ADDR_IPV4;
    c.servers[0].endpoint.address.length = 4u;
    c.servers[0].endpoint.address.bytes[0] = 127u;
    c.servers[0].endpoint.address.bytes[1] = 0u;
    c.servers[0].endpoint.address.bytes[2] = 0u;
    c.servers[0].endpoint.address.bytes[3] = 1u;
    c.servers[0].endpoint.port = 9011u;

    for (size_t i = 0u; i < AETHER_KEY_BYTES; ++i) {
        c.master_key[i] = (uint8_t)(0x20u + i);
    }

    assert(!c.topology_refreshed);

    /*
     * A resolveServers refresh was NOT yet consumed for this Cloud. Exhausting
     * the cached descriptor must first trigger the cheap resolveServers path,
     * not recovery.
     */
    f.fail_open_port = 9011u;
    aether_client_on_transport_state(&c, AETHER_CHANNEL_WORK, false);

    assert(c.state == AETHER_STATE_SERVER_RESOLVING);
    assert(f.last_open_channel == AETHER_CHANNEL_REGISTRATION);

    /*
     * Registration channel becomes writable -> getServerKey.
     */
    aether_client_on_transport_state(
        &c, AETHER_CHANNEL_REGISTRATION, true);
    assert(c.state == AETHER_STATE_SERVER_RESOLVING);

    uint8_t packet[2048];
    size_t p = 0u;
    put8(packet, &p, 0);
    put32(packet, &p, c.req_server_key);
    put_signed_hydrogen_key(
        packet, &p, f.registration_kx.pk, &f.root_signer);
    assert(aether_client_on_rx(
        &c, AETHER_CHANNEL_REGISTRATION, packet, p) == AETHER_OK);

    /*
     * Server responds to resolveServers with the SAME single stale descriptor.
     * This is the real transition that must set topology_refreshed.
     */
    uint8_t plain[1024];
    size_t q = 0u;
    put8(plain, &q, 0);
    put32(plain, &q, c.req_resolve);
    put_pack(plain, &q, 1);                   /* ServerDescriptor[] */
    put16(plain, &q, 7);                      /* sid */
    put_pack(plain, &q, 1);                   /* addresses */
    put8(plain, &q, 1);                       /* IPv4 */
    put8(plain, &q, 127); put8(plain, &q, 0); put8(plain, &q, 0); put8(plain, &q, 1);
    put_pack(plain, &q, 1);                   /* codecs */
    put8(plain, &q, 1);                       /* UDP */
    put16(plain, &q, 9011);

    uint8_t enc[2048];
    size_t enc_len = 0u;
    put_encrypted_packet(
        enc, &enc_len, 3, c.temp_key, plain, q);


    /*
     * The refreshed descriptor is now reachable, so the first work attempt
     * succeeds synchronously in transport.open().
     */
    f.fail_open_port = 0u;

    assert(aether_client_on_rx(
        &c, AETHER_CHANNEL_REGISTRATION, enc, enc_len) == AETHER_OK);

    /*
     * The refresh response was consumed: the flag must now be set by core
     * itself, not by the test.
     */
    assert(c.topology_refreshed);
    assert(c.state == AETHER_STATE_WORK_CONNECTING);

    /*
     * The refreshed descriptor endpoint is then refused. Every valid server
     * fails to open, so the refreshed topology is exhausted and core must
     * escalate to mandatory recovery instead of repeating resolveServers.
     */
    f.fail_open_port = 9011u;
    aether_client_on_transport_state(&c, AETHER_CHANNEL_WORK, false);

    assert(c.state == AETHER_STATE_RECOVERY_CONNECTING);
    assert(f.last_open_channel == AETHER_CHANNEL_REGISTRATION);
}



static void test_persisted_identity_empty_cloud_enters_recovery(void) {
    fixture_t f;
    memset(&f, 0, sizeof(f));
    assert(aether_hydrogen_init() == 0);
    hydro_sign_keygen(&f.root_signer);

    /*
     * Craft a persisted identity blob directly: registered=true, valid UID and
     * master key, but cloud_count == 0. This is the "identity exists, routing
     * is lost" case that mandatory recovery must handle without registration.
     */
    uint8_t blob[256];
    size_t p = 0u;

    put8(blob, &p, 'A'); put8(blob, &p, 'E');
    put8(blob, &p, 'C'); put8(blob, &p, '1');
    put8(blob, &p, 1u);                 /* version */
    put8(blob, &p, 1u);                 /* crypto_lib = Hydrogen */
    put8(blob, &p, 1u);                 /* registered */
    put8(blob, &p, 0u);                 /* reserved */

    aether_uuid_t uid = {
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718)
    };
    aether_uuid_t alias = {
        UINT64_C(0x2122232425262728),
        UINT64_C(0x3132333435363738)
    };
    put_uuid(blob, &p, uid);
    put_uuid(blob, &p, alias);

    for (size_t i = 0u; i < AETHER_KEY_BYTES; ++i) {
        put8(blob, &p, (uint8_t)(0x50u + i));
    }

    put8(blob, &p, 0u);                 /* cloud_count = 0 */
    put8(blob, &p, 0u);                 /* server_count = 0 */

    uint32_t crc = test_crc32(blob, p);
    put32(blob, &p, crc);

    memcpy(f.flash, blob, p);
    f.flash_len = p;

    aether_client_config_t cfg = config_for(&f);
    cfg.pow.generate = NULL;

    aether_client_t c;
    aether_client_init(&c, &cfg);

    assert(aether_client_start(&c) == AETHER_OK);
    assert(c.registered);
    assert(c.uid.msb == uid.msb && c.uid.lsb == uid.lsb);
    assert(c.cloud_count == 0u);

    /* No Cloud means the cheap resolveServers path is impossible: recovery. */
    assert(c.state == AETHER_STATE_RECOVERY_CONNECTING);
    assert(f.last_open_channel == AETHER_CHANNEL_REGISTRATION);
    assert(f.errors == 0u);
}








int main(void) {
    test_work_frame_yields_messages_one_by_one();
    test_full_registration_and_message();

    test_set_credentials_first_time();
    test_set_credentials_does_not_overwrite_existing();
    test_caller_credentials_start_enters_recovery();
    test_cached_servers_exhausted_escalates_to_recovery();
    test_persisted_state_wins_over_caller_candidate();
    test_persisted_identity_empty_cloud_enters_recovery();
    test_recovery_response_updates_routing_not_identity();
    test_recovery_save_failure_is_not_committed();

    puts("aether_client_test: OK");
    return 0;
}