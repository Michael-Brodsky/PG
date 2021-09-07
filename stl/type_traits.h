/*
 *	This file defines objects in the C++ Standard Template Library (STL) 
 *	type support library.
 *
 *	***************************************************************************
 *
 *	File: type_traits.h
 *	Date: August 22, 2021
 *	Version: 0.99
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
 *	Description:
 * 
 *		This file defines objects in the <type_traits> header of a C++ 
 *		Standard Template Library (STL) implementation - to the extent they 
 *		are supported by the Arduino implementation. The objects behave 
 *		according to the ISO C++11 Standard: (ISO/IEC 14882:2011), except as 
 *		noted.
 * 
 *	Notes:
 * 
 *		This implementation includes several of the useful C++14 and C++17 
 *		helper functions and types so far as they do not interfere with C++11
 *		compliance.
 *
 *	**************************************************************************/

#if !defined TYPE_TRAITS_H__
# define TYPE_TRAITS_H__ 20210822L

# include <stddef.h>	// Cstdlib common definitions.
# include <limits.h>	// Cstdlib numerical limits.
# include "pg.h"		// Pg environment.

# if defined __HAS_NAMESPACES

namespace std
{
#pragma region helper_functions

	template <class T, class U> struct is_same;

	// Compile-time constant of specified type with specified value.
	template<class T, T val>
	struct integral_constant
	{
		static constexpr T value = val;

		typedef T value_type;
		typedef integral_constant<T, val> type;

		constexpr operator value_type() const noexcept
		{
			return value;
		}
		constexpr value_type operator()() const noexcept 
		{ 
			return value; 
		}
	};

	// Compile-time constant of type bool with specified value.
	template<bool B>
	using bool_constant = integral_constant<bool, B>;

	// Compile-time constant of type bool with true value.
	using true_type = bool_constant<true>;

	// Compile-time constant of type bool with false value.
	using false_type = bool_constant<false>;

	// Compile-time logical negation of the type trait B.
	template<class B>
	struct negation : bool_constant<!bool(B::value)> { };

#pragma endregion
#pragma region const-volatile specifiers

	template<class T>
	struct remove_cv { typedef T type; };

	template<class T>
	struct remove_cv<const T> { typedef T type; };

	template<class T>
	struct remove_cv<volatile T> { typedef T type; };

	template<class T>
	struct remove_cv<const volatile T> { typedef T type; };

	template<class T>
	struct remove_const { typedef T type; };

	template<class T>
	struct remove_const<const T> { typedef T type; };

	template<class T>
	struct remove_volatile { typedef T type; };

	template<class T>
	struct remove_volatile<volatile T> { typedef T type; };

	template<class T> 
	struct add_cv { typedef const volatile T type; };

	template<class T> 
	struct add_const { typedef const T type; };

	template<class T> 
	struct add_volatile { typedef volatile T type; };

#pragma endregion
#pragma region reference_specifiers

	template<class T>
	struct remove_reference { typedef T type; };

	template<class T>
	struct remove_reference<T&> { typedef T type; };

	template<class T>
	struct remove_reference<T&&> { typedef T type; };

	template<class T>
	struct remove_cvref 
	{
		typedef typename std::remove_cv<typename remove_reference<T>::type>::type type;
	};

	namespace detail
	{
		template <class T>
		struct ref_type_identity { using type = T; };

		template <class T>
		auto try_add_lvalue_reference(int)->ref_type_identity<T&>;
		template <class T>
		auto try_add_lvalue_reference(...)->ref_type_identity<T>;

		template <class T>
		auto try_add_rvalue_reference(int)->ref_type_identity<T&&>;
		template <class T>
		auto try_add_rvalue_reference(...)->ref_type_identity<T>;

	} // namespace detail

	template <class T>
	struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0)) {};

	template <class T>
	struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0)) {};

	template<class T>
	typename add_rvalue_reference<T>::type declval();

