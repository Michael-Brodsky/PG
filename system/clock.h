/*
 *	This files defines implementation-specific sources for the std::chrono 
 *	"clock" objects.
 *
 *	***************************************************************************
 *
 *	File: clock.h
 *	Date: September 18, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2021 Michael Brodsky
 *
 *	***************************************************************************
 *
 *  This file is part of "Pretty Good" (Pg). "Pg" is free software:
 *	you can redistribute it and/or modify it under the terms of the
 *	GNU General Public License as published by the Free Software Foundation,
 *	either version 3 of the License, or (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *	WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *	along with this file. If not, see <http://www.gnu.org/licenses/>.
 *
 *	**************************************************************************/

#if !defined __PG_CLOCK_H 
# define __PG_CLOCK_H 20210918L

# include "ctime" // std::time_t type.
# include "ratio"
# include "api.h" // chg to sys/api.h, move pgtypes.h to sys/types.h and clock.h to sys/clock.h

# if defined __PG_HAS_NAMESPACES 

namespace pg
{

#  if defined ARDUINO 
#   define steady_clock_api micros
#   define system_clock_api millis
	using system_clock_period = std::milli;
	using steady_clock_period = std::micro;
#  endif // defined ARDUINO 

	struct steady_clock_t
	{
		using period = steady_clock_period;

		static inline std::time_t now() { return steady_clock_api(); }
	};

	struct system_clock_t
	{
		using period = system_clock_period;

		static inline std::time_t now() { return system_clock_api(); }
	};
}

//namespace std
//{
//	namespace chrono
//	{
//		
//		static std::time_t steady_clock_api() { return clock_api_steady(); }
//		static std::time_t system_clock_api() { return clock_api_system(); }
//	} // namespace chrono
//} // namespace std

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_CLOCK_H 