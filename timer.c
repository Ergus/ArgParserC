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

// Timer
static
void getTime(struct timespec *ts)
{
	if (clock_gettime(CLOCK_MONOTONIC, ts)) {
		const int error = errno;
		fprintf(stderr, "Error: timer %s \n", strerror(error));
		exit(1);
	}
}

static
double getNS(const struct timespec *ts)
{
	return ts->tv_sec * 1.0E9 + ts->tv_nsec;
}


// ttimer functions
static
void reset_ttimer(ttimer *out)
{
	out->_startTime.tv_nsec = 0; out->_startTime.tv_sec = 0;
	out->_endTime.tv_nsec = 0; out->_endTime.tv_sec = 0;
	out->_accumulated.tv_nsec = 0; out->_accumulated.tv_sec = 0;
}

int snprintf_ttimer(char out[], size_t maxsize, const ttimer *in)
{
	return snprintf(out, maxsize, "%g", getNS(&in->_accumulated));
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

	getTime(&reportable->_startTime);
}

// Function to create timer.
timer create_timer(const char *name)
{
	timer out;
	ttimer init;

	reset_ttimer(&init);
	strncpy (init.name, name, MAXNAME);
	out.tidx = push_ttimer_list (sing->ttimers, &init);
	assert(out.tidx >= 0);

	start_timer(&out);

	return out;
}

void stop_timer(timer *out)
{
	ttimer *reportable = get_ttimer_list(sing->ttimers, out->tidx);

	getTime(&reportable->_endTime);

	const struct timespec *startTime = &reportable->_startTime;
	const struct timespec *endTime = &reportable->_endTime;
	struct timespec *accumulated = &reportable->_accumulated;

	if (endTime->tv_nsec < startTime->tv_nsec) {
		accumulated->tv_nsec += 1E9L + endTime->tv_nsec - startTime->tv_nsec;
		accumulated->tv_sec += (endTime->tv_sec - 1 - startTime->tv_sec);
	} else {
		accumulated->tv_nsec += (endTime->tv_nsec - startTime->tv_nsec);
		accumulated->tv_sec += (endTime->tv_sec - startTime->tv_sec);
	}
}

