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

cmake_minimum_required(VERSION 3.5)

project(ArgParserC LANGUAGES C CXX)

if ((NOT CMAKE_BUILD_TYPE MATCHES "^Rel") AND (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
  option(WITH_COVERAGE "Generate coverage files:" false)
endif ()

if (WITH_COVERAGE)
  add_compile_options(--coverage)
  add_link_options(--coverage)
  set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
endif()

# Important starts here.
add_library(argparser STATIC argparser.c timer.c)
if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(argparser PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()


add_executable (test_c main.c)
target_link_libraries (test_c argparser)

add_executable (test_cpp main.cpp)
target_link_libraries (test_cpp argparser)
target_compile_features(test_cpp PUBLIC cxx_std_17)

# All these are only for tests, not built in release mode.
if (NOT CMAKE_BUILD_TYPE MATCHES "^Rel")

    enable_testing()

    macro(create_tests executable)

	add_test(NAME ${executable}_fail_1 COMMAND ${executable} 1)
	set_tests_properties(${executable}_fail_1 PROPERTIES
	    PASS_REGULAR_EXPRESSION "^Error: no enough CL arguments to set v_string_1")

	add_test(NAME ${executable}_fail_2 COMMAND ${executable} 1 2)
	set_tests_properties(${executable}_fail_2 PROPERTIES
	    PASS_REGULAR_EXPRESSION "^Error: no enough CL arguments to set v_double_1")

	add_test(NAME ${executable}_4 COMMAND ${executable} 1 name1 2.6)
	add_test(NAME ${executable}_6 COMMAND ${executable} 1 2 3 4 5 6)
	add_test(NAME ${executable}_7 COMMAND ${executable} 2.5 name1 2.5 5 3 name2)
	add_test(NAME ${executable}_space COMMAND ${executable} 1 2 3 4 5 6 "7 8")
	add_test(NAME ${executable}_9 COMMAND ${executable} 1 2 3 4 5 6 "7 7" 8)
	add_test(NAME ${executable}_rest COMMAND ${executable} 1 2 3 4 5 6 "7 7" 8 9 10 11)
	add_test(NAME ${executable}_json COMMAND ${executable} -json 1 2 3 4 5 6 7 8)

	add_test(NAME ${executable}_rest_check COMMAND ${executable} 1 2 3 4 5 6 "7 7" 8 9 10 11)
	set_tests_properties(${executable}_rest_check PROPERTIES
	    PASS_REGULAR_EXPRESSION "REST: \"9\" \"10\" \"11\"")

	add_test(NAME ${executable}_json_rest_check COMMAND ${executable} -json 1 2 3 4 5 6 7 8 9 10 11)
	set_tests_properties(${executable}_json_rest_check PROPERTIES
	    PASS_REGULAR_EXPRESSION "\"REST\":\\[\"9\",\"10\",\"11\"\\]")
    endmacro()

    create_tests(test_c)
    create_tests(test_cpp)

    get_property(TEST_NAMES DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY TESTS)
    foreach(test ${TEST_NAMES})
	set_tests_properties(${test} PROPERTIES TIMEOUT 3)
    endforeach()

endif ()
