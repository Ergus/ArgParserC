# Copyright (C) 2019  Jimmy Aguilar Mena

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8.12)

project(ArgParserC LANGUAGES C)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(--coverage)
  add_link_options(--coverage)
  set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
  find_program (COVERAGE_COMMAND gcov)
  message("HEE: ${COVERAGE_COMMAND}")
endif ()


add_library(argparser STATIC argparser.c timer.c)

if (WIN32)
  target_compile_definitions(argparser PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

add_executable (test_main main.c)
target_link_libraries (test_main argparser)

if (NOT CMAKE_BUILD_TYPE MATCHES "^Rel")

  enable_testing()

  add_test(NAME test_1 COMMAND test_main 1)
  set_tests_properties(test_1 PROPERTIES
    PASS_REGULAR_EXPRESSION "^Error: no enough CL arguments to set v_string_1")

  add_test(NAME test_2 COMMAND test_main 1 2)
  set_tests_properties(test_2 PROPERTIES
    PASS_REGULAR_EXPRESSION "^Error: no enough CL arguments to set v_double_1")

  add_test(NAME test_3 COMMAND test_main 1 name1 2.6)
  add_test(NAME test_4 COMMAND test_main 2.5 name1 2.5 5 3 name2)
  add_test(NAME test_5 COMMAND test_main 1 2 3 4 5 6)
  add_test(NAME test_6 COMMAND test_main 1 2 3 4 5 6 "7 8")
  add_test(NAME test_7 COMMAND test_main 1 2 3 4 5 6 "7 8" 9)

  foreach(x RANGE 1 7)
    set_tests_properties(test_${x} PROPERTIES TIMEOUT 3)
  endforeach()

endif ()
