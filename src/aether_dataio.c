
#include "aether_dataio.h"

/* Part 1: LE primitives + reader. Writer appended in later steps. */


/* ================= Little-endian primitives ================= */

static inline uint16_t le_r16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static inline uint32_t le_r32(const uint8_t *p) {
    return ((uint32_t)p[0])
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static inline uint64_t le_r64(const uint8_t *p) {
    return ((uint64_t)p[0])
         | ((uint64_t)p[1] << 8)
         | ((uint64_t)p[2] << 16)
         | ((uint64_t)p[3] << 24)
         | ((uint64_t)p[4] << 32)
         | ((uint64_t)p[5] << 40)
         | ((uint64_t)p[6] << 48)
         | ((uint64_t)p[7] << 56);
}

static inline void le_w16(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v);
    p[1] = (uint8_t)(v >> 8);
}

static inline void le_w32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v);
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static inline void le_w64(uint8_t *p, uint64_t v) {
    p[0] = (uint8_t)(v);
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
    p[4] = (uint8_t)(v >> 32);
    p[5] = (uint8_t)(v >> 40);
    p[6] = (uint8_t)(v >> 48);
    p[7] = (uint8_t)(v >> 56);
}


static inline uint32_t f32_to_bits(float v) {
    union { float f; uint32_t u; } x; x.f = v;
    uint32_t bits = x.u;
    /* Java Float.floatToIntBits: collapse every NaN payload to canonical NaN. */
    if ((bits & UINT32_C(0x7f800000)) == UINT32_C(0x7f800000) &&
        (bits & UINT32_C(0x007fffff)) != 0u) {
        return UINT32_C(0x7fc00000);
    }
    return bits;
}
static inline float f32_from_bits(uint32_t u) {
    union { float f; uint32_t u; } x; x.u = u; return x.f;
}
static inline uint64_t f64_to_bits(double v) {
    union { double d; uint64_t u; } x; x.d = v;
    uint64_t bits = x.u;
    /* Java Double.doubleToLongBits: collapse every NaN payload to canonical NaN. */
    if ((bits & UINT64_C(0x7ff0000000000000)) == UINT64_C(0x7ff0000000000000) &&
        (bits & UINT64_C(0x000fffffffffffff)) != 0u) {
        return UINT64_C(0x7ff8000000000000);
    }
    return bits;
}
static inline double f64_from_bits(uint64_t u) {
    union { double d; uint64_t u; } x; x.u = u; return x.d;
}


/* ================= Reader (DataIn) ================= */


void aether_data_in_init(aether_data_in_t *in, const uint8_t *data, size_t size) {
    if (!in) return;
    in->data          = data;
    in->size          = size;
    in->pos           = 0;
    in->error         = false;
    in->scratch_ctx   = NULL;
    in->scratch_alloc = NULL;
}


void aether_data_in_reset(aether_data_in_t *in) {
    if (!in) return;
    in->pos   = 0;
    in->error = false;
}

size_t aether_data_in_size_for_read(const aether_data_in_t *in) {
    if (!in || !in->data) return 0;
    return in->size - in->pos;
}

bool aether_data_in_is_empty(const aether_data_in_t *in) {
    return aether_data_in_size_for_read(in) == 0;
}

bool aether_data_in_is_readable(const aether_data_in_t *in) {
    return aether_data_in_size_for_read(in) != 0;
}

bool aether_data_in_error(const aether_data_in_t *in) {
    return in ? in->error : true;
}

void aether_data_in_clear_error(aether_data_in_t *in) {
    if (in) in->error = false;
}

uint8_t aether_data_in_read_ubyte(aether_data_in_t *in) {
    if (!in || !in->data || in->pos >= in->size) {
        if (in) in->error = true;
        return 0;
    }
    return in->data[in->pos++];
}

int8_t aether_data_in_read_byte(aether_data_in_t *in) {
    return (int8_t)aether_data_in_read_ubyte(in);
}

uint16_t aether_data_in_read_ushort(aether_data_in_t *in) {
    if (!in || !in->data || in->pos + 2u > in->size) {
        if (in) in->error = true;
        return 0;
    }
    uint16_t v = le_r16(&in->data[in->pos]);
    in->pos += 2u;
    return v;
}

int16_t aether_data_in_read_short(aether_data_in_t *in) {
    return (int16_t)aether_data_in_read_ushort(in);
}

uint16_t aether_data_in_read_char(aether_data_in_t *in) {
    /* Java: (char) readUShort() -- 16-bit UTF-16 code unit */
    return aether_data_in_read_ushort(in);
}

