#include "dt_entry.h"
#include "dt_entry_list.h"
#include "my_assert.h"
#include <linux/limits.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//#define TEST
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

const char *const RELATIVE_TAG_FILE_PATH = "/.dt/tags";
const char *const OPERATION_DESCRIPTOR_MESSAGE = "message";
const char *const OPERATION_SWITCH_DIRECTORY = "switch_directory";
const char *const FLAG_UPDATE_PREVIOUS = "--update-previous";
const char *const FLAG_LIST_TAGS = "-l";
const char *const FLAG_LIST_BOTH = "-L";
const char *const FLAG_LIST_TAGS_SORTED = "-s";
const char *const FLAG_LIST_BOTH_SORTED = "-S";
const char *const FLAG_LIST_BOTH_SORTED_DIRS = "-d";
const char *const PREV_TAG_NAME = "__dt_previous";

static char* get_tag_file_path()
{
    char* home_directory = getpwuid(getuid())->pw_dir;
    size_t home_directory_len = strlen(home_directory);
    size_t relative_tag_file_name_len = strlen(RELATIVE_TAG_FILE_PATH);
    size_t result_len = home_directory_len + relative_tag_file_name_len + 1;
    char* rv = malloc(result_len);

    memcpy(rv, home_directory, home_directory_len);
    memcpy(rv + home_directory_len,
           RELATIVE_TAG_FILE_PATH,
           relative_tag_file_name_len + 1);

    return rv;
}

static char* get_current_working_directory()
{
    char* working_dir = malloc(PATH_MAX);
    getcwd(working_dir, PATH_MAX);
    return working_dir;
}

static void update_previous_directory(const char* flag,
                                      const char* dir)
{
    char* tag_file_path;
    char* current_working_directory;
    dt_entry_list* tag_list;
    dt_entry* prev_entry;
    FILE* file;

    /* Make sure that the flag is correct: */
    if (strcmp(flag, FLAG_UPDATE_PREVIOUS) != 0) {
        printf("%s\nExpected flag %s",
                OPERATION_DESCRIPTOR_MESSAGE,
                FLAG_UPDATE_PREVIOUS);
        return;
    }

    /* Get the full path to the tags file: */
    tag_file_path = get_tag_file_path();

    /* Read the tags file into a list: */
    file = fopen(tag_file_path, "r");
    tag_list = dt_entry_list_alloc();
    dt_entry_list_read_from_file(tag_list, file);
    fclose(file);

    /* Get the possible previous tag: */
    prev_entry = dt_entry_list_match(tag_list, PREV_TAG_NAME);

    /* Get the current working directory: */
    current_working_directory = malloc(PATH_MAX);
    getcwd(current_working_directory, PATH_MAX);

    if (prev_entry) {
        /* Once here, just update the dir of the previous tag: */
        dt_entry_set_dir(prev_entry, dir);
    } else {
        /* No previous to begin with, just create one: */
        dt_entry_list_append_entry(tag_list,
                                   PREV_TAG_NAME,
                                   dir);
    }

    /* Rewrite the tags file: */
    file = fopen(tag_file_path, "w");
    free(tag_file_path);
    dt_entry_list_write_to_file(tag_list, file);
    dt_entry_list_destruct(tag_list);
    fclose(file);
}

static void jump_to_previous_directory()
{
    FILE* file;
    char* tag;
    char* next_path;
    dt_entry* entry;
    dt_entry_list list;
    char* tag_file_path = get_tag_file_path();

    dt_entry_list_construct(&list);
    file = fopen(tag_file_path, "r");
    free(tag_file_path);
    dt_entry_list_read_from_file(&list, file);
    fclose(file);

    entry = dt_entry_list_match(&list, PREV_TAG_NAME);
    tag = dt_entry_get_tag(entry);

    if (strcmp(tag, PREV_TAG_NAME) == 0) {
        next_path = dt_entry_get_dir(entry);
        dt_entry_set_dir(entry, get_current_working_directory());
    } else {
        next_path = get_current_working_directory();
        dt_entry_list_append_entry(&list, PREV_TAG_NAME, next_path);
    }

    file = fopen(tag_file_path, "w");
    dt_entry_list_write_to_file(&list, file);
    dt_entry_list_destruct(&list);
    fclose(file);
    printf("%s\n%s", OPERATION_SWITCH_DIRECTORY, next_path);
}

static int eq(const char *const str1, const char *const str2)
{
    return strcmp(str1, str2) == 0;
}