#pragma endregion
#pragma region pointer_specifiers

	template<class T> 
	struct remove_pointer { typedef T type; };

	template<class T> 
	struct remove_pointer<T*> { typedef T type; };

	template<class T> 
	struct remove_pointer<T* const> { typedef T type; };

	template<class T> 
	struct remove_pointer<T* volatile> { typedef T type; };

	template<class T> 
	struct remove_pointer<T* const volatile> { typedef T type; };

	namespace detail
	{
		template <class T>
		struct ptr_type_identity { using type = T; };

		template <class T>
		auto try_add_pointer(int)->ptr_type_identity<typename std::remove_reference<T>::type*> {};
		template <class T>
		auto try_add_pointer(...)->ptr_type_identity<T> {};

	} // namespace detail

	template <class T>
	struct add_pointer : decltype(detail::try_add_pointer<T>(0)) {};

#pragma endregion
#pragma region array_specifiers

	template<class T>
	struct remove_extent { typedef T type; };

	template<class T>
	struct remove_extent<T[]> { typedef T type; };

	template<class T, std::size_t N>
	struct remove_extent<T[N]> { typedef T type; };

	template<class T>
	struct remove_all_extents { typedef T type; };

	template<class T>
	struct remove_all_extents<T[]> 
	{
		typedef typename remove_all_extents<T>::type type;
	};

	template<class T, std::size_t N>
	struct remove_all_extents<T[N]> 
	{
		typedef typename remove_all_extents<T>::type type;
	};

#pragma endregion
#pragma region property_queries

	template<class T>
	struct alignment_of : std::integral_constant<
		std::size_t,
		alignof(T)
	> {};

	template<class T>
	struct rank : public std::integral_constant<std::size_t, 0> {};

	template<class T>
	struct rank<T[]> : public std::integral_constant<std::size_t, rank<T>::value + 1> {};

	template<class T, std::size_t N>
	struct rank<T[N]> : public std::integral_constant<std::size_t, rank<T>::value + 1> {};

	template<class T, unsigned N = 0>
	struct extent : std::integral_constant<std::size_t, 0> {};

	template<class T>
	struct extent<T[], 0> : std::integral_constant<std::size_t, 0> {};

	template<class T, unsigned N>
	struct extent<T[], N> : std::extent<T, N - 1> {};

	template<class T, std::size_t I>
	struct extent<T[I], 0> : std::integral_constant<std::size_t, I> {};

	template<class T, std::size_t I, unsigned N>
	struct extent<T[I], N> : std::extent<T, N - 1> {};

