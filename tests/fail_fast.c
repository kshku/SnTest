#include <sntest/sntest.h>

SN_TEST_INIT() {
    config->fail_fast = true;
    return true;
}

SN_TEST_ADD(fail_a) {
    SN_TEST_ASSERT_TRUE(false);
    return SN_TEST_PASS;
}

SN_TEST_ADD(fail_b) {
    SN_TEST_ASSERT_TRUE(false);
    return SN_TEST_PASS;
}

SN_TEST_RUN()
