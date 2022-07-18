#include <stdio.h>
#include "bloom.h"

void print_header(header h){
     printf("Header read from the binary file:\n version: %lu\n n: %lu \n p: %f\n k: %lu \n m: %lu \n N: %lu \n",
    h.version,
    h.n,
    h.p,
    h.k,
    h.m,
    h.N);
}