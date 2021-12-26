/*
 *	This file defines wrapper types that encapsulate objects or object pairs 
 *	and expose getters and setters with familiar semantics
 *
 *	***************************************************************************
 *
 *	File: Values.h
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

#if !defined __PG_SETTING_H 
# define __PG_SETTING_H 20211218L

# include "utility"	// std::pair

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// Wrapper type for an object of type T that exposes a getter and setter.
	template<class T>
	class Value
	{
	public:
		using value_type = T;

	public:
		Value() = default;
		Value(const value_type& value) : 
			value_(value) 
		{}

	public:
		value_type& operator()() { return value_; }
		const value_type& operator()() const { return value_; }

	private:
		value_type value_;
	};

	// Wrapper type for a pair of objects of type KeyType and ValueType that exposes getters and setters.
	template<class KeyType, class ValueType>
	class KeyValue
	{
	public:
		using key_type = KeyType;
		using value_type = ValueType;
		using key_value_type = std::pair<key_type, value_type>;

	public:
		KeyValue() = default;
		KeyValue(const key_type& key, const value_type& value) : 
			key_value_(key_value_type{ key,value }) 
		{}
		KeyValue(const key_value_type& key_value) : 
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

	// Wrapper type for a pair of objects of type T and DT that exposes getters and setters.
	template<class T, class DT>
	class DisplayValue
	{
	public:
		using value_type = T;
		using display_type = DT;
		using display_value_type = std::pair<value_type, display_type>;

	public:
		DisplayValue() = default;
		DisplayValue(const value_type& value1, const display_type& value2) : 
			display_value_(display_value_type{ value1,value2 }) 
		{}
		DisplayValue(const display_value_type& display_value) : 
			display_value_(display_value) 
		{}

	public:
		display_value_type& operator()() { return display_value_; }
		const display_value_type& operator()() const { return display_value_; }
		value_type& value() { return display_value_.first; }
		const value_type& value() const { return display_value_.first; }
		display_type& display_value() { return display_value_.second; }
		const display_type& display_value() const { return display_value_.second; }

	private:
		display_value_type display_value_;
	};

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_SETTING_H 