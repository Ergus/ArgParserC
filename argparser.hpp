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
#include <type_traits>

namespace {
	#include "argparser.h"

	using string = std::string;

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

	template <typename T, bool cl>
	class reportable_t {
		T _value;

	public:

		template<bool B2 = cl, typename std::enable_if_t<B2, int> = 0>
		explicit reportable_t(const std::string &name)
		{
			#define F(N,...)										\
			if constexpr (std::is_same<T, N>::value == true)		\
				_value = N(create_cl_##N(name.c_str()));
			TYPES
			#undef F
		}

		reportable_t(const std::string &name, const T value)
		{
			if constexpr (cl) {
				#define F(N,...)														\
					if constexpr (std::is_same<T, N>::value == true)	\
						_value = N(create_optional_cl_##N(name.c_str(), trait<T>::convert(value)));
				TYPES
				#undef F
			} else {
				#define F(N,...)														\
					if constexpr (std::is_same<T, N>::value == true)	\
						_value = N(create_reportable_##N(name.c_str(), trait<T>::convert(value)));
				TYPES
				#undef F
			}
		}

		operator T() { return _value; }
	};
}

namespace argparser {

	template <typename T>
	using cl = reportable_t<T, true>;

	template <typename T>
	using reportable = reportable_t<T, false>;

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
