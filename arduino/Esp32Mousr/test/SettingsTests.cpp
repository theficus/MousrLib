#include <cstdint>
#include "unity.h"
#include "Settings.h"

Settings s;

void test_writeUChar()
{
    // Speed
    uint8_t expected = 22;
    s.setMaxSpeed(expected);
    uint8_t actual = s.getMaxSpeed();
    TEST_ASSERT_EQUAL(expected, actual);
    s.setMaxSpeed(0xff); // Make sure we change to 100
    expected = 100;
    actual = s.getMaxSpeed();
    TEST_ASSERT_EQUAL(expected, actual);

    // Volume
    expected = 66;
    s.setVolume(expected);
    actual = s.getVolume();
    TEST_ASSERT_EQUAL(expected, actual);
    s.setVolume(0xff);
    expected = 100;
    actual = s.getVolume();
    TEST_ASSERT_EQUAL(expected, actual);
}