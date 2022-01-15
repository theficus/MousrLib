#include <Log.h>
#include <Log.cpp>
#include "UnitTest.h"

void test_writeLogLn();
void test_writeLogF();
void test_writeLog();
    
class LogTests : UnitTest
{
    public:
    void RunAll()
    {
        RUN_TEST(test_writeLogLn);
        RUN_TEST(test_writeLogF);
        RUN_TEST(test_writeLog);
    }
};