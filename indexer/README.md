# CS50 FA25 Final Project TSE Indexer
## Author: Cindy Jiayi Liu
## Date: 2025.11.16

This module is the second part of tiny search engine final project, the indexer.
The implementation Document: [IMPLEMENTATION.md]

## Usage
To build, either run `make` at top directory to generate all the executable programs, or run `make` in this directory. 

To clean up, run `make clean`.

```c
static index_t* indexBuild(const char* pageDirectory);
static void indexPage(webpage_t* page, index_t* index, int docID);
static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename);
```

## Assumptions

- `pageDirectory` has files named 1, 2, 3, ..., with no gaps.
- Each file in `pageDirectory` is formatted as defined by the crawler specification.
- The program is run with write permission to the current directory and sufficient space to write the index file.


## Files

* `Makefile` - compilation procedure
* `indexer.c` - the implementation
* `testng.sh` - test data
* `testing.out` - result of `make test &> testing.out`
* `IMPLEMENTATION.md` - implementation documentation

## Testing

To test, simply `make test`.

To test with valgrind, `make valgrind`.

## References

Requirement Specs: [REQUIREMENTS.md](https://github.com/CS50DartmouthFA2025/home/blob/main/labs/tse/indexer/REQUIREMENTS.md)

Design Specs: [DESIGN.md](https://github.com/CS50DartmouthFA2025/home/blob/main/labs/tse/indexer/DESIGN.md)

AI Assistance: Claude and Chat GPT is used in planning file hierachies, generating documentation for README.md and IMPLEMENTATION.md, and debugging overall codes.
