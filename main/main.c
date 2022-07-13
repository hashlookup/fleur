#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>

void usage(void)
{
    printf("hashlookup-yara [-h] -b bloomfilter \n\n");
    printf("Does not do much for now.\n"); 
}

int main(int argc, char* argv[])
{
    int opt;
    char* bloom_path;
    
    bloom_path = calloc(128,1);
    assert(bloom_path);  

    while ((opt = getopt(argc, argv, "b:h")) != -1) {
        switch (opt) {
            case 'b':
                strncpy(bloom_path , optarg, 128);
                break;
            case 'h':
                usage();
                return EXIT_SUCCESS;
            default: /* '?' */
                fprintf(stderr, "[ERROR] Invalid command line was specified\n");
        }
    }
    if (!bloom_path[0]){
        fprintf(stderr,"[ERROR] A path to a DCSO Bloomfilter file must be specified\n");
        return EXIT_FAILURE; 
    }

    fprintf(stderr, "[INFO] bloom filter path = %s\n", bloom_path);
}