#include <sntest/sntest.h>

/* The same parser, but reached the way a real test binary reaches it, through
 * SN_TEST_RUN's generated main. The parser tests in args.c check the values it
 * produces; this checks that the values reach the runner, and specifically that
 * a flag on the command line beats a value the hook hardcoded.
 *
 * The hook asks for "cherry" and thread_count 2. Run with no arguments, only
 * cherry runs. Run with --filter apple, apple runs instead, because the
 * command line wins. The names are chosen so the two cannot be confused:
 * apple fails, cherry passes, so the exit code alone tells which one ran. */

SN_TEST_INIT() {
    test_config->filter = "cherry";
    test_config->thread_count = 2;
    return true;
}

SN_TEST_ADD(apple) {
    SN_TEST_ASSERT_TRUE(false);
    return SN_TEST_PASS;
}

SN_TEST_ADD(cherry) {
    SN_TEST_ASSERT_TRUE(true);
    return SN_TEST_PASS;
}

SN_TEST_RUN()
