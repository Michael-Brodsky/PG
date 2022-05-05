/*
 *	This files is part of the Pg type support library . 
 *
 *	***************************************************************************
 *
 *	File: types.h
 *	Date: September 23, 2021
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
 *	**************************************************************************
 *
 *	Description:
 *
 *  This file declares the following types:
 *
 *        pin_t:  An unsigned integral type that holds any GPIO pin number.
 *     analog_t:  An unsigned integral type that represents an analog input 
 *                level (as converted to a digital value (see e.g. 
 *                `analogRead()').
 *	frequency_t:  A floating-point type used to hold a frequency in Hz.
 *
 *  This file defines the following constants:
 *
 *   InvalidPin:  constant indicating an invalid digital GPIO pin number.
 *
 *	**************************************************************************/ 

#if !defined __PG_TYPES_H
# define __PG_TYPES_H 20210923L

//# include "cstdint"
# include "pg.h"

# if !(defined __PIN_T_DEFINED || defined pin_t)
#  if !defined __PIN_T_TYPE
#   define __PIN_T_TYPE uint8_t
#  endif
#  define __PIN_T_DEFINED 1
# endif // !defined __PIN_T_DEFINED ...

# if !defined __ANALOG_T_DEFINED
#  if !defined __ANALOG_T_TYPE
#   define __ANALOG_T_TYPE uint16_t
#  endif
#  define __ANALOG_T_DEFINED 1 
# endif // !defined __ANALOG_T_DEFINED

# if !defined __FREQUENCY_T_DEFINED
#  if !defined __FREQUENCY_T_TYPE
#   define __FREQUENCY_T_TYPE float 
#  endif
#  define __FREQUENCY_T_DEFINED 1 
# endif // !defined __ANALOG_T_DEFINED

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
# if defined __PIN_T_DEFINED
	typedef __PIN_T_TYPE pin_t;				// Unsigned integral type that can hold any GPIO pin number.
	constexpr pin_t InvalidPin = 0xff;		// Constant indicating an invalid GPIO pin number.
# endif

# if defined __ANALOG_T_DEFINED
	typedef __ANALOG_T_TYPE analog_t;		// Unsigned integral type that represents an analog input level.
# endif 

# if defined __FREQUENCY_T_DEFINED
	typedef __FREQUENCY_T_TYPE frequency_t;	// Floating-point type that can hold any frequency in Hz.
# endif

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_TYPES_H
