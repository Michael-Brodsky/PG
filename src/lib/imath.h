/*
 *	This file defines a library of integer math functions.
 *
 *	***************************************************************************
 *
 *	File: imath.h
 *	Date: September 7, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky
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
 *	The imath library consists of mathematical functions that take integral
 *	arguments and return integral results and are significantly faster than
 *	their floating point counterparts and many standard library
 *	implementations. The algorithms avoid branching and employ bitwise
 *	operations where advantageous.
 *
 *	Library Functions:
 *
 *	iSgn(x): returns -1 if x < 0, else returns 0.
 *	iSign(x): returns -1 if x < 0, else returns +1.
 *	iSignOf(x): returns -1 if x < 0, 0 if x == 0, else returns +1.
 *	iAbs(x): returns the unsigned absolute value of signed integer x.
 *	iMax(a, b): returns the greater of a and b.
 *	iMin(a, b): returns the lesser of a and b.
 *	isEven(x): return true if x is even, else returns false.
 *	isOdd(x): return true if x is odd, else returns false.
 *	isSignNe(a, b): returns true if a and b have opposite signs, else false.
 *	isPow2(x): returns true if x is an integer power of two, else false.
 *	iNegateIf(x, flag): returns -x if flag is true, else returns x.
 *	iSwap(a, b): swaps the values of a and b.
 *	iDiv2(x, s): returns x / 2**s.
 *	iPow2(s): returns 2**s.
 *	iPow2x(x, s): returns x * 2**s.
 *	iLog2(x): returns the integral base 2 logarithm of x.
 *	iLog10(x): returns the integral base 10 logarithm of x.
 *	iMod2(x, n): returns x mod n, where n is an integer power of 2.
 *	iMod2m(x, m): returns x mod m, where m is one less than an integer power of 2.
 *	iPow2Ge(x): returns the smallest integer power of 2 greater than or equal to x.
 *	iPow2Le(x): returns the greatest integer power of 2 less than or equal to x.
 *	iGcd(a, b): returns the greatest common divisor of a and b.
 *	iLcm(a, b): returns the lowest common multiple of a and b.
 *	isCoprime(a, b): returns true if a and b are relatively prime, else false.
 * 
 *	Notes:
 * 
 *		Certain restrictions are placed on the library functions due to 
 *		limitations of the algorithms used to compute the results, notably  
 *		regarding the signed/unsigned type of the function arguments and 
 *		return types. Programs that do not obey these restrictions are ill-
 *		formed.
 *
 *		The restrictions are rooted in common-sense and should not pose 
 *		problems for clients. For example the absolute value function `iabs()' 
 *		takes a signed argument and returns an unsigned value. Since the 
 *		absolute value of an unsigned type is always the value unchanged and 
 *		the return value is always non-negative, it is redundant to take the 
 *		absolute value of an unsigned type and clients must be designed to 
 *		avoid such redundancies. Similarly, the "sign of" functions, `isgn()', 
 *		`isign()' and `isignof()' take only signed arguments as it is also 
 *		redundant to take the sign of an unsigned type because its sign is 
 *		known beforehand.
 * 
 *		Functions are enabled using template substitution based on the type of  
 *		calling parameters. Template substitution will fail if called with 
 *		invalid argument types, resulting in an ill-formed program.
 *
 *	**************************************************************************/

#if !defined __PG_IMATH_H
# define __PG_IMATH_H	20210907L

