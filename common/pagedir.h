#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdbool.h>
#include "../libcs50/webpage.h"

/* pagedir_init
 * Mark the given directory as a crawler-produced pageDirectory by
 * creating a '.crawler' file inside it.
 * Returns true on success, false on any error (e.g., directory not writable).
 */
bool pagedir_init(const char* pageDirectory);

/* pagedir_save
 * Save the given webpage to a file named by docID in the given directory.
 * File format:
 *   line 1: URL
 *   line 2: depth (integer)
 *   remaining lines: HTML of the page
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H