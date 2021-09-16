/*
 *	This file defines a library of engineering and scientific math functions.
 *
 *	***************************************************************************
 *
 *	File: fmath.h
 *	Date: September 14, 2021
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
 *	The fmath library consists of mathematical functions templates useful in 
 *	scientific and engineering applications where execution speed is critical.
 *	The functions avoid branching where possible and use polynomial 
 *	approximations to compute results.
 *
 *	Library Functions:
 *
 *	sign(x): returns -1 if x < 0, else returns +1.
 *	sqr(x): returns x**2.
 *	cube(x): returns x**3.
 *	cmp(a, b): returns 1 if a>b, -1 if a<b or 0 if a=b, without branching.
 *	clamp(x, low, hi): returns x clamped on [low, hi], if (low < hi).
 *	rads(x): returns x degrees converted to radians.
 *	deg(x): returns x radians converted to degrees.
 *	sine(x): returns an approximation of sin(x), where x in [-pi, pi] radians.
 *	cosine(x): returns an approximation of cos(x), where x in [-pi, pi] radians.
 *	tangent(x): returns an approximation of tan(x), where x in [-pi, pi] radians.
 *	arcsin(x): returns an approximation of asin(x), where x in [-1, 1] radians.
 *	arccos(x): returns an approximation of acos(x), where x in [-1, 1] radians.
 *	arctan(x): returns an approximation of atan(x), where x in [-1, 1] radians.
 *	lerp(x): returns the linear interpolant of x between two known points.
 *	bilerp(x, y): returns the bilinear interpolant of (x, y) between four known points.
 *
 *	Notes:
 *
 *		Certain restrictions are placed on the library functions with 
 *		respect to argument types, specifically floating point/integral types.
 *
 *		Functions are enabled using template substitution based on the type of
 *		calling parameters. Template substitution will fail if called with
 *		invalid argument types, resulting in an ill-formed program.
 *
 *	**************************************************************************/

#if !defined __PG_FMATH_H
# define __PG_FMATH_H	20210914L

# include "numbers"		// Numeric constants.
# include "cmath"		// std::sqrt, std::abs
# include "type_traits"	// Template substitution.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	namespace details
	{
		/* Helper types to enable functions and set return type. */

		template <class T, class U = T>
		struct is_float
		{
			typedef typename std::enable_if<std::is_floating_point<T>::value, U>::type type;
		};
	} // namespace details

	/* Returns -1 if x < 0, else returns +1.*/
	template <class T> 
	inline int sign(T x) { return (T(0) < x) - (x < T(0)); }

	/* Returns x**2. */
	template<class T>
	inline T sqr(const T& x) { return x * x; }

	/* Returns x**3. */
	template<class T>
	inline T cube(const T& x) { return x * x * x; }

	/* Returns 1 if a>b, -1 if a<b or 0 if a=b, without branching. */
	template<class T>
	inline T cmp(const T& a, const T& b) { return ((a > b) - (a < b)); }

	/* Returns `x' clamped in [low, hi], (low < hi). */
	template<class T>
	inline T clamp(const T& x, const T& low, const T& hi) 
	{ 
		// This generates three ASM instructions on ggc/clang, allegedly ;)
		const T t = x < low ? low : x;

		return t > hi ? hi : t;
	}

	/* Returns `rads' radians converted to degrees. */
	template<class T>
	inline typename details::is_float<T>::type 
		deg(const T& rads) { return rads / std::numbers::pi * 180; }

	/* Returns `deg' degrees converted to radians. */
	template<class T>
	inline typename details::is_float<T>::type 
		rads(const T& deg) { return deg / 180 * std::numbers::pi; }

	/* Returns an approximation of sin(rads), where rads in [-pi, pi] radians. */
	template<class T>
	inline typename details::is_float<T>::type 
		sine(const T& rads)
	{ 
		const T z = sqr(rads);

		return ((((z * 0.0000027557f - 0.00019841f) * z + 0.0083333f) * z - 0.16667f) * z + 1) * rads;
	}

	/* Returns an approximation of cos(rads), where rads in [-pi, pi] radians. */
	template<class T>
	inline typename details::is_float<T>::type 
		cosine(const T& rads)
	{ 
		const T z = sqr(rads);

		return rads * (1 + z * (-0.1666666f + z * (0.008333025f + z * (-0.000198074f + 2.6019031e-6 * z))));
	}

	/* Returns an approximation of tan(rads), where rads in [-pi, pi] radians. */
	template<class T>
	inline typename details::is_float<T>::type 
		tangent(const T& rads)
	{
		const T z = sqr(rads);

		return (((z * 0.092151584f + 0.11806635f) * z + 0.334961658f) * z + 1) * rads;
	}

	/* Returns an approximation of arcsin(rads), where rads in [-1, 1] radians. */
	template<class T>
	inline typename details::is_float<T>::type
		arcsin(const T& rads)
	{
		// Algo not so great for rads < 0, so we use |rads| and reflect that over -1 < rads < 0.
		const T z = std::abs(rads);

		return pg::sign(rads) * (std::numbers::pi_two - std::sqrt(1 - z) * 
			(1.5707288f - 0.2121144f * z + 0.074261f * pg::sqr(z) - 0.0187293f * pg::cube(z)));
	}

	/* Returns an approximation of arccos(rads), where rads in [-1, 1] radians. */
	template<class T>
	inline typename details::is_float<T>::type
		arccos(T rads)
	{
		// This is nVidia's implementation.
		float negate = float(rads < 0);

		rads = std::abs(rads);
		float ret = -0.0187293;
		ret = ret * rads;
		ret = ret + 0.0742610;
		ret = ret * rads;
		ret = ret - 0.2121144;
		ret = ret * rads;
		ret = ret + 1.5707288;
		ret = ret * std::sqrt(1.0 - rads);
		ret = ret - 2 * negate * ret;
		return negate * std::numbers::pi + ret;
	}

	/* Returns an approximation of arctan(rads), where rads in [-1, 1] radians. */
	template<class T>
	inline typename details::is_float<T>::type 
		arctan(const T& rads)
	{
		const T z = std::abs(rads);

		return rads * (std::numbers::pi_four + (1 - z) * (0.2447f + 0.663f * z));
	}

	/* Returns the linear interpolant of x between two known points. */
	template<class T>
	inline typename details::is_float<T>::type 
		lerp(const T& x, const T& x0, const T& x1, const T& y0, const T& y1)
	{
		return (y0 + (y1 - y0) * (x - x0) / (x1 - x0));
	}

	/* Returns the bilinear interpolant of (x, y) between four known points. */
	template<class T>
	inline typename details::is_float<T>::type 
		bilerp(
			const T& x, const T& y, 
			const T& x0, const T& x1, const T& x2,
			const T& y0, const T& y1, const T& y2,
			const T& q11, const T& q12, const T& q21, const T& q22)
	{
		return 1 / ((x2 - x1) * (y2 - y1)) * 
			(
				q11 * (x2 - x) * (y2 - y) +
				q21 * (x - x1) * (y2 - y) +
				q12 * (x2 - x) * (y - y1) +
				q22 * (x - x1) * (y - y1)
			);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif	/* !defined __PG_FMATH_H */