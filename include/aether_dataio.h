
#ifndef AETHER_DATAIO_H
#define AETHER_DATAIO_H


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#ifndef AETHER_META_MAX_RECURSION_DEPTH
#define AETHER_META_MAX_RECURSION_DEPTH 32u
#endif


#include "aether_client.h"


#ifdef __cplusplus
extern "C" {
#endif

/* ==== Reader (DataIn) ==== */

typedef struct aether_data_in {
    const uint8_t *data;
    size_t         size;
    size_t         pos;
    bool           error;

    /* Optional scratch allocator used by generated dynamic-array decode.
       If NULL, decoding a dynamic non-flat array fails. Callback must
       return memory that stays valid until the enclosing decode completes. */
    void *scratch_ctx;
    void *(*scratch_alloc)(void *ctx, size_t bytes, size_t align);
} aether_data_in_t;


/* ==== Writer (DataOut) ==== */
typedef struct aether_data_out {
    uint8_t *data;
    size_t   capacity;
    size_t   pos;
    bool     error;
} aether_data_out_t;

/* ================= Reader API ================= */

void   aether_data_in_init(aether_data_in_t *in, const uint8_t *data, size_t size);
void   aether_data_in_reset(aether_data_in_t *in);

size_t aether_data_in_size_for_read(const aether_data_in_t *in);
bool   aether_data_in_is_empty(const aether_data_in_t *in);
bool   aether_data_in_is_readable(const aether_data_in_t *in);
bool   aether_data_in_error(const aether_data_in_t *in);
void   aether_data_in_clear_error(aether_data_in_t *in);

size_t aether_data_in_read(aether_data_in_t *in, uint8_t *dst, size_t off, size_t len);
void   aether_data_in_read_bulk(aether_data_in_t *in, uint8_t *dst, size_t len);
size_t aether_data_in_read_int_array(aether_data_in_t *in, int32_t *dst, size_t off, size_t len);
size_t aether_data_in_to_array(aether_data_in_t *in, uint8_t *dst, size_t dst_cap);

void   aether_data_in_skip_bytes(aether_data_in_t *in, size_t n);
void   aether_data_in_skip_all(aether_data_in_t *in);

uint8_t  aether_data_in_read_ubyte(aether_data_in_t *in);
int8_t   aether_data_in_read_byte(aether_data_in_t *in);
uint16_t aether_data_in_read_ushort(aether_data_in_t *in);
int16_t  aether_data_in_read_short(aether_data_in_t *in);
uint16_t aether_data_in_read_char(aether_data_in_t *in);
int32_t  aether_data_in_read_int(aether_data_in_t *in);
uint32_t aether_data_in_read_uint(aether_data_in_t *in);
int64_t  aether_data_in_read_long(aether_data_in_t *in);
float    aether_data_in_read_float(aether_data_in_t *in);
double   aether_data_in_read_double(aether_data_in_t *in);
bool     aether_data_in_read_boolean(aether_data_in_t *in);

ptrdiff_t aether_data_in_index_of(const aether_data_in_t *in, size_t limit, uint8_t val);

/* ================= Writer API ================= */

void   aether_data_out_init(aether_data_out_t *out, uint8_t *data, size_t capacity);
void   aether_data_out_clear(aether_data_out_t *out);
void   aether_data_out_clear_error(aether_data_out_t *out);

size_t aether_data_out_size_for_write(const aether_data_out_t *out);
bool   aether_data_out_is_writable(const aether_data_out_t *out);
bool   aether_data_out_error(const aether_data_out_t *out);

size_t aether_data_out_write(aether_data_out_t *out, const uint8_t *src, size_t off, size_t len);
size_t aether_data_out_write_int_array(aether_data_out_t *out, const int32_t *src, size_t off, size_t len);


bool aether_data_out_write_boolean(aether_data_out_t *out, bool v);
bool aether_data_out_write_byte(aether_data_out_t *out, uint8_t v);
bool aether_data_out_write_short(aether_data_out_t *out, int16_t v);
bool aether_data_out_write_ushort(aether_data_out_t *out, uint16_t v);
bool aether_data_out_write_char(aether_data_out_t *out, uint16_t v);
bool aether_data_out_write_int(aether_data_out_t *out, int32_t v);
bool aether_data_out_write_uint(aether_data_out_t *out, uint32_t v);
bool aether_data_out_write_long(aether_data_out_t *out, int64_t v);
bool aether_data_out_write_float(aether_data_out_t *out, float v);
bool aether_data_out_write_double(aether_data_out_t *out, double v);



/* ============ PackInt (tiered compact integer) ============ */

/*
 * Compact integer encoding used by Aether PackInt / SerializerPackNumber
 * / DeserializerPackNumber / PackNumberStreamParser.
 *
 * Two Java API shapes share the SAME wire format:
 *   1) sync:      SerializerPackNumber.put(DataOut, long)
 *                 DeserializerPackNumber.put(DataIn) -> Number
 *   2) streaming: PackNumberStreamParser.nextByte(byte) -> boolean
 *
 * Both encode/decode identical bytes. C mirrors this with two APIs:
 *   - aether_data_out_write_pack / aether_data_in_read_pack    (sync)
 *   - aether_pack_parser_feed + aether_pack_parser_value       (stream)
 *
 * Encoded sizes: 1, 2, 4 or 8 bytes.
 * Supported range: [0, AETHER_PACK_U64 - 1].
 */

#define AETHER_PACK_U8   251u
#define AETHER_PACK_U16  1515u
#define AETHER_PACK_U32  1049835u
#define AETHER_PACK_U64  1099512677611ULL

/* Returns encoded size in bytes (1/2/4/8), or 0 if v exceeds AETHER_PACK_U64-1. */
int aether_pack_size(uint64_t v);

/* Sync encode/decode over aether_data_in_t / aether_data_out_t. */
bool     aether_data_out_write_pack(aether_data_out_t *out, uint64_t v);
uint64_t aether_data_in_read_pack(aether_data_in_t *in);

/* Streaming parser: feed one byte at a time (non-blocking). */
typedef struct aether_pack_parser {
    uint32_t state;       /* 0..3 */
    uint32_t bytes_read;  /* byte counter inside short/int stages */
    uint64_t val;
    uint64_t prefix;
    bool     done;
    bool     error;
} aether_pack_parser_t;

void     aether_pack_parser_init(aether_pack_parser_t *p);
void     aether_pack_parser_reset(aether_pack_parser_t *p);
/* Feed one byte; returns true once a complete value has been decoded. */
bool     aether_pack_parser_feed(aether_pack_parser_t *p, uint8_t b);
uint64_t aether_pack_parser_value(const aether_pack_parser_t *p);
bool     aether_pack_parser_done(const aether_pack_parser_t *p);
bool     aether_pack_parser_error(const aether_pack_parser_t *p);


/* ============ TieredInt (parameterized tiered integer) ============ */

/*
 * Parameterized tiered integer matching TieredIntFastMeta from the Java
 * side (io.aether.net.fastMeta.TieredIntFastMeta).
 *
 * Configuration (compile-time schema data):
 *   min_tier_bytes : 1, 2 or 4
 *   limits         : 1..3 strictly increasing uint32 values
 *   (min_tier_bytes << limit_count) <= 8
 *   first limit < (1 << (8*min_tier_bytes)) - 1
 *
 * Value range: [0, INT64_MAX], and additionally <= the configuration's
 * max encodable value (same bound as Java `maxEncodable`).
 *
 * Wire layout: min_tier_bytes bytes of LE head, followed by
 * `min_tier_bytes << i` LE bytes for every lower tier i that was used.
 *
 * Both functions return true on success, false on any error (bad
 * configuration, value out of range, truncated input, overflow). On error
 * the reader/writer error flag is set and the cursor is left in an
 * unspecified position; callers should discard the whole message.
 */

bool aether_data_out_write_tiered_int(
    aether_data_out_t *out,
    uint64_t value,
    uint8_t min_tier_bytes,
    const uint32_t *limits,
    size_t limit_count);

bool aether_data_in_read_tiered_int(
    aether_data_in_t *in,
    uint64_t *value,
    uint8_t min_tier_bytes,
    const uint32_t *limits,
    size_t limit_count);


/* ============ Element converter ABI ============ */

/*
 * Single ABI shared by every generated serializer / deserializer.
 *
 *   out / in   : dataio stream; the function advances the cursor itself.
 *   value      : pointer to the value. For a scalar it is &scalar;
 *                for a hierarchy / object it is a pointer to the object.
 *   depth      : recursion depth. Top-level entry uses 0. Generated code
 *                increments when descending into a field of a nested
 *                structural type.
 *
 * Returns false on any error; the dataio stream carries the sticky error
 * flag. depth is used by recursive types to bound stack usage.
 */
typedef bool (*aether_serialize_fn)(
    aether_data_out_t *out,
    const void *value,
    unsigned int depth);

typedef bool (*aether_deserialize_fn)(
    aether_data_in_t *in,
    void *value,
    unsigned int depth);

/* ============ Array I/O (FastMeta-style) ============ */

/*
 * Arrays on the wire:
 *   T[]   dynamic : pack(N) + N elements
 *   T[N]  static  : N elements, no length prefix
 *
 * Two view shapes:
 *
 *   1) Flat primitives (bool, byte, short, int, long, float, double, char).
 *      Handled as a contiguous block. byte[] is effectively one memcpy;
 *      other widths iterate dataio primitive helpers. There is no
 *      per-element callback.
 *
 *   2) Object arrays: array of pointers to individually-typed elements of
 *      arbitrary size. The caller supplies an element serializer /
 *      deserializer and the generic loop only dereferences items[i].
 *
 * All functions return true on success; false sets the dataio error flag.
 * On read, if the encoded N exceeds view->capacity the function fails and
 * leaves the cursor just past the length prefix - the caller must discard
 * the whole message.
 */

typedef enum {
    AETHER_FLAT_BOOL = 0,   /* 1 byte each */
    AETHER_FLAT_I8,         /* 1 byte each */
    AETHER_FLAT_U8,         /* 1 byte each */
    AETHER_FLAT_I16,        /* 2 bytes LE */
    AETHER_FLAT_U16,        /* 2 bytes LE */
    AETHER_FLAT_CHAR,       /* 2 bytes LE (UTF-16 code unit) */
    AETHER_FLAT_I32,        /* 4 bytes LE */
    AETHER_FLAT_U32,        /* 4 bytes LE */
    AETHER_FLAT_I64,        /* 8 bytes LE */
    AETHER_FLAT_U64,        /* 8 bytes LE */
    AETHER_FLAT_F32,        /* 4 bytes LE (IEEE 754) */
    AETHER_FLAT_F64         /* 8 bytes LE (IEEE 754) */
} aether_flat_kind_t;

typedef struct {
    size_t             length;   /* element count */
    const void        *data;     /* caller-owned buffer */
    aether_flat_kind_t kind;
} aether_const_flat_view_t;

typedef struct {
    size_t             capacity; /* in : max elements the buffer fits */
    size_t             length;   /* out: real count on read */
    void              *data;     /* caller-owned buffer */
    aether_flat_kind_t kind;
} aether_flat_view_t;

typedef struct {
    size_t        length;
    void *const  *items;   /* array of pointers to individual elements */
} aether_const_obj_view_t;

typedef struct {
    size_t   capacity;     /* in : max slots in items[] */
    size_t   length;       /* out: real count on read */
    void   **items;        /* array of pointers to caller-owned elements */
} aether_obj_view_t;

/* ---- Dynamic arrays: pack(N) + N elements ---- */

bool aether_data_out_write_flat_array(
    aether_data_out_t *out,
    const aether_const_flat_view_t *view,
    unsigned int depth);

bool aether_data_in_read_flat_array(
    aether_data_in_t *in,
    aether_flat_view_t *view,
    unsigned int depth);

bool aether_data_out_write_obj_array(
    aether_data_out_t *out,
    aether_serialize_fn item_fn,
    const aether_const_obj_view_t *view,
    unsigned int depth);

bool aether_data_in_read_obj_array(
    aether_data_in_t *in,
    aether_deserialize_fn item_fn,
    aether_obj_view_t *view,
    unsigned int depth);

/* ---- Static arrays: N elements, no length prefix ---- */

bool aether_data_out_write_static_flat_array(
    aether_data_out_t *out,
    const aether_const_flat_view_t *view,
    unsigned int depth);

bool aether_data_in_read_static_flat_array(
    aether_data_in_t *in,
    aether_flat_view_t *view,   /* view->capacity must equal N on entry */
    unsigned int depth);

bool aether_data_out_write_static_obj_array(
    aether_data_out_t *out,
    aether_serialize_fn item_fn,
    const aether_const_obj_view_t *view,
    unsigned int depth);

bool aether_data_in_read_static_obj_array(
    aether_data_in_t *in,
    aether_deserialize_fn item_fn,
    aether_obj_view_t *view,    /* view->capacity must equal N on entry */
    unsigned int depth);


/* ============ UUID ============ */

/*
 * Aether UUID wire representation:
 *   8 bytes of MSB in big-endian order, then
 *   8 bytes of LSB in big-endian order.
 * Total 16 bytes. Matches aether_meta_write_uuid / aether_meta_read_uuid.
 */

bool         aether_data_out_write_uuid(aether_data_out_t *out, aether_uuid_t v);
aether_uuid_t aether_data_in_read_uuid(aether_data_in_t *in);

/* ============ Length-prefixed byte view ============ */

/*
 * pack(length) followed by length raw bytes.
 * On read, if the encoded length exceeds dst_capacity the function fails
 * and leaves the cursor past the length prefix; the caller discards the
 * whole message.
 *
 * write: returns true on success.
 * read : returns true on success and writes the real count to *out_length.
 */

bool aether_data_out_write_bytes(aether_data_out_t *out, const void *data, size_t length);
bool aether_data_in_read_bytes (aether_data_in_t  *in,  void *dst, size_t dst_capacity, size_t *out_length);

/* Borrowed view: pack(length) + length bytes, dst points INTO in->data. */
bool aether_data_in_read_view(aether_data_in_t *in, const uint8_t **dst, size_t *out_length);


#ifdef __cplusplus
}
#endif

#endif /* AETHER_DATAIO_H */