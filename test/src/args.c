#include "sntest/sntest.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Command line parsing.
 *
 * The result is a partial configuration: only the fields the caller actually
 * named are recorded, in the set bitmask, so the runner can tell "--timeout 0"
 * apart from no --timeout at all and let the flag win over SN_TEST_INIT. A
 * shadow config compared against zero cannot do that, because a field set to
 * its own zero value would look unset.
 *
 * Values are always validated, even when there is nowhere to store them, so the
 * error reported does not depend on what the caller asked for.
 *
 * Nothing here allocates. The filter points into argv, which outlives the run. */

typedef struct ArgLevel {
    const char *name;
    SnLogLevel level;
} ArgLevel;

static const ArgLevel LEVELS[] = {
    {"trace", SN_LOG_LEVEL_TRACE},
    {"debug", SN_LOG_LEVEL_DEBUG},
    {"info",  SN_LOG_LEVEL_INFO },
    {"warn",  SN_LOG_LEVEL_WARN },
    {"error", SN_LOG_LEVEL_ERROR},
    {"fatal", SN_LOG_LEVEL_FATAL},
};

static const size_t LEVEL_COUNT = sizeof(LEVELS) / sizeof(LEVELS[0]);

static void usage(FILE *out, const char *program) {
    fprintf(out, "Usage: %s [options]\n\n", program ? program : "test");
    fprintf(out, "  --filter <substring>   only run tests whose name contains this\n");
    fprintf(out, "  --max-failures <n>      stop after n failures (0 = unlimited)\n");
    fprintf(out, "  --threads <n>           run tests on n worker threads (0 or 1 = sequential)\n");
    fprintf(out, "  --timeout <ms>          fail a test that runs longer than this many ms\n");
    fprintf(out, "  --log-level <level>     trace, debug, info, warn, error or fatal\n");
    fprintf(out, "  --no-color              do not color the output\n");
    fprintf(out, "  --list                  print the matching test names and run nothing\n");
    fprintf(out, "  --help                  print this and exit\n");
}

/* Reports a bad command line. The message goes to stderr and points at --help,
 * because a usage error is not a test failure and the exit code has to say so.
 *
 * The detail is built by the caller rather than formatted here, so that no call
 * site can end up printing a string that is not there. Printing a null pointer
 * through %s is undefined, and gcc at -O2 inlines far enough to see it. */
static SnTestParseResult usage_error(const char *program, const char *detail) {
    fprintf(stderr, "error: %s\n", detail);
    fprintf(stderr, "Try '%s --help' for the accepted options.\n", program ? program : "test");
    return SN_TEST_PARSE_ERROR;
}

#define DETAIL_CAP 256

/* No value followed the option, or the next token was another option. */
static SnTestParseResult no_value(const char *program, const char *option) {
    char detail[DETAIL_CAP];
    snprintf(detail, sizeof(detail), "%s needs a value", option);
    return usage_error(program, detail);
}

static SnTestParseResult bad_number(const char *program, const char *option, const char *value) {
    char detail[DETAIL_CAP];
    snprintf(detail, sizeof(detail), "%s does not accept '%s' as a number", option, value);
    return usage_error(program, detail);
}

static SnTestParseResult unknown_level(const char *program, const char *option, const char *value) {
    char detail[DETAIL_CAP];
    snprintf(detail, sizeof(detail), "%s does not know the level '%s'", option, value);
    return usage_error(program, detail);
}

static SnTestParseResult unknown_option(const char *program, const char *option) {
    char detail[DETAIL_CAP];
    snprintf(detail, sizeof(detail), "unknown option '%s'", option);
    return usage_error(program, detail);
}

static SnTestParseResult unexpected(const char *program, const char *arg) {
    char detail[DETAIL_CAP];
    snprintf(detail, sizeof(detail), "unexpected argument '%s'", arg);
    return usage_error(program, detail);
}

/* The comparison has to be case insensitive, but strcasecmp is POSIX and
 * _stricmp is Windows, so write it once here rather than pick a platform. */
static bool equals_ignore_case(const char *a, const char *b) {
    for (size_t i = 0;; ++i) {
        char ca = a[i];
        char cb = b[i];

        if (ca >= 'A' && ca <= 'Z') ca = (char)(ca - 'A' + 'a');
        if (cb >= 'A' && cb <= 'Z') cb = (char)(cb - 'A' + 'a');

        if (ca != cb) return false;
        if (ca == '\0') return true;
    }
}

