#include "sntest/sntest.h"

#include "sntest/logger.h"

typedef struct SnTestStats {
    uint32_t total, passed, failed, skipped;
} SnTestStats;

typedef struct SnTestContext {
    SnTestStats stats;
    sn_test_init_fn init;
    sn_test_deinit_fn deinit;
    sn_test_deinit_fn setup;
    sn_test_deinit_fn teardown;
} SnTestContext;

static SnTestContext context = {0};

static SN_TEST_HOOK_SECTION SnTestHook *sn_test_hook_anchor = NULL;
static SN_TEST_SECTION SnTest *sn_test_anchor = NULL;

static SnTestHook *find_hook(SnTestHook **hooks, size_t count, SnTestHookType type) {
    SnTestHook *found = NULL;

    for (size_t i = 0; i < count; ++i) {
        SnTestHook *it = hooks[i];
        if (!it || it->type != type) continue;
        if (found) {
            log_msg("Warning: duplicate test hook registered, ignoring the extra one\n", NULL);
            continue;
        }
        found = it;
    }

    return found;
}

static void resolve_hooks(void) {
#if defined(SN_OS_MAC)
    size_t count = 0;
    SnTestHook *hooks = SN_TEST_HOOK_BEGIN_COUNT(count);
#else
    SnTestHook **begin = SN_TEST_HOOK_BEGIN();
    SnTestHook **end = SN_TEST_HOOK_END();
    SnTestHook **hooks = begin;
    size_t count = (size_t)(end - begin);
#endif
    if (!hooks || count == 0) return;

    SnTestHook *init = find_hook(hooks, count, SN_TEST_HOOK_INIT);
    if (init) context.init = init->fn.init;

    SnTestHook *deinit = find_hook(hooks, count, SN_TEST_HOOK_DEINIT);
    if (deinit) context.deinit = deinit->fn.deinit;

    SnTestHook *setup = find_hook(hooks, count, SN_TEST_HOOK_SETUP);
    if (setup) context.setup = setup->fn.setup;

    SnTestHook *teardown = find_hook(hooks, count, SN_TEST_HOOK_TEARDOWN);
    if (teardown) context.teardown = teardown->fn.teardown;
}

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

        log_msg("Running test: %s...\n", it->name);
        if (context.setup) context.setup();
        SnTestResult res = it->fn();
        if (context.teardown) context.teardown();

        context.stats.total++;
        switch (res) {
            case SN_TEST_PASS:
                context.stats.passed++;
                log_msg("%s -> PASS\n", it->name);
                break;
            case SN_TEST_FAIL:
                context.stats.failed++;
                log_msg("%s -> FAIL\n", it->name);
                break;
            case SN_TEST_SKIP:
                context.stats.skipped++;
                log_msg("%s -> SKIP\n", it->name);
                break;
            default:
                break;
        }
    }
}

int sn_test_run_all_tests(SnTestConfig *config) {
    SN_UNUSED(config);
    resolve_hooks();

    if (context.init && !context.init(config)) {
        log_msg("Failed to initialize tests\n", NULL);
        return -1;
    }

    run_tests();

    if (context.deinit) context.deinit();

    log_msg("\nSummary\n------------\n", NULL);
    log_msg("Total: %d\n\n", context.stats.total);
    log_msg_fg(COLOR_GREEN, "Passed: %d\n", context.stats.passed);
    log_msg_fg(COLOR_RED, "Failed: %d\n", context.stats.failed);
    log_msg_fg(COLOR_YELLOW, "Skipped: %d\n", context.stats.skipped);
    log_msg("------------\n", NULL);
    return context.stats.failed > 0;
}