#pragma endregion
#pragma region type_properties

	template<class T>
	struct is_void : public false_type {};

	template<>
	struct is_void<void> : public true_type {};

	template<class T>
	struct is_const : public false_type {};

	template<class T>
	struct is_const<const T> : public true_type {};

	template<class T>
	struct is_volatile : std::false_type {};

	template<class T>
	struct is_volatile<volatile T> : std::true_type {};

	template <class T>
	struct is_reference : public false_type {};

	template <class T>
	struct is_reference<T&> : public true_type {};

	template <class T>
	struct is_reference<T&&> : public true_type {};

	template<class T> struct is_lvalue_reference : std::false_type {};

	template<class T> struct is_lvalue_reference<T&> : std::true_type {};

	template <class T> struct is_rvalue_reference : std::false_type {};

	template <class T> struct is_rvalue_reference<T&&> : std::true_type {};

	template<class T>
	struct is_array : public false_type {};

	template<class T, std::size_t N>
	struct is_array<T[N]> : public true_type {};

	template<class T>
	struct is_array<T[]> : public true_type {};

	template<class T>
	struct is_function : integral_constant<
		bool,
		!is_const<const T>::value && !is_reference<T>::value> {};

	namespace detail
	{
		template<class T>
		struct is_pointer_helper : false_type {};

		template<class T>
		struct is_pointer_helper<T*> : true_type {};
	}

	template<class T>
	struct is_pointer : detail::is_pointer_helper<typename remove_cv<T>::type> {};

	template<class T>
	struct is_member_pointer_helper : std::false_type {};

	template< class T, class U >
	struct is_member_pointer_helper<T U::*> : std::true_type {};

	template<class T>
	struct is_member_pointer :
		is_member_pointer_helper<typename std::remove_cv<T>::type> {};

	template<class T>
	struct is_member_function_pointer_helper : std::false_type {};

	template<class T, class U>
	struct is_member_function_pointer_helper<T U::*> : std::is_function<T> {};

	template<class T>
	struct is_member_function_pointer
		: is_member_function_pointer_helper< typename std::remove_cv<T>::type > {};

	template<class T>
	struct is_member_object_pointer : std::integral_constant<
		bool,
		std::is_member_pointer<T>::value &&
		!std::is_member_function_pointer<T>::value
	> {};

	template<class T>
	struct is_union : public integral_constant<bool, __is_union(T)> {};

	template<class T>
	struct is_enum : public integral_constant<bool, __is_enum(T)> {};

	template<class T>
	struct is_class : public integral_constant<bool, __is_class(T)> {};

	template<class T>
	struct is_null_pointer : std::is_same<nullptr_t, typename remove_cv<T>::type> {};

	template<class T>
	struct is_integral : public false_type {};

	template<>
	struct is_integral<bool> : public true_type {};

	template<>
	struct is_integral<char> : public true_type {};

	template<>
	struct is_integral<unsigned char> : public true_type {};

	template<>
	struct is_integral<signed char> : public true_type {};

	template<>
	struct is_integral<wchar_t> : public true_type {};

	template<>
	struct is_integral<char16_t> : public true_type {};

	template<>
	struct is_integral<char32_t> : public true_type {};

	template<>
	struct is_integral<unsigned short> : public true_type {};

	template<>
	struct is_integral<signed short> : public true_type {};

	template<>
	struct is_integral<unsigned int> : public true_type {};

	template<>
	struct is_integral<signed int> : public true_type {};

	template<>
	struct is_integral<unsigned long> : public true_type {};

	template<>
	struct is_integral<signed long> : public true_type {};

#if defined LLONG_MAX
	template<>
	struct is_integral<signed long long> : public true_type {};

	template<>
	struct is_integral<unsigned long long> : public true_type {};
