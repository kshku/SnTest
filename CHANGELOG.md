# Changelog

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