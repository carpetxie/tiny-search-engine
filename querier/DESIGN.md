## Querier Design Spec

This document describes how our `querier` is structured and how it interprets queries over the index. Our document will be described from a high level understanding to a lower level. 

---

### High‑level design

At a high level, the querier loads the on-disk index (build by the indexer) into memory. It then reads queries from standard input, one per line. We then normalize each query into a cleaned array of words(fuzzy matching logic). Now we must evaluate the Boolean query with `and`/`or` (with `and` having higher precendecne). Finally we compute the scores for each document using the rules from the specs and print the results in descending score order. 

Querier is deliberately split into small helpers:

- `line_clean` — validation and tokenization
- `index_loader` — load index file into an in‑memory structure
- `ctrs_merge` / `ctrs_intersect` — Boolean OR / AND over counters
- `bnf` — the query interpreter that implements the grammar
- `print_max` and helpers — ranking and printing in score order

---


### Inputs and outputs

Now let's take a look at the inputs and outputs of the function as a whole. We take in two command-line arguments:
- **Command‑line arguments**
  - `pageDirectory`: directory produced by the crawler
  - `indexFilename`: file produced by the indexer

Then we take inputs from the user(standard input):
- **Standard input**
  - One query per line, consisting of alphabetic words and optional `and` / `or` operators.

And finally, our output:
- **Output**
  - Either:
    - Error messages for incorrect input, or 
    - A pretty‑printed echo of the query, followed by matching documents in descending score order, each with score, docID, and URL.

---

### Core data structures

- **In‑memory index: `hashtable_t *`**
This is a look up to the following data_structure. 
  - Key: `char *word`
  - Value: `counters_t *postings`
  - Chosen because:
    - Constant‑time average lookup by word and it was easy to populate from the index file line‑by‑line.
  - The number of slots is chosen as `file_numLines(indexFilename)` so there is roughly one slot per indexed word(as hinted by the assignment).

- **Postings list: `counters_t *`**
Each word appears in many documents for some number of times. This data structure simply counts how many times a word appears in each document by pairing the `docID` to `count`:

  - Maps `docID -> count`.
  - Represents all occurrences of a given word across documents.
  - Used as the building block for all Boolean operations:
    - AND = intersection with `min(countA, countB)`
    - OR = union with `countA + countB`

- **Helper struct for intersections**
This is just a simple helper function for `counters_iterate` such that we see the counters of both sets( `ctrsA` and `ctrsB`). We can write this into `result`.
  ```c
  typedef struct intersect_arg {
    counters_t* result;
    counters_t* ctrsB;
  } intersect_arg_t;
  ```


- **Helper struct for ranking/printing**
We use this to have a mapping to the first occurence of a document with score X. We later zero this out so it won't appear again when we scan for the same score. 
  ```c
  typedef struct doc_score_pair {
    int docID;
    int score;
    counters_t* ctrs;
  } doc_score_pair_t;
  ```

---

### Major modules and responsibilities

Now here is the story of how everything fits together:

`main` handles all of the orchestration. It checks the two command‑line arguments, verifies that `pageDirectory/.crawler` and `pageDirectory/1` exist, sizes and loads the hashtable with `index_loader`, and then: prompt, read a line, clean it with `line_clean`, evaluate it with `bnf`, and print ranked results with `print_max`. On EOF it frees the index and exits.

`line_clean` does all the low‑level parsing. It walks the buffer to reject any non‑alphabetic, non‑whitespace characters, then lowercases and tokenizes in place so that `words[]` becomes an array of pointers into the buffer. On top of that it enforces the query “shape” rules: no operator at the beginning or end, no two operators in a row, and no single‑word query that is just `and` or `or`. If anything looks wrong, it prints an error and tells the caller not to continue.

`index_loader` is the bridge from disk to memory. It reads the index file line‑by‑line, builds a fresh `counters_t` for each word, parses `(docID, count)` pairs out of the rest of the line, and stores the result in the hashtable under that word. If a line cannot be parsed or inserted, it reports the problem and stops the load.

`ctrs_merge` and `ctrs_intersect` capture the Boolean logic in terms of counters. For the `or` functionality, we called it `ctrs_merge`: it walks one counters table and, for each `(docID, scoreB)`, either inserts that score into the other table or adds it to the existing score. For `AND`, we called it `ctrs_intersect`: it walks one table and, whenever the same `docID` appears in the other, it records the minimum of the two scores in a separate result table.

On top of those primitives, `bnf` implements the query grammar. It treats the token array as a sequence of “and sequences” separated by the literal word `or` WITH `AND` AS PRECEDENCE. For each and sequence it builds a running counters: it starts with the first word’s postings, then intersects with each additional word in the same group. Each finished and sequence is saved. At the end, `bnf` puts all of these and sequence results together into a single counters using repeated merges, and returns that final table as the scoring of the whole query.

`print_max` takes that final counters table and turns it into output the user can read. It first finds the largest score present. If that score is zero, there are no matching documents. Otherwise, it walks scores from that maximum down to 1, and for each score repeatedly scans the counters looking for documents with exactly that value. Each time it finds one, it prints the score and docID, opens the corresponding file in `pageDirectory` to grab the URL on the first line, prints the URL, and then sets that document’s score to zero so it will not be printed again.

---

### Error handling and robustness

The code is deliberately conservative about bad inputs and failures. Every file open is checked; if something cannot be opened we print a specific message and either exit (in `main`) or return an error flag from the helper. All of the query validation lives in `line_clean`, so incorrect queries get rejected before we ever touch the index. For memory, each allocation has a clear owner: the hashtable is destroyed with `hashtable_delete` and an `itemdelete` callback that frees the `counters_t` values, and any temporary counters or helper structs created inside `bnf` or `print_max` are freed right after use.

The entire directory operates as a pipeline such that when an error occurs, we know precisely where the error is. 

---

### Assumptions

To keep things focused, the querier leans on a few explicit assumptions. It assumes the index file produced by the indexer is well‑formed: one word per line, followed by `docID count` pairs. It assumes each crawler page file lives in the `pageDirectory`, is named by its numeric `docID`, and has the URL on the first line. And it assumes that calling `hashtable_new(file_numLines(indexFilename))` gives a table that is “good enough” in size for the number of unique words, which is the guideline suggested in the assignment.

In other words, we can guarantee the internal functions of the querier are correct. Our promise is that if the inputs to the querier are correct, then the output ought to be correct too. 