int32_t aether_data_in_read_int(aether_data_in_t *in) {
    if (!in || !in->data || in->pos + 4u > in->size) {
        if (in) in->error = true;
        return 0;
    }
    int32_t v = (int32_t)le_r32(&in->data[in->pos]);
    in->pos += 4u;
    return v;
}

uint32_t aether_data_in_read_uint(aether_data_in_t *in) {
    if (!in || !in->data || in->pos + 4u > in->size) {
        if (in) in->error = true;
        return 0;
    }
    uint32_t v = le_r32(&in->data[in->pos]);
    in->pos += 4u;
    return v;
}

int64_t aether_data_in_read_long(aether_data_in_t *in) {
    if (!in || !in->data || in->pos + 8u > in->size) {
        if (in) in->error = true;
        return 0;
    }
    int64_t v = (int64_t)le_r64(&in->data[in->pos]);
    in->pos += 8u;
    return v;
}

float aether_data_in_read_float(aether_data_in_t *in) {
    /* Java DataIn.readFloat(): Float.intBitsToFloat(readInt()) */
    return f32_from_bits((uint32_t)aether_data_in_read_int(in));
}

double aether_data_in_read_double(aether_data_in_t *in) {
    /* Java DataIn.readDouble(): Double.longBitsToDouble(readLong()) */
    return f64_from_bits((uint64_t)aether_data_in_read_long(in));
}

bool aether_data_in_read_boolean(aether_data_in_t *in) {
    /* Java DataIn.readBoolean(): readUByte() != 0 */
    return aether_data_in_read_ubyte(in) != 0;
}

size_t aether_data_in_read(aether_data_in_t *in, uint8_t *dst, size_t off, size_t len) {
    if (!in || (!dst && len != 0)) {
        if (in) in->error = true;
        return 0;
    }
    size_t avail = in->size - in->pos;
    size_t n = (len < avail) ? len : avail;
    for (size_t i = 0; i < n; i++) {
        dst[off + i] = in->data[in->pos + i];
    }
    in->pos += n;
    if (n < len) in->error = true;
    return n;
}

void aether_data_in_read_bulk(aether_data_in_t *in, uint8_t *dst, size_t len) {
    (void)aether_data_in_read(in, dst, 0, len);
}

size_t aether_data_in_read_int_array(aether_data_in_t *in, int32_t *dst, size_t off, size_t len) {
    if (!in || !dst) {
        if (in) in->error = true;
        return 0;
    }
    size_t i = 0;
    while (i < len) {
        if (in->pos + 4u > in->size) {
            in->error = true;
            break;
        }
        dst[off + i] = (int32_t)le_r32(&in->data[in->pos]);
        in->pos += 4u;
        i++;
    }
    return i;
}

size_t aether_data_in_to_array(aether_data_in_t *in, uint8_t *dst, size_t dst_cap) {
    /* Java DataIn.toArray(): copy remaining bytes, advance read pos. */
    if (!in || !dst) {
        if (in) in->error = true;
        return 0;
    }
    size_t avail = in->size - in->pos;
    size_t n = (avail < dst_cap) ? avail : dst_cap;
    for (size_t i = 0; i < n; i++) {
        dst[i] = in->data[in->pos + i];
    }
    in->pos += n;
    if (n < avail) in->error = true;
    return n;
}

void aether_data_in_skip_bytes(aether_data_in_t *in, size_t n) {
    if (!in) return;
    size_t avail = in->size - in->pos;
    if (n > avail) {
        in->pos   = in->size;
        in->error = true;
        return;
    }
    in->pos += n;
}

void aether_data_in_skip_all(aether_data_in_t *in) {
    if (!in) return;
    in->pos = in->size;
}

ptrdiff_t aether_data_in_index_of(const aether_data_in_t *in, size_t limit, uint8_t val) {
    /* Java DataInOutStatic.indexOf(): scan [pos, min(size, limit)). */
    if (!in || !in->data) return -1;
    size_t end = in->size;
    if (limit < end) end = limit;
    for (size_t i = in->pos; i < end; i++) {
        if (in->data[i] == val) return (ptrdiff_t)i;
    }
    return -1;
}

/* ================= Writer (DataOut) ================= */

void aether_data_out_init(aether_data_out_t *out, uint8_t *data, size_t capacity) {
    if (!out) return;
    out->data     = data;
    out->capacity = capacity;
    out->pos      = 0;
    out->error    = false;
}

void aether_data_out_clear(aether_data_out_t *out) {
    if (!out) return;
    out->pos   = 0;
    out->error = false;
}

void aether_data_out_clear_error(aether_data_out_t *out) {
    if (out) out->error = false;
}

