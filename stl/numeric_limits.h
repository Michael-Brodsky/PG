/*
 *	This file defines objects from the C++ Standard Template Library (STL)
 *  for querying various properties of arithmetic types.
 *
 *	***************************************************************************
 *
 *	File: numeric_limits.h
 *	Date: August 17, 2021
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
 *	***************************************************************************
 *
 *	Description:
 *
 *		This file defines objects in the <limits> header of the C++ Standard
 *		Template Library (STL) implementation - to the extent they are
 *		supported by the Arduino implementation. The objects behave according
 *		to the ISO C++11 Standard: (ISO/IEC 14882:2011), except as noted.
 * 
 *	Notes:
 * 
 *		The ISO C++11 Standard defines these objects in the <limits> header, 
 *		however Arduino header files should have the `.h' suffix and 
 *		`limits.h' would conflict with the cstdlib file <limits.h>, thus it 
 *		was decided to name this file `numeric_limits.h'
 * 
 *		Certain properties of arithmetic types cannot be computed because 
 *		required parameters are missing from/undefined in the Arduino 
 *		environment. These are marked `undefined'.
 *
 *	**************************************************************************/

#if !defined NUMERIC_LIMITS_H__
# define NUMERIC_LIMITS_H__ 20210817

# include <float.h>		// C-stdlib floating-point limits.
# include <math.h>		// `NAN' macro.
# include "pg.h"		// Pg environment.

# if defined min
#  undef min		// Arduino `min' macro define conflicts with std::min.
# endif
# if defined max
#  undef max		// Arduino `max' macro define conflicts with std::max.
# endif

# if defined ARDUINO
// Arduino environment does not define the following object-like macros, 
// which are required to compute certain values, so we define them here.

#  if !defined M_LOG10_2
#   define M_LOG10_2			0.301029995663981195214	/* log10(2) from <constants.h> */
#  endif
#  if !defined __WCHAR_MIN__
#   define __WCHAR_MIN__		INT16_MIN
#  endif
#  if !defined __WCHAR_MAX__
#   define __WCHAR_MAX__		INT16_MAX
#  endif
#  if !defined __FLT_RADIX__
#   define __FLT_RADIX__		2				/* b */
#  endif
#  if !defined FLT_ROUNDS
#   define FLT_ROUNDS			1
#  endif
#  if !defined __FLT_EVAL_METHOD__
#   define __FLT_EVAL_METHOD__	0
#  endif
#  if !defined __DECIMAL_DIG__
#   define __DECIMAL_DIG__		17
#  endif
#  if !defined __FLT_MANT_DIG__
#   define __FLT_MANT_DIG__		24				/* p */
#  endif
#  if !defined __FLT_EPSILON__
#   define __FLT_EPSILON__		1.19209290E-07F /* b**(1-p) */
#  endif
#  if !defined __FLT_DIG__
#   define __FLT_DIG__			6				/* floor((p-1)*log10(b))+(b == 10) */
#  endif
#  if !defined FLT_DECIMAL_DIG
#   define FLT_DECIMAL_DIG		9				/* ceil(p*log10(2)+1) */
#  endif
#  if !defined __FLT_MIN_EXP__
#   define __FLT_MIN_EXP__		(-125)			/* emin */
#  endif
#  if !defined __FLT_MIN__
#   define __FLT_MIN__			1.17549435E-38F /* b**(emin-1) */
#  endif
#  if !defined __FLT_MIN_10_EXP__
#   define __FLT_MIN_10_EXP__	(-37)			/* ceil(log10(b**(emin-1))) */
#  endif
#  if !defined __FLT_MAX_EXP__
#   define __FLT_MAX_EXP__		128				/* emax */
#  endif
#  if !defined __FLT_MAX__
#   define	__FLT_MAX__			3.40282347E+38F /* (1-b**(-p))*b**emax */
#  endif
#  if !defined __FLT_MAX_10_EXP__
#   define __FLT_MAX_10_EXP__	38				/* floor(log10((1-b**(-p))*b**emax)) */
#  endif
#  if !defined __DBL_MANT_DIG__
#   define __DBL_MANT_DIG__		53
#  endif
#  if !defined __DBL_EPSILON__
#   define __DBL_EPSILON__		2.2204460492503131E-16
#  endif
#  if !defined __DBL_DIG__
#   define __DBL_DIG__			15
#  endif
#  if !defined DBL_DECIMAL_DIG
#   define DBL_DECIMAL_DIG		__DECIMAL_DIG__				
#  endif
#  if !defined __DBL_MIN_EXP__
#   define __DBL_MIN_EXP__		(-1021)
#  endif
#  if !defined __DBL_MIN__
#   define __DBL_MIN__			2.2250738585072014E-308
#  endif
#  if !defined __DBL_MIN_10_EXP__
#   define __DBL_MIN_10_EXP__	(-307)
#  endif
#  if !defined __DBL_MAX_EXP__
#   define __DBL_MAX_EXP__		1024
#  endif
#  if !defined __DBL_MAX__
#   define __DBL_MAX__			1.7976931348623157E+308
#  endif
#  if !defined __DBL_MAX_10_EXP__
#   define __DBL_MAX_10_EXP__	308
#  endif
#  if !defined __LDBL_MANT_DIG__
#   define __LDBL_MANT_DIG__ 	__DBL_MANT_DIG__
#  endif
#  if !defined __LDBL_EPSILON__
#   define __LDBL_EPSILON__		__DBL_EPSILON__
#  endif
#  if !defined __LDBL_DIG__
#   define __LDBL_DIG__ 		__DBL_DIG__
#  endif
#  if !defined LDBL_DECIMAL_DIG
#   define LDBL_DECIMAL_DIG		DBL_DECIMAL_DIG				
#  endif
#  if !defined __LDBL_MIN_EXP__
#   define __LDBL_MIN_EXP__		__DBL_MIN_EXP__
#  endif
#  if !defined __LDBL_MIN__
#   define __LDBL_MIN__			__DBL_MIN__
#  endif
#  if !defined __LDBL_MIN_10_EXP__
#   define __LDBL_MIN_10_EXP__	__DBL_MIN_10_EXP__
#  endif
#  if !defined __LDBL_MAX_EXP__
#   define __LDBL_MAX_EXP__		__DBL_MAX_EXP__
#  endif
#  if !defined __LDBL_MAX__
#   define __LDBL_MAX__			__DBL_MAX__
#  endif
#  if !defined __LDBL_MAX_10_EXP__
#   define __LDBL_MAX_10_EXP__	__DBL_MAX_10_EXP__
#  endif
# endif // defined ARDUINO

