/*
 *	This file defines a library of cyclic-redundancy-check (CRC) and checksum 
 *	algorithms.
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
 *	     crc(): generates a crc remainder from an input byte stream and 
 *		        generator polynomial.
 *	checksum(): generates an inverted checksum from an input stream.
 * 
 *	The checksum() functions generate a checksum value from an input stream 
 *	(the message). Messages are verified by comparing checksums. If they are 
 *	equal, the messages are identical, else they are not.
 * 
 *	The crc() functions generate a crc remainder from an input stream and 
 *	generator polynomial. Messages are verified by appending the remainder to  
 *	the original message. If the remainder from the appended message is zero
 *	(0), the messages are identical, else they are not.
 *
 *	Notes:
 *
 * 	The checksum() functions operate on streams of any unsigned type and 
 *	return a checksum of the same type: 
 *	
 *		uint32_t msg[] = { 1,2,3,4,5,6,7,8,9 };
 *		uint32_t sum = checksum(std::begin(msg), std::end(msg));
 * 
 *	The crc() functions only operate on unsigned byte streams (unsigned char 
 *	or uint8_t). Generator polynomials must be unsigned types whose widths 
 *	are multiples of 8 bits, i.e. 8, 16, 32, etc., and the functions return a 
 *	a remainder of the same type as the polynomial. The message length, in 
 *	bytes, must be greater than or equal to the size of the polynomial (as 
 *	returned by the sizeof() operator, e.g. a 32-bit polynomial has a minimum 
 *	message length of four bytes). Programs attempting to call these functions 
 *	with other types are ill-formed. 
 * 
 *		uint8_t msg[] = { 1,2,3,4,5,6,7,8,9 };
 *		uint16_t poly = 0x1021;
 *		uint16_t remainder = crc(std::begin(msg), std::end(msg), poly);
 *
 *	**************************************************************************/

#if !defined __PG_CRC_H
# define __PG_CRC_H 20211019L

