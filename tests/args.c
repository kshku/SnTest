#include <sntest/sntest.h>

/* Tests for sn_test_parse_args. The parser is a pure function over argv, so
 * these call it directly instead of going through a child process, which keeps
 * the exit code and the parsed values both in reach.
 *
 * A second executable exercises the same parser from main, in cli.c, because
 * that is the only way to check the exit code a real run would produce. */

#define MAX_ARGS 32

/* One array shared by push_arg and parse. The parser only reads the pointers
 * and never writes through them, so casting away const is safe here. */
static char *arg_slots[MAX_ARGS];
static int arg_count = 0;

static void reset_args(void) {
    arg_count = 0;
}

/* The assert macros do a bare "return SN_TEST_FAIL", so they only work inside a
 * test function and not in a void helper like this one. Out of range pushes are
 * dropped instead, and the widest test here uses eleven slots, so there is
 * twenty-one to spare. arg_count bounds what the parser reads, so a dropped
 * argument would shorten the command line and the parse would not match. */
static void push_arg(const char *text) {
    if (arg_count < MAX_ARGS) arg_slots[arg_count++] = (char *)text;
}

static SnTestParseResult parse(SnTestConfigOverride *out) {
    return sn_test_parse_args(out, arg_count, arg_slots);
}

SN_TEST_ADD(no_arguments_sets_nothing) {
    reset_args();
    push_arg("prog");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_RUN);
    SN_TEST_ASSERT_EQ(override.set, 0);
    SN_TEST_ASSERT_FALSE(override.list);
    return SN_TEST_PASS;
}

SN_TEST_ADD(reads_every_option) {
    reset_args();
    push_arg("prog");
    push_arg("--filter");
    push_arg("abc");
    push_arg("--max-failures");
    push_arg("7");
    push_arg("--threads");
    push_arg("4");
    push_arg("--timeout");
    push_arg("250");
    push_arg("--log-level");
    push_arg("WARN");
    push_arg("--no-color");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_RUN);

    SN_TEST_ASSERT_EQ(override.config.filter ? 1 : 0, 1);
    SN_TEST_ASSERT_STR_EQ(override.config.filter, "abc");
    SN_TEST_ASSERT_EQ(override.config.max_failures, 7);
    SN_TEST_ASSERT_EQ(override.config.thread_count, 4);
    /* Milliseconds in, nanoseconds stored, so the field stays in the unit the
     * rest of the runner works in. */
    SN_TEST_ASSERT_EQ(override.config.timeout_ns, 250000000);
    SN_TEST_ASSERT_EQ(override.config.log_level, SN_LOG_LEVEL_WARN);
    SN_TEST_ASSERT_TRUE(override.config.no_color);

    /* Every field was named, so every bit has to be set. A missing bit here
     * would mean a flag parsed but did not reach the runner. */
    SN_TEST_ASSERT_EQ(override.set & SN_TEST_SET_FILTER, SN_TEST_SET_FILTER);
    SN_TEST_ASSERT_EQ(override.set & SN_TEST_SET_MAX_FAILURES, SN_TEST_SET_MAX_FAILURES);
    SN_TEST_ASSERT_EQ(override.set & SN_TEST_SET_THREAD_COUNT, SN_TEST_SET_THREAD_COUNT);
    SN_TEST_ASSERT_EQ(override.set & SN_TEST_SET_TIMEOUT_NS, SN_TEST_SET_TIMEOUT_NS);
    SN_TEST_ASSERT_EQ(override.set & SN_TEST_SET_LOG_LEVEL, SN_TEST_SET_LOG_LEVEL);
    SN_TEST_ASSERT_EQ(override.set & SN_TEST_SET_NO_COLOR, SN_TEST_SET_NO_COLOR);
    return SN_TEST_PASS;
}

SN_TEST_ADD(zero_is_distinguishable_from_unset) {
    /* The whole reason there is a set bitmask. Without it, --max-failures 0
     * would be indistinguishable from not passing the flag, and could not beat
     * a hook that set 1. */
    reset_args();
    push_arg("prog");
    push_arg("--max-failures");
    push_arg("0");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_RUN);
    SN_TEST_ASSERT_EQ(override.config.max_failures, 0);
    SN_TEST_ASSERT_EQ(override.set & SN_TEST_SET_MAX_FAILURES, SN_TEST_SET_MAX_FAILURES);
    return SN_TEST_PASS;
}

SN_TEST_ADD(only_named_fields_are_marked_set) {
    /* A flag for one field must not drag the others in, or it would overwrite
     * the hook's value for a field the user never mentioned. */
    reset_args();
    push_arg("prog");
    push_arg("--no-color");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_RUN);
    SN_TEST_ASSERT_EQ(override.set, SN_TEST_SET_NO_COLOR);
    SN_TEST_ASSERT_EQ(override.config.filter, NULL);
    SN_TEST_ASSERT_EQ(override.config.max_failures, 0);
    SN_TEST_ASSERT_EQ(override.config.thread_count, 0);
    return SN_TEST_PASS;
}

