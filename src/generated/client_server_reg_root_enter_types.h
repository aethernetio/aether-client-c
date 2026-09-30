#ifndef CLIENTSERVERREGROOTENTER_TYPES_H
#define CLIENTSERVERREGROOTENTER_TYPES_H

#include "aether_client.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} client_api_reg_safe_stream_t;

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} global_reg_client_api_stream_t;

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} global_api_stream_t;

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} server_registration_api_stream_t;

typedef enum {
    crypto_lib_SODIUM = 0,
    crypto_lib_HYDROGEN = 1,
    crypto_lib_P256_AES_GCM = 2
} crypto_lib_t;

typedef enum {
    pow_method_AE_BCRYPT_CRC32 = 0
} pow_method_t;

typedef enum {
    adsl_aether_codec_TCP = 0,
    adsl_aether_codec_UDP = 1,
    adsl_aether_codec_WS = 2,
    adsl_aether_codec_WSS = 3
} adsl_aether_codec_t;

typedef enum {
    status_PENDING = 0,
    status_COMPLETED = 1,
    status_FAILED = 2
} status_t;

typedef enum {
    server_type_REG = 0,
    server_type_WORK = 1
} server_type_t;

typedef struct sodium_chacha20_poly1305_t sodium_chacha20_poly1305_t;
typedef struct hydrogen_curve_private_t hydrogen_curve_private_t;
typedef struct hydrogen_curve_public_t hydrogen_curve_public_t;
typedef struct hydrogen_secret_box_t hydrogen_secret_box_t;
typedef struct sodium_curve_public_t sodium_curve_public_t;
typedef struct sodium_curve_private_t sodium_curve_private_t;
typedef struct sodium_sign_private_t sodium_sign_private_t;
typedef struct sodium_sign_public_t sodium_sign_public_t;
typedef struct hydrogen_sign_private_t hydrogen_sign_private_t;
typedef struct hydrogen_sign_public_t hydrogen_sign_public_t;
typedef struct sign_a_e_e_d25519_t sign_a_e_e_d25519_t;
typedef struct sign_h_y_d_r_o_g_e_n_t sign_h_y_d_r_o_g_e_n_t;
typedef struct signed_key_t signed_key_t;
typedef struct work_proof_b_crypt_t work_proof_b_crypt_t;
typedef struct work_proof_d_t_o_t work_proof_d_t_o_t;
typedef struct i_p_address_v4_t i_p_address_v4_t;
typedef struct i_p_address_v6_t i_p_address_v6_t;
typedef struct i_p_address_web_t i_p_address_web_t;
typedef struct coder_and_port_t coder_and_port_t;
typedef struct i_p_address_and_ports_t i_p_address_and_ports_t;
typedef struct i_p_address_and_ports_list_t i_p_address_and_ports_list_t;
typedef struct server_descriptor_t server_descriptor_t;
typedef struct server_descriptor_with_geo_t server_descriptor_with_geo_t;
typedef struct cloud_t cloud_t;
typedef struct money_operation_t money_operation_t;
typedef struct access_group_t access_group_t;
typedef struct u_u_i_d_and_cloud_t u_u_i_d_and_cloud_t;
typedef struct cloud_config_t cloud_config_t;
typedef struct applied_config_t applied_config_t;
typedef struct pair_keys_sign_t pair_keys_sign_t;
typedef struct pair_keys_asym_t pair_keys_asym_t;
typedef struct pair_keys_asym_signed_t pair_keys_asym_signed_t;
typedef struct pair_keys_sym_t pair_keys_sym_t;
typedef struct access_check_pair_t access_check_pair_t;
typedef struct access_check_result_t access_check_result_t;
typedef struct ip_info_t ip_info_t;
typedef struct p256_aes_gcm_symmetric_t p256_aes_gcm_symmetric_t;
typedef struct p256_aes_gcm_curve_private_t p256_aes_gcm_curve_private_t;
typedef struct p256_aes_gcm_curve_public_t p256_aes_gcm_curve_public_t;
typedef struct p256_aes_gcm_sign_private_t p256_aes_gcm_sign_private_t;
typedef struct p256_aes_gcm_sign_public_t p256_aes_gcm_sign_public_t;
typedef struct sign_p256_aes_gcm_t sign_p256_aes_gcm_t;