#include <lib/imath.h>	// is_unsigned type.
#include <array>		// iterator_traits.
#include <limits>		// numeric_limits.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	template<class T>
	struct crc_traits;

	struct crc8 {};				using crc_8 = crc_traits<crc8>;
								using crc_8_ccitt = crc_8;
	struct crc8_bluetooth {};	using crc_8_bluetooth = crc_traits<crc8_bluetooth>;
	struct crc8_cdma2000 {};	using crc_8_cdma2000 = crc_traits<crc8_cdma2000>;
	struct crc8_dallas1w {};	using crc_8_dallas_1_wire = crc_traits<crc8_dallas1w>;
	struct crc8_darc {};		using crc_8_darc = crc_traits<crc8_darc>;
	struct crc8_dvbs2 {};		using crc_8_dvb_s2 = crc_traits<crc8_dvbs2>;
	struct crc8_gsma {};		using crc_8_gsm_a = crc_traits<crc8_gsma>;
	struct crc8_gsmb {};		using crc_8_gsm_b = crc_traits<crc8_gsmb>;
	struct crc8_wcdma {};		using crc_8_wcdma = crc_traits<crc8_wcdma>;
	struct crc16 {};			using crc_16 = crc_traits<crc16>;
								using crc_16_ibm = crc_16;
								using crc_16_ansi = crc_16;
	struct crc16_ccitt {};		using crc_16_ccitt = crc_traits<crc16_ccitt>;
	struct crc16_cdma2000 {};	using crc_16_cdma2000 = crc_traits<crc16_cdma2000>;
	struct crc16_genibus {};	using crc_16_genibus = crc_traits<crc16_genibus>;
	struct crc16_kermit {};		using crc_16_kermit = crc_traits<crc16_kermit>;
	struct crc16_modbus {};		using crc_16_modbus = crc_traits<crc16_modbus>;
	struct crc16_rcrc {};		using crc_16_r_crc = crc_traits<crc16_rcrc>;
	struct crc16_usb {};		using crc_16_usb = crc_traits<crc16_usb>;
	struct crc16_x25 {};		using crc_16_x_25 = crc_traits<crc16_x25>;
	struct crc16_xmodem {};		using crc_16_xmodem = crc_traits<crc16_xmodem>;
	//struct crc16_zmodem {};		using crc_16_zmodem = crc_traits<crc16_zmodem>;
	struct crc32 {};			using crc_32 = crc_traits<crc32>;
	//struct crc32c {};			using crc_32c = crc_traits<crc32c>;
	//struct crc32_mpeg {};		using crc_32_mpeg = crc_traits<crc32_mpeg>;
	//struct crc32_bzip2 {};		using crc_32_bzip2 = crc_traits<crc32_bzip2>;
	//struct crc32_posix {};		using crc_32_posix = crc_traits<crc32_posix>;
	//struct crc32_jam {};		using crc_32_jam = crc_traits<crc32_jam>;
	//struct crc32_xfer {};		using crc_32_xfer = crc_traits<crc32_xfer>;

	template<class T>
	struct crc_traits
	{
		using type = crc_traits<T>;
		using value_type = typename T::value_type;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = T::polynomial;
		static constexpr value_type remainder = T::remainder;
		static constexpr bool reflect_data = T::reflect_data;
		static constexpr bool reflect_remainder = T::reflect_remainder;
		static constexpr value_type final_xor = T::final_xor;
		static constexpr value_type check = T::check;
	};

	template<>
	struct crc_traits<crc8>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x07;
		static constexpr value_type remainder = 0x00;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0x00;
		static constexpr value_type check = 0xF4;
	};

	template<>
	struct crc_traits<crc8_bluetooth>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0xA7;
		static constexpr value_type remainder = 0x00;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0x00;
		static constexpr value_type check = 0x26;
	};

	template<>
	struct crc_traits<crc8_cdma2000>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x9B;
		static constexpr value_type remainder = 0xFF;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0x00;
		static constexpr value_type check = 0xDA;
	};

	template<>
	struct crc_traits<crc8_darc>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x39;
		static constexpr value_type remainder = 0x00;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0x00;
		static constexpr value_type check = 0x15;
	};

	template<>
	struct crc_traits<crc8_dvbs2>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0xD5;
		static constexpr value_type remainder = 0x00;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0x00;
		static constexpr value_type check = 0xBC;
	};

	template<>
	struct crc_traits<crc8_dallas1w>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x31;
		static constexpr value_type remainder = 0x00;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0x00;
		static constexpr value_type check = 0xA1;
	};

	template<>
	struct crc_traits<crc8_gsma>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x1D;
		static constexpr value_type remainder = 0x00;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0x00;
		static constexpr value_type check = 0x37;
	};

	template<>
	struct crc_traits<crc8_gsmb>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x49;
		static constexpr value_type remainder = 0x00;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0xFF;
		static constexpr value_type check = 0x94;
	};

	template<>
	struct crc_traits<crc8_wcdma>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x9B;
		static constexpr value_type remainder = 0x00;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0x00;
		static constexpr value_type check = 0x25;
	};

	template<>
	struct crc_traits<crc16>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x8005;
		static constexpr value_type remainder = 0x0000;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0x0000;
		static constexpr value_type check = 0xBB3D;
	};

	template<>
	struct crc_traits<crc16_ccitt>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x1021;
		static constexpr value_type remainder = 0x1D0F;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0x0000;
		static constexpr value_type check = 0xE5CC;
	};

	template<>
	struct crc_traits<crc16_cdma2000>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0xC867;
		static constexpr value_type remainder = 0xFFFF;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0x0000;
		static constexpr value_type check = 0x4C06;
	};

	template<>
	struct crc_traits<crc16_genibus>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x1021;
		static constexpr value_type remainder = 0xFFFF;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0xFFFF;
		static constexpr value_type check = 0xD64E;
	};

	template<>
	struct crc_traits<crc16_kermit>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x1021;
		static constexpr value_type remainder = 0x0000;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0x0000;
		static constexpr value_type check = 0x2189;
	};

	template<>
	struct crc_traits<crc16_modbus>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x8005;
		static constexpr value_type remainder = 0xFFFF;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0x0000;
		static constexpr value_type check = 0x4B37;
	};

	template<>
	struct crc_traits<crc16_rcrc> // also dect_r
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x0589;
		static constexpr value_type remainder = 0x0000;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0x0001;
		static constexpr value_type check = 0x007E;
	};

	template<>
	struct crc_traits<crc16_usb>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x8005;
		static constexpr value_type remainder = 0xFFFF;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0xFFFF;
		static constexpr value_type check = 0xB4C8;
	};

	template<>
	struct crc_traits<crc16_x25>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x1021;
		static constexpr value_type remainder = 0xFFFF;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0xFFFF;
		static constexpr value_type check = 0x906E;
	};
	template<>
	struct crc_traits<crc16_xmodem>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x1021;
		static constexpr value_type remainder = 0x0000;
		static constexpr bool reflect_data = false;
		static constexpr bool reflect_remainder = false;
		static constexpr value_type final_xor = 0x0000;
		static constexpr value_type check = 0x31C3;
	};

	template<>
	struct crc_traits<crc32>
	{
		using value_type = uint32_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type polynomial = 0x04C11DB7;
		static constexpr value_type remainder = 0xFFFFFFFF;
		static constexpr bool reflect_data = true;
		static constexpr bool reflect_remainder = true;
		static constexpr value_type final_xor = 0xFFFFFFFF;
		static constexpr value_type check = 0xCBF43926;
	};

	namespace details
	{

		template<class Poly>
		struct crc_shift
		{
			// Returns the width in bits of type Poly.
			static constexpr Poly width() { return std::numeric_limits<Poly>::digits; }
			// Returns 2**(w-1), where w is the width in bits of type Poly.
			static constexpr Poly top() { return (Poly(1) << (width() - 1)); }
		};

		template<class T>
		T crc_reflect(T value)
		{
			T result = 0;

			for (uint8_t i = 0; i < std::numeric_limits<T>::digits; ++i, value >>= 1)
				result = (result << 1) | (value & 0x01);

			return result;
		}

		// Returns the CRC remainder for byte stream [first,last) using polynomial poly.
		template<class InputIt, class Poly>
		typename std::enable_if<
			(std::is_same<InputIt, uint8_t*>::value || std::is_same<InputIt, unsigned char*>::value) &&
			std::is_unsigned<Poly>::value, Poly>::type
			crc_impl(InputIt first, InputIt last, Poly poly, Poly xorin = 0, Poly xorout = 0, bool refin = false, bool refout = false)
		{
			// Message length must be >= size of generator polynomial, in bytes.
			assert(std::distance(first, last) >= sizeof(Poly));

			using value_type = typename std::iterator_traits<InputIt>::value_type;
			using shift_type = crc_shift<Poly>;
			Poly rem = xorin;

			for (; first != last; ++first)
			{
				value_type in = refin ? crc_reflect(*first) : *first;
				//if (refin) *first = crc_reflect(*first);
				rem ^= (Poly(in) << (shift_type::width() - CHAR_BIT));
				for (uint8_t j = 0; j < CHAR_BIT; ++j)
					rem = rem & shift_type::top()
					? (rem << 1) ^ poly
					: (rem << 1);
			}

			return xorout ^ (refout ? crc_reflect(rem) : rem);
		}

	} // namespace details

	// Returns the CRC remainder for byte stream [first,last) using polynomial poly.
	template<class InputIt, class Poly> inline
	Poly crc(InputIt first, InputIt last, Poly poly)
	{
		return details::crc_impl(first, last, poly);
	}

	// Returns the CRC remainder for byte stream [first,last) using polynomial poly.
	template<class InputIt, class T> inline
	typename crc_traits<T>::value_type crc(InputIt first, InputIt last, crc_traits<T> crc)
	{
		using crc_type = crc_traits<T>;

		return details::crc_impl(first, last, crc_type::polynomial, crc_type::remainder, 
			crc_type::final_xor, crc_type::reflect_data, crc_type::reflect_remainder);
	}

	// Returns the checksum for stream range [first, last).
	template<class InputIt> 
	typename details::is_unsigned<typename std::iterator_traits<InputIt>::value_type>::type
		checksum(InputIt first, InputIt last)
	{
		typename std::iterator_traits<InputIt>::value_type cs = 0;

		for (; first != last; ++first)
			cs += *first;

		return ~cs;
	}

	// Returns the checksum for stream array arr.
	template<class T, std::size_t N> inline 
	typename details::is_unsigned<T>::type checksum(T(&arr)[N])
	{
		return checksum(arr, arr + N);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_CRC_H


