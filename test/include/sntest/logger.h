#pragma once

#include <sncore/defines.h>
#include <snlogger/log_level.h>

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#color-codes
/**
 * @enum SnTestColor
 * @brief ANSI foreground/background colors.
 */
typedef enum SnTestColor {
    COLOR_BLACK = 30, /**< Black */
    COLOR_RED, /**< Red */
    COLOR_GREEN, /**< Green */
    COLOR_YELLOW, /**< Yellow */
    COLOR_BLUE, /**< Blue */
    COLOR_MAGENTA, /**< Magenta */
    COLOR_CYAN, /**< Cyan */
    COLOR_WHITE, /**< White */

    COLOR_DEFAULT = 39, /**< Default color */

    COLOR_MAX
} SnTestColor;

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#colors--graphics-mode
// Some terminals may not support some of graphics/color modes
// Can set multiple modes together (except DEFAULT)
/**
 * @enum SnTestColorMode
 * @brief ANSI graphics modes, combinable with the bitwise or operator.
 *
 * Some terminals may not support some of these modes. Can set multiple
 * modes together (except MODE_DEFAULT).
 */
typedef enum SnTestColorMode {
    MODE_DEFAULT = 0, /**< Default mode */
    MODE_BOLD = SN_BIT_FLAG(0), /**< Bold */
    MODE_DIM = SN_BIT_FLAG(1), /**< Dim */
    MODE_ITALIC = SN_BIT_FLAG(2), /**< Italic */
    MODE_UNDERLINE = SN_BIT_FLAG(3), /**< Underline */
    MODE_BLINKING = SN_BIT_FLAG(4), /**< Blinking */
    MODE_INVERSE = SN_BIT_FLAG(5), /**< Inverse */
    MODE_HIDDEN = SN_BIT_FLAG(6), /**< Hidden */
    MODE_STRIKETHROUGH = SN_BIT_FLAG(7), /**< Strikethrough */

    MODE_MAX
} SnTestColorMode;

/**
 * @brief Log a message with the given color and mode.
 *
 * @param fg The foreground color.
 * @param bg The background color.
 * @param mode The graphics mode (bitwise or of SnTestColorMode).
 * @param fmt The format string.
 */
void sn_test_log_msg(SnTestColor fg, SnTestColor bg, int mode, const char *fmt, ...);

/**
 * @brief Log a message with the default color and mode.
 *
 * @param msg The message (printf-style format).
 */
#define log_msg(msg, ...)                                                           \
    sn_test_log_msg(COLOR_DEFAULT, COLOR_DEFAULT, MODE_DEFAULT, msg, ##__VA_ARGS__)

/**
 * @brief Log a message with the given foreground color.
 *
 * @param fg The foreground color.
 * @param msg The message (printf-style format).
 */
#define log_msg_fg(fg, msg, ...)                                         \
    sn_test_log_msg(fg, COLOR_DEFAULT, MODE_DEFAULT, msg, ##__VA_ARGS__)

/**
 * @brief Log a message with the given foreground color and mode.
 *
 * @param fg The foreground color.
 * @param mode The graphics mode.
 * @param msg The message (printf-style format).
 */
#define log_msg_fg_mode(fg, mode, msg, ...)                      \
    sn_test_log_msg(fg, COLOR_DEFAULT, mode, msg, ##__VA_ARGS__)

/**
 * @brief Log a message with the given background color.
 *
 * @param bg The background color.
 * @param msg The message (printf-style format).
 */
#define log_msg_bg(bg, msg, ...)                                         \
    sn_test_log_msg(COLOR_DEFAULT, bg, MODE_DEFAULT, msg, ##__VA_ARGS__)

/**
 * @brief Log a message with the given background color and mode.
 *
 * @param bg The background color.
 * @param mode The graphics mode.
 * @param msg The message (printf-style format).
 */
#define log_msg_bg_mode(bg, mode, msg, ...)                      \
    sn_test_log_msg(COLOR_DEFAULT, bg, mode, msg, ##__VA_ARGS__)

/**
 * @brief Log a message with the given mode.
 *
 * @param mode The graphics mode.
 * @param msg The message (printf-style format).
 */
#define log_msg_mode(mode, msg, ...)                                        \
    sn_test_log_msg(COLOR_DEFAULT, COLOR_DEFAULT, mode, msg, ##__VA_ARGS__)

/**
 * @brief Log a message with the given foreground and background colors.
 *
 * @param fg The foreground color.
 * @param bg The background color.
 * @param msg The message (printf-style format).
 */
#define log_msg_color(fg, bg, msg, ...) sn_test_log_msg(fg, bg, MODE_DEFAULT, msg, ##__VA_ARGS__)

/**
 * @brief Log a message with the given color and mode.
 *
 * @param fg The foreground color.
 * @param bg The background color.
 * @param mode The graphics mode.
 * @param msg The message (printf-style format).
 */
#define log_msg_style(fg, bg, mode, msg, ...) sn_test_log_msg(fg, bg, mode, msg, ##__VA_ARGS__)

/**
 * @brief Initialize the test logger.
 *
 * Idempotent and thread-safe; called automatically by SN_TEST_RUN().
 */
void sn_test_logger_init(void);

/**
 * @brief Set the minimum log level to print.
 *
 * @param level The minimum log level.
 */
void sn_test_set_log_level(SnLogLevel level);

/**
 * @brief Disable ANSI colors in the output.
 */
void sn_test_logger_disable_color(void);

/**
 * @brief Deinitialize the test logger.
 */
void sn_test_logger_deinit(void);
