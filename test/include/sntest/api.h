#pragma once

#include <sncore/api_common.h>

#if defined(SN_TEST_STATIC)
    #define SN_TEST_API
#elif defined(SN_EXPORT)
    #define SN_TEST_API SN_API_HELPER_EXPORT
#else
    #define SN_TEST_API SN_API_HELPER_IMPORT
#endif
