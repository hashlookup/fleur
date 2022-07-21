# Fleur - A bloom filter implementation in C
Fleur implements a Bloom Filter library that is fully compatible with DSCO's [Go](https://github.com/DCSO/flor) and [python](https://github.com/DCSO/flor) implementations.

# Requirements
- gcc
- [cmake](https://ninja-build.org/)
- [ninja](https://ninja-build.org/)

Fleur has only been tested under Ubuntu 20.04

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

# Fleur command line tool usage

# Performances
Querying on 2438 sha1 hashes against ~800MB hashlookup filter, finding for both implementation 2176 known files:
```
/usr/bin/time -v  bash -c "cat test.txt | bloom check ../hashlookup-full.bloom"
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
/usr/bin/time -v  bash -c "cat test.txt | ./fleurcli -f ../hashlookup-full.bloom"
	Command being timed: "bash -c cat test.txt | ./fleurcli -f ../hashlookup-full.bloom"
	User time (seconds): 0.00
	System time (seconds): 0.33
	Percent of CPU this job got: 96%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.35
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 830508
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 207737
	Voluntary context switches: 16
	Involuntary context switches: 2
	Swaps: 0
	File system inputs: 0
	File system outputs: 0
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```