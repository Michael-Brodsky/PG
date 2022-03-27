/*
 *	This file defines wrapper types that encapsulate objects or object pairs 
 *	and expose getters and setters with familiar semantics
 *
 *	***************************************************************************
 *
 *	File: Wrappers.h
 *	Date: December 18, 2021
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
 *	**************************************************************************/

#if !defined __PG_WRAPPERS_H 
# define __PG_WRAPPERS_H 20211218L

# include "cstring"
# include "array"	// std::ArrayWrapper
# include "utility"	// std::pair


# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	template<class T, class U>
	using is_same_type = std::is_same<typename std::remove_cvref<typename std::remove_pointer<T>::type>::type, U>;

	// Wrapper type for an object of type T that exposes a getter and setter.
	template<class T>
	class ValueWrapper
	{
	public:
		using value_type = T;

	public:
		ValueWrapper() = default;
		ValueWrapper(const value_type& value) :
			value_(value) 
		{}

	public:
		operator value_type() { return value_; }
		value_type& operator()() { return value_; }
		const value_type& operator()() const { return value_; }

	private:
		value_type value_;
	};

	// Wrapper type for a pair of objects of type KeyType and ValueType that exposes getters and setters.
	template<class KeyType, class ValueType>
	class KeyValueWrapper
	{
	public:
		using key_type = KeyType;
		using value_type = ValueType;
		using key_value_type = std::pair<key_type, value_type>;

	public:
		KeyValueWrapper() = default;
		KeyValueWrapper(const key_type& key, const value_type& value) :
			key_value_(key_value_type{ key,value }) 
		{}
		KeyValueWrapper(const key_value_type& key_value) :
			key_value_(key_value) 
		{}

	public:
		key_value_type& operator()() { return key_value_; }
		const key_value_type& operator()() const { return key_value_; }
		key_type& key() { return key_value_.first; }
		const key_type& key() const { return key_value_.first; }
		value_type& value() { return key_value_.second; }
		const value_type& value() const { return key_value_.second; }

	private:
		key_value_type key_value_;
	};

	// Wrapper type that maps machine- to human-readable values and exposes getters and setters.
	template<class T, class CharT>
	class StringValueWrapper
	{
		static_assert(is_same_type<CharT, char>::value, "CharT must be of type or pointer to char");

	public:
		using value_type = T;
		using string_type = CharT;
		using string_value_type = std::pair<value_type, string_type>;

	public:
		StringValueWrapper() = default;
		StringValueWrapper(const value_type& value, const string_type& str) :
			string_value_(string_value_type{ value,str })
		{}
		StringValueWrapper(const string_value_type& string_value) :
			string_value_(string_value)
		{}

	public:
		friend bool operator==(const StringValueWrapper& other, const value_type& val) { return val == other.value(); }
		friend bool operator==(const StringValueWrapper& other, const string_type& val) { return !std::strncmp(other.string(), val, std::strlen(other.string())); }
		string_value_type& operator()() { return string_value_; }
		const string_value_type& operator()() const { return string_value_; }
		value_type& value() { return string_value_.first; }
		const value_type& value() const { return string_value_.first; }
		string_type& string() { return string_value_.second; }
		const string_type& string() const { return string_value_.second; }

	private:
		string_value_type string_value_;
	};

	template<class T>
	class CollectionWrapper
	{
	public:
		using container_type = std::ArrayWrapper<T*>;
		using value_type = typename container_type::value_type;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using pointer = typename container_type::pointer;
		using const_pointer = typename container_type::const_pointer;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;
		using difference_type = typename container_type::difference_type;
	public:
		CollectionWrapper() = default;
		template<std::size_t Size>
		CollectionWrapper(T* (&values)[Size]) :	container_(values) {}
		CollectionWrapper(T* values[], std::size_t n) : container_(values, n) {}
		CollectionWrapper(T** first, T** last) : container_(first, last) {}
		CollectionWrapper(std::initializer_list<T*> il) : container_(const_cast<T**>(il.begin()), il.size()) {}
		CollectionWrapper(const container_type& container) : container_(container) {}
		CollectionWrapper(const CollectionWrapper& other) = default;
		CollectionWrapper(CollectionWrapper&& other) = default;
	public:
		CollectionWrapper& operator=(const CollectionWrapper& other) = default;
		template<std::size_t Size>
		void values(T* (&vs)[Size]) { container_ = container_type(vs); }
		void values(T* vs[], std::size_t n) { container_ = container_type(vs, n); }
		void values(T** first, T** last) { container_ = container_type(first, last); }
		void values(std::initializer_list<T*> il) { container_ = container_type(const_cast<T**>(il.begin()), il.size()); }
		void values(const container_type& container) { container_ = container; }
		container_type& values() { return container_; }
		const container_type& values() const { return container_; }
	private:
		container_type container_;
	};

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_WRAPPERS_H 