/*
 *	This file defines types and functions that are part of the C++ Standard 
 *	Template Library (STL) general utility library.
 *
 *  ***************************************************************************
 *
 *	File: initializer_list.h
 *	Date: August 30, 2021
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
 *		This file defines objects in the <initializer_list> header of the C++
 *		Standard Template Library (STL) implementation - to the extent they
 *		are supported by the Arduino implementation. The objects behave
 *		according to the ISO C++11 Standard: (ISO/IEC 14882:2011), except as 
 *		noted.
 * 
 *		std::initializer_list is used to access the values in a C++ 
 *		initialization list, which is a list of elements of type const T. 
 *		Objects of this type are automatically constructed by the compiler 
 *		from initialization list declarations, whixh are lists of comma-
 *		separated elements enclosed in braces. The std::initializer_list class 
 *		template is not implicitly defined and this header <initializer_list.h> 
 *		must be included to access it, even if the type is used implicitly. 
 * 
 *		A std::initializer_list object is automatically constructed when:
 * 
 *			a braced-init-list is used to list-initialize an object, where 
 *			the corresponding constructor accepts a std::initializer_list 
 *			parameter, 
 * 
 *			a braced-init-list is used as the righthand operand of assignment 
 *			or as a function call argument, and the corresponding assignment 
 *			operator/function accepts a std::initializer_list parameter, 
 * 
 *			a braced-init-list is bound to auto, including in a ranged-based 
 *			for loop.
 * 
 *		std::initializer_list is implemented as a pointer and length. Copying 
 *		std::initializer_list does not copy the underlying objects. The 
 *		underlying array is a temporary array of type const T[N], in which 
 *		each element is copy-initialized from the corresponding element of the 
 *		original initializer list. The lifetime of the underlying array is the
 *		same as any other temporary object, except that initializing a 
 *		std::initializer_list object from the array extends the lifetime of 
 *		the array exactly like binding a reference to a temporary.
 *
 *	**************************************************************************/

#if !defined INITIALIZER_LIST_H__
# define INITIALIZER_LIST_H__ 20210822L

# include "pg.h" // Pg environment.

# if defined __HAS_NAMESPACES

namespace std
{
	// A lightweight proxy object providing access to an array of objects of type const T.
	template<class T>
	class initializer_list
	{
	public:
		using value_type = T;
		using reference = const T&;
		using const_reference = const T&;
		using size_type = std::size_t;
		using iterator = const T* ;
		using const_iterator = const T*;

	public:
		// Constructs an empty initializer list.
		inline constexpr initializer_list() noexcept : data_(), size_() {}

	public:
		// Returns the number of elements in the initializer list.
		inline constexpr std::size_t size() const noexcept { return size_; }
		// Returns a pointer to the first element.
		inline constexpr const T* begin() const noexcept { return data_; }
		// Returns a pointer to one past the last element.
		inline constexpr const T* end() const noexcept { return data_ + size_; }

	private:
		// Constructs an initializer list of size from the array pointed to by data.
		inline constexpr initializer_list(const T* data, std::size_t size) noexcept : 
			data_(data), size_(size)
		{}

	private:
		const T*		data_;	// Pointer to the underlying array.
		std::size_t		size_;	// Size, in elements, of the underlying array.
	};

	// Overload of std::begin for std::initializer_list, returns a pointer to the first element of list.
	template<class T>
	inline constexpr const T* begin(initializer_list<T> list) noexcept
	{
		return list.begin();
	}

	// Overload of std::end for std::initializer_list, returns a pointer to one past the last element of list.
	template<class T>
	inline constexpr const T* end(initializer_list<T> list) noexcept
	{
		return list.end();
	}
}

# else // !defined __HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __HAS_NAMESPACES
#endif // !defined INITIALIZER_LIST_H__