#define KEY_TYPE_SODIUM_CHACHA20_POLY1305 UINT8_C(6)
#define KEY_TYPE_HYDROGEN_CURVE_PRIVATE UINT8_C(1)
#define KEY_TYPE_HYDROGEN_CURVE_PUBLIC UINT8_C(2)
#define KEY_TYPE_HYDROGEN_SECRET_BOX UINT8_C(3)
#define KEY_TYPE_SODIUM_CURVE_PUBLIC UINT8_C(8)
#define KEY_TYPE_SODIUM_CURVE_PRIVATE UINT8_C(7)
#define KEY_TYPE_SODIUM_SIGN_PRIVATE UINT8_C(9)
#define KEY_TYPE_SODIUM_SIGN_PUBLIC UINT8_C(10)
#define KEY_TYPE_HYDROGEN_SIGN_PRIVATE UINT8_C(4)
#define KEY_TYPE_HYDROGEN_SIGN_PUBLIC UINT8_C(5)
#define KEY_TYPE_P256_AES_GCM_SYMMETRIC UINT8_C(11)
#define KEY_TYPE_P256_AES_GCM_CURVE_PRIVATE UINT8_C(12)
#define KEY_TYPE_P256_AES_GCM_CURVE_PUBLIC UINT8_C(13)
#define KEY_TYPE_P256_AES_GCM_SIGN_PRIVATE UINT8_C(14)
#define KEY_TYPE_P256_AES_GCM_SIGN_PUBLIC UINT8_C(15)

typedef struct {
    uint8_t type_id;
    union {
        const sodium_chacha20_poly1305_t *sodium_chacha20_poly1305;
        const hydrogen_curve_private_t *hydrogen_curve_private;
        const hydrogen_curve_public_t *hydrogen_curve_public;
        const hydrogen_secret_box_t *hydrogen_secret_box;
        const sodium_curve_public_t *sodium_curve_public;
        const sodium_curve_private_t *sodium_curve_private;
        const sodium_sign_private_t *sodium_sign_private;
        const sodium_sign_public_t *sodium_sign_public;
        const hydrogen_sign_private_t *hydrogen_sign_private;
        const hydrogen_sign_public_t *hydrogen_sign_public;
        const p256_aes_gcm_symmetric_t *p256_aes_gcm_symmetric;
        const p256_aes_gcm_curve_private_t *p256_aes_gcm_curve_private;
        const p256_aes_gcm_curve_public_t *p256_aes_gcm_curve_public;
        const p256_aes_gcm_sign_private_t *p256_aes_gcm_sign_private;
        const p256_aes_gcm_sign_public_t *p256_aes_gcm_sign_public;
    } as;
} key_ref_t;

#define KEY_SYMMETRIC_TYPE_SODIUM_CHACHA20_POLY1305 UINT8_C(6)
#define KEY_SYMMETRIC_TYPE_HYDROGEN_SECRET_BOX UINT8_C(3)
#define KEY_SYMMETRIC_TYPE_P256_AES_GCM_SYMMETRIC UINT8_C(11)

typedef struct {
    uint8_t type_id;
    union {
        const sodium_chacha20_poly1305_t *sodium_chacha20_poly1305;
        const hydrogen_secret_box_t *hydrogen_secret_box;
        const p256_aes_gcm_symmetric_t *p256_aes_gcm_symmetric;
    } as;
} key_symmetric_ref_t;

#define KEY_ASYMMETRIC_TYPE_HYDROGEN_CURVE_PRIVATE UINT8_C(1)
#define KEY_ASYMMETRIC_TYPE_HYDROGEN_CURVE_PUBLIC UINT8_C(2)
#define KEY_ASYMMETRIC_TYPE_SODIUM_CURVE_PUBLIC UINT8_C(8)
#define KEY_ASYMMETRIC_TYPE_SODIUM_CURVE_PRIVATE UINT8_C(7)
#define KEY_ASYMMETRIC_TYPE_P256_AES_GCM_CURVE_PRIVATE UINT8_C(12)
#define KEY_ASYMMETRIC_TYPE_P256_AES_GCM_CURVE_PUBLIC UINT8_C(13)

typedef struct {
    uint8_t type_id;
    union {
        const hydrogen_curve_private_t *hydrogen_curve_private;
        const hydrogen_curve_public_t *hydrogen_curve_public;
        const sodium_curve_public_t *sodium_curve_public;
        const sodium_curve_private_t *sodium_curve_private;
        const p256_aes_gcm_curve_private_t *p256_aes_gcm_curve_private;
        const p256_aes_gcm_curve_public_t *p256_aes_gcm_curve_public;
    } as;
} key_asymmetric_ref_t;

