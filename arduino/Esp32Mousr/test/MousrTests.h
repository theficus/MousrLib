void test_ParseMessage();
void test_mousrAlloc();
void test_getRawData();
void test_convertToBytes();
void test_createFromRaw();
void test_connectionStatusMap();
void test_messageCtor();

#define runMousrTests()  \
    test_ParseMessage(); \
    test_mousrAlloc(); \
    test_getRawData(); \
    test_convertToBytes(); \
    test_createFromRaw(); \
    test_connectionStatusMap(); \
    test_messageCtor();