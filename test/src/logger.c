#include "sntest/logger.h"

#include <snlogger/snlogger.h>
#include <snthreads/mutex.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(SN_OS_WINDOWS)
    #include <windows.h>

static bool enableVTProcessing(DWORD handle_type);
#else
    #include <unistd.h>
#endif

#define LOGGER_BUFFER_SIZE 1024

SN_INLINE int get_color_value(SnTestColor color, bool fg) {
    return color + (fg ? 0 : 10);
}

typedef struct stdout_stderr_sink {
    // 0 -> stdout, 1 -> stderr
    bool color_enabled[2];
} stdout_stderr_sink;

static void stdout_stderr_sink_write(const char *msg, size_t len, SnLogLevel level, void *data);
static void stdout_stderr_sink_open(void *data);
static void stdout_stderr_sink_flush(void *data);
static void sn_test_log_msg_locked(SnTestColor fg, SnTestColor bg, int mode, const char *fmt, va_list args);

static SnStaticLogger sl;
static char log_buffer[LOGGER_BUFFER_SIZE];
static stdout_stderr_sink sink_data;
static SnSink sinks[] = {
    {.open = stdout_stderr_sink_open, .flush = stdout_stderr_sink_flush, .write = stdout_stderr_sink_write, .data = &sink_data}
};

static SnMutex log_mutex;

void sn_test_logger_init(void) {
    sn_mutex_init(&log_mutex);
    sn_static_logger_init(&sl, log_buffer, LOGGER_BUFFER_SIZE, sinks, SN_ARRAY_LENGTH(sinks));
}

void sn_test_set_log_level(SnLogLevel level) {
    sn_static_logger_set_level(&sl, level);
}

void sn_test_logger_disable_color(void) {
    sink_data.color_enabled[0] = false;
    sink_data.color_enabled[1] = false;
}

void sn_test_logger_deinit(void) {
    sn_static_logger_deinit(&sl);
    sn_mutex_deinit(&log_mutex);
}

void sn_test_log_msg(SnTestColor fg, SnTestColor bg, int mode, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    sn_mutex_lock(&log_mutex);
    sn_test_log_msg_locked(fg, bg, mode, fmt, args);
    sn_mutex_unlock(&log_mutex);

    va_end(args);
}

static void sn_test_log_msg_locked(SnTestColor fg, SnTestColor bg, int mode, const char *fmt, va_list args) {
    char buffer[1024] = {0};
    int buffer_size = SN_ARRAY_LENGTH(buffer);
    int len = snprintf(buffer, buffer_size, "\x1b[");

    // set the modes
    if (mode != MODE_DEFAULT) {
        int i = 1;
        while (mode) {
            if (mode & 1) len += snprintf(buffer + len, buffer_size - len, "%d;", i);
            ++i;
            mode >>= 1;
        }
    }

    len += snprintf(buffer + len, buffer_size - len, "%d;%dm%s\x1b[0m", get_color_value(bg, false),
                    get_color_value(fg, true), fmt);

    if (len >= buffer_size) {
        sn_static_logger_log_va(&sl, SN_LOG_LEVEL_INFO,
                                "\x1b[1;31mToo long message, increase the buffer size or decrease "
                                "message length!\x1b[0m",
                                args);
        return;
    }

    buffer[len] = 0;
    sn_static_logger_log_va(&sl, SN_LOG_LEVEL_INFO, buffer, args);
}

static void stdout_stderr_sink_write(const char *msg, size_t len, SnLogLevel level, void *data) {
    SN_UNUSED(level);
    stdout_stderr_sink *sink = (stdout_stderr_sink *)data;
    if (sink->color_enabled[0]) {
        fwrite(msg, sizeof(char), len, stdout);
        return;
    }

    // escape codes aren't enabled
    // skip the starting escape code
    while (*msg != 'm') {
        msg++;
        len--;
    }
    msg++;
    len--;

    // ending escape code is always "\x1b[0m" -> len = 4
    fwrite(msg, sizeof(char), len - 4, stdout);
}

static void stdout_stderr_sink_open(void *data) {
    stdout_stderr_sink *sink = (stdout_stderr_sink *)data;
#if defined(SN_OS_WINDOWS)
    sink->color_enabled[0] = enableVTProcessing(STD_OUTPUT_HANDLE);
    sink->color_enabled[1] = enableVTProcessing(STD_ERROR_HANDLE);
#else
    sink->color_enabled[0] = isatty(STDOUT_FILENO);
    sink->color_enabled[1] = isatty(STDERR_FILENO);
#endif
}

static void stdout_stderr_sink_flush(void *data) {
    (void)data;
    fflush(stdout);
    fflush(stderr);
}

#if defined(SN_OS_WINDOWS)
static bool enableVTProcessing(DWORD handle_type) {
    HANDLE handle = GetStdHandle(handle_type);
    if (handle == INVALID_HANDLE_VALUE) return false;

    DWORD modes = 0;
    if (!GetConsoleMode(handle, &modes)) return false;

    modes |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    if (!SetConsoleMode(handle, modes)) return false;

    return true;
}
#endif