#define KEY_ASYMMETRIC_PUBLIC_TYPE_HYDROGEN_CURVE_PUBLIC UINT8_C(2)
#define KEY_ASYMMETRIC_PUBLIC_TYPE_SODIUM_CURVE_PUBLIC UINT8_C(8)
#define KEY_ASYMMETRIC_PUBLIC_TYPE_P256_AES_GCM_CURVE_PUBLIC UINT8_C(13)

typedef struct {
    uint8_t type_id;
    union {
        const hydrogen_curve_public_t *hydrogen_curve_public;
        const sodium_curve_public_t *sodium_curve_public;
        const p256_aes_gcm_curve_public_t *p256_aes_gcm_curve_public;
    } as;
} key_asymmetric_public_ref_t;

#define KEY_ASYMMETRIC_PRIVATE_TYPE_HYDROGEN_CURVE_PRIVATE UINT8_C(1)
#define KEY_ASYMMETRIC_PRIVATE_TYPE_SODIUM_CURVE_PRIVATE UINT8_C(7)
#define KEY_ASYMMETRIC_PRIVATE_TYPE_P256_AES_GCM_CURVE_PRIVATE UINT8_C(12)

typedef struct {
    uint8_t type_id;
    union {
        const hydrogen_curve_private_t *hydrogen_curve_private;
        const sodium_curve_private_t *sodium_curve_private;
        const p256_aes_gcm_curve_private_t *p256_aes_gcm_curve_private;
    } as;
} key_asymmetric_private_ref_t;

#define KEY_SIGN_TYPE_SODIUM_SIGN_PRIVATE UINT8_C(9)
#define KEY_SIGN_TYPE_SODIUM_SIGN_PUBLIC UINT8_C(10)
#define KEY_SIGN_TYPE_HYDROGEN_SIGN_PRIVATE UINT8_C(4)
#define KEY_SIGN_TYPE_HYDROGEN_SIGN_PUBLIC UINT8_C(5)
#define KEY_SIGN_TYPE_P256_AES_GCM_SIGN_PRIVATE UINT8_C(14)
#define KEY_SIGN_TYPE_P256_AES_GCM_SIGN_PUBLIC UINT8_C(15)

typedef struct {
    uint8_t type_id;
    union {
        const sodium_sign_private_t *sodium_sign_private;
        const sodium_sign_public_t *sodium_sign_public;
        const hydrogen_sign_private_t *hydrogen_sign_private;
        const hydrogen_sign_public_t *hydrogen_sign_public;
        const p256_aes_gcm_sign_private_t *p256_aes_gcm_sign_private;
        const p256_aes_gcm_sign_public_t *p256_aes_gcm_sign_public;
    } as;
} key_sign_ref_t;

#define KEY_SIGN_PUBLIC_TYPE_SODIUM_SIGN_PUBLIC UINT8_C(10)
#define KEY_SIGN_PUBLIC_TYPE_HYDROGEN_SIGN_PUBLIC UINT8_C(5)
#define KEY_SIGN_PUBLIC_TYPE_P256_AES_GCM_SIGN_PUBLIC UINT8_C(15)

typedef struct {
    uint8_t type_id;
    union {
        const sodium_sign_public_t *sodium_sign_public;
        const hydrogen_sign_public_t *hydrogen_sign_public;
        const p256_aes_gcm_sign_public_t *p256_aes_gcm_sign_public;
    } as;
} key_sign_public_ref_t;

#define KEY_SIGN_PRIVATE_TYPE_SODIUM_SIGN_PRIVATE UINT8_C(9)
#define KEY_SIGN_PRIVATE_TYPE_HYDROGEN_SIGN_PRIVATE UINT8_C(4)
#define KEY_SIGN_PRIVATE_TYPE_P256_AES_GCM_SIGN_PRIVATE UINT8_C(14)

typedef struct {
    uint8_t type_id;
    union {
        const sodium_sign_private_t *sodium_sign_private;
        const hydrogen_sign_private_t *hydrogen_sign_private;
        const p256_aes_gcm_sign_private_t *p256_aes_gcm_sign_private;
    } as;
} key_sign_private_ref_t;

