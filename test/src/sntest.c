#include "sntest/sntest.h"

#include "sntest/logger.h"

#include <sntime/sntime.h>
#include <stdio.h>
#include <string.h>

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

static void format_duration(SnTimeNs ns, char *buf, size_t cap) {
    if (ns < 1000) {
        snprintf(buf, cap, "%lld ns", (long long)ns);
    } else if (ns < 1000000) {
        snprintf(buf, cap, "%.2f us", (double)ns / 1e3);
    } else if (ns < 1000000000) {
        snprintf(buf, cap, "%.2f ms", (double)ns / 1e6);
    } else {
        snprintf(buf, cap, "%.2f s", (double)ns / 1e9);
    }
}

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

static void run_tests(SnTestConfig *config) {
#if defined(SN_OS_MAC)
    size_t count = 0;
    SnTest **tests = SN_TEST_BEGIN_COUNT(count);
#else
    SnTest **begin = SN_TEST_BEGIN();
    SnTest **end = SN_TEST_END();
    SnTest **tests = begin;
    size_t count = (size_t)(end - begin);
#endif
    if (!tests || count == 0) return;

    const char *filter = (config->filter && config->filter[0]) ? config->filter : NULL;

    for (size_t k = 0; k < count; ++k) {
        SnTest *it = tests[k];
        if (!it) continue;
        if (filter && !strstr(it->name, filter)) continue;

        log_msg("Running test: %s...\n", it->name);
        SnTimePoint start = sn_time_point_now();
        if (context.setup) context.setup();
        SnTestResult res = it->fn();
        if (context.teardown) context.teardown();
        SnTimeNs elapsed = sn_time_elapsed_ns(start, sn_time_point_now());

        char time_buf[32];
        format_duration(elapsed, time_buf, sizeof(time_buf));

        if (res == SN_TEST_PASS && config->timeout_ns && elapsed > config->timeout_ns) {
            char limit_buf[32];
            format_duration(config->timeout_ns, limit_buf, sizeof(limit_buf));
            log_msg("%s -> timed out (%s > limit %s)\n", it->name, time_buf, limit_buf);
            res = SN_TEST_FAIL;
        }

        context.stats.total++;
        switch (res) {
            case SN_TEST_PASS:
                context.stats.passed++;
                log_msg("%s -> PASS (%s)\n", it->name, time_buf);
                break;
            case SN_TEST_FAIL:
                context.stats.failed++;
                log_msg("%s -> FAIL (%s)\n", it->name, time_buf);
                if (config->max_failures && context.stats.failed >= config->max_failures) {
                    log_msg("Stopping: max_failures limit reached\n", NULL);
                    return;
                }
                break;
            case SN_TEST_SKIP:
                context.stats.skipped++;
                log_msg("%s -> SKIP (%s)\n", it->name, time_buf);
                break;
            default:
                break;
        }
    }
}

int sn_test_run_all_tests(SnTestConfig *config) {
    SnTimePoint total_start = sn_time_point_now();

    resolve_hooks();

    if (context.init && !context.init(config)) {
        log_msg("Failed to initialize tests\n", NULL);
        return -1;
    }

    if (config->no_color) sn_test_logger_disable_color();
    sn_test_set_log_level(config->log_level);

    run_tests(config);

    if (context.deinit) context.deinit();

    SnTimeNs total = sn_time_elapsed_ns(total_start, sn_time_point_now());
    char total_buf[32];
    format_duration(total, total_buf, sizeof(total_buf));

    log_msg("\nSummary\n------------\n", NULL);
    log_msg("Total: %d\n\n", context.stats.total);
    log_msg("Total time: %s\n\n", total_buf);
    log_msg_fg(COLOR_GREEN, "Passed: %d\n", context.stats.passed);
    log_msg_fg(COLOR_RED, "Failed: %d\n", context.stats.failed);
    log_msg_fg(COLOR_YELLOW, "Skipped: %d\n", context.stats.skipped);
    log_msg("------------\n", NULL);
    return context.stats.failed > 0;
}
