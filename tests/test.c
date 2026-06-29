#include <sntest/sntest.h>
#include <stdio.h>

bool sn_test_init(SnTestConfig *config) {
    SN_UNUSED(config);
    return true;
}

void sn_test_deinit(void) {
}

SN_TEST_ADD(print_test) {
    printf("inside test!\n");
    return SN_TEST_PASS;
}

SN_TEST_RUN()
