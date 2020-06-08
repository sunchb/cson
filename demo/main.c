
#ifdef _CSON_MULTI_ARRAY_SUPPORT_
extern void test3();
extern void test5();
extern void test6();
#else
extern void test1();
extern void test2();
#endif

#include "cson.h"
#include "stdio.h"

int main()
{
#ifdef _CSON_MULTI_ARRAY_SUPPORT_
    test3();
    test5();
    test6();
#else
    test1();
    test2();
#endif
    return 0;
}
