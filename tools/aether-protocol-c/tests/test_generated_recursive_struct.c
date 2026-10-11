
#include "aether_api.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static void assert_in_scratch(
    const void *pointer,
    const uint8_t *scratch,
    size_t scratch_size) {

    assert(pointer != NULL);
    assert((const uint8_t *)pointer >= scratch);
    assert((const uint8_t *)pointer < scratch + scratch_size);
}

static void test_node_recursive_round_trip(void) {
    rs_node_t next = {
        .value = -2,
        .next = NULL,
        .children = {
            .data = NULL,
            .length = 0u
        }
    };

    rs_node_t child = {
        .value = INT32_C(0x01020304),
        .next = NULL,
        .children = {
            .data = NULL,
            .length = 0u
        }
    };

    rs_node_t head = {
        .value = INT32_C(0x11223344),
        .next = &next,
        .children = {
            .data = &child,
            .length = 1u
        }
    };

    uint8_t encoded[64] = {0};
    size_t encoded_size = 0u;

    assert(
        rs_serialize_node(
            encoded,
            sizeof(encoded),
            &encoded_size,
            &head) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x00u,
        0x44u, 0x33u, 0x22u, 0x11u,

        0x01u,
        0xfeu, 0xffu, 0xffu, 0xffu,
        0x00u,

        0x01u,

        0x01u,
        0x04u, 0x03u, 0x02u, 0x01u,
        0x00u
    };

    assert(encoded_size == sizeof(expected));
    assert(memcmp(encoded, expected, sizeof(expected)) == 0);

    uint8_t scratch[512] = {0};
    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = encoded;
    ctx.reader.length = encoded_size;
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    rs_node_t decoded = {0};

    assert(
        rs_deserialize_node(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == encoded_size);

    assert(decoded.value == INT32_C(0x11223344));

    assert_in_scratch(
        decoded.next,
        scratch,
        sizeof(scratch));

    assert(decoded.next->value == -2);
    assert(decoded.next->next == NULL);
    assert(decoded.next->children.length == 0u);
    assert(decoded.next->children.data == NULL);

    assert(decoded.children.length == 1u);

    assert_in_scratch(
        decoded.children.data,
        scratch,
        sizeof(scratch));

    assert(decoded.children.data[0].value == INT32_C(0x01020304));
    assert(decoded.children.data[0].next == NULL);
    assert(decoded.children.data[0].children.length == 0u);
    assert(decoded.children.data[0].children.data == NULL);
}

static void test_inline_recursive_root(void) {
    rs_recursive_root_t value = {
        .rootId = INT32_C(0x55667788),
        .node = {
            .value = 7,
            .next = NULL,
            .children = {
                .data = NULL,
                .length = 0u
            }
        }
    };

    uint8_t encoded[32] = {0};
    size_t encoded_size = 0u;

    assert(
        rs_serialize_recursive_root(
            encoded,
            sizeof(encoded),
            &encoded_size,
            &value) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x88u, 0x77u, 0x66u, 0x55u,
        0x01u,
        0x07u, 0x00u, 0x00u, 0x00u,
        0x00u
    };

    assert(encoded_size == sizeof(expected));
    assert(memcmp(encoded, expected, sizeof(expected)) == 0);

    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = encoded;
    ctx.reader.length = encoded_size;

    rs_recursive_root_t decoded = {0};

    assert(
        rs_deserialize_recursive_root(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == encoded_size);
    assert(decoded.rootId == INT32_C(0x55667788));
    assert(decoded.node.value == 7);
    assert(decoded.node.next == NULL);
    assert(decoded.node.children.length == 0u);
    assert(decoded.node.children.data == NULL);
}

static void test_mutual_recursion_round_trip(void) {
    rs_mutual_a_t parent = {
        .tag = -2,
        .child = {
            .code = -3,
            .parent = NULL
        }
    };

    rs_mutual_a_t value = {
        .tag = INT8_C(0x7f),
        .child = {
            .code = INT16_C(0x1234),
            .parent = &parent
        }
    };

    uint8_t encoded[32] = {0};
    size_t encoded_size = 0u;

    assert(
        rs_serialize_mutual_a(
            encoded,
            sizeof(encoded),
            &encoded_size,
            &value) ==
        AETHER_OK);

    static const uint8_t expected[] = {
        0x7fu,
        0x00u,
        0x34u, 0x12u,
        0xfeu,
        0x01u,
        0xfdu, 0xffu
    };

    assert(encoded_size == sizeof(expected));
    assert(memcmp(encoded, expected, sizeof(expected)) == 0);

    uint8_t scratch[256] = {0};
    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = encoded;
    ctx.reader.length = encoded_size;
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    rs_mutual_a_t decoded = {0};

    assert(
        rs_deserialize_mutual_a(
            &ctx,
            &decoded) ==
        AETHER_OK);

    assert(ctx.reader.position == encoded_size);
    assert(decoded.tag == INT8_C(0x7f));
    assert(decoded.child.code == INT16_C(0x1234));

    assert_in_scratch(
        decoded.child.parent,
        scratch,
        sizeof(scratch));

    assert(decoded.child.parent->tag == -2);
    assert(decoded.child.parent->child.code == -3);
    assert(decoded.child.parent->child.parent == NULL);
}


static void test_recursive_encode_depth_limit(void) {
    rs_node_t value = {
        .value = 1,
        .next = NULL,
        .children = {
            .data = NULL,
            .length = 0u
        }
    };

    value.next = &value;

    uint8_t encoded[128] = {0};
    size_t encoded_size = 0u;

    assert(
        rs_serialize_node(
            encoded,
            sizeof(encoded),
            &encoded_size,
            &value) ==
        AETHER_ERR_OVERFLOW);

    /*
     * Named codec helpers commit their caller-visible position only after a
     * successful BODY. Hitting the recursion bound must therefore leave the
     * public serializer cursor unchanged even though scratch TX bytes may have
     * been touched before the failure was detected.
     */
    assert(encoded_size == 0u);
}


static void test_recursive_decode_depth_limit(void) {
    /*
     * Five nested Node values with no children. With the test target compiled
     * for four active named codec frames, entering the fifth Node must fail at
     * the private codec boundary before C stack growth becomes unbounded.
     *
     * Wire order for Node is:
     *   nullable-mask, value, next BODY (when present), children count.
     * The five zero children counts therefore appear while recursion unwinds.
     */
    static const uint8_t encoded[] = {
        0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x02u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x03u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x04u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x05u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    uint8_t scratch[512] = {0};
    aether_meta_read_context_t ctx = {0};
    ctx.reader.data = encoded;
    ctx.reader.length = sizeof(encoded);
    ctx.scratch = scratch;
    ctx.scratch_capacity = sizeof(scratch);

    rs_node_t decoded = {0};

    assert(
        rs_deserialize_node(
            &ctx,
            &decoded) ==
        AETHER_ERR_OVERFLOW);
}



int main(void) {
    test_node_recursive_round_trip();
    test_inline_recursive_root();
    test_mutual_recursion_round_trip();

    test_recursive_encode_depth_limit();
    test_recursive_decode_depth_limit();

    return 0;
}