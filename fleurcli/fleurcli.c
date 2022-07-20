#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <fleur.h>


struct BloomFilter * bf;
struct header bfh;

void usage(void)
{
    printf("fleurcli [-h] -b bloomfilter \n\n");
    printf("Does not do much for now.\n"); 
}

int main(int argc, char* argv[])
{
    int opt, check = 0 , add = 0, show = 0;
    char* bloom_path;
    char* to_add;
    char* to_check;
    
    bloom_path = calloc(128,1);
    assert(bloom_path);  

    // while ((opt = getopt(argc, argv, "b:a:hcs")) != -1) {
    while ((opt = getopt(argc, argv, "b:a:c:hs")) != -1) {
        switch (opt) {
            case 'b':
                strncpy(bloom_path , optarg, 128);
                break;
            case 'c':
                check = 1;
                to_check = calloc(128,1);
                strncpy(to_check , optarg, 128);
                break;
            case 'a':
                add = 1;
                to_add = calloc(128,1);
                strncpy(to_add , optarg, 128);
                break;
            case 's':
                show = 1;
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

    // fprintf(stderr, "[INFO] bloom filter path = %s\n", bloom_path);

    FILE* in = fopen(bloom_path, "rb");
    size_t elements_read = fread(&bfh, sizeof(bfh), 1, in);
    if(elements_read == 0){
        fprintf(stderr, "[ERROR] %s", strerror(errno)); 
    }

    bf = BloomFilterFromFile(&bfh, in);

    if (show == 1){
        print_filter(bf);
        return EXIT_SUCCESS;
    }

    if (add == 1){
        Add(to_add, strlen(to_add), bf);
        if (Check("checking value", strlen("checking value"), bf) == 1){
            printf("oui\n");
        }else{
            printf("non\n");
        }
        print_filter(bf);
        return EXIT_SUCCESS;
    }

    // Checking values from stdin
    if (check == 1){
        printf("%s", to_check);
        if (Check(to_check, strlen(to_check), bf) == 1){
            printf("oui\n");
        }else{
            printf("non\n");
        }

        // char c[80] = { 0 };
        // while(fgets(c, sizeof(c), stdin) != NULL)
        // {
        //     if (Check(c, 80, bf) == 1) {
        //         printf("oui");
        //         // printf("%s\n", c);
        //     }
        // }
    }

    fclose(in);
    return EXIT_SUCCESS;
}