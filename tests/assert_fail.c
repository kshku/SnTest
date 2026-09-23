#include <sntest/sntest.h>

SN_TEST_ADD(fail_early) {
    log_msg("fail_early start\n", NULL);
    SN_TEST_ASSERT_EQ(1, 2);
    log_msg("unreachable after failed assertion\n", NULL);
    return SN_TEST_PASS;
}

SN_TEST_ADD(continues_after_failure) {
    SN_TEST_ASSERT_TRUE(42 > 0);
    return SN_TEST_PASS;
}

SN_TEST_RUN()
