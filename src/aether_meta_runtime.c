

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



/*
 * Validate canonical TieredInt configuration and calculate the largest value
 * representable by its wire shape. Arithmetic saturates at UINT64_MAX because
 * schema limits are uint32 and callers only need an upper bound.
 */
static bool tiered_int_config(
    uint8_t base_bytes,
    const uint32_t *limits,
    size_t limit_count,
    uint64_t *max_encodable) {

    if (limits == NULL ||
        max_encodable == NULL ||
        (base_bytes != 1u &&
         base_bytes != 2u &&
         base_bytes != 4u) ||
        limit_count == 0u ||
        limit_count > 3u ||
        ((size_t)base_bytes << limit_count) > 8u) {

        return false;
    }

    for (size_t i = 1u; i < limit_count; ++i) {
        if (limits[i] <= limits[i - 1u]) {
            return false;
        }
    }

    const uint64_t word =
        UINT64_C(1) << (8u * base_bytes);

    const uint64_t header_max =
        word - UINT64_C(1);

    if ((uint64_t)limits[0] >= header_max) {
        return false;
    }

    uint64_t span =
        header_max -
        (uint64_t)limits[0] -
        UINT64_C(1);

    uint64_t tail =
        (uint64_t)limits[0] +
        UINT64_C(1) +
        header_max;

    uint64_t max_value;

    if (span != 0u &&
        word > (UINT64_MAX - tail) / span) {

        max_value = UINT64_MAX;
    } else {
        max_value =
            span * word + tail;
    }

    for (size_t i = 1u; i < limit_count; ++i) {
        const uint64_t limit =
            (uint64_t)limits[i];

        if (limit > max_value) {
            return false;
        }

        const unsigned int low_bytes =
            (unsigned int)base_bytes << i;

        const uint64_t modulus =
            UINT64_C(1) << (8u * low_bytes);

        span =
            max_value -
            limit -
            UINT64_C(1);

        tail =
            limit +
            UINT64_C(1) +
            modulus -
            UINT64_C(1);

        if (span != 0u &&
            modulus > (UINT64_MAX - tail) / span) {

            max_value = UINT64_MAX;
        } else {
            max_value =
                span * modulus + tail;
        }
    }

    *max_encodable =
        max_value;

    return true;
}


static aether_status_t tiered_int_write_le(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint64_t value,
    unsigned int bytes) {

    if (data == NULL ||
        position == NULL ||
        !writer_has(
            capacity,
            *position,
            bytes)) {

        return data == NULL ||
               position == NULL
            ? AETHER_ERR_ARGUMENT
            : AETHER_ERR_OVERFLOW;
    }

    for (unsigned int i = 0u;
         i < bytes;
         ++i) {

        data[(*position)++] =
            (uint8_t)(
                value >>
                (8u * i));
    }

    return AETHER_OK;
}


static bool tiered_int_read_le(
    aether_meta_reader_t *reader,
    unsigned int bytes,
    uint64_t *value) {

    if (reader == NULL ||
        value == NULL ||
        reader->position > reader->length ||
        bytes > reader->length - reader->position) {

        return false;
    }

    uint64_t result = 0u;

    for (unsigned int i = 0u;
         i < bytes;
         ++i) {

        result |=
            (uint64_t)
                reader->data[reader->position++]
            << (8u * i);
    }

    *value = result;
    return true;
}


aether_status_t aether_meta_write_tiered_int(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint64_t value,
    uint8_t min_tier_bytes,
    const uint32_t *limits,
    size_t limit_count) {

    if (position == NULL ||
        (capacity != 0u && data == NULL)) {

        return AETHER_ERR_ARGUMENT;
    }

    uint64_t max_encodable = 0u;

    if (!tiered_int_config(
            min_tier_bytes,
            limits,
            limit_count,
            &max_encodable) ||
        value > (uint64_t)INT64_MAX ||
        value > max_encodable) {

        return AETHER_ERR_ARGUMENT;
    }

    uint64_t current =
        value;

    uint64_t lows[3] = {
        0u,
        0u,
        0u
    };

    int highest = -1;

    for (size_t offset = 0u;
         offset < limit_count;
         ++offset) {

        const size_t i =
            limit_count -
            offset -
            1u;

        if (current <=
            (uint64_t)limits[i]) {

            continue;
        }

        const unsigned int low_bytes =
            (unsigned int)min_tier_bytes << i;

        const uint64_t modulus =
            UINT64_C(1) << (8u * low_bytes);

        const uint64_t delta =
            current -
            (uint64_t)limits[i] -
            UINT64_C(1);

        lows[i] =
            delta % modulus;

        current =
            delta / modulus +
            (uint64_t)limits[i] +
            UINT64_C(1);

        if ((int)i > highest) {
            highest = (int)i;
        }
    }

    aether_status_t status =
        tiered_int_write_le(
            data,
            capacity,
            position,
            current,
            min_tier_bytes);

    if (status != AETHER_OK) {
        return status;
    }

    for (int i = 0;
         i <= highest;
         ++i) {

        status =
            tiered_int_write_le(
                data,
                capacity,
                position,
                lows[i],
                (unsigned int)min_tier_bytes << (unsigned int)i);

        if (status != AETHER_OK) {
            return status;
        }
    }

    return AETHER_OK;
}


