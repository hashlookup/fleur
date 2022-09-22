# Fleur - A bloom filter implementation in C
Fleur implements a Bloom Filter library that is fully compatible with DCSO's [Go](https://github.com/DCSO/bloom) and [python](https://github.com/DCSO/flor) implementations.

# Requirements
- gcc version supporting C11
- [cmake](https://ninja-build.org/)
- [ninja](https://ninja-build.org/)

Fleur has only been tested under Ubuntu 20.04
```
sudo apt-get install cmake ninja-build gcc
```

# Compilation
```
git clone git@github.com:hashlookup/fleur.git
git submodule update --init --recursive
cmake -GNinja -DTARGET_GROUP=production . 
ninja -v
```

# Running tests
```
cmake -GNinja -DTARGET_GROUP=test . 
ninja -v
```
Each suite test's executable are then located under `./test/suite_n`.

# LibFleur usage
```
struct BloomFilter fleur_initialize(uint64_t n, double p, char *buf);
struct BloomFilter fleur_bloom_filter_from_file(FILE* f);

int fleur_add(BloomFilter * bf, char *buf, size_t buf_size);
int fleur_check(BloomFilter * bf, char *buf, size_t buf_size);
int fleur_join(BloomFilter * src, BloomFilter* dst);
int fleur_bloom_filter_to_file(BloomFilter * bf, FILE* of);
void fleur_set_data(BloomFilter * bf, char* buf, size_t buf_size );
void fleur_fingerprint(BloomFilter * bf, char *buf, size_t buf_size, uint64_t **fingerprint);

void fleur_destroy_filter(BloomFilter * bf);

void fleur_print_header(header * h);
void fleur_print_filter(BloomFilter * bf);
int fleur_check_header(header * h);
```
# Fleur command line tool usage
```
NAME:
   Fleurcli - Utility to work with bloom filters

USAGE:
   fleurcli [-c] command [command options] [arguments...] bloomfilter.file

VERSION:
   0.1

COMMANDS:
     create         Create a new Bloom filter and store it in the given filename.
     insert         Inserts new values into an existing Bloom filter.
     check          Checks values against an existing Bloom filter.
     set-data       Sets the data associated with the Bloom filter.
     get-data       Prints the data associated with the Bloom filter.
     show           Shows various details about a given Bloom filter.
```
Example interactions:
```
$./fleurcli -c create -p 0.0001 -n 100000 mytest.bloom
$bloom show mytest.bloom
File:				/home/jlouis/Git/fleur/fleurcli/mytest.bloom
Capacity:			100000
Elements present:	0
FP probability:		1.00e-04
Bits:				1917011
Hash functions:		14
$echo toto | ./fleurcli -c insert mytest.bloom 
$echo titi | ./fleurcli -c insert mytest.bloom 
$echo babar | ./fleurcli -c insert mytest.bloom
$echo tutu | ./fleurcli -c check mytest.bloom
$echo titi | ./fleurcli -c check mytest.bloom
titi
$echo titi | bloom check mytest.bloom 
titi
$./fleurcli -c show mytest.bloom 
Filter details:
 n: 100000 
 p: 0.000100
 k: 14 
 m: 1917011 
 N: 3 
 M: 29954
 Data: (null).%
```

# Performances
Querying on 2438 sha1 file hashes against ~800MB [hashlookup](https://hashlookup.circl.lu/) filter, finding for both implementations 2176 known files:
```
/usr/bin/time -v  bash -c "cat test.txt | bloom check ../hashlookup-full.bloom"
...
	Command being timed: "bash -c cat test.txt | bloom check ../hashlookup-full.bloom"
	User time (seconds): 1.23
	System time (seconds): 0.24
	Percent of CPU this job got: 100%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:01.47
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 859620
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 8449
	Voluntary context switches: 568
	Involuntary context switches: 15
	Swaps: 0
	File system inputs: 0
	File system outputs: 0
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

```
/usr/bin/time -v  bash -c "cat test.txt | ./fleurcli -c check ../hashlookup-full.bloom"
...
	Command being timed: "bash -c cat test.txt | ./fleurcli -c check ../hashlookup-full.bloom"
	User time (seconds): 0.01
	System time (seconds): 0.33
	Percent of CPU this job got: 98%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.35
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 830524
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 1
	Minor (reclaiming a frame) page faults: 207736
	Voluntary context switches: 15
	Involuntary context switches: 30
	Swaps: 0
	File system inputs: 8
	File system outputs: 0
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

# Acknowledgment

![](./img/cef.png)

The project has been co-funded by CEF-TC-2020-2 - 2020-EU-IA-0260 - JTAN - Joint Threat Analysis Network.
