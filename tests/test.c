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

SN_TEST_RUN()
