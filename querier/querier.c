/* querier.c	Final Project: CS50 Part 3: Querier.
 *
 * Implements querier.c based on the design spec, requirement spec, and implementation spec. 
 *
 * Authors: William Bowles, Cindy Liu (counters merge & intersect helper functions from activity 16)
 */

#include <stdio.h>
#include <stdlib.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/set.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

// struct for passing multiple arguments through counters_iterate
typedef struct intersect_arg {
  counters_t* result;
  counters_t* ctrsB;
} intersect_arg_t;

//struct used for a docID score pair for printing
typedef struct doc_score_pair {
  int docID;
  int score;
  counters_t* ctrs;
} doc_score_pair_t;

//function prototypes
static bool line_clean(char** words, char* buffer, int* wc);
static bool index_loader(hashtable_t* ht, char* filename);
static void ctrs_merge(counters_t* ctrsA, counters_t* ctrsB);
static void ctrs_merge_helper(void* arg, const int docID, const int score);
static void ctrs_intersect(counters_t* result, counters_t* ctrsA, counters_t* ctrsB);
static void ctrs_intersect_helper(void* arg, const int docID, const int score);
static counters_t* bnf(hashtable_t* index, char* words[], int word_count);
static void print_max(counters_t* ctrs, const char* pageDirectory);
static void find_max(void* arg, const int docID, const int score);
static void print_curr_max(void* arg, const int docID, const int score);
static void itemdelete(void* item);

int main(int argc, char *argv[]) 
{
  if (argc != 3) {
    fprintf(stderr, "Invalid parameters.\n");
    return 1;
  }

  //validate that the two parameters are outputs produced by crawler & indexer
  FILE* fp;
  char file[300];
  char file2[300];
  snprintf(file, sizeof(file), "%s/.crawler", argv[1]);
  snprintf(file2, sizeof(file2), "%s/1", argv[1]);
  if ((fp=fopen(file, "r")) == NULL) {
    fprintf(stderr, "pageDirectory formatted incorrectly.\n");
    return 2;
  }
  fclose(fp);

  if ((fp=fopen(file2, "r")) == NULL) {
    fprintf(stderr, "pageDirectory formatted incorrectly.\n");
    return 3;
  }
  fclose(fp);

  if ((fp=fopen(argv[2], "r")) == NULL) {
    fprintf(stderr, "indexFilename invalid.\n");
    return 4;
  } 

  //load the index from indexFilename into an internal data structure
  int index_lines = file_numLines(fp);
  hashtable_t* table = hashtable_new(index_lines);
  bool test = index_loader(table, argv[2]); //helper to accomplish this
  if (!test) {
    fprintf(stderr, "indexFilename invalid.\n");
    return 5;
  }
  fclose(fp);

  printf("Query? ");
  //Read search queries from stdin
  char buffer[200];
  while (fgets(buffer, sizeof(buffer), stdin) != NULL) { //grabs from stdin
    char* words[100];
    int word_count = 0;
    bool cont = true;
    cont = line_clean(words, buffer, &word_count); //saves the word count and builds a words array of strings
    if (cont) { //so if the query is valid, we can continue
      printf("Query: ");
      for (int i = 0; i < word_count; i++) {
        printf("%s ", words[i]); //prints the query
      }
      printf("\n");
      counters_t* search = bnf(table, words, word_count); //scores each document into a counters struct
      print_max(search, argv[1]); //prints scores in descending order
      counters_delete(search);
    }
    printf("\n");
    printf("Query? ");    
  }
  hashtable_delete(table, itemdelete);
  return 0;
}

/* ***************************
 * This function cleans a query by checking if it is formatted correctly.
 * Then, it stores each word into an array of strings.
 * It will return true if the query is valid, false otherwise. 
 */
static bool line_clean(char** words, char* buffer, int* wc)
{
  int word_count = 0;
  bool word_start = true;
  bool cont = true;
  for (int i = 0; buffer[i] != '\0'; i++) { //loops till end of buffer
    if (!isalpha(buffer[i]) && !isspace(buffer[i])) { //the query needs to be all letters or spaces. 
      fprintf(stderr, "Error: bad character '%c' in query.\n", buffer[i]);
      cont = false;
      return cont;
    }
  }
  if (cont) {
    for (int i = 0; buffer[i] != '\0'; i++) { //after passing this first check, loops again
      if (isalpha(buffer[i])) { //converts all letters to lowercase
        buffer[i] = tolower(buffer[i]);
        if (word_start) {
          word_start = false; //for starts of words, in words array of strings, have it point to the first char of word
          words[word_count] = &buffer[i];
          word_count += 1;
	}
      } else if (isspace(buffer[i])) {
        word_start = true; //a space marks the end of a word. replaces with null to have pointer in words stop at the space
        buffer[i] = '\0';
      }
    }
    if (word_count > 1) { //if there are more than 1 words
      if (strcmp(words[0], "and") == 0 || strcmp(words[word_count - 1], "and") == 0) { //'and' and 'or' cannot be at the start or end
        fprintf(stderr, "Error: 'and' cannot be first or last\n");
        cont = false;
	return cont;
      } else if (strcmp(words[0], "or") == 0 || strcmp(words[word_count - 1], "or") == 0) {
        fprintf(stderr, "Error: 'or' cannot be first or last\n");
        cont = false;
	return cont;
      }
      for (int i = 0; i < word_count - 1; i++) { //'and' and 'or' cannot be adjacent to each other
        if (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) {
          if (strcmp(words[i + 1], "and") == 0 || strcmp(words[i + 1], "or") == 0) {
            fprintf(stderr, "Error: 'and' and 'or' cannot be adjacent or repeated.\n");
            cont = false;
	    return cont;
          }
        }
      }
    } else if (word_count == 1) { //'and' or 'or' cannot be the only word in the query
      if (strcmp(words[0], "and") == 0 || strcmp(words[0], "or") == 0) {
        fprintf(stderr, "Error: 'and' cannot be first or last\n");
        cont = false;
	return cont;
      }
    } else { //no words, empty query. we want to do nothing with an empty query
      cont = false;
      return cont;
    }
  }
  *wc = word_count; //stores word count for later use. 
  return cont;
}
  