# include <cassert>		// assert() macro.
# include <limits>		// Required to deduce function argument types.
# include <type_traits>	// Required for function template substitution.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	namespace details
	{
		/* Helper types to enable functions and set return type. */

		template <class T, class U = T>
		struct is_integer
		{
			typedef typename std::enable_if<std::is_integral<T>::value, U>::type type;
		};

		template <class T, class U = T>
		struct is_unsigned
		{
			typedef typename std::enable_if<std::is_integral<T>::value &&
				std::is_unsigned<T>::value, U>::type type;
		};

		template <class T, class U = T>
		struct is_signed
		{
			typedef typename std::enable_if<std::is_integral<T>::value &&
				std::is_signed<T>::value, U>::type type;
		};

		// ilog2 specialization for 8-bit types.
		template <class T>
		inline T ilog2_8(T x)
		{
			T r = (x > 0xF) << 2; x >>= r;
			T s = (x > 0x3) << 1; x >>= s; r |= s;
			return (r |= (x >> 1));
		}

		// ilog2 specialization for 16-bit types.
		template <class T>
		inline T ilog2_16(T x)
		{
			T r = (x > 0xFF) << 3; x >>= r;
			T s = (x > 0xF) << 2; x >>= s; r |= s;
			s = (x > 0x3) << 1; x >>= s; r |= s;
			return (r |= (x >> 1));
		}

		// ilog2 specialization for 32-bit types.
		template <class T>
		inline T ilog2_32(T x)
		{
			T r = (x > 0xFFFF) << 4; x >>= r;
			T s = (x > 0xFF) << 3; x >>= s; r |= s;
			s = (x > 0xF) << 2; x >>= s; r |= s;
			s = (x > 0x3) << 1; x >>= s; r |= s;
			return (r |= (x >> 1));
		}

		// ilog2 specialization for 64-bit types.
		template <class T>
		inline T ilog2_64(T x)
		{
			T r = (x > 0xFFFFFFFF) << 5; x >>= r;
			T s = (x > 0xFFFF) << 4; x >>= s; r |= s;
			s = (x > 0xFF) << 3; x >>= s; r |= s;
			s = (x > 0xF) << 2; x >>= s; r |= s;
			s = (x > 0x3) << 1; x >>= s; r |= s;
			return (r |= (x >> 1));
		}
#if defined __HAS_128_BIT_INTEGERS
		// ilog2 specialization for 128-bit types.
		template <class T>
		inline T ilog2_128(T x)
		{
			T r = (x > 0xFFFFFFFFFFFFFFFF) << 6; x >>= r;
			T s = (x > 0xFFFFFFFF) << 5; x >>= s; r |= s;
			s = (x > 0xFFFF) << 4; x >>= s; r |= s;
			s = (x > 0xFF) << 3; x >>= s; r |= s;
			s = (x > 0xF) << 2; x >>= s; r |= s;
			s = (x > 0x3) << 1; x >>= s; r |= s;
			return (r |= (x >> 1));
		}
#endif // if defined __HAS_128_BIT_INTEGERS

		template <typename T, size_t N>
		struct iPow2Ge_;

		// ipow2ge specialization for 8-bit types.
		template <typename T>
		inline T iPow2Ge_8(T x)
		{
			x -= 1, x |= (x >> 1), x |= (x >> 2), x |= (x >> 4);
			return x + 1;
		}

		// ipow2ge specialization for 16-bit types.
		template <typename T>
		inline T iPow2Ge_16(T x) {
			x -= 1, x |= (x >> 1), x |= (x >> 2), x |= (x >> 4), x |= (x >> 8);
			return x + 1;
		}

		// ipow2ge specialization for 32-bit types.
		template <typename T>
		inline T iPow2Ge_32(T x) {
			x -= 1, x |= (x >> 1), x |= (x >> 2), x |= (x >> 4), x |= (x >> 8), x |= (x >> 16);
			return x + 1;
		}

		// ipow2ge specialization for 64-bit types.
		template <typename T>
		inline T iPow2Ge_64(T x) {
			x -= 1, x |= (x >> 1), x |= (x >> 2), x |= (x >> 4), x |= (x >> 8), x |= (x >> 16), x |= (x >> 32);
			return x + 1;
		}
#if defined __HAS_128_BIT_INTEGERS
		// ipow2ge specialization for 128-bit types.
		template <typename T>
		inline T iPow2Ge_128(T x) {
			x -= 1, x |= (x >> 1), x |= (x >> 2), x |= (x >> 4), x |= (x >> 8), x |= (x >> 16), x |= (x >> 32), x |= (x >> 64);
			return x + 1;
		}
#endif // if defined __HAS_128_BIT_INTEGERS

		// ipow2le specialization for 8-bit types.
		template <typename T>
		inline T iPow2Le_8(T x)
		{
			x |= (x >> 1), x |= (x >> 2), x |= (x >> 4);
			return x - (x >> 1);
		}

		// ipow2le specialization for 16-bit types.
		template <typename T>
		inline T iPow2Le_16(T x)
		{
			x |= (x >> 1), x |= (x >> 2), x |= (x >> 4), x |= (x >> 8);
			return x - (x >> 1);
		}

		// ipow2le specialization for 32-bit types.
		template <typename T>
		inline T iPow2Le_32(T x)
		{
			x |= (x >> 1), x |= (x >> 2), x |= (x >> 4), x |= (x >> 8), x |= (x >> 16);
			return x - (x >> 1);
		}

		// ipow2le specialization for 64-bit types.
		template <typename T>
		inline T iPow2Le_64(T x)
		{
			x |= (x >> 1), x |= (x >> 2), x |= (x >> 4), x |= (x >> 8), x |= (x >> 16), x |= (x >> 32);
			return x - (x >> 1);
		}
#if defined __HAS_128_BIT_INTEGERS
		// ipow2le specialization for 128-bit types.
		template <typename T>
		inline T iPow2Le_128(T x)
		{
			x |= (x >> 1), x |= (x >> 2), x |= (x >> 4), x |= (x >> 8), x |= (x >> 16), x |= (x >> 32), x |= (x >> 64);
			return x - (x >> 1);
		}
#endif // if defined __HAS_128_BIT_INTEGERS

		/* Helper functions to select type-specific specialization, above. */

		template <class T, size_t N>
		struct ilog2_;

		template <class T>
		struct ilog2_<T, 1>
		{
			T operator()(T x) const { return ilog2_8(x); }
		};

		template <class T>
		struct ilog2_<T, 2>
		{
			T operator()(T x) const { return ilog2_16(x); }
		};

		template <class T>
		struct ilog2_<T, 4>
		{
			T operator()(T x) const { return ilog2_32(x); }
		};

		template <class T>
		struct ilog2_<T, 8>
		{
			T operator()(T x) const { return ilog2_64(x); }
		};
#if defined __HAS_128_BIT_INTEGERS
		template <class T>
		struct ilog2_<T, 16>
		{
			T operator()(T x) const { return ilog2_128(x); }
		};
#endif // if defined __HAS_128_BIT_INTEGERS

		template <typename T>
		struct iPow2Ge_<T, 1>
		{
			T operator()(T x) const { return iPow2Ge_8(x); }
		};

		template <typename T>
		struct iPow2Ge_<T, 2>
		{
			T operator()(T x) const { return iPow2Ge_16(x); }
		};

		template <typename T>
		struct iPow2Ge_<T, 4>
		{
			T operator()(T x) const { return iPow2Ge_32(x); }
		};

		template <typename T>
		struct iPow2Ge_<T, 8>
		{
			T operator()(T x) const { return iPow2Ge_64(x); }
		};
#if defined __HAS_128_BIT_INTEGERS
		template <typename T>
		struct iPow2Ge_<T, 16>
		{
			T operator()(T x) const { return iPow2Ge_128(x); }
		};
#endif // if defined __HAS_128_BIT_INTEGERS

		template <typename T, size_t N>
		struct iPow2Le_;

		template <typename T>
		struct iPow2Le_<T, 1>
		{
			T operator()(T x) const { return iPow2Le_8(x); }
		};

		template <typename T>
		struct iPow2Le_<T, 2>
		{
			T operator()(T x) const { return iPow2Le_16(x); }
		};

		template <typename T>
		struct iPow2Le_<T, 4>
		{
			T operator()(T x) const { return iPow2Le_32(x); }
		};

		template <typename T>
		struct iPow2Le_<T, 8>
		{
			T operator()(T x) const { return iPow2Le_64(x); }
		};
#if defined __HAS_128_BIT_INTEGERS
		template <typename T>
		struct iPow2Le_<T, 16>
		{
			T operator()(T x) const { return iPow2Le_128(x); }
		};
#endif // if defined __HAS_128_BIT_INTEGERS
	} // namespace details

