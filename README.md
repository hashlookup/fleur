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