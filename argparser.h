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


#define TYPES					\
	F(int, d, atoi)				\
	F(double, lf, atof)			\
	F(char_p, s, (char_p))

typedef enum type_t {
	#define F(t,f,...) type_##t,
	TYPES
	#undef F
} type_t;

typedef struct generic_type {
	type_t type;
	char name[MAXNAME];
	union {
		#define F(t,f,...) t f;
		TYPES
		#undef F
	} value;
} generic_type;

void print_gt(generic_type * in);

// Arguments part

typedef struct global_args {
	int argc;
	char **argv;
	int it;
	generic_type *list;
} global_args;


#define F(T,F,C) T create_gt_##T (const char name[MAXNAME]);
TYPES
#undef F

#define F(T,F,C) T create_optional_gt_##T (const char name[MAXNAME], T def);
TYPES
#undef F


void init_args (int argc, char **argv);
void print_gt(generic_type * in);
void report_args ();

#endif
