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
#include <assert.h>

#ifndef dbprintf
#ifndef NDEBUG
	#define dbprintf(...) fprintf(stderr, __VA_ARGS__)
#else
	#define dbprintf(...)
#endif // NDEBUG
#endif //dbprintf

#define MAXLIST 16
#define MAXNAME 64
#define MAXSTRSIZE 256

void getTime(struct timespec *ts);
double getNS(const struct timespec *ts);
struct timespec diffTime(const struct timespec *t1, const struct timespec *t2);

// ttimer
typedef struct ttimer {
	char name[MAXNAME];
	struct timespec _startTime;
	struct timespec _endTime;
	struct timespec _accumulated;
} ttimer;

// time
typedef struct timer {
	size_t tidx;
} timer;

timer create_timer(const char *name);
double getNS_timer(const timer *in);
void start_timer(timer *out);
void stop_timer(timer *out);
void reset_timer(timer *out);

#define COPYPTR(IN,IGNORE,OUT) *OUT = IN

/* The types must have 3 arguments:
   1) type
   2) format for printf without %, should not be repeated
   3) function to convert FROM char)
   4) Format to convert TO char (default printf) */
#define TYPES											\
	F(int, int, d, sscanf, "%d")						\
	F(double, double, lg, sscanf, "%lg")				\
	F(size_t, size_t, zu, sscanf, "%zu")				\
	F(string, char *, s, COPYPTR, "\"%s\"")

// Generic type (this is the key of everything)
typedef struct generic_type {
	// Enum with the defined types
	enum type_t {
		#define F(N,T,...) N##_type_id,
		TYPES
		#undef F
		total_type_ids
	} type;

	char name[MAXNAME];
	char _buffer[MAXSTRSIZE];
	union {
		#define F(N,T,F,...) T F;
		TYPES
		#undef F
	} value;
} generic_type;

	// list container (this is very general)
#define list_for(T)														\
	typedef struct T##_list {											\
		T *list;														\
		size_t count;													\
		size_t max_size;												\
	} T##_list;															\
																		\
	void init_##T##_list (T##_list *in, size_t max);					\
	T *push_##T##_list (T##_list *in, T *val);							\
	size_t size_##T##_list (T##_list *in);								\
	size_t max_size_##T##_list (T##_list *in) ;							\
	void free_##T##_list (T##_list *in);								\
	T *get_##T##_list (T##_list *in, size_t idx);						\
	T *begin_##T##_list (const T##_list *in);							\
	T *end_##T##_list (const T##_list *in);								\
																		\
	int snprintf_##T(char out[], size_t maxsize, const T *in);			\
	T *get_named_##T##_list (T##_list *in, const char name[]);			\
	void copy_##T(T *out, const T *in);

list_for(generic_type);
list_for(ttimer);
#undef list_for

// Arguments part
#define GLOBALS									\
	F(generic_type, args_list)					\
	F(generic_type, reportables)				\
	F(ttimer, ttimers)

struct _global_args_t{
	int argc;
	char **argv;
	int args_it;
	#define F(T, N) T##_list *N;
	GLOBALS
	#undef F
	enum {_format_raw = 0, _format_json, _nformats} format;
	enum {_tformat_nanos = 0, _tformat_millis, _ntformats} time_format;
};

extern struct _global_args_t *sing;

// Expandable macros to add arguments to parse.
#define F(N, T,...)														\
	void set_gt_##N (generic_type *out, const char name[], const T val); \
	const T create_cl_##N (const char name[]);							\
	const T create_optional_cl_##N (const char name[], const T def);	\
	const T create_reportable_##N (const char name[], const T value);
	TYPES
#undef F


/*! General functions. */
void init_args(int argc, char **argv);
void report_args();
void free_args();
int get_rest_args(char ***rest);

#ifdef __cplusplus
}
#endif

#endif
