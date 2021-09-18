/* 
 *	This file defines a library of function templates for dealing with bit
 *	fields and bitwise operations.
 *
 *	**************************************************************************
 * 
 *	File: bits.h
 *  Date: December 8, 2015
 *  Version: 1.0
 *  Author: Michael Brodsky
 *  Email: mbrodskiis@gmail.com
 *  Copyright (c) 2012-2021 Michael Brodsky 

 *  **************************************************************************
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
 *  ***************************************************************************
 *
 *	Description:
 *
 *		The bits.h library functions operate on and return unsigned integral 
 *		types, treating them as bit-fields. The algorithms avoid branchung and 
 *		are optimized for speed. Function arguments must be unsigned integral 
 *		types, otherwise the program is ill-formed.
 * 
 *		bit(n): References the n-th bit in a bitfield.
 *		bitset(b, n): Sets the n-th bit in b.
 *		bitclr(b, n): Clears the n-th bit in b.
 *		bitflip(b, n): Complements the n-th bit in b.
 *		bitchg(b, n, f): Changes the n-th bit in b to the value of f.
 *		bitmerge(a, b, m): Merges the non-masked bits in a with the masked bits in b according to mask m.
 *		bitrev(b): Reverses the bit order of b.
 *		bitsetm(b, m): Sets the bits in b set in mask m.
 *		bitclrm(b, m): Clears the bits in b set in mask m.
 *		bitchgm(b, m, f): Changes the bits in b set in mask m to the value of f.
 *		bitflipm(b, m): Complements the bits in b set in mask m.
 *		bitisset(b, n): Checks whether the n-th bit in b is set.
 *		bitissetm(b, m): Checks whether the bits set in mask m are set in b.
 *		bitlsbset(b): Returns the rank of the least significant bit set in b.
 *		bitmlsbclr(b): Returns the mask of the least significant bit not set in b.
 *		bitnset(b): Returns the number of bits set in b.
 *		bitnlsbclr(b): Returns the number of consecutive lsbs not set in b.
 *		bitparity(b): Returns the bit parity of b.
 *		bitswap(a, b): Swaps the two values a and b.
 *		widthof<T>(): Returns the width of type T in bits.
 *		widthof(t): Returns the width of t in bits.
 *		bitintlv(x, y): Interleves the bits in x with those in y.
 * 
 *  **************************************************************************/

#if !defined __PG_BITS_H
# define __PG_BITS_H  2051215L

# include "cstddef" // CHAR_BIT
# include "imath.h"	// Template substitution helpers.
 
# if defined __PG_HAS_NAMESPACES

// Remove any conflicting macros defined by the implementation.

#  undef bit
#  undef bitset
#  undef bitclr
#  undef bitflip
#  undef bitchg
#  undef bitmerge
#  undef bitrev
#  undef bitsetm
#  undef bitclrm
#  undef bitchgm
#  undef bitflipm
#  undef bitissetm
#  undef bitlsbset
#  undef bitmlsbclr
#  undef bitnset
#  undef bitnlsbclr
#  undef bitparity
#  undef bitswap
#  undef widthof
#  undef bitintlv

namespace pg
{
	namespace details
	{
		template<class T> inline
			T bitrev_8(T b)
		{
			return  (b = (((b & 0xaa) >> 1) | ((b & 0x55) << 1)), 
				b = (((b & 0xcc) >> 2) | ((b & 0x33) << 2)), 
				b = (((b & 0xf0) >> 4) | ((b & 0x0f) << 4)));
		}

		template<class T> inline
			T bitrev_16(T b)
		{
			return  (b = bitrev_8(b), 
				(b = (((b & 0xff00) >> 8) | ((b & 0x00ff) << 8))));
		}

		template<class T> inline
			T bitrev_32(T b)
		{
			return (b = bitrev_16(b), 
				(b = (((b & 0x0000ffff) << 16) | ((b & 0xffff0000) >> 16))));
		}

		template<class T> inline
			T bitrev_64(T b)
		{
			return (b = (((b & 0x5555555555555555) << 1) | ((b & 0xaaaaaaaaaaaaaaaa) >> 1)), 
				b = (((b & 0x3333333333333333) << 2) | ((b & 0xcccccccccccccccc) >> 2)), 
				b = (((b & 0x0f0f0f0f0f0f0f0f) << 4) | ((b & 0xf0f0f0f0f0f0f0f0) >> 4)), 
				b = (((b & 0x00ff00ff00ff00ff) << 8) | ((b & 0xff00ff00ff00ff00) >> 8)), 
				b = (((b & 0x0000ffff0000ffff) << 16) | ((b & 0xffff0000ffff0000) >> 16)), 
				b = (((b & 0x00000000ffffffff) << 32) | ((b & 0xffffffffffffffff) >> 32)));
		}

