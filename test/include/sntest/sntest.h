#pragma once

#include "sntest/api.h"
#include "sntest/logger.h"

#include <sncore/defines.h>
#include <sntime/sntime.h>

/**
 * @enum SnTestResult
 * @brief Result of a test function.
 */
typedef enum SnTestResult {
    SN_TEST_PASS, /**< The test passed */
    SN_TEST_FAIL, /**< The test failed */
    SN_TEST_SKIP, /**< The test was skipped */
} SnTestResult;

/**
 * @typedef sn_test_fn
 * @brief Signature of a test function.
 *
 * @return Returns a SnTestResult.
 */
typedef SnTestResult (*sn_test_fn)(void);

/**
 * @struct SnTest
 * @brief A registered test.
 */
typedef struct SnTest {
    const char *name; /**< Name of the test */
    sn_test_fn fn; /**< Function to run the test */
} SnTest;

/**
 * @enum SnTestHookType
 * @brief The type of a lifecycle hook.
 */
typedef enum SnTestHookType {
    SN_TEST_HOOK_INIT, /**< Init hook, called once before any test */
    SN_TEST_HOOK_DEINIT, /**< Deinit hook, called once after all tests */
    SN_TEST_HOOK_SETUP, /**< Setup hook, called before each test */
    SN_TEST_HOOK_TEARDOWN, /**< Teardown hook, called after each test */
} SnTestHookType;

/**
 * @struct SnTestConfig
 * @brief Configuration for the test runner.
 *
 * Passed to sn_test_run_all_tests(), and configurable from the SN_TEST_INIT() hook.
 */
typedef struct SnTestConfig {
    uint32_t max_failures; /**< Stop the run after this many failures (0 = unlimited) */
    uint32_t thread_count; /**< Number of worker threads (0 or 1 = sequential) */
    SnTimeNs timeout_ns; /**< Per-test timeout in ns (0 = disabled) */
    const char *filter; /**< Only run tests whose name contains this substring (NULL = all) */
    bool no_color; /**< Disable ANSI colors in output */
    SnLogLevel log_level; /**< Minimum log level to print */
} SnTestConfig;

/**
 * @enum SnTestConfigSetField
 * @brief The fields of SnTestConfig a command line can set.
 */
typedef enum SnTestConfigSetField {
    SN_TEST_SET_MAX_FAILURES = SN_BIT_FLAG(0), /**< SnTestConfig::max_failures */
    SN_TEST_SET_THREAD_COUNT = SN_BIT_FLAG(1), /**< SnTestConfig::thread_count */
    SN_TEST_SET_TIMEOUT_NS = SN_BIT_FLAG(2), /**< SnTestConfig::timeout_ns */
    SN_TEST_SET_FILTER = SN_BIT_FLAG(3), /**< SnTestConfig::filter */
    SN_TEST_SET_NO_COLOR = SN_BIT_FLAG(4), /**< SnTestConfig::no_color */
    SN_TEST_SET_LOG_LEVEL = SN_BIT_FLAG(5), /**< SnTestConfig::log_level */
} SnTestConfigSetField;

/**
 * @struct SnTestConfigOverride
 * @brief A partial configuration from the command line.
 *
 * sn_test_parse_args() fills one of these. Only the fields named in set are
 * meaningful, and the runner applies those over whatever SN_TEST_INIT() wrote,
 * so a flag given on the command line always wins.
 */
typedef struct SnTestConfigOverride {
    SnTestConfig config; /**< Values for the fields named in set */
    uint32_t set; /**< Bitwise or of SnTestConfigSetField */
    bool list; /**< Print the matching test names and run nothing */
} SnTestConfigOverride;

/**
 * @enum SnTestParseResult
 * @brief What the caller should do after parsing the command line.
 */
typedef enum SnTestParseResult {
    SN_TEST_PARSE_RUN, /**< Run the tests with the parsed override */
    SN_TEST_PARSE_EXIT, /**< Help was asked for, exit successfully */
    SN_TEST_PARSE_ERROR, /**< The command line was wrong, exit with 2 */
} SnTestParseResult;

/**
 * @typedef sn_test_init_fn
 * @brief Signature of the init hook.
 *
 * @param test_config Pointer to configuration to customize.
 *
 * @return Returns true on success, else false (aborts the run).
 */
typedef bool (*sn_test_init_fn)(SnTestConfig *test_config);

/**
 * @typedef sn_test_deinit_fn
 * @brief Signature of the deinit, setup and teardown hooks.
 */
typedef void (*sn_test_deinit_fn)(void);

/**
 * @struct SnTestHook
 * @brief A lifecycle hook with its type.
 */
