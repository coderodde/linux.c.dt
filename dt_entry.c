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
    size_t len = strlen(entry->m_tag);
    char* rv = malloc(len + 1);
    memcpy(rv, entry->m_tag, len + 1);
    return rv;
}

char* dt_entry_get_dir(const dt_entry* entry)
{
    size_t len = strlen(entry->m_dir);
    char* rv = malloc(len + 1);
    memcpy(rv, entry->m_dir, len + 1);
    return rv;
}

void dt_entry_set_tag(dt_entry *const entry, const char* tag)
{
    size_t len = strlen(tag) + 1;
    entry->m_tag = malloc(len);
    memcpy(entry->m_tag, tag, len);
}

void dt_entry_set_dir(dt_entry *const entry, const char* dir)
{
    size_t len = strlen(dir) + 1;
    free(entry->m_dir);
    entry->m_dir = malloc(len);
    memcpy(entry->m_dir, dir, len);
}

// str1 columnwise, str2 row-wise:
static size_t lev_distance_impl(const char* str1,
                                const char* str2,
                                size_t len1,
                                size_t len2)
{
    size_t** dist = malloc((len1 + 1) * sizeof(size_t*));
    size_t i;
    size_t j;
    size_t i1;
    size_t i2;
    size_t cost;

    for (i = 0; i <= len1; i++)
        dist[i] = malloc((len2 + 1) * sizeof(size_t));

    for (i = 0; i <= len1; i++)
        dist[i][0] = i;

    for (i = 1; i <= len2; i++)
        dist[0][i] = i;

    for (i1 = 1; i1 <= len1; i1++) {
        for (i2 = 1; i2 <= len2; i2++) {
            if (str1[i1 - 1] == str2[i2 - 1])
                cost = 0;
            else
                cost = 1;

            dist[i1][i2] = MIN(MIN(dist[i1 - 1][i2] + 1,
                                   dist[i1][i2 - 1] + 1),
                             dist[i1 - 1][i2 - 1] + cost);
        }
    }

    return dist[len1][len2];
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

