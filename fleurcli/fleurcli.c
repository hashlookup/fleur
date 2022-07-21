#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <fleur.h>
#include <myutils.h>

struct BloomFilter * bf;
struct header bfh;

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
    printf("fleurcli [-h] -b bloomfilter \n\n");
    printf("Does not do much for now.\n"); 
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

    while ((opt = getopt(argc, argv, "m:p:n:h")) != -1) {
        switch (opt) {
            case 'm':
                strncpy(mode_str, optarg, 128);
                if (strcmp(optarg, "create\0") == 0 ){
                    cancelread =1;
                }
                break;
            case 'p':
                sscanf(optarg, "%lf", &p);
                continue;
            case 'n':
                sscanf(optarg, "%ld", &n);
                continue;
            case 'h':
                usage();
                return EXIT_SUCCESS;
            default: /* '?' */
                fprintf(stderr, "[ERROR] Invalid command line was specified\n");
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
        size_t elements_read = fread(&bfh, sizeof(bfh), 1, in);
        if(elements_read == 0){
            fprintf(stderr, "[ERROR] %s", strerror(errno)); 
        }

        bf = BloomFilterFromFile(&bfh, in);
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

    switch (keyfromstring(mode_str)) {
        case check: 
            while ((nread = getline(&buffer, &bufsize, stdin)) != -1) {
                if (Check(buffer, nread-1, bf) == 1){
                    printf("%s", buffer);
                }
            }
            free(buffer);
            free(bf->v);
            free(bloom_path);
            return EXIT_SUCCESS;
        case insert:
            while ((nread = getline(&buffer, &bufsize, stdin)) != -1) {
                Add(buffer, nread-1, bf);
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
            return EXIT_SUCCESS;
        case show:
            print_filter(bf);
            free(bf->v);
            free(bloom_path);
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
                    free(bf->Data);
                    bf->Data = calloc(totalnread, sizeof(unsigned char));
                    bf->datasize = totalnread;
                    memcpy(bf->Data, totalread, totalnread);
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
            return EXIT_SUCCESS;
        case BADKEY:
            usage();
            return EXIT_SUCCESS;
    }
}