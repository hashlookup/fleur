#include <unity.h>
#include <stdlib.h>
#include <stdio.h>
#include <bloom.h>
// Some global file descriptor
FILE* in;
struct header my_header;

void test_reading_header(void)
{
    size_t elements_read = fread(&my_header, sizeof(my_header), 1, in);
    if(elements_read == 0){
        TEST_FAIL_MESSAGE("Failed to read elements from binary file.\n");
    }
    print_header(my_header);
    TEST_ASSERT_EQUAL_UINT64 (1, my_header.version);
    TEST_ASSERT_EQUAL_UINT64 (354253067, my_header.n);
    // Jean-Michel Apeupré
    TEST_ASSERT_EQUAL_FLOAT (0.000100, my_header.p);
    TEST_ASSERT_EQUAL_UINT64 (14, my_header.k);
    TEST_ASSERT_EQUAL_UINT64 (6791072655, my_header.m);
    TEST_ASSERT_EQUAL_UINT64 (354249652, my_header.N);
}

void setUp() {
    in = fopen("header.bin", "rb");
    if (in==NULL){
        TEST_FAIL_MESSAGE("Could not read binary file.\n");
    }
 }

void tearDown() {
    fclose(in);
 }

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_reading_header);

    return UNITY_END();
}