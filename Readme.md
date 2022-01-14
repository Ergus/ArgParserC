<!--
 !-- Copyright (C) 2019  Jimmy Aguilar Mena
 !--
 !-- This program is free software: you can redistribute it and/or modify
 !-- it under the terms of the GNU General Public License as published by
 !-- the Free Software Foundation, either version 3 of the License, or
 !-- (at your option) any later version.
 !--
 !-- This program is distributed in the hope that it will be useful,
 !-- but WITHOUT ANY WARRANTY; without even the implied warranty of
 !-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 !-- GNU General Public License for more details.
 !--
 !-- You should have received a copy of the GNU General Public License
 !-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
  -->

# Readme


This is a very simple code that emulates metaprogramming with C macros
to parse arguments from command line and store them in an array and
report them at the end with the right format for `printf`.

Of course there are several limitations because metaprogramming is
not a C feature. But it is a good example of how to exploit macros
in C to produce scalable template-like function.


## Usage


The project is very simple built with cmake. So you can add it as a
cmake module or as a git submodule. Currently Linux, Max and Windows
are supported.

The compilation generates the **libargparser.a** which is a static object
you can link to in compilation time, so you don't need to redistribute
it with your executable as it doesn't becomes a dependency.

The function definitions are all in argparser.h; but the
implementations are distributed in different files. The final user
don't really need to deal with anything else more than:

```C
#include "argparser.h"
```

## API


The api provides different functions for 3 different functionalities;
most of the functions described here with TYPE are generated throw
macros, so they provide exactly the same functionalities, but for
different input and/or output types.

Most of the functions that receives a `name` argument uses it to
report the parameter in the report. The name is expected to be a
`const char[]` with no more than 64 characters.

There are a general use functions for the user:

	* `void init_args (int argc, char **argv)` : This one is needed before any call to tho the internal api. It allocates the internal datastructures and initializes the api.

	* `void report_args ()` : To print the report. This can be called several times if needed.

	* `void report_args_json ()` : Like `report_args` but aoutput is formated as json.

	* `void free_args ()` : Release all the internal's library memory and datastructures. Use this at the very end of the program.

The functionalities are:

1. Parse command line arguments in order and add them to the Final
   report and get the value converted to `TYPE`.

   * `const TYPE create_cl_TYPE (const char *name)` : Read a
     positional command line argument and convert it to *TYPE*. If no
     enough command line arguments are provided then the function
     fails and aborts the program. When TYPE is `string` then the
     function returns `char *`.

   * `const TYPE create_optional_cl_TYPE (const char *name, TYPE default)` :
     Read a positional command line argument and convert it to *TYPE*,
     if no enough command line arguments are provided then the
     function created the value with the `default` value.


2. Add specific values to the final report.

	* `const TYPE create_reportable_TYPE (const char *name, TYPE value)` :
      This creates a `reportable` value. This function add a variable
      with `name` and default value `val`. The report values are
      reported AFTER the command line arguments.

3. Add timers that automatically are added to the report.

	* `timer create_timer(const char *name)` : Creates a timer and start it.

	* `void start_timer(timer *out)` : start the timer to count
      (needed only after a call to `stop`.

	* `void stop_timer(timer *out)` : stop the timer
      temporarily. Successive calls to stop and start are useful to
      accumulate time values as the timer accumulator isn't reset.

	* `void reset_timer(timer *out)` : resets a timer. this discards
      all the previous values stored/accumulated. It is recommended to
      use this only after having saved the values to some other
      variable.

	* `getNS_timer(const timer *in)` : Returns the number of
      nano-seconds the timer has measured (accumulated or stored).

### TYPES

For now only 4 different types are supported: `int`, `double`,
`size_t` and `string`. The last one is just a name for functions
returning `char *` needed to provide a nice name for the macro. Extra
types can be added very easily.

To see how to use all of this there is a *main.c* file used to
generate the tests where most of the API functions are used.

The `string` stores strings in an internal buffer. Currently the
buffer size is limited to 256 chars (including the '\0') and in case
of overflow the string is truncated quietly.