static void list_tags_only(dt_entry_list* list)
{
    dt_entry* e;
    size_t i;

    for (i = 0; i != dt_entry_list_size(list); i++) {
        e = dt_entry_list_get(list, i);
        printf("%s\n", dt_entry_get_tag(e));
    }
}

static size_t get_max_tag_length(dt_entry_list* list)
{
    dt_entry* e;
    char* tag;
    size_t max_length_so_far = 0;
    size_t i;

    for (i = 0; i != dt_entry_list_size(list); i++) {
        e = dt_entry_list_get(list, i);
        tag = dt_entry_get_tag(e);
        max_length_so_far = MAX(max_length_so_far, strlen(tag));
    }

    return max_length_so_far;
}

static char* get_format_str(size_t max_tag_length)
{
    size_t num_of_digits;
    char* fmt; // ""
    char buffer[4]; /* Tag length maximum 1000 chars + zero terminator */
    sprintf(buffer, "%zu", max_tag_length);
    num_of_digits = strlen(buffer);
    fmt = malloc(10 + num_of_digits);
    sprintf(fmt, "%%-%zus %%s\n", max_tag_length);
    return fmt;
}

static void list_tags_and_dirs(dt_entry_list* list)
{
    dt_entry* e;
    size_t i;
    size_t max_tag_length = get_max_tag_length(list);
    const char* tag;
    const char* dir;
    char* format_str = get_format_str(max_tag_length);

    for (i = 0; i != dt_entry_list_size(list); i++) {
        e = dt_entry_list_get(list, i);
        tag = dt_entry_get_tag(e);
        dir = dt_entry_get_dir(e);
        printf(format_str, tag, dir);
    }

    free(format_str);
}

static void list_tag_file(const char* const flag)
{
    FILE* file = fopen(get_tag_file_path(), "r");
    dt_entry_list list;
    dt_entry_list_construct(&list);
    dt_entry_list_read_from_file(&list, file);

    if (eq(flag, FLAG_LIST_TAGS_SORTED)
        || eq(flag, FLAG_LIST_BOTH_SORTED)) {
        dt_entry_list_sort_by_tags(&list);
    } else if (eq(flag, FLAG_LIST_BOTH_SORTED_DIRS)) {
        dt_entry_list_sort_by_dirs(&list);
    }

    if (eq(flag, FLAG_LIST_TAGS) || eq(flag, FLAG_LIST_TAGS_SORTED)) {
        list_tags_only(&list);
    } else if (eq(flag, FLAG_LIST_BOTH)
        || eq(flag, FLAG_LIST_BOTH_SORTED)
        || eq(flag, FLAG_LIST_BOTH_SORTED_DIRS)){
        list_tags_and_dirs(&list);
    }
}

static void switch_directory(const char* const tag)
{
    dt_entry_list list;
    dt_entry* e;
    FILE* file;
    char* cwd;

    file = fopen(get_tag_file_path(), "r");
    dt_entry_list_construct(&list);
    dt_entry_list_read_from_file(&list, file);
    e = dt_entry_list_match(&list, tag);
    cwd = get_current_working_directory();

    if (e == NULL) {
        printf("%s\n%s", OPERATION_SWITCH_DIRECTORY, cwd);
        update_previous_directory(FLAG_UPDATE_PREVIOUS, cwd);
    } else {
        update_previous_directory(FLAG_UPDATE_PREVIOUS, cwd);
        printf("%s\n%s", OPERATION_SWITCH_DIRECTORY, dt_entry_get_dir(e));
    }
}

static void process_single_flag(char* flag)
{
    if (eq(flag, FLAG_LIST_TAGS)
        || eq(flag, FLAG_LIST_TAGS_SORTED)
        || eq(flag, FLAG_LIST_BOTH)
        || eq(flag, FLAG_LIST_BOTH_SORTED)
        || eq(flag, FLAG_LIST_BOTH_SORTED_DIRS)) {
        list_tag_file(flag);
    } else {
        switch_directory(flag);
    }
}

int main(int argc, char* argv[]) {
#ifdef TEST
    dt_entry_test();
    dt_entry_list_test();
    PRINT_TEST_RESULTS;
#else
      if (argc == 1)
          jump_to_previous_directory();
      else if (argc == 2)
          process_single_flag(argv[1]);
      else if (argc == 3)
          update_previous_directory(argv[1], argv[2]);

#endif
    return 0;
}