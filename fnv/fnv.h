#include <stdint.h>

// Same values as in flor / bloom
#define FNV_PRIME ((uint64_t)1099511628211)
#define FNV_OFFSET ((uint64_t)14695981039346656037UL)

typedef union {
    uint64_t h;
    unsigned char hexrepr[8];
}fnvhash;

uint64_t fnv1(char *buf, size_t buf_size);
void getDigest(fnvhash * fh, char * str);