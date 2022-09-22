#include <unity.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fleur.h>
#include <time.h>
// Some global file descriptor
FILE* inheader;
FILE* hang0;
FILE* hang1;
FILE* hang2;
FILE* infull;
FILE* join1;
FILE* join2;
FILE* join3;
struct header my_header;

typedef struct tester {
    BloomFilter *bf;
    char *buf[];
}tester;

char* GenerateTestValue(uint64_t length){
    unsigned char* buf = calloc(length, sizeof(char));
    for (uint64_t i = 0; i < length; i++){
		buf[i] = rand() % 256;
    }

	return (char*)buf;
}

tester * GenerateExampleFilter(BloomFilter *bf, uint64_t capacity, double p, uint64_t samples) {
    // seeding the prng
    srand((unsigned) time(0));
    (*bf) = fleur_initialize(capacity, p, "");
    struct tester *test = malloc(sizeof(BloomFilter*) + (samples * sizeof(char*)));
    test->bf = bf;
    unsigned char* str = "foobar";
	test->bf->Data = str; 
    test->bf->datasize = strlen(str);
	for (uint64_t i = 0; i < samples; i++) {
        test->buf[i] = GenerateTestValue(100);
        // printf("Adding %ld:%s\n", i, test->buf[i]);
        fleur_add(test->bf, test->buf[i], 100);
	}
	return test;
}