#endif // if defined LLONG_MAX

	template <class T> 
	struct is_integral<const T> : public is_integral<T> {};

	template <class T> 
	struct is_integral<volatile const T> : public is_integral<T> {};

	template <class T> 
	struct is_integral<volatile T> : public is_integral<T> {};

	template<class T>
	struct is_floating_point
		: std::integral_constant<
		bool,
		std::is_same<float, typename std::remove_cv<T>::type>::value ||
		std::is_same<double, typename std::remove_cv<T>::type>::value ||
		std::is_same<long double, typename std::remove_cv<T>::type>::value
		> {};

	template<class T>
	struct is_arithmetic : std::integral_constant<bool,
		std::is_integral<T>::value ||
		std::is_floating_point<T>::value> {};

	template<class T>
	struct is_scalar : std::integral_constant<bool,
		std::is_arithmetic<T>::value ||
		std::is_enum<T>::value ||
		std::is_pointer<T>::value ||
		std::is_member_pointer<T>::value ||
		std::is_null_pointer<T>::value>
	{};

	template<class T>
	struct is_object : std::integral_constant<bool,
		std::is_scalar<T>::value ||
		std::is_array<T>::value ||
		std::is_union<T>::value ||
		std::is_class<T>::value>
	{};

	template<class T>
	struct is_fundamental
		: std::integral_constant<
		bool,
		std::is_arithmetic<T>::value ||
		std::is_void<T>::value ||
		std::is_same<nullptr_t, typename std::remove_cv<T>::type>::value
		> {};

	template<class T>
	struct is_compound : std::integral_constant<bool, !std::is_fundamental<T>::value> {};

	namespace detail 
	{
		template<typename T, bool = std::is_arithmetic<T>::value>
		struct is_unsigned : std::integral_constant<bool, T(0) < T(-1) > {};

		template<typename T>
		struct is_unsigned<T, false> : std::false_type {};
	} // namespace detail

	template<typename T>
	struct is_unsigned : detail::is_unsigned<T>::type {};

	namespace detail 
	{
		template<typename T, bool = std::is_arithmetic<T>::value>
		struct is_signed : std::integral_constant<bool, T(-1) < T(0) > {};

		template<typename T>
		struct is_signed<T, false> : std::false_type {};
	} // namespace detail

	template<typename T>
	struct is_signed : detail::is_signed<T>::type {};

	namespace detail
	{
		template<class T>
		auto test_returnable(int) -> decltype(
			void(static_cast<T(*)()>(nullptr)), std::true_type{}
		);
		template<class>
		auto test_returnable(...)->std::false_type;

		template<class From, class To>
		auto test_implicitly_convertible(int) -> decltype(
			void(std::declval<void(&)(To)>()(std::declval<From>())), std::true_type{}
		);
		template<class, class>
		auto test_implicitly_convertible(...)->std::false_type;

	} // namespace detail

	template<class From, class To>
	struct is_convertible : std::integral_constant<bool,
		(decltype(detail::test_returnable<To>(0))::value&&
			decltype(detail::test_implicitly_convertible<From, To>(0))::value) ||
		(std::is_void<From>::value && std::is_void<To>::value)
	> {};


#pragma endregion
#pragma region type_relationships

	template<class T1, class T2>
	struct is_same : public false_type {};

	template<class T>
	struct is_same<T, T> : public true_type {};

	namespace details {
		template <typename B>
		std::true_type test_pre_ptr_convertible(const volatile B*);

		template <typename>
		std::false_type test_pre_ptr_convertible(const volatile void*);

		template <typename, typename>
		auto test_pre_is_base_of(...)->std::true_type;

		template <typename B, typename D>
		auto test_pre_is_base_of(int) ->
			decltype(test_pre_ptr_convertible<B>(static_cast<D*>(nullptr)));
	}

	template <typename Base, typename Derived>
	struct is_base_of :
		std::integral_constant<
		bool,
		std::is_class<Base>::value&& std::is_class<Derived>::value&&
		decltype(details::test_pre_is_base_of<Base, Derived>(0))::value
		> { };

