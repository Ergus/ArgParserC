
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

const char * global_string = "Value for reportable in global variable";

const char * get_global_string()
{
	return global_string;
}

int main(int argc, char *argv[])
{
	init_args (argc, argv);

	// Mandatory
	int v_int_1 = create_cl_int ("v_int_1");
	const char *v_string_1 = create_cl_string ("v_string_1");
	double v_double_1 = create_cl_double ("v_double_1");

	int it = 1 + (strcmp(argv[1],"-json") == 0);

	printf("# Read mandatory args: %d %s %lf\n", v_int_1, v_string_1, v_double_1);
	assert(v_int_1 == atoi(argv[it++]));
	assert(strcmp(v_string_1, argv[it++]) == 0);
	assert(v_double_1 == atof(argv[it++]));

	int o_int_1 = create_optional_cl_int ("o_int_1", -1);
	printf("# Read: o_int_1 %d\n", o_int_1);
	assert(o_int_1 == ((argc > 4 ? atoi(argv[it++]) : -1)));

	double o_double_1 = create_optional_cl_double ("o_double_1", 0.5);
	printf("# Read: o_double_1 %lf\n", o_double_1);
	assert(o_double_1 == ((argc > 5 ? atof(argv[it++]) : 0.5)));

	const char *o_string_1 = create_optional_cl_string ("o_string_1", NULL);
	printf("# Read: o_string_1 %s\n", o_string_1);
	assert((argc > 6 ? strcmp(o_string_1, argv[it++]) == 0 : o_string_1 == NULL));

	const char *o_string_2 = create_optional_cl_string ("o_string_2", "");
	printf("# Read: o_string_2 %s\n", o_string_2);
	assert(strcmp(o_string_2, ((argc > 7 ? argv[it++] : ""))) == 0);

	const char *o_string_3 = create_optional_cl_string ("o_string_3", "opt1 opt2");
	printf("# Read: o_string_3 %s\n", o_string_3);
	assert(strcmp(o_string_3, ((argc > 8 ? argv[it++] : "opt1 opt2"))) == 0);

	timer t1 = create_timer("timer_1");
	int r_int_1 = create_reportable_int("r_int_1", argc);
	printf("# Report: r_int_1 %d\n", r_int_1);
	assert(r_int_1 == argc);

	char **rest = NULL;
	int nrest = get_rest_args(&rest);
	if (nrest > 0){
		printf("# ");
		for (int i = 0; i < nrest; ++i) {
			printf("rest[%d]: \"%s\", ", i, rest[i]);
		}
		printf("\n");
	}

	double r_double_1 = create_reportable_double("r_double_1", v_double_1 / o_double_1);
	printf("# Report: r_double_1 %lf\n", r_double_1);
	assert(r_double_1 == v_double_1 / o_double_1);

	const char * r_string_1 = create_reportable_string("r_string_1", "hello");
	printf("# Report: r_string_1 %s\n", r_string_1);
	assert(strcmp(r_string_1, "hello") == 0);

	const char * r_string_2 = create_reportable_string("r_string_2", "hello world");
	printf("# Report: r_string_2 %s\n", r_string_2);
	assert(strcmp(r_string_2, "hello world") == 0);


	const char * r_string_3 = create_reportable_string("r_string_3", global_string);
	printf("# Report: r_string_3 %s\n", r_string_3);
	assert(strcmp(r_string_3, global_string) == 0);

	const char * r_string_4 = create_reportable_string("r_string_4", get_global_string());
	printf("# Report: r_string_4 %s\n", r_string_4);
	assert(strcmp(r_string_4, global_string) == 0);

	timer t2 = create_timer("timer_2"); // New timer to trigger realloc
	printf("# Start sleep\n");
	sleep(1);
	printf("# End sleep\n");
	stop_timer(&t2);

	const double diff_time1 = getNS_timer(&t1);
	printf("# Timer: T1 %lf\n", diff_time1);
	assert(diff_time1 == 0.);

	const double diff_time2 = getNS_timer(&t2);
	printf("# Times: T2 %lf\n", diff_time2);
	assert(diff_time2 >= 1E9);

	reset_timer(&t2);
	const double diff_time_reset = getNS_timer(&t2);
	printf("# Times: T2 (reset) %lf\n", diff_time_reset);
	assert(diff_time_reset == 0.);

	printf("# Reporting args\n");
	report_args();

	printf("\n# Call free args\n");
	free_args ();
	return 0;
}
