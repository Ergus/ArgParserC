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

global_args *sing = NULL;

// List containers
#define list_for(T)							\
	void init_##T##_list (T##_list *in, int max_size)		\
	{								\
		in->list = (T *) calloc (max_size, sizeof (T));		\
		in->max_size = max_size;				\
		in->count = 0;						\
	}								\
									\
	int push_##T##_list (T##_list *in, T *value)			\
	{								\
		if (in->count + 1 >= in->max_size) {			\
			in->max_size *= 2;				\
			in->list = realloc(in->list, in->max_size * sizeof(T)); \
			if (!in->list)					\
				return -1;				\
		}							\
									\
		in->list[in->count] = *value;				\
		in->list[in->count];					\
		return in->count++;					\
	}								\
									\
	void free_##T##_list (T##_list *in)				\
	{								\
		free (in->list);					\
	}								\
									\
									\
	T *get_##T##_list (T##_list *in, int idx)			\
	{								\
		if (idx < 0 || idx >= in->count)			\
			return NULL;					\
		return &in->list[idx];					\
	}								\
									\
	T *begin_##T##_list (T##_list *in) {return in->list;};		\
	T *end_##T##_list (T##_list *in) {return &in->list[in->count];}; \
									\

list_for(generic_type);
#undef list_for

generic_type *get_named_generic_type_list (generic_type_list *in,
                                           const char name[])
{
	for (generic_type *it = begin_generic_type_list(in);
	     it != end_generic_type_list(in); ++it)
		if (strcmp (name, it->name) == 0)
			return it;
	return NULL;
}



// The rest
#define F(T,F,C) void set_gt_##T (generic_type *out, const char name[MAXNAME], T val) \
	{								\
		out->type = type_##T ;					\
		strncpy (out->name, name, MAXNAME);			\
		out->value.F = val;					\
	}
TYPES
#undef F

#define F(T,F,C) T create_cl_##T (const char name[MAXNAME])		\
	{								\
		if (sing->args_it >= sing->argc) {			\
			fprintf (stderr,				\
			         "Error: no enough CL arguments to set %s (arg: %d) \n", \
			         name, sing->args_it);			\
			exit (1);					\
		}							\
									\
		const T val = C(sing->argv[sing->args_it++]);		\
									\
		generic_type out;					\
		set_gt_##T (&out, name, val);				\
		push_generic_type_list (sing->args_list, &out);		\
									\
		return val;						\
	}
TYPES
#undef F

#define F(T,F,C)							\
	T create_optional_cl_##T (const char *name, T def)	\
	{								\
		T val = def;						\
		const size_t it = sing->args_it++;			\
									\
		if (it < sing->argc)					\
			val = C(sing->argv[it]);			\
									\
		generic_type out;					\
		set_gt_##T (&out, name, val);				\
		push_generic_type_list (sing->args_list, &out);		\
									\
		return val;						\
	}
TYPES
#undef F


#define F(T,F,C) int create_reportable_##T (const char *name, T val)	\
	{								\
		generic_type out;					\
		set_gt_##T (&out, name, val);				\
		return push_generic_type_list (sing->reportables, &out); \
	}
TYPES
#undef F



// Implemented (no generated) functions.
void init_args(int argc, char **argv)
{
	if (!sing) {
		sing = (global_args *) malloc (sizeof(global_args));
		sing->argc = argc;
		sing->argv = argv;
		sing->args_it = 0;
		sing->args_list =
			(generic_type_list *) malloc (sizeof(generic_type_list));
		sing->reportables =
			(generic_type_list *) malloc (sizeof(generic_type_list));

		init_generic_type_list (sing->args_list, MAXLIST);
		init_generic_type_list (sing->reportables, MAXLIST);

		create_cl_char_p ("Executable");
	} else {
		fprintf(stderr, "Arguments can be  initialized only once.");
		exit(EXIT_FAILURE);
	}
}

void print_gt(generic_type * in)
{
	switch (in->type) {
		#define F(T,F,C)					\
			case ( type_##T ):				\
				printf ("%s: %" #F "\n",		\
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
	free_generic_type_list(sing->args_list);
	free (sing->args_list);
	free_generic_type_list(sing->reportables);
	free (sing->reportables);
	free(sing);
}

void report_args ()
{
	for (generic_type *it = begin_generic_type_list(sing->args_list);
	     it != end_generic_type_list(sing->args_list); ++it)
		print_gt (it);

	for (generic_type *it = begin_generic_type_list(sing->reportables);
	     it != end_generic_type_list(sing->reportables); ++it)
		print_gt (it);
}
