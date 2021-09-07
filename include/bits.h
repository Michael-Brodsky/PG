/*	
 *	This file defines several function-like macros for dealing with bit fields
 *	and bitwise operations. 
 *
 *	***************************************************************************
 * 
 *	File: bits.h
 *	Date: December 8, 2015
 *	Version: 0.99
 *	Author: Michael Brodsky
 *	Email: superhotmuffin@hotmail.com
 *	Copyright (c) 2012-2020 Michael Brodsky 

 *	**************************************************************************
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
 *	***************************************************************************
 *
 *	Description:
 *
 *		The algorithms are optimized for speed and avoid branching when 
 *		possible. Macro arguments with a trailing underscore `_' must be 
 *		modifiable lvalues. No type checking is performed.
 * 
 *  Notes:
 * 
 *      A function template version, with strong type checking is in testing
 *      and should be released soon.
 *
 *	**************************************************************************/

#ifndef BITS_H__
#define BITS_H__	20151215L

#include <limits.h>

/* References the n-th bit in a bitfield. */
#define bit(n)					(1U << (n))

/* Sets the n-th bit in b. */
#define bitset(b, n)			((b) | bit(n))

/* Clears the n-th bit in b. */
#define bitclr(b, n)			((b) & ~bit(n))

/* Complements the n-th bit in b. */
#define bitflip(b, n)			((b) ^ bit(n))

/* Changes the n-th bit in `b' to the value of `f', where f = 0 or 1. */
#define bitchange(b, n, f)		(((b) & ~bit(n)) | (-(f) & bit(n)))

/* Merges the non-masked bits in `a' with the masked bits in `b' according to 
 * the mask `m', i.e. the bits in `a' corresponding to the bits cleared in `m' 
 * are merged with the bits in `b' corrresponding to the bits set in `m'. */
#define bitmerge(a, b, m)		(((a) & ~(m)) | ((b) & (m)))

/* Reverses the bit order of byte `b'. */
#define bitrev8(b_)				(b_ = (((b_ & 0xaa) >> 1) | ((b_ & 0x55) << 1)), \
								 b_ = (((b_ & 0xcc) >> 2) | ((b_ & 0x33) << 2)), \
								 b_ = (((b_ & 0xf0) >> 4) | ((b_ & 0x0f) << 4)))

/* Reverses the bit order of the 16-bit word `b'. */
#define bitrev16(b_)			bitrev8(b_), \
								(b_ = (((b_ & 0xff00) >> 8) | ((b_ & 0x00ff) << 8)))

/* Reverses the bit order of the 32-bit word `b'. */
#define bitrev32(b_)			bitrev16(b_), \
								(b_ = (((b_ & 0x0000ffff) << 16) |((b_ & 0xffff0000) >> 16)))

/* Reverses the bit order of the 64-bit word `b'. */
#define bitrev64(b_)			(b_ = (((b_ & 0x5555555555555555) <<  1) | ((b_ & 0xaaaaaaaaaaaaaaaa) >>  1)), \
								 b_ = (((b_ & 0x3333333333333333) <<  2) | ((b_ & 0xcccccccccccccccc) >>  2)), \
								 b_ = (((b_ & 0x0f0f0f0f0f0f0f0f) <<  4) | ((b_ & 0xf0f0f0f0f0f0f0f0) >>  4)), \
								 b_ = (((b_ & 0x00ff00ff00ff00ff) <<  8) | ((b_ & 0xff00ff00ff00ff00) >>  8)), \
								 b_ = (((b_ & 0x0000ffff0000ffff) << 16) | ((b_ & 0xffff0000ffff0000) >> 16)), \
								 b_ = (((b_ & 0x00000000ffffffff) << 32) | ((b_ & 0xffffffffffffffff) >> 32)))

/* Reverses the bit order of an n-bit word `b' of type `T'. */
#define bitrev(T, b_)			do { \
	T r__ = b_; \
	size_t s__ = sizeof(b_) * CHAR_BIT - 1; \
	for (b_ >>= 1; b_; b_ >>= 1) { \
		r__ <<= 1; \
		r__ |= b_ & 1; \
		s__--; \
	} \
	b_ = (r__ <<= s__); \
} while(0)

/* Sets the bits in `b' set in mask `m'. */
#define bitsetm(b, m)			((b) | (m))

/* Clears the bits in `b' set in mask `m'. */
#define bitclrm(b, m)			((b) & ~(m))

/* Changes the bits in `b' set in mask `m' to the value of `f', where f = 0 or 1. */
#define bitchgm(b, m, f)		((b) ^ (-(f) ^ (b)) & (m))

/* Complements the bits in `b' set in mask `m'. */
#define bitflipm(b, m)			((b) ^ (m))

/* Expands to 1 if the n-th bit in `b' is set, else expands to 0. */
#define bitisset(b, n)			((b) & bit(n)) != 0

/* Expands to 1 if the bits set in mask `m' are set in `b', else expands to 0. */
#define bitissetm(b, m)			((b) & (m) == (m))

/* Expands to the rank of the least significant bit set in `b'. */
#define bitlsbset(b)			((b) ^ ((b) - 1) & (b))

/* Expands to the mask of the least significant bit not set in `b'. */
#define bitlsbclrm(b)			(~(x) & ((x) + 1))

/* Sets `n_' to the number of bits set in b'. */
#define bitsetn(b, n_)			do { \
	unsigned long v__ = (b); \
	for(n_ = 0; v__; n_++) v__ &= v__ - 1; \
} while(0)

/* Sets `n_' to the number of consecutive lsb not set in `b'. */
#define bitclrlsbn(b, n_)		do { \
	unsigned long v__ = (b); \
	if(b) { \
		v__ = (v__ ^ (v__ - 1)) >> 1; \
		for(n_ = 0; v__; n_++) v__ &= v__ - 1; \
	} \
	else n_ = CHAR_BIT * sizeof(n_); \
 } while(0)

/* Sets `f_' to the parity of  `b', where `f' = 0 or 1. */
#define bitparity(b, f_)		do { \
	unsigned long v__ = (b); \
	f_ = 0; \
	while(v__) { f_ = !f_; v__ = v__  & (v__ - 1); } \
} while(0)

/* Swaps two integral values `a' and `b'. */
#define bitswap(a_, b_)			(a_ == b_ || (a_ ^= b_), (b_ ^= a_), (a_ ^= b_))

#if !defined(widthof)
/* Expands to the width of type `T' in bits. */
# define widthof(T)				(sizeof(T) * CHAR_BIT)
#endif /* !defined(widthof_) */

/* Interleves the bits in `x' with those in `y' into `z'. */
#define bitintlv(x, y, z_)	do { \
	size_t i__; \
	for(i__ = 0; i__ < widthof(x); i__++) \
		z |= ((x) & 1U << i__) << i__ | ((y) & 1U << i__) << (i__ + 1); \
} while(0)

#endif /* !BITS_H__ */
