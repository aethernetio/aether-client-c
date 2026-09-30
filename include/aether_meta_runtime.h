

/*
- Minimal handwritten wire runtime used by generated ADSL bindings.
-
- This is intentionally not a reflection/runtime-schema layer. Generated code
- knows the complete schema at compile time and calls only the primitive
- operations required by that schema.
-
- Writer contract:
-
  - data points to caller-owned output storage;
-
  - capacity is the total size of that storage;
-
  - *position is the current cursor and is updated on success;
-
  - no heap allocation is performed;
-
  - overflow is reported as AETHER_ERR_OVERFLOW.
-
- Reader contract:
-
  - aether_meta_reader_t borrows immutable input bytes;
-
  - position is advanced while decoding;
-
  - false means malformed/truncated input;
-
  - callers treat a failed decode as a failed frame; individual primitive
- readers do not promise cursor rollback after partial input.
-
- Writers are out-of-line because generated production code reuses them at many
- call sites. Readers are static inline because current production decoding has
- few call sites and allowing -Os to inline them produces smaller linked MCU
- firmware without duplicating handwritten implementations.
   */
#ifndef AETHER_META_RUNTIME_H
#define AETHER_META_RUNTIME_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "aether_client.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
- Borrowed-input cursor.
-
- data/length describe one immutable byte span. position is the next unread
- byte and must never exceed length.
   */
  typedef struct {
   const uint8_t *data;
   size_t length;
   size_t position;
  } aether_meta_reader_t;



/*
- Fixed-width integer writers use explicit little-endian wire order.
-
- They never serialize host object representation directly, so output is stable
- across CPU endianness and alignment rules.
   */
  aether_status_t aether_meta_write_u8(
   uint8_t *data,
   size_t capacity,
   size_t *position,
   uint8_t value);
aether_status_t aether_meta_write_u16le(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint16_t value);
aether_status_t aether_meta_write_u32le(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint32_t value);
aether_status_t aether_meta_write_u64le(
    uint8_t *data,
    size_t capacity,
    size_t *position,
    uint64_t value);
/*
- Append an exact raw byte span without a length prefix.
-
- value may be NULL only when length is zero.
   */
  aether_status_t aether_meta_write_raw(
   uint8_t *data,
   size_t capacity,
   size_t *position,
   const uint8_t *value,
   size_t length);
/*
- Encode Aether's canonical compact unsigned integer representation.
-
- This is the Aether/FastMeta pack format, not LEB128/protobuf varint.
   */
  aether_status_t aether_meta_write_pack(
   uint8_t *data,
   size_t capacity,
   size_t *position,
   uint64_t value);
/*
- Write a packed length followed by exactly length bytes.
   */
  aether_status_t aether_meta_write_bytes(
   uint8_t *data,
   size_t capacity,
   size_t *position,
   const uint8_t *value,
   size_t length);
/*
- Write the canonical Aether UUID representation.
-
- UUIDs are emitted explicitly as the MSB half followed by the LSB half in
- canonical byte order. Never replace this with memcpy(aether_uuid_t).
   */
  aether_status_t aether_meta_write_uuid(
   uint8_t *data,
   size_t capacity,
   size_t *position,
   aether_uuid_t value);




/*
- Primitive readers below mirror the writer wire rules.
-
- They are kept in this shared handwritten header rather than emitted into each
- generated response source. static inline gives each translation unit the
- option to inline or locally share the primitive according to its -Os cost
- model while preserving a single implementation source.
   */
  static inline bool aether_meta_read_u8(

    aether_meta_reader_t *reader,
    uint8_t *value) {

    if (reader == NULL ||
        value == NULL ||
        reader->position >= reader->length) {

        return false;
    }

    *value =
        reader->data[reader->position++];
    return true;
}

static inline bool aether_meta_read_u16le(
    aether_meta_reader_t *reader,
    uint16_t *value) {

    uint8_t a;
    uint8_t b;

    if (value == NULL ||
        !aether_meta_read_u8(reader, &a) ||
        !aether_meta_read_u8(reader, &b)) {

        return false;
    }

    *value =
        (uint16_t)a |
        ((uint16_t)b << 8u);
    return true;
}

static inline bool aether_meta_read_u32le(
    aether_meta_reader_t *reader,
    uint32_t *value) {

    if (value == NULL) {
        return false;
    }

    uint32_t result = 0u;
    for (unsigned int i = 0u; i < 4u; ++i) {
        uint8_t byte;
        if (!aether_meta_read_u8(reader, &byte)) {
            return false;
        }
        result |=
            (uint32_t)byte << (i * 8u);
    }

    *value = result;
    return true;
}

