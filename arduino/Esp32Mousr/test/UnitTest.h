#include <unity.h>

class UnitTest
{
    public:
    virtual void Setup() {}
    virtual void Teardown() {}
    virtual void RunAll();
};