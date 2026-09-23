#pragma once

#include "sntest/api.h"
#include "sntest/logger.h"

#include <sncore/defines.h>

typedef enum SnTestResult {
    SN_TEST_PASS,
    SN_TEST_FAIL,
    SN_TEST_SKIP,
} SnTestResult;

typedef SnTestResult (*sn_test_fn)(void);

typedef struct SnTest {
    const char *name;
    sn_test_fn fn;
} SnTest;

typedef enum SnTestHookType {
    SN_TEST_HOOK_INIT,
    SN_TEST_HOOK_DEINIT,
    SN_TEST_HOOK_SETUP,
    SN_TEST_HOOK_TEARDOWN,
} SnTestHookType;

typedef struct SnTestConfig {
    bool fail_fast;
} SnTestConfig;

typedef bool (*sn_test_init_fn)(SnTestConfig *config);

typedef void (*sn_test_deinit_fn)(void);

typedef struct SnTestHook {
    SnTestHookType type;

    union {
        sn_test_init_fn init;
        sn_test_deinit_fn deinit;
        sn_test_deinit_fn setup;
        sn_test_deinit_fn teardown;
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

#define SN_TEST_ADD(fn)                                                     \
    static SnTestResult fn(void);                                           \
    static SnTest sn_test_struct_##fn = {#fn, fn};                          \
    static SN_TEST_SECTION SnTest *sn_test_ptr_##fn = &sn_test_struct_##fn; \
    static SnTestResult fn(void)

#define SN_TEST_INIT()                                                                         \
    static bool sn_test_hook_init_impl(SnTestConfig *config);                                  \
    static SnTestHook sn_test_hook_init_struct = {                                             \
        SN_TEST_HOOK_INIT,                                                                     \
        {.init = &sn_test_hook_init_impl},                                                     \
    };                                                                                         \
    static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_init_ptr = &sn_test_hook_init_struct; \
    static bool sn_test_hook_init_impl(SnTestConfig *config)

#define SN_TEST_DEINIT()                                                                           \
    static void sn_test_hook_deinit_impl(void);                                                    \
    static SnTestHook sn_test_hook_deinit_struct = {                                               \
        SN_TEST_HOOK_DEINIT,                                                                       \
        {.deinit = &sn_test_hook_deinit_impl},                                                     \
    };                                                                                             \
    static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_deinit_ptr = &sn_test_hook_deinit_struct; \
    static void sn_test_hook_deinit_impl(void)

#define SN_TEST_SETUP()                                                                          \
    static void sn_test_hook_setup_impl(void);                                                   \
    static SnTestHook sn_test_hook_setup_struct = {                                              \
        SN_TEST_HOOK_SETUP,                                                                      \
        {.setup = &sn_test_hook_setup_impl},                                                     \
    };                                                                                           \
    static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_setup_ptr = &sn_test_hook_setup_struct; \
    static void sn_test_hook_setup_impl(void)

#define SN_TEST_TEARDOWN()                                                                             \
    static void sn_test_hook_teardown_impl(void);                                                      \
    static SnTestHook sn_test_hook_teardown_struct = {                                                 \
        SN_TEST_HOOK_TEARDOWN,                                                                         \
        {.teardown = &sn_test_hook_teardown_impl},                                                     \
    };                                                                                                 \
    static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_teardown_ptr = &sn_test_hook_teardown_struct; \
    static void sn_test_hook_teardown_impl(void)

SN_TEST_API int sn_test_run_all_tests(SnTestConfig *config);

#define SN_TEST_RUN()                             \
    int main(void) {                              \
        sn_test_logger_init();                    \
        SnTestConfig config = {0};                \
        int ret = sn_test_run_all_tests(&config); \
        sn_test_logger_deinit();                  \
        return ret;                               \
    }

#include "sntest/asserts.h"
