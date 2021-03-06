/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Automagically Detected Files To Include=====*/
#include "unity.h"
#include "tests.h"

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test_writeUChar();
extern void test_binaryLog();
extern void test_connectionStatusMap();
extern void test_convertToBytes();
extern void test_mousrAlloc();
extern void test_getRawData();
extern void test_createFromRaw();
extern void test_ParseMessage();
extern void test_messageCtor();


/*=======Mock Management=====*/
static void CMock_Init(void)
{
}
static void CMock_Verify(void)
{
}
static void CMock_Destroy(void)
{
}

/*=======Setup (stub)=====*/
void setUp(void) {}

/*=======Teardown (stub)=====*/
void tearDown(void) {}

/*=======Test Reset Options=====*/
void resetTest(void);
void resetTest(void)
{
  tearDown();
  CMock_Verify();
  CMock_Destroy();
  CMock_Init();
  setUp();
}
void verifyTest(void);
void verifyTest(void)
{
  CMock_Verify();
}

/*=======Test Runner Used To Run Each Test=====*/
static void run_test(UnityTestFunction func, const char* name, UNITY_LINE_TYPE line_num)
{
    Unity.CurrentTestName = name;
    Unity.CurrentTestLineNumber = line_num;
#ifdef UNITY_USE_COMMAND_LINE_ARGS
    if (!UnityTestMatches())
        return;
#endif
    Unity.NumberOfTests++;
    UNITY_CLR_DETAILS();
    UNITY_EXEC_TIME_START();
    CMock_Init();
    if (TEST_PROTECT())
    {
        setUp();
        func();
    }
    if (TEST_PROTECT())
    {
        tearDown();
        CMock_Verify();
    }
    CMock_Destroy();
    UNITY_EXEC_TIME_STOP();
    UnityConcludeTest();
}

/*=======MAIN=====*/
int main(void)
{
  UnityBegin("tests.cpp");
  run_test(test_writeUChar, "test_writeUChar", 5);
  run_test(test_binaryLog, "test_binaryLog", 28);
  run_test(test_connectionStatusMap, "test_connectionStatusMap", 38);
  run_test(test_convertToBytes, "test_convertToBytes", 56);
  run_test(test_mousrAlloc, "test_mousrAlloc", 98);
  run_test(test_getRawData, "test_getRawData", 107);
  run_test(test_createFromRaw, "test_createFromRaw", 121);
  run_test(test_ParseMessage, "test_ParseMessage", 142);
  run_test(test_messageCtor, "test_messageCtor", 183);

  return UnityEnd();
}