typedef struct SnTestHook {
    SnTestHookType type; /**< Type of the hook */

    union {
        sn_test_init_fn init; /**< Init hook (SN_TEST_HOOK_INIT) */
        sn_test_deinit_fn deinit; /**< Deinit hook (SN_TEST_HOOK_DEINIT) */
        sn_test_deinit_fn setup; /**< Setup hook (SN_TEST_HOOK_SETUP) */
        sn_test_deinit_fn teardown; /**< Teardown hook (SN_TEST_HOOK_TEARDOWN) */
    } fn;
} SnTestHook;

#if defined(SN_OS_MAC)
    #include <mach-o/dyld.h>
    #include <mach-o/getsect.h>

    #define SN_TEST_SECTION __attribute__((used, section("__DATA,sn_tests")))
    #define SN_TEST_HOOK_SECTION __attribute__((used, section("__DATA,sn_test_hooks")))

SN_INLINE SnTest **sn_test_macos_section_begin(size_t *count) {
    unsigned long size = 0;

    const struct mach_header_64 *header = (const struct mach_header_64 *)_dyld_get_image_header(0);

    SnTest **data = (SnTest **)getsectiondata(header, "__DATA", "sn_tests", &size);

    *count = size / sizeof(SnTest);
    return data;
}

    #define SN_TEST_BEGIN_COUNT(count) sn_test_macos_section_begin(&(count))

SN_INLINE SnTestHook **sn_test_macos_hooks_begin(size_t *count) {
    unsigned long size = 0;

    const struct mach_header_64 *header = (const struct mach_header_64 *)_dyld_get_image_header(0);

    SnTestHook **data = (SnTestHook **)getsectiondata(header, "__DATA", "sn_test_hooks", &size);

    *count = size / sizeof(SnTestHook);
    return data;
}

    #define SN_TEST_HOOK_BEGIN_COUNT(count) sn_test_macos_hooks_begin(&(count))
#elif defined(SN_COMPILER_GCC) || defined(SN_COMPILER_CLANG)
    #define SN_TEST_SECTION __attribute__((used, section("sn_tests")))
    #define SN_TEST_HOOK_SECTION __attribute__((used, section("sn_test_hooks")))

extern SnTest *__start_sn_tests;
extern SnTest *__stop_sn_tests;

    #define SN_TEST_BEGIN() (&__start_sn_tests)
    #define SN_TEST_END() (&__stop_sn_tests)

extern SnTestHook *__start_sn_test_hooks;
extern SnTestHook *__stop_sn_test_hooks;

    #define SN_TEST_HOOK_BEGIN() (&__start_sn_test_hooks)
    #define SN_TEST_HOOK_END() (&__stop_sn_test_hooks)
#elif defined(SN_COMPILER_MSVC)
    #pragma section("sn_tests$a", read);
    #pragma section("sn_tests$m", read);
    #pragma section("sn_tests$z", read);
    #pragma section("sn_test_hooks$a", read);
    #pragma section("sn_test_hooks$m", read);
    #pragma section("sn_test_hooks$z", read);

__declspec(allocate("sn_tests$a")) static SnTest *__sn_tests_start = NULL;
__declspec(allocate("sn_tests$z")) static SnTest *__sn_tests_end = NULL;

__declspec(allocate("sn_test_hooks$a")) static SnTestHook *__sn_test_hooks_start = NULL;
__declspec(allocate("sn_test_hooks$z")) static SnTestHook *__sn_test_hooks_end = NULL;

    #define SN_TEST_SECTION __declspec(allocate("sn_tests$m"))
    #define SN_TEST_HOOK_SECTION __declspec(allocate("sn_test_hooks$m"))

    #define SN_TEST_BEGIN() (&__sn_tests_start + 1)
    #define SN_TEST_END() (&__sn_tests_end)

    #define SN_TEST_HOOK_BEGIN() (&__sn_test_hooks_start + 1)
    #define SN_TEST_HOOK_END() (&__sn_test_hooks_end)
#else
    #error "Should not reach here"
#endif

/**
 * @brief Define and register a test function.
 *
 * The test function must return a SnTestResult.
 *
 * @param fn Name of the test function (also used as the test name).
 */
#define SN_TEST_ADD(fn)                                                     \
    static SnTestResult fn(void);                                           \
    static SnTest sn_test_struct_##fn = {#fn, fn};                          \
    static SN_TEST_SECTION SnTest *sn_test_ptr_##fn = &sn_test_struct_##fn; \
    static SnTestResult fn(void)

/**
 * @brief Define the init hook.
 *
 * Called once before any test. If the hook returns false, the run aborts.
 * Use this to customize the SnTestConfig.
 *
 * The parameter is named test_config rather than config so that it does not read
 * like a type name at the call site.
 *
 * @param test_config Pointer to configuration to customize.
 *
 * @return Returns true on success, else false.
 */
#define SN_TEST_INIT()                                                                         \
    static bool sn_test_hook_init_impl(SnTestConfig *test_config);                             \
    static SnTestHook sn_test_hook_init_struct = {                                             \
        SN_TEST_HOOK_INIT,                                                                     \
        {.init = &sn_test_hook_init_impl},                                                     \
    };                                                                                         \
    static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_init_ptr = &sn_test_hook_init_struct; \
    static bool sn_test_hook_init_impl(SnTestConfig *test_config)

