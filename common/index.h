#ifndef __INDEX_H
#define __INDEX_H

#include <stdio.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"

typedef hashtable_t index_t;

index_t* index_new(const int num_slots);


void index_add(index_t* index, const char* word, const int docID);

void index_save(index_t* index, FILE* fp);

index_t* index_load(FILE* fp);

void index_delete(index_t* index);

#endif

