#include "common.h"
#include "utility.h"
#include "UtilityTests.h"

void test_binaryLog()
{
    s_printf("Binary=" BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(UINT32_MAX));
    s_printf("Binary=" BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(INT32_MAX));
}