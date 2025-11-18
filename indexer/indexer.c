/* indexer.c 
 * CS50 FA25 Final Project 'indexer' Module
 * Author: Cindy Jiayi Liu, Group 1
 * Date: 2025.11.15
 */ 

 // declarations

 #include <stdio.h>
 #include <stdlib.h>
 #include "../libcs50/webpage.h"
 #include "../common/index.h"
 #include "../common/pagedir.h"
 #include "../common/word.h"
 
 static index_t* indexBuild(const char* pageDirectory);
 static void indexPage(webpage_t* page, index_t* index, int docID);
 static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename);
 
 int main(int argc, char* argv[]) {
   char *pageDirectory, *indexFilename;
 
   // Parse the command line
   parseArgs(argc, argv, &pageDirectory, &indexFilename);
 
   // Validate pageDirectory
   if (!pagedir_validate(pageDirectory)) {
     fprintf(stderr, "Invalid page directory '%s'.\n", pageDirectory);
     return 2;
   }
 
   // Build the index from the pages in the pageDirectory
   index_t* index = indexBuild(pageDirectory);
   if (index == NULL) {
     fprintf(stderr, "Failed to create an index.\n");
     return 3;
   }
 
   // Create (if doesn't exist) or open (overwrite) the index file for writing
   FILE* fp = fopen(indexFilename, "w");
   if (fp == NULL) {
     fprintf(stderr, "Cannot open file '%s' for writing.\n", indexFilename);
     index_delete(index);
     return 4;
   }
   // Save the index
   index_save(index, fp);
   fclose(fp);
 
   // Cleanup
   index_delete(index);
   printf("Indexer has successfully completed.\n");
 
   return 0;
 }
 
 // Function to parse the command line
 static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename) {
   // invalid usage
   if (argc != 3) {
     fprintf(stderr, "Usage: %s pageDirectory indexFilename\n", argv[0]);
     exit(1);
   }
   *pageDirectory = argv[1];
   *indexFilename = argv[2];
 }
 
 // Build an in-memory index from webpage files it finds in the pageDirectory
 static index_t* indexBuild(const char* pageDirectory) {
   // Create a new index with an initial capacity for 500 entries
   index_t* index = index_new(500);
   if (index == NULL) {
     // Handle allocation error
     fprintf(stderr, "Error: Could not create index.\n");
     return NULL;
   }
   int docID_new = 1;    // Document ID starts from 1
   // Continuously read documents from pageDirectory
   while(true) {
     webpage_t* webpageNew = pagedir_load(pageDirectory, docID_new);
     if(webpageNew == NULL) {
         break;  // Exit loop if no more pages are found
     }
     // Process the loaded webpage
     indexPage(webpageNew, index, docID_new);
     docID_new += 1;
     // Free the allocated memory for the loaded webpage
     webpage_delete(webpageNew);
   }
 
   return index;
 }
 
 // Scan a webpage document to add its words to the index
 static void indexPage(webpage_t* page, index_t* index, int docID) {
   int pos = 0;
   char* word;
 
   // Extract words from the webpage
   while ((word = webpage_getNextWord(page, &pos)) != NULL) {
     // Only consider words that have 3 or more characters
     if (strlen(word) >= 3) {
       // Normalize the word and add it to index
       char* normalizedWord = NormalizeWord(word);
       index_add(index, normalizedWord, docID);
       free(normalizedWord);
     }
     free(word);
   }
 }
 