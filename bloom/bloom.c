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
uint64_t * Fingerprint(uint64_t value[], uint64_t fingerprint[], BloomFilter * filter) {
    fingerprint = calloc(filter->k, sizeof(uint64_t));
  
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