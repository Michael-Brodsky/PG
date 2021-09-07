/*
 *	This file defines several C++ Standard Template Library (STL) fixed-size 
 *	array types and functions.
 *
 *  ***************************************************************************
 *
 *	File: array.h
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
 *		This file defines objects in the <array> header of the C++
 *		Standard Template Library (STL) implementation - to the extent they
 *		are supported by the Arduino implementation. The objects behave
 *		according to the ISO C++11 Standard: (ISO/IEC 14882:2011). 
 * 
 *	**************************************************************************/

#if !defined ARRAY_H__
# define ARRAY_H__ 20210822L

# include <assert.h>	// `assert()' macro.
# include "iterator.h"	// Container iterator support.
# include "algorithm.h"	// Container algorithm functions.
# include "utility.h"	// Needed to specialize std::tuple types for std::array.

# if defined __HAS_NAMESPACES

namespace std
{
#pragma region random_access_iterators

// Iterator type for mutable sequences.
template<class T>
struct RandomAccessIterator
{
	using type = T*;
};

// Iterator type for immutable sequences.
template<class T>
struct ConstRandomAccessIterator
{
	using type = const T*;
};

#pragma endregion
# pragma region ConstReverseIterator

// Iterator type for immutable reversed sequences.
template<class RandomIt>
class ConstReverseIterator 
	: public iterator<
		typename iterator_traits<RandomIt>::iterator_category,
		typename iterator_traits<RandomIt>::value_type, 
		typename iterator_traits<RandomIt>::difference_type, 
		typename iterator_traits<RandomIt>::pointer, 
		typename iterator_traits<RandomIt>::reference>
{
public:		/**** Member Types and Constants ****/
	typedef RandomIt iterator_type;
	typedef ConstReverseIterator<iterator_type> self_type;
	typedef typename iterator_traits<iterator_type>::iterator_category iterator_category;
	typedef typename iterator_traits<iterator_type>::value_type value_type;
	typedef typename iterator_traits<iterator_type>::difference_type difference_type;
	typedef typename iterator_traits<iterator_type>::pointer pointer;
	typedef typename iterator_traits<iterator_type>::reference reference;

public:		/**** Ctors ****/
	ConstReverseIterator();
	explicit ConstReverseIterator(iterator_type);
	template<class Other>
	ConstReverseIterator(const ConstReverseIterator<Other>&);

public:		/**** Member Functions ****/
	iterator_type base() const;
	reference operator*() const;
	pointer operator->() const;
	reference operator[](difference_type) const;
	template<class Other>
	bool operator==(const ConstReverseIterator<Other>&) const;
	template<class Other>
	bool operator!=(const ConstReverseIterator<Other>&) const;
	template<class Other>
	bool operator<(const ConstReverseIterator<Other>&) const;
	template<class Other>
	bool operator>(const ConstReverseIterator<Other>&) const;
	template<class Other>
	bool operator<=(const ConstReverseIterator<Other>&) const;
	template<class Other>
	bool operator>=(const ConstReverseIterator<Other>&) const;
	self_type& operator++();
	self_type& operator--();
	self_type operator++(int);
	self_type operator--(int);
	self_type operator+(difference_type) const;
	self_type operator-(difference_type) const;
	self_type& operator+=(difference_type);
	self_type& operator-=(difference_type);
	
protected:	/**** Member Objects ****/
	RandomIt current_;
};

# pragma endregion

# pragma region ReverseIterator

// Iterator type for mutable reversed sequences.
template<class RandomIt>
class ReverseIterator	
	: public ConstReverseIterator<RandomIt>
{
public:		/**** Member Types and Constants ****/
	typedef RandomIt iterator_type;
	typedef ReverseIterator<iterator_type> self_type;
	typedef ConstReverseIterator<iterator_type> base_type;
	typedef typename iterator_traits<iterator_type>::iterator_category iterator_category;
	typedef typename iterator_traits<iterator_type>::value_type value_type;
	typedef typename iterator_traits<iterator_type>::difference_type difference_type;
	typedef typename iterator_traits<iterator_type>::pointer pointer;
	typedef typename iterator_traits<iterator_type>::reference reference;

public:		/**** Ctors ****/
	ReverseIterator();
	explicit ReverseIterator(iterator_type);
	template<class Other>
	ReverseIterator(const ReverseIterator<Other>&);
	template<class Other>
	self_type& operator=(const ReverseIterator<Other>&);

public:		/**** Member Functions ****/
	self_type& operator++();
	self_type& operator--();
	self_type operator++(int);
	self_type operator--(int);
	self_type operator+(difference_type) const;
	self_type operator-(difference_type) const;
	self_type& operator+=(difference_type);
	self_type& operator-=(difference_type);
};

# pragma endregion
# pragma region array 

// Sequence container type that encapsulates fixed-size arrays.
template<class T, std::size_t Size>
struct array 
{
/**** Member Types and Constants ****/
	typedef array<T, Size> self_type;
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef typename RandomAccessIterator<T>::type iterator;
	typedef typename ConstRandomAccessIterator<T>::type const_iterator;
	typedef ReverseIterator<iterator> reverse_iterator;
	typedef ConstReverseIterator<const_iterator> const_reverse_iterator;

/**** Member Functions ****/
	reference				at(size_type);
	const_reference			at(size_type) const;
	reference				operator[](size_type);
	const_reference			operator[](size_type) const;
	reference				front();
	const_reference			front() const;
	reference				back();
	const_reference			back() const;
	pointer					data();
	const_pointer			data() const;
	constexpr size_type		size() const noexcept;
	constexpr size_type		max_size() const noexcept;
	constexpr bool			empty() const noexcept;
	iterator				begin();
	const_iterator			begin() const;
	const_iterator			cbegin() const;
	iterator				end();
	const_iterator			end() const;
	const_iterator			cend() const;
	reverse_iterator		rbegin();
	const_reverse_iterator	rbegin() const;
	const_reverse_iterator	crbegin() const;
	reverse_iterator		rend();
	const_reverse_iterator	rend() const;
	const_reverse_iterator	crend() const;
	void					fill(const_reference);
	void					swap(self_type&);

/**** Member Objects ****/
	T	data_[Size == 0 ? 1U : Size];
};

# pragma endregion
# pragma region zero_length_array 

// Array template specialization for a zero element container.
template<class T>
struct array<T, 0>	
{
/**** Member Types ****/
	typedef array<T, 0> self_type;
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef typename RandomAccessIterator<T>::type iterator;
	typedef typename ConstRandomAccessIterator<T>::type const_iterator;
	typedef ReverseIterator<iterator> reverse_iterator;
	typedef ConstReverseIterator<const_iterator> const_reverse_iterator;

/**** Member Functions ****/
	reference				at(size_type);
	const_reference			at(size_type) const;
	reference				operator[](size_type);
	const_reference			operator[](size_type) const;
	reference				front();
	const_reference			front() const;
	reference				back();
	const_reference			back() const;
	pointer					data();
	const_pointer			data() const;
	constexpr size_type		size() const noexcept;
	constexpr size_type		max_size() const noexcept;
	constexpr bool			empty() const noexcept;
	iterator				begin();
	const_iterator			begin() const;
	const_iterator			cbegin() const;
	iterator				end();
	const_iterator			end() const;
	const_iterator			cend() const;
	reverse_iterator		rbegin();
	const_reverse_iterator	rbegin() const;
	const_reverse_iterator	crbegin() const;
	reverse_iterator		rend();
	const_reverse_iterator	rend() const;
	const_reverse_iterator	crend() const;
	void					fill(const_reference);
	void					swap(self_type&);
	
/**** Member Objects ****/
	T	data_[1U];
};

# pragma endregion
# pragma region ArrayWrapper 

/* Non-standard type, probably belongs in a separate file.*/

// Wrapper type for C-style fixed-size arrays of undeclared size.
template <class T>	
class ArrayWrapper 
{
public:		/**** Member Types ****/
	typedef ArrayWrapper<T> self_type;
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef typename RandomAccessIterator<T>::type iterator;
	typedef typename ConstRandomAccessIterator<T>::type const_iterator;
	typedef ReverseIterator<iterator> reverse_iterator;
	typedef ConstReverseIterator<const_iterator> const_reverse_iterator;

public:		/**** Ctors ****/
	ArrayWrapper() = default;
	template <std::size_t Size>
	explicit ArrayWrapper(T (&)[Size]);
	ArrayWrapper(T [], std::size_t);
	ArrayWrapper(T*, T*);

public:		/**** Member Functions ****/
	reference				at(size_type n);
	const_reference			at(size_type n) const;
	reference				operator[](size_type n);
	const_reference			operator[](size_type n) const;
	reference				front();
	const_reference			front() const;
	reference				back();
	const_reference			back() const;
	pointer					data();
	const_pointer			data() const;
	size_type				size() const;
	size_type				max_size() const;
	bool					empty() const;
	iterator				begin();
	const_iterator			begin() const;
	const_iterator			cbegin() const;
	iterator				end();
	const_iterator			end() const;
	const_iterator			cend() const;
	reverse_iterator		rbegin();
	const_reverse_iterator	rbegin() const;
	const_reverse_iterator	crbegin() const;
	reverse_iterator		rend();
	const_reverse_iterator	rend() const;
	const_reverse_iterator	crend() const;
	void					fill(const_reference);
	void					swap(self_type&);

private:	/**** Member Objects ****/
	T*			data_;
	size_type	size_;
};

# pragma endregion
#pragma region ConstReverseIterator_ctors

template <class RandomIt>
ConstReverseIterator<RandomIt>::ConstReverseIterator() : 
	current_() 
{
}

template <class RandomIt>
ConstReverseIterator<RandomIt>::ConstReverseIterator(iterator_type it) : 
	current_(it) 
{
}

template <class RandomIt>
template<class Other>
ConstReverseIterator<RandomIt>::ConstReverseIterator(const ConstReverseIterator<Other>& other) : 
	current_(other.base()) 
{
}

#pragma endregion

#pragma region ConstReverseIterator_member_functions

template<class RandomIt> inline 
	typename ConstReverseIterator<RandomIt>::iterator_type 
		ConstReverseIterator<RandomIt>::base() const 
{ 
	return current_; 
}

template<class RandomIt> inline 
	typename ConstReverseIterator<RandomIt>::reference 
		ConstReverseIterator<RandomIt>::operator*() const 
{
	iterator_type it(current_); return *--it;
}

template<class RandomIt> inline 
	typename ConstReverseIterator<RandomIt>::pointer 
		ConstReverseIterator<RandomIt>::operator->() const 
{ 
	return &(**this); 
}

template<class RandomIt> inline 
	typename ConstReverseIterator<RandomIt>::reference 
		ConstReverseIterator<RandomIt>::operator[](difference_type n) const 
{ 
	return (*(*this + n)); 
}

template<class RandomIt>
template<class Other>
	inline bool ConstReverseIterator<RandomIt>::operator==
		(const ConstReverseIterator<Other>& other) const 
{ 
	return current_ == other.base(); 
}

template<class RandomIt>
template<class Other>
	inline bool ConstReverseIterator<RandomIt>::operator!=
		(const ConstReverseIterator<Other>& other) const 
{ 
	return !(*this == other); 
}

template<class RandomIt>
template<class Other>
	inline bool ConstReverseIterator<RandomIt>::operator<
		(const ConstReverseIterator<Other>& other) const 
{ 
	return other.base() < current_; 
}

template<class RandomIt>
template<class Other>
	inline bool ConstReverseIterator<RandomIt>::operator>
		(const ConstReverseIterator<Other>& other) const 
{ 
	return other < *this; 
}

template<class RandomIt>
template<class Other>
	inline bool ConstReverseIterator<RandomIt>::operator<=
		(const ConstReverseIterator<Other>& other) const 
{
	return !(other < *this); 
}

template<class RandomIt>
template<class Other>
	inline bool ConstReverseIterator<RandomIt>::operator>=
		(const ConstReverseIterator<Other>& other) const 
{ 
	return !(*this < other); 
}

template<class RandomIt> inline 
	ConstReverseIterator<RandomIt>& 
		ConstReverseIterator<RandomIt>::operator++() 
{ 
	--current_; 
	return *this; 
}

template<class RandomIt> inline 
	ConstReverseIterator<RandomIt>& 
		ConstReverseIterator<RandomIt>::operator--() 
{ 
	++current_; 
	return *this; 
}

template<class RandomIt> inline 
	ConstReverseIterator<RandomIt>
		ConstReverseIterator<RandomIt>::operator++(int) 
{
	self_type it = *this; 
	--current_; 
	return it; 
}
template<class RandomIt> inline 
	ConstReverseIterator<RandomIt> 
		ConstReverseIterator<RandomIt>::operator--(int) 
{ 
	self_type it = *this; 
	++current_; 
	return it; 
}

template<class RandomIt> inline 
	ConstReverseIterator<RandomIt> 
		ConstReverseIterator<RandomIt>::operator+(difference_type n) const 
{ 
	return self_type(this->current_ - n); 
}

template<class RandomIt> inline 
	ConstReverseIterator<RandomIt> 
		ConstReverseIterator<RandomIt>::operator-(difference_type n) const 
{ 
	return self_type(current_ + n); 
}

template<class RandomIt> inline 
	ConstReverseIterator<RandomIt>& 
		ConstReverseIterator<RandomIt>::operator+=(difference_type n) 
{ 
	current_ -= n; 
	return *this; 
}

template<class RandomIt> inline 
	ConstReverseIterator<RandomIt>& 
		ConstReverseIterator<RandomIt>::operator-=(difference_type n) 
{ 
	current_ += n; 
	return *this; 
}

#pragma endregion

#pragma region ReverseIterator_ctors

template<class RandomIt>
ReverseIterator<RandomIt>::ReverseIterator() : 
	base_type() 
{
}

template<class RandomIt>
ReverseIterator<RandomIt>::ReverseIterator(iterator_type it) : 
	base_type(it) 
{
}

template<class RandomIt>
template<class Other>
ReverseIterator<RandomIt>::ReverseIterator(const ReverseIterator<Other>& other) : 
	base_type(other.base()) 
{
}

template<class RandomIt>
template<class Other> inline 
	ReverseIterator<RandomIt>& ReverseIterator<RandomIt>::operator=
		(const ReverseIterator<Other>& other) 
{ 
	base_type::current_ = other.base(); 
	return *this; 
}

#pragma endregion

#pragma region ReverseIterator_member_functions

template<class RandomIt> inline 
	ReverseIterator<RandomIt>& 
		ReverseIterator<RandomIt>::operator++() 
{ 
	base_type::operator++(); 
	return *this; 
}

template<class RandomIt> inline 
	ReverseIterator<RandomIt>& 
		ReverseIterator<RandomIt>::operator--() 
{ 
	base_type::operator--(); 
	return *this; 
}

template<class RandomIt> inline 
	ReverseIterator<RandomIt> 
		ReverseIterator<RandomIt>::operator++(int) 
{ 
	self_type it(*this); 
	++*this; 
	return it; 
}

template<class RandomIt> inline 
	ReverseIterator<RandomIt> 
		ReverseIterator<RandomIt>::operator--(int) 
{ 
	self_type it(*this); 
	--*this; 
	return it; 
}

template<class RandomIt> inline 
	ReverseIterator<RandomIt> 
		ReverseIterator<RandomIt>::operator+(difference_type n) const 
{ 
	self_type it(*this); 
	return (it += n); 
}

template<class RandomIt> inline 
	ReverseIterator<RandomIt> 
		ReverseIterator<RandomIt>::operator-(difference_type n) const 
{ 
	self_type it(*this); 
	return (it -= n); 
}

template<class RandomIt> inline 
	ReverseIterator<RandomIt>& 
		ReverseIterator<RandomIt>::operator+=(difference_type n) 
{ 
	base_type::operator+=(n); 
	return *this; 
}

template<class RandomIt> inline 
	ReverseIterator<RandomIt>& 
		ReverseIterator<RandomIt>::operator-=(difference_type n) 
{ 
	base_type::operator-=(n); 
	return *this; 
}

#pragma endregion
#pragma region array_member_functions

template<class T, std::size_t Size>
typename array<T, Size>::reference array<T, Size>::at(size_type n) 
{ 
	assert(n < Size); 
	if(Size <= n) 
		n = Size - 1U; 
	return data_[n]; 
}

template<class T, std::size_t Size>
typename array<T, Size>::const_reference array<T, Size>::at(size_type n) const 
{
	assert(n < Size); 
	if(Size <= n) 
		n = Size - 1U; 
	return data_[n]; 
}

template<class T, std::size_t Size>
typename array<T, Size>::reference array<T, Size>::operator[](size_type n) 
{
	return data_[n]; 
}

template<class T, std::size_t Size>
	typename array<T, Size>::const_reference 
		array<T, Size>::operator[](size_type n) const 
{ 
	return data_[n]; 
}

template<class T, std::size_t Size>
typename array<T, Size>::reference array<T, Size>::front() 
{ 
	return data_[0]; 
}

template<class T, std::size_t Size>
typename array<T, Size>::const_reference array<T, Size>::front() const 
{ 
	return data_[0]; 
}

template<class T, std::size_t Size>
typename array<T, Size>::reference array<T, Size>::back() 
{ 
	return data_[Size - 1U]; 
}

template<class T, std::size_t Size>
typename array<T, Size>::const_reference array<T, Size>::back() const 
{ 
	return data_[Size - 1U]; 
}

template<class T, std::size_t Size>
typename array<T, Size>::pointer array<T, Size>::data() 
{ 
	return data_; 
}

template<class T, std::size_t Size>
typename array<T, Size>::const_pointer array<T, Size>::data() const 
{ 
	return data_; 
}

template<class T, std::size_t Size>
constexpr typename array<T, Size>::size_type array<T, Size>::size() const 
{ 
	return Size; 
} 

template<class T, std::size_t Size>
constexpr typename array<T, Size>::size_type array<T, Size>::max_size() const 
{ 
	return Size; 
}

template<class T, std::size_t Size>
constexpr bool array<T, Size>::empty() const 
{ 
	return Size == 0; 
}

template<class T, std::size_t Size>
typename array<T, Size>::iterator array<T, Size>::begin() 
{ 
	return iterator(data_); 
}

template<class T, std::size_t Size>
typename array<T, Size>::const_iterator array<T, Size>::begin() const 
{ 
	return const_iterator(data_); 
}

template<class T, std::size_t Size>
typename array<T, Size>::const_iterator array<T, Size>::cbegin() const 
{ 
	return static_cast<const self_type *>(this)->begin(); 
}

template<class T, std::size_t Size>
typename array<T, Size>::iterator array<T, Size>::end() 
{ 
	return iterator(data_ + Size); 
}

template<class T, std::size_t Size>
typename array<T, Size>::const_iterator array<T, Size>::end() const 
{ 
	return const_iterator(data_ + Size); 
}

template<class T, std::size_t Size>
typename array<T, Size>::const_iterator array<T, Size>::cend() const 
{ 
	return static_cast<const self_type *>(this)->end(); 
}

template<class T, std::size_t Size>
	typename array<T, Size>::reverse_iterator 
		array<T, Size>::rbegin() 
{	
	return reverse_iterator(this->end()); 
}

template<class T, std::size_t Size>
	typename array<T, Size>::const_reverse_iterator 
		array<T, Size>::rbegin() const 
{	
	return const_reverse_iterator(this->end()); 
}

template<class T, std::size_t Size>
	typename array<T, Size>::const_reverse_iterator 
		array<T, Size>::crbegin() const 
{	
	return static_cast<const self_type *>(this)->rbegin();  
}

template<class T, std::size_t Size>
	typename array<T, Size>::reverse_iterator 
		array<T, Size>::rend() 
{	
	return reverse_iterator(this->begin()); 
}

template<class T, std::size_t Size>
	typename array<T, Size>::const_reverse_iterator 
		array<T, Size>::rend() const 
{	
	return const_reverse_iterator(this->begin()); 
}

template<class T, std::size_t Size>
	typename array<T, Size>::const_reverse_iterator 
		array<T, Size>::crend() const 
{	
	return static_cast<const self_type *>(this)->rend();  
}

template<class T, std::size_t Size>
void array<T, Size>::fill(const_reference value)
{
	std::fill(begin(), end(), value);
}

template<class T, std::size_t Size>
void array<T, Size>::swap(self_type& other) 
{ 
	std::swap_ranges(this->data_, (this->data_ + Size), other.data_);
}

#pragma endregion

#pragma region array_0_member_functions

template <class T>
typename array<T, 0>::reference array<T, 0>::at(size_type n) 
{ 
	assert(n < 0); 
	return data_[0]; 
}

template <class T>
typename array<T, 0>::const_reference array<T, 0>::at(size_type n) const 
{
	assert(n < 0); 
	return data_[0]; 
}

template <class T>
typename array<T, 0>::reference array<T, 0>::operator[](size_type n) 
{ 
	return data_[0]; 
}

template <class T>
typename array<T, 0>::const_reference array<T, 0>::operator[](size_type n) const 
{ 
	return data_[0]; 
}

template <class T>
typename array<T, 0>::reference array<T, 0>::front() 
{ 
	return data_[0]; 
}

template <class T>
typename array<T, 0>::const_reference array<T, 0>::front() const 
{	
	return data_[0]; 
}

template <class T>
typename array<T, 0>::reference array<T, 0>::back() 
{ 
	return data_[0]; 
}

template <class T>
typename array<T, 0>::const_reference array<T, 0>::back() const 
{ 
	return data_[0]; 
}

template <class T>
typename array<T, 0>::pointer array<T, 0>::data() 
{ 
	return nullptr; 
}

template <class T>
typename array<T, 0>::const_pointer array<T, 0>::data() const 
{ 
	return nullptr; 
}

template <class T>
constexpr typename array<T, 0>::size_type array<T, 0>::size() const
{ 
	return 0; 
}

template <class T>
constexpr typename array<T, 0>::size_type array<T, 0>::max_size() const
{ 
	return 0; 
}

template <class T>
constexpr bool array<T, 0>::empty() const
{ 
	return true; 
}

template<class T>
typename array<T, 0>::iterator array<T, 0>::begin() 
{ 
	return iterator(); 
}

template<class T>
typename array<T, 0>::const_iterator array<T, 0>::begin() const 
{ 
	return const_iterator(); 
}

template<class T>
typename array<T, 0>::const_iterator array<T, 0>::cbegin() const 
{ 
	return static_cast<const self_type *>(this)->begin(); 
}

template<class T>
typename array<T, 0>::iterator array<T, 0>::end() 
{ 
	return iterator(); 
}

template<class T>
typename array<T, 0>::const_iterator array<T, 0>::end() const 
{ 
	return const_iterator(); 
}

template<class T>
typename array<T, 0>::const_iterator array<T, 0>::cend() const 
{ 
	return static_cast<const self_type *>(this)->end(); 
}

template<class T>
	typename array<T, 0>::reverse_iterator 
		array<T, 0>::rbegin() 
{	
	return reverse_iterator(this->end()); 
}

template<class T>
	typename array<T, 0>::const_reverse_iterator 
		array<T, 0>::rbegin() const 
{	
	return const_reverse_iterator(this->end()); 
}

template<class T>
	typename array<T, 0>::const_reverse_iterator 
		array<T, 0>::crbegin() const 
{	
	return static_cast<const self_type *>(this)->rbegin();  
}

template<class T>
	typename array<T, 0>::reverse_iterator 
		array<T, 0>::rend() 
{	
	return reverse_iterator(this->begin()); 
}

template<class T>
	typename array<T, 0>::const_reverse_iterator 
		array<T, 0>::rend() const 
{	
	return const_reverse_iterator(this->begin()); 
}

template<class T>
	typename array<T, 0>::const_reverse_iterator 
		array<T, 0>::crend() const 
{	
	return static_cast<const self_type *>(this)->rend();  
}

template<class T>
void array<T, 0>::fill(const_reference value)
{
	std::fill(begin(), end(), value);
}

template<class T>
void array<T, 0>::swap(self_type& other) 
{ 
	std::swap_ranges(this->data_, (this->data_ + 0), other.data_); 
}

#pragma endregion
#pragma region ArrayWrapper_ctors

template <class T>
template <std::size_t Size>
ArrayWrapper<T>::ArrayWrapper(T (&data)[Size]) : 
	data_(data), size_(Size) 
{ 
}

template <class T>
ArrayWrapper<T>::ArrayWrapper(T data[], std::size_t size) :
	data_(data), size_(size) 
{ 
}

template <class T>
ArrayWrapper<T>::ArrayWrapper(T* begin, T* end) : 
	data_(begin), size_(distance(begin, end)) 
{ 
}

#pragma endregion
#pragma region ArrayWrapper_member_functions

template<class T>
typename ArrayWrapper<T>::reference ArrayWrapper<T>::at(size_type n) 
{ 
	assert(n < size_); 
	if(size_ <= n) 
		n = size_ - 1U; 
	return data_[n]; 
}

template<class T>
typename ArrayWrapper<T>::const_reference ArrayWrapper<T>::at(size_type n) const 
{
	assert(n < size_); 
	if(size_ <= n) 
		n = size_ - 1U; 
	return data_[n]; 
}

template<class T>
typename ArrayWrapper<T>::reference ArrayWrapper<T>::operator[](size_type n) 
{
	return data_[n]; 
}

template<class T>
	typename ArrayWrapper<T>::const_reference 
		ArrayWrapper<T>::operator[](size_type n) const 
{ 
	return data_[n]; 
}

template<class T>
typename ArrayWrapper<T>::reference ArrayWrapper<T>::front() 
{ 
	return data_[0]; 
}

template<class T>
typename ArrayWrapper<T>::const_reference ArrayWrapper<T>::front() const 
{ 
	return data_[0]; 
}

template<class T>
typename ArrayWrapper<T>::reference ArrayWrapper<T>::back() 
{ 
	return data_[size_ - 1U]; 
}

template<class T>
typename ArrayWrapper<T>::const_reference ArrayWrapper<T>::back() const 
{ 
	return data_[size_ - 1U]; 
}

template<class T>
typename ArrayWrapper<T>::pointer ArrayWrapper<T>::data() 
{ 
	return data_; 
}

template<class T>
typename ArrayWrapper<T>::const_pointer ArrayWrapper<T>::data() const 
{ 
	return data_; 
}

template<class T>
typename ArrayWrapper<T>::size_type ArrayWrapper<T>::size() const 
{ 
	return size_; 
} 

template<class T>
typename ArrayWrapper<T>::size_type ArrayWrapper<T>::max_size() const 
{ 
	return size_; 
}

template<class T>
bool ArrayWrapper<T>::empty() const 
{ 
	return size_ == 0; 
}

template<class T>
typename ArrayWrapper<T>::iterator ArrayWrapper<T>::begin() 
{ 
	return iterator(data_); 
}

template<class T>
typename ArrayWrapper<T>::const_iterator ArrayWrapper<T>::begin() const 
{ 
	return const_iterator(data_); 
}

template<class T>
typename ArrayWrapper<T>::const_iterator ArrayWrapper<T>::cbegin() const 
{ 
	return static_cast<const self_type *>(this)->begin(); 
}

template<class T>
typename ArrayWrapper<T>::iterator ArrayWrapper<T>::end() 
{ 
	return iterator(data_ + size_); 
}

template<class T>
typename ArrayWrapper<T>::const_iterator ArrayWrapper<T>::end() const 
{ 
	return const_iterator(data_ + size_); 
}

template<class T>
typename ArrayWrapper<T>::const_iterator ArrayWrapper<T>::cend() const 
{ 
	return static_cast<const self_type*>(this)->end(); 
}

template <class T>
	typename ArrayWrapper<T>::reverse_iterator 
		ArrayWrapper<T>::rbegin() 
{	
	return reverse_iterator(this->end()); 
}

template <class T>
	typename ArrayWrapper<T>::const_reverse_iterator 
		ArrayWrapper<T>::rbegin() const 
{	
	return const_reverse_iterator(this->end()); 
}

template <class T>
	typename ArrayWrapper<T>::const_reverse_iterator 
		ArrayWrapper<T>::crbegin() const 
{	
	return static_cast<const self_type*>(this)->rbegin();  
}

template <class T>
	typename ArrayWrapper<T>::reverse_iterator 
		ArrayWrapper<T>::rend() 
{	
	return reverse_iterator(this->begin()); 
}

template <class T>
	typename ArrayWrapper<T>::const_reverse_iterator 
		ArrayWrapper<T>::rend() const 
{	
	return const_reverse_iterator(this->begin()); 
}

template <class T>
	typename ArrayWrapper<T>::const_reverse_iterator 
		ArrayWrapper<T>::crend() const 
{	
	return static_cast<const self_type*>(this)->rend();  
}

template <class T>
void ArrayWrapper<T>::fill(const_reference value)
{
	std::fill(begin(), end(), value);
}

template<class T>
void ArrayWrapper<T>::swap(self_type& other) 
{ 
	std::swap_ranges(this->data_, (this->data_ + size_), other.data_); 
}

#pragma endregion
#pragma region array_non-member_functions

template<class T, std::size_t Size>
void swap(array<T, Size>& lhs, array<T, Size>& rhs)
{	// Swap the contents of two containers.
	return (lhs.swap(rhs));
}

template<class T, std::size_t Size>
bool operator==(const array<T, Size>& lhs, const array<T, Size>& rhs)
{	// Returns `true' if the contents of two containers are equal, else returns `false'.
	return (std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template<class T, std::size_t Size>
bool operator!=(const array<T, Size>& lhs, const array<T, Size>& rhs)
{	// Returns `true' if the contents of two containers are not equal, else returns `false'.
	return (!(lhs == rhs));
}

template<class T, std::size_t Size>
bool operator<(const array<T, Size>& lhs, const array<T, Size>& rhs)
{	// Returns `true' if the contents of container `lhs' is less than `rhs', else returns `false'.
	return (lexicographical_compare(lhs.begin(), lhs.end(),
		rhs.begin(), rhs.end()));
}

template<class T, std::size_t Size>
bool operator>(const array<T, Size>& lhs, const array<T, Size>& rhs)
{	// Returns `true' if the contents of container `lhs' is greater than `rhs', else returns `false'.
	return (rhs < lhs);
}

template<class T, std::size_t Size>
bool operator<=(const array<T, Size>& lhs, const array<T, Size>& rhs)
{	// Returns `true' if the contents of container `lhs' is less than or equal to `rhs', else returns `false'.
	return (!(rhs < lhs));
}

template<class T, std::size_t Size>
bool operator>=(const array<T, Size>& lhs, const array<T, Size>& rhs)
{	// Returns `true' if the contents of container `lhs' is greater than or equal to `rhs', else returns `false'.
	return (!(lhs < rhs));
}

#pragma endregion

#pragma region miscellaneous_functions

// Specialization of std::tuple_size for std::array.
template<class T, std::size_t N>
struct tuple_size<std::array<T, N>> :
	std::integral_constant<std::size_t, N>
{};

// Specialization of std::size for std::array.
// ???

// Specialization of std::get for std::array.
template<std::size_t I, class T, std::size_t N>
T& get(std::array<T, N>& a) noexcept
{
	return a[I];
}

// Specialization of std::get for std::array.
template<std::size_t I, class T, std::size_t N>
T&& get(std::array<T, N>&& a) noexcept
{
	return std::forward<T>(a[I]);
}

// Specialization of std::get for std::array.
template<std::size_t I, class T, std::size_t N>
const T& get(const std::array<T, N>& a) noexcept
{
	return a[I];
}

// Specialization of std::get for std::array.
template< std::size_t I, class T, std::size_t N >
const T&& get(const std::array<T, N>&& a) noexcept 
{
	return std::forward<T>(a[I]);
}

// Creates an object of type `array' from an C-style array of 
// unspecified size. The `array' size and elements are identical to 
// those of the C-style array. Standard template argument deduction/
// substitution rules apply. Example: int i[] = {1, 2, 3}; 
// auto a = make_array(i);
template<class T, std::size_t Size>
array<T, Size> make_array(T(&t)[Size])
{
	array<T, Size> tmp;
	for (typename array<T, Size>::size_type it = 0; it != Size; ++it)
		tmp[it] = t[it];
	return tmp;
}

namespace experimental
{
	namespace details
	{
		template<class> struct is_ref_wrapper : std::false_type {};

		template<class T>
		using not_ref_wrapper = std::negation<is_ref_wrapper<typename decay<T>::type>>;

		template <class D, class...> struct return_type_helper { using type = D; };
		template <class... Types>
		struct return_type_helper<void, Types...> : std::common_type<Types...> {
			static_assert(std::conjunction<not_ref_wrapper<Types>...>::value,
				"Types cannot contain reference_wrappers when D is void");
		};

		template <class D, class... Types>
		using return_type = std::array<typename return_type_helper<D, Types...>::type,
			sizeof...(Types)>;
	} // namespace details

	// std::experimental::make_array(). Creates an object of type `array' 
	// whose size is equal to the number of arguments and whose elements 
	// are initialized from the corresponding arguments. Template argument 
	// type `D' must be specified and cannot be of type `void'. Example: 
	// auto a = make_array<int>(1, 2, 3);

	template < class D = void, class... Types>
	constexpr details::return_type<D, Types...> make_array(Types&&... t) {
		return { std::forward<Types>(t)... };
	}
}

#pragma endregion
} // namespace std

# else // !defined __HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __HAS_NAMESPACES
#endif // !defined ARRAY_H__