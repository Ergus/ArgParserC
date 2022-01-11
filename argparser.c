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

global_args_t *sing = NULL;

// List containers
#define list_for(T)														\
	void init_##T##_list (T##_list *in, size_t max_size)				\
	{																	\
		in->list = (T *) malloc(max_size * sizeof (T));					\
		in->max_size = max_size;										\
		in->count = 0;													\
	}																	\
																		\
	int push_##T##_list (T##_list *in, T *value)						\
	{																	\
		if (in->count + 1 >= in->max_size) {							\
			in->max_size *= 2;											\
			dbprintf ("Reallocating array to %d\n", in->max_size);		\
																		\
			T *tmp = (T *) malloc(2 * in->max_size * sizeof(T));		\
			for (size_t i = 0; i < in->max_size; ++i){					\
				tmp[i] = in->list[i];									\
			}															\
			free(in->list);												\
			in->list = tmp;												\
																		\
			if (!in->list)												\
				return -1;												\
		}																\
																		\
		in->list[in->count] = *value;									\
		in->list[in->count];											\
		return in->count++;												\
	}																	\
																		\
	void free_##T##_list (T##_list *in)									\
	{																	\
		free(in->list);													\
	}																	\
																		\
																		\
	T *get_##T##_list (T##_list *in, size_t idx)						\
	{																	\
		if (idx >= in->count)											\
			return NULL;												\
		return &in->list[idx];											\
	}																	\
																		\
	T *begin_##T##_list (const T##_list *in) {return in->list;};		\
	T *end_##T##_list (const T##_list *in) {return &in->list[in->count];}; \
																		\

list_for(generic_type);
list_for(ttimer);
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
#define F(T,F,C,P)														\
	void set_gt_##T (generic_type *out, const char name[], T val)		\
	{																	\
		out->type = T##_type_id ;										\
		strncpy (out->name, name, MAXNAME);								\
		out->value.F = val;												\
	}
TYPES
#undef F

#define F(T,F,C,P)														\
	T create_cl_##T (const char name[])									\
	{																	\
		if (sing->args_it >= sing->argc) {								\
			dbprintf ("Error: no enough CL arguments to set %s (arg: %d) \n", \
			          name, sing->args_it);								\
			exit (1);													\
		}																\
																		\
		T val;															\
		C(sing->argv[sing->args_it++], "%" #F, &val);					\
																		\
		generic_type out;												\
		set_gt_##T (&out, name, val);									\
		push_generic_type_list (sing->args_list, &out);					\
																		\
		return val;														\
	}
TYPES
#undef F

#define F(T,F,C,P)												\
	T create_optional_cl_##T (const char name[], T def)			\
	{															\
		T val = def;											\
																\
		if (sing->args_it < sing->argc) {						\
			C(sing->argv[sing->args_it++], "%" #F, &val);		\
		}														\
																\
		generic_type out;										\
		set_gt_##T (&out, name, val);							\
		push_generic_type_list (sing->args_list, &out);			\
																\
		return val;												\
	}
TYPES
#undef F


#define F(T,F,C,P)														\
	int create_reportable_##T (const char name[], T val)				\
	{																	\
		generic_type out;												\
		set_gt_##T(&out, name, val);									\
		return push_generic_type_list (sing->reportables, &out);		\
	}
TYPES
#undef F


int snprintf_generic_type(char out[], size_t maxsize,const generic_type *in)
{
	assert(in->type < total_type_ids);
	switch (in->type) {
#define F(T,F,C,P)												\
		case ( T##_type_id ):									\
			return P(out, maxsize, "%" #F, in->value.F);
		TYPES
#undef F
	default:
		dbprintf ("Error printing generic type\n");
	}
	return -1;
}


// Implemented (no generated) functions.
void init_args(int argc, char **argv)
{
	if (sing) {
		dbprintf("Arguments can be initialized only once.\n");
		exit(EXIT_FAILURE);
	}

	sing = (global_args_t *) malloc(sizeof(global_args_t));
	sing->argc = argc;
	sing->argv = argv;
	sing->args_it = 0;
	sing->args_list =
		(generic_type_list *) malloc(sizeof(generic_type_list));
	sing->reportables =
		(generic_type_list *) malloc(sizeof(generic_type_list));
	sing->ttimers =
		(ttimer_list *) malloc(sizeof(ttimer_list));

	init_generic_type_list (sing->args_list, MAXLIST);
	init_generic_type_list (sing->reportables, MAXLIST);
	init_ttimer_list (sing->ttimers, MAXLIST);

	create_cl_char_p ("Executable");
}

void free_args()
{
	free_generic_type_list(sing->args_list);
	free(sing->args_list);
	free_generic_type_list(sing->reportables);
	free(sing->reportables);
	free_ttimer_list(sing->ttimers);
	free(sing->ttimers);
	free(sing);
	sing = NULL;
}

static
void report_args_base(const char start[], const char sep[],
                      const char formatpair[], // sep, key: value
                      const char close[])
{
	int counter = 0;
	char buff[MAXSTRSIZE];

#define F(T, N)															\
	for (const T *it = begin_##T##_list(sing->N); it != end_##T##_list(sing->N); ++it) { \
		snprintf_##T(buff, MAXSTRSIZE, it);								\
		printf(formatpair, (counter++ ? sep : start), it->name, buff);	\
	}
	GLOBALS
#undef F

	printf("%s", close);
}

void report_args()
{
	report_args_base("", "\n", "%s%s: %s", "\n");
}

void report_args_json()
{
	report_args_base("{", ",", "%s\"%s\":%s","}");
}
