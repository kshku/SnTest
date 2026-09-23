#include <sntest/sntest.h>

SN_TEST_INIT() {
    config->filter = "banana";
    return true;
}

SN_TEST_ADD(apple) {
    SN_TEST_ASSERT_TRUE(false);
    return SN_TEST_PASS;
}

SN_TEST_ADD(banana) {
    SN_TEST_ASSERT_TRUE(true);
    return SN_TEST_PASS;
}

SN_TEST_RUN()
