#include <stdint.h>
#include "myutils.h"

char* print_bin(uint64_t n)
{
    // mind the nullbyte
    static char str[65];
    uint64_t i = (uint64_t)1 << 63;
    int j = 0;
    for (; i > 0; i = i / 2){
        if (n & i){
            str[j] = '1';
        }else{
            str[j] = '0';
        }
        j++;
    }
    return str;
}
