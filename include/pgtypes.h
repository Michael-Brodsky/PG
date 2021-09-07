/*
 *	This file defines several plain-old-data (POD) types and constants that are
 *	useful for adding context to type declarations and fixing type-correctness
 *	issues in the Arduino API.
 *
 *	***************************************************************************
 *
 *	File: pgtypes.h
 *	Date: August 14, 2021
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
 *  Description:
 *
 *  This file declares the following types:
 *
 *        pin_t: An unsigned integral type that holds any GPIO pin number.
 *     analog_t: An unsigned integral type that represents an analog input
 *               level (as converted to a digital value (see e.g.
 *               `analogRead()').
 *
 *  This file defines the following constants:
 *
 *   InvalidPin: constant indicating an invalid GPIO pin number.
 *	  AnalogMax: maximum value an object of type `analog_t' should hold.
 *                                
 *	**************************************************************************/

#if !defined PGTYPES_H__
# define PGTYPES_H__ 20210814L

# include "pg.h"	// Pg environment.

# if !(defined __PIN_T_DEFINED || defined pin_t)
#  if !defined __PIN_T_TYPE
#   define __PIN_T_TYPE uint8_t
#  endif
typedef __PIN_T_TYPE pin_t;				// Unsigned integral type that can hold any GPIO pin number.
#  define __PIN_T_DEFINED 1
# endif // !defined __PIN_T_DEFINED ...

# if (defined __PIN_T_DEFINED && !defined __INVALID_PIN_DEFINED)
static const pin_t InvalidPin = 0xff;	// Constant indicating an invalid GPIO pin number.
#  define __INVALID_PIN_DEFINED 1
# endif // ... !defined __INVALID_PIN_DEFINED

# if !defined __ANALOG_T_DEFINED
#  if !defined __ANALOG_T_TYPE
#   define __ANALOG_T_TYPE uint16_t
#  endif
typedef __ANALOG_T_TYPE analog_t;		// Unsigned integral type that represents an analog input level.
#  define __ANALOG_T_DEFINED 1 
#  if defined ANALOG_MAX 
#   undef ANALOG_MAX
#  endif
#  define ANALOG_MAX 1023U				// Maximum value an object of type `analog_t' should hold.
# endif // !defined __ANALOG_T_DEFINED

#endif // !defined PGTYPES_H__
