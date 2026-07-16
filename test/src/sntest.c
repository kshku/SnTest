#include "sntest/sntest.h"

typedef struct SnTestStats {
    uint32_t total, passed, failed, skipped;
} SnTestStats;

typedef struct SnTestContext {
    SnTestStats stats;
} SnTestContext;

static SnTestContext context = {0};

static void run_tests(void) {
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
        SnTestResult res = it->fn();

        context.stats.total++;
        switch (res) {
            case SN_TEST_PASS:
                context.stats.passed++;
                break;
            case SN_TEST_FAIL:
                context.stats.failed++;
                break;
            case SN_TEST_SKIP:
                context.stats.skipped++;
                break;
            default:
                break;
        }
    }
}

int sn_test_run_all_tests(SnTestConfig *config) {
    SN_UNUSED(config);
    run_tests();
    return context.stats.failed > 0;
}
