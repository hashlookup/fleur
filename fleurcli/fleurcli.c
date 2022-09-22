#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <fleur.h>
#include <myutils.h>

BloomFilter bf, dst;

#define BADKEY (int)(-1)
#define NKEYS (sizeof(lookuptable)/sizeof(t_symstruct))
typedef struct { char *key; int val; } t_symstruct;
enum modes{check, insert, show, create, setdata, getdata, join};
static t_symstruct lookuptable[] = {
    { "check", check },
    { "insert", insert },
    { "show", show },
    { "create", create },
    { "set-data", setdata },
    { "get-data", getdata },
    { "join", join}
};

int keyfromstring(char *key)
{
    int i;
    for (i=0; i < NKEYS; i++) {
        t_symstruct *sym = &lookuptable[i];
        if (strcmp(sym->key, key) == 0)
            return sym->val;
    }
    return BADKEY;
}

void usage(void)
{
    printf("NAME:\n");
    printf("   Fleurcli - Utility to work with bloom filters\n");
    printf("\n");
    printf("USAGE:\n");
    printf("   fleurcli [-c] command [command options] [arguments...] bloomfilter.file\n");
    printf("\n");
    printf("VERSION:\n");
    printf("   0.1\n");
    printf("\n");
    printf("COMMANDS:\n");
    printf("     create         Create a new Bloom filter and store it in the given filename.\n");
    printf("     insert         Inserts new values into an existing Bloom filter.\n");
    printf("     check          Checks values against an existing Bloom filter.\n");
    printf("     set-data       Sets the data associated with the Bloom filter.\n");
    printf("     get-data       Prints the data associated with the Bloom filter.\n");
    printf("     show           Shows various details about a given Bloom filter.\n");

}

