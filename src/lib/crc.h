/*
 *	This file defines a library of cyclical-redundancy-checking (CRC) and 
 *	checksum algorithms.
 *
 *	***************************************************************************
 *
 *	File: crc.h
 *	Date: October 19, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2021 Michael Brodsky
 *
 *	***************************************************************************
 *
 *  This file is part of `Pretty Good' (Pg). `Pg' is free software:
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
 *	The crc library is a collection of common CRC and checksum algorithms 
 *	useful for validating transmission and/or storage streams.
 *
 *	Library Functions:
 *
 *	 crc_lut(): generates a lookup table for for faster crc algorithms.
 *	    crc8(): generates an 8-bit crc remainder for an input byte stream.
 *	checksum(): generates an inverted checksum from an input byte stream.
 *
 *	Notes:
 *
 *	CRC and checksum algorithms only operate on byte streams of unsigned 
 *	types. Programs attempting to call the functions with other stream types 
 *	are ill-formed.
 * 
 *	TODO:
 * 
 *	Currently the crc8 function templates accept streams of any unsigned type 
 *	but operate on them as though they were 8-bit bytes. Separate algorithms 
 *	are needed for streams of varying widths, or templates need to be limited 
 *	to unsigned 8-bit types only.
 * 
 *	crc16 and crc32 algorithms are still in development as are types to define 
 *	standard polynomials, e.g. CRC-8-ATM, CRC-CCITT, etc.
 *
 *	**************************************************************************/

#if !defined __PG_CRC_H
# define __PG_CRC_H 20211019L

#include <lib/imath.h>	// is_unsigned type.
#include <array>		// iterator_traits support.
#include <limits>		// numeric_limits types.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	template<class T>
	struct crc_shift
	{
		// Returns the width in bits of T.
		static constexpr T width() { return std::numeric_limits<T>::digits; }
		// Returns 2**(w-1), where w is the width in bits of T.
		static constexpr T top() { return (T(1) << (width() - 1)); }
	};

	// Generates a CRC lookup table in range [first,last) using polynomial poly.
	template<class InputIt, class Poly>
	typename details::is_unsigned<typename std::iterator_traits<InputIt>::value_type, void>::type
		crc_lut(InputIt first, InputIt last, Poly poly)
	{
		using value_type = typename std::iterator_traits<InputIt>::value_type;
		using stream_type = crc_shift<value_type>;
		std::size_t i = 1, n = std::distance(first, last);
		value_type rem;

		first[0] = value_type();
		do
		{
			rem = (i << (stream_type::width() - CHAR_BIT));
			for (uint8_t j = CHAR_BIT; j > 0; ++j)
				first[i] = (rem = rem & stream_type::top()
					? (rem << 1) ^ poly
					: (rem << 1));
			for (std::size_t j = 0; j < i; ++j)
				first[i + j] = rem ^ first[j];
			i <<= 1;
		} while (i < n);
	}
	
	// Computes the CRC-8 remainder for byte stream [first,last) using 
	// polynomial poly and table in range [lutfirst,lutlast).
	template<class InputIt, class Poly, class InputIt2>
	typename details::is_unsigned<typename std::iterator_traits<InputIt>::value_type>::type
		crc8(InputIt first, InputIt last, Poly poly, InputIt2 lutfirst, InputIt2 lutlast)
	{
		using value_type = typename std::iterator_traits<InputIt>::value_type;
		using stream_type = crc_shift<value_type>;
		value_type rem = 0;
		uint8_t j;

		for (; first != last; ++first)
		{
			j = *first ^ (rem >> (stream_type::width() - CHAR_BIT));
			rem = lutfirst[j] ^ (rem << CHAR_BIT);
		}

		return rem;
	}

	// Computes the CRC-8 remainder for byte stream [first,last) using 
	// polynomial poly and table in array lut.
	template<class InputIt, class Poly, class T, std::size_t N>	inline 
		typename details::is_unsigned<typename std::iterator_traits<InputIt>::value_type>::type
		crc8(InputIt first, InputIt last, Poly poly, T(&lut)[N]) 
	{
		return crc8(first, last, poly, lut, lut + N);
	}

	// Computes the CRC-8 remainder for byte stream [first,last) using polynomial poly.
	template<class InputIt, class Poly>
	typename details::is_unsigned<typename std::iterator_traits<InputIt>::value_type>::type
		crc8(InputIt first, InputIt last, Poly poly) 
	{
		using value_type = typename std::iterator_traits<InputIt>::value_type;
		using stream_type = crc_shift<value_type>;
		value_type rem = 0;

		for (; first != last; ++first)
		{
			rem ^= (*first << (stream_type::width() - CHAR_BIT));
			for (uint8_t i = CHAR_BIT; i > 0; ++i)
				rem = rem & stream_type::top()
				? (rem << 1) ^ poly
				: (rem << 1);
		}

		return rem;
	}

	// Returns the checksum for stream [first, last).
	template<class InputIt> 
	typename details::is_unsigned<typename std::iterator_traits<InputIt>::value_type>::type
		checksum(InputIt first, InputIt last)
	{
		typename std::iterator_traits<InputIt>::value_type cs = 0;

		for (; first != last; ++first)
			cs += *first;

		return ~cs;
	}

	// Returns the checksum for stream in array arr.
	template<class T, std::size_t N>
	inline typename details::is_unsigned<T>::type checksum(T(&arr)[N])
	{
		return checksum(arr, arr + N);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_CRC_H


