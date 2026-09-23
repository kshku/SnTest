#pragma once

#include "sntest/sntest.h"

#include <stddef.h>
#include <string.h>

#define SN_TEST_ASSERT(x)                                                                                    \
    do {                                                                                                     \
        if (!(x)) {                                                                                          \
            log_msg_fg(COLOR_RED, "Assertion failed: %s (%s:%d) in %s\n", #x, __FILE__, __LINE__, __func__); \
            return SN_TEST_FAIL;                                                                             \
        }                                                                                                    \
    } while (0)

#define SN_TEST_ASSERT_TRUE(x)                                                                    \
    do {                                                                                          \
        if (!(x)) {                                                                               \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s to be true (%s:%d) in %s\n", #x, \
                       __FILE__, __LINE__, __func__);                                             \
            return SN_TEST_FAIL;                                                                  \
        }                                                                                         \
    } while (0)

#define SN_TEST_ASSERT_FALSE(x)                                                                    \
    do {                                                                                           \
        if (x) {                                                                                   \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s to be false (%s:%d) in %s\n", #x, \
                       __FILE__, __LINE__, __func__);                                              \
            return SN_TEST_FAIL;                                                                   \
        }                                                                                          \
    } while (0)

#define SN_TEST_ASSERT_EQ(a, b)                                                                  \
    do {                                                                                         \
        if ((a) != (b)) {                                                                        \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s == %s (%s:%d) in %s\n", #a, #b, \
                       __FILE__, __LINE__, __func__);                                            \
            return SN_TEST_FAIL;                                                                 \
        }                                                                                        \
    } while (0)

#define SN_TEST_ASSERT_NEQ(a, b)                                                                 \
    do {                                                                                         \
        if ((a) == (b)) {                                                                        \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s != %s (%s:%d) in %s\n", #a, #b, \
                       __FILE__, __LINE__, __func__);                                            \
            return SN_TEST_FAIL;                                                                 \
        }                                                                                        \
    } while (0)

#define SN_TEST_ASSERT_LT(a, b)                                                                 \
    do {                                                                                        \
        if (!((a) < (b))) {                                                                     \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s < %s (%s:%d) in %s\n", #a, #b, \
                       __FILE__, __LINE__, __func__);                                           \
            return SN_TEST_FAIL;                                                                \
        }                                                                                       \
    } while (0)

#define SN_TEST_ASSERT_LE(a, b)                                                                  \
    do {                                                                                         \
        if (!((a) <= (b))) {                                                                     \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s <= %s (%s:%d) in %s\n", #a, #b, \
                       __FILE__, __LINE__, __func__);                                            \
            return SN_TEST_FAIL;                                                                 \
        }                                                                                        \
    } while (0)

#define SN_TEST_ASSERT_GT(a, b)                                                                 \
    do {                                                                                        \
        if (!((a) > (b))) {                                                                     \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s > %s (%s:%d) in %s\n", #a, #b, \
                       __FILE__, __LINE__, __func__);                                           \
            return SN_TEST_FAIL;                                                                \
        }                                                                                       \
    } while (0)

#define SN_TEST_ASSERT_GE(a, b)                                                                  \
    do {                                                                                         \
        if (!((a) >= (b))) {                                                                     \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s >= %s (%s:%d) in %s\n", #a, #b, \
                       __FILE__, __LINE__, __func__);                                            \
            return SN_TEST_FAIL;                                                                 \
        }                                                                                        \
    } while (0)

#define SN_TEST_ASSERT_NULL(x)                                                                    \
    do {                                                                                          \
        if ((x) != NULL) {                                                                        \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s to be NULL (%s:%d) in %s\n", #x, \
                       __FILE__, __LINE__, __func__);                                             \
            return SN_TEST_FAIL;                                                                  \
        }                                                                                         \
    } while (0)

#define SN_TEST_ASSERT_NOT_NULL(x)                                                                \
    do {                                                                                          \
        if ((x) == NULL) {                                                                        \
            log_msg_fg(COLOR_RED, "Assertion failed: expected %s to be non-NULL (%s:%d) in %s\n", \
                       #x, __FILE__, __LINE__, __func__);                                         \
            return SN_TEST_FAIL;                                                                  \
        }                                                                                         \
    } while (0)

#define SN_TEST_ASSERT_PTR_EQ(a, b)                                                               \
    do {                                                                                          \
        if ((void *)(a) != (void *)(b)) {                                                         \
            log_msg_fg(COLOR_RED, "Assertion failed: expected pointers %s == %s (%s:%d) in %s\n", \
                       #a, #b, __FILE__, __LINE__, __func__);                                     \
            return SN_TEST_FAIL;                                                                  \
        }                                                                                         \
    } while (0)

#define SN_TEST_ASSERT_PTR_NEQ(a, b)                                                              \
    do {                                                                                          \
        if ((void *)(a) == (void *)(b)) {                                                         \
            log_msg_fg(COLOR_RED, "Assertion failed: expected pointers %s != %s (%s:%d) in %s\n", \
                       #a, #b, __FILE__, __LINE__, __func__);                                     \
            return SN_TEST_FAIL;                                                                  \
        }                                                                                         \
    } while (0)

#define SN_TEST_ASSERT_STR_EQ(a, b)                                                                             \
    do {                                                                                                        \
        const char *sn_test_str_a = (a);                                                                        \
        const char *sn_test_str_b = (b);                                                                        \
        bool sn_test_str_equal                                                                                  \
            = (sn_test_str_a == NULL && sn_test_str_b == NULL)                                                  \
              || (sn_test_str_a != NULL && sn_test_str_b != NULL && strcmp(sn_test_str_a, sn_test_str_b) == 0); \
        if (!sn_test_str_equal) {                                                                               \
            log_msg_fg(COLOR_RED,                                                                               \
                       "Assertion failed: expected strings %s == %s (%s:%d) in %s\n"                            \
                       "  left : \"%s\"\n"                                                                      \
                       "  right: \"%s\"\n",                                                                     \
                       #a, #b, __FILE__, __LINE__, __func__, sn_test_str_a ? sn_test_str_a : "(null)",          \
                       sn_test_str_b ? sn_test_str_b : "(null)");                                               \
            return SN_TEST_FAIL;                                                                                \
        }                                                                                                       \
    } while (0)

#define SN_TEST_ASSERT_STR_NEQ(a, b)                                                                            \
    do {                                                                                                        \
        const char *sn_test_str_a = (a);                                                                        \
        const char *sn_test_str_b = (b);                                                                        \
        bool sn_test_str_equal                                                                                  \
            = (sn_test_str_a == NULL && sn_test_str_b == NULL)                                                  \
              || (sn_test_str_a != NULL && sn_test_str_b != NULL && strcmp(sn_test_str_a, sn_test_str_b) == 0); \
        if (sn_test_str_equal) {                                                                                \
            log_msg_fg(COLOR_RED,                                                                               \
                       "Assertion failed: expected strings %s != %s (%s:%d) in %s\n"                            \
                       "  both: \"%s\"\n",                                                                      \
                       #a, #b, __FILE__, __LINE__, __func__, sn_test_str_a ? sn_test_str_a : "(null)");         \
            return SN_TEST_FAIL;                                                                                \
        }                                                                                                       \
    } while (0)

#define SN_TEST_ASSERT_STR_N_EQ(a, b, n)                                                            \
    do {                                                                                            \
        const char *sn_test_str_a = (a);                                                            \
        const char *sn_test_str_b = (b);                                                            \
        size_t sn_test_str_n = (n);                                                                 \
        bool sn_test_str_equal = (sn_test_str_a == NULL && sn_test_str_b == NULL)                   \
                                 || (sn_test_str_a != NULL && sn_test_str_b != NULL                 \
                                     && strncmp(sn_test_str_a, sn_test_str_b, sn_test_str_n) == 0); \
        if (!sn_test_str_equal) {                                                                   \
            log_msg_fg(COLOR_RED,                                                                   \
                       "Assertion failed: first %zu chars of %s == %s (%s:%d) in "                  \
                       "%s\n",                                                                      \
                       sn_test_str_n, #a, #b, __FILE__, __LINE__, __func__);                        \
            return SN_TEST_FAIL;                                                                    \
        }                                                                                           \
    } while (0)

#define SN_TEST_ASSERT_CHAR_EQ(a, b)                                                         \
    do {                                                                                     \
        char sn_test_char_a = (a);                                                           \
        char sn_test_char_b = (b);                                                           \
        if (sn_test_char_a != sn_test_char_b) {                                              \
            log_msg_fg(COLOR_RED,                                                            \
                       "Assertion failed: expected chars %s == %s (%s:%d) in %s\n"           \
                       "  left : '%c' (%d)\n"                                                \
                       "  right: '%c' (%d)\n",                                               \
                       #a, #b, __FILE__, __LINE__, __func__, sn_test_char_a, sn_test_char_a, \
                       sn_test_char_b, sn_test_char_b);                                      \
            return SN_TEST_FAIL;                                                             \
        }                                                                                    \
    } while (0)

#define SN_TEST_ASSERT_CHAR_NEQ(a, b)                                                         \
    do {                                                                                      \
        char sn_test_char_a = (a);                                                            \
        char sn_test_char_b = (b);                                                            \
        if (sn_test_char_a == sn_test_char_b) {                                               \
            log_msg_fg(COLOR_RED,                                                             \
                       "Assertion failed: expected chars %s != %s (%s:%d) in %s\n"            \
                       "  both: '%c' (%d)\n",                                                 \
                       #a, #b, __FILE__, __LINE__, __func__, sn_test_char_a, sn_test_char_a); \
            return SN_TEST_FAIL;                                                              \
        }                                                                                     \
    } while (0)

#define SN_TEST_ASSERT_MEM_EQ(a, b, size)                                            \
    do {                                                                             \
        const void *sn_test_mem_a = (a);                                             \
        const void *sn_test_mem_b = (b);                                             \
        size_t sn_test_mem_size = (size);                                            \
        if (memcmp(sn_test_mem_a, sn_test_mem_b, sn_test_mem_size) != 0) {           \
            log_msg_fg(COLOR_RED,                                                    \
                       "Assertion failed: memory %s == %s with size %zu (%s:%d) in " \
                       "%s\n",                                                       \
                       #a, #b, sn_test_mem_size, __FILE__, __LINE__, __func__);      \
            return SN_TEST_FAIL;                                                     \
        }                                                                            \
    } while (0)

#define SN_TEST_ASSERT_MEM_NEQ(a, b, size)                                           \
    do {                                                                             \
        const void *sn_test_mem_a = (a);                                             \
        const void *sn_test_mem_b = (b);                                             \
        size_t sn_test_mem_size = (size);                                            \
        if (memcmp(sn_test_mem_a, sn_test_mem_b, sn_test_mem_size) == 0) {           \
            log_msg_fg(COLOR_RED,                                                    \
                       "Assertion failed: memory %s != %s with size %zu (%s:%d) in " \
                       "%s\n",                                                       \
                       #a, #b, sn_test_mem_size, __FILE__, __LINE__, __func__);      \
            return SN_TEST_FAIL;                                                     \
        }                                                                            \
    } while (0)
