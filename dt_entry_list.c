//// /////////////////////////////////
 // Created by rodde on 11.6.2019. //
///////////////////////////////// ////

#include "dt_entry_list.h"
#include "dt_entry.h"
#include "my_assert.h"
#include <linux/limits.h>
#include <limits.h>
#include <stddef.h> // NULL, size_t
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // malloc, qsort
#include <string.h> // strcmp, mem
#define MAX_TAG_LENGTH 11

static const size_t DEFAULT_CAPACITY = 32;

typedef struct dt_entry_list {
    size_t m_size;
    size_t m_capacity;
    dt_entry** m_entries;
} dt_entry_list;


void dt_entry_list_construct(dt_entry_list* list)
{
    list->m_size = 0;
    list->m_capacity = DEFAULT_CAPACITY;
    // m_entries is an array of tag entry pointers:
    list->m_entries = malloc(DEFAULT_CAPACITY * sizeof(dt_entry*));
}

dt_entry_list* dt_entry_list_alloc()
{
    dt_entry_list* list = malloc(sizeof(*list));
    dt_entry_list_construct(list);
    return list;
}

void dt_entry_list_destruct(dt_entry_list* list)
{
    free(list->m_entries);
    list->m_entries = NULL;
    list->m_capacity = 0;
    list->m_size = 0;
}

void dt_entry_list_free(dt_entry_list** p_list)
{
    dt_entry_list_destruct(*p_list);
    *p_list = NULL;
}

size_t dt_entry_list_size(const dt_entry_list* list)
{
    return list->m_size;
}

dt_entry* dt_entry_list_get(const dt_entry_list* list, const size_t index)
{
    if (index >= list->m_size)
        return NULL;

    return list->m_entries[index];
}

static void dt_entry_list_ensure_capacity(dt_entry_list* list)
{
    size_t new_capacity;
    dt_entry** new_entries;

    if (list->m_size < list->m_capacity)
        return;

    new_capacity = list->m_capacity << 1;
    new_entries = malloc(new_capacity * sizeof(*new_entries));
    memcpy(new_entries, list->m_entries, list->m_size * sizeof(*new_entries));
}

void dt_entry_list_append_entry(dt_entry_list* list, dt_entry* entry)
{
    dt_entry_list_ensure_capacity(list);
    list->m_entries[list->m_size++] = entry;
}

dt_entry* dt_entry_list_match(const dt_entry_list* list, const char* pattern)
{
    size_t i;
    size_t best_cost = SIZE_MAX;
    size_t current_cost;
    dt_entry* best_entry = NULL;

    if (list->m_size == 0) return NULL;

    for (i = 0; i != list->m_size; i++) {
        current_cost = dt_entry_levenshtein_distance(list->m_entries[i], pattern);

        if (best_cost > current_cost) {
            best_cost = current_cost;
            best_entry = list->m_entries[i];

            if (best_cost == 0) {
                // Cannot improve the best cost since we have an exact match:
                return best_entry;
            }
        }
    }

    // We get here only if we don't have an exact match.
    // Returns the closest match:
    return best_entry;
}

int dt_entry_list_read(dt_entry_list* list, FILE* file)
{
    char tag[MAX_TAG_LENGTH];
    char dir[PATH_MAX];

    if (!list || !file)
        return EXIT_FAILURE;

    while (fscanf(file, "%s %s", tag, dir) != EOF)
        dt_entry_list_append_entry(list, dt_entry_alloc(tag, dir));

    return EXIT_SUCCESS;
}

int dt_entry_list_write(const dt_entry_list* list, FILE* file)
{
    dt_entry* e;
    size_t i;
    char* separator = "";

    for (i = 0; i != dt_entry_list_size(list); i++) {
        e = dt_entry_list_get(list, i);
        fprintf(file,
                "%s%s %s",
                separator,
                dt_entry_get_tag(e),
                dt_entry_get_dir(e));
        separator = "\n";
    }
}

static int tag_cmp(const void* a, const void* b)
{
    const dt_entry** ea = (const dt_entry**) a;
    const dt_entry** eb = (const dt_entry**) b;
    return strcmp(dt_entry_get_tag(*ea),
                  dt_entry_get_tag(*eb));
}

