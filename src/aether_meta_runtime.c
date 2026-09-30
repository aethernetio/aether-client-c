

/*
 * Shared out-of-line writer half of the generated-code wire runtime.
 *
 * These helpers are deliberately small, bounded and allocation-free. Production
 * generated send paths reuse them enough that sharing reduces linked MCU flash.
 *
 * Reader primitives live as static inline functions in
 * include/aether_meta_runtime.h. That split is intentional and was chosen from
 * linked ESP32 size measurements rather than source-line count.
 */
#include "aether_meta_runtime.h"

#include <limits.h>
#include <string.h>


static bool writer_has(
    size_t capacity,
    size_t position,
    size_t length) {

    return position <= capacity &&
           length <= capacity - position;
}

aether_status_t aether_meta_write_u8(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint8_t value) {

    if (data == NULL || position == NULL) {
        return AETHER_ERR_ARGUMENT;
    }
    if (!writer_has(capacity, *position, 1u)) {
        return AETHER_ERR_OVERFLOW;
    }

    data[(*position)++] = value;
    return AETHER_OK;
}

aether_status_t aether_meta_write_u16le(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint16_t value) {

    if (data == NULL || position == NULL) {
        return AETHER_ERR_ARGUMENT;
    }
    if (!writer_has(capacity, *position, 2u)) {
        return AETHER_ERR_OVERFLOW;
    }

    data[(*position)++] = (uint8_t)value;
    data[(*position)++] = (uint8_t)(value >> 8u);
    return AETHER_OK;
}

aether_status_t aether_meta_write_u32le(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint32_t value) {

    if (data == NULL || position == NULL) {
        return AETHER_ERR_ARGUMENT;
    }
    if (!writer_has(capacity, *position, 4u)) {
        return AETHER_ERR_OVERFLOW;
    }

    for (unsigned int i = 0u; i < 4u; ++i) {
        data[(*position)++] =
            (uint8_t)(value >> (i * 8u));
    }
    return AETHER_OK;
}

aether_status_t aether_meta_write_u64le(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint64_t value) {

    if (data == NULL || position == NULL) {
        return AETHER_ERR_ARGUMENT;
    }
    if (!writer_has(capacity, *position, 8u)) {
        return AETHER_ERR_OVERFLOW;
    }

    for (unsigned int i = 0u; i < 8u; ++i) {
        data[(*position)++] =
            (uint8_t)(value >> (i * 8u));
    }
    return AETHER_OK;
}

aether_status_t aether_meta_write_raw(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    const uint8_t *value,
    size_t length) {

    if (data == NULL ||
        position == NULL ||
        (value == NULL && length != 0u)) {

        return AETHER_ERR_ARGUMENT;
    }
    if (!writer_has(capacity, *position, length)) {
        return AETHER_ERR_OVERFLOW;
    }

    if (length != 0u) {
        memcpy(data + *position, value, length);
    }
    *position += length;
    return AETHER_OK;
}

aether_status_t aether_meta_write_pack(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint64_t value) {

    if (value < UINT64_C(251)) {
        return aether_meta_write_u8(
            data, capacity, position, (uint8_t)value);
    }

    if (value < UINT64_C(1515)) {
        uint64_t packed = value - UINT64_C(251);
        aether_status_t status =
            aether_meta_write_u8(
                data,
                capacity,
                position,
                (uint8_t)(((packed >> 8u) & UINT64_C(0xff)) +
                          UINT64_C(251)));
        if (status != AETHER_OK) {
            return status;
        }
        return aether_meta_write_u8(
            data, capacity, position, (uint8_t)packed);
    }

    if (value < UINT64_C(1049835)) {
        uint64_t packed = value - UINT64_C(1515);
        aether_status_t status =
            aether_meta_write_u8(
                data, capacity, position, UINT8_C(255));
        if (status != AETHER_OK) {
            return status;
        }
        status =
            aether_meta_write_u8(
                data,
                capacity,
                position,
                (uint8_t)(((packed >> 16u) - UINT64_C(251)) +
                          UINT64_C(1515)));
        if (status != AETHER_OK) {
            return status;
        }
        return aether_meta_write_u16le(
            data,
            capacity,
            position,
            (uint16_t)packed);
    }

    const uint64_t limit =
        UINT64_C(1049835) +
        UINT64_C(4294967296) * UINT64_C(256);

    if (value >= limit) {
        return AETHER_ERR_OVERFLOW;
    }

    uint64_t packed =
        value - UINT64_C(1049835);

    aether_status_t status =
        aether_meta_write_u8(
            data, capacity, position, UINT8_C(255));
    if (status != AETHER_OK) {
        return status;
    }

    status =
        aether_meta_write_u8(
            data, capacity, position, UINT8_C(255));
    if (status != AETHER_OK) {
        return status;
    }

    uint16_t high =
        (uint16_t)(((packed >> 32u) - UINT64_C(1515)) +
                   UINT64_C(1049835));

    status =
        aether_meta_write_u16le(
            data, capacity, position, high);
    if (status != AETHER_OK) {
        return status;
    }

    return aether_meta_write_u32le(
        data,
        capacity,
        position,
        (uint32_t)packed);
}

aether_status_t aether_meta_write_bytes(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    const uint8_t *value,
    size_t length) {

    if (value == NULL && length != 0u) {
        return AETHER_ERR_ARGUMENT;
    }

    aether_status_t status =
        aether_meta_write_pack(
            data,
            capacity,
            position,
            (uint64_t)length);

    if (status != AETHER_OK) {
        return status;
    }

    return aether_meta_write_raw(
        data,
        capacity,
        position,
        value,
        length);
}

aether_status_t aether_meta_write_uuid(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    aether_uuid_t value) {

    if (data == NULL || position == NULL) {
        return AETHER_ERR_ARGUMENT;
    }
    if (!writer_has(capacity, *position, 16u)) {
        return AETHER_ERR_OVERFLOW;
    }

    for (int shift = 56; shift >= 0; shift -= 8) {
        data[(*position)++] =
            (uint8_t)(value.msb >> (unsigned int)shift);
    }
    for (int shift = 56; shift >= 0; shift -= 8) {
        data[(*position)++] =
            (uint8_t)(value.lsb >> (unsigned int)shift);
    }

    return AETHER_OK;
}