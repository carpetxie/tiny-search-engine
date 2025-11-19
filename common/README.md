# CS50 Final Project
## CS50 Fall 2025

### common 

The common directory contains shared modules used by several components of the Tiny Search Engine. The only module implemented in this assignment is pagedir, which supports the crawler by validating page directories and saving webpage files. 

Below are the assumptions made during implementation, along with any differences from the TSE specifications and any known limintations. 

### Assumptions 

* The caller (e.g., crawler) passes in a directory that already exists in the filesystem. `pagedir_init` only checks for writability and creates the `.crawler` file. 
* The directory name passed to `pagedir_init` ends with a trailing slash. This is consistent with the provided test scripts. 
* The caller is responsible for freeing the dynamically allocated strings returned by `pagedir_construct_path`. 

### Differences from Spec 

* `pagedir_construct_path` allocates exactly enough memory for the combined path without inserting an extra `/`. It assumes the caller includes the slash if needed. 
* Errors are handled by returning `false` rather than printing errors, which keeps behavior simple and leaves message handling to the caller. 
* Ordering/Formatting of the output files matches the crawler specification, but no additional validation is done on the webpage contents.

### Known Limitations 

* The module does not verify that `.crawler` already exists for later components, it only creates it for the crawler. 
* If the caller provides a directory without a trailing slash, filepaths may concatenate incorrectly. 
* `pagedir_save` overwrites existing files with the same docID without warning. 
* No attempt is made to sanitize filenames