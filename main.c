#include "dt_entry.h"
#include "dt_entry_list.h"
#include "my_assert.h"
#include <linux/limits.h>
#include <pwd.h>
#include <stdio.h>  // printf
#include <string.h> // strcmp
#include <unistd.h>
//#define TEST

const char *const RELATIVE_TAG_FILE_PATH = "/.dt/tags";
const char *const OPERATION_DESCRIPTOR_MESSAGE = "message";
const char *const FLAG_UPDATE_PREVIOUS = "--update-previous";
const char *const PREV_TAG_NAME = "__dt_previous";

static char* get_tag_file_path(const char* home_directory)
{
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

static void update_previous_directory(const char* flag,
                                      const char* dir)
{
    char* home_directory;
    char* tag_file_path;
    char* current_working_directory;
    dt_entry_list* tag_list;
    dt_entry* prev_entry;
    FILE* file;

    // Make sure that the flag is correct:
    if (strcmp(flag, FLAG_UPDATE_PREVIOUS) != 0) {
        printf("%s\nExpected flag %s ",
                OPERATION_DESCRIPTOR_MESSAGE,
                FLAG_UPDATE_PREVIOUS);
        return;
    }

    // Get the home directory:
    home_directory = getpwuid(getuid())->pw_dir;

    // Get the full path to the tags file:
    tag_file_path = get_tag_file_path(home_directory);

    // Read the tags file into a list:
    file = fopen(tag_file_path, "r");
    tag_list = dt_entry_list_alloc();
    dt_entry_list_read_from_file(tag_list, file);
    fclose(file);

    // Get the possible previous tag:
    prev_entry = dt_entry_list_match(tag_list, PREV_TAG_NAME);

    // Get the current working directory:
    current_working_directory = malloc(PATH_MAX);
    getcwd(current_working_directory, PATH_MAX);

    if (prev_entry) {
        // Once here, just update the dir of the previous tag:
        dt_entry_set_dir(prev_entry, dir);
    } else {
        // No previous to begin with, just create one:
        dt_entry_list_append_entry(tag_list,
                                   PREV_TAG_NAME,
                                   dir);
    }

    // Rewrite the tags file:
    file = fopen(tag_file_path, "w");
    free(tag_file_path);
    dt_entry_list_write_to_file(tag_list, file);
    dt_entry_list_destruct(tag_list);
    fclose(file);
}

static void jump_to_previous_directory()
{
    dt_entry_list* list;

}

static void process_single_flag(char* flag)
{

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