#pragma endregion
#pragma region sign_modifiers

	template<class T>
	struct make_signed { typedef T type; };

	template<>
	struct make_signed<char> { typedef signed char type; };

	template<>
	struct make_signed<signed char> { typedef signed char type; };

	template<>
	struct make_signed<unsigned char> { typedef signed char type; };

	template<>
	struct make_signed<volatile char> { typedef volatile signed char type; };

	template<>
	struct make_signed<volatile signed char> { typedef volatile signed char type; };

	template<>
	struct make_signed<volatile unsigned char> { typedef volatile signed char type; };

	template<>
	struct make_signed<const char> { typedef const signed char type; };

	template<>
	struct make_signed<const signed char> { typedef const signed char type; };

	template<>
	struct make_signed<const unsigned char> { typedef const signed char type; };

	template<>
	struct make_signed<const volatile char> { typedef const volatile signed char type; };

	template<>
	struct make_signed<const volatile signed char> { typedef const volatile signed char type; };

	template<>
	struct make_signed<const volatile unsigned char> { typedef const volatile signed char type; };

	template<>
	struct make_signed<wchar_t> { typedef wchar_t type; };

	template<>
	struct make_signed<char16_t> { typedef signed short type; };

	template<>
	struct make_signed<char32_t> { typedef signed long type; };

	template<>
	struct make_signed<volatile wchar_t> { typedef volatile wchar_t type; };

	template<>
	struct make_signed<volatile char16_t> { typedef volatile signed short type; };

	template<>
	struct make_signed<volatile char32_t> { typedef volatile signed long type; };

	template<>
	struct make_signed<const wchar_t> { typedef const wchar_t type; };

	template<>
	struct make_signed<const char16_t> { typedef const signed short type; };

	template<>
	struct make_signed<const char32_t> { typedef const signed long type; };

	template<>
	struct make_signed<const volatile wchar_t> { typedef const volatile wchar_t type; };

	template<>
	struct make_signed<const volatile char16_t> { typedef const volatile signed short type; };

	template<>
	struct make_signed<const volatile char32_t> { typedef const volatile signed long type; };

	template<>
	struct make_signed<signed short> { typedef signed short type; };

	template<>
	struct make_signed<unsigned short> { typedef signed short type; };

	template<>
	struct make_signed<volatile signed short> { typedef volatile signed short type; };

	template<>
	struct make_signed<volatile unsigned short> { typedef volatile signed short type; };

	template<>
	struct make_signed<const signed short> { typedef const signed short type; };

	template<>
	struct make_signed<const unsigned short> { typedef const signed short type; };

	template<>
	struct make_signed<const volatile signed short> { typedef const volatile signed short type; };

	template<>
	struct make_signed<const volatile unsigned short> { typedef const volatile signed short type; };

	template<>
	struct make_signed<signed int> { typedef signed int type; };

	template<>
	struct make_signed<unsigned int> { typedef signed int type; };

	template<>
	struct make_signed<volatile signed int> { typedef volatile signed int type; };

	template<>
	struct make_signed<volatile unsigned int> { typedef volatile signed int type; };

	template<>
	struct make_signed<const signed int> { typedef const signed int type; };

	template<>
	struct make_signed<const unsigned int> { typedef const signed int type; };

	template<>
	struct make_signed<const volatile signed int> { typedef const volatile signed int type; };

	template<>
	struct make_signed<const volatile unsigned int> { typedef const volatile signed int type; };

	template<>
	struct make_signed<signed long> { typedef signed long type; };

	template<>
	struct make_signed<unsigned long> { typedef signed long type; };

	template<>
	struct make_signed<volatile signed long> { typedef volatile signed long type; };

	template<>
	struct make_signed<volatile unsigned long> { typedef volatile signed long type; };

	template<>
	struct make_signed<const signed long> { typedef const signed long type; };

	template<>
	struct make_signed<const unsigned long> { typedef const signed long type; };

	template<>
	struct make_signed<const volatile signed long> { typedef const volatile signed long type; };

	template<>
	struct make_signed<const volatile unsigned long> { typedef const volatile signed long type; };

#if defined LLONG_MAX
	template<>
	struct make_signed<signed long long> { typedef signed long long type; };

	template<>
	struct make_signed<unsigned long long> { typedef signed long long type; };

	template<>
	struct make_signed<volatile signed long long> { typedef volatile signed long long type; };

	template<>
	struct make_signed<volatile unsigned long long> { typedef volatile signed long long type; };

	template<>
	struct make_signed<const signed long long> { typedef const signed long long type; };

	template<>
	struct make_signed<const unsigned long long> { typedef const signed long long type; };

	template<>
	struct make_signed<const volatile signed long long> { typedef const volatile signed long long type; };

	template<>
	struct make_signed<const volatile unsigned long long> { typedef const volatile signed long long type; };
