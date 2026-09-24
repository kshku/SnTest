# SnTest

Minimal C testing library. Provides automatic test registration, assertions, optional lifecycle hooks,
per-test timing, filtering, timeouts, and a configurable parallel runner.

Tests are registered via linker sections, so no manual test list is required — just include the header,
add tests, and call `SN_TEST_RUN()`.

## Features

- **Automatic registration** — tests are discovered at link time; no registry to maintain
- **Assertions** — integer, pointer, string, char, and memory comparisons with expressive output
- **Lifecycle hooks** — optional `init` / `deinit` / `setup` / `teardown` callbacks
- **Per-test timing** — each test reports its duration; an optional overall timeout turns slow tests into failures
- **Filtering** — run only tests whose name contains a substring
- **Configurable stop condition** — stop after `max_failures` failures (``0`` = unlimited)
- **Parallel runner** — `thread_count > 1` runs tests across worker threads
- **ANSI-colored output** — color and graphics modes, disableable and level-controlled
- **No allocation** — the runner uses fixed-size buffers and no dynamic memory allocation

## API

### Registering Tests

| Macro | Description |
|-------|-------------|
| `SN_TEST_ADD(name)` | Define and register a test function. It must return a `SnTestResult` (`SN_TEST_PASS`, `SN_TEST_FAIL`, `SN_TEST_SKIP`) |
| `SN_TEST_RUN()` | Generate a `main()` that initializes logging, runs all tests, and returns the appropriate exit code |

Each test runs as:

1. `setup()` (if registered)
2. the test function
3. `teardown()` (if registered)

A test that takes longer than `timeout_ns` is reported as `FAIL`, even if it returned `SN_TEST_PASS`.

### Lifecycle Hooks

| Macro | Signature | Called |
|-------|-----------|--------|
| `SN_TEST_INIT()` | `bool (SnTestConfig *config)` | Once, before any test; `false` aborts the run |
| `SN_TEST_DEINIT()` | `void (void)` | Once, after all tests |
| `SN_TEST_SETUP()` | `void (void)` | Before each test |
| `SN_TEST_TEARDOWN()` | `void (void)` | After each test |

### Configuration

Tests configure the runner through the `SnTestConfig` passed to the `SN_TEST_INIT()` hook
(or directly to `sn_test_run_all_tests()`).

| Field | Description |
|-------|-------------|
| `max_failures` | Stop the run after this many failures (`0` = unlimited) |
| `thread_count` | Number of worker threads (`0`/`1` = sequential) |
| `timeout_ns` | Per-test timeout in nanoseconds (`0` = disabled) |
| `filter` | Run only tests whose name contains this substring (`NULL` = all) |
| `no_color` | Disable ANSI colors in output |
| `log_level` | Minimum `SnLogLevel` to print |

### Programmatic Entry Point

```c
SN_TEST_API int sn_test_run_all_tests(SnTestConfig *config);
```

Runs all tests and returns the number of failures (`0` = success, non-zero = failures, `-1` = init failure).
Useful when you need a custom `main()` instead of `SN_TEST_RUN()`.

## Assertions

On failure, an assertion prints a diagnostic (expression, file, line, function) and returns
`SN_TEST_FAIL` immediately.

| Macro | Checks |
|-------|--------|
| `SN_TEST_ASSERT(x)` | `x` is truthy |
| `SN_TEST_ASSERT_TRUE(x)` / `SN_TEST_ASSERT_FALSE(x)` | `x` is `true` / `false` |
| `SN_TEST_ASSERT_EQ(a, b)` / `SN_TEST_ASSERT_NEQ(a, b)` | `a == b` / `a != b` |
| `SN_TEST_ASSERT_LT`, `SN_TEST_ASSERT_LE`, `SN_TEST_ASSERT_GT`, `SN_TEST_ASSERT_GE` | order comparisons |
| `SN_TEST_ASSERT_NULL(x)` / `SN_TEST_ASSERT_NOT_NULL(x)` | `x` is / is not `NULL` |
| `SN_TEST_ASSERT_PTR_EQ(a, b)` / `SN_TEST_ASSERT_PTR_NEQ(a, b)` | pointer comparison |
| `SN_TEST_ASSERT_STR_EQ(a, b)` / `SN_TEST_ASSERT_STR_NEQ(a, b)` | `strcmp` comparison (NULL-safe) |
| `SN_TEST_ASSERT_STR_N_EQ(a, b, n)` | `strncmp` comparison (NULL-safe) |
| `SN_TEST_ASSERT_CHAR_EQ(a, b)` / `SN_TEST_ASSERT_CHAR_NEQ(a, b)` | char comparison with value dump |
| `SN_TEST_ASSERT_MEM_EQ(a, b, size)` / `SN_TEST_ASSERT_MEM_NEQ(a, b, size)` | `memcmp` comparison |

## Logging

`log_msg` and friends wrap `sn_test_log_msg()` and support ANSI colors and graphics modes.

```c
log_msg("Hello\n");                              // default color
log_msg_fg(COLOR_GREEN, "Pass\n");               // green text
log_msg_fg_mode(COLOR_RED, MODE_BOLD, "Fail\n"); // bold red text
```

## Usage

Create a single source file that becomes your test executable:

```c
#include <sntest/sntest.h>

SN_TEST_INIT() {
    config->thread_count = 4;
    config->max_failures = 3;
    return true;
}

SN_TEST_ADD(math_addition) {
    SN_TEST_ASSERT_EQ(2 + 2, 4);
    return SN_TEST_PASS;
}

SN_TEST_ADD(math_division) {
    SN_TEST_ASSERT_EQ(6 / 2, 3);
    return SN_TEST_PASS;
}

SN_TEST_ADD(word_length) {
    SN_TEST_ASSERT_STR_EQ(strlen("hello"), 5);
    return SN_TEST_PASS;
}

SN_TEST_ADD(skip_me) {
    return SN_TEST_SKIP;
}

SN_TEST_RUN()
```

Run the executable and see per-test results plus a summary:

```
Running test: math_addition...
math_addition -> PASS (42 ns)
Running test: math_division...
math_division -> PASS (38 ns)
Running test: word_length...
word_length -> PASS (50 ns)
Running test: skip_me...
skip_me -> SKIP (0 ns)

Summary
------------
Total: 4

Total time: 230 us

Passed: 3
Failed: 0
Skipped: 1
------------
```

## Adding to your project

```cmake
include(FetchContent)
FetchContent_Declare(sntest
    GIT_REPOSITORY https://github.com/kshku/SnTest.git
    GIT_TAG <tag>  # e.g., v0.1.0
)
FetchContent_MakeAvailable(sntest)

target_link_libraries(myapp PRIVATE sntest)
```

## Build

```sh
cmake -B build
cmake --build build
```

| Option | Default | Description |
|--------|---------|-------------|
| `SN_TEST_BUILD_SHARED` | `OFF` | Build as shared library |
| `SN_TEST_BUILD_TEST` | `OFF` | Build tests |

## Documentation

API documentation can be generated using Doxygen.

```sh
cmake --build build --target sn_test_docs
```

Generated documentation will be available in `build/docs/output/html/index.html`.

## Platform Support

| Platform | Backend |
|----------|---------|
| Linux | pthreads |
| macOS | pthreads |
| Windows | Win32 threads |

## Dependencies

- **SnCore** — fetched automatically via FetchContent
- **SnLogger** — fetched automatically via FetchContent
- **SnThreads** — fetched automatically via FetchContent
- **SnTime** — fetched automatically via FetchContent