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

if ((NOT CMAKE_BUILD_TYPE MATCHES "^Rel") AND (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
  option(WITH_COVERAGE "Generate coverage files:" false)
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  add_definitions(argparser PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

if (WITH_COVERAGE)
  add_compile_options(--coverage)
  add_link_options(--coverage)
  set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
endif()

# Important starts here.
add_library(argparser STATIC argparser.c timer.c)

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

  add_test(NAME test_fail1 COMMAND test_main 1 name1 2.6)
  add_test(NAME test_fail2 COMMAND test_main 2.5 name1 2.5 5 3 name2)
  add_test(NAME test_6 COMMAND test_main 1 2 3 4 5 6)
  add_test(NAME test_space COMMAND test_main 1 2 3 4 5 6 "7 8")
  add_test(NAME test_9 COMMAND test_main 1 2 3 4 5 6 "7 7" 8)
  add_test(NAME test_rest COMMAND test_main 1 2 3 4 5 6 "7 7" 8 9 10 11)
  add_test(NAME test_json COMMAND test_main -json 1 2 3 4 5 6 7 8)

  get_property(TEST_NAMES DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY TESTS)
  foreach(test ${TEST_NAMES})
    set_tests_properties(${test} PROPERTIES TIMEOUT 3)
  endforeach()

endif ()
