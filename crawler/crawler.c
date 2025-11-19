#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../libcs50/webpage.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/bag.h"
#include "../common/pagedir.h"

/**************** function prototypes ****************/
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

/**************** main ****************/
int main(const int argc, char* argv[])
{
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;

    // will exit non-zero on error
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

    crawl(seedURL, pageDirectory, maxDepth);

    // seedURL is owned by the webpage_t created in crawl and is freed there
    return 0;
}

/**************** parseArgs ****************/
/* Given command-line arguments, extract seedURL, pageDirectory, maxDepth.
 * On any error, print a message to stderr and exit non-zero.
 * Only returns if arguments are valid.
 */
static void
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth)
{
    // check number of args
    if (argc != 4) {
        fprintf(stderr, "Usage: %s seedURL pageDirectory maxDepth\n", argv[0]);
        exit(1);
    }

    char* rawURL = argv[1];
    char* dir    = argv[2];
    char* depthStr = argv[3];

    // normalize URL and validate it is internal
    char* normURL = normalizeURL(rawURL);
    if (normURL == NULL) {
        fprintf(stderr, "Error: unable to normalize URL '%s'\n", rawURL);
        exit(1);
    }

    if (!isInternalURL(normURL)) {
        fprintf(stderr, "Error: seedURL '%s' is not internal\n", normURL);
        free(normURL);
        exit(1);
    }

    // initialize pageDirectory (.crawler file)
    if (!pagedir_init(dir)) {
        fprintf(stderr, "Error: pageDirectory '%s' is not a writable directory\n", dir);
        free(normURL);
        exit(1);
    }

    // parse maxDepth
    int depth;
    if (sscanf(depthStr, "%d", &depth) != 1) {
        fprintf(stderr, "Error: maxDepth '%s' is not an integer\n", depthStr);
        free(normURL);
        exit(1);
    }
    if (depth < 0 || depth > 10) {
        fprintf(stderr, "Error: maxDepth %d is not in [0,10]\n", depth);
        free(normURL);
        exit(1);
    }

    // success: fill out parameters
    *seedURL       = normURL;  // caller (via webpage) will free this
    *pageDirectory = dir;      // pointer into argv, no need to free
    *maxDepth      = depth;
}

/**************** crawl ****************/
/* Core crawler:
 * - initialize hashtable of seen URLs and bag of pages to crawl
 * - add seedURL as depth 0
 * - repeatedly fetch, save, and scan pages until bag is empty
 */
static void
crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
    // hashtable of seen URLs
    hashtable_t* pagesSeen = hashtable_new(200);
    if (pagesSeen == NULL) {
        fprintf(stderr, "Error: could not allocate pagesSeen hashtable\n");
        exit(2);
    }

    // record the seed URL as seen
    hashtable_insert(pagesSeen, seedURL, "");  // value is unused

    // bag of pages to crawl
    bag_t* pagesToCrawl = bag_new();
    if (pagesToCrawl == NULL) {
        fprintf(stderr, "Error: could not allocate pagesToCrawl bag\n");
        hashtable_delete(pagesSeen, NULL);
        exit(2);
    }

    // create first webpage and insert into bag
    webpage_t* seedPage = webpage_new(seedURL, 0, NULL);
    if (seedPage == NULL) {
        fprintf(stderr, "Error: could not allocate seed webpage\n");
        hashtable_delete(pagesSeen, NULL);
        bag_delete(pagesToCrawl, NULL);
        exit(2);
    }
    bag_insert(pagesToCrawl, seedPage);

    int docID = 1;
    webpage_t* page;

    // main crawl loop
    while ( (page = bag_extract(pagesToCrawl)) != NULL ) {

        if (webpage_fetch(page)) {
            // fetched successfully
            printf("Fetched: %s\n", webpage_getURL(page));

            // save the page to pageDirectory/docID
            pagedir_save(page, pageDirectory, docID);
            docID++;

            // if not at maxDepth, scan for links
            if (webpage_getDepth(page) < maxDepth) {
                printf("Scanning: %s\n", webpage_getURL(page));
                pageScan(page, pagesToCrawl, pagesSeen);
            }
        } else {
            fprintf(stderr, "Warning: failed to fetch %s\n", webpage_getURL(page));
        }

        webpage_delete(page); // frees URL and HTML
    }

    // clean up
    hashtable_delete(pagesSeen, NULL);      // values are string literals, don’t free
    bag_delete(pagesToCrawl, NULL);        // bag should now be empty
}

/**************** pageScan ****************/
/* Given a fetched webpage, scan for URLs.
 * For each internal URL not yet seen:
 *   - insert into hashtable pagesSeen
 *   - create a new webpage_t at depth+1 and insert into bag pagesToCrawl
 */
static void
pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    int pos = 0;
    char* rawURL;
    int nextDepth = webpage_getDepth(page) + 1;

    while ((rawURL = webpage_getNextURL(page, &pos)) != NULL) {
        // normalize rawURL without freeing it yet
        char* url = normalizeURL(rawURL);

        if (url == NULL) {
            printf("IgnUnNormal: %s\n", rawURL);
            free(rawURL);
            continue;
        }

        printf("Found: %s\n", url);

        if (!isInternalURL(url)) {
            printf("IgnExtrn: %s\n", url);
            free(url);
            free(rawURL);
            continue;
        }

        // internal URL
        if (hashtable_insert(pagesSeen, url, "")) {
            // url is new; create a webpage_t that takes ownership of `url`
            webpage_t* newPage = webpage_new(url, nextDepth, NULL);
            if (newPage != NULL) {
                bag_insert(pagesToCrawl, newPage);
                printf("Added: %s\n", url);
                // DO NOT free(url); webpage_delete(newPage) will free it later
            } else {
                // allocation failed; avoid leak
                free(url);
            }
        } 
        else {
            // duplicate URL
            printf("IgnDupl: %s\n", url);
            free(url);
        }

        free(rawURL);  // always safe to free here
    }
}