////  ////////////////////////////////
 // Created by rodde on 11.6.2019. //
///////////////////////////////// ////

#include "my_assert.h"

size_t total_assertions = 0;
size_t total_assertion_errors = 0;

int assert(int cond, const char* err_msg, char* file_name, int line)
{
    if (!cond)
    {
        total_assertion_errors++;

        fprintf(stderr,
                "'%s' is not true in file '%s' at line %d.\n",
                err_msg,
                file_name,
                line);
    }

    total_assertions++;
    return cond;
}

void print_test_results()
{
    if (total_assertions == 0)
    {
        puts("Nothing tested.");
    }
    else
    {
        printf(
                "Assertions: %lu, errors: %lu, pass percentage: %0.2f.\n",
                (unsigned long) total_assertions,
                (unsigned long) total_assertion_errors,
                100.0f - (100.0f * total_assertion_errors) / total_assertions);

        if (total_assertion_errors == 0)
        {
            puts("TESTS PASSED!");
        }
        else
        {
            puts("Some tests failed.");
        }
    }
}