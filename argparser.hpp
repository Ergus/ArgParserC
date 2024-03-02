/*
 * Copyright (C) 2024  Jimmy Aguilar Mena
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

#pragma once

#include <string>

namespace {
#include "argparser.h"

	template <typename T>
	struct trait{
		using type = T;
		static type convert(const T& in)
		{
			return in;
		}
	};

	template <>
	struct trait<std::string>{
		using type = const char*;
		static const char* convert(const std::string& in)
		{
			return in.c_str();
		}
	};
}

namespace argparser {

	using string = std::string;

	template <typename T>
	T cl(const std::string &name)
	{
		#define F(N,...)											 \
			if constexpr (std::is_same<T, N>::value == true)		\
				return N(create_cl_##N(name.c_str()));
			TYPES
		#undef F
	}

	template <typename T>
	T cl(const std::string &name, const T value)
	{
		#define F(N,...)												\
			if constexpr (std::is_same<T, N>::value == true)			\
				return N(create_optional_cl_##N(name.c_str(), trait<T>::convert(value)));
			TYPES
		#undef F
	}

	template <typename T>
	T reportable(const std::string &name, const T value)
	{
		#define F(N,...)									\
			if constexpr (std::is_same<T, N>::value == true) \
				return N(create_reportable_##N(name.c_str(), trait<T>::convert(value)));
			TYPES
		#undef F
	}

	void init(int argc, char **argv) {
		init_args(argc, argv);
	}

	template<bool json = false>
	void report() {
		if constexpr(json)
			report_args_json();
		else
			report_args();
	}

	void free() {
		free_args();
	}

	int get_rest(char **rest) {
		return get_rest_args(&rest);
	}
};
