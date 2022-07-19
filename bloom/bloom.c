#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fnv.h>
#include "bloom.h"


// BloomFilterFromFile return a pointer to a BloomFilter from a 
// bloom filter header and a file descriptor.
struct BloomFilter * BloomFilterFromFile(struct header * h, FILE* f){
    static struct BloomFilter my_bloom;
    my_bloom.n = h->n;
    my_bloom.p = h->p;
    my_bloom.k = h->k;
    my_bloom.N = h->N;
    my_bloom.m = h->m;
    my_bloom.M =  ceil(my_bloom.m / 64.0);

    // Get data size
    int err = fseek(f, 0, SEEK_END);
    if(err!=0){
        perror("Cannot seek in binary file.");
    }
    long size = ftell(f);
    fseek(f, 48, SEEK_SET);

    long datasize = size - ceil((my_bloom.M*64)/8) - 48;

    // Load bitarray
    my_bloom.v = calloc(my_bloom.M, sizeof(uint64_t));
    long elements_read = fread(my_bloom.v, sizeof(uint64_t), my_bloom.M, f);
    if(elements_read == 0){
        perror("Cannot load bitarray.");
    }

    // Load remaining data
    if (datasize > 0) {
        my_bloom.Data = calloc(datasize, sizeof(unsigned char));
        elements_read = fread(my_bloom.Data, sizeof(unsigned char), datasize, f);
        if(elements_read == 0){
            perror("Cannot load bloom filter metadata.");
        }
    }
    
    return &my_bloom;
}

// Fingerprint returns the fingerprint of a given value, as an array of index
// values.
void Fingerprint(char *buf, size_t buf_size,  uint64_t **fingerprint, BloomFilter * filter) {
    uint64_t* tmp = calloc(filter->k, sizeof(uint64_t));
    uint64_t h = fnv1(buf, buf_size);
    uint64_t hn = h % m;
    for (uint64_t i = 0; i < filter->k; i++){
		hn = (hn * g) % m;
		tmp[i] = (uint64_t)hn % filter->m;
    }

    free(*fingerprint);
    *fingerprint = tmp;
}

// Initialize returns a new, empty Bloom filter with the given capacity (n)
// and FP probability (p).
struct BloomFilter * Initialize(uint64_t n, double p){
    static struct BloomFilter bf;
	bf.m = abs(ceil((double)(n) * log(p) / pow(log(2.0), 2.0)));
	bf.n = n;
	bf.p = p;
	bf.M = ceil(bf.m / 64.0);
	bf.k = ceil(log(2) * bf.m / bf.n );
    bf.v = calloc(bf.M, sizeof(uint64_t));
	return &bf;
}

void print_header(header h){
     printf("Header details:\n version: %lu\n n: %lu \n p: %f\n k: %lu \n m: %lu \n N: %lu \n",
    h.version,
    h.n,
    h.p,
    h.k,
    h.m,
    h.N);
}

void print_filter(BloomFilter * b){
     printf("Filter details:\n n: %lu \n p: %f\n k: %lu \n m: %lu \n N: %lu \n M: %lu\n Data: %s.",
    b->n,
    b->p,
    b->k,
    b->m,
    b->N,
    b->M,
    b->Data);
}