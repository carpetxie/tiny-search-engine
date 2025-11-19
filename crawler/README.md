# CS50 Final Project
## CS50 Fall 2025

### crawler 

The crawler is the first component of the Tiny Search Engine (TSE). Its job is to: 
1. Start from a seed URL 
2. Retrive ("fetch") the corresponding webpage 
3. Parse ("scan") that page for links 
4. Add new, internal URLs to a bag for future crawling 
5. Save every fetched page into a specified directory 
6. Repeat until no pages remain or maximum depth is reached

The crawler uses modules from the `libcs50` library (`bag`, `hashtable`, `webpage`) and the `pagedir` module from `common`. 

### Usage 

The crawler module is implemented in `crawler.c` and built with the provided Makefile. It exports the following executable: 

```c
./crawler seedURL pageDirectory maxDepth 
```

Arguments: 
* `seedURL`: Must be a valid internal URL for the TSE sites 
* `pageDirectory`: Must be a writable directory; crawler creates .crawler inside it 
* `maxDepth`: Integer from 0 to 10, inclusive 

### Implementation

The crawler follows this algorithm: 
1. Normalize and validate the seed URL 
2. Initialize te page directory by creating the `.crawler` file 
3. Create a hashtable (`pagesSeen`) to track which URLs have already been visited
4. Create a bag (`pagesToCrawl`) and insert the seed webpage at depth 0
5. While the bag is not empty: 
    - Remove a webapge from the bag 
    - Fetch its HTML 
    - Save it in `pageDirectory`
    - If te page depth is less than `maxDepth`, scan its HTML for links 
    - Normalize and check each discovered URL 
    - If the URL is internal and not yet seen, add it to the hashtable and bag 
6. Free all allocated data structures 

Pages are saved using the `pagedir_save()` function, which writes the URL, depth, and full HTML into files named 1, 2, 3, ad so on. 

### Differences from Spec

* The crawler exits using exit() with non-zero codes on error rather than returning error codes from main. This still satisfies the spec requirement to exit non-zero for invalid usage. 
* Failed fetches are not retired; the crawler prints a warning and continues. 
* Normalized URLs are sometimes printed after freeing the original raw URL; this is handled safely, but output ordering may differ slightly from the spec. 
* Extreme pages with thousands of links may slow the crawler due to repeated normalization and hashing. 

### Assumptions 

* The seed URL mus tbe internal and reachable. 
* The page directory must exist and be writable.
* The maximum depth must be an integer in the range [0, 10].
* Memory for URLs is owned by their corresponding `webpage_t` objects and is freed by `webpage_delete()`.
* The crawler ignores external or malformde URLs. 

### Files 

* `Makefile` - compilation rules for building and testing the crawler 
* `crawler.c` - main crawler implementation 
* `testing.sh` - script to test crawler functionality 

### Compilation

To compile, `make`.
To remove object files and the executable, `make clean`.

### Testing

The `testing.sh` script performs argument-checking tests and small-depth crawls of sample CS50 TSE websites. Run `make test`. This creates a `testing.out` file containing the full test log. 