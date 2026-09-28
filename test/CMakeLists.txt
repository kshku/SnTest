# Tests register themselves into a linker section that the linker delimits with
# __start_sn_tests and __stop_sn_tests. Those are defined per object that has
# the section, so a shared libsntest carries its own, empty, copy of the
# registry and the runner reads that instead of the executable's. There is no
# portable way to hide them, a version script only works with GNU ld, so
# SnTest stays static.
add_library(sntest STATIC)
target_compile_definitions(sntest PUBLIC SN_TEST_STATIC)

target_include_directories(sntest PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(sntest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(sntest PRIVATE sn_test_configs)
target_link_libraries(sntest PUBLIC sncore snlogger snthreads sntime)

add_subdirectory(src)