#endif // defined LLONG_MAX

	template<class T>
	struct make_unsigned { typedef T type; };

	template<>
	struct make_unsigned<char> { typedef unsigned char type; };

	template<>
	struct make_unsigned<signed char> { typedef unsigned char type; };

	template<>
	struct make_unsigned<unsigned char> { typedef unsigned char type; };

	template<>
	struct make_unsigned<volatile char> { typedef volatile unsigned char type; };

	template<>
	struct make_unsigned<volatile signed char> { typedef volatile unsigned char type; };

	template<>
	struct make_unsigned<volatile unsigned char> { typedef volatile unsigned char type; };

	template<>
	struct make_unsigned<const char> { typedef const unsigned char type; };

	template<>
	struct make_unsigned<const signed char> { typedef const unsigned char type; };

	template<>
	struct make_unsigned<const unsigned char> { typedef const unsigned char type; };

	template<>
	struct make_unsigned<const volatile char> { typedef const volatile unsigned char type; };

	template<>
	struct make_unsigned<const volatile signed char> { typedef const volatile unsigned char type; };

	template<>
	struct make_unsigned<const volatile unsigned char> { typedef const volatile unsigned char type; };

	template<>
	struct make_unsigned<wchar_t> { typedef unsigned short type; };

	template<>
	struct make_unsigned<char16_t> { typedef char16_t type; };

	template<>
	struct make_unsigned<char32_t> { typedef char32_t type; };

	template<>
	struct make_unsigned<volatile wchar_t> { typedef volatile unsigned short type; };

	template<>
	struct make_unsigned<volatile char16_t> { typedef volatile char16_t type; };

	template<>
	struct make_unsigned<volatile char32_t> { typedef volatile char32_t type; };

	template<>
	struct make_unsigned<const wchar_t> { typedef const unsigned short type; };

	template<>
	struct make_unsigned<const char16_t> { typedef const char16_t type; };

	template<>
	struct make_unsigned<const char32_t> { typedef const char32_t type; };

	template<>
	struct make_unsigned<const volatile wchar_t> { typedef const volatile unsigned short type; };

	template<>
	struct make_unsigned<const volatile char16_t> { typedef const volatile char16_t type; };

	template<>
	struct make_unsigned<const volatile char32_t> { typedef const volatile char32_t type; };

	template<>
	struct make_unsigned<signed short> { typedef unsigned short type; };

	template<>
	struct make_unsigned<unsigned short> { typedef unsigned short type; };

	template<>
	struct make_unsigned<volatile signed short> { typedef volatile unsigned short type; };

	template<>
	struct make_unsigned<volatile unsigned short> { typedef volatile unsigned short type; };

	template<>
	struct make_unsigned<const signed short> { typedef const unsigned short type; };

	template<>
	struct make_unsigned<const unsigned short> { typedef const unsigned short type; };

	template<>
	struct make_unsigned<const volatile signed short> { typedef const volatile unsigned short type; };

	template<>
	struct make_unsigned<const volatile unsigned short> { typedef const volatile unsigned short type; };

	template<>
	struct make_unsigned<signed int> { typedef unsigned int type; };

	template<>
	struct make_unsigned<unsigned int> { typedef unsigned int type; };

	template<>
	struct make_unsigned<volatile signed int> { typedef volatile unsigned int type; };

	template<>
	struct make_unsigned<volatile unsigned int> { typedef volatile unsigned int type; };

	template<>
	struct make_unsigned<const signed int> { typedef const unsigned int type; };

	template<>
	struct make_unsigned<const unsigned int> { typedef const unsigned int type; };

	template<>
	struct make_unsigned<const volatile signed int> { typedef const volatile unsigned int type; };

	template<>
	struct make_unsigned<const volatile unsigned int> { typedef const volatile unsigned int type; };

	template<>
	struct make_unsigned<signed long> { typedef unsigned long type; };

	template<>
	struct make_unsigned<unsigned long> { typedef unsigned long type; };

	template<>
	struct make_unsigned<volatile signed long> { typedef volatile unsigned long type; };

	template<>
	struct make_unsigned<volatile unsigned long> { typedef volatile unsigned long type; };

	template<>
	struct make_unsigned<const signed long> { typedef const unsigned long type; };

	template<>
	struct make_unsigned<const unsigned long> { typedef const unsigned long type; };

	template<>
	struct make_unsigned<const volatile signed long> { typedef const volatile unsigned long type; };

	template<>
	struct make_unsigned<const volatile unsigned long> { typedef const volatile unsigned long type; };

