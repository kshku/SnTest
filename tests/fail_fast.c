#include <sntest/sntest.h>

static int ran = 0;

SN_TEST_SETUP() {
    ran++;
}

SN_TEST_ADD(fail_a) {
    SN_TEST_ASSERT_TRUE(false);
    return SN_TEST_PASS;
}

SN_TEST_ADD(fail_b) {
    SN_TEST_ASSERT_TRUE(false);
    return SN_TEST_PASS;
}

int main(void) {
    sn_test_logger_init();
    SnTestConfig config = {0};
    config.fail_fast = true;
    int ret = sn_test_run_all_tests(&config);
    sn_test_logger_deinit();
    return (ran == 1 && ret == 1) ? 0 : 1;
}
