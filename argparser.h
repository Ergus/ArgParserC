/*
 * Copyright (C) 2019  Jimmy Aguilar Mena
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLIST 16
#define MAXNAME 32

typedef char * char_p;

/* The types must have 3 arguments:
   1) type
   2) format for printf without %, should not be repeated
   3) function to convert from char) */
#define TYPES					\
	F(int, d, atoi)				\
	F(double, lf, atof)			\
	F(char_p, s, (char_p))

// Enum with the defined types
typedef enum type_t {
	#define F(t,f,...) type_##t,
	TYPES
	#undef F
} type_t;

// Generic type (this is the key of everything)
typedef struct generic_type {
	type_t type;
	char name[MAXNAME];
	union {
		#define F(t,f,...) t f;
		TYPES
		#undef F
	} value;
} generic_type;

// list container
#define list_for(T)				\
	typedef struct T##_list {		\
		T *list;			\
		int count;			\
		int max;			\
	} T##_list;				\
						\
	void init_##T##_list (T##_list *in, int max);	\
	bool push_##T##_list (T##_list *in, T *val);	\
	inline int size_##T##_list (T##_list *in) {return in->count;};	\
	void free_##T##_list (T##_list *in);				\
	T *get_##T##_list (T##_list *in, int idx);			\
	T *begin_##T##_list (T##_list *in);	\
	T *end_##T##_list (T##_list *in); \

list_for(generic_type);
#undef list_for

// Arguments part
typedef struct global_args {
	int argc;
	char **argv;
	int args_it;
	generic_type_list *args_list;
} global_args;

// Expandable macros to add arguments to parse.
#define F(T,F,C) T create_gt_##T (const char name[MAXNAME]);
TYPES
#undef F

#define F(T,F,C) T create_optional_gt_##T (const char name[MAXNAME], T def);
TYPES
#undef F

// Manual functions
void init_args (int argc, char **argv);
void print_gt(generic_type * in);
void report_args ();
void free_args ();
#endif
