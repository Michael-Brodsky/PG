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
 *	fact(x): returns x!
 *	cmp(a, b): returns 1 if a>b, -1 if a<b or 0 if a=b, without branching.
 *	clamp(x, low, hi): returns x clamped in [low, hi], if (low < hi).
 *	exp(x): returns an approximation of e**x.
 *	rads(x): returns x degrees converted to radians.
 *	deg(x): returns x radians converted to degrees.
 *	sin(x): returns an approximation of sin(x), where x in [-pi, pi] radians.
 *	cos(x): returns an approximation of cos(x), where x in [-pi, pi] radians.
 *	tan(x): returns an approximation of tan(x), where x in [-pi, pi] radians.
 *	sec(x): returns an approximation of sec(x), where x in [-pi, pi] radians.
 *	csc(x): returns an approximation of csc(x), where x in [-pi, pi] radians.
 *	cot(x): returns an approximation of cot(x), where x in [-pi, pi] radians.
 *	asin(x): returns an approximation of asin(x), where x in [-1, 1] radians.
 *	acos(x): returns an approximation of acos(x), where x in [-1, 1] radians.
 *	atan(x): returns an approximation of atan(x), where x in [-1, 1] radians.
 *	sinh(x): returns an approximation of sinh(x).
 *	cosh(x): returns an approximation of cosh(x).
 *	tanh(x): returns an approximation of tanh(x).
 *	sech(x): returns an approximation of sech(x).
 *	csch(x): returns an approximation of csch(x).
 *	coth(x): returns an approximation of coth(x).
 *	hypot(x,y): approximates the hypotenuse of a right triangle with sides x, y.
 *	atan2(y,x): approximates the angle between the positive x-axis and ray (x,y) [see comments]. 
 *	norm(x,xmin,xmax,ymin,ymax): normalize x in [xmin,xmax] to x in [ymin,ymax].
 *	lerp(x): returns the linear interpolant of x between two known points.
 *	bilerp(x, y): returns the bilinear interpolant of (x, y) between four known points.
 *	mean(first, last): returns the arithmetic mean of range [first, last).
 *	median(first, last): returns the median value in the sorted range [first, last).
 *  mode(first, last): returns the mode value in the sorted range [first, last).
 *	range(first, last): returns the range of the values in range [first, last).
 *
 *	Notes:
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
# include "algorithm"	// std::min, std::max
# include "numeric"		// Numeric algos.
# include "imath.h"		// iseven, isodd

# if defined __PG_HAS_NAMESPACES 

// Remove any conflicting macros possibly defined by the implementation.