static int dir_cmp(const void* a, const void* b) {
    const dt_entry** ea = (const dt_entry**) a;
    const dt_entry** eb = (const dt_entry**) b;
    return strcmp(dt_entry_get_dir(*ea),
                  dt_entry_get_dir(*eb));
}

void dt_entry_list_sort_by_tags(dt_entry_list* list)
{
    qsort(list->m_entries, list->m_size, sizeof(dt_entry*), tag_cmp);
}

void dt_entry_list_sort_by_dirs(dt_entry_list* list)
{
    qsort(list->m_entries, list->m_size, sizeof(dt_entry*), dir_cmp);
}

dt_entry_list* dt_entry_list_clone(dt_entry_list* list)
{
    size_t i;
    dt_entry_list* clone = dt_entry_list_alloc();

    for (i = 0; i != list->m_size; i++)
        dt_entry_list_append_entry(clone, dt_entry_list_get(list, i));

    return clone;
}

static void dt_entry_list_construct_destruct_test()
{
    dt_entry_list* list = dt_entry_list_alloc();
    dt_entry_list_free(&list);
    ASSERT(list == NULL);
}

static void dt_entry_list_append_get_size_test()
{
    size_t i;

    dt_entry* entries[] = {
            dt_entry_alloc("A", "a"),
            dt_entry_alloc("B", "b"),
            dt_entry_alloc("C", "c"),
            dt_entry_alloc("D", "d"),
            dt_entry_alloc("E", "e"),
    };

    dt_entry_list list;
    dt_entry_list_construct(&list);

    for (i = 0; i < sizeof(entries) /
                    sizeof(entries[0]); i++) {
        ASSERT(dt_entry_list_size(&list) == i);
        dt_entry_list_append_entry(&list, entries[i]);
        ASSERT(dt_entry_list_size(&list) == i + 1);
    }

    ASSERT(dt_entry_list_get(&list, 0) == entries[0]);
    ASSERT(dt_entry_list_get(&list, 1) == entries[1]);
    ASSERT(dt_entry_list_get(&list, 2) == entries[2]);
    ASSERT(dt_entry_list_get(&list, 3) == entries[3]);
    ASSERT(dt_entry_list_get(&list, 4) == entries[4]);

    dt_entry_list_destruct(&list);
}

static void dt_entry_list_sort_test()
{
    size_t i;
    dt_entry* entries[] = {
            dt_entry_alloc("fd", "df"),
            dt_entry_alloc("at", "ta"),
            dt_entry_alloc("re", "er"),
            dt_entry_alloc("xz", "zx"),
            dt_entry_alloc("js", "sj"),
    };

    dt_entry_list list;
    dt_entry_list_construct(&list);

    for (i = 0; i < sizeof(entries) /
                    sizeof(entries[0]); i++) {
        dt_entry_list_append_entry(&list, entries[i]);
    }

    dt_entry_list_sort_by_tags(&list);

    ASSERT(dt_entry_list_get(&list, 0) == entries[1]);
    ASSERT(dt_entry_list_get(&list, 1) == entries[0]);
    ASSERT(dt_entry_list_get(&list, 2) == entries[4]);
    ASSERT(dt_entry_list_get(&list, 3) == entries[2]);
    ASSERT(dt_entry_list_get(&list, 4) == entries[3]);

    dt_entry_list_sort_by_dirs(&list);

    ASSERT(dt_entry_list_get(&list, 0) == entries[0]);
    ASSERT(dt_entry_list_get(&list, 1) == entries[2]);
    ASSERT(dt_entry_list_get(&list, 2) == entries[4]);
    ASSERT(dt_entry_list_get(&list, 3) == entries[1]);
    ASSERT(dt_entry_list_get(&list, 4) == entries[3]);

    dt_entry_list_destruct(&list);
}

void dt_entry_list_test()
{
    dt_entry_list_construct_destruct_test();
    dt_entry_list_append_get_size_test();
    dt_entry_list_sort_test();
}