SN_TEST_ADD(list_is_not_a_config_field) {
    reset_args();
    push_arg("prog");
    push_arg("--list");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_RUN);
    SN_TEST_ASSERT_TRUE(override.list);
    SN_TEST_ASSERT_EQ(override.set, 0);
    return SN_TEST_PASS;
}

SN_TEST_ADD(help_exits_instead_of_running) {
    reset_args();
    push_arg("prog");
    push_arg("--help");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_EXIT);
    return SN_TEST_PASS;
}

SN_TEST_ADD(log_level_is_case_insensitive) {
    reset_args();
    push_arg("prog");
    push_arg("--log-level");
    push_arg("TrAcE");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_RUN);
    SN_TEST_ASSERT_EQ(override.config.log_level, SN_LOG_LEVEL_TRACE);
    return SN_TEST_PASS;
}

SN_TEST_ADD(every_log_level_is_accepted) {
    static const struct {
        const char *text;
        SnLogLevel level;
    } cases[] = {
        {"trace", SN_LOG_LEVEL_TRACE},
        {"DEBUG", SN_LOG_LEVEL_DEBUG},
        {"Info",  SN_LOG_LEVEL_INFO },
        {"warn",  SN_LOG_LEVEL_WARN },
        {"ERROR", SN_LOG_LEVEL_ERROR},
        {"fatal", SN_LOG_LEVEL_FATAL},
    };

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        reset_args();
        push_arg("prog");
        push_arg("--log-level");
        push_arg(cases[i].text);

        SnTestConfigOverride override = {0};
        SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_RUN);
        SN_TEST_ASSERT_EQ(override.config.log_level, cases[i].level);
    }
    return SN_TEST_PASS;
}

SN_TEST_ADD(rejects_unknown_option) {
    reset_args();
    push_arg("prog");
    push_arg("--nonsense");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_ERROR);
    return SN_TEST_PASS;
}

SN_TEST_ADD(rejects_missing_value) {
    reset_args();
    push_arg("prog");
    push_arg("--filter");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_ERROR);
    return SN_TEST_PASS;
}

SN_TEST_ADD(rejects_bad_number) {
    reset_args();
    push_arg("prog");
    push_arg("--threads");
    push_arg("banana");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_ERROR);
    return SN_TEST_PASS;
}

/* A negative count is not a number here. strtoull wraps "-1" into
 * 18446744073709551615, which the range check then refuses, so the count never
 * reaches the runner. Both the sign check and the range check reject it; this
 * pins the outcome rather than which one does the work. */
SN_TEST_ADD(rejects_negative_number) {
    reset_args();
    push_arg("prog");
    push_arg("--threads");
    push_arg("-1");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_ERROR);
    return SN_TEST_PASS;
}

SN_TEST_ADD(rejects_number_with_trailing_junk) {
    reset_args();
    push_arg("prog");
    push_arg("--max-failures");
    push_arg("3x");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_ERROR);
    return SN_TEST_PASS;
}

SN_TEST_ADD(rejects_unknown_log_level) {
    reset_args();
    push_arg("prog");
    push_arg("--log-level");
    push_arg("verbose");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_ERROR);
    return SN_TEST_PASS;
}

SN_TEST_ADD(rejects_bare_positional) {
    reset_args();
    push_arg("prog");
    push_arg("banana");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_ERROR);
    return SN_TEST_PASS;
}

/* A value that starts with one dash is a value, not an option, so a filter can
 * be a substring that happens to look like a short flag. */
SN_TEST_ADD(value_may_start_with_one_dash) {
    reset_args();
    push_arg("prog");
    push_arg("--filter");
    push_arg("-abc");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_RUN);
    SN_TEST_ASSERT_STR_EQ(override.config.filter, "-abc");
    return SN_TEST_PASS;
}

/* But a long option is not swallowed as a value. Otherwise --filter --help
 * would filter on the literal string "--help" and never print the help. */
SN_TEST_ADD(option_is_not_swallowed_as_a_value) {
    reset_args();
    push_arg("prog");
    push_arg("--filter");
    push_arg("--help");

    SnTestConfigOverride override = {0};
    SN_TEST_ASSERT_EQ(parse(&override), SN_TEST_PARSE_ERROR);
    return SN_TEST_PASS;
}

/* argv[0] is allowed to be NULL by the standard, and the parser prints it in
 * --help, so it has to cope rather than crash. */
SN_TEST_ADD(copes_with_null_program_name) {
    SN_TEST_ASSERT_EQ(sn_test_parse_args(NULL, 0, NULL), SN_TEST_PARSE_RUN);
    return SN_TEST_PASS;
}

SN_TEST_RUN()
