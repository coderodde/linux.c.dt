//// /////////////////////////////////
 // Created by rodde on 11.6.2019. //
///////////////////////////////// ////

#ifndef LINUX_C_DT_DT_ENTRY_LIST_H
#define LINUX_C_DT_DT_ENTRY_LIST_H
#include "dt_entry.h"
#include <stdio.h>

typedef struct dt_entry_list {
    size_t m_size;
    size_t m_capacity;
    dt_entry** m_entries;
} dt_entry_list;

// Constructs the entry list:
void dt_entry_list_construct(dt_entry_list*);

// Allocates the entry list and constructs it via 'dt_entry_list_construct':
dt_entry_list* dt_entry_list_alloc();

// Destructs the entry list:
void dt_entry_list_destruct(dt_entry_list*);

// Destructs the entry list and frees the memory:
void dt_entry_list_free(dt_entry_list**);

// Returns the number of directory tags stored in the given list:
size_t dt_entry_list_size(const dt_entry_list* list);

// Appends the given entry to the given entry list:
void dt_entry_list_append_entry(dt_entry_list* list,
                                const char* tag,
                                const char* dir);

// Returns the ith directory tag:
dt_entry* dt_entry_list_get(const dt_entry_list* list, const size_t i);

// Reads the contents of the text file represented by 'file' into
// the argument entry list:
int dt_entry_list_read_from_file(dt_entry_list* list, FILE* file);

// Writes the entire content of the entry list to the 'file':
int dt_entry_list_write_to_file(const dt_entry_list* list, FILE* file);

// Performs an approximating search for an entry. If there is exact match,
// the first entry with minimum Levenshtein distance will be returned:
dt_entry* dt_entry_list_match(const dt_entry_list*, const char* pattern);

// Sorts the entry list by tags:
void dt_entry_list_sort_by_tags(dt_entry_list* list);

// Sorts the entry list by directory names:
void dt_entry_list_sort_by_dirs(dt_entry_list* list);

// Clones the given tag list:
void dt_entry_list_append(dt_entry_list* list, dt_entry_list* clone);

// Runs the unit tests:
void dt_entry_list_test();

#endif //LINUX_C_DT_DT_ENTRY_LIST_H
