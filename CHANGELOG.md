# Changelog

## [0.3.0] - 2026-09-28

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