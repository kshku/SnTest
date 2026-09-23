#include <sntest/sntest.h>

SN_TEST_INIT() {
    config->thread_count = 4;
    return true;
}

SN_TEST_ADD(work_0) {
    sn_time_sleep_ms(10);
    return SN_TEST_PASS;
}

SN_TEST_ADD(work_1) {
    sn_time_sleep_ms(10);
    return SN_TEST_PASS;
}

SN_TEST_ADD(work_2) {
    sn_time_sleep_ms(10);
    return SN_TEST_PASS;
}

SN_TEST_ADD(work_3) {
    sn_time_sleep_ms(10);
    return SN_TEST_PASS;
}

SN_TEST_ADD(work_4) {
    sn_time_sleep_ms(10);
    return SN_TEST_PASS;
}

SN_TEST_ADD(work_5) {
    sn_time_sleep_ms(10);
    return SN_TEST_PASS;
}

SN_TEST_ADD(work_6) {
    sn_time_sleep_ms(10);
    return SN_TEST_PASS;
}

SN_TEST_ADD(work_7) {
    sn_time_sleep_ms(10);
    return SN_TEST_PASS;
}

SN_TEST_RUN()