/* ***************************
 * This will load the index file into a hashtable. 
 * Per the assumptions given in the assignment, assumes the indexFilename is formatted correctly.
 * Format found in indexers REQUIREMENTS.md: word docID count docID count ...
 */
static bool index_loader(hashtable_t* ht, char* filename)
{
  FILE* fp = fopen(filename, "r");
  char word[20];
  char* line;
  while ((line = file_readLine(fp)) != NULL) { //for each line in the index file
    int loc = 0;
    int docID; 
    int cnt;
    bool test1 = true;
    counters_t* ctrs = counters_new(); //we wanna make a new counters for a word
    if (ctrs == NULL) {
      mem_free(line);
      fprintf(stderr, "Fatal error: counters_new failed.\n");
      fclose(fp);
      return false;
    }
    sscanf(line, "%s %n", word, &loc); //scan the word and remember how many characters read
    char* curr = line + loc; //shifts the line by the amount of characters read
    while (sscanf(curr, "%d %d %n", &docID, &cnt, &loc) == 2) { //while there are pairs of docID / scores to read
      curr += loc; //we wanna move the line for the next scan
      test1 = counters_set(ctrs, docID, cnt); //set the counters
      if (!test1) { //if this didn't work, indexFilename isn't formatted correctly.
        fprintf(stderr, "indexFilename formatted incorrectly.\n");
	mem_free(line);
	fclose(fp);
	counters_delete(ctrs);
	return test1;
      }
    }
    test1 = hashtable_insert(ht, word, ctrs); //insert it to the hashtable
    if (!test1) { //same deal, just safety checks
      fprintf(stderr, "indexFilename formatted incorrectly.\n");
      mem_free(line);
      fclose(fp);
      counters_delete(ctrs);
      return test1;
    }
    mem_free(line);
  }
  fclose(fp);
  return true;
}
  
/* ***************************
 * Runs counters_iterate with a merge helper. Merges two counteres.
 */
static void ctrs_merge(counters_t* ctrsA, counters_t* ctrsB)
{
  counters_iterate(ctrsB, ctrsA, ctrs_merge_helper);
}

/* ***************************
 * Merges two counters by going through all docID/score pairs in one, and checking in the other.
 */
static void ctrs_merge_helper(void* arg, const int docID, const int score)
{
  counters_t* ctrsA = arg;
  int scoreB = score;

  int scoreA = counters_get(ctrsA, docID);
  if (scoreA == 0) { //curr score does not exist in ctrsA. add it there. 
    counters_set(ctrsA, docID, scoreB);
  } else { //if it does, combine their two values and store in ctrsA. 
    scoreA += scoreB;
    counters_set(ctrsA, docID, scoreA);
  }
}

/* ***************************
 *Runs counters_iterate with an intersect helper. 
 */
static void ctrs_intersect(counters_t* result, counters_t* ctrsA, counters_t* ctrsB)
{
  intersect_arg_t* arg = mem_assert(malloc(sizeof(intersect_arg_t)), "intersect arg");
  arg->result = result; //this struct is used so we can pass two ctrs into arg. we wanna put the intersect of A and B into result.
  arg->ctrsB = ctrsB;

  counters_iterate(ctrsA, arg, ctrs_intersect_helper);
  free(arg);
}

/* ***************************
 *This helper finds the minimum score given a key in ctrsA and B and sets the min in result.
 */
static void ctrs_intersect_helper(void* arg, const int docID, const int score)
{
  intersect_arg_t* data = arg;
  counters_t* result = data->result;
  counters_t* ctrsB = data->ctrsB;

  int valA = score;
  int valB = counters_get(ctrsB, docID);

  if (valB != 0) { //if it exists in both ctrs, grabs the smaller number and sets the doc and min in result
    int min = valA;
    if (valB < min) {
      min = valB;
    }
    counters_set(result, docID, min);	
  }
}

