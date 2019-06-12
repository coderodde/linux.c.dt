//// /////////////////////////////////
 // Created by rodde on 11.6.2019. //
///////////////////////////////// ////
#include "dt_entry.h"
#include "my_assert.h"
#include <stddef.h> // NULL
#include <stdlib.h> // size_t
#include <string.h> // strlen

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

void dt_entry_construct(dt_entry* entry,
                        const char* tag,
                        const char* dir)
{
    size_t len;

    len = strlen(tag) + 1;
    entry->m_tag = malloc(len);
    memcpy(entry->m_tag, tag, len);

    len = strlen(dir) + 1;
    entry->m_dir = malloc(len);
    memcpy(entry->m_dir, dir, len);
}

dt_entry* dt_entry_alloc(const char* tag, const char* dir)
{
    dt_entry* e = malloc(sizeof(*e));
    dt_entry_construct(e, tag, dir);
    return e;
}

char* dt_entry_get_tag(const dt_entry* entry)
{
    return entry->m_tag;
}

char* dt_entry_get_dir(const dt_entry* entry)
{
    return entry->m_dir;
}

void dt_entry_set_tag(dt_entry* entry, char* tag)
{
    entry->m_tag = tag;
}

void dt_entry_set_dir(dt_entry* entry, char* dir)
{
    entry->m_dir = dir;
}

static size_t lev_distance_impl(const char* str1, const char* str2, size_t i1, size_t i2)
{
    size_t cost, tmp1, tmp2;

    if (i1 == 0) return i2;
    if (i2 == 0) return i1;

    if (str1[i1 - 1] == str2[i2 - 1])
        cost = 0;
    else
        cost = 1;

    tmp1 = MIN(lev_distance_impl(str1, str2, i1 - 1, i2) + 1,
               lev_distance_impl(str1, str2, i1, i2 - 1) + 1);

    tmp2 = lev_distance_impl(str1, str2, i1 - 1, i2 - 1) + cost;

    return MIN(tmp1, tmp2);
}

static size_t lev_distance(const char* str1, const char* str2)
{
    return lev_distance_impl(str1,
                             str2,
                             strlen(str1),
                             strlen(str2));
}

size_t dt_entry_levenshtein_distance(const dt_entry* entry, const char* str)
{
    return lev_distance(entry->m_tag, str);
}

void dt_entry_destruct(dt_entry* entry)
{
    free(entry->m_tag);
    free(entry->m_dir);
    entry->m_tag = NULL;
    entry->m_dir = NULL;
}

void dt_entry_free(dt_entry** entry)
{
    dt_entry_destruct(*entry);
    free(*entry);
    *entry = NULL;
}

static void dt_entry_test_levenshtein()
{
    dt_entry* de = dt_entry_alloc("abcd", "dir");
    ASSERT(dt_entry_levenshtein_distance(de, "bcd") == 1);
    ASSERT(dt_entry_levenshtein_distance(de, "1abcd") == 1);
    ASSERT(dt_entry_levenshtein_distance(de, "fbcd") == 1);
    ASSERT(dt_entry_levenshtein_distance(de, "abcd") == 0);
    ASSERT(dt_entry_levenshtein_distance(de, "xyzbcd") == 3);
    ASSERT(dt_entry_levenshtein_distance(de, "1b23")  == 3);
    ASSERT(dt_entry_levenshtein_distance(de, "agcdefjk") == 5);
    dt_entry_free(&de);
    ASSERT(de == NULL);
}

void dt_entry_test() {
    dt_entry_test_levenshtein();
}

