#include "sntest/sntest.h"

int sn_test_run_all_tests(SnTestConfig *config) {
    SN_UNUSED(config);
#if defined(SN_OS_MAC)
    size_t count = 0;
    SnTest **tests = SN_TEST_BEGIN_COUNT(count);

    for (size_t i = 0; i < count; ++i) {
        SnTest *it = tests[i];
#else
    SnTest **begin = SN_TEST_BEGIN();
    SnTest **end = SN_TEST_END();

    for (SnTest **p = begin; p < end; ++p) {
        SnTest *it = *p;
#endif
        if (!it) continue;

        // log_info("Running test: %s", it->name);
        it->fn();
    }

    return 0;
}