/**
 * @brief Define the deinit hook.
 *
 * Called once after all tests have run.
 */
#define SN_TEST_DEINIT()                                                                           \
    static void sn_test_hook_deinit_impl(void);                                                    \
    static SnTestHook sn_test_hook_deinit_struct = {                                               \
        SN_TEST_HOOK_DEINIT,                                                                       \
        {.deinit = &sn_test_hook_deinit_impl},                                                     \
    };                                                                                             \
    static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_deinit_ptr = &sn_test_hook_deinit_struct; \
    static void sn_test_hook_deinit_impl(void)

/**
 * @brief Define the setup hook.
 *
 * Called before each test.
 */
#define SN_TEST_SETUP()                                                                          \
    static void sn_test_hook_setup_impl(void);                                                   \
    static SnTestHook sn_test_hook_setup_struct = {                                              \
        SN_TEST_HOOK_SETUP,                                                                      \
        {.setup = &sn_test_hook_setup_impl},                                                     \
    };                                                                                           \
    static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_setup_ptr = &sn_test_hook_setup_struct; \
    static void sn_test_hook_setup_impl(void)

/**
 * @brief Define the teardown hook.
 *
 * Called after each test.
 */
#define SN_TEST_TEARDOWN()                                                                             \
    static void sn_test_hook_teardown_impl(void);                                                      \
    static SnTestHook sn_test_hook_teardown_struct = {                                                 \
        SN_TEST_HOOK_TEARDOWN,                                                                         \
        {.teardown = &sn_test_hook_teardown_impl},                                                     \
    };                                                                                                 \
    static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_teardown_ptr = &sn_test_hook_teardown_struct; \
    static void sn_test_hook_teardown_impl(void)

/**
 * @brief Run all the registered tests, then apply a command line override.
 *
 * SN_TEST_INIT() runs first and may set any field it likes. The fields named in
 * override->set are then written over the top, so a flag given on the command
 * line always beats a value hardcoded in the hook. If override->list is set the
 * matching test names are printed and nothing is run.
 *
 * @param config Pointer to configuration (can be NULL-initialized).
 * @param override Pointer to the command line override, or NULL for none.
 *
 * @return Returns the number of failures (0 on success, -1 on init failure).
 */
SN_TEST_API int sn_test_run_all_tests_override(SnTestConfig *config, const SnTestConfigOverride *override);

/**
 * @brief Run all the registered tests.
 *
 * Also applies the logger settings (color, log level) from the config
 * and prints the summary after the run.
 *
 * @param config Pointer to configuration (can be NULL-initialized).
 *
 * @return Returns the number of failures (0 on success, -1 on init failure).
 */
SN_TEST_API int sn_test_run_all_tests(SnTestConfig *config);

/**
 * @brief Parse a command line into a configuration override.
 *
 * Recognised options are --filter, --max-failures, --threads, --timeout,
 * --log-level, --no-color, --list and --help. --timeout takes milliseconds and
 * stores nanoseconds. --log-level takes trace, debug, info, warn, error or
 * fatal, in any case. An empty --filter means no filter, matching the
 * SN_TESTConfig field it feeds.
 *
 * Usage errors are reported on stderr and ask for --help.
 *
 * @param out Receives the override, zeroed first. May be NULL if only the
 *            result matters, which is what the --help and error paths do.
 * @param argc Argument count from main.
 * @param argv Argument vector from main. argv[0] may be NULL.
 *
 * @return Returns what the caller should do next.
 */
SN_TEST_API SnTestParseResult sn_test_parse_args(SnTestConfigOverride *out, int argc, char **argv);

/**
 * @brief Generate the entry point for the test executable.
 *
 * Initializes logging, parses the command line, runs the tests with a
 * zero-initialized config that the command line then overrides, deinitializes
 * logging and returns the exit code. The exit code is 0 when every test passed,
 * 1 when any failed, and 2 when the command line could not be parsed.
 */
#define SN_TEST_RUN()                                                     \
    int main(int argc, char **argv) {                                     \
        sn_test_logger_init();                                            \
        SnTestConfigOverride override = {0};                              \
        int ret = 0;                                                      \
        switch (sn_test_parse_args(&override, argc, argv)) {              \
            case SN_TEST_PARSE_RUN: {                                     \
                SnTestConfig config = {0};                                \
                ret = sn_test_run_all_tests_override(&config, &override); \
                break;                                                    \
            }                                                             \
            case SN_TEST_PARSE_EXIT:                                      \
                break;                                                    \
            case SN_TEST_PARSE_ERROR:                                     \
                ret = 2;                                                  \
                break;                                                    \
        }                                                                 \
        sn_test_logger_deinit();                                          \
        return ret;                                                       \
    }

#include "sntest/asserts.h"
