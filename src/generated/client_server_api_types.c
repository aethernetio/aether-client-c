#include "client_server_api_types.h"
#include "aether_meta_runtime.h"

#include <string.h>

static aether_status_t client_server_api_serialize_key_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_impl(
    aether_meta_read_context_t *ctx,
    key_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_key_symmetric_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_symmetric_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_symmetric_impl(
    aether_meta_read_context_t *ctx,
    key_symmetric_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_key_asymmetric_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_asymmetric_impl(
    aether_meta_read_context_t *ctx,
    key_asymmetric_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_key_asymmetric_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_public_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_asymmetric_public_impl(
    aether_meta_read_context_t *ctx,
    key_asymmetric_public_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_key_asymmetric_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_private_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_asymmetric_private_impl(
    aether_meta_read_context_t *ctx,
    key_asymmetric_private_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_key_sign_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_sign_impl(
    aether_meta_read_context_t *ctx,
    key_sign_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_key_sign_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_public_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_sign_public_impl(
    aether_meta_read_context_t *ctx,
    key_sign_public_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_key_sign_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_private_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_sign_private_impl(
    aether_meta_read_context_t *ctx,
    key_sign_private_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sodium_chacha20_poly1305_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_chacha20_poly1305_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sodium_chacha20_poly1305_impl(
    aether_meta_read_context_t *ctx,
    sodium_chacha20_poly1305_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_hydrogen_curve_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_hydrogen_curve_private_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_curve_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_hydrogen_curve_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_hydrogen_curve_public_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_curve_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_hydrogen_secret_box_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_secret_box_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_hydrogen_secret_box_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_secret_box_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sodium_curve_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sodium_curve_public_impl(
    aether_meta_read_context_t *ctx,
    sodium_curve_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sodium_curve_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sodium_curve_private_impl(
    aether_meta_read_context_t *ctx,
    sodium_curve_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sodium_sign_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sodium_sign_private_impl(
    aether_meta_read_context_t *ctx,
    sodium_sign_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sodium_sign_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sodium_sign_public_impl(
    aether_meta_read_context_t *ctx,
    sodium_sign_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_hydrogen_sign_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_hydrogen_sign_private_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_sign_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_hydrogen_sign_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_hydrogen_sign_public_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_sign_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sign_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sign_impl(
    aether_meta_read_context_t *ctx,
    sign_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sign_a_e_e_d25519_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_a_e_e_d25519_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sign_a_e_e_d25519_impl(
    aether_meta_read_context_t *ctx,
    sign_a_e_e_d25519_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sign_h_y_d_r_o_g_e_n_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_h_y_d_r_o_g_e_n_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sign_h_y_d_r_o_g_e_n_impl(
    aether_meta_read_context_t *ctx,
    sign_h_y_d_r_o_g_e_n_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_signed_key_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const signed_key_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_signed_key_impl(
    aether_meta_read_context_t *ctx,
    signed_key_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_peer_identity_data_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_identity_data_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_peer_identity_data_impl(
    aether_meta_read_context_t *ctx,
    peer_identity_data_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_peer_identity_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_identity_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_peer_identity_impl(
    aether_meta_read_context_t *ctx,
    peer_identity_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_work_proof_config_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_config_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_work_proof_config_impl(
    aether_meta_read_context_t *ctx,
    work_proof_config_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_work_proof_b_crypt_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_b_crypt_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_work_proof_b_crypt_impl(
    aether_meta_read_context_t *ctx,
    work_proof_b_crypt_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_work_proof_d_t_o_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_d_t_o_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_work_proof_d_t_o_impl(
    aether_meta_read_context_t *ctx,
    work_proof_d_t_o_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_i_p_address_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_i_p_address_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_i_p_address_v4_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v4_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_i_p_address_v4_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_v4_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_i_p_address_v6_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v6_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_i_p_address_v6_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_v6_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_i_p_address_web_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_web_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_i_p_address_web_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_web_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_coder_and_port_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const coder_and_port_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_coder_and_port_impl(
    aether_meta_read_context_t *ctx,
    coder_and_port_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_i_p_address_and_ports_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_i_p_address_and_ports_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_and_ports_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_i_p_address_and_ports_list_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_list_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_i_p_address_and_ports_list_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_and_ports_list_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_server_descriptor_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_server_descriptor_impl(
    aether_meta_read_context_t *ctx,
    server_descriptor_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_server_descriptor_with_geo_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_with_geo_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_server_descriptor_with_geo_impl(
    aether_meta_read_context_t *ctx,
    server_descriptor_with_geo_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_cloud_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_cloud_impl(
    aether_meta_read_context_t *ctx,
    cloud_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_money_operation_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const money_operation_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_money_operation_impl(
    aether_meta_read_context_t *ctx,
    money_operation_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_access_group_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_group_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_access_group_impl(
    aether_meta_read_context_t *ctx,
    access_group_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_u_u_i_d_and_cloud_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const u_u_i_d_and_cloud_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_u_u_i_d_and_cloud_impl(
    aether_meta_read_context_t *ctx,
    u_u_i_d_and_cloud_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_cloud_config_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_config_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_cloud_config_impl(
    aether_meta_read_context_t *ctx,
    cloud_config_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_applied_config_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const applied_config_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_applied_config_impl(
    aether_meta_read_context_t *ctx,
    applied_config_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_pair_keys_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_pair_keys_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_pair_keys_sign_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sign_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_pair_keys_sign_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_sign_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_pair_keys_asym_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_pair_keys_asym_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_asym_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_pair_keys_asym_signed_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_signed_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_pair_keys_asym_signed_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_asym_signed_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_pair_keys_sym_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sym_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_pair_keys_sym_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_sym_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_access_check_pair_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_pair_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_access_check_pair_impl(
    aether_meta_read_context_t *ctx,
    access_check_pair_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_access_check_result_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_result_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_access_check_result_impl(
    aether_meta_read_context_t *ctx,
    access_check_result_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_ip_info_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ip_info_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_ip_info_impl(
    aether_meta_read_context_t *ctx,
    ip_info_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_p256_aes_gcm_symmetric_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_symmetric_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_p256_aes_gcm_symmetric_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_symmetric_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_p256_aes_gcm_curve_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_p256_aes_gcm_curve_private_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_curve_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_p256_aes_gcm_curve_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_p256_aes_gcm_curve_public_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_curve_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_p256_aes_gcm_sign_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_p256_aes_gcm_sign_private_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_sign_private_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_p256_aes_gcm_sign_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_p256_aes_gcm_sign_public_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_sign_public_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_sign_p256_aes_gcm_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_p256_aes_gcm_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_sign_p256_aes_gcm_impl(
    aether_meta_read_context_t *ctx,
    sign_p256_aes_gcm_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_telemetry_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const telemetry_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_telemetry_impl(
    aether_meta_read_context_t *ctx,
    telemetry_ref_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_telemetry_c_p_p_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const telemetry_c_p_p_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_telemetry_c_p_p_impl(
    aether_meta_read_context_t *ctx,
    telemetry_c_p_p_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_message_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const message_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_message_impl(
    aether_meta_read_context_t *ctx,
    message_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_client_activity_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_activity_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_client_activity_impl(
    aether_meta_read_context_t *ctx,
    client_activity_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_key_value_pair_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_value_pair_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_key_value_pair_impl(
    aether_meta_read_context_t *ctx,
    key_value_pair_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_client_log_entry_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_log_entry_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_client_log_entry_impl(
    aether_meta_read_context_t *ctx,
    client_log_entry_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_client_connection_info_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_connection_info_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_client_connection_info_impl(
    aether_meta_read_context_t *ctx,
    client_connection_info_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_message_info_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const message_info_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_message_info_impl(
    aether_meta_read_context_t *ctx,
    message_info_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_uap_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const uap_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_uap_impl(
    aether_meta_read_context_t *ctx,
    uap_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_ice_candidate_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ice_candidate_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_ice_candidate_impl(
    aether_meta_read_context_t *ctx,
    ice_candidate_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_web_rtc_session_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const web_rtc_session_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_web_rtc_session_impl(
    aether_meta_read_context_t *ctx,
    web_rtc_session_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_client_timing_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_timing_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_client_timing_impl(
    aether_meta_read_context_t *ctx,
    client_timing_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_probe_sample_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const probe_sample_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_probe_sample_impl(
    aether_meta_read_context_t *ctx,
    probe_sample_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_probe_report_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const probe_report_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_probe_report_impl(
    aether_meta_read_context_t *ctx,
    probe_report_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_finish_result_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const finish_result_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_finish_result_impl(
    aether_meta_read_context_t *ctx,
    finish_result_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_registration_direct_result_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const registration_direct_result_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_registration_direct_result_impl(
    aether_meta_read_context_t *ctx,
    registration_direct_result_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_serialize_recovery_result_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const recovery_result_t *value,
    unsigned int recursion_depth);

static aether_status_t client_server_api_deserialize_recovery_result_impl(
    aether_meta_read_context_t *ctx,
    recovery_result_t *value,
    unsigned int recursion_depth);

aether_status_t client_server_api_serialize_key(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_ref_t *value) {

    return client_server_api_serialize_key_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 6u:
        if ((*value).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key(
    aether_meta_read_context_t *ctx,
    key_ref_t *value) {

    return client_server_api_deserialize_key_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_impl(
    aether_meta_read_context_t *ctx,
    key_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_0 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_chacha20_poly1305 = _meta_child_0;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_0 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_curve_private = _meta_child_0;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_0 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_curve_public = _meta_child_0;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_0 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_secret_box = _meta_child_0;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_0 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_curve_public = _meta_child_0;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_0 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_curve_private = _meta_child_0;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_0 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.sodium_sign_private = _meta_child_0;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_0 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_sign_public = _meta_child_0;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_0 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.hydrogen_sign_private = _meta_child_0;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_0 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_sign_public = _meta_child_0;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_0 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.p256_aes_gcm_symmetric = _meta_child_0;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_0 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.p256_aes_gcm_curve_private = _meta_child_0;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_0 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.p256_aes_gcm_curve_public = _meta_child_0;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_0 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.p256_aes_gcm_sign_private = _meta_child_0;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_0 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.p256_aes_gcm_sign_public = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_key_symmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_symmetric_ref_t *value) {

    return client_server_api_serialize_key_symmetric_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_symmetric_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_symmetric_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 6u:
        if ((*value).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key_symmetric(
    aether_meta_read_context_t *ctx,
    key_symmetric_ref_t *value) {

    return client_server_api_deserialize_key_symmetric_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_symmetric_impl(
    aether_meta_read_context_t *ctx,
    key_symmetric_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_0 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_chacha20_poly1305 = _meta_child_0;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_0 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_secret_box = _meta_child_0;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_0 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.p256_aes_gcm_symmetric = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_key_asymmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_ref_t *value) {

    return client_server_api_serialize_key_asymmetric_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_asymmetric_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key_asymmetric(
    aether_meta_read_context_t *ctx,
    key_asymmetric_ref_t *value) {

    return client_server_api_deserialize_key_asymmetric_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_asymmetric_impl(
    aether_meta_read_context_t *ctx,
    key_asymmetric_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 1u: {
            hydrogen_curve_private_t *_meta_child_0 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_curve_private = _meta_child_0;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_0 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_curve_public = _meta_child_0;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_0 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_curve_public = _meta_child_0;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_0 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_curve_private = _meta_child_0;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_0 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.p256_aes_gcm_curve_private = _meta_child_0;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_0 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.p256_aes_gcm_curve_public = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_key_asymmetric_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_public_ref_t *value) {

    return client_server_api_serialize_key_asymmetric_public_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_asymmetric_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_public_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 2u:
        if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key_asymmetric_public(
    aether_meta_read_context_t *ctx,
    key_asymmetric_public_ref_t *value) {

    return client_server_api_deserialize_key_asymmetric_public_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_asymmetric_public_impl(
    aether_meta_read_context_t *ctx,
    key_asymmetric_public_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 2u: {
            hydrogen_curve_public_t *_meta_child_0 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_curve_public = _meta_child_0;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_0 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_curve_public = _meta_child_0;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_0 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.p256_aes_gcm_curve_public = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_key_asymmetric_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_private_ref_t *value) {

    return client_server_api_serialize_key_asymmetric_private_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_asymmetric_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_asymmetric_private_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key_asymmetric_private(
    aether_meta_read_context_t *ctx,
    key_asymmetric_private_ref_t *value) {

    return client_server_api_deserialize_key_asymmetric_private_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_asymmetric_private_impl(
    aether_meta_read_context_t *ctx,
    key_asymmetric_private_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 1u: {
            hydrogen_curve_private_t *_meta_child_0 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_curve_private = _meta_child_0;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_0 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_curve_private = _meta_child_0;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_0 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.p256_aes_gcm_curve_private = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_key_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_ref_t *value) {

    return client_server_api_serialize_key_sign_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_sign_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 9u:
        if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 14u:
        if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key_sign(
    aether_meta_read_context_t *ctx,
    key_sign_ref_t *value) {

    return client_server_api_deserialize_key_sign_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_sign_impl(
    aether_meta_read_context_t *ctx,
    key_sign_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 9u: {
            sodium_sign_private_t *_meta_child_0 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.sodium_sign_private = _meta_child_0;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_0 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_sign_public = _meta_child_0;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_0 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.hydrogen_sign_private = _meta_child_0;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_0 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_sign_public = _meta_child_0;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_0 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.p256_aes_gcm_sign_private = _meta_child_0;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_0 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.p256_aes_gcm_sign_public = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_key_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_public_ref_t *value) {

    return client_server_api_serialize_key_sign_public_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_sign_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_public_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 10u:
        if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 5u:
        if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key_sign_public(
    aether_meta_read_context_t *ctx,
    key_sign_public_ref_t *value) {

    return client_server_api_deserialize_key_sign_public_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_sign_public_impl(
    aether_meta_read_context_t *ctx,
    key_sign_public_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 10u: {
            sodium_sign_public_t *_meta_child_0 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.sodium_sign_public = _meta_child_0;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_0 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.hydrogen_sign_public = _meta_child_0;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_0 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.p256_aes_gcm_sign_public = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_key_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_private_ref_t *value) {

    return client_server_api_serialize_key_sign_private_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_sign_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_sign_private_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 9u:
        if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 4u:
        if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key_sign_private(
    aether_meta_read_context_t *ctx,
    key_sign_private_ref_t *value) {

    return client_server_api_deserialize_key_sign_private_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_sign_private_impl(
    aether_meta_read_context_t *ctx,
    key_sign_private_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 9u: {
            sodium_sign_private_t *_meta_child_0 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.sodium_sign_private = _meta_child_0;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_0 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.hydrogen_sign_private = _meta_child_0;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_0 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            ((*value)).as.p256_aes_gcm_sign_private = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sodium_chacha20_poly1305(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_chacha20_poly1305_t *value) {

    return client_server_api_serialize_sodium_chacha20_poly1305_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sodium_chacha20_poly1305_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_chacha20_poly1305_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sodium_chacha20_poly1305(
    aether_meta_read_context_t *ctx,
    sodium_chacha20_poly1305_t *value) {

    return client_server_api_deserialize_sodium_chacha20_poly1305_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sodium_chacha20_poly1305_impl(
    aether_meta_read_context_t *ctx,
    sodium_chacha20_poly1305_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_hydrogen_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_private_t *value) {

    return client_server_api_serialize_hydrogen_curve_private_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_hydrogen_curve_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_hydrogen_curve_private(
    aether_meta_read_context_t *ctx,
    hydrogen_curve_private_t *value) {

    return client_server_api_deserialize_hydrogen_curve_private_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_hydrogen_curve_private_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_curve_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_hydrogen_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_public_t *value) {

    return client_server_api_serialize_hydrogen_curve_public_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_hydrogen_curve_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_curve_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_hydrogen_curve_public(
    aether_meta_read_context_t *ctx,
    hydrogen_curve_public_t *value) {

    return client_server_api_deserialize_hydrogen_curve_public_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_hydrogen_curve_public_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_curve_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_hydrogen_secret_box(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_secret_box_t *value) {

    return client_server_api_serialize_hydrogen_secret_box_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_hydrogen_secret_box_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_secret_box_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_hydrogen_secret_box(
    aether_meta_read_context_t *ctx,
    hydrogen_secret_box_t *value) {

    return client_server_api_deserialize_hydrogen_secret_box_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_hydrogen_secret_box_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_secret_box_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sodium_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_public_t *value) {

    return client_server_api_serialize_sodium_curve_public_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sodium_curve_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sodium_curve_public(
    aether_meta_read_context_t *ctx,
    sodium_curve_public_t *value) {

    return client_server_api_deserialize_sodium_curve_public_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sodium_curve_public_impl(
    aether_meta_read_context_t *ctx,
    sodium_curve_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sodium_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_private_t *value) {

    return client_server_api_serialize_sodium_curve_private_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sodium_curve_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_curve_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sodium_curve_private(
    aether_meta_read_context_t *ctx,
    sodium_curve_private_t *value) {

    return client_server_api_deserialize_sodium_curve_private_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sodium_curve_private_impl(
    aether_meta_read_context_t *ctx,
    sodium_curve_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sodium_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_private_t *value) {

    return client_server_api_serialize_sodium_sign_private_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sodium_sign_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sodium_sign_private(
    aether_meta_read_context_t *ctx,
    sodium_sign_private_t *value) {

    return client_server_api_deserialize_sodium_sign_private_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sodium_sign_private_impl(
    aether_meta_read_context_t *ctx,
    sodium_sign_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sodium_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_public_t *value) {

    return client_server_api_serialize_sodium_sign_public_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sodium_sign_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sodium_sign_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sodium_sign_public(
    aether_meta_read_context_t *ctx,
    sodium_sign_public_t *value) {

    return client_server_api_deserialize_sodium_sign_public_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sodium_sign_public_impl(
    aether_meta_read_context_t *ctx,
    sodium_sign_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_hydrogen_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_private_t *value) {

    return client_server_api_serialize_hydrogen_sign_private_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_hydrogen_sign_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_hydrogen_sign_private(
    aether_meta_read_context_t *ctx,
    hydrogen_sign_private_t *value) {

    return client_server_api_deserialize_hydrogen_sign_private_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_hydrogen_sign_private_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_sign_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_hydrogen_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_public_t *value) {

    return client_server_api_serialize_hydrogen_sign_public_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_hydrogen_sign_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const hydrogen_sign_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_hydrogen_sign_public(
    aether_meta_read_context_t *ctx,
    hydrogen_sign_public_t *value) {

    return client_server_api_deserialize_hydrogen_sign_public_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_hydrogen_sign_public_impl(
    aether_meta_read_context_t *ctx,
    hydrogen_sign_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_ref_t *value) {

    return client_server_api_serialize_sign_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sign_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sign_a_e_e_d25519->data.data, 64u);
    pos += 64u;
        break;
    case 2u:
        if ((*value).as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sign_h_y_d_r_o_g_e_n->data.data, 64u);
    pos += 64u;
        break;
    case 3u:
        if ((*value).as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.sign_p256_aes_gcm->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sign(
    aether_meta_read_context_t *ctx,
    sign_ref_t *value) {

    return client_server_api_deserialize_sign_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sign_impl(
    aether_meta_read_context_t *ctx,
    sign_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 1u: {
            sign_a_e_e_d25519_t *_meta_child_0 = (sign_a_e_e_d25519_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_a_e_e_d25519_t), _Alignof(sign_a_e_e_d25519_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.sign_a_e_e_d25519 = _meta_child_0;
            break;
        }
        case 2u: {
            sign_h_y_d_r_o_g_e_n_t *_meta_child_0 = (sign_h_y_d_r_o_g_e_n_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_h_y_d_r_o_g_e_n_t), _Alignof(sign_h_y_d_r_o_g_e_n_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.sign_h_y_d_r_o_g_e_n = _meta_child_0;
            break;
        }
        case 3u: {
            sign_p256_aes_gcm_t *_meta_child_0 = (sign_p256_aes_gcm_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_p256_aes_gcm_t), _Alignof(sign_p256_aes_gcm_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            ((*value)).as.sign_p256_aes_gcm = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sign_a_e_e_d25519(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_a_e_e_d25519_t *value) {

    return client_server_api_serialize_sign_a_e_e_d25519_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sign_a_e_e_d25519_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_a_e_e_d25519_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sign_a_e_e_d25519(
    aether_meta_read_context_t *ctx,
    sign_a_e_e_d25519_t *value) {

    return client_server_api_deserialize_sign_a_e_e_d25519_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sign_a_e_e_d25519_impl(
    aether_meta_read_context_t *ctx,
    sign_a_e_e_d25519_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sign_h_y_d_r_o_g_e_n(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_h_y_d_r_o_g_e_n_t *value) {

    return client_server_api_serialize_sign_h_y_d_r_o_g_e_n_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sign_h_y_d_r_o_g_e_n_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_h_y_d_r_o_g_e_n_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sign_h_y_d_r_o_g_e_n(
    aether_meta_read_context_t *ctx,
    sign_h_y_d_r_o_g_e_n_t *value) {

    return client_server_api_deserialize_sign_h_y_d_r_o_g_e_n_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sign_h_y_d_r_o_g_e_n_impl(
    aether_meta_read_context_t *ctx,
    sign_h_y_d_r_o_g_e_n_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_signed_key(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const signed_key_t *value) {

    return client_server_api_serialize_signed_key_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_signed_key_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const signed_key_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->key.type_id;
    switch (value->key.type_id) {
    case 6u:
        if (value->key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->sign.type_id;
    switch (value->sign.type_id) {
    case 1u:
        if (value->sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_a_e_e_d25519->data.data, 64u);
    pos += 64u;
        break;
    case 2u:
        if (value->sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_h_y_d_r_o_g_e_n->data.data, 64u);
    pos += 64u;
        break;
    case 3u:
        if (value->sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_p256_aes_gcm->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_signed_key(
    aether_meta_read_context_t *ctx,
    signed_key_t *value) {

    return client_server_api_deserialize_signed_key_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_signed_key_impl(
    aether_meta_read_context_t *ctx,
    signed_key_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_1 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.sodium_chacha20_poly1305 = _meta_child_1;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_1 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.hydrogen_curve_private = _meta_child_1;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_1 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.hydrogen_curve_public = _meta_child_1;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_1 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.hydrogen_secret_box = _meta_child_1;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_1 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.sodium_curve_public = _meta_child_1;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_1 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.sodium_curve_private = _meta_child_1;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_1 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->key).as.sodium_sign_private = _meta_child_1;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_1 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.sodium_sign_public = _meta_child_1;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_1 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->key).as.hydrogen_sign_private = _meta_child_1;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_1 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.hydrogen_sign_public = _meta_child_1;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_1 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.p256_aes_gcm_symmetric = _meta_child_1;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_1 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.p256_aes_gcm_curve_private = _meta_child_1;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_1 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->key).as.p256_aes_gcm_curve_public = _meta_child_1;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_1 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.p256_aes_gcm_sign_private = _meta_child_1;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_1 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->key).as.p256_aes_gcm_sign_public = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->key).type_id = _meta_type_id_1;
    }
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 1u: {
            sign_a_e_e_d25519_t *_meta_child_1 = (sign_a_e_e_d25519_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_a_e_e_d25519_t), _Alignof(sign_a_e_e_d25519_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->sign).as.sign_a_e_e_d25519 = _meta_child_1;
            break;
        }
        case 2u: {
            sign_h_y_d_r_o_g_e_n_t *_meta_child_1 = (sign_h_y_d_r_o_g_e_n_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_h_y_d_r_o_g_e_n_t), _Alignof(sign_h_y_d_r_o_g_e_n_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->sign).as.sign_h_y_d_r_o_g_e_n = _meta_child_1;
            break;
        }
        case 3u: {
            sign_p256_aes_gcm_t *_meta_child_1 = (sign_p256_aes_gcm_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_p256_aes_gcm_t), _Alignof(sign_p256_aes_gcm_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->sign).as.sign_p256_aes_gcm = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->sign).type_id = _meta_type_id_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_peer_identity_data(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_identity_data_t *value) {

    return client_server_api_serialize_peer_identity_data_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_peer_identity_data_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_identity_data_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->key.type_id;
    switch (value->key.type_id) {
    case 10u:
        if (value->key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 5u:
        if (value->key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->key.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->key.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_peer_identity_data(
    aether_meta_read_context_t *ctx,
    peer_identity_data_t *value) {

    return client_server_api_deserialize_peer_identity_data_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_peer_identity_data_impl(
    aether_meta_read_context_t *ctx,
    peer_identity_data_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->uid))) return AETHER_ERR_PROTOCOL;
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 10u: {
            sodium_sign_public_t *_meta_child_1 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.sodium_sign_public = _meta_child_1;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_1 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->key).as.hydrogen_sign_public = _meta_child_1;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_1 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->key).as.p256_aes_gcm_sign_public = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->key).type_id = _meta_type_id_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_peer_identity(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_identity_t *value) {

    return client_server_api_serialize_peer_identity_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_peer_identity_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const peer_identity_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = client_server_api_serialize_peer_identity_data_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->data)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_signed_key_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->serverSigningKey)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->sign.type_id;
    switch (value->sign.type_id) {
    case 1u:
        if (value->sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_a_e_e_d25519 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_a_e_e_d25519->data.data, 64u);
    pos += 64u;
        break;
    case 2u:
        if (value->sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_h_y_d_r_o_g_e_n == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_h_y_d_r_o_g_e_n->data.data, 64u);
    pos += 64u;
        break;
    case 3u:
        if (value->sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (value->sign.as.sign_p256_aes_gcm == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->sign.as.sign_p256_aes_gcm->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_peer_identity(
    aether_meta_read_context_t *ctx,
    peer_identity_t *value) {

    return client_server_api_deserialize_peer_identity_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_peer_identity_impl(
    aether_meta_read_context_t *ctx,
    peer_identity_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        aether_status_t meta_status = client_server_api_deserialize_peer_identity_data_impl(
            ctx,
            &(value->data),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_deserialize_signed_key_impl(
            ctx,
            &(value->serverSigningKey),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 1u: {
            sign_a_e_e_d25519_t *_meta_child_1 = (sign_a_e_e_d25519_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_a_e_e_d25519_t), _Alignof(sign_a_e_e_d25519_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->sign).as.sign_a_e_e_d25519 = _meta_child_1;
            break;
        }
        case 2u: {
            sign_h_y_d_r_o_g_e_n_t *_meta_child_1 = (sign_h_y_d_r_o_g_e_n_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_h_y_d_r_o_g_e_n_t), _Alignof(sign_h_y_d_r_o_g_e_n_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->sign).as.sign_h_y_d_r_o_g_e_n = _meta_child_1;
            break;
        }
        case 3u: {
            sign_p256_aes_gcm_t *_meta_child_1 = (sign_p256_aes_gcm_t *)aether_meta_scratch_alloc(ctx, sizeof(sign_p256_aes_gcm_t), _Alignof(sign_p256_aes_gcm_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->sign).as.sign_p256_aes_gcm = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->sign).type_id = _meta_type_id_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_work_proof_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_config_ref_t *value) {

    return client_server_api_serialize_work_proof_config_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_work_proof_config_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_config_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.work_proof_b_crypt == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.work_proof_b_crypt == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)(*value).as.work_proof_b_crypt->costBCrypt);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)(*value).as.work_proof_b_crypt->poolSize);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)(*value).as.work_proof_b_crypt->maxHashVal);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_work_proof_config(
    aether_meta_read_context_t *ctx,
    work_proof_config_ref_t *value) {

    return client_server_api_deserialize_work_proof_config_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_work_proof_config_impl(
    aether_meta_read_context_t *ctx,
    work_proof_config_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 1u: {
            work_proof_b_crypt_t *_meta_child_0 = (work_proof_b_crypt_t *)aether_meta_scratch_alloc(ctx, sizeof(work_proof_b_crypt_t), _Alignof(work_proof_b_crypt_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    {
        uint8_t _meta_byte = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_byte)) return AETHER_ERR_PROTOCOL;
        (_meta_child_0->costBCrypt) = (int8_t)_meta_byte;
    }
    {
        uint8_t _meta_byte = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_byte)) return AETHER_ERR_PROTOCOL;
        (_meta_child_0->poolSize) = (int8_t)_meta_byte;
    }
    if (!aether_meta_read_i32le(&ctx->reader, &(_meta_child_0->maxHashVal))) return AETHER_ERR_PROTOCOL;
    }
            ((*value)).as.work_proof_b_crypt = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_work_proof_b_crypt(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_b_crypt_t *value) {

    return client_server_api_serialize_work_proof_b_crypt_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_work_proof_b_crypt_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_b_crypt_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->costBCrypt);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->poolSize);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->maxHashVal);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_work_proof_b_crypt(
    aether_meta_read_context_t *ctx,
    work_proof_b_crypt_t *value) {

    return client_server_api_deserialize_work_proof_b_crypt_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_work_proof_b_crypt_impl(
    aether_meta_read_context_t *ctx,
    work_proof_b_crypt_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_byte = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_byte)) return AETHER_ERR_PROTOCOL;
        (value->costBCrypt) = (int8_t)_meta_byte;
    }
    {
        uint8_t _meta_byte = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_byte)) return AETHER_ERR_PROTOCOL;
        (value->poolSize) = (int8_t)_meta_byte;
    }
    if (!aether_meta_read_i32le(&ctx->reader, &(value->maxHashVal))) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_work_proof_d_t_o(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_d_t_o_t *value) {

    return client_server_api_serialize_work_proof_d_t_o_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_work_proof_d_t_o_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const work_proof_d_t_o_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->salt.data, value->salt.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->suffix.data, value->suffix.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->poolSize);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->maxHashVal);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_signed_key_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->globalKey)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_work_proof_d_t_o(
    aether_meta_read_context_t *ctx,
    work_proof_d_t_o_t *value) {

    return client_server_api_deserialize_work_proof_d_t_o_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_work_proof_d_t_o_impl(
    aether_meta_read_context_t *ctx,
    work_proof_d_t_o_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_view(&ctx->reader, &(value->salt).data, &(value->salt).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->suffix).data, &(value->suffix).length)) return AETHER_ERR_PROTOCOL;
    {
        uint8_t _meta_byte = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_byte)) return AETHER_ERR_PROTOCOL;
        (value->poolSize) = (int8_t)_meta_byte;
    }
    if (!aether_meta_read_i32le(&ctx->reader, &(value->maxHashVal))) return AETHER_ERR_PROTOCOL;
    {
        aether_status_t meta_status = client_server_api_deserialize_signed_key_impl(
            ctx,
            &(value->globalKey),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_i_p_address(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_ref_t *value) {

    return client_server_api_serialize_i_p_address_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_i_p_address_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if ((*value).as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if ((*value).as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*value).as.i_p_address_web->data.data, (*value).as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_i_p_address(
    aether_meta_read_context_t *ctx,
    i_p_address_ref_t *value) {

    return client_server_api_deserialize_i_p_address_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_i_p_address_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 1u: {
            i_p_address_v4_t *_meta_child_0 = (i_p_address_v4_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_v4_t), _Alignof(i_p_address_v4_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        4u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 4u);
    ctx->reader.position += 4u;
    }
            ((*value)).as.i_p_address_v4 = _meta_child_0;
            break;
        }
        case 2u: {
            i_p_address_v6_t *_meta_child_0 = (i_p_address_v6_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_v6_t), _Alignof(i_p_address_v6_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (ctx->reader.position > ctx->reader.length ||
        16u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_0->data).data, ctx->reader.data + ctx->reader.position, 16u);
    ctx->reader.position += 16u;
    }
            ((*value)).as.i_p_address_v6 = _meta_child_0;
            break;
        }
        case 3u: {
            i_p_address_web_t *_meta_child_0 = (i_p_address_web_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_web_t), _Alignof(i_p_address_web_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (!aether_meta_read_view(&ctx->reader, &(_meta_child_0->data).data, &(_meta_child_0->data).length)) return AETHER_ERR_PROTOCOL;
    }
            ((*value)).as.i_p_address_web = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_i_p_address_v4(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v4_t *value) {

    return client_server_api_serialize_i_p_address_v4_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_i_p_address_v4_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v4_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 4u);
    pos += 4u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_i_p_address_v4(
    aether_meta_read_context_t *ctx,
    i_p_address_v4_t *value) {

    return client_server_api_deserialize_i_p_address_v4_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_i_p_address_v4_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_v4_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        4u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 4u);
    ctx->reader.position += 4u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_i_p_address_v6(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v6_t *value) {

    return client_server_api_serialize_i_p_address_v6_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_i_p_address_v6_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_v6_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 16u);
    pos += 16u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_i_p_address_v6(
    aether_meta_read_context_t *ctx,
    i_p_address_v6_t *value) {

    return client_server_api_deserialize_i_p_address_v6_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_i_p_address_v6_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_v6_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        16u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 16u);
    ctx->reader.position += 16u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_i_p_address_web(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_web_t *value) {

    return client_server_api_serialize_i_p_address_web_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_i_p_address_web_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_web_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->data.data, value->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_i_p_address_web(
    aether_meta_read_context_t *ctx,
    i_p_address_web_t *value) {

    return client_server_api_deserialize_i_p_address_web_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_i_p_address_web_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_web_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_view(&ctx->reader, &(value->data).data, &(value->data).length)) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_coder_and_port(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const coder_and_port_t *value) {

    return client_server_api_serialize_coder_and_port_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_coder_and_port_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const coder_and_port_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->codec;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->port);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_coder_and_port(
    aether_meta_read_context_t *ctx,
    coder_and_port_t *value) {

    return client_server_api_deserialize_coder_and_port_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_coder_and_port_impl(
    aether_meta_read_context_t *ctx,
    coder_and_port_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_enum = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_enum)) return AETHER_ERR_PROTOCOL;
        if (_meta_enum >= 4u) return AETHER_ERR_PROTOCOL;
        (value->codec) = (adsl_aether_codec_t)_meta_enum;
    }
    if (!aether_meta_read_i16le(&ctx->reader, &(value->port))) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_i_p_address_and_ports(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_t *value) {

    return client_server_api_serialize_i_p_address_and_ports_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_i_p_address_and_ports_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->address.type_id;
    switch (value->address.type_id) {
    case 1u:
        if (value->address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->address.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->address.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (value->address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->address.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->address.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (value->address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (value->address.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->address.as.i_p_address_web->data.data, value->address.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (value->coderAndPorts.length > 0u && value->coderAndPorts.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_coderAndPorts_length = (uint64_t)value->coderAndPorts.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_coderAndPorts_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_coderAndPorts_index = 0u; value_coderAndPorts_index < value->coderAndPorts.length; ++value_coderAndPorts_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_coder_and_port_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->coderAndPorts.data[value_coderAndPorts_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_i_p_address_and_ports(
    aether_meta_read_context_t *ctx,
    i_p_address_and_ports_t *value) {

    return client_server_api_deserialize_i_p_address_and_ports_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_i_p_address_and_ports_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_and_ports_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 1u: {
            i_p_address_v4_t *_meta_child_1 = (i_p_address_v4_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_v4_t), _Alignof(i_p_address_v4_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        4u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 4u);
    ctx->reader.position += 4u;
    }
            (value->address).as.i_p_address_v4 = _meta_child_1;
            break;
        }
        case 2u: {
            i_p_address_v6_t *_meta_child_1 = (i_p_address_v6_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_v6_t), _Alignof(i_p_address_v6_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        16u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 16u);
    ctx->reader.position += 16u;
    }
            (value->address).as.i_p_address_v6 = _meta_child_1;
            break;
        }
        case 3u: {
            i_p_address_web_t *_meta_child_1 = (i_p_address_web_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_web_t), _Alignof(i_p_address_web_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (!aether_meta_read_view(&ctx->reader, &(_meta_child_1->data).data, &(_meta_child_1->data).length)) return AETHER_ERR_PROTOCOL;
    }
            (value->address).as.i_p_address_web = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->address).type_id = _meta_type_id_1;
    }
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        coder_and_port_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(coder_and_port_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (coder_and_port_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(coder_and_port_t), _Alignof(coder_and_port_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(coder_and_port_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_coder_and_port_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->coderAndPorts).data = _meta_buf_1;
        (value->coderAndPorts).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_i_p_address_and_ports_list(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_list_t *value) {

    return client_server_api_serialize_i_p_address_and_ports_list_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_i_p_address_and_ports_list_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const i_p_address_and_ports_list_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (value->addresses.length > 0u && value->addresses.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_addresses_length = (uint64_t)value->addresses.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_addresses_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_addresses_index = 0u; value_addresses_index < value->addresses.length; ++value_addresses_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_i_p_address_and_ports_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->addresses.data[value_addresses_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_i_p_address_and_ports_list(
    aether_meta_read_context_t *ctx,
    i_p_address_and_ports_list_t *value) {

    return client_server_api_deserialize_i_p_address_and_ports_list_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_i_p_address_and_ports_list_impl(
    aether_meta_read_context_t *ctx,
    i_p_address_and_ports_list_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        i_p_address_and_ports_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(i_p_address_and_ports_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (i_p_address_and_ports_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(i_p_address_and_ports_t), _Alignof(i_p_address_and_ports_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(i_p_address_and_ports_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_i_p_address_and_ports_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->addresses).data = _meta_buf_1;
        (value->addresses).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_server_descriptor(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_t *value) {

    return client_server_api_serialize_server_descriptor_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_server_descriptor_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_i_p_address_and_ports_list_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->ipAddress)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->signingKeys.length > 0u && value->signingKeys.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_signingKeys_length = (uint64_t)value->signingKeys.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_signingKeys_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_signingKeys_index = 0u; value_signingKeys_index < value->signingKeys.length; ++value_signingKeys_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_signed_key_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->signingKeys.data[value_signingKeys_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_server_descriptor(
    aether_meta_read_context_t *ctx,
    server_descriptor_t *value) {

    return client_server_api_deserialize_server_descriptor_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_server_descriptor_impl(
    aether_meta_read_context_t *ctx,
    server_descriptor_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i16le(&ctx->reader, &(value->id))) return AETHER_ERR_PROTOCOL;
    {
        aether_status_t meta_status = client_server_api_deserialize_i_p_address_and_ports_list_impl(
            ctx,
            &(value->ipAddress),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        signed_key_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(signed_key_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (signed_key_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(signed_key_t), _Alignof(signed_key_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(signed_key_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_signed_key_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->signingKeys).data = _meta_buf_1;
        (value->signingKeys).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_server_descriptor_with_geo(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_with_geo_t *value) {

    return client_server_api_serialize_server_descriptor_with_geo_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_server_descriptor_with_geo_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const server_descriptor_with_geo_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->time);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_i_p_address_and_ports_list_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->ipAddress)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t bits = aether_meta_java_double_bits(value->latitude);
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t bits = aether_meta_java_double_bits(value->longitude);
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->type;
    if (value->signingKeys.length > 0u && value->signingKeys.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_signingKeys_length = (uint64_t)value->signingKeys.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_signingKeys_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_signingKeys_index = 0u; value_signingKeys_index < value->signingKeys.length; ++value_signingKeys_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_signed_key_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->signingKeys.data[value_signingKeys_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_server_descriptor_with_geo(
    aether_meta_read_context_t *ctx,
    server_descriptor_with_geo_t *value) {

    return client_server_api_deserialize_server_descriptor_with_geo_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_server_descriptor_with_geo_impl(
    aether_meta_read_context_t *ctx,
    server_descriptor_with_geo_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i16le(&ctx->reader, &(value->id))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i32le(&ctx->reader, &(value->time))) return AETHER_ERR_PROTOCOL;
    {
        aether_status_t meta_status = client_server_api_deserialize_i_p_address_and_ports_list_impl(
            ctx,
            &(value->ipAddress),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t _meta_bits = 0u;
        if (!aether_meta_read_u64le(&ctx->reader, &_meta_bits)) return AETHER_ERR_PROTOCOL;
        memcpy(&(value->latitude), &_meta_bits, sizeof(_meta_bits));
    }
    {
        uint64_t _meta_bits = 0u;
        if (!aether_meta_read_u64le(&ctx->reader, &_meta_bits)) return AETHER_ERR_PROTOCOL;
        memcpy(&(value->longitude), &_meta_bits, sizeof(_meta_bits));
    }
    {
        uint8_t _meta_enum = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_enum)) return AETHER_ERR_PROTOCOL;
        if (_meta_enum >= 2u) return AETHER_ERR_PROTOCOL;
        (value->type) = (server_type_t)_meta_enum;
    }
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        signed_key_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(signed_key_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (signed_key_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(signed_key_t), _Alignof(signed_key_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(signed_key_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_signed_key_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->signingKeys).data = _meta_buf_1;
        (value->signingKeys).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_cloud(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_t *value) {

    return client_server_api_serialize_cloud_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_cloud_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (value->data.length > 0u && value->data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_data_length = (uint64_t)value->data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_data_index = 0u; value_data_index < value->data.length; ++value_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->data.data[value_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_cloud(
    aether_meta_read_context_t *ctx,
    cloud_t *value) {

    return client_server_api_deserialize_cloud_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_cloud_impl(
    aether_meta_read_context_t *ctx,
    cloud_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        int16_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(int16_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (int16_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(int16_t), _Alignof(int16_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(int16_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    if (!aether_meta_read_i16le(&ctx->reader, &(_meta_buf_1[_meta_i_1]))) return AETHER_ERR_PROTOCOL;
        }
        (value->data).data = _meta_buf_1;
        (value->data).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_money_operation(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const money_operation_t *value) {

    return client_server_api_serialize_money_operation_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_money_operation_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const money_operation_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->from);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->to);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->amount);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->time);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->credit);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->status;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_money_operation(
    aether_meta_read_context_t *ctx,
    money_operation_t *value) {

    return client_server_api_deserialize_money_operation_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_money_operation_impl(
    aether_meta_read_context_t *ctx,
    money_operation_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i64le(&ctx->reader, &(value->id))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->from))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->to))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i64le(&ctx->reader, &(value->amount))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i64le(&ctx->reader, &(value->time))) return AETHER_ERR_PROTOCOL;
    {
        uint8_t _meta_bool = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_bool)) return AETHER_ERR_PROTOCOL;
        (value->credit) = ((int8_t)_meta_bool > 0) ? true : false;
    }
    {
        uint8_t _meta_enum = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_enum)) return AETHER_ERR_PROTOCOL;
        if (_meta_enum >= 3u) return AETHER_ERR_PROTOCOL;
        (value->status) = (status_t)_meta_enum;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_access_group(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_group_t *value) {

    return client_server_api_serialize_access_group_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_access_group_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_group_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->time);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->owner);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->data.length > 0u && value->data.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_data_length = (uint64_t)value->data.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_data_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_data_index = 0u; value_data_index < value->data.length; ++value_data_index) {
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->data.data[value_data_index]);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_access_group(
    aether_meta_read_context_t *ctx,
    access_group_t *value) {

    return client_server_api_deserialize_access_group_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_access_group_impl(
    aether_meta_read_context_t *ctx,
    access_group_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->id))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i32le(&ctx->reader, &(value->time))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->owner))) return AETHER_ERR_PROTOCOL;
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        aether_uuid_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(aether_uuid_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (aether_uuid_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(aether_uuid_t), _Alignof(aether_uuid_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(aether_uuid_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    if (!aether_meta_read_uuid(&ctx->reader, &(_meta_buf_1[_meta_i_1]))) return AETHER_ERR_PROTOCOL;
        }
        (value->data).data = _meta_buf_1;
        (value->data).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_u_u_i_d_and_cloud(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const u_u_i_d_and_cloud_t *value) {

    return client_server_api_serialize_u_u_i_d_and_cloud_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_u_u_i_d_and_cloud_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const u_u_i_d_and_cloud_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_cloud_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->cloud)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_u_u_i_d_and_cloud(
    aether_meta_read_context_t *ctx,
    u_u_i_d_and_cloud_t *value) {

    return client_server_api_deserialize_u_u_i_d_and_cloud_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_u_u_i_d_and_cloud_impl(
    aether_meta_read_context_t *ctx,
    u_u_i_d_and_cloud_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->uid))) return AETHER_ERR_PROTOCOL;
    {
        aether_status_t meta_status = client_server_api_deserialize_cloud_impl(
            ctx,
            &(value->cloud),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_cloud_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_config_t *value) {

    return client_server_api_serialize_cloud_config_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_cloud_config_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const cloud_config_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->subjectUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->configVersion);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_cloud_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->cloud)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_cloud_config(
    aether_meta_read_context_t *ctx,
    cloud_config_t *value) {

    return client_server_api_deserialize_cloud_config_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_cloud_config_impl(
    aether_meta_read_context_t *ctx,
    cloud_config_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->subjectUid))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i64le(&ctx->reader, &(value->configVersion))) return AETHER_ERR_PROTOCOL;
    {
        aether_status_t meta_status = client_server_api_deserialize_cloud_impl(
            ctx,
            &(value->cloud),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_applied_config(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const applied_config_t *value) {

    return client_server_api_serialize_applied_config_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_applied_config_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const applied_config_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->subjectUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->configVersion);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_applied_config(
    aether_meta_read_context_t *ctx,
    applied_config_t *value) {

    return client_server_api_deserialize_applied_config_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_applied_config_impl(
    aether_meta_read_context_t *ctx,
    applied_config_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->subjectUid))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i64le(&ctx->reader, &(value->configVersion))) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_pair_keys(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_ref_t *value) {

    return client_server_api_serialize_pair_keys_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_pair_keys_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 3u:
        if ((*value).as.pair_keys_sign == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_sign->privateKey.type_id;
    switch ((*value).as.pair_keys_sign->privateKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_sign->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_sign->publicKey.type_id;
    switch ((*value).as.pair_keys_sign->publicKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_sign->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sign->publicKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
        break;
    case 1u:
        if ((*value).as.pair_keys_asym == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_asym->privateKey.type_id;
    switch ((*value).as.pair_keys_asym->privateKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_asym->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_asym->publicKey.type_id;
    switch ((*value).as.pair_keys_asym->publicKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_asym->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym->publicKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
        break;
    case 2u:
        if ((*value).as.pair_keys_asym_signed == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_asym_signed->privateKey.type_id;
    switch ((*value).as.pair_keys_asym_signed->privateKey.type_id) {
    case 6u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_asym_signed->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_signed_key_impl(
            tx,
            tx_capacity,
            &pos,
            (&((*value).as.pair_keys_asym_signed->publicKey)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    case 4u:
        if ((*value).as.pair_keys_sym == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_sym->clientToServer.type_id;
    switch ((*value).as.pair_keys_sym->clientToServer.type_id) {
    case 6u:
        if ((*value).as.pair_keys_sym->clientToServer.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->clientToServer.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->clientToServer.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_sym->clientToServer.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->clientToServer.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->clientToServer.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_sym->clientToServer.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->clientToServer.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->clientToServer.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).as.pair_keys_sym->serverToClient.type_id;
    switch ((*value).as.pair_keys_sym->serverToClient.type_id) {
    case 6u:
        if ((*value).as.pair_keys_sym->serverToClient.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->serverToClient.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->serverToClient.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if ((*value).as.pair_keys_sym->serverToClient.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->serverToClient.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->serverToClient.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if ((*value).as.pair_keys_sym->serverToClient.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.pair_keys_sym->serverToClient.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, (*value).as.pair_keys_sym->serverToClient.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_pair_keys(
    aether_meta_read_context_t *ctx,
    pair_keys_ref_t *value) {

    return client_server_api_deserialize_pair_keys_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_pair_keys_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 3u: {
            pair_keys_sign_t *_meta_child_0 = (pair_keys_sign_t *)aether_meta_scratch_alloc(ctx, sizeof(pair_keys_sign_t), _Alignof(pair_keys_sign_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    {
        uint8_t _meta_type_id_2 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_2)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_2) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_2 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_chacha20_poly1305 = _meta_child_2;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_2 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_curve_private = _meta_child_2;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_2 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_curve_public = _meta_child_2;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_2 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_secret_box = _meta_child_2;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_2 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_curve_public = _meta_child_2;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_2 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_curve_private = _meta_child_2;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_2 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.sodium_sign_private = _meta_child_2;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_2 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_sign_public = _meta_child_2;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_2 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.hydrogen_sign_private = _meta_child_2;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_2 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_sign_public = _meta_child_2;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_2 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_symmetric = _meta_child_2;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_2 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_curve_private = _meta_child_2;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_2 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_curve_public = _meta_child_2;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_2 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_sign_private = _meta_child_2;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_2 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_sign_public = _meta_child_2;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (_meta_child_0->privateKey).type_id = _meta_type_id_2;
    }
    {
        uint8_t _meta_type_id_2 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_2)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_2) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_2 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.sodium_chacha20_poly1305 = _meta_child_2;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_2 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.hydrogen_curve_private = _meta_child_2;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_2 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.hydrogen_curve_public = _meta_child_2;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_2 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.hydrogen_secret_box = _meta_child_2;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_2 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.sodium_curve_public = _meta_child_2;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_2 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.sodium_curve_private = _meta_child_2;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_2 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->publicKey).as.sodium_sign_private = _meta_child_2;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_2 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.sodium_sign_public = _meta_child_2;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_2 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->publicKey).as.hydrogen_sign_private = _meta_child_2;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_2 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.hydrogen_sign_public = _meta_child_2;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_2 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_symmetric = _meta_child_2;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_2 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_curve_private = _meta_child_2;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_2 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_curve_public = _meta_child_2;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_2 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_sign_private = _meta_child_2;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_2 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_sign_public = _meta_child_2;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (_meta_child_0->publicKey).type_id = _meta_type_id_2;
    }
    }
            ((*value)).as.pair_keys_sign = _meta_child_0;
            break;
        }
        case 1u: {
            pair_keys_asym_t *_meta_child_0 = (pair_keys_asym_t *)aether_meta_scratch_alloc(ctx, sizeof(pair_keys_asym_t), _Alignof(pair_keys_asym_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    {
        uint8_t _meta_type_id_2 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_2)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_2) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_2 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_chacha20_poly1305 = _meta_child_2;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_2 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_curve_private = _meta_child_2;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_2 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_curve_public = _meta_child_2;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_2 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_secret_box = _meta_child_2;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_2 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_curve_public = _meta_child_2;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_2 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_curve_private = _meta_child_2;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_2 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.sodium_sign_private = _meta_child_2;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_2 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_sign_public = _meta_child_2;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_2 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.hydrogen_sign_private = _meta_child_2;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_2 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_sign_public = _meta_child_2;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_2 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_symmetric = _meta_child_2;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_2 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_curve_private = _meta_child_2;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_2 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_curve_public = _meta_child_2;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_2 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_sign_private = _meta_child_2;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_2 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_sign_public = _meta_child_2;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (_meta_child_0->privateKey).type_id = _meta_type_id_2;
    }
    {
        uint8_t _meta_type_id_2 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_2)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_2) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_2 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.sodium_chacha20_poly1305 = _meta_child_2;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_2 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.hydrogen_curve_private = _meta_child_2;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_2 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.hydrogen_curve_public = _meta_child_2;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_2 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.hydrogen_secret_box = _meta_child_2;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_2 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.sodium_curve_public = _meta_child_2;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_2 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.sodium_curve_private = _meta_child_2;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_2 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->publicKey).as.sodium_sign_private = _meta_child_2;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_2 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.sodium_sign_public = _meta_child_2;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_2 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->publicKey).as.hydrogen_sign_private = _meta_child_2;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_2 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.hydrogen_sign_public = _meta_child_2;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_2 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_symmetric = _meta_child_2;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_2 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_curve_private = _meta_child_2;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_2 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_curve_public = _meta_child_2;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_2 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_sign_private = _meta_child_2;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_2 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->publicKey).as.p256_aes_gcm_sign_public = _meta_child_2;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (_meta_child_0->publicKey).type_id = _meta_type_id_2;
    }
    }
            ((*value)).as.pair_keys_asym = _meta_child_0;
            break;
        }
        case 2u: {
            pair_keys_asym_signed_t *_meta_child_0 = (pair_keys_asym_signed_t *)aether_meta_scratch_alloc(ctx, sizeof(pair_keys_asym_signed_t), _Alignof(pair_keys_asym_signed_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    {
        uint8_t _meta_type_id_2 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_2)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_2) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_2 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_chacha20_poly1305 = _meta_child_2;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_2 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_curve_private = _meta_child_2;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_2 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_curve_public = _meta_child_2;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_2 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_secret_box = _meta_child_2;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_2 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_curve_public = _meta_child_2;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_2 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_curve_private = _meta_child_2;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_2 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.sodium_sign_private = _meta_child_2;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_2 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.sodium_sign_public = _meta_child_2;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_2 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.hydrogen_sign_private = _meta_child_2;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_2 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.hydrogen_sign_public = _meta_child_2;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_2 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_symmetric = _meta_child_2;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_2 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_curve_private = _meta_child_2;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_2 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_curve_public = _meta_child_2;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_2 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_sign_private = _meta_child_2;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_2 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (_meta_child_0->privateKey).as.p256_aes_gcm_sign_public = _meta_child_2;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (_meta_child_0->privateKey).type_id = _meta_type_id_2;
    }
    {
        aether_status_t meta_status = client_server_api_deserialize_signed_key_impl(
            ctx,
            &(_meta_child_0->publicKey),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
            ((*value)).as.pair_keys_asym_signed = _meta_child_0;
            break;
        }
        case 4u: {
            pair_keys_sym_t *_meta_child_0 = (pair_keys_sym_t *)aether_meta_scratch_alloc(ctx, sizeof(pair_keys_sym_t), _Alignof(pair_keys_sym_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    {
        uint8_t _meta_type_id_2 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_2)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_2) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_2 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->clientToServer).as.sodium_chacha20_poly1305 = _meta_child_2;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_2 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->clientToServer).as.hydrogen_secret_box = _meta_child_2;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_2 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->clientToServer).as.p256_aes_gcm_symmetric = _meta_child_2;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (_meta_child_0->clientToServer).type_id = _meta_type_id_2;
    }
    {
        uint8_t _meta_type_id_2 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_2)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_2) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_2 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->serverToClient).as.sodium_chacha20_poly1305 = _meta_child_2;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_2 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->serverToClient).as.hydrogen_secret_box = _meta_child_2;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_2 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_2 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_2, 0, sizeof(*_meta_child_2));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_2->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (_meta_child_0->serverToClient).as.p256_aes_gcm_symmetric = _meta_child_2;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (_meta_child_0->serverToClient).type_id = _meta_type_id_2;
    }
    }
            ((*value)).as.pair_keys_sym = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_pair_keys_sign(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sign_t *value) {

    return client_server_api_serialize_pair_keys_sign_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_pair_keys_sign_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sign_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->privateKey.type_id;
    switch (value->privateKey.type_id) {
    case 6u:
        if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->publicKey.type_id;
    switch (value->publicKey.type_id) {
    case 6u:
        if (value->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_pair_keys_sign(
    aether_meta_read_context_t *ctx,
    pair_keys_sign_t *value) {

    return client_server_api_deserialize_pair_keys_sign_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_pair_keys_sign_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_sign_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_1 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_chacha20_poly1305 = _meta_child_1;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_1 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_curve_private = _meta_child_1;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_1 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_curve_public = _meta_child_1;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_1 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_secret_box = _meta_child_1;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_1 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_curve_public = _meta_child_1;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_1 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_curve_private = _meta_child_1;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_1 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.sodium_sign_private = _meta_child_1;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_1 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_sign_public = _meta_child_1;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_1 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.hydrogen_sign_private = _meta_child_1;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_1 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_sign_public = _meta_child_1;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_1 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_symmetric = _meta_child_1;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_1 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_curve_private = _meta_child_1;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_1 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.p256_aes_gcm_curve_public = _meta_child_1;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_1 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_sign_private = _meta_child_1;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_1 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.p256_aes_gcm_sign_public = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->privateKey).type_id = _meta_type_id_1;
    }
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_1 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.sodium_chacha20_poly1305 = _meta_child_1;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_1 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.hydrogen_curve_private = _meta_child_1;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_1 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.hydrogen_curve_public = _meta_child_1;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_1 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.hydrogen_secret_box = _meta_child_1;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_1 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.sodium_curve_public = _meta_child_1;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_1 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.sodium_curve_private = _meta_child_1;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_1 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->publicKey).as.sodium_sign_private = _meta_child_1;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_1 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.sodium_sign_public = _meta_child_1;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_1 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->publicKey).as.hydrogen_sign_private = _meta_child_1;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_1 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.hydrogen_sign_public = _meta_child_1;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_1 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.p256_aes_gcm_symmetric = _meta_child_1;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_1 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.p256_aes_gcm_curve_private = _meta_child_1;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_1 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->publicKey).as.p256_aes_gcm_curve_public = _meta_child_1;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_1 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.p256_aes_gcm_sign_private = _meta_child_1;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_1 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->publicKey).as.p256_aes_gcm_sign_public = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->publicKey).type_id = _meta_type_id_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_pair_keys_asym(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_t *value) {

    return client_server_api_serialize_pair_keys_asym_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_pair_keys_asym_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->privateKey.type_id;
    switch (value->privateKey.type_id) {
    case 6u:
        if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->publicKey.type_id;
    switch (value->publicKey.type_id) {
    case 6u:
        if (value->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->publicKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->publicKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_pair_keys_asym(
    aether_meta_read_context_t *ctx,
    pair_keys_asym_t *value) {

    return client_server_api_deserialize_pair_keys_asym_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_pair_keys_asym_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_asym_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_1 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_chacha20_poly1305 = _meta_child_1;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_1 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_curve_private = _meta_child_1;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_1 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_curve_public = _meta_child_1;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_1 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_secret_box = _meta_child_1;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_1 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_curve_public = _meta_child_1;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_1 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_curve_private = _meta_child_1;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_1 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.sodium_sign_private = _meta_child_1;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_1 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_sign_public = _meta_child_1;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_1 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.hydrogen_sign_private = _meta_child_1;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_1 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_sign_public = _meta_child_1;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_1 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_symmetric = _meta_child_1;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_1 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_curve_private = _meta_child_1;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_1 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.p256_aes_gcm_curve_public = _meta_child_1;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_1 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_sign_private = _meta_child_1;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_1 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.p256_aes_gcm_sign_public = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->privateKey).type_id = _meta_type_id_1;
    }
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_1 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.sodium_chacha20_poly1305 = _meta_child_1;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_1 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.hydrogen_curve_private = _meta_child_1;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_1 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.hydrogen_curve_public = _meta_child_1;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_1 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.hydrogen_secret_box = _meta_child_1;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_1 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.sodium_curve_public = _meta_child_1;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_1 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.sodium_curve_private = _meta_child_1;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_1 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->publicKey).as.sodium_sign_private = _meta_child_1;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_1 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.sodium_sign_public = _meta_child_1;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_1 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->publicKey).as.hydrogen_sign_private = _meta_child_1;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_1 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.hydrogen_sign_public = _meta_child_1;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_1 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.p256_aes_gcm_symmetric = _meta_child_1;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_1 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.p256_aes_gcm_curve_private = _meta_child_1;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_1 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->publicKey).as.p256_aes_gcm_curve_public = _meta_child_1;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_1 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->publicKey).as.p256_aes_gcm_sign_private = _meta_child_1;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_1 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->publicKey).as.p256_aes_gcm_sign_public = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->publicKey).type_id = _meta_type_id_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_pair_keys_asym_signed(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_signed_t *value) {

    return client_server_api_serialize_pair_keys_asym_signed_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_pair_keys_asym_signed_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_asym_signed_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->privateKey.type_id;
    switch (value->privateKey.type_id) {
    case 6u:
        if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 1u:
        if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 2u:
        if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 8u:
        if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_public->data.data, 32u);
    pos += 32u;
        break;
    case 7u:
        if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 9u:
        if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 10u:
        if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.sodium_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.sodium_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 4u:
        if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_private->data.data, 64u);
    pos += 64u;
        break;
    case 5u:
        if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.hydrogen_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.hydrogen_sign_public->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    case 12u:
        if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_private->data.data, 32u);
    pos += 32u;
        break;
    case 13u:
        if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_curve_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_curve_public->data.data, 64u);
    pos += 64u;
        break;
    case 14u:
        if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_private == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_private->data.data, 32u);
    pos += 32u;
        break;
    case 15u:
        if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (value->privateKey.as.p256_aes_gcm_sign_public == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->privateKey.as.p256_aes_gcm_sign_public->data.data, 64u);
    pos += 64u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_signed_key_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->publicKey)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_pair_keys_asym_signed(
    aether_meta_read_context_t *ctx,
    pair_keys_asym_signed_t *value) {

    return client_server_api_deserialize_pair_keys_asym_signed_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_pair_keys_asym_signed_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_asym_signed_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_1 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_chacha20_poly1305 = _meta_child_1;
            break;
        }
        case 1u: {
            hydrogen_curve_private_t *_meta_child_1 = (hydrogen_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_private_t), _Alignof(hydrogen_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_curve_private = _meta_child_1;
            break;
        }
        case 2u: {
            hydrogen_curve_public_t *_meta_child_1 = (hydrogen_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_curve_public_t), _Alignof(hydrogen_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_curve_public = _meta_child_1;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_1 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_secret_box = _meta_child_1;
            break;
        }
        case 8u: {
            sodium_curve_public_t *_meta_child_1 = (sodium_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_public_t), _Alignof(sodium_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_curve_public = _meta_child_1;
            break;
        }
        case 7u: {
            sodium_curve_private_t *_meta_child_1 = (sodium_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_curve_private_t), _Alignof(sodium_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_curve_private = _meta_child_1;
            break;
        }
        case 9u: {
            sodium_sign_private_t *_meta_child_1 = (sodium_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_private_t), _Alignof(sodium_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.sodium_sign_private = _meta_child_1;
            break;
        }
        case 10u: {
            sodium_sign_public_t *_meta_child_1 = (sodium_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_sign_public_t), _Alignof(sodium_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.sodium_sign_public = _meta_child_1;
            break;
        }
        case 4u: {
            hydrogen_sign_private_t *_meta_child_1 = (hydrogen_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_private_t), _Alignof(hydrogen_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.hydrogen_sign_private = _meta_child_1;
            break;
        }
        case 5u: {
            hydrogen_sign_public_t *_meta_child_1 = (hydrogen_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_sign_public_t), _Alignof(hydrogen_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.hydrogen_sign_public = _meta_child_1;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_1 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_symmetric = _meta_child_1;
            break;
        }
        case 12u: {
            p256_aes_gcm_curve_private_t *_meta_child_1 = (p256_aes_gcm_curve_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_private_t), _Alignof(p256_aes_gcm_curve_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_curve_private = _meta_child_1;
            break;
        }
        case 13u: {
            p256_aes_gcm_curve_public_t *_meta_child_1 = (p256_aes_gcm_curve_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_curve_public_t), _Alignof(p256_aes_gcm_curve_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.p256_aes_gcm_curve_public = _meta_child_1;
            break;
        }
        case 14u: {
            p256_aes_gcm_sign_private_t *_meta_child_1 = (p256_aes_gcm_sign_private_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_private_t), _Alignof(p256_aes_gcm_sign_private_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->privateKey).as.p256_aes_gcm_sign_private = _meta_child_1;
            break;
        }
        case 15u: {
            p256_aes_gcm_sign_public_t *_meta_child_1 = (p256_aes_gcm_sign_public_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_sign_public_t), _Alignof(p256_aes_gcm_sign_public_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }
            (value->privateKey).as.p256_aes_gcm_sign_public = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->privateKey).type_id = _meta_type_id_1;
    }
    {
        aether_status_t meta_status = client_server_api_deserialize_signed_key_impl(
            ctx,
            &(value->publicKey),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_pair_keys_sym(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sym_t *value) {

    return client_server_api_serialize_pair_keys_sym_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_pair_keys_sym_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const pair_keys_sym_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->clientToServer.type_id;
    switch (value->clientToServer.type_id) {
    case 6u:
        if (value->clientToServer.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->clientToServer.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->clientToServer.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->clientToServer.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->clientToServer.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->clientToServer.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->clientToServer.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->clientToServer.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->clientToServer.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->serverToClient.type_id;
    switch (value->serverToClient.type_id) {
    case 6u:
        if (value->serverToClient.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->serverToClient.as.sodium_chacha20_poly1305 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->serverToClient.as.sodium_chacha20_poly1305->data.data, 32u);
    pos += 32u;
        break;
    case 3u:
        if (value->serverToClient.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (value->serverToClient.as.hydrogen_secret_box == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->serverToClient.as.hydrogen_secret_box->data.data, 32u);
    pos += 32u;
        break;
    case 11u:
        if (value->serverToClient.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (value->serverToClient.as.p256_aes_gcm_symmetric == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->serverToClient.as.p256_aes_gcm_symmetric->data.data, 32u);
    pos += 32u;
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_pair_keys_sym(
    aether_meta_read_context_t *ctx,
    pair_keys_sym_t *value) {

    return client_server_api_deserialize_pair_keys_sym_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_pair_keys_sym_impl(
    aether_meta_read_context_t *ctx,
    pair_keys_sym_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_1 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->clientToServer).as.sodium_chacha20_poly1305 = _meta_child_1;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_1 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->clientToServer).as.hydrogen_secret_box = _meta_child_1;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_1 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->clientToServer).as.p256_aes_gcm_symmetric = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->clientToServer).type_id = _meta_type_id_1;
    }
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 6u: {
            sodium_chacha20_poly1305_t *_meta_child_1 = (sodium_chacha20_poly1305_t *)aether_meta_scratch_alloc(ctx, sizeof(sodium_chacha20_poly1305_t), _Alignof(sodium_chacha20_poly1305_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->serverToClient).as.sodium_chacha20_poly1305 = _meta_child_1;
            break;
        }
        case 3u: {
            hydrogen_secret_box_t *_meta_child_1 = (hydrogen_secret_box_t *)aether_meta_scratch_alloc(ctx, sizeof(hydrogen_secret_box_t), _Alignof(hydrogen_secret_box_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->serverToClient).as.hydrogen_secret_box = _meta_child_1;
            break;
        }
        case 11u: {
            p256_aes_gcm_symmetric_t *_meta_child_1 = (p256_aes_gcm_symmetric_t *)aether_meta_scratch_alloc(ctx, sizeof(p256_aes_gcm_symmetric_t), _Alignof(p256_aes_gcm_symmetric_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }
            (value->serverToClient).as.p256_aes_gcm_symmetric = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->serverToClient).type_id = _meta_type_id_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_access_check_pair(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_pair_t *value) {

    return client_server_api_serialize_access_check_pair_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_access_check_pair_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_pair_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->sourceUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->targetUid);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_access_check_pair(
    aether_meta_read_context_t *ctx,
    access_check_pair_t *value) {

    return client_server_api_deserialize_access_check_pair_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_access_check_pair_impl(
    aether_meta_read_context_t *ctx,
    access_check_pair_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->sourceUid))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->targetUid))) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_access_check_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_result_t *value) {

    return client_server_api_serialize_access_check_result_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_access_check_result_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const access_check_result_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->sourceUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->targetUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u8(tx, tx_capacity, &pos, (uint8_t)value->hasAccess);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_access_check_result(
    aether_meta_read_context_t *ctx,
    access_check_result_t *value) {

    return client_server_api_deserialize_access_check_result_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_access_check_result_impl(
    aether_meta_read_context_t *ctx,
    access_check_result_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->sourceUid))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->targetUid))) return AETHER_ERR_PROTOCOL;
    {
        uint8_t _meta_bool = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_bool)) return AETHER_ERR_PROTOCOL;
        (value->hasAccess) = ((int8_t)_meta_bool > 0) ? true : false;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_ip_info(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ip_info_t *value) {

    return client_server_api_serialize_ip_info_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_ip_info_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ip_info_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->ip.type_id;
    switch (value->ip.type_id) {
    case 1u:
        if (value->ip.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ip.as.i_p_address_v4 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 4u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->ip.as.i_p_address_v4->data.data, 4u);
    pos += 4u;
        break;
    case 2u:
        if (value->ip.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ip.as.i_p_address_v6 == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 16u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->ip.as.i_p_address_v6->data.data, 16u);
    pos += 16u;
        break;
    case 3u:
        if (value->ip.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    if (value->ip.as.i_p_address_web == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->ip.as.i_p_address_web->data.data, value->ip.as.i_p_address_web->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->port);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t bits = aether_meta_java_double_bits(value->latitude);
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t bits = aether_meta_java_double_bits(value->longitude);
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, bits);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_ip_info(
    aether_meta_read_context_t *ctx,
    ip_info_t *value) {

    return client_server_api_deserialize_ip_info_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_ip_info_impl(
    aether_meta_read_context_t *ctx,
    ip_info_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    {
        uint8_t _meta_type_id_1 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_1)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_1) {
        case 1u: {
            i_p_address_v4_t *_meta_child_1 = (i_p_address_v4_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_v4_t), _Alignof(i_p_address_v4_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        4u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 4u);
    ctx->reader.position += 4u;
    }
            (value->ip).as.i_p_address_v4 = _meta_child_1;
            break;
        }
        case 2u: {
            i_p_address_v6_t *_meta_child_1 = (i_p_address_v6_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_v6_t), _Alignof(i_p_address_v6_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (ctx->reader.position > ctx->reader.length ||
        16u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((_meta_child_1->data).data, ctx->reader.data + ctx->reader.position, 16u);
    ctx->reader.position += 16u;
    }
            (value->ip).as.i_p_address_v6 = _meta_child_1;
            break;
        }
        case 3u: {
            i_p_address_web_t *_meta_child_1 = (i_p_address_web_t *)aether_meta_scratch_alloc(ctx, sizeof(i_p_address_web_t), _Alignof(i_p_address_web_t));
            if (_meta_child_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_1, 0, sizeof(*_meta_child_1));
    {
    if (!aether_meta_read_view(&ctx->reader, &(_meta_child_1->data).data, &(_meta_child_1->data).length)) return AETHER_ERR_PROTOCOL;
    }
            (value->ip).as.i_p_address_web = _meta_child_1;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        (value->ip).type_id = _meta_type_id_1;
    }
    if (!aether_meta_read_i16le(&ctx->reader, &(value->port))) return AETHER_ERR_PROTOCOL;
    {
        uint64_t _meta_bits = 0u;
        if (!aether_meta_read_u64le(&ctx->reader, &_meta_bits)) return AETHER_ERR_PROTOCOL;
        memcpy(&(value->latitude), &_meta_bits, sizeof(_meta_bits));
    }
    {
        uint64_t _meta_bits = 0u;
        if (!aether_meta_read_u64le(&ctx->reader, &_meta_bits)) return AETHER_ERR_PROTOCOL;
        memcpy(&(value->longitude), &_meta_bits, sizeof(_meta_bits));
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_p256_aes_gcm_symmetric(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_symmetric_t *value) {

    return client_server_api_serialize_p256_aes_gcm_symmetric_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_p256_aes_gcm_symmetric_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_symmetric_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_p256_aes_gcm_symmetric(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_symmetric_t *value) {

    return client_server_api_deserialize_p256_aes_gcm_symmetric_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_p256_aes_gcm_symmetric_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_symmetric_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_p256_aes_gcm_curve_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_private_t *value) {

    return client_server_api_serialize_p256_aes_gcm_curve_private_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_p256_aes_gcm_curve_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_p256_aes_gcm_curve_private(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_curve_private_t *value) {

    return client_server_api_deserialize_p256_aes_gcm_curve_private_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_p256_aes_gcm_curve_private_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_curve_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_p256_aes_gcm_curve_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_public_t *value) {

    return client_server_api_serialize_p256_aes_gcm_curve_public_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_p256_aes_gcm_curve_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_curve_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_p256_aes_gcm_curve_public(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_curve_public_t *value) {

    return client_server_api_deserialize_p256_aes_gcm_curve_public_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_p256_aes_gcm_curve_public_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_curve_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_p256_aes_gcm_sign_private(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_private_t *value) {

    return client_server_api_serialize_p256_aes_gcm_sign_private_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_p256_aes_gcm_sign_private_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 32u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 32u);
    pos += 32u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_p256_aes_gcm_sign_private(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_sign_private_t *value) {

    return client_server_api_deserialize_p256_aes_gcm_sign_private_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_p256_aes_gcm_sign_private_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_sign_private_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        32u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 32u);
    ctx->reader.position += 32u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_p256_aes_gcm_sign_public(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_public_t *value) {

    return client_server_api_serialize_p256_aes_gcm_sign_public_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_p256_aes_gcm_sign_public_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const p256_aes_gcm_sign_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_p256_aes_gcm_sign_public(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_sign_public_t *value) {

    return client_server_api_deserialize_p256_aes_gcm_sign_public_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_p256_aes_gcm_sign_public_impl(
    aether_meta_read_context_t *ctx,
    p256_aes_gcm_sign_public_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_sign_p256_aes_gcm(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_p256_aes_gcm_t *value) {

    return client_server_api_serialize_sign_p256_aes_gcm_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_sign_p256_aes_gcm_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const sign_p256_aes_gcm_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 64u > tx_capacity) return AETHER_ERR_OVERFLOW;
    memcpy(tx + pos, value->data.data, 64u);
    pos += 64u;

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_sign_p256_aes_gcm(
    aether_meta_read_context_t *ctx,
    sign_p256_aes_gcm_t *value) {

    return client_server_api_deserialize_sign_p256_aes_gcm_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_sign_p256_aes_gcm_impl(
    aether_meta_read_context_t *ctx,
    sign_p256_aes_gcm_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (ctx->reader.position > ctx->reader.length ||
        64u > ctx->reader.length - ctx->reader.position) return AETHER_ERR_PROTOCOL;
    memcpy((value->data).data, ctx->reader.data + ctx->reader.position, 64u);
    ctx->reader.position += 64u;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_telemetry(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const telemetry_ref_t *value) {

    return client_server_api_serialize_telemetry_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_telemetry_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const telemetry_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)(*value).type_id;
    switch ((*value).type_id) {
    case 1u:
        if ((*value).as.telemetry_c_p_p == NULL) return AETHER_ERR_ARGUMENT;
    if ((*value).as.telemetry_c_p_p == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)(*value).as.telemetry_c_p_p->utm_id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*value).as.telemetry_c_p_p->blob.data, (*value).as.telemetry_c_p_p->blob.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*value).as.telemetry_c_p_p->lib_version.data, (*value).as.telemetry_c_p_p->lib_version.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*value).as.telemetry_c_p_p->os.data, (*value).as.telemetry_c_p_p->os.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, (*value).as.telemetry_c_p_p->compiler.data, (*value).as.telemetry_c_p_p->compiler.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
        break;
    default:
        return AETHER_ERR_ARGUMENT;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_telemetry(
    aether_meta_read_context_t *ctx,
    telemetry_ref_t *value) {

    return client_server_api_deserialize_telemetry_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_telemetry_impl(
    aether_meta_read_context_t *ctx,
    telemetry_ref_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
        uint8_t _meta_type_id_0 = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_type_id_0)) return AETHER_ERR_PROTOCOL;
        switch (_meta_type_id_0) {
        case 1u: {
            telemetry_c_p_p_t *_meta_child_0 = (telemetry_c_p_p_t *)aether_meta_scratch_alloc(ctx, sizeof(telemetry_c_p_p_t), _Alignof(telemetry_c_p_p_t));
            if (_meta_child_0 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_child_0, 0, sizeof(*_meta_child_0));
    {
    if (!aether_meta_read_i32le(&ctx->reader, &(_meta_child_0->utm_id))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(_meta_child_0->blob).data, &(_meta_child_0->blob).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(_meta_child_0->lib_version).data, &(_meta_child_0->lib_version).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(_meta_child_0->os).data, &(_meta_child_0->os).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(_meta_child_0->compiler).data, &(_meta_child_0->compiler).length)) return AETHER_ERR_PROTOCOL;
    }
            ((*value)).as.telemetry_c_p_p = _meta_child_0;
            break;
        }
        default:
            return AETHER_ERR_PROTOCOL;
        }
        ((*value)).type_id = _meta_type_id_0;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_telemetry_c_p_p(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const telemetry_c_p_p_t *value) {

    return client_server_api_serialize_telemetry_c_p_p_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_telemetry_c_p_p_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const telemetry_c_p_p_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->utm_id);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->blob.data, value->blob.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->lib_version.data, value->lib_version.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->os.data, value->os.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->compiler.data, value->compiler.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_telemetry_c_p_p(
    aether_meta_read_context_t *ctx,
    telemetry_c_p_p_t *value) {

    return client_server_api_deserialize_telemetry_c_p_p_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_telemetry_c_p_p_impl(
    aether_meta_read_context_t *ctx,
    telemetry_c_p_p_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i32le(&ctx->reader, &(value->utm_id))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->blob).data, &(value->blob).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->lib_version).data, &(value->lib_version).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->os).data, &(value->os).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->compiler).data, &(value->compiler).length)) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_message(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const message_t *value) {

    return client_server_api_serialize_message_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_message_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const message_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->data.data, value->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_message(
    aether_meta_read_context_t *ctx,
    message_t *value) {

    return client_server_api_deserialize_message_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_message_impl(
    aether_meta_read_context_t *ctx,
    message_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->uid))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->data).data, &(value->data).length)) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_client_activity(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_activity_t *value) {

    return client_server_api_serialize_client_activity_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_client_activity_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_activity_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_nullable_mask = UINT64_C(0);
    if (value->serverId == NULL) value_nullable_mask |= (UINT64_C(1) << 0u);
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)((value_nullable_mask >> 0u) & UINT64_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->timestamp);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)value->type;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->clientUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->details.data, value->details.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->serverId != NULL) {
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)(*value->serverId));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_client_activity(
    aether_meta_read_context_t *ctx,
    client_activity_t *value) {

    return client_server_api_deserialize_client_activity_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_client_activity_impl(
    aether_meta_read_context_t *ctx,
    client_activity_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    uint64_t _meta_nullable_mask = UINT64_C(0);
    {
        uint8_t _meta_mask_byte = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_mask_byte)) return AETHER_ERR_PROTOCOL;
        _meta_nullable_mask |= ((uint64_t)_meta_mask_byte) << 0u;
    }
    if (!aether_meta_read_i64le(&ctx->reader, &(value->timestamp))) return AETHER_ERR_PROTOCOL;
    {
        uint8_t _meta_enum = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_enum)) return AETHER_ERR_PROTOCOL;
        if (_meta_enum >= 6u) return AETHER_ERR_PROTOCOL;
        (value->type) = (client_activity_type_t)_meta_enum;
    }
    if (!aether_meta_read_uuid(&ctx->reader, &(value->clientUid))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->details).data, &(value->details).length)) return AETHER_ERR_PROTOCOL;
    if ((_meta_nullable_mask & (UINT64_C(1) << 0u)) == 0u) {
        int16_t *_meta_tmp_0 = (int16_t *)aether_meta_scratch_alloc(ctx, sizeof(int16_t), _Alignof(int16_t));
        if (_meta_tmp_0 == NULL) return AETHER_ERR_OVERFLOW;
        memset(_meta_tmp_0, 0, sizeof(*_meta_tmp_0));
    if (!aether_meta_read_i16le(&ctx->reader, &((*_meta_tmp_0)))) return AETHER_ERR_PROTOCOL;
        value->serverId = _meta_tmp_0;
    } else {
        value->serverId = NULL;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_key_value_pair(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_value_pair_t *value) {

    return client_server_api_serialize_key_value_pair_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_key_value_pair_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const key_value_pair_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->key.data, value->key.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->value.data, value->value.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_key_value_pair(
    aether_meta_read_context_t *ctx,
    key_value_pair_t *value) {

    return client_server_api_deserialize_key_value_pair_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_key_value_pair_impl(
    aether_meta_read_context_t *ctx,
    key_value_pair_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_view(&ctx->reader, &(value->key).data, &(value->key).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->value).data, &(value->value).length)) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_client_log_entry(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_log_entry_t *value) {

    return client_server_api_serialize_client_log_entry_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_client_log_entry_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_log_entry_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->timestamp);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->level.data, value->level.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->message.data, value->message.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->context.length > 0u && value->context.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_context_length = (uint64_t)value->context.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_context_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_context_index = 0u; value_context_index < value->context.length; ++value_context_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_key_value_pair_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->context.data[value_context_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_client_log_entry(
    aether_meta_read_context_t *ctx,
    client_log_entry_t *value) {

    return client_server_api_deserialize_client_log_entry_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_client_log_entry_impl(
    aether_meta_read_context_t *ctx,
    client_log_entry_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i64le(&ctx->reader, &(value->timestamp))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->level).data, &(value->level).length)) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_view(&ctx->reader, &(value->message).data, &(value->message).length)) return AETHER_ERR_PROTOCOL;
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        key_value_pair_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(key_value_pair_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (key_value_pair_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(key_value_pair_t), _Alignof(key_value_pair_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(key_value_pair_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_key_value_pair_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->context).data = _meta_buf_1;
        (value->context).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_client_connection_info(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_connection_info_t *value) {

    return client_server_api_serialize_client_connection_info_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_client_connection_info_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_connection_info_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_nullable_mask = UINT64_C(0);
    if (value->disconnectedAt == NULL) value_nullable_mask |= (UINT64_C(1) << 0u);
    if (pos + 1u > tx_capacity) return AETHER_ERR_OVERFLOW;
    tx[pos++] = (uint8_t)((value_nullable_mask >> 0u) & UINT64_C(0xff));
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->connectedAt);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->disconnectedAt != NULL) {
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)(*value->disconnectedAt));
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    {
        aether_status_t meta_status = aether_meta_write_u16le(tx, tx_capacity, &pos, (uint16_t)value->serverId);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->context.length > 0u && value->context.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_context_length = (uint64_t)value->context.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_context_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_context_index = 0u; value_context_index < value->context.length; ++value_context_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_key_value_pair_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->context.data[value_context_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->protocol.data, value->protocol.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_client_connection_info(
    aether_meta_read_context_t *ctx,
    client_connection_info_t *value) {

    return client_server_api_deserialize_client_connection_info_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_client_connection_info_impl(
    aether_meta_read_context_t *ctx,
    client_connection_info_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    uint64_t _meta_nullable_mask = UINT64_C(0);
    {
        uint8_t _meta_mask_byte = 0u;
        if (!aether_meta_read_u8(&ctx->reader, &_meta_mask_byte)) return AETHER_ERR_PROTOCOL;
        _meta_nullable_mask |= ((uint64_t)_meta_mask_byte) << 0u;
    }
    if (!aether_meta_read_i64le(&ctx->reader, &(value->connectedAt))) return AETHER_ERR_PROTOCOL;
    if ((_meta_nullable_mask & (UINT64_C(1) << 0u)) == 0u) {
        int64_t *_meta_tmp_0 = (int64_t *)aether_meta_scratch_alloc(ctx, sizeof(int64_t), _Alignof(int64_t));
        if (_meta_tmp_0 == NULL) return AETHER_ERR_OVERFLOW;
        memset(_meta_tmp_0, 0, sizeof(*_meta_tmp_0));
    if (!aether_meta_read_i64le(&ctx->reader, &((*_meta_tmp_0)))) return AETHER_ERR_PROTOCOL;
        value->disconnectedAt = _meta_tmp_0;
    } else {
        value->disconnectedAt = NULL;
    }
    if (!aether_meta_read_i16le(&ctx->reader, &(value->serverId))) return AETHER_ERR_PROTOCOL;
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        key_value_pair_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(key_value_pair_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (key_value_pair_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(key_value_pair_t), _Alignof(key_value_pair_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(key_value_pair_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_key_value_pair_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->context).data = _meta_buf_1;
        (value->context).length = _meta_len_1;
    }
    if (!aether_meta_read_view(&ctx->reader, &(value->protocol).data, &(value->protocol).length)) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_message_info(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const message_info_t *value) {

    return client_server_api_serialize_message_info_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_message_info_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const message_info_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->timestamp);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->fromUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->toUid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->size);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->messageId);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_message_info(
    aether_meta_read_context_t *ctx,
    message_info_t *value) {

    return client_server_api_deserialize_message_info_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_message_info_impl(
    aether_meta_read_context_t *ctx,
    message_info_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i64le(&ctx->reader, &(value->timestamp))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->fromUid))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->toUid))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i32le(&ctx->reader, &(value->size))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->messageId))) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_uap(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const uap_t *value) {

    return client_server_api_serialize_uap_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_uap_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const uap_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->deltaMs);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->lastReadTimestamp);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_uap(
    aether_meta_read_context_t *ctx,
    uap_t *value) {

    return client_server_api_deserialize_uap_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_uap_impl(
    aether_meta_read_context_t *ctx,
    uap_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i64le(&ctx->reader, &(value->deltaMs))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i64le(&ctx->reader, &(value->lastReadTimestamp))) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_ice_candidate(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ice_candidate_t *value) {

    return client_server_api_serialize_ice_candidate_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_ice_candidate_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const ice_candidate_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->data.data, value->data.length);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_ice_candidate(
    aether_meta_read_context_t *ctx,
    ice_candidate_t *value) {

    return client_server_api_deserialize_ice_candidate_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_ice_candidate_impl(
    aether_meta_read_context_t *ctx,
    ice_candidate_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_view(&ctx->reader, &(value->data).data, &(value->data).length)) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_web_rtc_session(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const web_rtc_session_t *value) {

    return client_server_api_serialize_web_rtc_session_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_web_rtc_session_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const web_rtc_session_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_bytes(tx, tx_capacity, &pos, value->sdp.data, value->sdp.length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->candidates.length > 0u && value->candidates.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_candidates_length = (uint64_t)value->candidates.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_candidates_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_candidates_index = 0u; value_candidates_index < value->candidates.length; ++value_candidates_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_ice_candidate_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->candidates.data[value_candidates_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_web_rtc_session(
    aether_meta_read_context_t *ctx,
    web_rtc_session_t *value) {

    return client_server_api_deserialize_web_rtc_session_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_web_rtc_session_impl(
    aether_meta_read_context_t *ctx,
    web_rtc_session_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_view(&ctx->reader, &(value->sdp).data, &(value->sdp).length)) return AETHER_ERR_PROTOCOL;
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        ice_candidate_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(ice_candidate_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (ice_candidate_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(ice_candidate_t), _Alignof(ice_candidate_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(ice_candidate_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_ice_candidate_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->candidates).data = _meta_buf_1;
        (value->candidates).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_client_timing(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_timing_t *value) {

    return client_server_api_serialize_client_timing_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_client_timing_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const client_timing_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->lastActivityDeltaMs);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->nextPingDeltaMs);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_client_timing(
    aether_meta_read_context_t *ctx,
    client_timing_t *value) {

    return client_server_api_deserialize_client_timing_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_client_timing_impl(
    aether_meta_read_context_t *ctx,
    client_timing_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i64le(&ctx->reader, &(value->lastActivityDeltaMs))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i64le(&ctx->reader, &(value->nextPingDeltaMs))) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_probe_sample(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const probe_sample_t *value) {

    return client_server_api_serialize_probe_sample_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_probe_sample_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const probe_sample_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->sequence);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u64le(tx, tx_capacity, &pos, (uint64_t)value->receivedAtMs);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_probe_sample(
    aether_meta_read_context_t *ctx,
    probe_sample_t *value) {

    return client_server_api_deserialize_probe_sample_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_probe_sample_impl(
    aether_meta_read_context_t *ctx,
    probe_sample_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i32le(&ctx->reader, &(value->sequence))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i64le(&ctx->reader, &(value->receivedAtMs))) return AETHER_ERR_PROTOCOL;
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_probe_report(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const probe_report_t *value) {

    return client_server_api_serialize_probe_report_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_probe_report_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const probe_report_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->testId);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->firstSequence);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_u32le(tx, tx_capacity, &pos, (uint32_t)value->count);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->samples.length > 0u && value->samples.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_samples_length = (uint64_t)value->samples.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_samples_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_samples_index = 0u; value_samples_index < value->samples.length; ++value_samples_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_probe_sample_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->samples.data[value_samples_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_probe_report(
    aether_meta_read_context_t *ctx,
    probe_report_t *value) {

    return client_server_api_deserialize_probe_report_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_probe_report_impl(
    aether_meta_read_context_t *ctx,
    probe_report_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_i32le(&ctx->reader, &(value->testId))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i32le(&ctx->reader, &(value->firstSequence))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_i32le(&ctx->reader, &(value->count))) return AETHER_ERR_PROTOCOL;
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        probe_sample_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(probe_sample_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (probe_sample_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(probe_sample_t), _Alignof(probe_sample_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(probe_sample_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_probe_sample_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->samples).data = _meta_buf_1;
        (value->samples).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_finish_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const finish_result_t *value) {

    return client_server_api_serialize_finish_result_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_finish_result_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const finish_result_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->alias);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_cloud_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->cloud)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_finish_result(
    aether_meta_read_context_t *ctx,
    finish_result_t *value) {

    return client_server_api_deserialize_finish_result_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_finish_result_impl(
    aether_meta_read_context_t *ctx,
    finish_result_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->alias))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->uid))) return AETHER_ERR_PROTOCOL;
    {
        aether_status_t meta_status = client_server_api_deserialize_cloud_impl(
            ctx,
            &(value->cloud),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_registration_direct_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const registration_direct_result_t *value) {

    return client_server_api_serialize_registration_direct_result_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_registration_direct_result_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const registration_direct_result_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->alias);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->uid);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_cloud_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->cloud)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_registration_direct_result(
    aether_meta_read_context_t *ctx,
    registration_direct_result_t *value) {

    return client_server_api_deserialize_registration_direct_result_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_registration_direct_result_impl(
    aether_meta_read_context_t *ctx,
    registration_direct_result_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->alias))) return AETHER_ERR_PROTOCOL;
    if (!aether_meta_read_uuid(&ctx->reader, &(value->uid))) return AETHER_ERR_PROTOCOL;
    {
        aether_status_t meta_status = client_server_api_deserialize_cloud_impl(
            ctx,
            &(value->cloud),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    return AETHER_OK;
}

aether_status_t client_server_api_serialize_recovery_result(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const recovery_result_t *value) {

    return client_server_api_serialize_recovery_result_impl(
        tx,
        tx_capacity,
        position,
        value,
        0u);
}

static aether_status_t client_server_api_serialize_recovery_result_impl(
    uint8_t *tx,
    size_t tx_capacity,
    size_t *position,
    const recovery_result_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (position == NULL || value == NULL ||
        (tx_capacity != 0u && tx == NULL)) {
        return AETHER_ERR_ARGUMENT;
    }

    if (*position > tx_capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    size_t pos = *position;
    if (value == NULL) return AETHER_ERR_ARGUMENT;
    {
        aether_status_t meta_status = aether_meta_write_uuid(tx, tx_capacity, &pos, value->alias);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        aether_status_t meta_status = client_server_api_serialize_cloud_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->cloud)),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    if (value->servers.length > 0u && value->servers.data == NULL) return AETHER_ERR_ARGUMENT;
    uint64_t value_servers_length = (uint64_t)value->servers.length;
    {
        aether_status_t meta_status = aether_meta_write_pack(tx, tx_capacity, &pos, value_servers_length);
        if (meta_status != AETHER_OK) return meta_status;
    }
    for (size_t value_servers_index = 0u; value_servers_index < value->servers.length; ++value_servers_index) {
    {
        aether_status_t meta_status = client_server_api_serialize_server_descriptor_impl(
            tx,
            tx_capacity,
            &pos,
            (&(value->servers.data[value_servers_index])),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    }

    *position = pos;
    return AETHER_OK;
}

aether_status_t client_server_api_deserialize_recovery_result(
    aether_meta_read_context_t *ctx,
    recovery_result_t *value) {

    return client_server_api_deserialize_recovery_result_impl(
        ctx,
        value,
        0u);
}

static aether_status_t client_server_api_deserialize_recovery_result_impl(
    aether_meta_read_context_t *ctx,
    recovery_result_t *value,
    unsigned int recursion_depth) {

    if (recursion_depth >= AETHER_META_MAX_RECURSION_DEPTH) {
        return AETHER_ERR_OVERFLOW;
    }

    if (ctx == NULL || value == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    memset(value, 0, sizeof(*value));

    {
    if (!aether_meta_read_uuid(&ctx->reader, &(value->alias))) return AETHER_ERR_PROTOCOL;
    {
        aether_status_t meta_status = client_server_api_deserialize_cloud_impl(
            ctx,
            &(value->cloud),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
    {
        uint64_t _meta_len64_1 = 0u;
        if (!aether_meta_read_pack(&ctx->reader, &_meta_len64_1)) return AETHER_ERR_PROTOCOL;
        if (_meta_len64_1 > (uint64_t)SIZE_MAX) return AETHER_ERR_PROTOCOL;
        size_t _meta_len_1 = (size_t)_meta_len64_1;
        server_descriptor_t *_meta_buf_1 = NULL;
        if (_meta_len_1 > 0u) {
            if (_meta_len_1 > SIZE_MAX / sizeof(server_descriptor_t)) return AETHER_ERR_PROTOCOL;
            _meta_buf_1 = (server_descriptor_t *)aether_meta_scratch_alloc(ctx, _meta_len_1 * sizeof(server_descriptor_t), _Alignof(server_descriptor_t));
            if (_meta_buf_1 == NULL) return AETHER_ERR_OVERFLOW;
            memset(_meta_buf_1, 0, _meta_len_1 * sizeof(server_descriptor_t));
        }
        for (size_t _meta_i_1 = 0u; _meta_i_1 < _meta_len_1; ++_meta_i_1) {
    {
        aether_status_t meta_status = client_server_api_deserialize_server_descriptor_impl(
            ctx,
            &(_meta_buf_1[_meta_i_1]),
            recursion_depth + 1u);
        if (meta_status != AETHER_OK) return meta_status;
    }
        }
        (value->servers).data = _meta_buf_1;
        (value->servers).length = _meta_len_1;
    }
    }

    return AETHER_OK;
}

