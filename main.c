#include "dt_entry.h"
#include "dt_entry_list.h"
#include "my_assert.h"
#include <stdio.h>
#define TEST

int main() {
#ifdef TEST
    dt_entry_test();
    dt_entry_list_test();
    PRINT_TEST_RESULTS;
#else
    printf("Hello, World!\n");
#endif
    return 0;
}