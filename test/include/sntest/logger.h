#pragma once

#include <sncore/defines.h>
#include <snlogger/log_level.h>

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#color-codes
typedef enum SnTestColor {
    COLOR_BLACK = 30,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,

    COLOR_DEFAULT = 39,

    COLOR_MAX
} SnTestColor;

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#colors--graphics-mode
// Some terminals may not support some of graphics/color modes
// Can set multiple modes together (except DEFAULT)
typedef enum SnTestColorMode {
    MODE_DEFAULT = 0,
    MODE_BOLD = SN_BIT_FLAG(0),
    MODE_DIM = SN_BIT_FLAG(1),
    MODE_ITALIC = SN_BIT_FLAG(2),
    MODE_UNDERLINE = SN_BIT_FLAG(3),
    MODE_BLINKING = SN_BIT_FLAG(4),
    MODE_INVERSE = SN_BIT_FLAG(5),
    MODE_HIDDEN = SN_BIT_FLAG(6),
    MODE_STRIKETHROUGH = SN_BIT_FLAG(7),

    MODE_MAX
} SnTestColorMode;

void sn_test_log_msg(SnTestColor fg, SnTestColor bg, int mode, const char *fmt, ...);

#define log_msg(msg, ...)                                                           \
    sn_test_log_msg(COLOR_DEFAULT, COLOR_DEFAULT, MODE_DEFAULT, msg, ##__VA_ARGS__)

#define log_msg_fg(fg, msg, ...)                                         \
    sn_test_log_msg(fg, COLOR_DEFAULT, MODE_DEFAULT, msg, ##__VA_ARGS__)

#define log_msg_fg_mode(fg, mode, msg, ...)                      \
    sn_test_log_msg(fg, COLOR_DEFAULT, mode, msg, ##__VA_ARGS__)

#define log_msg_bg(bg, msg, ...)                                         \
    sn_test_log_msg(COLOR_DEFAULT, bg, MODE_DEFAULT, msg, ##__VA_ARGS__)

#define log_msg_bg_mode(bg, mode, msg, ...)                      \
    sn_test_log_msg(COLOR_DEFAULT, bg, mode, msg, ##__VA_ARGS__)

#define log_msg_mode(mode, msg, ...)                                        \
    sn_test_log_msg(COLOR_DEFAULT, COLOR_DEFAULT, mode, msg, ##__VA_ARGS__)

#define log_msg_color(fg, bg, msg, ...) sn_test_log_msg(fg, bg, MODE_DEFAULT, msg, ##__VA_ARGS__)

#define log_msg_style(fg, bg, mode, msg, ...) sn_test_log_msg(fg, bg, mode, msg, ##__VA_ARGS__)

void sn_test_logger_init(void);
void sn_test_set_log_level(SnLogLevel level);
void sn_test_logger_disable_color(void);
void sn_test_logger_deinit(void);
