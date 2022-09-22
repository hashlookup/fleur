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
    header h;

    //number of 64-bit integers (generated automatically)
    uint64_t M;

	// bit array - on the heap
    uint64_t *v;

	// arbitrary data that we can attach to the filter - on the stack
	unsigned char *Data;

    // has the bloom filter been modified?
    int modified;
    // error on instanciation
    int error;

} BloomFilter;

static const uint64_t m = 18446744073709551557LLU;
static const uint64_t g = 18446744073709550147LLU;

struct BloomFilter fleur_initialize(uint64_t n, double p, char *buf);
struct BloomFilter fleur_bloom_filter_from_file(FILE* f);

int fleur_add(BloomFilter * bf, char *buf, size_t buf_size);
int fleur_check(BloomFilter * bf, char *buf, size_t buf_size);
int fleur_join(BloomFilter * src, BloomFilter* dst);
int fleur_bloom_filter_to_file(BloomFilter * bf, FILE* of);
void fleur_set_data(BloomFilter * bf, char* buf, size_t buf_size );
void fleur_fingerprint(BloomFilter * bf, char *buf, size_t buf_size, uint64_t **fingerprint);

void fleur_destroy_filter(BloomFilter * bf);

void fleur_print_header(header * h);
void fleur_print_filter(BloomFilter * bf);
int fleur_check_header(header * h);