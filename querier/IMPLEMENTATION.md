## Querier Implementation Spec

This document explains how my querier implementation realizes the design: what each function actually does, how they fit together, and how I tested the result. 

---

### Build and usage

From the top‑level `tse` directory, I build everything with `make`.

This compiles the support libraries, `common`, `crawler`, `indexer`, and finally `querier/querier`.

To run the querier:

```
./querier pageDirectory indexFilename
```

Here `pageDirectory` must be a crawler output directory (it contains `.crawler` and at least file `1`), and `indexFilename` must be an index file produced by the indexer. Once started, the program reads one query per line from standard input.

---

### Main control flow (`main`)

`main` first checks that there are exactly two command‑line arguments; otherwise it prints `Invalid parameters.` and exits with status `1`. It then validates the `pageDirectory` by opening `pageDirectory/.crawler` and `pageDirectory/1`. If either fails, it prints `pageDirectory formatted incorrectly.` and exits with status `2` or `3`. Next, it validates the `indexFilename` by trying to open it for reading; failure here prints `indexFilename invalid.` and exits with status `4`.

Once the index file is open, `main` calls `file_numLines` to count its lines, uses that count to create an in‑memory hashtable with `hashtable_new(index_lines)`, and calls `index_loader` to fill it. If `index_loader` returns `false`, `main` prints `indexFilename invalid.` and exits with status `5`.

After setup, `main` prints `Query? `, reads a line with `fgets`, and passes that buffer to `line_clean`, which returns an array of tokens and a word count. If the query is valid, it echoes the cleaned query, passes the tokens into `bnf` to get a `counters_t*` of document scores, and then calls `print_max` to print ranked results. It frees that temporary counters and repeats until EOF. At the end, it deletes the in‑memory index with `hashtable_delete(table, itemdelete)` and returns `0`.

---

### Loading the index (`index_loader`)

```
static bool index_loader(hashtable_t* ht, char* filename);
```

It opens `filename` for reading and then processes the file one line at a time using `file_readLine`. For each line, it:

- allocates a new `counters_t` for that word,
- uses `sscanf` with `%s %n` to extract the word and remember how many characters were consumed,
- moves a `curr` pointer past the word and repeatedly calls `sscanf(curr, "%d %d %n", ...)` to grab `(docID, count)` pairs, and
- for each pair, calls `counters_set(ctrs, docID, cnt)`.

After all pairs on the line are processed, it inserts `word -> ctrs` into the hashtable with `hashtable_insert`. The line buffer from `file_readLine` is freed via `mem_free` before moving to the next line.

If allocation of `ctrs` fails, or if setting a counter or inserting into the hashtable fails, `index_loader` reports an error, frees any allocated data for that line, closes the file, and returns `false`. Otherwise it finishes the file, closes it, and returns `true`.

---

### Cleaning and validating queries (`line_clean`)

```
static bool line_clean(char** words, char* buffer, int* wc);
```

`line_clean` first scans `buffer` and rejects any character that is neither `isalpha` nor `isspace`. If it finds one, it prints `Error: bad character 'x' in query.` and returns `false`. If that passes, it makes a second pass that both lowercases and tokenizes:

- each alphabetic character is lowered in place,
- whitespace characters are turned into `'\0'`, and
- the start of each new word is recorded in `words[word_count]`.

After that, it checks the shape rules: if there is more than one word, the first and last words may not be `and` or `or`, and no two adjacent tokens can both be operators. If there is exactly one word, it cannot be just `and` or `or`. If there are no words at all, the line is treated as an empty query and the function quietly returns `false`. On success, `line_clean` writes the number of tokens into `*wc` and returns `true`. On any failure, it prints a short, specific error and returns `false`, and the caller simply skips that query.

---

### Boolean operations on counters

```
static void ctrs_merge(counters_t* ctrsA, counters_t* ctrsB);
static void ctrs_intersect(counters_t* result, counters_t* ctrsA, counters_t* ctrsB);
```

`ctrs_merge` implements OR. It iterates over all `(docID, scoreB)` pairs in `ctrsB`, looks up `scoreA = counters_get(ctrsA, docID)`, and writes back either `scoreB` (if the doc was not present in `ctrsA`) or `scoreA + scoreB` (if it was). This is used both to seed a new andsequence from a single word’s counters and to accumulate the OR of all andsequences at the end of `bnf`.

