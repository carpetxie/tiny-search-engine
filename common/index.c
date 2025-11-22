
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"

index_t* index_new(const int num_slots)
{
  return hashtable_new(num_slots);
}

void index_add(index_t* index, const char* word, const int docID)
{
  if (index == NULL || word == NULL || docID < 1) {
    return;
  }

  counters_t* ctrs = hashtable_find(index, word);
  if (ctrs == NULL) {
    ctrs = counters_new();
    hashtable_insert(index, word, ctrs);
  }
  counters_add(ctrs, docID);
}

static void save_helper(void* arg, const int docID, const int count)
{
  FILE* fp = arg;
  fprintf(fp, " %d %d", docID, count);
}

static void save_word(void* arg, const char* key, void* item)
{
  FILE* fp = arg;
  counters_t* ctrs = item;
  fprintf(fp, "%s", key);
  counters_iterate(ctrs, fp, save_helper);
  fprintf(fp, "\n");
}

void index_save(index_t* index, FILE* fp)
{
  if (index == NULL || fp == NULL) {
    return;
  }
  hashtable_iterate(index, fp, save_word);
}

index_t* index_load(FILE* fp)
{
  if (fp == NULL) {
    return NULL;
  }

  int num_lines = file_numLines(fp);
  index_t* index = index_new(num_lines);
  if (index == NULL) {
    return NULL;
  }

  char* line;
  while ((line = file_readLine(fp)) != NULL) {
    char word[100];
    int pos = 0;
    int docID, count;

    if (sscanf(line, "%s%n", word, &pos) == 1) {
      char* rest = line + pos;
      while (sscanf(rest, "%d %d%n", &docID, &count, &pos) == 2) {
        counters_t* ctrs = hashtable_find(index, word);
        if (ctrs == NULL) {
          ctrs = counters_new();
          hashtable_insert(index, word, ctrs);
        }
        counters_set(ctrs, docID, count);
        rest += pos;
      }
    }
    mem_free(line);
  }
  return index;
}

static void delete_helper(void* item)
{
  counters_t* ctrs = item;
  if (ctrs != NULL) {
    counters_delete(ctrs);
  }
}

void index_delete(index_t* index)
{
  if (index != NULL) {
    hashtable_delete(index, delete_helper);
  }
}

