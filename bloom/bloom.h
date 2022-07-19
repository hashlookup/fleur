#include <stdint.h>

typedef struct BloomFilter {
	//desired maximum number of elements
    uint64_t n;

	//desired false positive probability
    double p;

	//number of hash functions
    uint64_t k;

	//number of bits
    uint64_t m;

	//number of elements in the filter
    uint64_t N;

	//number of 64-bit integers (generated automatically)
    uint64_t M;

	//bit array - dynamic
    uint64_t *v;

	//arbitrary data that we can attach to the filter - dynamic
	unsigned char *Data;
}BloomFilter;

typedef struct header{
    uint64_t version;
    uint64_t n;
    double p;
    uint64_t k;
    uint64_t m;
    uint64_t N;
}header;

static const uint64_t m = 18446744073709551557LLU;
static const uint64_t g = 18446744073709550147LLU;

struct BloomFilter * BloomFilterFromFile(header * h, FILE* f);
struct BloomFilter * Read();
uint64_t * Fingerprint(uint64_t value[], uint64_t fingerprint[], BloomFilter * filter);
void print_header(header h);
void print_filter(BloomFilter * b);