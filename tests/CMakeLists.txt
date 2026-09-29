enable_testing()

set(SRCS
    test.c
)

add_executable(sn_test_test)
target_sources(sn_test_test PRIVATE ${SRCS})
target_link_libraries(sn_test_test PRIVATE sntest)

add_test(NAME sn_test_test COMMAND sn_test_test)

add_executable(sn_test_args)
target_sources(sn_test_args PRIVATE args.c)
target_link_libraries(sn_test_args PRIVATE sntest)

add_test(NAME sn_test_args COMMAND sn_test_args)

# The hook asks for "cherry" and cherry passes, so no arguments passes.
add_executable(sn_test_cli)
target_sources(sn_test_cli PRIVATE cli.c)
target_link_libraries(sn_test_cli PRIVATE sntest)

add_test(NAME sn_test_cli COMMAND sn_test_cli)

# --filter apple beats the hook's "cherry", and apple fails, so the command line
# winning shows up as a non-zero exit. This is the precedence rule under test.
add_test(NAME sn_test_cli_filter_overrides_hook COMMAND sn_test_cli --filter apple)
set_tests_properties(sn_test_cli_filter_overrides_hook PROPERTIES WILL_FAIL TRUE)

# Same binary, --list must print the matching names and run nothing, so it exits
# zero even though the filter would otherwise have reached the failing apple.
add_test(NAME sn_test_cli_list COMMAND sn_test_cli --list --filter cherry)
set_tests_properties(sn_test_cli_list PROPERTIES PASS_REGULAR_EXPRESSION "cherry")

add_test(NAME sn_test_cli_help COMMAND sn_test_cli --help)
set_tests_properties(sn_test_cli_help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

# A usage error exits 2, distinct from the 1 that means a test failed, so a CI
# job can tell "the tests broke" from "the command line was wrong".
add_test(NAME sn_test_cli_bad_option COMMAND sn_test_cli --nonsense)
set_tests_properties(sn_test_cli_bad_option PROPERTIES WILL_FAIL TRUE)

add_test(NAME sn_test_cli_bad_value COMMAND sn_test_cli --threads banana)
set_tests_properties(sn_test_cli_bad_value PROPERTIES WILL_FAIL TRUE)

add_executable(sn_test_log)
target_sources(sn_test_log PRIVATE log.c)
target_link_libraries(sn_test_log PRIVATE sntest)

add_test(NAME sn_test_log COMMAND sn_test_log)

add_executable(sn_test_no_hooks)
target_sources(sn_test_no_hooks PRIVATE no_hooks.c)
target_link_libraries(sn_test_no_hooks PRIVATE sntest)

add_test(NAME sn_test_no_hooks COMMAND sn_test_no_hooks)

add_executable(sn_test_no_tests)
target_sources(sn_test_no_tests PRIVATE no_tests.c)
target_link_libraries(sn_test_no_tests PRIVATE sntest)

add_test(NAME sn_test_no_tests COMMAND sn_test_no_tests)

add_executable(sn_test_assert_fail)
target_sources(sn_test_assert_fail PRIVATE assert_fail.c)
target_link_libraries(sn_test_assert_fail PRIVATE sntest)

add_test(NAME sn_test_assert_fail COMMAND sn_test_assert_fail)
set_tests_properties(sn_test_assert_fail PROPERTIES WILL_FAIL TRUE)

add_executable(sn_test_max_failures)
target_sources(sn_test_max_failures PRIVATE max_failures.c)
target_link_libraries(sn_test_max_failures PRIVATE sntest)

add_test(NAME sn_test_max_failures COMMAND sn_test_max_failures)
set_tests_properties(sn_test_max_failures PROPERTIES WILL_FAIL TRUE)

add_executable(sn_test_filter)
target_sources(sn_test_filter PRIVATE filter.c)
target_link_libraries(sn_test_filter PRIVATE sntest)

add_test(NAME sn_test_filter COMMAND sn_test_filter)

add_executable(sn_test_timeout)
target_sources(sn_test_timeout PRIVATE timeout.c)
target_link_libraries(sn_test_timeout PRIVATE sntest)

add_test(NAME sn_test_timeout COMMAND sn_test_timeout)
set_tests_properties(sn_test_timeout PROPERTIES WILL_FAIL TRUE)

add_executable(sn_test_parallel)
target_sources(sn_test_parallel PRIVATE parallel.c)
target_link_libraries(sn_test_parallel PRIVATE sntest)

add_test(NAME sn_test_parallel COMMAND sn_test_parallel)
