#include <sntest/sntest.h>

SN_TEST_INIT() {
    SN_UNUSED(config);
    log_msg("Initializing tests\n", NULL);
    return true;
}

SN_TEST_DEINIT() {
    log_msg("Deinitializing tests\n", NULL);
}

SN_TEST_SETUP() {
    log_msg("Setting up test\n", NULL);
}

SN_TEST_TEARDOWN() {
    log_msg("Tearing down test\n", NULL);
}

SN_TEST_ADD(print_test) {
    log_msg("Inside test\n", NULL);
    log_msg_fg(COLOR_GREEN, "Inside test\n", NULL);
    log_msg_bg(COLOR_RED, "Inside test\n", NULL);
    log_msg_mode(MODE_BOLD | MODE_BLINKING, "Inside test\n", NULL);
    log_msg_color(COLOR_RED, COLOR_GREEN, "Inside test\n", NULL);
    log_msg_style(COLOR_GREEN, COLOR_MAGENTA, MODE_BLINKING, "Inside test\n", NULL);
    return SN_TEST_PASS;
}

SN_TEST_ADD(assert_test) {
    int a = 5;
    const char *str = "hello";
    const char *str_cmp = "hello";
    const char *str_longer = "hello world";
    int nums[3] = {1, 2, 3};
    int nums_other[3] = {1, 2, 3};
    int nums_different[3] = {4, 5, 6};

    SN_TEST_ASSERT(true);
    SN_TEST_ASSERT_TRUE(a > 0);
    SN_TEST_ASSERT_FALSE(a < 0);
    SN_TEST_ASSERT_EQ(a, 5);
    SN_TEST_ASSERT_NEQ(a, 4);
    SN_TEST_ASSERT_LT(a, 6);
    SN_TEST_ASSERT_LE(a, 5);
    SN_TEST_ASSERT_GT(a, 4);
    SN_TEST_ASSERT_GE(a, 5);
    SN_TEST_ASSERT_NULL(NULL);
    SN_TEST_ASSERT_NOT_NULL(str);
    SN_TEST_ASSERT_PTR_EQ(str, str_cmp);
    SN_TEST_ASSERT_PTR_NEQ(str, str_longer);
    SN_TEST_ASSERT_STR_EQ(str, str_cmp);
    SN_TEST_ASSERT_STR_NEQ(str, str_longer);
    SN_TEST_ASSERT_STR_N_EQ(str, str_cmp, 3);
    SN_TEST_ASSERT_CHAR_EQ('a', 'a');
    SN_TEST_ASSERT_CHAR_NEQ('a', 'b');
    SN_TEST_ASSERT_MEM_EQ(nums, nums_other, sizeof(nums));
    SN_TEST_ASSERT_MEM_NEQ(nums, nums_different, sizeof(nums));

    return SN_TEST_PASS;
}

SN_TEST_RUN()
