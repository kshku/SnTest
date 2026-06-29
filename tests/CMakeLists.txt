enable_testing()

set(SRCS
    test.c
)

add_executable(sn_test_test)
target_sources(sn_test_test PRIVATE ${SRCS})
target_link_libraries(sn_test_test PRIVATE sntest)

add_test(NAME sn_test_test COMMAND sn_test_test)
