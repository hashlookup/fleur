#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct header {
    uint64_t version;
	//desired maximum number of elements
    uint64_t n;
	//desired false positive probability
    double p;
	//number of hash functions
    uint64_t k;
	//number of bits
    uint64_t m;
	//number of elements
    uint64_t N;
} header;

typedef struct BloomFilter {
    // version, needed for serialization 
    uint64_t version;
    // datasize, needed for serialization 
    uint64_t datasize;

    // bloom filter file header
    header *h;

    //number of 64-bit integers (generated automatically)
    uint64_t M;

	// bit array - dynamic
    uint64_t *v;

	// arbitrary data that we can attach to the filter - dynamic
	unsigned char *Data;

    // has the bloom filter been modified?
    int modified;

} BloomFilter;

static const uint64_t m = 18446744073709551557LLU;
static const uint64_t g = 18446744073709550147LLU;

int Add(BloomFilter * bf, char *buf, size_t buf_size);
void SetData(BloomFilter * bf, char* buf, size_t buf_size );
int Check(BloomFilter * bf, char *buf, size_t buf_size);
struct BloomFilter * Initialize(uint64_t n, double p);
struct BloomFilter * BloomFilterFromFile(FILE* f);
void BloomFilterToFile(BloomFilter * bf, FILE* of);
void Fingerprint(BloomFilter * bf, char *buf, size_t buf_size, uint64_t **fingerprint);
void print_header(header h);
void print_filter(BloomFilter * bf);