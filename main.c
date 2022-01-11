
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

#include <unistd.h>

int main(int argc, char *argv[])
{
	init_args (argc, argv);

	int v_int_1 = create_cl_int ("vint_1");
	char *v_char_1 = create_cl_char_p ("vchar_1");
	double v_double_1 = create_cl_double ("vdouble_1");

	int o_int_1 = create_optional_cl_int ("oint_1", -1);
	double o_double_1 = create_optional_cl_double ("odouble_1", 0.5);
	char *o_char_1 = create_optional_cl_char_p ("ochar_p_1", NULL);
	char *o_char_2 = create_optional_cl_char_p ("ochar_p_2", "");

	timer t = create_timer("My_timer");
	create_reportable_int ("reportable", argc);

	printf ("Second timer to trigger\n");
	create_reportable_double ("Reportable2", v_double_1 / o_double_1);

	timer t2 = create_timer("My_timer2"); // New timer to trigger realloc
	usleep (1000);
	stop_timer(&t2);

	report_args();

	report_args_json();

	free_args ();
	return 0;
}
