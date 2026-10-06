#ifndef CLIENTSERVERAPI_TYPES_H
#define CLIENTSERVERAPI_TYPES_H

#include "aether_meta_runtime.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} login_stream_t;

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} login_client_stream_t;

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} client_interaction_client_stream_t;

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t length;
} client_api_stream_t;

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

typedef enum {
    client_activity_type_CONNECT = 0,
    client_activity_type_DISCONNECT = 1,
    client_activity_type_MESSAGE_SEND = 2,
    client_activity_type_MESSAGE_RECEIVE = 3,
    client_activity_type_COMMAND_EXEC = 4,
    client_activity_type_API_CALL = 5
} client_activity_type_t;

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
typedef struct peer_identity_data_t peer_identity_data_t;
typedef struct peer_identity_t peer_identity_t;
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
typedef struct telemetry_c_p_p_t telemetry_c_p_p_t;
typedef struct message_t message_t;
typedef struct client_activity_t client_activity_t;
typedef struct key_value_pair_t key_value_pair_t;
typedef struct client_log_entry_t client_log_entry_t;
typedef struct client_connection_info_t client_connection_info_t;
typedef struct message_info_t message_info_t;
typedef struct uap_t uap_t;
typedef struct ice_candidate_t ice_candidate_t;
typedef struct web_rtc_session_t web_rtc_session_t;
typedef struct client_timing_t client_timing_t;
typedef struct probe_sample_t probe_sample_t;
typedef struct probe_report_t probe_report_t;
typedef struct finish_result_t finish_result_t;
typedef struct registration_direct_result_t registration_direct_result_t;
typedef struct recovery_result_t recovery_result_t;

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

#define TELEMETRY_TYPE_TELEMETRY_C_P_P UINT8_C(1)

typedef struct {
    uint8_t type_id;
    union {
        const telemetry_c_p_p_t *telemetry_c_p_p;
    } as;
} telemetry_ref_t;

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

#ifndef AETHER_ADSLC_SIGNED_KEY_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SIGNED_KEY_ARRAY_VIEW_T_DEFINED
typedef struct {
    const signed_key_t *data;
    size_t length;
} signed_key_array_view_t;
#endif

#ifndef AETHER_ADSLC_SIGNED_KEY_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SIGNED_KEY_ARRAY_VIEW_T_DEFINED
typedef struct {
    const signed_key_t *data;
    size_t length;
} signed_key_array_view_t;
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

#ifndef AETHER_ADSLC_KEY_VALUE_PAIR_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_KEY_VALUE_PAIR_ARRAY_VIEW_T_DEFINED
typedef struct {
    const key_value_pair_t *data;
    size_t length;
} key_value_pair_array_view_t;
#endif

#ifndef AETHER_ADSLC_KEY_VALUE_PAIR_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_KEY_VALUE_PAIR_ARRAY_VIEW_T_DEFINED
typedef struct {
    const key_value_pair_t *data;
    size_t length;
} key_value_pair_array_view_t;
#endif

#ifndef AETHER_ADSLC_ICE_CANDIDATE_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_ICE_CANDIDATE_ARRAY_VIEW_T_DEFINED
typedef struct {
    const ice_candidate_t *data;
    size_t length;
} ice_candidate_array_view_t;
#endif

#ifndef AETHER_ADSLC_PROBE_SAMPLE_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_PROBE_SAMPLE_ARRAY_VIEW_T_DEFINED
typedef struct {
    const probe_sample_t *data;
    size_t length;
} probe_sample_array_view_t;
#endif

#ifndef AETHER_ADSLC_SERVER_DESCRIPTOR_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SERVER_DESCRIPTOR_ARRAY_VIEW_T_DEFINED
typedef struct {
    const server_descriptor_t *data;
    size_t length;
} server_descriptor_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_MESSAGE_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_MESSAGE_ARRAY_VIEW_T_DEFINED
typedef struct {
    const message_t *data;
    size_t length;
} message_array_view_t;
#endif

#ifndef AETHER_ADSLC_SERVER_DESCRIPTOR_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SERVER_DESCRIPTOR_ARRAY_VIEW_T_DEFINED
typedef struct {
    const server_descriptor_t *data;
    size_t length;
} server_descriptor_array_view_t;
#endif

