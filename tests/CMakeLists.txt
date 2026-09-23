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
