/*
 *	This file defines a class for serializing/deserializing objects to the 
 *	onboard EEPROM.
 *	
 *	***************************************************************************
 *
 *	File: EEStream.h
 *	Date: July 17, 2021
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
 *	***************************************************************************/

#if !defined __PG_EESTREAM_H 
#define __PG_EESTREAM_H 20210717L

# include <Arduino.h>	// Arduino genral API.
# include <EEPROM.h>	// Arduino EEPROM api.
# include "../interfaces/iserializable.h"	// `iserializable' interface.
# include "array"		// Fixed-size array types & iterators'.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// Type that serializes objects to and from the onboard EEPROM.
	class EEStream
	{
	public:
		using address_type = unsigned;	// Arduino EEPROM addressing type alias.
		using size_type = unsigned;		// Serializable object size type.

	public:
		EEStream() : address_() {}
		EEStream(const EEStream&) = delete;
		EEStream& operator=(const EEStream&) = delete;

	public:
		// Stream insertion operator.
		template<class T>
		EEStream& operator<<(const T&);
		// Stream extraction operator.
		template<class T>
		EEStream& operator>>(T&);
		// Returns a mutable reference to the current read/write address.
		address_type& address();
		// Returns a immutable reference to the current read/write address.
		const address_type& address() const;
		// Resets the EEPROM read/write address.
		void reset();
		// Deserializes an array of objects of type `T' from the EEPROM.
		template<class T, size_type N>
		void load(T(&t)[N]);
		// Deserializes an array of objects of type `T' from the EEPROM.
		template<class T>
		void load(T t[], size_type n);
		// Deserializes a range of objects of type `T' from the EEPROM.
		template<class T>
		void load(T* first, T* last);
		// Deserializes an object of type `T' from the EEPROM.
		template<class T>
		void load(T& t);
		// Serializes a collection of objects of type `T' to the EEPROM.
		template<class T, size_type N>
		void store(const T(&t)[N]);
		// Serializes a collection of objects of type `T' to the EEPROM.
		template<class T>
		void store(const T t[], size_type n);
		// Serializes a collection of objects of type `T' to the EEPROM.
		template<class T>
		void store(const T* first, const T* last);
		// Serializes an object of type `T' to the EEPROM.
		template<class T>
		void store(const T& t);

	public:
		// Reads the value of an object of type `T' from the EEPROM at the given address. 
		template<class T>
		static address_type	get(address_type, T&);
		// Reads the value of a `String' object from the EEPROM at the given address. 
		static address_type	get(address_type, String&);
		// Reads the value of a c-string object from the EEPROM at the given address. 
		static address_type	get(address_type, char*);
		// Writes the value of an object of type `T' to the EEPROM at the given address. 
		template<class T>
		static address_type	put(address_type, const T&);
		// Writes the value of a `String' object to the EEPROM at the given address. 
		static address_type	put(address_type, const String&);
		// Writes the value of a c-string object to the EEPROM at the given address. 
		static address_type	put(address_type, const char*);
		// Writes the value of an object of type `T' to the EEPROM at the given address 
		// if it differs from the currently stored value at that address.
		template<class T>
		static address_type	update(address_type, const T&);

	private:
		address_type address_;	// The current EEPROM read/write address.
	};

#pragma region non-static_functions

	template<class T>
	EEStream& EEStream::operator<<(const T& t)
	{
		address_ += update(address_, t);

		return *this;
	}

	template<class T>
	EEStream& EEStream::operator>>(T& t)
	{
		address_ += get(address_, t);

		return *this;
	}

	template<class T, EEStream::size_type N>
	void EEStream::load(T(&t)[N])
	{
		for (size_type i = 0; i < N; ++i)
			load(t[i]);
	}

	template<class T>
	void EEStream::load(T t[], size_type n)
	{
		for (size_type i = 0; i < n; ++i)
			load(t[i]);
	}

	template<class T>
	void EEStream::load(T* first, T* last)
	{
		for (auto it = first; it < last; ++it)
			load(*it);
	}

	template<class T>
	void EEStream::load(T& t)
	{
		T* p = &t;
		
		static_cast<iserializable*>(p)->deserialize(*this);
	}

	template<class T, EEStream::size_type N>
	void EEStream::store(const T(&t)[N])
	{
		for (size_type i = 0; i < N; ++i)
			store(t[i]);
	}

	template<class T>
	void EEStream::store(const T t[], size_type n)
	{
		for (size_type i = 0; i < n; ++i)
			store(t[i]);
	}

	template<class T>
	void EEStream::store(const T* first, const T* last)
	{
		for (auto it = first; it < last; ++it)
			store(*it);
	}

	template<class T>
	void EEStream::store(const T& t)
	{
		const T* p = &t;

		static_cast<const iserializable*>(p)->serialize(*this);
	}

	template<class T>
	EEStream::address_type EEStream::get(address_type address, T& value)
	{
		EEPROM.get(address, value);

		return sizeof(value);
	}

	template<class T>
	EEStream::address_type EEStream::put(address_type address, const T& value)
	{
		EEPROM.put(address, value);

		return sizeof(value);
	}

	template <class T>
	EEStream::address_type EEStream::update(address_type address, const T& value)
	{
		T t;
		address_type n = get(address, t);

		if (t != value)
			n = put(address, value);

		return n;	// Return the number of bytes read, or written if the update occured.
	}

#pragma endregion
#pragma region static_functions

	EEStream::address_type& EEStream::address()
	{
		return address_;
	}

	const EEStream::address_type& EEStream::address() const
	{
		return address_;
}

	void EEStream::reset()
	{
		address_ = 0;
	}

	EEStream::address_type EEStream::get(address_type address, String& value)
	{
		// String objects must be read as individual chars, preceded by the count, 
		// with an upper limit of 256 chars.

		return get(address, (char*)value.c_str());
	}

	EEStream::address_type EEStream::get(address_type address, char* value)
	{
		// C-strings must be read as individual chars, preceded by the count, with an 
		// upper limit of 256 chars.

		uint8_t count = EEPROM.read(address);
		address_type first = address;

		for (uint8_t i = 0U; i < count; i++)
			*value++ = static_cast<char>(EEPROM.read(++address));

		*value = '\0';
		return ++address - first;
	}

	EEStream::address_type EEStream::put(address_type address, const String& value)
	{
		// String objects must be written as individual chars, preceded by the count, 
		// with an upper limit of 256 chars.

		return put(address, value.c_str());
	}

	EEStream::address_type EEStream::put(address_type address, const char* value)
	{
		// C-strings must be written as individual chars, preceded by the count, with an 
		// upper limit of 256 chars.

		uint8_t count = strlen(value);
		address_type first = address;

		EEPROM.write(address, count);
		for (uint8_t i = 0U; i < count; i++)
			EEPROM.write(++address, value[i]);

		return ++address - first;
	}

#pragma endregion

} // namespace pg 

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_EESTREAM_H 