#define SIGN_TYPE_SIGN_A_E_E_D25519 UINT8_C(1)
#define SIGN_TYPE_SIGN_H_Y_D_R_O_G_E_N UINT8_C(2)
#define SIGN_TYPE_SIGN_P256_AES_GCM UINT8_C(3)

typedef struct {
    uint8_t type_id;
    union {
        const sign_a_e_e_d25519_t *sign_a_e_e_d25519;
        const sign_h_y_d_r_o_g_e_n_t *sign_h_y_d_r_o_g_e_n;
        const sign_p256_aes_gcm_t *sign_p256_aes_gcm;
    } as;
} sign_ref_t;

#define WORK_PROOF_CONFIG_TYPE_WORK_PROOF_B_CRYPT UINT8_C(1)

typedef struct {
    uint8_t type_id;
    union {
        const work_proof_b_crypt_t *work_proof_b_crypt;
    } as;
} work_proof_config_ref_t;

#define I_P_ADDRESS_TYPE_I_P_ADDRESS_V4 UINT8_C(1)
#define I_P_ADDRESS_TYPE_I_P_ADDRESS_V6 UINT8_C(2)
#define I_P_ADDRESS_TYPE_I_P_ADDRESS_WEB UINT8_C(3)

typedef struct {
    uint8_t type_id;
    union {
        const i_p_address_v4_t *i_p_address_v4;
        const i_p_address_v6_t *i_p_address_v6;
        const i_p_address_web_t *i_p_address_web;
    } as;
} i_p_address_ref_t;

#define PAIR_KEYS_TYPE_PAIR_KEYS_SIGN UINT8_C(3)
#define PAIR_KEYS_TYPE_PAIR_KEYS_ASYM UINT8_C(1)
#define PAIR_KEYS_TYPE_PAIR_KEYS_ASYM_SIGNED UINT8_C(2)
#define PAIR_KEYS_TYPE_PAIR_KEYS_SYM UINT8_C(4)

typedef struct {
    uint8_t type_id;
    union {
        const pair_keys_sign_t *pair_keys_sign;
        const pair_keys_asym_t *pair_keys_asym;
        const pair_keys_asym_signed_t *pair_keys_asym_signed;
        const pair_keys_sym_t *pair_keys_sym;
    } as;
} pair_keys_ref_t;

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
typedef struct {
    int8_t data[64];
} byte_array_64_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
typedef struct {
    int8_t data[64];
} byte_array_64_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
typedef struct {
    int8_t data[64];
} byte_array_64_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
typedef struct {
    int8_t data[64];
} byte_array_64_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_4_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_4_T_DEFINED
typedef struct {
    int8_t data[4];
} byte_array_4_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_16_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_16_T_DEFINED
typedef struct {
    int8_t data[16];
} byte_array_16_t;
#endif

#ifndef AETHER_ADSLC_CODER_AND_PORT_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_CODER_AND_PORT_ARRAY_VIEW_T_DEFINED
typedef struct {
    const coder_and_port_t *data;
    size_t length;
} coder_and_port_array_view_t;
#endif

#ifndef AETHER_ADSLC_I_P_ADDRESS_AND_PORTS_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_I_P_ADDRESS_AND_PORTS_ARRAY_VIEW_T_DEFINED
typedef struct {
    const i_p_address_and_ports_t *data;
    size_t length;
} i_p_address_and_ports_array_view_t;
#endif

#ifndef AETHER_ADSLC_SHORT_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SHORT_ARRAY_VIEW_T_DEFINED
typedef struct {
    const int16_t *data;
    size_t length;
} short_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
typedef struct {
    int8_t data[64];
} byte_array_64_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_32_T_DEFINED
typedef struct {
    int8_t data[32];
} byte_array_32_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
typedef struct {
    int8_t data[64];
} byte_array_64_t;
#endif

#ifndef AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
#define AETHER_ADSLC_BYTE_ARRAY_64_T_DEFINED
typedef struct {
    int8_t data[64];
} byte_array_64_t;
#endif

#ifndef AETHER_ADSLC_INT_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_INT_ARRAY_VIEW_T_DEFINED
typedef struct {
    const int32_t *data;
    size_t length;
} int_array_view_t;
#endif

#ifndef AETHER_ADSLC_SERVER_DESCRIPTOR_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SERVER_DESCRIPTOR_ARRAY_VIEW_T_DEFINED
typedef struct {
    const server_descriptor_t *data;
    size_t length;
} server_descriptor_array_view_t;
#endif

