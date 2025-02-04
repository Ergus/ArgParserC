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
#include "argparser.h"
#include <vector>

namespace {

	using string = std::string;

	/**
	   These specialized traits are needed to handle the strings properly

	   Remember that C strings are very problematic to handle securely and the 
	   C backed we have uses several tricks to manage them. So, we need the
	   special traits in order to have a latter simpler code.
	*/
	template <typename T>
	struct trait{
		static T convert(const T& in)
		{
			return in;
		}
	};

	template <>
	struct trait<std::string>{
		static const char* convert(const std::string& in)
		{
			return in.c_str();
		}
	};

	/**
	   C++ wrapper template class for reportables

	   This includes the command line arguments and explicit reportables.
	   We use a dirty macro trick to generate specialized code that calls the
	   right C backend.
	   This is the best example of how macros+templates should NEVER be used
	   together.
	*/
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
	void report()
	{
		report_args();
	}

	void free()
	{
		free_args();
	}

	std::vector<std::string> get_rest()
	{
		char **rest;
		int nrest = get_rest_args(&rest);

		std::vector<std::string> result(nrest);

		for(int i = 0; i < nrest; ++i)
			result[i] = std::string(rest[i]);

		return result;
	}

	class time {
		timer t;

	public:
		explicit time(const std::string& name)
			: t(create_timer(name.c_str()))
		{}

		double getNS() const { return getNS_timer(&t); }
		void start() { start_timer(&t); }
		void stop() { stop_timer(&t); }
		void reset() { reset_timer(&t); }

		friend std::ostream& operator<<(std::ostream& os, const time& self)
		{
			char buffer[256];
			snprintf_timer(&self.t, buffer, 255);
			os << std::string(buffer);
			return os;
		}
	};

};