#  undef sign
#  undef sqr
#  undef cube
#  undef fact
#  undef cmp
#  undef clamp
#  undef rads
#  undef deg
#  undef sin
#  undef cos
#  undef tan
#  undef sec
#  undef csc
#  undef cot
#  undef asin
#  undef acos
#  undef atan
#  undef sinh
#  undef cosh
#  undef tanh
#  undef sech
#  undef csch
#  undef coth
#  undef hypot
#  undef atan2
#  undef norm
#  undef lerp
#  undef bilerp
#  undef mean
#  undef median
#  undef mode
#  undef range

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

		/* Recursive implementation of exp(x). */

		template<typename T, size_t degree, size_t i = 0>
		struct exp_impl {
			static T evaluate(T x) {
				constexpr T c = 1.0 / static_cast<T>(1U << degree);
				x = exp_impl<T, degree, i + 1>::evaluate(x);
				return x * x;
			}
		};

		template<typename T, size_t degree>
		struct exp_impl<T, degree, degree> {
			static T evaluate(T x) {
				constexpr T c = 1.0 / static_cast<T>(1u << degree);
				x = 1.0 + c * x;
				return x;
			}
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

	template <class T>
	inline typename details::is_integer<T>::type 
		fact(T x)
	{
		return (x == 1 || x == 0) ? 1 : fact(x - 1) * x;
	}

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

	// Returns an approximation of e**x.
	template<class T>
	inline T exp(const T& x)
	{
		constexpr const size_t N = 13; // number of iterations.

		return details::exp_impl<T, N>::evaluate(x);
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
		sin(const T& rads)
	{ 
		const T z = pg::sqr(rads);
		
		return rads * (0.9999999946860073367 + z * (-0.1666665668400715135 + z * 
			(0.008333025138969367298 + z * (-0.0001980741872742697087 + 2.60190306765146018e-6 * z))));
	}

	/* Returns an approximation of cos(rads), where rads in [-pi, pi] radians. */
	template<class T>
	inline typename details::is_float<T>::type 
		cos(const T& rads)
	{ 
		const T z = pg::sqr(rads);

		return 0.9999999990181006763 + z * (-0.4999999804925358106 + z * 
			(0.04166659852743524949 + z * (-0.001388796971511749935 + z * 
				(0.0000247432468979897784 - 2.5792418318252055e-7 * z))));
	}

	/* Returns an approximation of tan(rads), where rads in [-pi, pi] radians. */
	template<class T>
	inline typename details::is_float<T>::type 
		tan(const T& rads)
	{
		const T z = pg::sqr(rads);

		return (((z * 0.092151584 + 0.11806635) * z + 0.334961658) * z + 1) * rads;
	}

	/* Returns an approximation of sec(rads), where rads in [-pi, pi] radians. */
	template<class T>
	inline typename details::is_float<T>::type
		sec(const T& rads)
	{
		return 1 / pg::cos(rads);
	}

	/* Returns an approximation of csc(rads), where rads in [-pi, pi] radians. */
	template<class T>
	inline typename details::is_float<T>::type
		csc(const T& rads)
	{
		return 1 / pg::sin(rads);
	}

	/* Returns an approximation of cot(rads), where rads in [-pi, pi] radians. */
	template<class T>
	inline typename details::is_float<T>::type
		cot(const T& rads)
	{
		return 1 / pg::tan(rads);
	}

	/* Returns an approximation of sinh(rads). */
	template<class T>
	inline typename details::is_float<T>::type
		sinh(const T& rads)
	{
		return (pg::exp(rads) - pg::exp(-rads)) / 2;
	} 

	/* Returns an approximation of cosh(rads). */
	template<class T>
	inline typename details::is_float<T>::type
		cosh(const T& rads)
	{
		return (pg::exp(rads) + pg::exp(-rads)) / 2;
	}

	/* Returns an approximation of tanh(rads). */
	template<class T>
	inline typename details::is_float<T>::type
		tanh(const T& rads)
	{
		return (pg::exp(rads * 2) - 1) / (pg::exp(rads * 2) + 1);
	}

	/* Returns an approximation of coth(rads). */
	template<class T>
	inline typename details::is_float<T>::type
		coth(const T& rads)
	{
		return (pg::exp(rads * 2) + 1) / (pg::exp(rads * 2) - 1);
	}

	/* Returns an approximation of sech(rads). */
	template<class T>
	inline typename details::is_float<T>::type
		sech(const T& rads)
	{
		return (2 * pg::exp(rads)) / (pg::exp(rads * 2) + 1);
	}

	/* Returns an approximation of csch(rads). */
	template<class T>
	inline typename details::is_float<T>::type
		csch(const T& rads)
	{
		return (2 * pg::exp(rads)) / (pg::exp(rads * 2) - 1);
	}

	/* Returns an approximation of asin(rads), where rads in [-1, 1] radians. */
	template<class T>
	inline typename details::is_float<T>::type
		asin(const T& rads)
	{
		// Algo not so great for rads < 0, so we use |rads| and reflect that over -1 < rads < 0.
		const T z = std::abs(rads);

		return pg::sign(rads) * (std::numbers::pi_inv_two - std::sqrt(1 - z) * 
			(1.5707288 - 0.2121144 * z + 0.074261 * pg::sqr(z) - 0.0187293 * pg::cube(z)));
	}

	/* Returns an approximation of acos(rads), where rads in [-1, 1] radians. */
	template<class T>
	inline typename details::is_float<T>::type
		acos(T rads)
	{
		// This is nVidia's implementation.
		const T negate = T(rads < 0);
		T ret = -0.0187293;

		rads = std::abs(rads);
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

	/* Returns an approximation of atan(rads), where rads in [-1, 1] radians. */
	template<class T>
	inline typename details::is_float<T>::type 
		atan(const T& rads)
	{
		constexpr const T a = 0.0776509570923569;
		constexpr const T b = -0.287434475393028;
		constexpr const T c = std::numbers::pi_inv_four - a - b;
		const T z = pg::sqr(rads);

		return ((a * z + b) * z + c) * rads;
	}

	/* Returns an approximation of the length of the hypotenuse of a right triangle with sides (x,y). */
	template<class T>
	inline typename details::is_float<T>::type
		hypot(const T& x, const T& y)
	{
		constexpr const T a0 = 127. / 128., b0 = 3./16., a1 = 27. / 32., b1 = 71. / 128.;
		const T z0 = a0 * std::max(std::abs(x), std::abs(y)) + b0 * std::min(std::abs(x), std::abs(y));
		const T z1 = a1 * std::max(std::abs(x), std::abs(y)) + b1 * std::min(std::abs(x), std::abs(y));

		return std::max(z0, z1);
	}

	/* Returns an approximation of atan2(y, x), where y/x in [-1, 1] radians. */
	template<class T>
	inline typename details::is_float<T>::type
		atan2(const T& y, const T& x)
	{
		const int sx = static_cast<int>(pg::sign(x)), sy = static_cast<int>((int)pg::sign(y));

		return pg::sqr(sx) * pg::atan(y / x) + ((1 - sx) >> 1) * (1 + sy - pg::sqr(sy)) * std::numbers::pi;
	}

	/* Normalizes x in [xmin,xmax] to x in [ymin,ymax]. */
	template<class T>
	T norm(T x, T xmin, T xmax, T ymin, T ymax)
	{
		assert(xmax != xmin);

		return (ymax - ymin) / (xmax - xmin) * (x - xmax) + ymax;
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

	/* Returns the arithmetic mean of range [first, last). */
	template<class InputIt>	
		inline typename std::iterator_traits<InputIt>::value_type
			mean(InputIt first, InputIt last)
	{
		using value_type = typename std::iterator_traits<InputIt>::value_type;
		
		return first == last 
			? value_type() 
			: std::accumulate(first, last, value_type()) / std::distance(first, last);
	}

	/* Returns the median value in the sorted range [first, last). */
	template<class InputIt>
		inline typename std::iterator_traits<InputIt>::value_type
			median(InputIt first, InputIt last)
	{
		using value_type = typename std::iterator_traits<InputIt>::value_type;
		using difference_type = typename std::iterator_traits<InputIt>::difference_type;

		const difference_type d = std::distance(first, last);

		return first == last 
			? value_type() 
			: pg::isodd(d)
				? *(first + d / 2)
				: ((*(first + (d - 1) / 2) + *(first + (d + 1) / 2)) / 2);
	}

	/* Returns the mode value in the sorted range [first, last). */
	template<class InputIt>
	inline typename std::iterator_traits<InputIt>::value_type
		mode(InputIt first, InputIt last, typename std::iterator_traits<InputIt>::value_type nomode = 
			typename std::iterator_traits<InputIt>::value_type())
	{
		using value_type = typename std::iterator_traits<InputIt>::value_type;
		value_type result = nomode;
		unsigned count = 1, max_count = 0;

		if (first != last)
		{
			value_type key = *first++;

			for (; first != last; ++first)
			{
				if (key == *first) count++;
				else
				{
					if (count > max_count)
					{
						result = *(first - 1);
						max_count = count;
					}
					key = *first;
					count = 1;
				}
			}
			if (max_count < 2)
				result = nomode;
		}

		return result;
	}

	/* Returns the range of the values in range [first, last). */
	template<class InputIt>
	inline typename std::iterator_traits<InputIt>::value_type
		range(InputIt first, InputIt last)
	{
		return *std::max_element(first, last) - *std::min_element(first, last);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif	/* !defined __PG_FMATH_H */