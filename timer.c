/*
 * Copyright (C) 2019  Jimmy Aguilar Mena
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "argparser.h"
#include <time.h>
#include <errno.h>
#include <assert.h>

#ifdef _MSC_VER
#include <windows.h>
#include <sysinfoapi.h>

#define CLOCK_MONOTONIC 1

static
int clock_gettime(int _ignore, struct timespec *spec)       //C-file part
{
	__int64 wintime;
	GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime      -=116444736000000000i64;           //1jan1601 to 1jan1970
	spec->tv_sec  =wintime / 10000000i64;           //seconds
	spec->tv_nsec =wintime % 10000000i64 *100;      //nano-seconds
	return 0;
}
#endif // _MSC_VER

// Timer
void getTime(struct timespec *ts)
{
	if (clock_gettime(CLOCK_MONOTONIC, ts)) {
		const int error = errno;
		fprintf(stderr, "Error: timer %s \n", strerror(error));
		exit(1);
	}
}

double getNS(const struct timespec *ts)
{
	return ts->tv_sec * 1.0E9 + ts->tv_nsec;
}

struct timespec diffTime(const struct timespec *t1, const struct timespec *t2)
{
	assert(t2->tv_sec >= t1->tv_sec);
	struct timespec ret;

	if (t2->tv_nsec < t1->tv_nsec) {
		ret.tv_nsec = 1000000000L + t2->tv_nsec - t1->tv_nsec;
		ret.tv_sec = (t2->tv_sec - 1 - t1->tv_sec);
	} else {
		ret.tv_nsec = (t2->tv_nsec - t1->tv_nsec);
		ret.tv_sec = (t2->tv_sec - t1->tv_sec);
	}
	return ret;
}

// ttimer functions (Specializations).
static
void reset_ttimer(ttimer *out)
{
	out->_startTime.tv_nsec = 0; out->_startTime.tv_sec = 0;
	out->_endTime.tv_nsec = 0; out->_endTime.tv_sec = 0;
	out->_accumulated.tv_nsec = 0; out->_accumulated.tv_sec = 0;
}

int snprintf_ttimer(char out[], size_t maxsize, const ttimer *in)
{
	double tmp = getNS(&in->_accumulated);
	switch (sing->time_format) {
	case _tformat_nanos:
		return snprintf(out, maxsize, "%g ns", tmp);
	case _tformat_millis:
		return snprintf(out, maxsize, "%g ms", tmp / 10E6);
	default:
		perror("Internal argparser error. Wrong time format");
	}
}


void copy_ttimer(ttimer *out, const ttimer *in)
{
	*out = *in;
}

static
void start_ttimer(ttimer *out)
{
	getTime(&out->_startTime);
}

static
void stop_ttimer(ttimer *out)
{
	getTime(&out->_endTime);

	struct timespec diff =  diffTime(&out->_startTime, &out->_endTime);

	out->_accumulated.tv_nsec += diff.tv_nsec;
	out->_accumulated.tv_sec += diff.tv_sec;
}

// timer functions (public)
double getNS_timer(const timer *in)
{
	return getNS(&get_ttimer_list(sing->ttimers, in->tidx)->_accumulated);
}

void reset_timer(timer *out)
{
	reset_ttimer(get_ttimer_list(sing->ttimers, out->tidx));
}

void start_timer(timer *out)
{
	ttimer *reportable = get_ttimer_list(sing->ttimers, out->tidx);
	start_ttimer(reportable);
}

int snprintf_timer(const timer *in, char out[], size_t maxsize)
{
	ttimer *reportable = get_ttimer_list(sing->ttimers, in->tidx);
	return snprintf_ttimer(out, maxsize, reportable);
}

// Function to create timer.
timer create_timer(const char *name)
{
	timer out;
	ttimer init;

	reset_ttimer(&init);
	strncpy (init.name, name, MAXNAME);
	ttimer *pushed = push_ttimer_list (sing->ttimers, &init);
	out.tidx = pushed - begin_ttimer_list(sing->ttimers);
	assert(out.tidx < size_ttimer_list(sing->ttimers));

	start_timer(&out);

	return out;
}

void stop_timer(timer *out)
{
	ttimer *reportable = get_ttimer_list(sing->ttimers, out->tidx);
	stop_ttimer(reportable);
}

