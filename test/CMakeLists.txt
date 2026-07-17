if(SN_TEST_BUILD_SHARED)
    add_library(sntest SHARED)
    target_compile_definitions(sntest PRIVATE SN_EXPORT)
else()
    add_library(sntest STATIC)
    target_compile_definitions(sntest PUBLIC SN_TEST_STATIC)
endif()

target_include_directories(sntest PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(sntest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(sntest PRIVATE sn_test_configs)
target_link_libraries(sntest PUBLIC sncore snlogger)

add_subdirectory(src)
