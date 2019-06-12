cmake_minimum_required(VERSION 3.14)
project(linux_c_dt C)

set(CMAKE_C_STANDARD 90)

add_executable(linux_c_dt main.c dt_entry.h dt_entry.c my_assert.h my_assert.c dt_entry_list.h dt_entry_list.c)