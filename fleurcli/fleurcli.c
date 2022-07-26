#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <fleur.h>
#include <myutils.h>

BloomFilter * bf;

#define BADKEY (int)(-1)
#define NKEYS (sizeof(lookuptable)/sizeof(t_symstruct))
typedef struct { char *key; int val; } t_symstruct;
enum modes{check, insert, show, create, setdata, getdata};
static t_symstruct lookuptable[] = {
    { "check", check },
    { "insert", insert },
    { "show", show },
    { "create", create },
    { "set-data", setdata },
    { "get-data", getdata }
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
    char* mode_str;
    
    bloom_path = calloc(128,1);
    mode_str = calloc(128,1);

    while ((opt = getopt(argc, argv, "c:p:n:h")) != -1) {
        switch (opt) {
            case 'c':
                strncpy(mode_str, optarg, 128);
                if (strcmp(optarg, "create\0") == 0 ){
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
                free(mode_str);
                return EXIT_SUCCESS;
            default: /* '?' */
                fprintf(stderr, "[ERROR] Invalid command line was specified\n");
                free(bloom_path);
                free(mode_str);
                return EXIT_FAILURE;
        }
    }

    for(; optind < argc; optind++){     
        strncpy(bloom_path , argv[optind], 128);
    }
      
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

        bf = BloomFilterFromFile(in);
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
                if (Check(bf, buffer, nread-1) == 1){
                    printf("%s", buffer);
                }
            }
            free(buffer);
            free(bf->v);
            free(bloom_path);
            free(mode_str);
            return EXIT_SUCCESS;
        case insert:
            while (((nread = getline(&buffer, &bufsize, stdin)) != -1) && (res != -1)) {
                res = Add(bf, buffer, nread-1);
            }
            // Save to bloom filter file
            f = fopen(bloom_path, "wb");
            if (f == NULL) {
                fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                return EXIT_FAILURE;
            }
            BloomFilterToFile(bf, f);
            fclose(f);
            free(bf->v);
            free(bloom_path);
            free(mode_str);
            return EXIT_SUCCESS;
        case show:
            print_filter(bf);
            free(bf->v);
            free(bloom_path);
            free(mode_str);
            return EXIT_SUCCESS;
        case create:
            bf = Initialize(n, p);
            f = fopen(bloom_path, "wb+");
            if (f == NULL) {
                fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                return EXIT_FAILURE;
            }
            BloomFilterToFile(bf, f);
            fclose(f);
            free(bf->v);
            free(bloom_path);
            free(mode_str);
            return EXIT_SUCCESS;
        case getdata:
            printf("%s", bf->Data);
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
                    SetData(bf, totalread, totalnread);
                    f = fopen(bloom_path, "wb");
                    if (f == NULL) {
                        fprintf(stderr, "[ERROR] %s", strerror(errno)); 
                        return EXIT_FAILURE;
                    }
                    BloomFilterToFile(bf, f);
                    fclose(f);
                    free(totalread);
                    free(bf->v);
                    free(bf->Data);
                }
            free(bloom_path);
            free(mode_str);
            return EXIT_SUCCESS;
        case BADKEY:
            usage();
            return EXIT_SUCCESS;
    }
}