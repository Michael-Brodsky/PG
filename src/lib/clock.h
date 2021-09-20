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

# if defined ARDUINO 
#  include "Arduino.h"
#  define clock_api micros
# endif

# include "ctime" // std::time_t type.

# if defined __PG_HAS_NAMESPACES 

namespace std
{
	namespace chrono
	{
		static std::time_t steady_clock_api() { return clock_api(); }
	}
}

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_CLOCK_H 