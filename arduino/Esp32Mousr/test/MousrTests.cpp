#include <unity.h>

#include "Mousr.h"

void test_ParseMessage()
{
    // 0x305d4e0e3f464db53e85b6923f00030000000000
    MousrData d("0x305d4e0e3f464db53e85b6923f00030000000000");
    TEST_ASSERT_EQUAL(MousrMessage::ROBOT_POSE, d.getMessageKind());
}