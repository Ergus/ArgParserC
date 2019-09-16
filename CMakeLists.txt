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

cmake_minimum_required(VERSION 2.6)

project (ArgParserC)

add_library (argparser argparser.c timer.c)

add_executable (test_main main.c)
target_link_libraries (test_main argparser)

enable_testing()

add_test(NAME test_1 COMMAND test_main 1)
set_tests_properties(test_1 PROPERTIES
  PASS_REGULAR_EXPRESSION "Error: no enough CL arguments to set vchar_p")

add_test(NAME test_2 COMMAND test_main 1 2)
set_tests_properties(test_2 PROPERTIES
  PASS_REGULAR_EXPRESSION "Error: no enough CL arguments to set vdouble")

add_test(NAME test_3 COMMAND test_main 1 name 2.6)
add_test(NAME test_4 COMMAND test_main 2.5 name1 2.5 5 3 name2)
add_test(NAME test_5 COMMAND test_main 1 2 3 4 5 6)