int main(int argc, char* argv[])
{
    int opt;
    int cancelread = 0;
    double p = 0.01;
    uint64_t n = 10000;
    char* bloom_path;
    char* bloom_path_dst;
    char* mode_str;
    
    bloom_path = calloc(128,1);
    bloom_path_dst = calloc(128,1);
    mode_str = calloc(128,1);

    while ((opt = getopt(argc, argv, "c:p:n:h")) != -1) {
        switch (opt) {
            case 'c':
                snprintf(mode_str, 128, "%s", optarg);
                if ((strcmp(optarg, "create\0") == 0) || (strcmp(optarg, "join\0") == 0)){
                    cancelread =1;
                }
                break;
            case 'p':
                if(sscanf(optarg, "%lf", &p) != 1){
                    fprintf(stderr, "[ERROR] Invalid command line was specified\n");
                    return EXIT_FAILURE;
                }
                continue;
            case 'n':
                if(sscanf(optarg, "%lu", &n) != 1){
                    fprintf(stderr, "[ERROR] Invalid command line was specified\n");
                    return EXIT_FAILURE;
                }
                continue;
            case 'h':
                usage();
                free(bloom_path);
                free(bloom_path_dst);
                free(mode_str);
                return EXIT_SUCCESS;
            default: /* '?' */
                fprintf(stderr, "[ERROR] Invalid command line was specified\n");
                free(bloom_path);
                free(bloom_path_dst);
                free(mode_str);
                return EXIT_FAILURE;
        }
    }


    snprintf(bloom_path, 128, "%s", argv[argc-1]);
    
      
    if (!bloom_path[0]){
        fprintf(stderr,"[ERROR] A path must be specified\n");
        return EXIT_FAILURE; 
    }

    if (!cancelread){
        FILE* in = fopen(bloom_path, "rb");
        if (in == NULL) {
            fprintf(stderr, "[ERROR] %s", strerror(errno)); 
            return EXIT_FAILURE;
        }

        bf = fleur_bloom_filter_from_file(in);
        if (bf.error != 0){
            fclose(in);
            return EXIT_FAILURE;
        }
        fclose(in);
    }
    // variables for reading from stdin
    char *buffer = NULL;
    size_t bufsize = 64;
    size_t nread; 
    // variable for file manipulation
    FILE* f;
    // variable for data manipulation
    size_t totalnread; 
    unsigned char* totalread;
    // res holding insertin result
    int res = 1;

    switch (keyfromstring(mode_str)) {
        case check: 
            while ((nread = getline(&buffer, &bufsize, stdin)) != -1) {
                if (fleur_check(&bf, buffer, nread-1) == 1){
                    printf("%s", buffer);
                }
            }
            fleur_destroy_filter(&bf);
            free(buffer);
            free(bloom_path);
            free(bloom_path_dst);
            free(mode_str);
            return EXIT_SUCCESS;
        case insert:
            while (((nread = getline(&buffer, &bufsize, stdin)) != -1) && (res != -1)) {
                res = fleur_add(&bf, buffer, nread-1);
            }
            // Save to bloom filter file
            f = fopen(bloom_path, "wb");
            if (f == NULL) {
                fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                return EXIT_FAILURE;
            }
            fleur_bloom_filter_to_file(&bf, f);
            fclose(f);
            fleur_destroy_filter(&bf);
            free(bloom_path);
            free(bloom_path_dst);
            free(mode_str);
            return EXIT_SUCCESS;
        case show:
            fleur_print_filter(&bf);
            fleur_destroy_filter(&bf);
            free(bloom_path);
            free(bloom_path_dst);
            free(mode_str);
            return EXIT_SUCCESS;
        case create:
            bf = fleur_initialize(n, p, "");
            f = fopen(bloom_path, "wb+");
            if (f == NULL) {
                fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                return EXIT_FAILURE;
            }
            // TODO
            fleur_bloom_filter_to_file(&bf, f);
            fleur_destroy_filter(&bf);
            fclose(f);
            free(bloom_path);
            free(bloom_path_dst);
            free(mode_str);
            return EXIT_SUCCESS;
        case getdata:
            printf("%s", bf.Data);
            return EXIT_SUCCESS;
        case setdata:
                totalnread = 0;
                while ((nread = getline(&buffer, &bufsize, stdin)) != -1) {
                    if (totalnread == 0){
                        totalread = calloc(nread - 1, sizeof(unsigned char));
                        memcpy(totalread, buffer, nread);
                    }else{
                        totalread = realloc(totalread, (totalnread + (nread - 1)));
                        memcpy(totalread + totalnread, buffer, (nread -1));
                    }
                    totalnread += (nread - 1);
                }
                if (totalnread != 0){
                    fleur_set_data(&bf, totalread, totalnread);
                    f = fopen(bloom_path, "wb");
                    if (f == NULL) {
                        fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                        return EXIT_FAILURE;
                    }
                    fleur_bloom_filter_to_file(&bf, f);
                    fclose(f);
                    free(totalread);
                    fleur_destroy_filter(&bf);
                }
            free(bloom_path);
            free(bloom_path_dst);
            free(mode_str);
            return EXIT_SUCCESS;
        case join:
            snprintf(bloom_path, 128, "%s", argv[argc-2]);
            snprintf(bloom_path_dst, 128, "%s", argv[argc-1]);
            
            if (!bloom_path[0]){
                fprintf(stderr,"[ERROR] A path must be specified\n");
                return EXIT_FAILURE; 
            }
            FILE* in = fopen(bloom_path, "rb");
            if (in == NULL) {
                fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                return EXIT_FAILURE;
            }

            bf = fleur_bloom_filter_from_file(in);
            if (bf.error != 0){
                fclose(in);
                fleur_destroy_filter(&bf);
                return EXIT_FAILURE;
            }
            fclose(in);
            if (!bloom_path_dst[0]){
                fprintf(stderr,"[ERROR] A destination path must be specified\n");
                return EXIT_FAILURE; 
            }
            
            FILE* fdst = fopen(bloom_path_dst, "rb");
            if (fdst == NULL) {
                fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                return EXIT_FAILURE;
            }
            dst = fleur_bloom_filter_from_file(fdst);
            if (dst.error != 0){
                printf("%d\n", dst.error);
                return EXIT_FAILURE;
            }
            fclose(fdst);
            int err;
            err = fleur_join(&bf, &dst);
            if(err != 1){
                return EXIT_FAILURE;
            }

            fleur_print_filter(&dst);

            fdst = fopen(bloom_path_dst, "w");
            if (fdst == NULL) {
                fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                return EXIT_FAILURE;
            }

            // TODO
            int ret = 1;
            fleur_bloom_filter_to_file(&dst, fdst);
            if (ret != 1){
                fprintf(stderr, "[ERROR] %s", strerror(errno)); 
            }
            fclose(fdst);
            return EXIT_SUCCESS;

        case BADKEY:
            usage();
            return EXIT_SUCCESS;
    }
}