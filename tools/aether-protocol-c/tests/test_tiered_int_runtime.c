
#include "aether_meta_runtime.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>


static const uint32_t limits[] = {

    UINT32_C(250),
    UINT32_C(1514),
    UINT32_C(1049834)

};


static void expect_wire(
    uint64_t value,
    const uint8_t *expected,
    size_t expected_length) {

    uint8_t wire[16] = {0};
    size_t position = 0u;

    assert(
        aether_meta_write_tiered_int(
            wire,
            sizeof(wire),
            &position,
            value,
            1u,
            limits,
            3u) ==
        AETHER_OK);

    assert(position == expected_length);
    assert(memcmp(wire, expected, expected_length) == 0);

    aether_meta_reader_t reader = {
        wire,
        position,
        0u
    };

    uint64_t decoded = 0u;

    assert(
        aether_meta_read_tiered_int(
            &reader,
            &decoded,
            1u,
            limits,
            3u));

    assert(decoded == value);
    assert(reader.position == position);
}


static void test_java_wire_boundaries(void) {
    /*
     * These are the canonical Aether/Java TieredInt boundaries also used by
     * the existing intpack golden regression. They are independent vectors,
     * not values derived from this runtime implementation.
     */
    static const uint8_t zero[] = {
        0x00u
    };
    static const uint8_t first_limit[] = {
        0xfau
    };
    static const uint8_t first_extended[] = {
        0xfbu, 0x00u
    };
    static const uint8_t second_limit[] = {
        0xffu, 0xefu
    };
    static const uint8_t second_extended[] = {
        0xffu, 0xf0u, 0x00u, 0x00u
    };
    static const uint8_t third_limit[] = {
        0xffu, 0xffu, 0xffu, 0xfeu
    };
    static const uint8_t third_extended[] = {
        0xffu, 0xffu, 0x00u, 0xffu,
        0x00u, 0x00u, 0x00u, 0x00u
    };
    static const uint8_t max_value[] = {
        0xffu, 0xffu, 0xffu, 0xffu,
        0xffu, 0xffu, 0xffu, 0xffu
    };

    expect_wire(
        UINT64_C(0),
        zero,
        sizeof(zero));

    expect_wire(
        UINT64_C(250),
        first_limit,
        sizeof(first_limit));

    expect_wire(
        UINT64_C(251),
        first_extended,
        sizeof(first_extended));

    expect_wire(
        UINT64_C(1514),
        second_limit,
        sizeof(second_limit));

    expect_wire(
        UINT64_C(1515),
        second_extended,
        sizeof(second_extended));

    expect_wire(
        UINT64_C(1049834),
        third_limit,
        sizeof(third_limit));

    expect_wire(
        UINT64_C(1049835),
        third_extended,
        sizeof(third_extended));

    expect_wire(
        UINT64_C(1099512677610),
        max_value,
        sizeof(max_value));
}


static void test_truncated_input(void) {
    /*
     * 1515 canonically needs four bytes: ff f0 00 00.
     * Keep only three so decode has entered tier 2 but cannot read its full
     * two-byte low part.
     */
    static const uint8_t wire[] = {
        0xffu, 0xf0u, 0x00u
    };

    aether_meta_reader_t reader = {
        wire,
        sizeof(wire),
        0u
    };

    uint64_t decoded = 0u;

    assert(
        !aether_meta_read_tiered_int(
            &reader,
            &decoded,
            1u,
            limits,
            3u));
}


static void test_java_long_range(void) {
    static const uint32_t wide_limits[] = {
        UINT32_C(0)
    };

    /*
     * This is the canonical base=4, limit=0 encoding of UINT64_MAX:
     * header 0xffffffff followed by low word 0xffffffff.
     * Java must reject it because TieredInt's public value is Long.
     */
    static const uint8_t too_large[] = {
        0xffu, 0xffu, 0xffu, 0xffu,
        0xffu, 0xffu, 0xffu, 0xffu
    };

    aether_meta_reader_t reader = {
        too_large,
        sizeof(too_large),
        0u
    };

    uint64_t decoded = 0u;

    assert(
        !aether_meta_read_tiered_int(
            &reader,
            &decoded,
            4u,
            wide_limits,
            1u));

    uint8_t wire[16] = {0};
    size_t position = 0u;

    assert(
        aether_meta_write_tiered_int(
            wire,
            sizeof(wire),
            &position,
            UINT64_MAX,
            4u,
            wide_limits,
            1u) ==
        AETHER_ERR_ARGUMENT);

    assert(position == 0u);
}


static void test_configured_range(void) {
    uint8_t wire[16] = {0};
    size_t position = 0u;

    assert(
        aether_meta_write_tiered_int(
            wire,
            sizeof(wire),
            &position,
            (uint64_t)INT64_MAX,
            1u,
            limits,
            3u) ==
        AETHER_ERR_ARGUMENT);

    assert(position == 0u);
}


static void test_invalid_config(void) {
    static const uint32_t unordered[] = {
        UINT32_C(10),
        UINT32_C(10)
    };

    uint8_t wire[16] = {0};
    size_t position = 0u;

    assert(
        aether_meta_write_tiered_int(
            wire,
            sizeof(wire),
            &position,
            UINT64_C(1),
            1u,
            unordered,
            2u) ==
        AETHER_ERR_ARGUMENT);

    assert(position == 0u);

    aether_meta_reader_t reader = {
        wire,
        sizeof(wire),
        0u
    };

    uint64_t value = 0u;

    assert(
        !aether_meta_read_tiered_int(
            &reader,
            &value,
            3u,
            limits,
            3u));
}


int main(void) {
    test_java_wire_boundaries();
    test_truncated_input();
    test_java_long_range();
    test_configured_range();
    test_invalid_config();

    return 0;
}