#ifndef AETHER_ADSLC_U_U_I_D_AND_CLOUD_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_U_U_I_D_AND_CLOUD_ARRAY_VIEW_T_DEFINED
typedef struct {
    const u_u_i_d_and_cloud_t *data;
    size_t length;
} u_u_i_d_and_cloud_array_view_t;
#endif

#ifndef AETHER_ADSLC_ACCESS_GROUP_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_ACCESS_GROUP_ARRAY_VIEW_T_DEFINED
typedef struct {
    const access_group_t *data;
    size_t length;
} access_group_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_ACCESS_CHECK_RESULT_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_ACCESS_CHECK_RESULT_ARRAY_VIEW_T_DEFINED
typedef struct {
    const access_check_result_t *data;
    size_t length;
} access_check_result_array_view_t;
#endif

#ifndef AETHER_ADSLC_CLOUD_CONFIG_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_CLOUD_CONFIG_ARRAY_VIEW_T_DEFINED
typedef struct {
    const cloud_config_t *data;
    size_t length;
} cloud_config_array_view_t;
#endif

#ifndef AETHER_ADSLC_MESSAGE_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_MESSAGE_ARRAY_VIEW_T_DEFINED
typedef struct {
    const message_t *data;
    size_t length;
} message_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
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

#ifndef AETHER_ADSLC_APPLIED_CONFIG_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_APPLIED_CONFIG_ARRAY_VIEW_T_DEFINED
typedef struct {
    const applied_config_t *data;
    size_t length;
} applied_config_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_UUID_ARRAY_VIEW_T_DEFINED
typedef struct {
    const aether_uuid_t *data;
    size_t length;
} uuid_array_view_t;
#endif

#ifndef AETHER_ADSLC_ACCESS_CHECK_PAIR_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_ACCESS_CHECK_PAIR_ARRAY_VIEW_T_DEFINED
typedef struct {
    const access_check_pair_t *data;
    size_t length;
} access_check_pair_array_view_t;
#endif

#ifndef AETHER_ADSLC_CLIENT_ACTIVITY_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_CLIENT_ACTIVITY_ARRAY_VIEW_T_DEFINED
typedef struct {
    const client_activity_t *data;
    size_t length;
} client_activity_array_view_t;
#endif

#ifndef AETHER_ADSLC_CLIENT_LOG_ENTRY_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_CLIENT_LOG_ENTRY_ARRAY_VIEW_T_DEFINED
typedef struct {
    const client_log_entry_t *data;
    size_t length;
} client_log_entry_array_view_t;
#endif

#ifndef AETHER_ADSLC_CLIENT_CONNECTION_INFO_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_CLIENT_CONNECTION_INFO_ARRAY_VIEW_T_DEFINED
typedef struct {
    const client_connection_info_t *data;
    size_t length;
} client_connection_info_array_view_t;
#endif

#ifndef AETHER_ADSLC_MESSAGE_INFO_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_MESSAGE_INFO_ARRAY_VIEW_T_DEFINED
typedef struct {
    const message_info_t *data;
    size_t length;
} message_info_array_view_t;
#endif

#ifndef AETHER_ADSLC_SERVER_DESCRIPTOR_WITH_GEO_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SERVER_DESCRIPTOR_WITH_GEO_ARRAY_VIEW_T_DEFINED
typedef struct {
    const server_descriptor_with_geo_t *data;
    size_t length;
} server_descriptor_with_geo_array_view_t;
#endif

#ifndef AETHER_ADSLC_SHORT_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SHORT_ARRAY_VIEW_T_DEFINED
typedef struct {
    const int16_t *data;
    size_t length;
} short_array_view_t;
#endif

#ifndef AETHER_ADSLC_SHORT_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_SHORT_ARRAY_VIEW_T_DEFINED
typedef struct {
    const int16_t *data;
    size_t length;
} short_array_view_t;
#endif

#ifndef AETHER_ADSLC_INT_ARRAY_VIEW_T_DEFINED
#define AETHER_ADSLC_INT_ARRAY_VIEW_T_DEFINED
typedef struct {
    const int32_t *data;
    size_t length;
} int_array_view_t;
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

struct peer_identity_data_t {
    aether_uuid_t uid;
    key_sign_public_ref_t key;
};

struct peer_identity_t {
    peer_identity_data_t data;
    signed_key_t serverSigningKey;
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
    signed_key_array_view_t signingKeys;
};

