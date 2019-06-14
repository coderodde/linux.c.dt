//// /////////////////////////////////////////////////
 // Created by Rodion "rodde" Efremov on 11.6.2019.//
///////////////////////////////////////////////// ////

#ifndef LINUX_C_DT_DT_ENTRY_H
#define LINUX_C_DT_DT_ENTRY_H
#include <stddef.h>

/************************************************************
* dt_entry always keeps its own copies of both the strings. *
************************************************************/

typedef struct dt_entry {
    char* m_tag;
    char* m_dir;
} dt_entry;

/* Constructs the directory tagger entry: */
void dt_entry_construct(dt_entry* entry, const char* tag, const char* dir);

/* Allocates a new entry and consturcts it.: */
dt_entry* dt_entry_alloc(const char* tag, const char* dir);

/* Gets the copy of the tag string: */
char* dt_entry_get_tag(const dt_entry* entry);

/* Gets the copy of the directory string: */
char* dt_entry_get_dir(const dt_entry* entry);

/* Sets the new directory string. This entry will have its own copy */
/* of the string: */
void dt_entry_set_dir(dt_entry *const entry, const char* dir);

/* Computes the levenshtein distance between the directory and the pattern: */
size_t dt_entry_levenshtein_distance(const dt_entry* entry, const char* str);

/* Releases all the resources held by this entry: */
void dt_entry_destruct(dt_entry* entry);

/* Destructs the entry and frees its memory: */
void dt_entry_free(dt_entry** entry);

/* Runs the unit tests: */
void dt_entry_test();


#endif //LINUX_C_DT_DT_ENTRY_H