static inline bool aether_meta_read_u64le(
    aether_meta_reader_t *reader,
    uint64_t *value) {

    if (value == NULL) {
        return false;
    }

    uint64_t result = 0u;
    for (unsigned int i = 0u; i < 8u; ++i) {
        uint8_t byte;
        if (!aether_meta_read_u8(reader, &byte)) {
            return false;
        }
        result |=
            (uint64_t)byte << (i * 8u);
    }

    *value = result;
    return true;
}

static inline bool aether_meta_read_i16le(
    aether_meta_reader_t *reader,
    int16_t *value) {

    uint16_t raw;
    if (value == NULL ||
        !aether_meta_read_u16le(reader, &raw)) {

        return false;
    }

    *value = (int16_t)raw;
    return true;
}

static inline bool aether_meta_read_i32le(
    aether_meta_reader_t *reader,
    int32_t *value) {

    uint32_t raw;
    if (value == NULL ||
        !aether_meta_read_u32le(reader, &raw)) {

        return false;
    }

    *value = (int32_t)raw;
    return true;
}

static inline bool aether_meta_read_i64le(
    aether_meta_reader_t *reader,
    int64_t *value) {

    uint64_t raw;
    if (value == NULL ||
        !aether_meta_read_u64le(reader, &raw)) {

        return false;
    }

    *value = (int64_t)raw;
    return true;
}


/*
- Decode the canonical Aether packed unsigned integer.
-
- The tier boundaries must remain bit-for-bit compatible with
- aether_meta_write_pack(). A truncated tier returns false.
   */
  static inline bool aether_meta_read_pack(

    aether_meta_reader_t *reader,
    uint64_t *value) {

    if (value == NULL) {
        return false;
    }

    const uint64_t u8 = UINT64_C(251);
    const uint64_t u16 = UINT64_C(1515);
    const uint64_t u32 = UINT64_C(1049835);
    const uint64_t u64 =
        u32 + UINT64_C(4294967296) * UINT64_C(256);

    uint8_t first;
    if (!aether_meta_read_u8(reader, &first)) {
        return false;
    }

    uint64_t result = first;
    if (result < u8) {
        *value = result;
        return true;
    }

    uint8_t next;
    if (!aether_meta_read_u8(reader, &next)) {
        return false;
    }

    result =
        ((result - u8) << 8u) +
        u8 +
        next;

    if (result < u16) {
        *value = result;
        return true;
    }

    uint16_t following16;
    if (!aether_meta_read_u16le(
            reader,
            &following16)) {

        return false;
    }

    result =
        ((result - u16) << 16u) +
        u16 +
        following16;

    if (result < u32) {
        *value = result;
        return true;
    }

    uint32_t following32;
    if (!aether_meta_read_u32le(
            reader,
            &following32)) {

        return false;
    }

    result =
        ((result - u32) << 32u) +
        u32 +
        following32;

    if (result >= u64) {
        return false;
    }

    *value = result;
    return true;
}


/*
- Decode a packed byte/string field without copying it.
-
- *data points into reader->data and is valid only as long as the original input
- storage remains valid. The encoded length is checked against SIZE_MAX and the
- remaining reader span before the view is returned.
   */
  static inline bool aether_meta_read_view(

    aether_meta_reader_t *reader,
    const uint8_t **data,
    size_t *length) {

    if (reader == NULL ||
        data == NULL ||
        length == NULL) {

        return false;
    }

    uint64_t count64;
    if (!aether_meta_read_pack(
            reader,
            &count64) ||
        count64 > (uint64_t)SIZE_MAX) {

        return false;
    }

    size_t count =
        (size_t)count64;

    if (reader->position > reader->length ||
        count > reader->length - reader->position) {

        return false;
    }

    *data =
        reader->data + reader->position;
    *length =
        count;
    reader->position +=
        count;

    return true;
}


/*
- Decode the canonical 16-byte UUID representation explicitly.
-
- The two uint64_t fields are reconstructed from bytes so the result is
- independent of host endianness and aether_uuid_t padding/layout.
   */
  static inline bool aether_meta_read_uuid(

    aether_meta_reader_t *reader,
    aether_uuid_t *value) {

    if (reader == NULL ||
        value == NULL ||
        reader->position > reader->length ||
        reader->length - reader->position < 16u) {

        return false;
    }

    uint64_t msb = 0u;
    uint64_t lsb = 0u;

    for (unsigned int i = 0u; i < 8u; ++i) {
        msb =
            (msb << 8u) |
            reader->data[reader->position++];
    }

    for (unsigned int i = 0u; i < 8u; ++i) {
        lsb =
            (lsb << 8u) |
            reader->data[reader->position++];
    }

    value->msb = msb;
    value->lsb = lsb;
    return true;
}


#ifdef __cplusplus
}
#endif

#endif