static bool parse_log_level(const char *text, SnLogLevel *out) {
    for (size_t i = 0; i < LEVEL_COUNT; ++i) {
        if (equals_ignore_case(text, LEVELS[i].name)) {
            *out = LEVELS[i].level;
            return true;
        }
    }
    return false;
}

/* strtoull with the checks that matter: something had to be consumed, all of it
 * had to be digits, and the result has to fit.
 *
 * The sign is refused explicitly. C says strtoull accepts a leading minus and
 * negates into the unsigned result, so without this "-1" would come back as
 * 18446744073709551615. The range check rejects that too, so this is belt and
 * braces rather than the only thing standing between "-1" and a thread count of
 * 4294967295, but it says why the number was refused instead of leaving the
 * reason to an overflow. */
static bool parse_uint(const char *text, uint64_t max, uint64_t *out) {
    if (!text[0]) return false;
    if (text[0] == '-' || text[0] == '+') return false;

    char *end = NULL;
    unsigned long long value = strtoull(text, &end, 10);

    if (!end || end == text || *end != '\0') return false;
    if (value > max) return false;

    *out = (uint64_t)value;
    return true;
}

SnTestParseResult sn_test_parse_args(SnTestConfigOverride *out, int argc, char **argv) {
    SnTestConfigOverride parsed = {0};
    const char *program = (argc > 0) ? argv[0] : NULL;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (!arg) continue;

        /* Every option takes its value as the next argument. There is no
         * --opt=value form, because splitting on '=' means either refusing a
         * value that legitimately contains one or walking the string, and no
         * option here needs that. */
        const char *value = (i + 1 < argc) ? argv[i + 1] : NULL;

        /* A value may start with a single dash, so "--filter -x" works, but a
         * token that looks like a long option is treated as the next option
         * rather than swallowed as a value. Otherwise "--filter --help" would
         * quietly filter on the string "--help". */
        if (value && value[0] == '-' && value[1] == '-') value = NULL;

        if (strcmp(arg, "--help") == 0) {
            usage(stdout, program);
            return SN_TEST_PARSE_EXIT;
        }

        if (strcmp(arg, "--list") == 0) {
            parsed.list = true;
            continue;
        }

        if (strcmp(arg, "--no-color") == 0) {
            parsed.config.no_color = true;
            parsed.set |= SN_TEST_SET_NO_COLOR;
            continue;
        }

        if (strcmp(arg, "--filter") == 0) {
            if (!value) return no_value(program, arg);
            parsed.config.filter = value;
            parsed.set |= SN_TEST_SET_FILTER;
            ++i;
            continue;
        }

        if (strcmp(arg, "--max-failures") == 0) {
            uint64_t number = 0;
            if (!value) return no_value(program, arg);
            if (!parse_uint(value, UINT32_MAX, &number)) return bad_number(program, arg, value);
            parsed.config.max_failures = (uint32_t)number;
            parsed.set |= SN_TEST_SET_MAX_FAILURES;
            ++i;
            continue;
        }

        if (strcmp(arg, "--threads") == 0) {
            uint64_t number = 0;
            if (!value) return no_value(program, arg);
            if (!parse_uint(value, UINT32_MAX, &number)) return bad_number(program, arg, value);
            parsed.config.thread_count = (uint32_t)number;
            parsed.set |= SN_TEST_SET_THREAD_COUNT;
            ++i;
            continue;
        }

        if (strcmp(arg, "--timeout") == 0) {
            uint64_t number = 0;
            if (!value) return no_value(program, arg);
            /* The field holds nanoseconds, but nobody types nanoseconds, so take
             * milliseconds and convert. The range check has to cover the
             * multiplication, since that is where a large value would wrap. */
            if (!parse_uint(value, (uint64_t)INT64_MAX / 1000000u, &number))
                return bad_number(program, arg, value);
            parsed.config.timeout_ns = (SnTimeNs)(number * 1000000u);
            parsed.set |= SN_TEST_SET_TIMEOUT_NS;
            ++i;
            continue;
        }

        if (strcmp(arg, "--log-level") == 0) {
            if (!value) return no_value(program, arg);
            if (!parse_log_level(value, &parsed.config.log_level))
                return unknown_level(program, arg, value);
            parsed.set |= SN_TEST_SET_LOG_LEVEL;
            ++i;
            continue;
        }

        if (arg[0] == '-') return unknown_option(program, arg);

        return unexpected(program, arg);
    }

    if (out) *out = parsed;
    return SN_TEST_PARSE_RUN;
}
