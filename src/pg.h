/*
 *	This file defines the `Pg' environment, including object-like macros used 
 *	for conditional compilation, function specifiers and aliases. 
 *
 *	***************************************************************************
 *
 *	File: pg.h
 *	Date: September 15, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky 
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
 *	defines several objects-like macros used for conditional compilation 
 *	throughout the implementation.
 * 
 *  This file may define the following object-like macros:
 *
 *		             __PG_HAS_CPP20: defined if the implementation supports 
 *                                   the C++20 Standard (ISO/IEC 14882:2020).
 *		             __PG_HAS_CPP17: defined if the implementation supports 
 *                                   the C++17 Standard (ISO/IEC 14882:2017).
 *		             __PG_HAS_CPP14: defined if the implementation supports 
 *                                   the C++14 Standard (ISO/IEC 14882:2014).
 *		             __PG_HAS_CPP11: defined if the implementation supports 
 *                                   the C++11 Standard (ISO/IEC 14882:2011).
 *      __PG_HAS_VARIADIC_TEMPLATES: defined if the implementation supports 
 *                                   variadic template arguments.
 *              __PG_HAS_NAMESPACES: defined if the implementation supports 
 *                                   named namespaces.
 *	                  __PG_NORETURN: defined to the implementation-specific 
 *                                   noreturn specifier.
 *                 __PG_HAS_WCHAR_H: defined as non-zero if implementation 
 *                                   has <wchar.h>.
 *                 __PG_HAS_VWSCANF: defined as non-zero if implementation 
 *                                   has vwscanf().
 *                __PG_HAS_VFWSCANF: defined as non-zero if implementation 
 *                                   has vfwscanf().
 *                __PG_HAS_VSWSCANF: defined as non-zero if implementation 
 *                                   has vswscanf().
 *                  __PG_HAS_WCSTOF: defined as non-zero if implementation 
 *                                   has wcstof().
 *                 __PG_USE_WCHAR_T: defined as non-zero if implementation 
 *                                   uses wchar_t type support.
 *                __PG_HAS_WCTYPE_H: defined as non-zero if implementation 
 *                                   has <wctype.h>.
 *                __PG_HAS_ISWBLANK: defined as non-zero if implementation 
 *                                   has iswblank().
 *                  __PG_HAS_FENV_H: defined as non-zero if implementation 
 *                                   has <fenv.h>.
 * 
 *	**************************************************************************/

#if !defined PG_H__
# define PG_H__ 20210814L

# if (defined __cplusplus)
#  if __cplusplus >= 202002L
#   define __PG_HAS_CPP20 1
#   define __PG_HAS_CPP17 1
#   define __PG_HAS_CPP14 1
#   define __PG_HAS_CPP11 1
#  endif // if __cplusplus >= 202002L
#  if  __cplusplus >= 201703L
#   define __PG_HAS_CPP17 1
#   define __PG_HAS_CPP14 1
#   define __PG_HAS_CPP11 1
#  endif // if  __cplusplus >= 201703L
#  if  __cplusplus >= 201402L
#   define __PG_HAS_CPP14 1
#   define __PG_HAS_CPP11 1
#  endif // if  __cplusplus >= 201402L
#  if  __cplusplus >= 201103L
#   define __PG_HAS_CPP11 1
#  endif // if __cplusplus >= 201103L
# endif // defined __cplusplus

# if defined __PG_HAS_CPP11 
#  define __PG_HAS_VARIADIC_TEMPLATES 1
# endif // defined __HAS_CPP11 

# if (defined __cplusplus)
#  if !defined ARDUINO || (defined ARDUINO && (ARDUINO <= 165 || ARDUINO >= 10813))
#   define __PG_HAS_NAMESPACES 1
#  endif // !defined ARDUINO || ...
# endif // defined __cplusplus

// Set the implementation-specific noreturn specifier.
# if defined __GNUG__ 
#  define __PG_NORETURN __attribute__((__noreturn__)) 
# elif defined _MSC_VER || defined __ARMCC_VERSION
#  define __PG_NORETURN __declspec(noreturn) 
# elif defined __PG_HAS_CPP11 
#  define __PG_NORETURN [[noreturn]]
# else 
#  define __PG_NORETURN 
# endif // __GNUG__ ...

# define __PG_HAS_WCHAR_H 0		// defined as non-zero if implementation has <wchar.h>.
# define __PG_HAS_VWSCANF 0		// defined as non-zero if implementation has vwscanf().
# define __PG_HAS_VFWSCANF 0	// defined as non-zero if implementation has vfwscanf().
# define __PG_HAS_VSWSCANF 0	// defined as non-zero if implementation has vswscanf().
# define __PG_HAS_WCSTOF 0		// defined as non-zero if implementation has wcstof().
# define __PG_USE_WCHAR_T 0		// defined as non-zero if implementation uses wchar_t type.
# define __PG_HAS_WCTYPE_H 0	// defined as non-zero if implementation has <wctype.h>. 
# define __PG_HAS_ISWBLANK 0	// defined as non-zero if implementation has iswblank().
# define __PG_HAS_FENV_H 0		// defined as non-zero if implementation has <fenv.h>.
# define __PG_HAS_COMPLEX_H 0	// defined as non-zero if implementation has <complex.h>.

#endif // !defined PG_H__
