#include <sntest/sntest.h>

SN_TEST_INIT() {
    test_config->timeout_ns = 1000000;
    return true;
}

SN_TEST_ADD(sleeps_too_long) {
    sn_time_sleep_ms(5);
    return SN_TEST_PASS;
}

SN_TEST_RUN()
