#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../libcs50/webpage.h"
#include "pagedir.h"

/**************** local helper ****************/
/* buildPath: 
 * Allocate and return a new string "pageDirectory/docIDorName".
 * Caller is responsible for freeing the result.
 */
static char*
buildPath(const char* pageDirectory, const char* name)
{
    size_t lenDir  = strlen(pageDirectory);
    size_t lenName = strlen(name);

    // space for dir + '/' + name + '\0'
    size_t len = lenDir + 1 + lenName + 1;
    char* path = malloc(len);
    if (path == NULL) {
        return NULL;
    }

    // always insert '/'
    snprintf(path, len, "%s/%s", pageDirectory, name);
    return path;
}

/**************** pagedir_init ****************/
bool
pagedir_init(const char* pageDirectory)
{
    if (pageDirectory == NULL) { // null argument 
        return false;
    }

    char* crawlerPath = buildPath(pageDirectory, ".crawler");
    if (crawlerPath == NULL) { // null path 
        return false;
    }

    FILE* fp = fopen(crawlerPath, "w");
    if (fp == NULL) { // null file (cannot open)
        free(crawlerPath);
        return false;
    }

    fclose(fp);
    free(crawlerPath);
    return true;
}

/**************** pagedir_save ****************/
void
pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
    if (page == NULL || pageDirectory == NULL) { // null arguments 
        return;
    }

    char docName[20];
    snprintf(docName, sizeof(docName), "%d", docID);

    char* pagePath = buildPath(pageDirectory, docName);
    if (pagePath == NULL) {
        fprintf(stderr, "Error: could not allocate filepath for docID %d\n", docID);
        return;
    }

    FILE* fp = fopen(pagePath, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open file '%s' for writing\n", pagePath);
        free(pagePath);
        return;
    }

    // line 1: URL
    fprintf(fp, "%s\n", webpage_getURL(page));
    // line 2: depth
    fprintf(fp, "%d\n", webpage_getDepth(page));
    // remaining lines: HTML
    fprintf(fp, "%s\n", webpage_getHTML(page));

    fclose(fp);
    free(pagePath);
}


bool
pagedir_validate(const char* pageDirectory)
{
    if (pageDirectory == NULL) {
        return false;
    }

    char* crawlerPath = buildPath(pageDirectory, ".crawler");
    if (crawlerPath == NULL) {
        return false;
    }

    FILE* fp = fopen(crawlerPath, "r");
    free(crawlerPath);

    if (fp == NULL) {
        return false;
    }

    fclose(fp);
    return true;
}

webpage_t*
pagedir_load(const char* pageDirectory, const int docID)
{
    if (pageDirectory == NULL || docID < 1) {
        return NULL;
    }

    char docName[20];
    snprintf(docName, sizeof(docName), "%d", docID);

    char* pagePath = buildPath(pageDirectory, docName);
    if (pagePath == NULL) {
        return NULL;
    }

    FILE* fp = fopen(pagePath, "r");
    free(pagePath);

    if (fp == NULL) {
        return NULL;
    }

    char* url = NULL;
    size_t len = 0;
    ssize_t read = getline(&url, &len, fp);
    if (read <= 0) {
        fclose(fp);
        return NULL;
    }
    if (url[read - 1] == '\n') {
        url[read - 1] = '\0';
    }

    int depth;
    if (fscanf(fp, "%d\n", &depth) != 1) {
        free(url);
        fclose(fp);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    getline(&url, &len, fp); 
    char depthLine[20];
    fgets(depthLine, sizeof(depthLine), fp);  
    long htmlStart = ftell(fp);
    long htmlSize = fileSize - htmlStart;
    
    char* html = malloc(htmlSize + 1);
    if (html == NULL) {
        free(url);
        fclose(fp);
        return NULL;
    }
    
    size_t bytesRead = fread(html, 1, htmlSize, fp);
    html[bytesRead] = '\0';
    
    fclose(fp);

    webpage_t* page = webpage_new(url, depth, html);
    if (page == NULL) {
        free(url);
        free(html);
        return NULL;
    }

    return page;
}
