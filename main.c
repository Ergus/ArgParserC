
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

int main(int argc, char *argv[])
{
	init_args (argc, argv);

	int v_int = create_cl_int ("vint");
	char *v_char_p = create_cl_char_p ("vchar_p");
	double v_double = create_cl_double ("vdouble");

	int o_int = create_optional_cl_int ("oint", -1);
	int o_double = create_optional_cl_double ("odouble", 0.5);
	char *o_char = create_optional_cl_char_p ("ochar_p", NULL);

	timer *t = create_timer("My_timer");
	create_reportable_int ("reportable", argc);

	free_timer(t);
	report_args ();
	free_args ();
	return 0;
}
