#include <unity.h>
#include <stdlib.h>
#include <string.h>
#include <fnv.h>
#include <math.h>


void test_fnv(void){
   char output[17];
   char* str1 =  "test";

   uint64_t h = fnv1(str1, strlen(str1));
   fnvhash test;
   test.h = h;
   getDigest(&test, output);
   printf("%s\n", output);
   TEST_ASSERT_EQUAL_STRING("8c093f7e9fccbf69", output);
}

void setUp() { }

void tearDown() { }

int main(void)
{

    UNITY_BEGIN();

    RUN_TEST(test_fnv);

    return UNITY_END();
}