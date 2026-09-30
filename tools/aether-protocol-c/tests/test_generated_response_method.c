
#include "response_method_response.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <stddef.h>
#include <stdint.h>


static void write_uuid(
    uint8_t *data,
    uint64_t msb,
    uint64_t lsb) {

    for (size_t i = 0u;
         i < 8u;
         ++i) {

        data[i] =
            (uint8_t)(
                msb >>
                ((7u - i) * 8u));

        data[8u + i] =
            (uint8_t)(
                lsb >>
                ((7u - i) * 8u));
    }
}


static global_api_finish_response_storage_t make_storage(
    int16_t *cloud,
    size_t capacity) {

    global_api_finish_response_storage_t storage = {
        .cloud_data_storage = cloud,
        .cloud_data_capacity = capacity
    };

    return storage;
}


static void test_result(void) {
    uint8_t frame[44] = {0};

    frame[0] = 0u;
    frame[1] = 0x78u;
    frame[2] = 0x56u;
    frame[3] = 0x34u;
    frame[4] = 0x12u;

    write_uuid(
        frame + 5u,
        UINT64_C(0x0102030405060708),
        UINT64_C(0x1112131415161718));

    write_uuid(
        frame + 21u,
        UINT64_C(0x2122232425262728),
        UINT64_C(0x3132333435363738));

    frame[37] = 3u;

    frame[38] = 0x34u;
    frame[39] = 0x12u;

    frame[40] = 0xfeu;
    frame[41] = 0xffu;

    frame[42] = 0xffu;
    frame[43] = 0x7fu;

    int16_t cloud[8] = {0};

    global_api_finish_response_storage_t storage =
        make_storage(
            cloud,
            8u);

    global_api_finish_response_frame_t decoded = {0};

    size_t consumed = 0u;

    assert(
        global_api_finish_response_decode(
            &storage,
            frame,
            sizeof(frame),
            &decoded,
            &consumed) ==
        AETHER_OK);

    assert(consumed == sizeof(frame));
    assert(decoded.kind == GLOBAL_API_FINISH_RESPONSE_RESULT);
    assert(decoded.request_id == UINT32_C(0x12345678));

    assert(
        decoded.result.alias.msb ==
        UINT64_C(0x0102030405060708));

    assert(
        decoded.result.alias.lsb ==
        UINT64_C(0x1112131415161718));

    assert(
        decoded.result.uid.msb ==
        UINT64_C(0x2122232425262728));

    assert(
        decoded.result.uid.lsb ==
        UINT64_C(0x3132333435363738));

    assert(decoded.result.cloud.data.length == 3u);
    assert(decoded.result.cloud.data.data == cloud);
    assert(cloud[0] == INT16_C(0x1234));
    assert(cloud[1] == -2);
    assert(cloud[2] == INT16_C(0x7fff));
}


static void test_error(void) {
    uint8_t frame[] = {
        1u,
        0x04u,
        0x03u,
        0x02u,
        0x01u
    };

    int16_t cloud[1] = {0};

    global_api_finish_response_storage_t storage =
        make_storage(
            cloud,
            1u);

    global_api_finish_response_frame_t decoded = {0};

    size_t consumed = 0u;

    assert(
        global_api_finish_response_decode(
            &storage,
            frame,
            sizeof(frame),
            &decoded,
            &consumed) ==
        AETHER_OK);

    assert(consumed == sizeof(frame));
    assert(decoded.kind == GLOBAL_API_FINISH_RESPONSE_ERROR);
    assert(decoded.request_id == UINT32_C(0x01020304));
}


static void test_pack_251(void) {
    uint8_t frame[
        5u +
        16u +
        16u +
        2u +
        251u * 2u] = {0};

    frame[0] = 0u;
    frame[1] = 1u;

    size_t p =
        5u + 16u + 16u;

    frame[p++] = 251u;
    frame[p++] = 0u;

    for (size_t i = 0u;
         i < 251u;
         ++i) {

        int16_t value =
            (int16_t)((int)i - 125);

        uint16_t raw =
            (uint16_t)value;

        frame[p++] =
            (uint8_t)raw;

        frame[p++] =
            (uint8_t)(raw >> 8u);
    }

    assert(p == sizeof(frame));

    int16_t cloud[251] = {0};

    global_api_finish_response_storage_t storage =
        make_storage(
            cloud,
            251u);

    global_api_finish_response_frame_t decoded = {0};

    size_t consumed = 0u;

    assert(
        global_api_finish_response_decode(
            &storage,
            frame,
            sizeof(frame),
            &decoded,
            &consumed) ==
        AETHER_OK);

    assert(consumed == sizeof(frame));
    assert(decoded.result.cloud.data.length == 251u);
    assert(cloud[0] == -125);
    assert(cloud[125] == 0);
    assert(cloud[250] == 125);
}


static void test_overflow_and_truncation(void) {
    uint8_t frame[
        5u +
        16u +
        16u +
        1u +
        6u] = {0};

    frame[0] = 0u;
    frame[37] = 3u;

    int16_t cloud[2] = {0};

    global_api_finish_response_storage_t storage =
        make_storage(
            cloud,
            2u);

    global_api_finish_response_frame_t decoded = {0};

    size_t consumed = 99u;

    assert(
        global_api_finish_response_decode(
            &storage,
            frame,
            sizeof(frame),
            &decoded,
            &consumed) ==
        AETHER_ERR_OVERFLOW);

    assert(consumed == 0u);

    uint8_t truncated[] = {
        0u,
        1u,
        2u,
        3u
    };

    consumed = 88u;

    assert(
        global_api_finish_response_decode(
            &storage,
            truncated,
            sizeof(truncated),
            &decoded,
            &consumed) ==
        AETHER_ERR_PROTOCOL);

    assert(consumed == 0u);
}


static void test_concatenated(void) {
    uint8_t frames[43] = {0};

    frames[0] = 0u;
    frames[1] = 7u;
    frames[37] = 0u;

    const size_t first_length = 38u;

    frames[first_length] = 1u;
    frames[first_length + 1u] = 9u;

    int16_t cloud[1] = {0};

    global_api_finish_response_storage_t storage =
        make_storage(
            cloud,
            1u);

    global_api_finish_response_frame_t decoded = {0};

    size_t first = 0u;

    assert(
        global_api_finish_response_decode(
            &storage,
            frames,
            sizeof(frames),
            &decoded,
            &first) ==
        AETHER_OK);

    assert(first == first_length);
    assert(decoded.kind == GLOBAL_API_FINISH_RESPONSE_RESULT);

    size_t second = 0u;

    assert(
        global_api_finish_response_decode(
            &storage,
            frames + first,
            sizeof(frames) - first,
            &decoded,
            &second) ==
        AETHER_OK);

    assert(second == 5u);
    assert(decoded.kind == GLOBAL_API_FINISH_RESPONSE_ERROR);
    assert(decoded.request_id == 9u);
}


int main(void) {
    test_result();
    test_error();
    test_pack_251();
    test_overflow_and_truncation();
    test_concatenated();

    return 0;
}
