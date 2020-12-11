#include "CuTest.h"

#include "string.h"
#include "ctype.h"

char* StrToUpper(char* str) {
        char* p;
        for (p = str ; *p ; ++p)
                *p = toupper(*p);
        return str;
}

void TestStrToUpper(CuTest *tc) {
        char* input = strdup("hello world");
        char* actual = StrToUpper(input);
        char* expected = "HELLO WORLD";
        CuAssertStrEquals(tc, expected, actual);
}

CuSuite* StrUtilGetSuite() {
        CuSuite* suite = CuSuiteNew();
        SUITE_ADD_TEST(suite, TestStrToUpper);
        return suite;
}