size_t aether_data_out_size_for_write(const aether_data_out_t *out) {
    if (!out || !out->data) return 0;
    return out->capacity - out->pos;
}

bool aether_data_out_is_writable(const aether_data_out_t *out) {
    return aether_data_out_size_for_write(out) > 0;
}

bool aether_data_out_error(const aether_data_out_t *out) {
    return out ? out->error : true;
}


bool aether_data_out_write_byte(aether_data_out_t *out, uint8_t v) {
    if (!out || !out->data || out->pos >= out->capacity) {
        if (out) out->error = true;
        return false;
    }
    out->data[out->pos++] = v;
    return true;
}

bool aether_data_out_write_boolean(aether_data_out_t *out, bool v) {
    /* Java DataOut.writeBoolean(): writeByte(v ? 1 : 0) */
    return aether_data_out_write_byte(out, v ? 1u : 0u);
}

bool aether_data_out_write_ushort(aether_data_out_t *out, uint16_t v) {
    if (!out || !out->data || out->pos + 2u > out->capacity) {
        if (out) out->error = true;
        return false;
    }
    le_w16(&out->data[out->pos], v);
    out->pos += 2u;
    return true;
}

bool aether_data_out_write_short(aether_data_out_t *out, int16_t v) {
    return aether_data_out_write_ushort(out, (uint16_t)v);
}

bool aether_data_out_write_char(aether_data_out_t *out, uint16_t v) {
    /* Java DataOut.writeChar(): writeShort((short) v) */
    return aether_data_out_write_ushort(out, v);
}

bool aether_data_out_write_uint(aether_data_out_t *out, uint32_t v) {
    if (!out || !out->data || out->pos + 4u > out->capacity) {
        if (out) out->error = true;
        return false;
    }
    le_w32(&out->data[out->pos], v);
    out->pos += 4u;
    return true;
}

bool aether_data_out_write_int(aether_data_out_t *out, int32_t v) {
    return aether_data_out_write_uint(out, (uint32_t)v);
}

bool aether_data_out_write_long(aether_data_out_t *out, int64_t v) {
    if (!out || !out->data || out->pos + 8u > out->capacity) {
        if (out) out->error = true;
        return false;
    }
    le_w64(&out->data[out->pos], (uint64_t)v);
    out->pos += 8u;
    return true;
}

bool aether_data_out_write_float(aether_data_out_t *out, float v) {
    /* Java DataOut.writeFloat(): writeInt(Float.floatToIntBits(v)) */
    return aether_data_out_write_uint(out, f32_to_bits(v));
}

bool aether_data_out_write_double(aether_data_out_t *out, double v) {
    /* Java DataOut.writeDouble(): writeLong(Double.doubleToLongBits(v)) */
    return aether_data_out_write_long(out, (int64_t)f64_to_bits(v));
}


size_t aether_data_out_write(aether_data_out_t *out, const uint8_t *src, size_t off, size_t len) {
    if (!out || (!src && len != 0)) {
        if (out) out->error = true;
        return 0;
    }
    size_t avail = out->capacity - out->pos;
    size_t n = (len < avail) ? len : avail;
    for (size_t i = 0; i < n; i++) {
        out->data[out->pos + i] = src[off + i];
    }
    out->pos += n;
    if (n < len) out->error = true;
    return n;
}

size_t aether_data_out_write_int_array(aether_data_out_t *out, const int32_t *src, size_t off, size_t len) {
    if (!out || !src) {
        if (out) out->error = true;
        return 0;
    }
    size_t i = 0;
    while (i < len) {
        if (out->pos + 4u > out->capacity) {
            out->error = true;
            break;
        }
        le_w32(&out->data[out->pos], (uint32_t)src[off + i]);
        out->pos += 4u;
        i++;
    }
    return i;
}



/* ============ PackInt (tiered compact integer) ============ */

int aether_pack_size(uint64_t v) {
    if (v < AETHER_PACK_U8)  return 1;
    if (v < AETHER_PACK_U16) return 2;
    if (v < AETHER_PACK_U32) return 4;
    if (v < AETHER_PACK_U64) return 8;
    return 0;
}

