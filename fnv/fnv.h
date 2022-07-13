#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define FNV1_64_INIT ((uint64_t)0xcbf29ce484222325ULL)
#define FNV1A_64_INIT FNV1_64_INIT
#define FNV_64_PRIME ((uint64_t)0x100000001b3ULL)

typedef struct FNV164_CTX {
	uint64_t state;
}FNV164_CTX;

void FNV164Init(FNV164_CTX *context);
void FNV164Update(FNV164_CTX *context, const unsigned char *input, unsigned int inputLen);
void FNV164Final(unsigned char digest[16], FNV164_CTX * context);

struct FNV164_CTX * FNV164_CTX_create (void);
void FNV164_CTX_delete (FNV164_CTX * context);

static uint64_t fnv_64_buf(void *buf, size_t len, uint64_t hval);