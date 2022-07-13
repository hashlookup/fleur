#include <unity.h>
#include <stdlib.h>
#include <fnv.h>


// Some global var for fnv test
FNV164_CTX* context;

void test_fnv(void)
{

    unsigned char digest[8];
    FNV164Init(context);
    const char* str1 =  "tête de lard ivrogne. Bandit macrocéphale bibendum, pyrophore écornifleur,scélérat coquin pantoufle porc-épic mal embouché soulographe. Va-nu-pieds";

    FNV164Update(context, (const unsigned char*)str1, strlen(str1));
    FNV164Final(digest, context);
    char output[40];
    // sprintf(output, "%02x",(unsigned int) *digest);
    sprintf(output, "%02x%02x%02x%02x%02x%02x%02x%02x",
        (unsigned int) digest[0],
        (unsigned int) digest[1],
        (unsigned int) digest[2],
        (unsigned int) digest[3],
        (unsigned int) digest[4],
        (unsigned int) digest[5],
        (unsigned int) digest[6],
        (unsigned int) digest[7]
        );

    TEST_ASSERT_EQUAL_STRING("bc06a2bbebfbdd53", output);
}

void setUp() {
    context = FNV164_CTX_create();
 }

void tearDown() {
    FNV164_CTX_delete(context);
 }

int main(void)
{

    UNITY_BEGIN();

    RUN_TEST(test_fnv);

    return UNITY_END();
}