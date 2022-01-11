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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#ifndef dbprintf
#ifndef NDEBUG
	#define dbprintf(...) fprintf(stderr, __VA_ARGS__)
#else
	#define dbprintf(...)
#endif // NDEBUG
#endif //dbprintf

#define MAXLIST 16
#define MAXNAME 128
#define MAXSTRSIZE 256

typedef struct ttimer {
	char name[MAXNAME];
	struct timespec _startTime;
	struct timespec _endTime;
	struct timespec _accumulated;
} ttimer;

typedef struct timer {
	int tidx;
} timer;

/*! Timer functions. */
timer create_timer(const char *name);
double getNS_timer(const timer *in);
void print_timer(const timer *in);
void start_timer(timer *out);
void stop_timer(timer *out);
void reset_timer(timer *out);

typedef char * char_p;

#define COPYPTR(IN,IGNORE,OUT) *OUT = IN

#define COPY_STRING(buff, FORMAT, input)				\
	snprintf(buff, MAXNAME, "\"" FORMAT "\"", input)

#define PRINT_STRING(buff, MAXSIZE, FORMAT, input)		\
	snprintf(buff, MAXSIZE, "\"" FORMAT "\"", input)

/* The types must have 3 arguments:
   1) type
   2) format for printf without %, should not be repeated
   3) function to convert FROM char)
   4) function to convert TO char (default printf) */
#define TYPES										\
	F(int, d, sscanf, snprintf)						\
	F(double, lg, sscanf, snprintf)					\
	F(size_t, zu, sscanf, snprintf)					\
	F(char_p, s, COPYPTR, PRINT_STRING)

// Generic type (this is the key of everything)
typedef struct generic_type {
	// Enum with the defined types
	enum type_t {
		#define F(T,...) type_##T,
		TYPES
		#undef F
	} type;

	char name[MAXNAME];
	union {
		#define F(T,F,...) T F;
		TYPES
		#undef F
	} value;
} generic_type;

	// list container (this is very general)
#define list_for(T)														\
	typedef struct T##_list {											\
		T *list;														\
		int count;														\
		int max_size;													\
	} T##_list;															\
																		\
	void init_##T##_list (T##_list *in, int max);						\
	int push_##T##_list (T##_list *in, T *val);							\
	inline int size_##T##_list (T##_list *in) {return in->count;};		\
	inline int max_size_##T##_list (T##_list *in) {return in->max_size;}; \
	void free_##T##_list (T##_list *in);								\
	T *get_##T##_list (T##_list *in, int idx);							\
	T *begin_##T##_list (const T##_list *in);							\
	T *end_##T##_list (const T##_list *in);								\
																		\
	int snprintf_##T(char out[], size_t maxsize, const T *in)

list_for(generic_type);
list_for(ttimer);
#undef list_for

generic_type *get_named_generic_type_list (generic_type_list *in,
                                           const char name[]);

// Arguments part
#define GLOBALS									\
	F(generic_type, args_list)					\
	F(generic_type, reportables)				\
	F(ttimer, ttimers)

typedef struct global_args_t {
	int argc;
	char **argv;
	int args_it;
#define F(T, N) T##_list *N;
	GLOBALS
#undef F
} global_args_t;

extern global_args_t *sing;

// Expandable macros to add arguments to parse.
#define F(T,...) \
	void set_gt_##T (generic_type *out, const char name[], T val);	\
	T create_cl_##T (const char name[]);							\
	T create_optional_cl_##T (const char name[], T def);			\
	int create_reportable_##T (const char name[], T value);
TYPES
#undef F


/*! General functions. */
void init_args(int argc, char **argv);
void report_args();
void report_args_json();
void free_args();

#ifdef __cplusplus
}
#endif

#endif
