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
#include <time.h>
#include <errno.h>


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

void reset_timer(timer *out)
{
	out->_startTime.tv_nsec = 0; out->_startTime.tv_sec = 0;
	out->_endTime.tv_nsec = 0; out->_endTime.tv_sec = 0;
	out->_accumulated.tv_nsec = 0; out->_accumulated.tv_sec = 0;
	*out->val_p = 0.0;
}

timer *create_timer(const char *name)
{
	timer *out = (timer *) malloc(sizeof (timer));
	out->val_p = create_reportable_double (name, 0.0);

	void reset_timer(timer *out);

	getTime(&out->_startTime);

	return out;
}

void start_timer(timer *out)
{
	getTime(&out->_startTime);
}


void stop_timer(timer *out)
{
	getTime(&out->_endTime);

	if (out->_endTime.tv_nsec < out->_startTime.tv_nsec) {
		const long nsec = 1E9L + out->_endTime.tv_nsec - out->_startTime.tv_nsec;
		out->_accumulated.tv_nsec += nsec;
		out->_accumulated.tv_sec += out->_endTime.tv_sec - 1 - out->_startTime.tv_sec;
	} else {
		out->_accumulated.tv_nsec += out->_endTime.tv_nsec - out->_startTime.tv_nsec;
		out->_accumulated.tv_sec += out->_endTime.tv_sec - out->_startTime.tv_sec;
	}

	*out->val_p = out->_accumulated.tv_sec * 1.0E9 + out->_accumulated.tv_nsec;
}

void free_timer(timer *out)
{
	stop_timer(out);
	free(out);
}