#if defined LLONG_MAX
	template<>
	struct make_unsigned<signed long long> { typedef unsigned long long type; };

	template<>
	struct make_unsigned<unsigned long long> { typedef unsigned long long type; };

	template<>
	struct make_unsigned<volatile signed long long> { typedef volatile unsigned long long type; };

	template<>
	struct make_unsigned<volatile unsigned long long> { typedef volatile unsigned long long type; };

	template<>
	struct make_unsigned<const signed long long> { typedef const unsigned long long type; };

	template<>
	struct make_unsigned<const unsigned long long> { typedef const unsigned long long type; };

	template<>
	struct make_unsigned<const volatile signed long long> { typedef const volatile unsigned long long type; };

	template<>
	struct make_unsigned<const volatile unsigned long long> { typedef const volatile unsigned long long type; };
#endif // defined LLONG_MAX

#pragma endregion
#pragma region miscellaneous_transformations

	template<bool B, class T, class F>
	struct conditional { typedef T type; };

	template<class T, class F>
	struct conditional<false, T, F> { typedef F type; };

	template<bool B, class T = void>
	struct enable_if {};

	template<class T>
	struct enable_if<true, T> { typedef T type; };

	template<class T>
	struct decay {
	private:
		typedef typename remove_reference<T>::type U;
	public:
		typedef typename conditional<
			is_array<U>::value,
			typename remove_extent<U>::type*,
			typename conditional<
			is_function<U>::value,
			typename add_pointer<U>::type,
			typename remove_cv<U>::type
			>::type
		>::type type;
	};

	template<class...>
	using void_t = void;

	// primary template (used for zero types)
	template<class...>
	struct common_type {};

	//////// one type
	template <class T>
	struct common_type<T> : common_type<T, T> {};

	namespace detail 
	{

		template<class T1, class T2>
		using conditional_result_t = decltype(false ? std::declval<T1>() : std::declval<T2>());

		template<class, class, class = void>
		struct decay_conditional_result {};
		template<class T1, class T2>
		struct decay_conditional_result<T1, T2, void_t<conditional_result_t<T1, T2>>>
			: std::decay<conditional_result_t<T1, T2>> {};

		template<class T1, class T2, class = void>
		struct common_type_2_impl : decay_conditional_result<const T1&, const T2&> {};

		template<class T1, class T2>
		struct common_type_2_impl<T1, T2, void_t<conditional_result_t<T1, T2>>>
			: decay_conditional_result<T1, T2> {};
	}

	//////// two types
	template<class T1, class T2>
	struct common_type<T1, T2>
		: std::conditional<std::is_same<T1, typename std::decay<T1>::type>::value&&
		std::is_same<T2, typename std::decay<T2>::type>::value,
		detail::common_type_2_impl<T1, T2>,
		common_type<typename std::decay<T2>::type,
		typename std::decay<T2>::type>>::type{};

	//////// 3+ types
	namespace detail {
		template<class AlwaysVoid, class T1, class T2, class...R>
		struct common_type_multi_impl {};
		template<class T1, class T2, class...R>
		struct common_type_multi_impl<void_t<typename common_type<T1, T2>::type>, T1, T2, R...>
			: common_type<typename common_type<T1, T2>::type, R...> {};
	}

	template<class T1, class T2, class... R>
	struct common_type<T1, T2, R...>
		: detail::common_type_multi_impl<void, T1, T2, R...> {};

	/* Experimental C++14 helper types.*/

	//template< class... T >
	//using common_type_t = typename common_type<T...>::type;

	//template< bool B, class T, class F >
	//using conditional_t = typename conditional<B, T, F>::type;

	template<class...> struct conjunction : std::true_type { };
	template<class U> struct conjunction<U> : U { };
	template<class U, class... Bn>
	struct conjunction<U, Bn...>
		: std::conditional<bool(U::value), std::conjunction<Bn...>, U>::type {};

