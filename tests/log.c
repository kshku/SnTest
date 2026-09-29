#include <sntest/sntest.h>

/* Every macro form, so a change to how they forward their arguments cannot drop
 * one of them by accident.
 *
 * Half of them pass a format string on its own, with no arguments behind it.
 * That is the case the GNU , ##__VA_ARGS__ extension used to be needed for, and
 * it is also a real call into sn_test_log_msg with nothing in its variadic part,
 * so this exercises that path at run time too, not just at compile time. */

SN_TEST_ADD(log_forms) {
    log_msg("default, no arguments\n");
    log_msg("default, %d argument\n", 1);

    log_msg_fg(COLOR_RED, "foreground only, no arguments\n");
    log_msg_bg(COLOR_BLUE, "background only, no arguments\n");
    log_msg_mode(MODE_BOLD, "mode only, no arguments\n");
    log_msg_fg_mode(COLOR_GREEN, MODE_UNDERLINE, "foreground and mode, no arguments\n");
    log_msg_bg_mode(COLOR_CYAN, MODE_BLINKING, "background and mode, no arguments\n");
    log_msg_color(COLOR_YELLOW, COLOR_BLUE, "both colors, no arguments\n");

    log_msg_style(COLOR_MAGENTA, COLOR_BLACK, MODE_ITALIC, "everything, no arguments\n");
    log_msg_style(COLOR_WHITE, COLOR_BLACK, MODE_BOLD, "everything, %s and %d\n", "arguments", 2);

    return SN_TEST_PASS;
}

SN_TEST_RUN()
