
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

#include <iostream>
#include "argparser.hpp"

#ifdef _MSC_VER
#include <Windows.h>
#define sleep(X) Sleep(X * 1000)
#else
#include <unistd.h>
#endif

const std::string global_string = "Value for reportable in global variable";

int main(int argc, char *argv[])
{
	init_args (argc, argv);

	int v_int_1 = argparser::cl<int>("v_int_1");
	std::string v_string_1 = argparser::cl<std::string>("v_string_1");
	double v_double_1 = argparser::cl<double>("v_double_1");

	int it = 1 + (std::string(argv[1]) == "-json");

	std::cout << "# Read mandatory args: "
	          << v_int_1 << " "
	          << v_string_1 << " "
	          << v_double_1 << " "
	          << std::endl;
	assert(v_int_1 == std::atoi(argv[it++]));
	assert(v_string_1 == std::string(argv[it++]));
	assert(v_double_1 == std::atof(argv[it++]));

	int o_int_1 = argparser::cl<int>("o_int_1", -1);
	std::cout << "# Read: o_int_1 " <<  o_int_1 << std::endl;
	assert(o_int_1 == ((argc > 4 ? atoi(argv[it++]) : -1)));

	double o_double_1 = argparser::cl<double> ("o_double_1", 0.5);
	std::cout << "# Read: o_double_1 " << o_double_1 << std::endl;
	assert(o_double_1 == ((argc > 5 ? atof(argv[it++]) : 0.5)));

	const std::string o_string_1 = argparser::cl<std::string>("o_string_1", std::string());
	std::cout << "# Read: o_string_1 " << o_string_1 << std::endl;
	assert(argc > 6
	       ? (o_string_1 == std::string(argv[it++]))
	       : o_string_1.empty());

	const std::string o_string_2 = argparser::cl<std::string> ("o_string_2", "");
	std::cout << "# Read: o_string_2 " << o_string_2 << std::endl;
	assert(o_string_2 == ((argc > 7 ? argv[it++] : "")));

	const std::string o_string_3 = argparser::cl<std::string> ("o_string_3", "opt1 opt2");
	std::cout << "# Read: o_string_3 " << o_string_3 << std::endl;
	assert(o_string_3 == ((argc > 8 ? argv[it++] : "opt1 opt2")));

	argparser::time t1("timer_1");
	int r_int_1 = argparser::reportable<int>("r_int_1", argc);
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

	double r_double_1 = argparser::reportable<double>("r_double_1", v_double_1 / o_double_1);
	std::cout << "# Report: r_double_1 " << r_double_1 << std::endl;
	assert(r_double_1 == v_double_1 / o_double_1);

	const std::string r_string_1 = argparser::reportable<std::string>("r_string_1", "hello");
	std::cout << "# Report: r_string_1 " << r_string_1 << std::endl;
	assert(r_string_1 == "hello");

	const std::string r_string_2 = argparser::reportable<std::string>("r_string_2", "hello world");
	std::cout << "# Report: r_string_2 " << r_string_2 << std::endl;
	assert(r_string_2 == "hello world");

	const std::string r_string_3 = argparser::reportable<std::string>("r_string_3", global_string);
	std::cout << "# Report: r_string_3 " << r_string_3 << std::endl;
	assert(r_string_3 == global_string);

	argparser::time t2("timer_2"); // New timer to trigger realloc
	std::cout << "# Start sleep" << std::endl;
	sleep(1);
	std::cout << "# End sleep\n" << std::endl;
	t2.stop();

	const double diff_time1 = t1.getNS();
	printf("# Timer: T1 %lf\n", diff_time1);
	assert(diff_time1 == 0.);

	const double diff_time2 = t2.getNS();
	printf("# Times: T2 %lf\n", diff_time2);
	assert(diff_time2 >= 1E9);

	t2.reset();
	const double diff_time_reset = t2.getNS();
	printf("# Times: T2 (reset) %lf\n", diff_time_reset);
	assert(diff_time_reset == 0.);

	std::cout << "# Reporting args" << std::endl;
	argparser::report<>();

	std::cout << "\n# Call free args" << std::endl;

	argparser::free();
	return 0;
}