#pragma endregion

#  if defined __HAS_CPP14 // C++14 helper types

	template<class T>
	using remove_cv_t = typename remove_cv<T>::type;

	template<class T>
	using remove_const_t = typename remove_const<T>::type;

	template<class T>
	using remove_volatile_t = typename remove_volatile<T>::type;

	template<class T>
	using add_cv_t = typename add_cv<T>::type;

	template<class T>
	using add_const_t = typename add_const<T>::type;

	template<class T>
	using add_volatile_t = typename add_volatile<T>::type;

	template<class T>
	using remove_reference_t = typename remove_reference<T>::type;

	template<class T>
	using remove_cvref_t = typename remove_cvref<T>::type;

	template<class T>
	using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

	template<class T>
	using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

	template<class T>
	using remove_pointer_t = typename remove_pointer<T>::type;

	template<class T>
	using add_pointer_t = typename add_pointer<T>::type;

	template<class T>
	using remove_extent_t = typename remove_extent<T>::type;

	template<class T>
	using remove_all_extents_t = typename remove_all_extents<T>::type;

	template<class T>
	using make_signed_t = typename make_signed<T>::type;

	template<class T>
	using make_unsigned_t = typename make_unsigned<T>::type;

	template< bool B, class T, class F >
	using conditional_t = typename conditional<B, T, F>::type;

	template<bool B, class T = void>
	using enable_if_t = typename enable_if<B, T>::type;

	template<class T>
	using decay_t = typename decay<T>::type;

	template<class...>
	using void_t = void;

#  endif // defined __HAS_CPP17
#  if defined __HAS_CPP17 // C++17 helper functions

	template<class B>
	inline constexpr bool negation_v = negation<B>::value;

	template<class T>
	inline constexpr std::size_t rank_v = rank<T>::value;

	template< class T, unsigned N = 0 >
	inline constexpr std::size_t extent_v = extent<T, N>::value;

	template< class T >
	inline constexpr bool is_void_v = is_void<T>::value;

	template<class T>
	inline constexpr bool is_const_v = is_const<T>::value;

	template<class T>
	inline constexpr bool is_volatile_v = is_volatile<T>::value;

	template<class T>
	inline constexpr bool is_reference_v = is_reference<T>::value;

	template<class T>
	inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

	template<class T>
	inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

	template<class T>
	inline constexpr bool is_array_v = is_array<T>::value;

	template< class T >
	inline constexpr bool is_function_v = is_function<T>::value;

	template< class T >
	inline constexpr bool is_pointer_v = is_pointer<T>::value;

	template<class T>
	inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

	template<class T>
	inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

	template<class T>
	inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

	template<class T>
	inline constexpr bool is_union_v = is_union<T>::value;

	template<class T>
	inline constexpr bool is_enum_v = is_enum<T>::value;

	template<class T>
	inline constexpr bool is_class_v = is_class<T>::value;

	template<class T>
	inline constexpr bool is_integral_v = is_integral<T>::value;

	template< class T >
	inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

	template<class T>
	inline constexpr bool is_scalar_v = is_scalar<T>::value;

	template<class T>
	inline constexpr bool is_object_v = is_object<T>::value;

	template<class T>
	inline constexpr bool is_fundamental_v = is_fundamental<T>::value;

	template<class T>
	inline constexpr bool is_compound_v = is_compound<T>::value;

	template<class T>
	inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

	template<class T, class U>
	inline constexpr bool is_same_v = is_same<T, U>::value;

	template<class Base, class Derived>
	inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

	template<class... B>
	inline constexpr bool conjunction_v = conjunction<B...>::value;

#  endif
} // namespace std

# else // !defined __HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __HAS_NAMESPACES
#endif // !defined TYPE_TRAITS_H__