/* ***************************
 * BNF functionality for the given BNF in the instructions.
 * First, runs a loop to collect andsequences, which are the intersections of words adjacent to each other separated by 'or'.
 * Puts all of the andsequences into an array of counters.
 * Then, for 'or' functionality, goes through this array of counters. and stores the union of each doc id in a new counters struct.
 * Returns this new counters struct, with the scores for the query for each docID.
 * NOTE: and has precedence over or. 
 */
static counters_t* bnf(hashtable_t* index, char* words[], int word_count)
{
  counters_t** andargs = mem_malloc(sizeof(counters_t*) * word_count); //array of counters i will use to collect andsequences
  int andarg_count = 0; //number of andsequences
  counters_t* curr = NULL; //running count for an and sequence
  
  for (int i = 0; i < word_count; i++) { //for each word
    if (strcmp(words[i], "or") != 0) { //if it is or, skip.
      if (strcmp(words[i], "and") != 0) { //if it is and, skip. and is implied. 
        if (i == 0 || strcmp(words[i-1], "or") == 0) { //if it is the start of the words or or was the previous words,
          curr = counters_new(); //we need to create a new running count. 
	  counters_t* in = hashtable_find(index, words[i]); //if the word is in the index, starts running count with all of its values
	  if (in != NULL) {
            ctrs_merge(curr, in);
	  }
        } else { //otherwise, there is already a running count for it
	  counters_t* in = hashtable_find(index, words[i]); //grab it
          if (in != NULL && curr != NULL) { //if it is found, and if curr isnt null
	    counters_t* intersect = counters_new(); //create the intersection and set it to curr
	    ctrs_intersect(intersect, curr, in);
	    counters_delete(curr);
	    curr = intersect;
	  } else {
	    counters_delete(curr); //if a word is not found, the intersection for this andsequence must be 0. curr is set to null. 
	    curr = NULL;
	  }
	}	  
      }
    } else { //once hit 'or', add the andsequence to the array and increment the count. reset curr. 
      if (curr != NULL) {	    
        andargs[andarg_count] = curr;
        andarg_count += 1;
	curr = NULL;
      }
    }
  }
  if (curr != NULL) { //for the final andsequence, if curr isn't null
    andargs[andarg_count] = curr;
    andarg_count += 1;
  }

  counters_t* res = counters_new(); //now, we can do the 'or' functionality. everything left must be 'or', so we get union of everything.
  for (int i = 0; i < andarg_count; i++) {
    ctrs_merge(res, andargs[i]);
    counters_delete(andargs[i]);
  }
  mem_free(andargs);
  return res; //return the union of all of the andsequences.
}

/* ***************************
 * Function that prints the scores in descending order, ignoring when the score is 0. 
 * First, finds the largest possible score. 
 * Then, runs a loop in descending order starting with that score.
 */
static void print_max(counters_t* ctrs, const char* pageDirectory) 
{
  int curr_max = 0;
  counters_iterate(ctrs, &curr_max, find_max); //stores highest score
  if (curr_max == 0) { //highest score 0 means no documents match.
    printf("No documents match.\n");
  }
  for (int i = curr_max; i > 0; i--) { //goes in descending order
    doc_score_pair_t* pair = mem_malloc(sizeof(doc_score_pair_t)); //struct to save docID, score, and counters
    pair->docID = 0;
    pair->score = i; //searches for this score
    pair->ctrs = ctrs; 
    counters_iterate(ctrs, pair, print_curr_max); //runs helper to find the closest match to score, saves docID
    if (pair->docID != 0) { //if it found a match, we wanna first print it with the url, and then look for all other matches before descending
      while (pair->docID != 0) {
        char url[200];
	snprintf(url, sizeof(url), "%s/%d", pageDirectory, pair->docID); //builds path
        FILE* fp;
	fp = fopen(url, "r");
	char* line = file_readLine(fp); //first line is url, read it
	printf(" %s", line); //prints the url
        printf("\n");
	pair->docID = 0;
	mem_free(line);
	fclose(fp);
	counters_iterate(ctrs, pair, print_curr_max); //run iterate with same score
      }
    }
    mem_free(pair);
  }
}

/* ***************************
 * Simple helper to find the largest score
 */
static void find_max(void* arg, const int docID, const int score)
{
  int* curr_max = arg;
  if (docID != 0) { //safety check
    if (score > *curr_max) { 
      *curr_max = score; //stores largest score
    }
  }
}

/* ***************************
 * Helper to print score doc pairs, and to find the closest matching score with docID.
 */
static void print_curr_max(void* arg, const int docID, const int score)
{
  doc_score_pair_t* pair = arg;
  if (pair->docID == 0) { //indicator. if docID isn't 0, this means we found a pair already.
    if (score == pair->score) { //if matching score
      printf("score  %d  doc  %d:", score, docID); //print in format
      pair->docID = docID; //saves the docID
      counters_set(pair->ctrs, docID, 0); //sets score to 0 so it isn't checked again
    }
  }
}

/* ***************************
 * Deletes a counters table in the hashtable of words -> counters
 */
static void itemdelete(void* item) 
{
  if (item != NULL) {
    counters_delete(item);
  }
}





