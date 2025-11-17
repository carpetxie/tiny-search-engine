#include <stdio.h>
#include <stdlib.h>
#include "../indexer/indexer.c"
#include "../common/common.c"
#include "../crawler/crawler.c"
#include "../libcs50/hashtable.h"
#include "../libcs50/file.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void index_loader(hashtable_t* ht);

int main(int argc, char *argv[]) 
{
  if (argc != 3) {
    fprintf(stderr, "Invalid parameters.\n");
    return 1;
  }

  //TODO: validate that the two parameters are outputs produced by crawler & indexer
  
  //load the index from indexFilename into an internal data structure
  int index_lines = file_numLines(argv[2]);
  hashtable_t* table = hashtable_new(index_lines);
  index_loader(table, argv[2]);
  //TODO: Return here

  //Read search queries from stdin
  char buffer[200];
  char* words[100];
  int word_count = 0;
  bool word_start = true;
  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    for (int i = 0; i < sizeof(buffer); i++) {
      if (!isalpha(buffer[i] || !isspace(buffer[i]) || buffer[i] != NULL)) {
        fprintf(stderr, "Error: bad character '%c' in query.\n", buffer[i]);
      } else {
        if (isalpha(buffer[i])) {
	  buffer[i] = tolower(buffer[i]);
          if (word_start) {
            word_start = false;
	    words[word_count] = buffer[i];
	    word_count += 1;
	  }
	} else if (isspace(buffer[i])) {
          word_start = true;
	  buffer[i] = '\0';
	}
      }
      if (strcmp(words[0], "and") == 0 || strcmp(words[word_count], "and") == 0) {
        fprintf(stderr, "Error: 'and' cannot be first or last\n");
      } else if (strcmp(words[0], "or") == 0 || strcmp(words[word_count], "or") == 0) {
        fprintf(stderr, "Error: 'or' cannot be first or last\n");
      }
      for (int i = 0; i < word_count - 1; i++) {
        if (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) {
          if (strcmp(words[i + 1], "and") == 0 || strcmp(words[i + 1], "or") == 0) {
            fprintf(stderr, "Error: 'and' and 'or' cannot be adjacent or repeated.\n");
	  }
	}
      }
    }      
  

  return 0;
}

void index_loader(hashtable_t* ht, char* filename)
{
  printf("hi");
}
  