bool aether_meta_read_tiered_int(
    aether_meta_reader_t *reader,
    uint64_t *value,
    uint8_t min_tier_bytes,
    const uint32_t *limits,
    size_t limit_count) {

    if (reader == NULL ||
        value == NULL) {

        return false;
    }

    uint64_t max_encodable = 0u;

    if (!tiered_int_config(
            min_tier_bytes,
            limits,
            limit_count,
            &max_encodable)) {

        return false;
    }

    uint64_t current = 0u;

    if (!tiered_int_read_le(
            reader,
            min_tier_bytes,
            &current)) {

        return false;
    }

    if (current <=
        (uint64_t)limits[0]) {

        *value = current;
        return true;
    }

    for (size_t i = 0u;
         i < limit_count;
         ++i) {

        const uint64_t limit =
            (uint64_t)limits[i];

        if (current <= limit) {
            return false;
        }

        const unsigned int low_bytes =
            (unsigned int)min_tier_bytes << i;

        const uint64_t modulus =
            UINT64_C(1) << (8u * low_bytes);

        uint64_t low = 0u;

        if (!tiered_int_read_le(
                reader,
                low_bytes,
                &low)) {

            return false;
        }

        const uint64_t high =
            current -
            limit -
            UINT64_C(1);

        const uint64_t tail =
            limit +
            UINT64_C(1) +
            low;

        if (high != 0u &&
            modulus >
                (UINT64_MAX - tail) /
                high) {

            return false;
        }

        current =
            high * modulus +
            tail;

        if (i + 1u == limit_count ||
            current <=
                (uint64_t)limits[i + 1u]) {

            if (current >
                    max_encodable ||
                current >
                    (uint64_t)INT64_MAX) {

                return false;
            }

            *value = current;
            return true;
        }
    }

    return false;
}



aether_status_t aether_meta_collapse_queue_register(
    aether_meta_collapse_queue_t *queue,
    void *ctx,
    aether_meta_collapse_serialize_fn serialize,
    aether_meta_collapse_commit_fn commit) {

    if (queue == NULL ||
        ctx == NULL ||
        serialize == NULL ||
        commit == NULL ||
        queue->count > queue->capacity ||
        (queue->capacity != 0u && queue->items == NULL)) {

        return AETHER_ERR_ARGUMENT;
    }

    for (size_t i = 0u; i < queue->count; ++i) {
        aether_meta_collapse_pending_t *pending =
            &queue->items[i];

        if (pending->ctx != ctx) {
            continue;
        }

        return pending->serialize == serialize &&
               pending->commit == commit
            ? AETHER_OK
            : AETHER_ERR_ARGUMENT;
    }

    if (queue->count >= queue->capacity) {
        return AETHER_ERR_OVERFLOW;
    }

    aether_meta_collapse_pending_t *pending =
        &queue->items[queue->count++];

    pending->ctx = ctx;
    pending->serialize = serialize;
    pending->commit = commit;

    return AETHER_OK;
}


void aether_meta_collapse_queue_cancel(
    aether_meta_collapse_queue_t *queue,
    void *ctx) {

    if (queue == NULL ||
        ctx == NULL ||
        queue->count > queue->capacity ||
        (queue->count != 0u && queue->items == NULL)) {

        return;
    }

    for (size_t i = 0u; i < queue->count; ++i) {
        if (queue->items[i].ctx != ctx) {
            continue;
        }

        for (size_t j = i + 1u;
             j < queue->count;
             ++j) {

            queue->items[j - 1u] =
                queue->items[j];
        }

        queue->count--;
        return;
    }
}


aether_status_t aether_meta_collapse_queue_flush(
    aether_meta_collapse_queue_t *queue,
    void *send_ctx,
    aether_meta_collapse_send_fn send,
    uint8_t *data,
    size_t capacity) {

    if (queue == NULL ||
        queue->count > queue->capacity ||
        (queue->count != 0u && queue->items == NULL)) {

        return AETHER_ERR_ARGUMENT;
    }

    if (queue->count == 0u) {
        return AETHER_OK;
    }

    if (send == NULL || data == NULL) {
        return AETHER_ERR_ARGUMENT;
    }

    size_t position = 0u;

    for (size_t i = 0u; i < queue->count; ++i) {
        aether_meta_collapse_pending_t *pending =
            &queue->items[i];

        if (pending->ctx == NULL ||
            pending->serialize == NULL ||
            pending->commit == NULL) {

            return AETHER_ERR_ARGUMENT;
        }

        aether_status_t status =
            pending->serialize(
                pending->ctx,
                data,
                capacity,
                &position);

        if (status != AETHER_OK) {
            return status;
        }
    }

    aether_status_t status =
        send(
            send_ctx,
            data,
            position);

    if (status != AETHER_OK) {
        return status;
    }

    for (size_t i = 0u; i < queue->count; ++i) {
        queue->items[i].commit(
            queue->items[i].ctx);
    }

    queue->count = 0u;

    return AETHER_OK;
}
