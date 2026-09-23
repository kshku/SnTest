enable_testing()

set(SRCS
    test.c
)

add_executable(sn_test_test)
target_sources(sn_test_test PRIVATE ${SRCS})
target_link_libraries(sn_test_test PRIVATE sntest)

add_test(NAME sn_test_test COMMAND sn_test_test)

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