#pragma region library_functions

	// returns -1 if x < 0, else 0.
	template <class T>
	inline typename details::is_signed<T>::type
		isgn(T x)
	{
		return -static_cast<T>(
			static_cast<typename std::make_unsigned<T>::type>
			(static_cast<T>(x)) >> (sizeof(T) * CHAR_BIT - 1));
	}

	// returns -1 if x < 0, else +1.
	template <class T>
	inline typename  details::is_signed<T>::type
		isign(T x)
	{
		return +1 | pg::isgn(x);
	}

	// returns -1 if x < 0, 0 if x == 0, else +1.
	template <class T>
	inline typename details::is_signed<T>::type
		isignof(T x)
	{
		return (x != 0) | pg::isgn(x);
	}

	// returns the unsigned absolute value of signed integer x.
	template <class T>
	inline typename std::make_unsigned<typename details::is_signed<T>::type>::type
		iabs(T x)
	{
		// T must be signed. Return type must be unsigned, else iabs(STYPE_MIN) = STYPE_MIN
		// signed int i = -42;			// OK
		// unsigned int j = iabs(i);	// OK
		T const m = pg::isgn(x);

		return static_cast<typename std::make_unsigned<T>::type>((x + m) ^ m);
	}

	// returns the greater of a and b.
	template <class T>
	inline typename details::is_integer<T>::type
		imax(T a, T b)
	{
		return (a ^ ((a ^ b) & -(a < b)));
	}

	// returns the lesser of a and b.
	template <class T>
	inline typename details::is_integer<T>::type
		imin(T a, T b)
	{
		return (b ^ ((a ^ b) & -(a < b)));
	}

	// return true if x is even.
	template <class T>
	inline typename details::is_integer<T, bool>::type
		iseven(T x)
	{
		return ((x & 0x1) == 0);
	}

	template <class T>
	inline typename details::is_integer<T, bool>::type
		isodd(T x) // return true if x is odd.
	{
		return ((x & 0x1) != 0);
	}

	// returns true if a and b have opposite signs.
	template <class T>
	inline typename details::is_signed<T, bool>::type
		issignne(T a, T b)
	{
		return ((a ^ b) < 0);
	}

	// returns true if x is an integer power of two.
	template <class T>
	inline typename details::is_unsigned<T, bool>::type
		ispow2(T x)
	{
		return (x && !(x & (x - 1)));
	}

	// returns -x if f is true, else returns x.
	template <class T>
	inline typename details::is_signed<T>::type
		inegateif(T x, bool f)
	{
		return ((x ^ -f) + f);
	}

	// swaps the values of a and b.
	template <class T>
	inline typename details::is_integer<T, void>::type
		iswap(T& a, T& b)
	{
		(&a == &b) || ((a ^= b), (b ^= a), (a ^= b));
	}

	// returns x / 2**s.
	template <class T>
	inline typename details::is_unsigned<T>::type
		idiv2(T x, uint8_t s)
	{
		return (x >> s);
	}

	// returns 2**s.
	inline auto ipow2(uint8_t s) -> typename std::make_unsigned<decltype(0x1 << s)>::type
	{
		return (0x1 << s);
	}

	// Template version of ipow2(), returns 2**s.
	template<class T>
	inline T ipow2t(T s)
	{
		return ((T)1 << s);
	}

	// returns x * 2**s.
	template <typename T>
	inline typename details::is_unsigned<T>::type
		ipow2x(T x, T s)
	{
		return (x << s);
	}

	// returns the integral base 2 logarithm of x.
	template <class T>
	inline typename details::is_unsigned<T>::type
		ilog2(T x)
	{
		return details::ilog2_<T, sizeof(T)>()(x);
	}

	// returns the integral base 10 logarithm of x.
	template <typename T>
	inline typename details::is_unsigned<T>::type
		ilog10(T x)
	{
		static T const powersOf10[] =
		{
			T(1), T(10), T(100), T(1000), T(10000), T(100000),
			T(1000000), T(10000000), T(100000000), T(1000000000)
		};
		const T tmp = (pg::ilog2(x) + 1) * 1233 >> 12;

		return tmp - (x < powersOf10[tmp]);
	}

	// returns x % m, where m is one less than an integer power of two.
	template <typename T>
	inline typename details::is_unsigned<T>::type
		imod2m(T x, T m)
	{
		assert(ispow2(m + 1));

		return x & m;
	}

	// returns x % n, where n is an integer power of two.
	template <typename T>
	inline typename details::is_unsigned<T>::type
		imod2(T x, T n)
	{
		return imod2m(x, n - 1);
	}

	// returns the smallest integer power of two greater than or equal to x.
	template <typename T>
	inline typename details::is_unsigned<T>::type
		ipow2ge(T x)
	{
		return details::iPow2Ge_<T, sizeof(T)>()(x);
	}

	// returns the greatest integer power of two less than or equal to x.
	template <typename T>
	inline typename details::is_unsigned<T>::type
		ipow2le(T x)
	{
		return details::iPow2Le_<T, sizeof(T)>()(x);
	}

	namespace details
	{

		// returns the greatest common divisor of a and b, b != 0, (Binary method).
		template <typename T>
		inline typename details::is_unsigned<T>::type
			igcd_(T a, T b)
		{
			unsigned d = 0;

			for (; pg::iseven(a | b); ++d)
			{
				a = pg::idiv2(a, 1);
				b = pg::idiv2(b, 1);
			}

			while (pg::iseven(a)) 
				a = pg::idiv2(a, 1);

			do
			{
				while (pg::iseven(b)) b = pg::idiv2(b, 1);
				if (a > b) 
					pg::iswap(a, b);
				b -= a;

			} while (b != 0);

			return pg::ipow2x(a, d);
		}
	} // namespace details

	template <typename T>
	inline typename details::is_unsigned<T>::type
		igcd(T a, T b)
	{
		return b == 0 ? a : details::igcd_(a, b);
	}

	// returns the lowest common multiple of a and b.
	template <typename T>
	inline typename details::is_unsigned<T>::type
		ilcm(T a, T b)
	{
		return (a * b) / pg::igcd(a, b);
	}

	// returns true if a and b are relatively prime.
	template <typename T>
	inline typename details::is_unsigned<T, bool>::type
		iscoprime(T a, T b)
	{
		return pg::igcd(a, b) == 1;
	}
#pragma endregion

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_IMATH_H
