/* indextest.c 
 * CS50 FA25 Final Project 'indexer' Module
 * Author: Cindy Jiayi Liu, Group 1
 * Date: 2025.11.15
 */ 

#include <stdio.h>
#include <stdlib.h>
#include "../common/index.h" 

static void loadAndSaveIndex(const char* oldIndexFilename, const char* newIndexFilename);

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s oldIndexFilename newIndexFilename\n", argv[0]);
    return 1;
  }
  const char* oldIndexFilename = argv[1];
  const char* newIndexFilename = argv[2];

  // Load the old index from file and save it into a new file
  loadAndSaveIndex(oldIndexFilename, newIndexFilename);

  return 0;
}

// Function to load the index from a file and then save it to a new file
static void loadAndSaveIndex(const char* oldIndexFilename, const char* newIndexFilename) {
  // Open the old index file for reading
  FILE* oldFile = fopen(oldIndexFilename, "r");
  if (oldFile == NULL) {
    fprintf(stderr, "Error: Could not open %s for reading\n", oldIndexFilename);
    exit(2);
  }

  // Create and load the index from the old file
  index_t* index = index_load(oldFile);  // Assumes index_load() function exists
  fclose(oldFile);

  if (index == NULL) {
    fprintf(stderr, "Error: Failed to load index from %s\n", oldIndexFilename);
    exit(3);
  }

  // Open the new index file for writing
  FILE* newFile = fopen(newIndexFilename, "w");
  if (newFile == NULL) {
    fprintf(stderr, "Error: Could not open %s for writing\n", newIndexFilename);
    index_delete(index); // Clean up index before exiting
    exit(4);
  }

  // Save the index to the new file
  index_save(index, newFile);  // Assumes index_save() function exists
  fclose(newFile);

  // Clean up the index
  index_delete(index);
  printf("Successfully loaded index from %s and saved it to %s\n", oldIndexFilename, newIndexFilename);
}
