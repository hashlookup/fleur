#include <inttypes.h>
#include <stdio.h>
#include "fnv.h"

uint64_t fnv1(char *buf, size_t buf_size) {
   uint64_t h = FNV_OFFSET;
   for (size_t i = 0; i < buf_size; i++) {
      h *= FNV_PRIME;
      h ^= buf[i];
   }
   return h;
}


void getDigest(fnvhash * fh, char * str){
  sprintf(str, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",
        fh->hexrepr[7],
        fh->hexrepr[6],
        fh->hexrepr[5],
        fh->hexrepr[4],
        fh->hexrepr[3],
        fh->hexrepr[2],
        fh->hexrepr[1],
        fh->hexrepr[0]
        );
}