`ctrs_intersect` implements AND. It wraps a small `intersect_arg_t` that holds a `result` counters and a pointer to `ctrsB`, then iterates over `(docID, valA)` in `ctrsA`. For each doc it looks up `valB = counters_get(ctrsB, docID)`. If `valB` is non‑zero, it stores `min(valA, valB)` into `result`.

---

### Evaluating the query (`bnf`)

```
static counters_t* bnf(hashtable_t* index, char* words[], int word_count);
```

`bnf` takes the cleaned token array and the in‑memory index and returns a `counters_t*` mapping `docID -> score` for the whole query. It allocates an array `andargs` large enough to hold a `counters_t*` for each token, and uses a running `counters_t* curr` to accumulate the current andsequence.

It then walks through `words[]` once:

- When it sees `or`, it appends any non‑NULL `curr` into `andargs` and resets `curr` to `NULL`.
- When it sees `and`, it does nothing; AND is implicit.
- When it sees a regular word, it looks up that word in the index:
  - if we are at the start of the query or right after an `or`, it starts a new andsequence by setting `curr = counters_new()` and, if the word exists, seeding it with that word’s counters via `ctrs_merge`;
  - otherwise we are still in the same andsequence, so if both the word and `curr` exist, it builds a new intersection with `ctrs_intersect`, deletes the old `curr`, and replaces it; if the word is missing, it deletes `curr` and sets it to `NULL`, meaning this andsequence matches no documents.

After the loop, any remaining non‑NULL `curr` is also appended to `andargs`. In the second stage, `bnf` creates an empty `counters_t* res` and folds all of the andsequence results into it using `ctrs_merge`, deleting each intermediate counters and finally freeing `andargs`. The `res` table it returns is the final document scores for the query.

---

### Ranking and output (`print_max` and helpers)

```
static void print_max(counters_t* ctrs, const char* pageDirectory);
static void find_max(void* arg, const int docID, const int score);
static void print_curr_max(void* arg, const int docID, const int score);
```

`print_max` takes the result of `bnf` and the `pageDirectory` and prints matching documents in descending order by score.

First it uses `find_max` and `counters_iterate` to find the largest score present in the counters. If that maximum is zero, it prints `No documents match.` and returns. Otherwise it loops `i` from `max` down to `1`. For each `i`, it prepares a `doc_score_pair_t` that carries the target score and a pointer to the counters, and calls `counters_iterate` with `print_curr_max`. That helper looks for the first `(docID, score)` where `score == pair->score`, prints `score  <score>  doc  <docID>:`, records the `docID` into the pair, and sets that document’s score in the counters to zero so it will not be seen again. As long as `pair->docID` is non‑zero, `print_max` opens `pageDirectory/docID`, reads its first line (the URL) with `file_readLine`, prints the URL, resets `pair->docID` to zero, and calls `counters_iterate` again to look for another document with the same score.

---

### Memory management



- `index_loader` frees each `line` returned by `file_readLine` and, on error, deletes any partially built `counters_t` and closes the file.
- `bnf` allocates the `andargs` array as well as several temporary counters (`curr` and intersections); it deletes each andsequence counters after merging into `res` and frees the array itself at the end.
- `print_max` allocates one `doc_score_pair_t` per outer score loop and frees it at the end of that iteration; it also frees each URL line read from the page files.
- `main` deletes the per‑query result counters right after printing and deletes the index with `hashtable_delete(table, itemdelete)`, where `itemdelete` simply calls `counters_delete` on each value.

---

### Testing strategy

The querier is tested with a mix of small manual cases and a simple test script.

By hand, I try single‑word queries that do and do not exist, mixed‑case queries to confirm normalization, and a variety of operator edge cases: `and`/`or` at the beginning or end, adjacent operators, and blank or whitespace‑only lines. I also compare related Boolean queries like `wordA and wordB`, `wordA or wordB`, and `wordA or wordB and wordC` and reason about which documents should appear to confirm AND/OR precedence. For ranking, I use small indexes where I can predict the exact scores and check that the output is in descending score order and that ties are preserved.

On top of that, the `testing.sh` script in the `querier` directory runs `./querier` against indices under `/cs50/shared/tse/output`, feeds in several query files, and saves all output into `testing.out` for easy re‑runs and regression checking. The `make test` target is just a thin wrapper around this script. When possible, I also run `valgrind` (outside the CS50 grading environment) to spot memory leaks or invalid accesses during typical runs.