		template <class T, size_t N>
		struct bitrev_impl;

		template <class T>
		struct bitrev_impl<T, 1>
		{
			T operator()(T b) const { return bitrev_8(b); }
		};

		template <class T>
		struct bitrev_impl<T, 2>
		{
			T operator()(T b) const { return bitrev_16(b); }
		};

		template <class T>
		struct bitrev_impl<T, 4>
		{
			T operator()(T b) const { return bitrev_32(b); }
		};

		template <class T>
		struct bitrev_impl<T, 8>
		{
			T operator()(T b) const { return bitrev_64(b); }
		};
	} // namespace details

#if defined CHAR_BIT
	/* Returns the width of type T in bits. */
	template<class T>
	inline typename details::is_integer<T, size_t>::type
		widthof() { return (sizeof(T) * CHAR_BIT); }

	/* Returns the width of t in bits. */
	template<class T>
	inline typename details::is_integer<T, size_t>::type
		widthof(T t) { return widthof<T>(); }
#endif // defined CHAR_BIT

	/* References the n-th bit in a bitfield. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bit(uint8_t n) { return (static_cast<T>(1) << n); }

	/* Sets the n-th bit in b. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitset(T b, uint8_t n) { return (b | bit<T>(n)); }

	/* Clears the n-th bit in b. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitclr(T b, uint8_t n) { return (b & ~bit<T>(n)); }

	/* Complements the n-th bit in b. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitflip(T b, uint8_t n) { return (b ^ bit<T>(n)); }

	/* Changes the n-th bit in b to the value of f. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitchg(T b, uint8_t n, bool f) { return ((b & ~bit<T>(n)) | (-f & bit<T>(n))); }
	
	/* Merges the non-masked bits in a with the masked bits in b according to
	 * the mask m, i.e. the bits in a corresponding to the bits cleared in m
	 * are merged with the bits in b corrresponding to the bits set in m. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitmerge(T a, T b, T m) { return ((a & ~m) | (b & m)); }

	/* Reverses the bit order of b. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitrev(T b)
	{
		return details::bitrev_impl<T, sizeof(T)>()(b);
	}

	/* Sets the bits in b set in mask m. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitsetm(T b, T m) { return (b | m); }

	/* Clears the bits in b set in mask m. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitclrm(T b, T m) { return (b & ~m); }

	/* Changes the bits in b set in mask m to the value of f. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitchgm(T b, T m, bool f) { return (b ^ (-f ^ b) & m); }

	/* Complements the bits in b set in mask m. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitflipm(T b, T m) { return (b ^ m); }

	/* Checks whether the n-th bit in b is set. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitisset(T b, uint8_t n) { return	(b & bit<T>(n)) != 0; }

	/* Checks whether the bits set in mask m are set in b. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitissetm(T b, T m) { return (b & m == m); }

	/* Returns the rank of the least significant bit set in b. */
	template<class T>
	inline typename details::is_unsigned<T, size_t>::type
		bitlsbset(T b) { return (b ^ (b - 1) & b); }

	/* Returns the mask of the least significant bit not set in b. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitmlsbclr(T b) { return (~b & (b + 1)); }

	/* Returns the number of bits set in b'. */
	template<class T> 
	inline typename details::is_unsigned<T, size_t>::type
		bitnset(T b)
	{
		size_t n = 0;

		for (; b; n++) 
			b &= b - 1;

		return n;
	}

#if defined CHAR_BIT
	/* Returns the number of consecutive lsbs not set in b. */
	template<class T> 
	inline typename details::is_unsigned<T, size_t>::type
		bitnlsbclr(T b)
	{
		T n = pg::widthof(b);

		if (b) {
			b = (b ^ (b - 1)) >> 1;
			for (n = 0; b; n++) b &= b - 1;
		}

		return static_cast<size_t>(n);
	}
#endif // defined CHAR_BIT

	/* Returns the bit parity of b. */
	template<class T> 
	inline typename details::is_unsigned<T, bool>::type
		bitparity(T b)
	{
		bool f = 0;

		while (b) { f = !f; b = b & (b - 1); }

		return f;
	}

	/* Swaps the two values a and b. */
	template<class T>
	inline typename details::is_unsigned<T, void>::type
		bitswap(T& a, T& b) { (a == b || (a ^= b), (b ^= a), (a ^= b)); }

	/* Interleves the bits in x with those in y. */
	template<class T>
	inline typename details::is_unsigned<T>::type
		bitintlv(T x, T y) 
	{
		T z = T();

		for (size_t i = 0; i < widthof(x); i++)
			z |= (x & 1ULL << i) << i | (y & 1ULL << i) << (i + 1);

		return z;
	}
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error requires namespace support.
# endif // defined __PG_HAS_NAMESPACES

#endif /* !defined __PG_BITS_H */
