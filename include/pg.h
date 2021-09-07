/*
 *	This file defines the `Pg' environment, including object-like macros used 
 *	for conditional compilation, memory models and arithmetic types defined in 
 *	the C++ `std' namespace.
 *
 *	***************************************************************************
 *
 *	File: pg.h
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
 *	This is the base file for the `pg' implementation on the Arduino. It 
 *	defines several objects and types used by dependent files throughout the 
 *	implementation.
 * 
 *  This file defines the following object-like macros:
 *
 *      __HAS_VARIADIC_TEMPLATES: defined if the implementation supports 
 *                                variadic template arguments.
 *              __HAS_NAMESPACES: defined if the implementation supports 
 *                                named namespaces.
 * 
 *  (Note: several other macros are defined by this file, however they are 
 *  only used internally here and are therefore not listed.)
 *
 *  This file declares the following types in the `std' namespace:
 *
 *	   std::time_t:  An unsigned integral type that represents a time interval.
 *     std::size_t:  An unsigned integral type that can store the size of any  
 *                   object depending on the memory model used.
 *  std::ptrdiff_t:  An signed integral type returned by pointer arithmetic 
 *                   operations, depending on the memory model used.
 *	 std::intmax_t:	 The maximum-width signed integer type. 
 *	std::uintmax_t:	 The maximum-width unsigned integer type.
 * 
 *	The `std::size_t' and `std::ptrdiff_t' default to the native Arduino 
 *	`size_t' and `ptrdiff_t' types if no memory model is defined.
 * 
 *	**************************************************************************/

#if !defined PG_H__
# define PG_H__ 20210814L

# include <stddef.h>	// Common implementation-specific typedefs.
# include <limits.h>	// C-stdlib integral limits.
# include <stdint.h>	// C99 exact-width integral types.

# if (defined __cplusplus)
#  if __cplusplus >= 202002L
#   define __HAS_CPP20 1
#  elif  __cplusplus >= 201703L
#   define __HAS_CPP17 1
#  elif  __cplusplus >= 201402L
#   define __HAS_CPP14 1
#  elif  __cplusplus >= 201103L
#   define __HAS_CPP11 1
#  endif // if __cplusplus >= ...
# endif // defined __cplusplus

# if defined __HAS_CPP11 
#  define __HAS_VARIADIC_TEMPLATES 1
# endif // defined __HAS_CPP11 

# if (defined __cplusplus)
#  if !defined ARDUINO || (defined ARDUINO && (ARDUINO <= 165 || ARDUINO >= 10813))
#   define __HAS_NAMESPACES 1
#  endif // !defined ARDUINO || ...
# endif // defined __cplusplus

# if !defined LLONG_MAX
#  define LLONG_MAX     9223372036854775807LL
# endif
# if !defined LLONG_MIN
#  define LLONG_MIN		(-9223372036854775807LL - 1)
# endif
# if !defined ULLONG_MAX
#  define ULLONG_MAX    0xffffffffffffffffULL
# endif

# if !defined __TIME_TYPE_DEFINED 
#  if !defined __TIME_TYPE
#   define __TIME_TYPE unsigned long
#  endif
#  define __TIME_TYPE_DEFINED 1
# endif // !defined __PIN_T_DEFINED ...

# if !defined __SIZE_TYPE_DEFINED 
#  if ! defined __SIZE_TYPE
#   if defined __MEMORY_MODEL_TINY
#    define __SIZE_TYPE uint8_t 
#   elif defined __MEMORY_MODEL_HUGE
#    define __SIZE_TYPE unsigned long
#   else
#    define __SIZE_TYPE size_t
#   endif
#  endif
#  define __SIZE_TYPE_DEFINED 1
# endif // !defined __SIZE_TYPE_DEFINED

# if !defined __PTRDIFF_TYPE_DEFINED 
#  if ! defined __PTRDIFF_TYPE
#   if defined __MEMORY_MODEL_TINY
#    define __PTRDIFF_TYPE int8_t 
#   elif defined __MEMORY_MODEL_HUGE
#    define __PTRDIFF_TYPE signed long
#   else
#    define __PTRDIFF_TYPE ptrdiff_t
#   endif
#  endif
#  define __PTRDIFF_TYPE_DEFINED 1
# endif // !defined __PTRDIFF_TYPE_DEFINED 

# if !defined __INTMAX_TYPE_DEFINED 
#  if ! defined __INTMAX_TYPE
#   define __INTMAX_TYPE intmax_t 
#  endif
#  define __INTMAX_TYPE_DEFINED 1
# endif // !defined __INTMAX_TYPE_DEFINED 

# if !defined __UINTMAX_TYPE_DEFINED 
#  if ! defined __UINTMAX_TYPE
#   define __UINTMAX_TYPE uintmax_t
#  endif
#  define __UINTMAX_TYPE_DEFINED 1
# endif // !defined __UINTMAX_TYPE_DEFINED 

# if defined __HAS_NAMESPACES 
namespace std
{
#  if defined __SIZE_TYPE_DEFINED
	typedef __SIZE_TYPE size_t;			// Unsigned integral type can store the maximum size of a theoretically possible object of any type.
#  endif
#  if defined __PTRDIFF_TYPE_DEFINED
	typedef __PTRDIFF_TYPE ptrdiff_t;	// Signed integral type of the result of subtracting two pointers.
#  endif
#  if defined __TIME_TYPE_DEFINED 
	typedef __TIME_TYPE time_t;			// Unsigned integral type that represents a time interval.
#  endif
#  if defined __INTMAX_TYPE_DEFINED
	typedef __INTMAX_TYPE intmax_t;		// Maximum-width signed integral type.
#  endif
#  if defined __UINTMAX_TYPE_DEFINED
	typedef __UINTMAX_TYPE uintmax_t;	// Maximum-width unsigned integral type.
#  endif
} // namespace std
# endif // defined __HAS_NAMESPACES

#endif // !defined PG_H__
