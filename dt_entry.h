//// /////////////////////////////////////////////////
 // Created by Rodion "rodde" Efremov on 11.6.2019.//
///////////////////////////////////////////////// ////

#ifndef LINUX_C_DT_DT_ENTRY_H
#define LINUX_C_DT_DT_ENTRY_H
#include <stddef.h>

typedef struct dt_entry {
    char* m_tag;
    char* m_dir;
} dt_entry;

// Constructs the directory tagger entry.
void dt_entry_construct(dt_entry* entry, char* tag, char* dir);
dt_entry* dt_entry_alloc(char* tag, char* dir);
char* dt_entry_get_tag(const dt_entry* entry);
char* dt_entry_get_dir(const dt_entry* entry);
void dt_entry_set_tag(dt_entry* entry, char* tag);
void dt_entry_set_dir(dt_entry* entry, char* dir);
size_t dt_entry_levenshtein_distance(const dt_entry* entry, const char* str);
void dt_entry_destruct(dt_entry* entry);
void dt_entry_free(dt_entry** entry);
void dt_entry_test();


#endif //LINUX_C_DT_DT_ENTRY_H