# if defined __HAS_NAMESPACES 

namespace std
{
	// std::float_denorm_style indicates support of subnormal values by floating-point types.
	enum float_denorm_style {
		denorm_indeterminate = -1,
		denorm_absent = 0,
		denorm_present = 1
	};

	// std::float_round_style indicates the rounding style used by floating-point arithmetics.
	enum float_round_style {
		round_indeterminate = -1,
		round_toward_zero = 0,
		round_to_nearest = 1,
		round_toward_infinity = 2,
		round_toward_neg_infinity = 3
	};

	// Primary template for std::numeric_limits.
	template<class T> class numeric_limits
	{
	public:
		static constexpr bool is_specialized = false;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = false;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = false;
		static constexpr bool is_modulo = false;
		static constexpr int digits = 0;
		static constexpr int digits10 = 0;
		static constexpr int max_digits10 = 0; 
		static constexpr int radix = 0;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = false;
		static constexpr bool tinyness_before = false;
		static constexpr T min() noexcept { return T(); }
		static constexpr bool lowest() noexcept { return T(); }
		static constexpr T max() noexcept { return T(); }
		static constexpr T epsilon() noexcept { return T(); }
		static constexpr T round_error() noexcept { return T(); }
		static constexpr T infinity() noexcept { return T(); }
		static constexpr T quiet_NaN() noexcept { return T(); }
		static constexpr T signaling_NaN() noexcept { return T(); }
		static constexpr T denorm_min() noexcept { return T(); }
	};

