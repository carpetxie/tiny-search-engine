
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "word.h"
#include "../libcs50/mem.h"

char* NormalizeWord(const char* word)
{
  if (word == NULL) {
    return NULL;
  }

  char* normalized = mem_malloc(strlen(word) + 1);
  for (int i = 0; word[i] != '\0'; i++) {
    normalized[i] = tolower(word[i]);
  }
  normalized[strlen(word)] = '\0';

  return normalized;
}

