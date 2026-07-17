#include <sntest/sntest.h>

bool sn_test_init(SnTestConfig *config) {
    SN_UNUSED(config);
    return true;
}

void sn_test_deinit(void) {
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

SN_TEST_RUN()
