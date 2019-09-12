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

#include "argparser.h"

static global_args *sing = NULL;

#define F(T,F,C) T create_gt_##T (const char name[MAXNAME])		\
	{								\
		if (sing->it >= sing->argc) {				\
			fprintf (stderr, "Error: no enough arguments to set %s\n", name); \
			exit (1);						\
		}							\
		const size_t it = sing->it++;				\
		const T val = C(sing->argv[it]);			\
		generic_type *out = &(sing->list[it]);			\
									\
		out->type = type_##T ;					\
		strncpy (out->name, name, MAXNAME);			\
		out->value.F = val;					\
									\
		return val;						\
	}

TYPES
#undef F

#define F(T,F,C)							\
	T create_optional_gt_##T (const char name[MAXNAME], T def)	\
	{								\
		T val = def;						\
		const size_t it = sing->it++;				\
									\
		if (it < sing->argc)					\
			val = C(sing->argv[it]);			\
									\
		generic_type *out = &(sing->list[it]);			\
									\
		out->type = type_##T ;					\
		strncpy (out->name, name, MAXNAME);			\
		out->value.F = val;					\
									\
		return val;						\
	}

TYPES
#undef F

void init_args(int argc, char **argv)
{
	if (!sing) {
		sing = (global_args *) malloc (sizeof(global_args));
		sing->argc = argc;
		sing->argv = argv;
		sing->it   = 0;
		sing->list = (generic_type *) malloc(MAXLIST * sizeof(generic_type));
	} else {
		fprintf(stderr, "Arguments can be  initialized only once.");
		exit(EXIT_FAILURE);
	}
	create_gt_char_p ("Executable");
}

void print_gt(generic_type * in)
{
	switch (in->type) {
		#define F(T,F,C)					\
			case ( type_##T ):				\
				printf ("%s: %" #F "\n",			\
					in->name, in->value.F );	\
				break;
		TYPES
		#undef F
	default:
		fprintf (stderr, "Error printing generic type");
	}
}

void free_args ()
{
	free(sing->list);
	free(sing);
}

void report_args ()
{
	for (size_t i = 0; i < sing->it; ++i)
		print_gt (&(sing->list[i]));
}



