#include "sntest/logger.h"

#include <snlogger/console_sink.h>
#include <snthreads/mutex.h>
#include <stdio.h>

/* Every message the log_msg macros produce is informational, so the level is
 * fixed and only used to filter. */
#define LOG_LEVEL SN_LOG_LEVEL_INFO

/* A private sink, not sn_console_std_sink. SnTest picks the color of every
 * message itself through the log_msg macros, so it must not also get the
 * sink's own color by level, and it must not reconfigure a global that other
 * code in the process may be sharing. */
static SnConsoleSink console;
static SnMutex log_mutex;
static SnLogLevel min_level = SN_LOG_LEVEL_TRACE;

void sn_test_logger_init(void) {
    sn_mutex_init(&log_mutex);
    min_level = SN_LOG_LEVEL_TRACE;

    sn_console_sink_init(&console, stdout);
    sn_console_sink_set_level_color(&console, false);
}

void sn_test_set_log_level(SnLogLevel level) {
    sn_mutex_lock(&log_mutex);
    min_level = level;
    sn_mutex_unlock(&log_mutex);
}

void sn_test_logger_disable_color(void) {
    sn_mutex_lock(&log_mutex);
    sn_console_sink_set_color(&console, SN_CONSOLE_COLOR_OFF);
    sn_mutex_unlock(&log_mutex);
}

void sn_test_logger_deinit(void) {
    sn_console_flush(&console);
    sn_mutex_deinit(&log_mutex);
}

void sn_test_log_msg(SnTestColor fg, SnTestColor bg, int mode, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    /* The console sink does no locking of its own, which is what keeps it free
     * of a dependency on a threading library, so serializing is ours to do.
     *
     * The lock has to cover the whole record, not just one stdio call. A
     * record is emitted as several: the opening escape, the text, and the
     * reset. stdio's own lock stops any single call from being torn, but
     * nothing stops one thread's escape sequence from landing inside another
     * thread's record, which would garble both the color and the text. Tests
     * log from every worker thread when thread_count is above one. */
    sn_mutex_lock(&log_mutex);

    if (min_level <= LOG_LEVEL) {
        /* SnTestColor and SnConsoleColor are both the ANSI SGR numbers, 30 to
         * 37 with 39 for the terminal default, so the values carry over
         * unchanged. */
        sn_console_write_va(&console, (SnConsoleColor)fg, (SnConsoleColor)bg, mode, fmt, args);
    }

    sn_mutex_unlock(&log_mutex);

    va_end(args);
}