struct server_descriptor_with_geo_t {
    int16_t id;
    int32_t time;
    i_p_address_and_ports_list_t ipAddress;
    double latitude;
    double longitude;
    server_type_t type;
    signed_key_array_view_t signingKeys;
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

struct telemetry_c_p_p_t {
    int32_t utm_id;
    aether_bytes_view_t blob;
    aether_bytes_view_t lib_version;
    aether_bytes_view_t os;
    aether_bytes_view_t compiler;
};

struct message_t {
    aether_uuid_t uid;
    aether_bytes_view_t data;
};

struct client_activity_t {
    int64_t timestamp;
    client_activity_type_t type;
    aether_uuid_t clientUid;
    aether_bytes_view_t details;
    const int16_t *serverId;
};

struct key_value_pair_t {
    aether_bytes_view_t key;
    aether_bytes_view_t value;
};

struct client_log_entry_t {
    int64_t timestamp;
    aether_bytes_view_t level;
    aether_bytes_view_t message;
    key_value_pair_array_view_t context;
};

struct client_connection_info_t {
    int64_t connectedAt;
    const int64_t *disconnectedAt;
    int16_t serverId;
    key_value_pair_array_view_t context;
    aether_bytes_view_t protocol;
};

struct message_info_t {
    int64_t timestamp;
    aether_uuid_t fromUid;
    aether_uuid_t toUid;
    int32_t size;
    aether_uuid_t messageId;
};

struct uap_t {
    int64_t deltaMs;
    int64_t lastReadTimestamp;
};

struct ice_candidate_t {
    aether_bytes_view_t data;
};

struct web_rtc_session_t {
    aether_bytes_view_t sdp;
    ice_candidate_array_view_t candidates;
};

struct client_timing_t {
    int64_t lastActivityDeltaMs;
    int64_t nextPingDeltaMs;
};

struct probe_sample_t {
    int32_t sequence;
    int64_t receivedAtMs;
};

struct probe_report_t {
    int32_t testId;
    int32_t firstSequence;
    int32_t count;
    probe_sample_array_view_t samples;
};

struct finish_result_t {
    aether_uuid_t alias;
    aether_uuid_t uid;
    cloud_t cloud;
};

struct registration_direct_result_t {
    aether_uuid_t alias;
    aether_uuid_t uid;
    cloud_t cloud;
};

struct recovery_result_t {
    aether_uuid_t alias;
    cloud_t cloud;
    server_descriptor_array_view_t servers;
};

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:3 types.Key */
aether_status_t client_server_api_serialize_key(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_ref_t *value);

aether_status_t client_server_api_deserialize_key(
    aether_meta_read_context_t *ctx,
    key_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:5 types.KeySymmetric */
aether_status_t client_server_api_serialize_key_symmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_symmetric_ref_t *value);

aether_status_t client_server_api_deserialize_key_symmetric(
    aether_meta_read_context_t *ctx,
    key_symmetric_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:8 types.KeyAsymmetric */
aether_status_t client_server_api_serialize_key_asymmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_ref_t *value);

aether_status_t client_server_api_deserialize_key_asymmetric(
    aether_meta_read_context_t *ctx,
    key_asymmetric_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:11 types.KeyAsymmetricPublic */
aether_status_t client_server_api_serialize_key_asymmetric_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_public_ref_t *value);

aether_status_t client_server_api_deserialize_key_asymmetric_public(
    aether_meta_read_context_t *ctx,
    key_asymmetric_public_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:14 types.KeyAsymmetricPrivate */
aether_status_t client_server_api_serialize_key_asymmetric_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_private_ref_t *value);

aether_status_t client_server_api_deserialize_key_asymmetric_private(
    aether_meta_read_context_t *ctx,
    key_asymmetric_private_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:17 types.KeySign */
aether_status_t client_server_api_serialize_key_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_ref_t *value);

aether_status_t client_server_api_deserialize_key_sign(
    aether_meta_read_context_t *ctx,
    key_sign_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:20 types.KeySignPublic */
aether_status_t client_server_api_serialize_key_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_public_ref_t *value);

aether_status_t client_server_api_deserialize_key_sign_public(
    aether_meta_read_context_t *ctx,
    key_sign_public_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:23 types.KeySignPrivate */
aether_status_t client_server_api_serialize_key_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_private_ref_t *value);

aether_status_t client_server_api_deserialize_key_sign_private(
    aether_meta_read_context_t *ctx,
    key_sign_private_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:26 types.SodiumChacha20Poly1305 */
aether_status_t client_server_api_serialize_sodium_chacha20_poly1305(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_chacha20_poly1305_t *value);

aether_status_t client_server_api_deserialize_sodium_chacha20_poly1305(
    aether_meta_read_context_t *ctx,
    sodium_chacha20_poly1305_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:31 types.HydrogenCurvePrivate */
aether_status_t client_server_api_serialize_hydrogen_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_private_t *value);

aether_status_t client_server_api_deserialize_hydrogen_curve_private(
    aether_meta_read_context_t *ctx,
    hydrogen_curve_private_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:36 types.HydrogenCurvePublic */
aether_status_t client_server_api_serialize_hydrogen_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_public_t *value);

aether_status_t client_server_api_deserialize_hydrogen_curve_public(
    aether_meta_read_context_t *ctx,
    hydrogen_curve_public_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:41 types.HydrogenSecretBox */
aether_status_t client_server_api_serialize_hydrogen_secret_box(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_secret_box_t *value);

aether_status_t client_server_api_deserialize_hydrogen_secret_box(
    aether_meta_read_context_t *ctx,
    hydrogen_secret_box_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:46 types.SodiumCurvePublic */
aether_status_t client_server_api_serialize_sodium_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_public_t *value);

aether_status_t client_server_api_deserialize_sodium_curve_public(
    aether_meta_read_context_t *ctx,
    sodium_curve_public_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:51 types.SodiumCurvePrivate */
aether_status_t client_server_api_serialize_sodium_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_private_t *value);

aether_status_t client_server_api_deserialize_sodium_curve_private(
    aether_meta_read_context_t *ctx,
    sodium_curve_private_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:56 types.SodiumSignPrivate */
aether_status_t client_server_api_serialize_sodium_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_private_t *value);

aether_status_t client_server_api_deserialize_sodium_sign_private(
    aether_meta_read_context_t *ctx,
    sodium_sign_private_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:61 types.SodiumSignPublic */
aether_status_t client_server_api_serialize_sodium_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_public_t *value);

aether_status_t client_server_api_deserialize_sodium_sign_public(
    aether_meta_read_context_t *ctx,
    sodium_sign_public_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:66 types.HydrogenSignPrivate */
aether_status_t client_server_api_serialize_hydrogen_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_private_t *value);

aether_status_t client_server_api_deserialize_hydrogen_sign_private(
    aether_meta_read_context_t *ctx,
    hydrogen_sign_private_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:71 types.HydrogenSignPublic */
aether_status_t client_server_api_serialize_hydrogen_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_public_t *value);

aether_status_t client_server_api_deserialize_hydrogen_sign_public(
    aether_meta_read_context_t *ctx,
    hydrogen_sign_public_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:76 types.Sign */
aether_status_t client_server_api_serialize_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_ref_t *value);

aether_status_t client_server_api_deserialize_sign(
    aether_meta_read_context_t *ctx,
    sign_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:78 types.SignAE_ED25519 */
aether_status_t client_server_api_serialize_sign_a_e_e_d25519(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_a_e_e_d25519_t *value);

aether_status_t client_server_api_deserialize_sign_a_e_e_d25519(
    aether_meta_read_context_t *ctx,
    sign_a_e_e_d25519_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:83 types.SignHYDROGEN */
aether_status_t client_server_api_serialize_sign_h_y_d_r_o_g_e_n(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_h_y_d_r_o_g_e_n_t *value);

aether_status_t client_server_api_deserialize_sign_h_y_d_r_o_g_e_n(
    aether_meta_read_context_t *ctx,
    sign_h_y_d_r_o_g_e_n_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:88 types.SignedKey */
aether_status_t client_server_api_serialize_signed_key(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const signed_key_t *value);

aether_status_t client_server_api_deserialize_signed_key(
    aether_meta_read_context_t *ctx,
    signed_key_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:93 types.PeerIdentityData */
aether_status_t client_server_api_serialize_peer_identity_data(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_identity_data_t *value);

aether_status_t client_server_api_deserialize_peer_identity_data(
    aether_meta_read_context_t *ctx,
    peer_identity_data_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:97 types.PeerIdentity */
aether_status_t client_server_api_serialize_peer_identity(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_identity_t *value);

aether_status_t client_server_api_deserialize_peer_identity(
    aether_meta_read_context_t *ctx,
    peer_identity_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:111 types.WorkProofConfig */
aether_status_t client_server_api_serialize_work_proof_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_config_ref_t *value);

aether_status_t client_server_api_deserialize_work_proof_config(
    aether_meta_read_context_t *ctx,
    work_proof_config_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:113 types.WorkProofBCrypt */
aether_status_t client_server_api_serialize_work_proof_b_crypt(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_b_crypt_t *value);

aether_status_t client_server_api_deserialize_work_proof_b_crypt(
    aether_meta_read_context_t *ctx,
    work_proof_b_crypt_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:120 types.WorkProofDTO */
aether_status_t client_server_api_serialize_work_proof_d_t_o(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_d_t_o_t *value);

aether_status_t client_server_api_deserialize_work_proof_d_t_o(
    aether_meta_read_context_t *ctx,
    work_proof_d_t_o_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:127 types.IPAddress */
aether_status_t client_server_api_serialize_i_p_address(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_ref_t *value);

aether_status_t client_server_api_deserialize_i_p_address(
    aether_meta_read_context_t *ctx,
    i_p_address_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:129 types.IPAddressV4 */
aether_status_t client_server_api_serialize_i_p_address_v4(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v4_t *value);

aether_status_t client_server_api_deserialize_i_p_address_v4(
    aether_meta_read_context_t *ctx,
    i_p_address_v4_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:134 types.IPAddressV6 */
aether_status_t client_server_api_serialize_i_p_address_v6(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v6_t *value);

aether_status_t client_server_api_deserialize_i_p_address_v6(
    aether_meta_read_context_t *ctx,
    i_p_address_v6_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:139 types.IPAddressWeb */
aether_status_t client_server_api_serialize_i_p_address_web(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_web_t *value);

aether_status_t client_server_api_deserialize_i_p_address_web(
    aether_meta_read_context_t *ctx,
    i_p_address_web_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:150 types.CoderAndPort */
aether_status_t client_server_api_serialize_coder_and_port(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const coder_and_port_t *value);

aether_status_t client_server_api_deserialize_coder_and_port(
    aether_meta_read_context_t *ctx,
    coder_and_port_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:154 types.IPAddressAndPorts */
aether_status_t client_server_api_serialize_i_p_address_and_ports(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_t *value);

aether_status_t client_server_api_deserialize_i_p_address_and_ports(
    aether_meta_read_context_t *ctx,
    i_p_address_and_ports_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:158 types.IPAddressAndPortsList */
aether_status_t client_server_api_serialize_i_p_address_and_ports_list(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_list_t *value);

aether_status_t client_server_api_deserialize_i_p_address_and_ports_list(
    aether_meta_read_context_t *ctx,
    i_p_address_and_ports_list_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:161 types.ServerDescriptor */
aether_status_t client_server_api_serialize_server_descriptor(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_t *value);

aether_status_t client_server_api_deserialize_server_descriptor(
    aether_meta_read_context_t *ctx,
    server_descriptor_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:166 types.ServerDescriptorWithGeo */
aether_status_t client_server_api_serialize_server_descriptor_with_geo(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_with_geo_t *value);

aether_status_t client_server_api_deserialize_server_descriptor_with_geo(
    aether_meta_read_context_t *ctx,
    server_descriptor_with_geo_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:175 types.Cloud */
aether_status_t client_server_api_serialize_cloud(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_t *value);

aether_status_t client_server_api_deserialize_cloud(
    aether_meta_read_context_t *ctx,
    cloud_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:183 types.MoneyOperation */
aether_status_t client_server_api_serialize_money_operation(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const money_operation_t *value);

aether_status_t client_server_api_deserialize_money_operation(
    aether_meta_read_context_t *ctx,
    money_operation_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:192 types.AccessGroup */
aether_status_t client_server_api_serialize_access_group(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_group_t *value);

aether_status_t client_server_api_deserialize_access_group(
    aether_meta_read_context_t *ctx,
    access_group_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:198 types.UUIDAndCloud */
aether_status_t client_server_api_serialize_u_u_i_d_and_cloud(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const u_u_i_d_and_cloud_t *value);

aether_status_t client_server_api_deserialize_u_u_i_d_and_cloud(
    aether_meta_read_context_t *ctx,
    u_u_i_d_and_cloud_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:203 types.CloudConfig */
aether_status_t client_server_api_serialize_cloud_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_config_t *value);

aether_status_t client_server_api_deserialize_cloud_config(
    aether_meta_read_context_t *ctx,
    cloud_config_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:209 types.AppliedConfig */
aether_status_t client_server_api_serialize_applied_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const applied_config_t *value);

aether_status_t client_server_api_deserialize_applied_config(
    aether_meta_read_context_t *ctx,
    applied_config_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:216 types.PairKeys */
aether_status_t client_server_api_serialize_pair_keys(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_ref_t *value);

aether_status_t client_server_api_deserialize_pair_keys(
    aether_meta_read_context_t *ctx,
    pair_keys_ref_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:218 types.PairKeysSign */
aether_status_t client_server_api_serialize_pair_keys_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sign_t *value);

aether_status_t client_server_api_deserialize_pair_keys_sign(
    aether_meta_read_context_t *ctx,
    pair_keys_sign_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:224 types.PairKeysAsym */
aether_status_t client_server_api_serialize_pair_keys_asym(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_t *value);

aether_status_t client_server_api_deserialize_pair_keys_asym(
    aether_meta_read_context_t *ctx,
    pair_keys_asym_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:230 types.PairKeysAsymSigned */
aether_status_t client_server_api_serialize_pair_keys_asym_signed(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_signed_t *value);

aether_status_t client_server_api_deserialize_pair_keys_asym_signed(
    aether_meta_read_context_t *ctx,
    pair_keys_asym_signed_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:236 types.PairKeysSym */
aether_status_t client_server_api_serialize_pair_keys_sym(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sym_t *value);

aether_status_t client_server_api_deserialize_pair_keys_sym(
    aether_meta_read_context_t *ctx,
    pair_keys_sym_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:242 types.AccessCheckPair */
aether_status_t client_server_api_serialize_access_check_pair(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_pair_t *value);

aether_status_t client_server_api_deserialize_access_check_pair(
    aether_meta_read_context_t *ctx,
    access_check_pair_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:246 types.AccessCheckResult */
aether_status_t client_server_api_serialize_access_check_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_result_t *value);

aether_status_t client_server_api_deserialize_access_check_result(
    aether_meta_read_context_t *ctx,
    access_check_result_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:252 types.IpInfo */
aether_status_t client_server_api_serialize_ip_info(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ip_info_t *value);

aether_status_t client_server_api_deserialize_ip_info(
    aether_meta_read_context_t *ctx,
    ip_info_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:262 types.P256AesGcmSymmetric */
aether_status_t client_server_api_serialize_p256_aes_gcm_symmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_symmetric_t *value);

aether_status_t client_server_api_deserialize_p256_aes_gcm_symmetric(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_symmetric_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:267 types.P256AesGcmCurvePrivate */
aether_status_t client_server_api_serialize_p256_aes_gcm_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_private_t *value);

aether_status_t client_server_api_deserialize_p256_aes_gcm_curve_private(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_curve_private_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:272 types.P256AesGcmCurvePublic */
aether_status_t client_server_api_serialize_p256_aes_gcm_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_public_t *value);

aether_status_t client_server_api_deserialize_p256_aes_gcm_curve_public(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_curve_public_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:277 types.P256AesGcmSignPrivate */
aether_status_t client_server_api_serialize_p256_aes_gcm_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_private_t *value);

aether_status_t client_server_api_deserialize_p256_aes_gcm_sign_private(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_sign_private_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:282 types.P256AesGcmSignPublic */
aether_status_t client_server_api_serialize_p256_aes_gcm_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_public_t *value);

aether_status_t client_server_api_deserialize_p256_aes_gcm_sign_public(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_sign_public_t *value);

/* ADSL: /home/angel/projects/aether/common-api/src/main/resources/common.adsl.yaml:287 types.SignP256AesGcm */
aether_status_t client_server_api_serialize_sign_p256_aes_gcm(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_p256_aes_gcm_t *value);

aether_status_t client_server_api_deserialize_sign_p256_aes_gcm(
    aether_meta_read_context_t *ctx,
    sign_p256_aes_gcm_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:7 types.Telemetry */
aether_status_t client_server_api_serialize_telemetry(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const telemetry_ref_t *value);

aether_status_t client_server_api_deserialize_telemetry(
    aether_meta_read_context_t *ctx,
    telemetry_ref_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:9 types.TelemetryCPP */
aether_status_t client_server_api_serialize_telemetry_c_p_p(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const telemetry_c_p_p_t *value);

aether_status_t client_server_api_deserialize_telemetry_c_p_p(
    aether_meta_read_context_t *ctx,
    telemetry_c_p_p_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:37 types.Message */
aether_status_t client_server_api_serialize_message(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const message_t *value);

aether_status_t client_server_api_deserialize_message(
    aether_meta_read_context_t *ctx,
    message_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:53 types.ClientActivity */
aether_status_t client_server_api_serialize_client_activity(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_activity_t *value);

aether_status_t client_server_api_deserialize_client_activity(
    aether_meta_read_context_t *ctx,
    client_activity_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:61 types.KeyValuePair */
aether_status_t client_server_api_serialize_key_value_pair(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_value_pair_t *value);

aether_status_t client_server_api_deserialize_key_value_pair(
    aether_meta_read_context_t *ctx,
    key_value_pair_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:66 types.ClientLogEntry */
aether_status_t client_server_api_serialize_client_log_entry(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_log_entry_t *value);

aether_status_t client_server_api_deserialize_client_log_entry(
    aether_meta_read_context_t *ctx,
    client_log_entry_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:72 types.ClientConnectionInfo */
aether_status_t client_server_api_serialize_client_connection_info(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_connection_info_t *value);

aether_status_t client_server_api_deserialize_client_connection_info(
    aether_meta_read_context_t *ctx,
    client_connection_info_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:79 types.MessageInfo */
aether_status_t client_server_api_serialize_message_info(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const message_info_t *value);

aether_status_t client_server_api_deserialize_message_info(
    aether_meta_read_context_t *ctx,
    message_info_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:88 types.Uap */
aether_status_t client_server_api_serialize_uap(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const uap_t *value);

aether_status_t client_server_api_deserialize_uap(
    aether_meta_read_context_t *ctx,
    uap_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:94 types.IceCandidate */
aether_status_t client_server_api_serialize_ice_candidate(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ice_candidate_t *value);

aether_status_t client_server_api_deserialize_ice_candidate(
    aether_meta_read_context_t *ctx,
    ice_candidate_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:97 types.WebRtcSession */
aether_status_t client_server_api_serialize_web_rtc_session(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const web_rtc_session_t *value);

aether_status_t client_server_api_deserialize_web_rtc_session(
    aether_meta_read_context_t *ctx,
    web_rtc_session_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:102 types.ClientTiming */
aether_status_t client_server_api_serialize_client_timing(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_timing_t *value);

aether_status_t client_server_api_deserialize_client_timing(
    aether_meta_read_context_t *ctx,
    client_timing_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:107 types.ProbeSample */
aether_status_t client_server_api_serialize_probe_sample(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const probe_sample_t *value);

aether_status_t client_server_api_deserialize_probe_sample(
    aether_meta_read_context_t *ctx,
    probe_sample_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:111 types.ProbeReport */
aether_status_t client_server_api_serialize_probe_report(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const probe_report_t *value);

aether_status_t client_server_api_deserialize_probe_report(
    aether_meta_read_context_t *ctx,
    probe_report_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:118 types.FinishResult */
aether_status_t client_server_api_serialize_finish_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const finish_result_t *value);

aether_status_t client_server_api_deserialize_finish_result(
    aether_meta_read_context_t *ctx,
    finish_result_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:124 types.RegistrationDirectResult */
aether_status_t client_server_api_serialize_registration_direct_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const registration_direct_result_t *value);

aether_status_t client_server_api_deserialize_registration_direct_result(
    aether_meta_read_context_t *ctx,
    registration_direct_result_t *value);

/* ADSL: /home/angel/projects/aether/client-server-work-api/src/main/resources/ClientServerApi.adsl.yaml:130 types.RecoveryResult */
aether_status_t client_server_api_serialize_recovery_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const recovery_result_t *value);

aether_status_t client_server_api_deserialize_recovery_result(
    aether_meta_read_context_t *ctx,
    recovery_result_t *value);

#endif