struct sodium_chacha20_poly1305_t {
    byte_array_32_t data;
};

struct hydrogen_curve_private_t {
    byte_array_32_t data;
};

struct hydrogen_curve_public_t {
    byte_array_32_t data;
};

struct hydrogen_secret_box_t {
    byte_array_32_t data;
};

struct sodium_curve_public_t {
    byte_array_32_t data;
};

struct sodium_curve_private_t {
    byte_array_32_t data;
};

struct sodium_sign_private_t {
    byte_array_64_t data;
};

struct sodium_sign_public_t {
    byte_array_32_t data;
};

struct hydrogen_sign_private_t {
    byte_array_64_t data;
};

struct hydrogen_sign_public_t {
    byte_array_32_t data;
};

struct sign_a_e_e_d25519_t {
    byte_array_64_t data;
};

struct sign_h_y_d_r_o_g_e_n_t {
    byte_array_64_t data;
};

struct signed_key_t {
    key_ref_t key;
    sign_ref_t sign;
};

struct work_proof_b_crypt_t {
    int8_t costBCrypt;
    int8_t poolSize;
    int32_t maxHashVal;
};

struct work_proof_d_t_o_t {
    aether_bytes_view_t salt;
    aether_bytes_view_t suffix;
    int8_t poolSize;
    int32_t maxHashVal;
    signed_key_t globalKey;
};

struct i_p_address_v4_t {
    byte_array_4_t data;
};

struct i_p_address_v6_t {
    byte_array_16_t data;
};

struct i_p_address_web_t {
    aether_bytes_view_t data;
};

struct coder_and_port_t {
    adsl_aether_codec_t codec;
    int16_t port;
};

struct i_p_address_and_ports_t {
    i_p_address_ref_t address;
    coder_and_port_array_view_t coderAndPorts;
};

struct i_p_address_and_ports_list_t {
    i_p_address_and_ports_array_view_t addresses;
};

struct server_descriptor_t {
    int16_t id;
    i_p_address_and_ports_list_t ipAddress;
};

struct server_descriptor_with_geo_t {
    int16_t id;
    int32_t time;
    i_p_address_and_ports_list_t ipAddress;
    double latitude;
    double longitude;
    server_type_t type;
};

struct cloud_t {
    short_array_view_t data;
};

struct money_operation_t {
    int64_t id;
    aether_uuid_t from;
    aether_uuid_t to;
    int64_t amount;
    int64_t time;
    bool credit;
    status_t status;
};

struct access_group_t {
    aether_uuid_t id;
    int32_t time;
    aether_uuid_t owner;
    uuid_array_view_t data;
};

struct u_u_i_d_and_cloud_t {
    aether_uuid_t uid;
    cloud_t cloud;
};

struct cloud_config_t {
    aether_uuid_t subjectUid;
    int64_t configVersion;
    cloud_t cloud;
};

struct applied_config_t {
    aether_uuid_t subjectUid;
    int64_t configVersion;
};

struct pair_keys_sign_t {
    key_ref_t privateKey;
    key_ref_t publicKey;
};

struct pair_keys_asym_t {
    key_ref_t privateKey;
    key_ref_t publicKey;
};

struct pair_keys_asym_signed_t {
    key_ref_t privateKey;
    signed_key_t publicKey;
};

struct pair_keys_sym_t {
    key_symmetric_ref_t clientToServer;
    key_symmetric_ref_t serverToClient;
};

struct access_check_pair_t {
    aether_uuid_t sourceUid;
    aether_uuid_t targetUid;
};

struct access_check_result_t {
    aether_uuid_t sourceUid;
    aether_uuid_t targetUid;
    bool hasAccess;
};

struct ip_info_t {
    i_p_address_ref_t ip;
    int16_t port;
    double latitude;
    double longitude;
};

struct p256_aes_gcm_symmetric_t {
    byte_array_32_t data;
};

struct p256_aes_gcm_curve_private_t {
    byte_array_32_t data;
};

struct p256_aes_gcm_curve_public_t {
    byte_array_64_t data;
};

struct p256_aes_gcm_sign_private_t {
    byte_array_32_t data;
};

struct p256_aes_gcm_sign_public_t {
    byte_array_64_t data;
};

struct sign_p256_aes_gcm_t {
    byte_array_64_t data;
};

#endif
