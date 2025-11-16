# CS50 TSE Indexer
## Implementation Spec

This document outlines the implementation-specific decisions made for the TSE Indexer, building on the Requirements Specification [REQUIREMENTS.md](https://github.com/CS50DartmouthFA2025/home/blob/main/labs/tse/indexer/REQUIREMENTS.md) and Design Specification [DESIGN.md](https://github.com/CS50DartmouthFA2025/home/blob/main/labs/tse/indexer/DESIGN.md).
Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use two data structures:
Hashtable: To efficiently map words to document IDs and their occurrence counts, ensuring quick lookups and insertions.
Counters: To keep track of the number of occurrences of each word in each document. Each entry in the hashtable points to a counters data structure.

## Control flow

The Indexer is implemented in `indexer.c`, with four functions.

### main

The main function orchestrates the indexer's operations:

* Validate and parse command-line arguments using `parseArgs`.
* Initialize an empty index with `index_new`.
* Build the index from the crawled pages using `indexBuild`.
* Save the built index to a file using `index_save`.
* Clean up and free allocated resources.

### parseArgs

This function extracts and validates the command-line arguments, ensuring the correct number of arguments and validating the page directory and index filename.

* check if the number of the arguments is correct
* for `pageDirectory`, call `pagedir_validate` to check
* for `indexFilename`, Check if it is writable. This can involve trying to open the file with write permissions using `fopen` or equivalent system calls.
* if any trouble is found, print an error to stderr and exit non-zero


### indexBuild

This function constructs the index:
* Iteratively loads each webpage using `pagedir_load`, starting from the first document.
* For each loaded webpage, it extracts, normalizes, and adds words to the index with `indexPage`.
* Continues until no more webpages are found in the directory.

Pseudocode:

```c
    Initialize index as a new empty index structure
    Set docID to 1
    while true:
        Construct the file path for the current docID in pageDirectory
        Attempt to load the webpage from this file path
        if the webpage is successfully loaded:
            Call indexPage with the loaded webpage, index, and docID
            Increment docID for the next iteration
        else:
            Break from the loop as no more webpages are available
    return the built index
```

### indexPage

Processes a single webpage, extracts words from the webpage content.
Normalizes each word.
Adds the normalized word along with its document ID to the index.

Pseudocode:

```c
	Function indexPage(page, index, docID):
    Initialize position to 0
    while true:
        Extract the next word from the webpage starting at the current position
        if a word is extracted:
            Normalize the word to ensure uniformity (e.g., convert to lowercase)
            if the word meets the criteria for inclusion (e.g., length greater than 2 characters):
                Add the word along with the current docID and a count of 1 to the index
                if the word is already present for this docID, increment its count
            Move to the next position in the webpage
        else:
            Break from the loop as no more words are available
```

## Other modules

### word

The `word`, a re-usable module is designed to support the Tiny Search Engine (TSE) by providing functionality for word normalization. This functionality is essential for ensuring that the indexing and querying processes treat variations of a word (due to case differences) as the same word. For instance, "Apple", "apple", and "APPLE" should all be considered the same word by the TSE. This module encapsulates the knowledge of how to normalize a word, making it a reusable component within the TSE architecture, particularly useful for the Indexer and Querier components.

Pseudocode for `NormalizeWord`:

```c
	Function NormalizeWord(word):
    Check if the input word is NULL
        if so, return NULL to indicate error or invalid input

    Calculate the length of the input word
    Allocate memory for a new string of the same length + 1 for the null terminator
        if memory allocation fails, return NULL

    for each character in the input word
        Convert the character to lowercase and store it in the new string

    Null-terminate the new string
    return the new string
```

### pagedir

We add below two functions to module `pagedir.c` to support the Indexer.

Pseudocode for `pagedir_validate`:

```c
    if pageDirectory is NULL
        return false

    Construct crawlerFile path as pageDirectory/.crawler

    Check if crawlerFile exists using stat
    if the file exists
        return true
    else
        return false
```

Pseudocode for `pagedir_load`:

```c
    if pageDirectory is NULL
        return NULL

    Calculate the length needed for docID as a string

    if calculation fails (length <= 0)
        return NULL

    Allocate memory for docID string (docIdStr) with length + 1 for null terminator
    if allocation fails
        return NULL

    Convert docID to a string and store in docIdStr

    Calculate the length needed for the full file path

    Allocate memory for full file path (fullPath)
    if allocation fails
        free docIdStr and return NULL

    Construct fullPath as pageDirectory/docIdStr

    Open fullPath for reading
    if opening fails
        Free docIdStr and fullPath, then return NULL

    Read URL, depth string (depthStr), and HTML from the file
    Convert depthStr to an integer (depth)
    Create a new webpage object with URL, depth, and HTML
    Free depthStr, docIdStr, and fullPath
    Close the file

    return the new webpage object
```

### index

The `index.c` module is part of the Tiny Search Engine (TSE) project, designed to create, manage, and utilize an in-memory inverted index. 

An inverted index maps from words to a list of documents (and positions within those documents) where the words occur. 

This module provides functionalities such as adding words to the index, saving the index to a file, loading an index from a file, setting and getting the count of words in documents, and cleaning up the index structure.

Pseudocode for `index_new`:

```c
    allocate memory for an index structure
    initialize a new hashtable with num_slots
    if hashtable initialization is successful
        return the initialized index
    else
        Free the allocated memory for the index
        return NULL
```

Pseudocode for `index_add`:

```c
    if index, word are valid and docID, count are positive
        Find the counters structure for the word in the hashtable
        if the word does not exist in the hashtable
            Create a new counters structure
            Insert the new word and counters into the hashtable
        Increment the count for docID in the counters structure
        return true
    else
        return false
```

Pseudocode for `index_save`:

```c
    if index and fp are valid
        Iterate through each word in the hashtable
        for each word, iterate through its counters
        print each word and its counters to the file in a predefined format
```

Pseudocode for `index_iterate`:

```c
    if index and itemfunc are valid
        Iterate through each word in the hashtable
```

Pseudocode for `index_load`:

```c
    if fp is valid
        Create a new index with an estimated size
        while there are words to read from the file
            Read the word and its document counts
            for each document count pair
                Set the count for the document ID in a new counters structure
            Insert the word and its counters into the index
        return the loaded index
    else
        return NULL
```

Pseudocode for `index_delete`:

```c
    if index is valid
        Iterate through the hashtable and delete each counters structure
        Free the hashtable
        Free the index
```

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
static index_t* indexBuild(const char* pageDirectory);
static void indexPage(webpage_t* page, index_t* index, int docID);
static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename);
```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `pagedir.h` and is not repeated here.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
bool pagedir_validate(const char* pageDirectory);
webpage_t* pagedir_load(const char* pageDirectory, int docID)
```

### index

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `index.h` and is not repeated here.

```c
index_t* index_new(const int num_slots)
void index_add(index_t* index, const char* word, const int docID, const int count);
void index_save(const index_t* index, FILE* fp);
void index_iterate(index_t* index, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));
index_t* index_load(FILE* fp);
void index_delete(index_t* index);
```

### word

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `word.h` and is not repeated here.

```c
char* NormalizeWord(const char* word);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally.

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

There are two units (word and index).
The two units are tiny; they could be tested using a small C 'driver' inside `word.c`, `index.c` to invoke its functions with various arguments, but it is likely sufficient to observe its behavior during the system test.

### Integration/system testing

We write a script `testing.sh` that invokes the crawler several times, with a variety of command-line arguments.
First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made.
Second, a run with valgrind over `indexer`.
Third, use the `indexcmp` to compare the generated index file with the files provided inside the shared folder.
Run that script with `bash -v testing.sh` so the output of crawler is intermixed with the commands used to invoke the indexer.
Verify correct behavior by studying the output, and by sampling the files created in the respective pageDirectories.