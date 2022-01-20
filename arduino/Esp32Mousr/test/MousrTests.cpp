#include <unity.h>

#include "Log.h"
#include "Mousr.h"

void test_mousrAlloc()
{
    s_writeLogLn("test_mousrAlloc()");
    string data = "0x307b3c0b3fce824a3ebd45933f00030000000000";
    MousrData d(data);
    d = MousrData(data);
    d = MousrData(data);
    d = MousrData(data);
}

void test_ParseMessage()
{
    // ROBOT_POSE message
    {
        string data = "0x307b3c0b3fce824a3ebd45933f00030000000000";
        MousrData d(data);
        TEST_ASSERT_EQUAL(MousrMessage::ROBOT_POSE, d.getMessageKind());
        TEST_ASSERT_EQUAL(20, d.getMessageLength());

        auto cooked = d.getMessageData();
        TEST_ASSERT_EQUAL(MousrMessage::ROBOT_POSE, cooked->msg);
        TEST_ASSERT_EQUAL_FLOAT(1.150566, cooked->movement.angle);
        TEST_ASSERT_EQUAL_FLOAT(0.197765, cooked->movement.held);
        TEST_ASSERT_EQUAL_FLOAT(0.543892, cooked->movement.speed);
    }

    {
        // BATTERY_VOLTAGE
        string data = "0x625c000000002015000000000000000000000000";
        MousrData d = MousrData(data);
        TEST_ASSERT_EQUAL(MousrMessage::BATTERY_VOLTAGE, d.getMessageKind());
        TEST_ASSERT_EQUAL(20, d.getMessageLength());
        
        auto cooked = d.getMessageData();
        TEST_ASSERT_EQUAL(MousrMessage::BATTERY_VOLTAGE, cooked->msg);
        TEST_ASSERT_EQUAL(92, cooked->battery.voltage);
    }
}

void test_toHexString()
{
    string data = "0x307b3c0b3fce824a3ebd45933f00030000000000";
    MousrData d(data);
    string actual = d.toString();
    TEST_ASSERT_EQUAL_STRING(data.c_str(), actual.c_str());
}