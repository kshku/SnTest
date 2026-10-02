# Changelog

## [0.4.2] - 2026-10-02

### Changed
- -Wconversion and -Wsign-conversion are on for gcc and clang. The code was
  already clean of both, and stays clean with the conversion warnings its
  dependencies now carry

## [0.4.1] - 2026-09-28

### Changed
- Take sncore v0.3.1 rather than v0.2.0
- Take snlogger v0.3.2 rather than v0.3.1
- Take snthreads v0.2.2 rather than v0.2.1

## [0.4.0] - 2026-09-28

### Added
- The generated entry point takes a command line. sn_test_parse_args() reads
  --filter, --max-failures, --threads, --timeout, --log-level, --no-color, --list
  and --help into an SnTestConfigOverride, and SN_TEST_RUN() hands it to
  sn_test_run_all_tests_override(). --timeout takes milliseconds and stores
  nanoseconds, since nobody types nanoseconds, and --log-level is case
  insensitive
- A usage error exits 2, which is distinct from the 1 that means a test failed,
  so a CI job can tell the tests broke from the command line being wrong
- The parameter of SN_TEST_INIT() is named test_config rather than config, so it
  does not read like a type name at the call site

### Changed
- A flag on the command line now wins over a value hardcoded in SN_TEST_INIT().
  The parser records which fields were actually named, and only those are
  written over what the hook produced. That needs the set bitmask on
  SnTestConfigOverride: a shadow config compared against zero cannot tell
  --max-failures 0 from no --max-failures at all, so the flag would silently do
  nothing
- sn_test_run_all_tests() keeps its signature and now takes no override. Code
  with its own main does not have to change
- The platform ifdef that bounds the test and hook sections moved into two
  helpers, because --list needed it a third time

## [0.3.0] - 2026-09-28

## [0.3.0] - 2026-09-28

### Added
- A test that calls every macro in the log_msg family, both with and without
  arguments, so that changing how they forward their arguments cannot quietly
  drop a form

### Changed
- The color and escape code handling now lives in SnLogger's console sink, and
  SnLogger is pinned to v0.3.1 for it. The public API is unchanged: the
  SnTestColor and SnTestColorMode enums, the log_msg family of macros and the
  sn_test_logger functions are all exactly as they were
- Color is now decided on each write instead of once at init, and it follows
  the stream, so redirected output and CI logs are plain text
- sn_test_set_log_level and sn_test_logger_disable_color now take the logger
  mutex, so they cannot race a worker thread that is logging. Tests log from
  every worker thread once thread_count is above one

### Fixed
- The escape codes are no longer written and then scanned back out again when
  color is off. They were located by searching for the first 'm' with no bound
  on the search, and the trailing four bytes were assumed to be the reset. When
  color is off the plain text is now emitted directly
- The SGR codes for MODE_INVERSE, MODE_HIDDEN and MODE_STRIKETHROUGH were wrong.
  The modes were mapped to SGR by counting bits, but 6 is unused, so those three
  are SGR 7, 8 and 9 and not 6, 7 and 8
- snprintf was trusted to report how much it wrote, but it reports how much it
  wanted to write, so a message that did not fit advanced the buffer pointer
  past the end of the buffer. The check that noticed this ran after the damage
- The Too long message diagnostic is gone, along with the record buffer it
  existed for. There is no fixed size record any more, so a long message simply
  prints in full
- The stderr half of the sink was tracked and configured but never written to.
  Every record went to stdout
- The log_msg family of macros no longer uses , ##__VA_ARGS__ to swallow a
  missing argument. That is a GNU extension and is not in C, so -Wpedantic
  reports every one of the eight macros on clang. Each one now passes the format
  string as its first variadic argument, which puts the comma inside a call
  where there is nothing to swallow. The call forms are unchanged, and a message
  with no conversions is still just the format string, since fmt is a named
  parameter rather than part of the variadic part

## [0.2.0] - 2026-09-28

### Removed
- Remove the shared library build, SnTest is always static now. Tests register
  into a linker section that the linker delimits with __start_sn_tests and
  __stop_sn_tests, and a shared libsntest carries its own, empty, copy of that
  registry, which the runner then reads instead of the executable's. Hiding
  those symbols needs a linker version script, which only GNU ld has

## [0.1.0] - 2026-09-24

- First release. See [0.0.0] section in CHANGELOG.md for full changelog.

## [0.0.0] - 2026-06-29

### Added
- Test registration via linker sections (`SN_TEST_ADD`) and generated entry point (`SN_TEST_RUN`)
- Colored logger with ANSI support for test output
- Test summary with total, passed, failed and skipped counts
- Optional lifecycle hooks: init, deinit, setup, teardown
- Assertions for booleans, integers, pointers, strings, chars and memory blocks
- Per-test timing via SnTime
- Config options: max failures, timeout, filter, no color, log level
- Parallel test runner via `thread_count` config using SnThreads
- SnCore, SnLogger, SnThreads and SnTime dependencies
- CI workflows (Linux, macOS, Windows, formatting, sanitizers)