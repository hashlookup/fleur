#include <unity.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <bloom.h>
// Some global file descriptor
FILE* inheader;
FILE* infull;
struct header my_header;


void test_reading_header(void)
{
    size_t elements_read = fread(&my_header, sizeof(my_header), 1, inheader);
    if(elements_read == 0){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    print_header(my_header);
    TEST_ASSERT_EQUAL_UINT64 (1, my_header.version);
    TEST_ASSERT_EQUAL_UINT64 (354253067, my_header.n);
    // Jean-Michel Apeupré
    TEST_ASSERT_EQUAL_FLOAT (0.0001, my_header.p);
    TEST_ASSERT_EQUAL_UINT64 (14, my_header.k);
    TEST_ASSERT_EQUAL_UINT64 (6791072655, my_header.m);
    TEST_ASSERT_EQUAL_UINT64 (354249652, my_header.N);
}

void test_reading_full(void)
{
    // Get header
    size_t elements_read = fread(&my_header, sizeof(my_header), 1, infull);
    if(elements_read == 0){
        TEST_FAIL_MESSAGE(strerror(errno));
    }

    TEST_ASSERT_EQUAL_UINT64 (1, my_header.version);
    TEST_ASSERT_EQUAL_UINT64 (1000, my_header.n);
    // Jean-Michel Apeupré
    TEST_ASSERT_EQUAL_FLOAT (0.000001, my_header.p);
    TEST_ASSERT_EQUAL_UINT64 (20, my_header.k);
    TEST_ASSERT_EQUAL_UINT64 (28755, my_header.m);
    TEST_ASSERT_EQUAL_UINT64 (3, my_header.N);

    struct BloomFilter * my_bloom;
    my_bloom = BloomFilterFromFile(&my_header, infull);

    TEST_ASSERT_EQUAL_UINT64 (450, my_bloom->M);
    TEST_ASSERT_EQUAL_STRING("toto\n", my_bloom->Data);
    print_filter(my_bloom);
}

void test_fingerprint(void){
    // uint64_t = Fingerprint();
    // TEST_FAIL_MESSAGE("FAILURE ATM");
}

void setUp() {
    inheader = fopen("header.bin", "rb");
    if (inheader==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    infull = fopen("datatest.bloom", "rb");
    if (infull==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
 }

void tearDown() {
    fclose(inheader);
    fclose(infull);
 }

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_reading_header);
    RUN_TEST(test_reading_full);
    // RUN_TEST(test_fingerprint);

    return UNITY_END();
}