void test_reading_header(void)
{
    header my_header;
    size_t elements_read = fread(&my_header, sizeof(my_header), 1, inheader);
    if(elements_read == 0){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    fleur_print_header(&my_header);
    TEST_ASSERT_EQUAL_UINT64 (1, my_header.version);
    TEST_ASSERT_EQUAL_UINT64 (354253067, my_header.n);
    // Jean-Michel Apeupré
    TEST_ASSERT_EQUAL_FLOAT (0.0001, my_header.p);
    TEST_ASSERT_EQUAL_UINT64 (14, my_header.k);
    TEST_ASSERT_EQUAL_UINT64 (6791072655, my_header.m);
    TEST_ASSERT_EQUAL_UINT64 (354249652, my_header.N);
    TEST_ASSERT_EQUAL_INT (1, fleur_check_header(&my_header));

    // hang0 p too small
    elements_read = fread(&my_header, sizeof(my_header), 1, hang0);
    if(elements_read == 0){
        TEST_FAIL_MESSAGE(strerror(errno));
    }else{
        TEST_ASSERT_EQUAL_INT (0, fleur_check_header(&my_header));
    }
    fleur_print_header(&my_header);

    // hang1 bad header version
    elements_read = fread(&my_header, sizeof(my_header), 1, hang1);
    if(elements_read == 0){
        TEST_FAIL_MESSAGE(strerror(errno));
    }else{
        TEST_ASSERT_EQUAL_INT (0, fleur_check_header(&my_header));
    }
    fleur_print_header(&my_header);
}

void test_reading_full(void)
{
    BloomFilter bf = fleur_bloom_filter_from_file(infull);
    TEST_ASSERT_EQUAL_INT(0, bf.error);
    TEST_ASSERT_EQUAL_UINT64(450, bf.M);
    fleur_print_filter(&bf);
    fleur_destroy_filter(&bf);

    // hang2 p above 1
    BloomFilter bf1 = fleur_bloom_filter_from_file(hang2);
    TEST_ASSERT_EQUAL_INT (1, bf1.error);
    fleur_destroy_filter(&bf1);
}

void test_writing(void){
    const char* path = "./writing-test.bloom";
    int ret = 1;
    FILE* f = fopen(path, "wb");
    BloomFilter bf;
    struct tester *test = GenerateExampleFilter(&bf, 1000, 0.001, 100);
    ret = fleur_bloom_filter_to_file(test->bf, f);
    if (ret != 1) {
		TEST_FAIL_MESSAGE("Error writing filter to file.");
    }
    fleur_destroy_filter(&bf);
    fclose(f);
}

void test_fingerprint(void){
    char str[80];
    BloomFilter bf = fleur_initialize(100000, 0.01, "");
    uint64_t* fp = calloc(7, sizeof(uint64_t));
	uint64_t expected[7] = {20311, 36825, 412501, 835777, 658914, 853361, 307361};
	fleur_fingerprint(&bf, "bar", strlen("bar"), &fp);
    for (uint64_t i = 0; i < bf.h.k ; i++ ){
		if (fp[i] != expected[i]) {
            sprintf(str, "Wrong fingerprint: %ld vs. %ld", fp[i], expected[i]);
		    TEST_FAIL_MESSAGE(str);
			break;
		}
	}
    fleur_destroy_filter(&bf);
    free(fp);
}

void test_initialize(void){
    BloomFilter bf = fleur_initialize(10000, 0.001, "");
    BloomFilter bf1 = fleur_initialize(100, 0.00001, "toto");
    BloomFilter bf2 = fleur_initialize(9000, 0.01, "titi");

    char str[80];
	if (bf.h.k != 10) {
		TEST_FAIL_MESSAGE("k does not match expectation!\n");
        (strerror(errno));
	}
	if (bf.h.m != 143775 ){
        sprintf(str, "m does not match expectation: %lu\n", bf.h.m);
		TEST_FAIL_MESSAGE(str);
	}
	if (bf.M != (uint64_t)ceil((double)bf.h.m/64)){
        sprintf(str, "M does not match expectation: %lu\n", bf.M);
		TEST_FAIL_MESSAGE(str);
	}
	for (uint64_t i = 0 ; i < bf.M; i++) {
		if (bf.v[i] != 0) {
			TEST_FAIL_MESSAGE("Filter value is not initialized to zero!\n");
		}
	}

    fleur_print_filter(&bf);
    fleur_print_filter(&bf1);
    fleur_print_filter(&bf2);
    fleur_destroy_filter(&bf);
    fleur_destroy_filter(&bf1);
    fleur_destroy_filter(&bf2);
}

//This tests the checking of values against a given filter
void test_checking(void) {
	uint64_t capacity = 100000;
	double p = 0.001;
	uint64_t samples = 100000;
    BloomFilter bf;
    struct tester *test = GenerateExampleFilter(&bf, capacity, p, samples);
    for (uint64_t i = 0; i < samples; i ++){
        if (fleur_check(test->bf, test->buf[i], 100) == 0) {
            TEST_FAIL_MESSAGE("Did not find test value in filter!");
		}
    }
    char* str = "this is not in the filter";
    if (fleur_check(test->bf, str, strlen(str)) == 1) {
        TEST_FAIL_MESSAGE("This value is not in the filter!");
    }
    for (int i = 0; i < samples; i++){
        free(test->buf[i]);
    }
    fleur_destroy_filter(test->bf);
    free(test);
}

// This tests joining two filters into one
void test_joining(void) {
    BloomFilter j1 = fleur_bloom_filter_from_file(join1);
    BloomFilter j0 = fleur_bloom_filter_from_file(infull);
    BloomFilter j2 = fleur_bloom_filter_from_file(join2);
    BloomFilter j3 = fleur_bloom_filter_from_file(join3);
    TEST_ASSERT_EQUAL_INT (-1, fleur_join(&j2, &j1));
    TEST_ASSERT_EQUAL_INT (0, fleur_join(&j0, &j1));
    TEST_ASSERT_EQUAL_INT (1, fleur_join(&j3, &j1));
    fleur_destroy_filter(&j0);
    fleur_destroy_filter(&j1);
    fleur_destroy_filter(&j2);
}

void setUp() {
    inheader = fopen("header.bin", "rb");
    if (inheader==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    hang0 = fopen("hang0.bin", "rb");
    if (hang0==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    hang1 = fopen("hang1.bin", "rb");
    if (hang1==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    hang2 = fopen("hang2.bin", "rb");
    if (hang2==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    infull = fopen("datatest.bloom", "rb");
    if (infull==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    join1 = fopen("join1.bloom", "rb");
    if (join1==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    join2 = fopen("join2.bloom", "rb");
    if (join2==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
    join3 = fopen("join3.bloom", "rb");
    if (join3==NULL){
        TEST_FAIL_MESSAGE(strerror(errno));
    }
 }

void tearDown() {
    fclose(inheader);
    fclose(infull);
    fclose(hang0);
    fclose(hang1);
    fclose(hang2);
    fclose(join1);
    fclose(join2);
    fclose(join3);
 }

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_reading_header);
    RUN_TEST(test_reading_full);
    RUN_TEST(test_initialize);
    RUN_TEST(test_fingerprint);
    RUN_TEST(test_checking);
    RUN_TEST(test_writing);
    RUN_TEST(test_joining);

    return UNITY_END();
}