bool aether_data_out_write_pack(aether_data_out_t *out, uint64_t v) {
    if (v < AETHER_PACK_U8) {
        return aether_data_out_write_byte(out, (uint8_t)v);
    } else if (v < AETHER_PACK_U16) {
        uint64_t d = v - AETHER_PACK_U8;
        if (!aether_data_out_write_byte(out, (uint8_t)(((d >> 8) & 0xFFu) + AETHER_PACK_U8))) return false;
        return aether_data_out_write_byte(out, (uint8_t)(d & 0xFFu));
    } else if (v < AETHER_PACK_U32) {
        uint64_t d = v - AETHER_PACK_U16;
        if (!aether_data_out_write_byte(out, 255u)) return false;
        if (!aether_data_out_write_byte(out,
                (uint8_t)((((d >> 16) - AETHER_PACK_U8 + AETHER_PACK_U16)) & 0xFFu))) return false;
        return aether_data_out_write_ushort(out, (uint16_t)(d & 0xFFFFu));
    } else if (v < AETHER_PACK_U64) {
        uint64_t d = v - AETHER_PACK_U32;
        if (!aether_data_out_write_byte(out, 255u)) return false;
        if (!aether_data_out_write_byte(out, 255u)) return false;
        if (!aether_data_out_write_ushort(out,
                (uint16_t)((((d >> 32) - AETHER_PACK_U16 + AETHER_PACK_U32)) & 0xFFFFu))) return false;
        return aether_data_out_write_uint(out, (uint32_t)(d & 0xFFFFFFFFu));
    }
    if (out) out->error = true;
    return false;
}

uint64_t aether_data_in_read_pack(aether_data_in_t *in) {
    if (!in) return 0;
    uint64_t b1 = aether_data_in_read_ubyte(in);
    if (in->error) return 0;
    if (b1 < AETHER_PACK_U8) return b1;
    uint64_t b2 = aether_data_in_read_ubyte(in);
    if (in->error) return 0;
    uint64_t val = ((b1 - AETHER_PACK_U8) << 8) + AETHER_PACK_U8 + b2;
    if (val < AETHER_PACK_U16) return val;
    uint64_t f = aether_data_in_read_ushort(in);
    if (in->error) return 0;
    val = ((val - AETHER_PACK_U16) << 16) + AETHER_PACK_U16 + f;
    if (val < AETHER_PACK_U32) return val;
    uint64_t f1 = aether_data_in_read_uint(in);
    if (in->error) return 0;
    val = ((val - AETHER_PACK_U32) << 32) + AETHER_PACK_U32 + f1;
    if (val < AETHER_PACK_U64) return val;
    in->error = true;
    return 0;
}

void aether_pack_parser_init(aether_pack_parser_t *p) {
    if (!p) return;
    p->state = 0; p->bytes_read = 0;
    p->val = 0; p->prefix = 0;
    p->done = false; p->error = false;
}

void aether_pack_parser_reset(aether_pack_parser_t *p) {
    aether_pack_parser_init(p);
}

bool aether_pack_parser_feed(aether_pack_parser_t *p, uint8_t b) {
    if (!p || p->error || p->done) return false;
    switch (p->state) {
        case 0:
            p->val = b;
            if (p->val < AETHER_PACK_U8) { p->done = true; return true; }
            p->state = 1;
            return false;
        case 1:
            p->val = ((p->val - AETHER_PACK_U8) << 8) + AETHER_PACK_U8 + b;
            if (p->val < AETHER_PACK_U16) { p->done = true; return true; }
            p->state = 2; p->bytes_read = 0;
            p->prefix = p->val; p->val = 0;
            return false;
        case 2:
            p->val |= ((uint64_t)b) << (8u * p->bytes_read);
            p->bytes_read++;
            if (p->bytes_read == 2u) {
                uint64_t f = p->val & 0xFFFFu;
                p->val = ((p->prefix - AETHER_PACK_U16) << 16) + AETHER_PACK_U16 + f;
                if (p->val < AETHER_PACK_U32) { p->done = true; return true; }
                p->state = 3; p->bytes_read = 0;
                p->prefix = p->val; p->val = 0;
            }
            return false;
        case 3:
            p->val |= ((uint64_t)b) << (8u * p->bytes_read);
            p->bytes_read++;
            if (p->bytes_read == 4u) {
                uint64_t f1 = p->val & 0xFFFFFFFFu;
                p->val = ((p->prefix - AETHER_PACK_U32) << 32) + AETHER_PACK_U32 + f1;
                if (p->val < AETHER_PACK_U64) { p->done = true; return true; }
                p->error = true;
                return false;
            }
            return false;
        default:
            p->error = true;
            return false;
    }
}

uint64_t aether_pack_parser_value(const aether_pack_parser_t *p) {
    return p ? p->val : 0;
}

bool aether_pack_parser_done(const aether_pack_parser_t *p) {
    return p ? p->done : false;
}

bool aether_pack_parser_error(const aether_pack_parser_t *p) {
    return p ? p->error : true;
}