	// Specialization of template std::numeric_limits<bool>. 
	template<> class numeric_limits<bool>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr bool digits = 1;
		static constexpr bool digits10 = 0;
		static constexpr bool max_digits10 = 0;
		static constexpr bool radix = 2;
		static constexpr bool min_exponent = 0;
		static constexpr bool min_exponent10 = 0;
		static constexpr bool max_exponent = 0;
		static constexpr bool max_exponent10 = 0;
		static constexpr bool traps = false;
		static constexpr bool tinyness_before = false;
		static constexpr bool min() noexcept { return false; }
		static constexpr bool lowest() noexcept { return false; }
		static constexpr bool max() noexcept { return true; }
		static constexpr bool epsilon() noexcept { return false; }
		static constexpr bool round_error() noexcept { return false; }
		static constexpr bool infinity() noexcept { return false; }
		static constexpr bool quiet_NaN() noexcept { return false; }
		static constexpr bool signaling_NaN() noexcept { return false; }
		static constexpr bool denorm_min() noexcept { return false; }
	};

	// Specialization of template std::numeric_limits<char>. 
	template<> class numeric_limits<char>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr char digits = CHAR_BIT - numeric_limits<char>::is_signed;
		static constexpr char digits10 = numeric_limits<char>::digits * M_LOG10_2;
		static constexpr char max_digits10 = 0;
		static constexpr char radix = 2;
		static constexpr char min_exponent = 0;
		static constexpr char min_exponent10 = 0;
		static constexpr char max_exponent = 0;
		static constexpr char max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr char min() noexcept { return CHAR_MIN; }
		static constexpr char lowest() noexcept { return CHAR_MIN; }
		static constexpr char max() noexcept { return CHAR_MAX; }
		static constexpr char epsilon() noexcept { return 0; }
		static constexpr char round_error() noexcept { return 0; }
		static constexpr char infinity() noexcept { return 0; }
		static constexpr char quiet_NaN() noexcept { return 0; }
		static constexpr char signaling_NaN() noexcept { return 0; }
		static constexpr char denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<signed char>. 
	template<> class numeric_limits<signed char>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr signed char digits = CHAR_BIT - 1;
		static constexpr signed char digits10 = numeric_limits<signed char>::digits * M_LOG10_2;
		static constexpr signed char max_digits10 = 0;
		static constexpr signed char radix = 2;
		static constexpr signed char min_exponent = 0;
		static constexpr signed char min_exponent10 = 0;
		static constexpr signed char max_exponent = 0;
		static constexpr signed char max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr signed char min() noexcept { return SCHAR_MIN; }
		static constexpr signed char lowest() noexcept { return SCHAR_MIN; }
		static constexpr signed char max() noexcept { return SCHAR_MAX; }
		static constexpr signed char epsilon() noexcept { return 0; }
		static constexpr signed char round_error() noexcept { return 0; }
		static constexpr signed char infinity() noexcept { return 0; }
		static constexpr signed char quiet_NaN() noexcept { return 0; }
		static constexpr signed char signaling_NaN() noexcept { return 0; }
		static constexpr signed char denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<unsigned char>.
	template<> class numeric_limits<unsigned char>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr unsigned char digits = CHAR_BIT;
		static constexpr unsigned char digits10 = numeric_limits<unsigned char>::digits * M_LOG10_2;
		static constexpr unsigned char max_digits10 = 0;
		static constexpr unsigned char radix = 2;
		static constexpr unsigned char min_exponent = 0;
		static constexpr unsigned char min_exponent10 = 0;
		static constexpr unsigned char max_exponent = 0;
		static constexpr unsigned char max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr unsigned char min() noexcept { return 0; }
		static constexpr unsigned char lowest() noexcept { return 0; }
		static constexpr unsigned char max() noexcept { return UCHAR_MAX; }
		static constexpr unsigned char epsilon() noexcept { return 0; }
		static constexpr unsigned char round_error() noexcept { return 0; }
		static constexpr unsigned char infinity() noexcept { return 0; }
		static constexpr unsigned char quiet_NaN() noexcept { return 0; }
		static constexpr unsigned char signaling_NaN() noexcept { return 0; }
		static constexpr unsigned char denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<wchar_t>.
	template<> class numeric_limits<wchar_t>
	{
	public:
		static constexpr bool is_specialized = true; 
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr wchar_t digits = CHAR_BIT * sizeof(wchar_t) - numeric_limits<wchar_t>::is_signed;
		static constexpr wchar_t digits10 = numeric_limits<wchar_t>::digits * M_LOG10_2;
		static constexpr wchar_t max_digits10 = 0;
		static constexpr wchar_t radix = 2;
		static constexpr wchar_t min_exponent = 0;
		static constexpr wchar_t min_exponent10 = 0;
		static constexpr wchar_t max_exponent = 0;
		static constexpr wchar_t max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr wchar_t min() noexcept { return WCHAR_MIN; }
		static constexpr wchar_t lowest() noexcept { return WCHAR_MIN; }
		static constexpr wchar_t max() noexcept { return WCHAR_MAX; }
		static constexpr wchar_t epsilon() noexcept { return 0; }
		static constexpr wchar_t round_error() noexcept { return 0; }
		static constexpr wchar_t infinity() noexcept { return 0; }
		static constexpr wchar_t quiet_NaN() noexcept { return 0; }
		static constexpr wchar_t signaling_NaN() noexcept { return 0; }
		static constexpr wchar_t denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<char16_t>.
	template<> class numeric_limits<char16_t>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr char16_t digits = CHAR_BIT * sizeof(char16_t);
		static constexpr char16_t digits10 = numeric_limits<char16_t>::digits * M_LOG10_2;
		static constexpr char16_t max_digits10 = 0;
		static constexpr char16_t radix = 2;
		static constexpr char16_t min_exponent = 0;
		static constexpr char16_t min_exponent10 = 0;
		static constexpr char16_t max_exponent = 0;
		static constexpr char16_t max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr char16_t min() noexcept { return 0; }
		static constexpr char16_t lowest() noexcept { return 0; }
		static constexpr char16_t max() noexcept { return UINT_LEAST16_MAX; }
		static constexpr char16_t epsilon() noexcept { return 0; }
		static constexpr char16_t round_error() noexcept { return 0; }
		static constexpr char16_t infinity() noexcept { return 0; }
		static constexpr char16_t quiet_NaN() noexcept { return 0; }
		static constexpr char16_t signaling_NaN() noexcept { return 0; }
		static constexpr char16_t denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<char32_t>.
	template<> class numeric_limits<char32_t>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr char32_t digits = CHAR_BIT * sizeof(char32_t);
		static constexpr char32_t digits10 = numeric_limits<char32_t>::digits * M_LOG10_2;
		static constexpr char32_t max_digits10 = 0;
		static constexpr char32_t radix = 2;
		static constexpr char32_t min_exponent = 0;
		static constexpr char32_t min_exponent10 = 0;
		static constexpr char32_t max_exponent = 0;
		static constexpr char32_t max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr char32_t min() noexcept { return 0; }
		static constexpr char32_t lowest() noexcept { return 0; }
		static constexpr char32_t max() noexcept { return UINT_LEAST32_MAX; }
		static constexpr char32_t epsilon() noexcept { return 0; }
		static constexpr char32_t round_error() noexcept { return 0; }
		static constexpr char32_t infinity() noexcept { return 0; }
		static constexpr char32_t quiet_NaN() noexcept { return 0; }
		static constexpr char32_t signaling_NaN() noexcept { return 0; }
		static constexpr char32_t denorm_min() noexcept { return 0; }

	};

	// Specialization of template std::numeric_limits<signed short int>.
	template<> class numeric_limits<short>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr short digits = CHAR_BIT * sizeof(short) - 1;
		static constexpr short digits10 = numeric_limits<short>::digits * M_LOG10_2;
		static constexpr short max_digits10 = 0;
		static constexpr short radix = 2;
		static constexpr short min_exponent = 0;
		static constexpr short min_exponent10 = 0;
		static constexpr short max_exponent = 0;
		static constexpr short max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr short min() noexcept { return SHRT_MIN; }
		static constexpr short lowest() noexcept { return SHRT_MIN; }
		static constexpr short max() noexcept { return SHRT_MAX; }
		static constexpr short epsilon() noexcept { return 0; }
		static constexpr short round_error() noexcept { return 0; }
		static constexpr short infinity() noexcept { return 0; }
		static constexpr short quiet_NaN() noexcept { return 0; }
		static constexpr short signaling_NaN() noexcept { return 0; }
		static constexpr short denorm_min() noexcept { return 0; }

	};

	// Specialization of template std::numeric_limits<unsigned short int>.
	template<> class numeric_limits<unsigned short>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr unsigned short digits = CHAR_BIT * sizeof(short);
		static constexpr unsigned short digits10 = numeric_limits<unsigned short>::digits * M_LOG10_2;
		static constexpr unsigned short max_digits10 = 0;
		static constexpr unsigned short radix = 2;
		static constexpr unsigned short min_exponent = 0;
		static constexpr unsigned short min_exponent10 = 0;
		static constexpr unsigned short max_exponent = 0;
		static constexpr unsigned short max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr unsigned short min() noexcept { return 0; }
		static constexpr unsigned short lowest() noexcept { return 0; }
		static constexpr unsigned short max() noexcept { return UINT_MAX; }
		static constexpr unsigned short epsilon() noexcept { return 0; }
		static constexpr unsigned short round_error() noexcept { return 0; }
		static constexpr unsigned short infinity() noexcept { return 0; }
		static constexpr unsigned short quiet_NaN() noexcept { return 0; }
		static constexpr unsigned short signaling_NaN() noexcept { return 0; }
		static constexpr unsigned short denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<signed int>.
	template<> class numeric_limits<int>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr int digits = CHAR_BIT * sizeof(int) - 1;
		static constexpr int digits10 = numeric_limits<int>::digits * M_LOG10_2;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr int min() noexcept { return INT_MIN; }
		static constexpr int lowest() noexcept { return INT_MIN; }
		static constexpr int max() noexcept { return INT_MAX; }
		static constexpr int epsilon() noexcept { return 0; }
		static constexpr int round_error() noexcept { return 0; }
		static constexpr int infinity() noexcept { return 0; }
		static constexpr int quiet_NaN() noexcept { return 0; }
		static constexpr int signaling_NaN() noexcept { return 0; }
		static constexpr int denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<unsigned int>.
	template<> class numeric_limits<unsigned int>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr unsigned int digits = CHAR_BIT * sizeof(int);
		static constexpr unsigned int digits10 = numeric_limits<unsigned int>::digits * M_LOG10_2;
		static constexpr unsigned int max_digits10 = 0;
		static constexpr unsigned int radix = 2;
		static constexpr unsigned int min_exponent = 0;
		static constexpr unsigned int min_exponent10 = 0;
		static constexpr unsigned int max_exponent = 0;
		static constexpr unsigned int max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr unsigned int min() noexcept { return 0; }
		static constexpr unsigned int lowest() noexcept { return 0; }
		static constexpr unsigned int max() noexcept { return UINT_MAX; }
		static constexpr unsigned int epsilon() noexcept { return 0; }
		static constexpr unsigned int round_error() noexcept { return 0; }
		static constexpr unsigned int infinity() noexcept { return 0; }
		static constexpr unsigned int quiet_NaN() noexcept { return 0; }
		static constexpr unsigned int signaling_NaN() noexcept { return 0; }
		static constexpr unsigned int denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<signed long int>.
	template<> class numeric_limits<long>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr long digits = CHAR_BIT * sizeof(long) - 1;
		static constexpr long digits10 = numeric_limits<long>::digits * M_LOG10_2;
		static constexpr long max_digits10 = 0;
		static constexpr long radix = 2;
		static constexpr long min_exponent = 0;
		static constexpr long min_exponent10 = 0;
		static constexpr long max_exponent = 0;
		static constexpr long max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr long min() noexcept { return LONG_MIN; }
		static constexpr long lowest() noexcept { return LONG_MIN; }
		static constexpr long max() noexcept { return LONG_MAX; }
		static constexpr long epsilon() noexcept { return 0; }
		static constexpr long round_error() noexcept { return 0; }
		static constexpr long infinity() noexcept { return 0; }
		static constexpr long quiet_NaN() noexcept { return 0; }
		static constexpr long signaling_NaN() noexcept { return 0; }
		static constexpr long denorm_min() noexcept { return 0; }
	};

	// Specialization of template std::numeric_limits<unsigned long int>.
	template<> class numeric_limits<unsigned long>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr unsigned long digits = CHAR_BIT * sizeof(long);
		static constexpr unsigned long digits10 = numeric_limits<unsigned long>::digits * M_LOG10_2;
		static constexpr unsigned long max_digits10 = 0;
		static constexpr unsigned long radix = 2;
		static constexpr unsigned long min_exponent = 0;
		static constexpr unsigned long min_exponent10 = 0;
		static constexpr unsigned long max_exponent = 0;
		static constexpr unsigned long max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr unsigned long min() noexcept { return 0; }
		static constexpr unsigned long lowest() noexcept { return 0; }
		static constexpr unsigned long max() noexcept { return ULONG_MAX; }
		static constexpr unsigned long epsilon() noexcept { return 0; }
		static constexpr unsigned long round_error() noexcept { return 0; }
		static constexpr unsigned long infinity() noexcept { return 0; }
		static constexpr unsigned long quiet_NaN() noexcept { return 0; }
		static constexpr unsigned long signaling_NaN() noexcept { return 0; }
		static constexpr unsigned long denorm_min() noexcept { return 0; }
	};

	#if defined LLONG_MAX

	// Specialization of template std::numeric_limits<signed long long int>.
	template<> class numeric_limits<long long>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr long long digits = CHAR_BIT * sizeof(long long) - 1;
		static constexpr long long digits10 = numeric_limits<long long>::digits * M_LOG10_2;
		static constexpr long long max_digits10 = 0;
		static constexpr long long radix = 2;
		static constexpr long long min_exponent = 0;
		static constexpr long long min_exponent10 = 0;
		static constexpr long long max_exponent = 0;
		static constexpr long long max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr long long min() noexcept { return LLONG_MIN; }
		static constexpr long long lowest() noexcept { return LLONG_MIN; }
		static constexpr long long max() noexcept { return LLONG_MAX; }
		static constexpr long long epsilon() noexcept { return 0; }
		static constexpr long long round_error() noexcept { return 0; }
		static constexpr long long infinity() noexcept { return 0; }
		static constexpr long long quiet_NaN() noexcept { return 0; }
		static constexpr long long signaling_NaN() noexcept { return 0; }
		static constexpr long long denorm_min() noexcept { return 0; }
	};
	#endif // defined LLONG_MAX
	#if defined ULLONG_MAX

	// Specialization of template std::numeric_limits<unsigned long long int>.
	template<> class numeric_limits<unsigned long long>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr unsigned long long digits = CHAR_BIT * sizeof(long long);
		static constexpr unsigned long long digits10 = numeric_limits<unsigned long long>::digits * M_LOG10_2;
		static constexpr unsigned long long max_digits10 = 0;
		static constexpr unsigned long long radix = 2;
		static constexpr unsigned long long min_exponent = 0;
		static constexpr unsigned long long min_exponent10 = 0;
		static constexpr unsigned long long max_exponent = 0;
		static constexpr unsigned long long max_exponent10 = 0;
		static constexpr bool traps = true; /* undefined */
		static constexpr bool tinyness_before = false;
		static constexpr unsigned long long min() noexcept { return 0; }
		static constexpr unsigned long long lowest() noexcept { return 0; }
		static constexpr unsigned long long max() noexcept { return ULLONG_MAX; }
		static constexpr unsigned long long epsilon() noexcept { return 0; }
		static constexpr unsigned long long round_error() noexcept { return 0; }
		static constexpr unsigned long long infinity() noexcept { return 0; }
		static constexpr unsigned long long quiet_NaN() noexcept { return 0; }
		static constexpr unsigned long long signaling_NaN() noexcept { return 0; }
		static constexpr unsigned long long denorm_min() noexcept { return 0; }
	};

	#endif // defined ULLONG_MAX

	// Specialization of template std::numeric_limits<float>.
	template<> class numeric_limits<float>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = false;
		static constexpr bool has_infinity = false; /* undefined */
		static constexpr bool has_quiet_NaN = true;
		static constexpr bool has_signaling_NaN = false; /* undefined */
		static constexpr float_denorm_style has_denorm = denorm_present;
		static constexpr bool has_denorm_loss = false; /* undefined */
		static constexpr float_round_style round_style = round_to_nearest;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr int digits = FLT_MANT_DIG;
		static constexpr int digits10 = FLT_DIG;
		static constexpr int max_digits10 = FLT_DECIMAL_DIG;
		static constexpr int radix = FLT_RADIX;
		static constexpr int min_exponent = FLT_MIN_EXP;
		static constexpr int min_exponent10 = FLT_MIN_10_EXP;
		static constexpr int max_exponent = FLT_MAX_EXP;
		static constexpr int max_exponent10 = FLT_MAX_10_EXP;
		static constexpr bool traps = false; /* undefined */
		static constexpr bool tinyness_before = false; /* undefined */
		static constexpr float min() noexcept { return FLT_MIN; } 
		static constexpr float lowest() noexcept { return -FLT_MAX; } 
		static constexpr float max() noexcept { return FLT_MAX; } 
		static constexpr float epsilon() noexcept { return FLT_EPSILON; } 
		static constexpr float round_error() noexcept { return 0.5F; } 
		static constexpr float infinity() noexcept { return float(); } /* undefined */
		static constexpr float quiet_NaN() noexcept { return NAN; } 
		static constexpr float signaling_NaN() noexcept { return float(); } /* undefined */
		static constexpr float denorm_min() noexcept { return float(); } /* undefined */
	};

	// Specialization of template std::numeric_limits<double>.
	template<> class numeric_limits<double>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = false;
		static constexpr bool has_infinity = false; /* undefined */
		static constexpr bool has_quiet_NaN = true;
		static constexpr bool has_signaling_NaN = false; /* undefined */
		static constexpr float_denorm_style has_denorm = denorm_present;
		static constexpr bool has_denorm_loss = false; /* undefined */
		static constexpr float_round_style round_style = round_to_nearest;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr int digits = DBL_MANT_DIG;
		static constexpr int digits10 = DBL_DIG;
		static constexpr int max_digits10 = DBL_DECIMAL_DIG;
		static constexpr int radix = FLT_RADIX;
		static constexpr int min_exponent = DBL_MIN_EXP;
		static constexpr int min_exponent10 = DBL_MIN_10_EXP;
		static constexpr int max_exponent = DBL_MAX_EXP;
		static constexpr int max_exponent10 = DBL_MAX_10_EXP;
		static constexpr bool traps = false; /* undefined */
		static constexpr bool tinyness_before = false; /* undefined */
		static constexpr double min() noexcept { return DBL_MIN; } 
		static constexpr double lowest() noexcept { return -DBL_MAX; }
		static constexpr double max() noexcept { return DBL_MAX; }
		static constexpr double epsilon() noexcept { return DBL_EPSILON; }
		static constexpr double round_error() noexcept { return 0.5; }
		static constexpr double infinity() noexcept { return double(); } /* undefined */
		static constexpr double quiet_NaN() noexcept { return NAN; }
		static constexpr double signaling_NaN() noexcept { return double(); } /* undefined */
		static constexpr double denorm_min() noexcept { return double(); } /* undefined */
	};

	# if defined LDBL_MAX

	// Specialization of template std::numeric_limits<long double>.
	template<> class numeric_limits<long double>
	{
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = false;
		static constexpr bool has_infinity = false; /* undefined */
		static constexpr bool has_quiet_NaN = true;
		static constexpr bool has_signaling_NaN = false; /* undefined */
		static constexpr float_denorm_style has_denorm = denorm_present;
		static constexpr bool has_denorm_loss = false; /* undefined */
		static constexpr float_round_style round_style = round_to_nearest;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr int digits = LDBL_MANT_DIG; 
		static constexpr int digits10 = LDBL_DIG; 
		static constexpr int max_digits10 = LDBL_DECIMAL_DIG;
		static constexpr int radix = FLT_RADIX; 
		static constexpr int min_exponent = LDBL_MIN_EXP; 
		static constexpr int min_exponent10 = LDBL_MIN_10_EXP; 
		static constexpr int max_exponent = LDBL_MAX_EXP; 
		static constexpr int max_exponent10 = LDBL_MAX_10_EXP; 
		static constexpr bool traps = false; /* undefined */
		static constexpr bool tinyness_before = false; /* undefined */
		static constexpr long double min() noexcept { return LDBL_MIN; } 
		static constexpr long double lowest() noexcept { return -LDBL_MAX; } 
		static constexpr long double max() noexcept { return LDBL_MAX; } 
		static constexpr long double epsilon() noexcept { return LDBL_EPSILON; } 
		static constexpr long double round_error() noexcept { return 0.5L; } 
		static constexpr long double infinity() noexcept { return double(); } /* undefined */
		static constexpr long double quiet_NaN() noexcept { return NAN; } 
		static constexpr long double signaling_NaN() noexcept { return double(); } /* undefined */
		static constexpr long double denorm_min() noexcept { return double(); } /* undefined */
	};

# endif // defined LDBL_MAX

} // namespace std


# else // !defined __HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __HAS_NAMESPACES 
#endif // !defined NUMERIC_LIMITS_H__
