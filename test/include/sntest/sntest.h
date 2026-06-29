#pragma once

#include "sntest/api.h"

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

typedef struct SnTestConfig {
    bool placeholder;
} SnTestConfig;

#if defined(SN_OS_MAC)
    #include <mach-o/dyld.h>
    #include <mach-o/getsect.h>

    #define SN_TEST_SECTION __attribute__((used, section("__DATA,sn_tests")))

SN_INLINE SnTest **sn_test_macos_section_begin(size_t *count) {
    unsigned long size = 0;

    const struct mach_header_64 *header = (const struct mach_header_64 *)_dyld_get_image_header(0);

    SnTest **data = (SnTest **)getsectiondata(header, "__DATA", "sn_tests", &size);

    *count = size / sizeof(SnTest);
    return data;
}

    #define SN_TEST_BEGIN_COUNT(count) sn_test_macos_section_begin(&(count))
#elif defined(SN_COMPILER_GCC) || defined(SN_COMPILER_CLANG)
    #define SN_TEST_SECTION __attribute__((used, section("sn_tests")))

extern SnTest *__start_sn_tests;
extern SnTest *__stop_sn_tests;

    #define SN_TEST_BEGIN() (&__start_sn_tests)
    #define SN_TEST_END() (&__stop_sn_tests)
#elif defined(SN_COMPILER_MSVC)
    #pragma section("sn_tests$a", read);
    #pragma section("sn_tests$m", read);
    #pragma section("sn_tests$z", read);

__declspec(allocate("sn_tests$a")) static SnTest *__sn_tests_start = NULL;
__declspec(allocate("sn_tests$z")) static SnTest *__sn_tests_end = NULL;

    #define SN_TEST_SECTION __declspec(allocate("sn_tests$m"))

    #define SN_TEST_BEGIN() (&__sn_tests_start + 1)
    #define SN_TEST_END() (&__sn_tests_end)
#else
    #error "Should not reach here"
#endif

#define SN_TEST_ADD(fn)                                                     \
    static SnTestResult fn(void);                                           \
    static SnTest sn_test_struct_##fn = {#fn, fn};                          \
    static SN_TEST_SECTION SnTest *sn_test_ptr_##fn = &sn_test_struct_##fn; \
    static SnTestResult fn(void)

SN_TEST_API int sn_test_run_all_tests(SnTestConfig *config);

bool sn_test_init(SnTestConfig *config);
void sn_test_deinit(void);

#define SN_TEST_RUN()                             \
    int main(void) {                              \
        SnTestConfig config = {0};                \
        if (!sn_test_init(&config)) return -1;    \
        int ret = sn_test_run_all_tests(&config); \
        sn_test_deinit();                         \
        return ret;                               \
    }