/* ============ TieredInt (parameterized tiered integer) ============ */

/*
 * Mirrors TieredIntFastMeta from io.aether.net.fastMeta.
 * Same wire format as aether_meta_write_tiered_int / aether_meta_read_tiered_int,
 * but operating on the dataio structs.
 */

static bool dataio_tiered_int_config(
    uint8_t base_bytes,
    const uint32_t *limits,
    size_t limit_count,
    uint64_t *max_encodable) {

    if (limits == NULL ||
        max_encodable == NULL ||
        (base_bytes != 1u && base_bytes != 2u && base_bytes != 4u) ||
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

    const uint64_t word       = UINT64_C(1) << (8u * base_bytes);
    const uint64_t header_max = word - UINT64_C(1);

    if ((uint64_t)limits[0] >= header_max) {
        return false;
    }

    uint64_t span = header_max - (uint64_t)limits[0] - UINT64_C(1);
    uint64_t tail = (uint64_t)limits[0] + UINT64_C(1) + header_max;
    uint64_t max_value;

    if (span != 0u && word > (UINT64_MAX - tail) / span) {
        max_value = UINT64_MAX;
    } else {
        max_value = span * word + tail;
    }

    for (size_t i = 1u; i < limit_count; ++i) {
        const uint64_t limit = (uint64_t)limits[i];

        if (limit > max_value) {
            return false;
        }

        const unsigned int low_bytes = (unsigned int)base_bytes << i;
        const uint64_t modulus       = UINT64_C(1) << (8u * low_bytes);

        span = max_value - limit - UINT64_C(1);
        tail = limit + UINT64_C(1) + modulus - UINT64_C(1);

        if (span != 0u && modulus > (UINT64_MAX - tail) / span) {
            max_value = UINT64_MAX;
        } else {
            max_value = span * modulus + tail;
        }
    }

    *max_encodable = max_value;
    return true;
}

static bool dataio_write_le(
    aether_data_out_t *out, uint64_t value, unsigned int bytes) {

    if (out == NULL || out->data == NULL ||
        bytes > out->capacity - out->pos) {
        if (out) out->error = true;
        return false;
    }

    for (unsigned int i = 0u; i < bytes; ++i) {
        out->data[out->pos++] = (uint8_t)(value >> (8u * i));
    }
    return true;
}

static bool dataio_read_le(
    aether_data_in_t *in, unsigned int bytes, uint64_t *value) {

    if (in == NULL || value == NULL || in->data == NULL ||
        in->pos > in->size ||
        bytes > in->size - in->pos) {
        if (in) in->error = true;
        return false;
    }

    uint64_t result = 0u;
    for (unsigned int i = 0u; i < bytes; ++i) {
        result |= (uint64_t)in->data[in->pos++] << (8u * i);
    }
    *value = result;
    return true;
}

bool aether_data_out_write_tiered_int(
    aether_data_out_t *out,
    uint64_t value,
    uint8_t min_tier_bytes,
    const uint32_t *limits,
    size_t limit_count) {

    if (out == NULL) return false;

    uint64_t max_encodable = 0u;
    if (!dataio_tiered_int_config(
            min_tier_bytes, limits, limit_count, &max_encodable) ||
        value > (uint64_t)INT64_MAX ||
        value > max_encodable) {
        out->error = true;
        return false;
    }

    uint64_t current = value;
    uint64_t lows[3] = { 0u, 0u, 0u };
    int highest = -1;

    for (size_t offset = 0u; offset < limit_count; ++offset) {
        const size_t i = limit_count - offset - 1u;

        if (current <= (uint64_t)limits[i]) {
            continue;
        }

        const unsigned int low_bytes =
            (unsigned int)min_tier_bytes << (unsigned int)i;
        const uint64_t modulus = UINT64_C(1) << (8u * low_bytes);
        const uint64_t delta   = current - (uint64_t)limits[i] - UINT64_C(1);

        lows[i] = delta % modulus;
        current = delta / modulus + (uint64_t)limits[i] + UINT64_C(1);

        if ((int)i > highest) highest = (int)i;
    }

    if (!dataio_write_le(out, current, min_tier_bytes)) return false;

    for (int i = 0; i <= highest; ++i) {
        if (!dataio_write_le(
                out, lows[i],
                (unsigned int)min_tier_bytes << (unsigned int)i)) {
            return false;
        }
    }
    return true;
}

bool aether_data_in_read_tiered_int(
    aether_data_in_t *in,
    uint64_t *value,
    uint8_t min_tier_bytes,
    const uint32_t *limits,
    size_t limit_count) {

    if (in == NULL || value == NULL) return false;

    uint64_t max_encodable = 0u;
    if (!dataio_tiered_int_config(
            min_tier_bytes, limits, limit_count, &max_encodable)) {
        in->error = true;
        return false;
    }

    uint64_t current = 0u;
    if (!dataio_read_le(in, min_tier_bytes, &current)) return false;

    if (current <= (uint64_t)limits[0]) {
        *value = current;
        return true;
    }

    for (size_t i = 0u; i < limit_count; ++i) {
        const uint64_t limit = (uint64_t)limits[i];

        if (current <= limit) {
            in->error = true;
            return false;
        }

        const unsigned int low_bytes =
            (unsigned int)min_tier_bytes << (unsigned int)i;
        const uint64_t modulus = UINT64_C(1) << (8u * low_bytes);

        uint64_t low = 0u;
        if (!dataio_read_le(in, low_bytes, &low)) return false;

        const uint64_t high = current - limit - UINT64_C(1);
        const uint64_t tail = limit + UINT64_C(1) + low;

        if (high != 0u && modulus > (UINT64_MAX - tail) / high) {
            in->error = true;
            return false;
        }

        current = high * modulus + tail;

        if (i + 1u == limit_count ||
            current <= (uint64_t)limits[i + 1u]) {

            if (current > max_encodable || current > (uint64_t)INT64_MAX) {
                in->error = true;
                return false;
            }
            *value = current;
            return true;
        }
    }

    in->error = true;
    return false;
}


/* ============ Array I/O ============ */

/*
 * One element at a time through existing dataio primitives. No per-element
 * function pointers for flat primitives; byte[] becomes a straight byte
 * loop (memcpy-equivalent), other widths call the LE primitives.
 */

static bool dataio_flat_write_one(
    aether_data_out_t *out, aether_flat_kind_t kind, const void *elem) {

    switch (kind) {
        case AETHER_FLAT_BOOL: {
            const uint8_t *p = (const uint8_t *)elem;
            aether_data_out_write_boolean(out, *p != 0);
            return !out->error;
        }
        case AETHER_FLAT_I8:
        case AETHER_FLAT_U8: {
            const uint8_t *p = (const uint8_t *)elem;
            aether_data_out_write_byte(out, *p);
            return !out->error;
        }
        case AETHER_FLAT_I16:
        case AETHER_FLAT_U16:
        case AETHER_FLAT_CHAR: {
            const uint16_t *p = (const uint16_t *)elem;
            aether_data_out_write_ushort(out, *p);
            return !out->error;
        }
        case AETHER_FLAT_I32:
        case AETHER_FLAT_U32: {
            const uint32_t *p = (const uint32_t *)elem;
            aether_data_out_write_uint(out, *p);
            return !out->error;
        }
        case AETHER_FLAT_I64:
        case AETHER_FLAT_U64: {
            const uint64_t *p = (const uint64_t *)elem;
            aether_data_out_write_long(out, (int64_t)*p);
            return !out->error;
        }
        case AETHER_FLAT_F32: {
            const float *p = (const float *)elem;
            aether_data_out_write_float(out, *p);
            return !out->error;
        }
        case AETHER_FLAT_F64: {
            const double *p = (const double *)elem;
            aether_data_out_write_double(out, *p);
            return !out->error;
        }
        default:
            out->error = true;
            return false;
    }
}

static bool dataio_flat_read_one(
    aether_data_in_t *in, aether_flat_kind_t kind, void *elem) {

    switch (kind) {
        case AETHER_FLAT_BOOL: {
            uint8_t *p = (uint8_t *)elem;
            *p = aether_data_in_read_boolean(in) ? 1u : 0u;
            return !in->error;
        }
        case AETHER_FLAT_I8:
        case AETHER_FLAT_U8: {
            uint8_t *p = (uint8_t *)elem;
            *p = aether_data_in_read_ubyte(in);
            return !in->error;
        }
        case AETHER_FLAT_I16:
        case AETHER_FLAT_U16:
        case AETHER_FLAT_CHAR: {
            uint16_t *p = (uint16_t *)elem;
            *p = aether_data_in_read_ushort(in);
            return !in->error;
        }
        case AETHER_FLAT_I32:
        case AETHER_FLAT_U32: {
            uint32_t *p = (uint32_t *)elem;
            *p = aether_data_in_read_uint(in);
            return !in->error;
        }
        case AETHER_FLAT_I64:
        case AETHER_FLAT_U64: {
            uint64_t *p = (uint64_t *)elem;
            *p = (uint64_t)aether_data_in_read_long(in);
            return !in->error;
        }
        case AETHER_FLAT_F32: {
            float *p = (float *)elem;
            *p = aether_data_in_read_float(in);
            return !in->error;
        }
        case AETHER_FLAT_F64: {
            double *p = (double *)elem;
            *p = aether_data_in_read_double(in);
            return !in->error;
        }
        default:
            in->error = true;
            return false;
    }
}

static size_t dataio_flat_elem_size(aether_flat_kind_t kind) {
    switch (kind) {
        case AETHER_FLAT_BOOL:
        case AETHER_FLAT_I8:
        case AETHER_FLAT_U8:   return 1u;
        case AETHER_FLAT_I16:
        case AETHER_FLAT_U16:
        case AETHER_FLAT_CHAR: return 2u;
        case AETHER_FLAT_I32:
        case AETHER_FLAT_U32:
        case AETHER_FLAT_F32:  return 4u;
        case AETHER_FLAT_I64:
        case AETHER_FLAT_U64:
        case AETHER_FLAT_F64:  return 8u;
        default:               return 0u;
    }
}

/* ---- Flat: write N elements from a contiguous buffer ---- */

static bool dataio_write_flat_n(
    aether_data_out_t *out,
    const aether_const_flat_view_t *view) {

    const size_t esz = dataio_flat_elem_size(view->kind);
    if (esz == 0u) { out->error = true; return false; }

    const uint8_t *base = (const uint8_t *)view->data;
    for (size_t i = 0u; i < view->length; ++i) {
        if (!dataio_flat_write_one(out, view->kind, base + i * esz)) return false;
    }
    return true;
}

static bool dataio_read_flat_n(
    aether_data_in_t *in,
    aether_flat_view_t *view,
    size_t n) {

    const size_t esz = dataio_flat_elem_size(view->kind);
    if (esz == 0u) { in->error = true; return false; }
    if (n > view->capacity) { in->error = true; return false; }

    uint8_t *base = (uint8_t *)view->data;
    for (size_t i = 0u; i < n; ++i) {
        if (!dataio_flat_read_one(in, view->kind, base + i * esz)) return false;
    }
    view->length = n;
    return true;
}

/* ---- Dynamic arrays ---- */

bool aether_data_out_write_flat_array(
    aether_data_out_t *out,
    const aether_const_flat_view_t *view,
    unsigned int depth) {

    (void)depth;
    if (out == NULL || view == NULL) return false;
    aether_data_out_write_pack(out, (uint64_t)view->length);
    if (out->error) return false;
    return dataio_write_flat_n(out, view);
}

bool aether_data_in_read_flat_array(
    aether_data_in_t *in,
    aether_flat_view_t *view,
    unsigned int depth) {

    (void)depth;
    if (in == NULL || view == NULL) return false;
    uint64_t n = aether_data_in_read_pack(in);
    if (in->error) return false;
    if (n > view->capacity) { in->error = true; return false; }
    return dataio_read_flat_n(in, view, (size_t)n);
}

bool aether_data_out_write_obj_array(
    aether_data_out_t *out,
    aether_serialize_fn item_fn,
    const aether_const_obj_view_t *view,
    unsigned int depth) {

    if (out == NULL || view == NULL || (view->length != 0u && item_fn == NULL)) {
        if (out) out->error = true;
        return false;
    }
    aether_data_out_write_pack(out, (uint64_t)view->length);
    if (out->error) return false;
    for (size_t i = 0u; i < view->length; ++i) {
        if (!item_fn(out, view->items[i], depth + 1u)) return false;
    }
    return true;
}

bool aether_data_in_read_obj_array(
    aether_data_in_t *in,
    aether_deserialize_fn item_fn,
    aether_obj_view_t *view,
    unsigned int depth) {

    if (in == NULL || view == NULL) return false;
    uint64_t n = aether_data_in_read_pack(in);
    if (in->error) return false;
    if (n > view->capacity) { in->error = true; return false; }
    if (n != 0u && item_fn == NULL) { in->error = true; return false; }
    for (size_t i = 0u; i < (size_t)n; ++i) {
        if (!item_fn(in, view->items[i], depth + 1u)) return false;
    }
    view->length = (size_t)n;
    return true;
}

/* ---- Static arrays ---- */

bool aether_data_out_write_static_flat_array(
    aether_data_out_t *out,
    const aether_const_flat_view_t *view,
    unsigned int depth) {

    (void)depth;
    if (out == NULL || view == NULL) return false;
    return dataio_write_flat_n(out, view);
}

bool aether_data_in_read_static_flat_array(
    aether_data_in_t *in,
    aether_flat_view_t *view,
    unsigned int depth) {

    (void)depth;
    if (in == NULL || view == NULL) return false;
    /* Static: caller must pre-set view->capacity = N; length = capacity. */
    return dataio_read_flat_n(in, view, view->capacity);
}

bool aether_data_out_write_static_obj_array(
    aether_data_out_t *out,
    aether_serialize_fn item_fn,
    const aether_const_obj_view_t *view,
    unsigned int depth) {

    if (out == NULL || view == NULL || (view->length != 0u && item_fn == NULL)) {
        if (out) out->error = true;
        return false;
    }
    for (size_t i = 0u; i < view->length; ++i) {
        if (!item_fn(out, view->items[i], depth + 1u)) return false;
    }
    return true;
}

bool aether_data_in_read_static_obj_array(
    aether_data_in_t *in,
    aether_deserialize_fn item_fn,
    aether_obj_view_t *view,
    unsigned int depth) {

    if (in == NULL || view == NULL) return false;
    /* Static: caller pre-sets view->capacity = N; length = capacity. */
    if (view->capacity != 0u && item_fn == NULL) { in->error = true; return false; }
    for (size_t i = 0u; i < view->capacity; ++i) {
        if (!item_fn(in, view->items[i], depth + 1u)) return false;
    }
    view->length = view->capacity;
    return true;
}


/* ============ UUID ============ */

/*
 * Wire: MSB big-endian (8 bytes) then LSB big-endian (8 bytes).
 * Matches aether_meta_write_uuid / aether_meta_read_uuid.
 */

bool aether_data_out_write_uuid(aether_data_out_t *out, aether_uuid_t v) {
    if (out == NULL || out->data == NULL || out->pos + 16u > out->capacity) {
        if (out) out->error = true;
        return;
    }
    for (int shift = 56; shift >= 0; shift -= 8) {
        out->data[out->pos++] = (uint8_t)(v.msb >> (unsigned int)shift);
    }
    for (int shift = 56; shift >= 0; shift -= 8) {
        out->data[out->pos++] = (uint8_t)(v.lsb >> (unsigned int)shift);
    }
}

aether_uuid_t aether_data_in_read_uuid(aether_data_in_t *in) {
    aether_uuid_t result;
    result.msb = 0u;
    result.lsb = 0u;
    if (in == NULL || in->data == NULL || in->pos + 16u > in->size) {
        if (in) in->error = true;
        return result;
    }
    for (int shift = 56; shift >= 0; shift -= 8) {
        result.msb |= ((uint64_t)in->data[in->pos++]) << (unsigned int)shift;
    }
    for (int shift = 56; shift >= 0; shift -= 8) {
        result.lsb |= ((uint64_t)in->data[in->pos++]) << (unsigned int)shift;
    }
    return result;
}

/* ============ Length-prefixed byte view ============ */

bool aether_data_out_write_bytes(aether_data_out_t *out, const void *data, size_t length) {
    if (out == NULL) return false;
    if (data == NULL && length != 0u) { out->error = true; return false; }
    aether_data_out_write_pack(out, (uint64_t)length);
    if (out->error) return false;
    if (length == 0u) return true;
    const uint8_t *src = (const uint8_t *)data;
    if (length > out->capacity - out->pos) {
        out->error = true;
        return false;
    }
    for (size_t i = 0u; i < length; ++i) {
        out->data[out->pos + i] = src[i];
    }
    out->pos += length;
    return true;
}

bool aether_data_in_read_bytes(aether_data_in_t *in, void *dst, size_t dst_capacity, size_t *out_length) {
    if (in == NULL || out_length == NULL) return false;
    if (dst == NULL && dst_capacity != 0u) { in->error = true; return false; }
    uint64_t n = aether_data_in_read_pack(in);
    if (in->error) return false;
    if (n > dst_capacity) { in->error = true; return false; }
    if (n > in->size - in->pos) { in->error = true; return false; }
    if (n != 0u) {
        uint8_t *d = (uint8_t *)dst;
        for (size_t i = 0u; i < (size_t)n; ++i) {
            d[i] = in->data[in->pos + i];
        }
    }
    in->pos += (size_t)n;
    *out_length = (size_t)n;
    return true;
}


bool aether_data_in_read_view(
    aether_data_in_t *in, const uint8_t **dst, size_t *out_length) {

    if (in == NULL || dst == NULL || out_length == NULL) return false;
    uint64_t n = aether_data_in_read_pack(in);
    if (in->error) return false;
    if (n > in->size - in->pos) { in->error = true; return false; }
    *dst = in->data + in->pos;
    *out_length = (size_t)n;
    in->pos += (size_t)n;
    return true;
}