//
// Created by rodde on 11.6.2019.
//

#ifndef LINUX_C_DT_MY_ASSERT_H
#define LINUX_C_DT_MY_ASSERT_H

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(CONDITION) assert(CONDITION, #CONDITION, __FILE__, __LINE__)
#define PRINT_TEST_RESULTS print_test_results(total_assertions, total_assertion_errors)

extern size_t total_assertions;
extern size_t total_assertion_errors;

int assert(int cond, const char* err_msg, char* file_name, int line);
void print_test_results();

#endif //LINUX_C_DT_MY_ASSERT_H
