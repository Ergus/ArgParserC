
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

#ifdef _MSC_VER
#include <Windows.h>
#define sleep(X) Sleep(X * 1000)
#else
#include <unistd.h>
#endif

int main(int argc, char *argv[])
{
	init_args (argc, argv);

	int v_int_1 = create_cl_int ("vint_1");
	char *v_char_1 = create_cl_char_p ("vchar_1");
	double v_double_1 = create_cl_double ("vdouble_1");

	printf("# Read mandatory args: %d %s %lf\n", v_int_1, v_char_1, v_double_1);
	assert(v_int_1 == atoi(argv[1]));
	assert(strcmp(v_char_1, argv[2]) == 0);
	assert(v_double_1 == atof(argv[3]));

	int o_int_1 = create_optional_cl_int ("oint_1", -1);
	printf("# Read: o_int_1 %d\n", o_int_1);
	assert(o_int_1 == ((argc > 4 ? atoi(argv[4]) : -1)));

	double o_double_1 = create_optional_cl_double ("odouble_1", 0.5);
	printf("# Read: o_double_1 %lf\n", o_double_1);
	assert(o_double_1 == ((argc > 5 ? atof(argv[5]) : 0.5)));

	char *o_char_1 = create_optional_cl_char_p ("ochar_p_1", NULL);
	printf("# Read: o_char_1 %s\n", o_char_1);
	assert((argc > 6 ? strcmp(o_char_1, argv[6]) == 0 : o_char_1 == NULL));

	char *o_char_2 = create_optional_cl_char_p ("ochar_p_2", "");
	printf("# Read: o_char_2 %s\n", o_char_2);
	assert(strcmp(o_char_2, ((argc > 7 ? argv[7] : ""))) == 0);

	char *o_char_3 = create_optional_cl_char_p ("ochar_p_3", "opt1 opt2");
	printf("# Read: o_char_3 %s\n", o_char_3);
	assert(strcmp(o_char_3, ((argc > 8 ? argv[8] : "opt1 opt2"))) == 0);

	timer t1 = create_timer("timer_1");
	int r_int_1 = create_reportable_int("r_int_1", argc);
	printf("# Report: r_int_1 %d\n", r_int_1);
	assert(r_int_1 == argc);

	double r_double_1 = create_reportable_double("r_double_1", v_double_1 / o_double_1);
	printf("# Report: r_double_1 %lf\n", r_double_1);
	assert(r_double_1 == v_double_1 / o_double_1);

	char * r_char_1 = create_reportable_char_p("r_char_1", "hello");
	printf("# Report: r_char_1 %s\n", r_char_1);
	assert(strcmp(r_char_1, "hello") == 0);

	char * r_char_2 = create_reportable_char_p("r_char_2", "hello world");
	printf("# Report: r_char_2 %s\n", r_char_2);
	assert(strcmp(r_char_2, "hello world") == 0);

	timer t2 = create_timer("timer_2"); // New timer to trigger realloc
	printf("# Start sleep\n");
	sleep(1);
	printf("# End sleep\n");
	stop_timer(&t2);

	const double diff_time1 = getNS_timer(&t1);
	printf("# Timer: T1 %lf\n", diff_time1);
	assert(diff_time1 == 0);

	const double diff_time2 = getNS_timer(&t2);
	printf("# Times: T2 %lf\n", diff_time2);
	assert(diff_time2 >= 1E9);

	printf("# Reporting args\n");
	report_args();

	report_args_json();

	printf("\n# Call free args\n");
	free_args ();
	return 0;
}
