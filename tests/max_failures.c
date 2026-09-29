#include <sntest/sntest.h>

SN_TEST_INIT() {
    test_config->max_failures = 1;
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
