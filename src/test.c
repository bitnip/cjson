#include "cutil/src/assertion.h"

int asserts_passed = 0;
int asserts_failed = 0;

void testJSONLexer();
void testJSONParser();
void testJSONUnparser();

int main() {
    testJSONLexer();
    testJSONParser();
    testJSONUnparser();

    printf("Asserts Passed: %d, Failed: %d\n",
        asserts_passed, asserts_failed);
    return asserts_failed;
}
