/*
 *	This file provides support for streaming objects to and from the onboard 
 *	EEPROM.
 *
 *	***************************************************************************
 *
 *	File: EEStream.h
 *	Date: November 2, 2021
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
 *	The EEStream class provides simple object streaming to and from the 
 *	onboard EEPROM memory and manages memory addressing. EEStream defines an 
 *	insertion (operator<<) and extraction (operator>>) operator, an address() 
 *	function overload which can be used to get/set the current EEPROM read/
 *	write address and a reset() function to reset the address back to zero.
 *	Two i/o manipulator types, `update' and `noupdate' are also provided to 
 *	toggle between EEPROM update/overwrite functionality.
 * 
 *	Clients stream objects to (write to) the EEPROM using the insertion 
 *	operator: 
 * 
 *		EEStream e;	// Address initialized to 0.
 *		int i = 42;
 *		float f = 99.9;
 *		const char* str = "Hello World!"; // C-style NULL-terminated string.
 *		e << i;		// Writes 42 at address 0 and advances the address.
 *		e << f;		// Writes 99.9 after 42 and advances the address.
 *		e << str;	// Writes "Hello World!" after 99.9 and advances the addr.
 * 
 *	By reseting the address and reading objects from the EEPROM in the same 
 *	order they were written, large collections of objects can be saved to and 
 *	recalled from the EEPROM with just a few simple lines of code:
 * 
 *		e.reset();
 * 
 *	Clients stream objects from (read from) the EEPROM using the extraction  
 *	operator: 
 * 
 *		e >> i;
 *		e >> f;
 *		e >> str;   // String buffers must be allocated and large enough to 
 *					// hold the data read from the EEPROM, including the 
 *					// terminating NULL, e.g.
 *		char buf[13]; 
 *		e >> buf;	// OK to hold "Hello World!"
 * 
 *	The address() function either gets or sets the current read/write address: 
 * 
 *		std::size_t addr = e.address();
 *		e.address() = ++addr;
 * 
 *	The nested i/o manipulator types `update' and `noupdate' are used to turn 
 *	the EEPROM update/noupdate functionality on and off. If update is on, 
 *	data is only written if it differs from the currently stored data (much 
 *	like the Arduino EEPROM.update() function except that it works with any 
 *	type instead of only one byte at a time). If update is off, data is 
 *	automatically written regardless of the EEPROM's current contents. Objects 
 *	must be equal comparable to use update. The update and noupdate types are 
 *	simply streamed to the EEStream object, like any other type, the same way 
 *	std::boolalpha is used with std::iostream in the C++ Standard Library:
 * 
 *		e << EEStream::update(); // Turns on updating.
 *		e << EEStream::noupdate(); // Turns off updating.
 * 
 *	Client types can override the insertion and extraction operators to 
 *	implement custom streaming behavior: 
 * 
 *	class A { 
 *		...
 *  public:
 *		// Operator overrides can implement any functionality they choose. 
 *		EEStream& operator>>(EEStream& e) { e << fl; e << ln; } 
 *		EEStream& operator<<(EEStream& e) { e >> fl; e >> ln; } 
 *		// Objects should be equal-comparable.
 *		bool operator==(const A& other) { return fl == other.fl && ln == other.ln; }
 *  private:
 *		float fl;
 *		long  ln;
 *  }
 * 
 *	Types using their overrides must appear as the lefthand side operand:
 *
 *		EEStream ee;
 *		A a;
 *		a >> ee;	// Uses A's insertion operator.
 *		ee << a;	// Uses EEStream's insertion operator.
 * 
 *	**************************************************************************/

#if !defined __PG_EESTREAM_H
# define __PG_EESTREAM_H 20211102L

# include <Arduino.h>	// Arduino system api.
# include <EEPROM.h>	// Arduino EEPROM api.
# include "type_traits"	// Type support library.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// Type that provides EEPROM streaming services.
	class EEStream
	{
	public:
		using address_type = unsigned;	// EEPROM addressing type alias.

		struct update {};	// Tag type used to enable EEPROM update function.
		struct noupdate {};	// Tag type used to disable EEPROM update function.

	public:
		// Constructs an EEStream.
		EEStream();
		// No copy constructor.
		EEStream(const EEStream&) = delete;
		// Move constructor.
		EEStream(EEStream&&) = default;
		// No copy assignment operator.
		EEStream& operator=(const EEStream&) = delete;

	public:
		// Stream insertion operator.
		template<class T>
		EEStream& operator<<(const T&);
		// Stream extraction operator.
		template<class T>
		EEStream& operator>>(T&);
		// Stream array insertion operator.
		template<class T, std::size_t N>
		EEStream& operator<<(const T(&t)[N]);
		// Stream array extraction operator.
		template<class T, std::size_t N>
		EEStream& operator>>(T(&t)[N]);
		// Returns a mutable reference to the current read/write address.
		address_type& address();
		// Returns a immutable reference to the current read/write address.
		const address_type& address() const;
		// Resets the EEPROM read/write address to zero.
		void reset();

	private:
		// Reads an object of type T from the EEPROM.
		template<class T>
		std::size_t read(address_type, T&);
		// Reads a NULL-terminated string from the EEPROM.
		std::size_t read(address_type, char*);
		// Reads a NULL-terminated string from the EEPROM.
		std::size_t read(address_type, unsigned char*);
		// Reads a NULL-terminated string from the EEPROM.
		std::size_t read(address_type, signed char*);
		// Reads an object of type String from the EEPROM.
		std::size_t read(address_type, String&);
		// Writes an object of type T to the EEPROM.
		template<class T>
		std::size_t write(address_type, const T&);
		// Writes a NULL-terminated string to the EEPROM.
		std::size_t write(address_type, const char*);
		// Writes a NULL-terminated string to the EEPROM.
		std::size_t write(address_type, const unsigned char*);
		// Writes a NULL-terminated string to the EEPROM.
		std::size_t write(address_type, const signed char*);
		// Writes an object of type String to the EEPROM.
		std::size_t write(address_type, const String&);
		// I/O manipulator "update" handler.
		std::size_t write(address_type, update);
		//I/O manipulator "noupdate" handler.
		std::size_t write(address_type, noupdate);

	private:
		address_type	address_;	// The current EEPROM read/write address.
		bool			update_;	// Flag indicating whether to put or update data on writes.
	};

#pragma region Implementation

	EEStream::EEStream() : 
		address_(), update_() 
	{
	
	}

	typename EEStream::address_type& EEStream::address()
	{
		return address_;
	}

	const typename EEStream::address_type& EEStream::address() const 
	{
		return address_;
	}

	void EEStream::reset()
	{
		address_ = 0;
	}

	template<class T>
	EEStream& EEStream::operator<<(const T& t)
	{
		address_ += write(address_, t);

		return *this;
	}

	template<class T>
	EEStream& EEStream::operator>>(T& t)
	{
		address_ += read(address_, t);

		return *this;
	}

	template<class T, std::size_t N>
	EEStream& EEStream::operator<<(const T(&t)[N])
	{
		for (std::size_t i = 0; i < N; ++i)
			address_ += write(address_, t[i]);

		return *this;
	}

	template<class T, std::size_t N>
	EEStream& EEStream::operator>>(T(&t)[N])
	{
		for (std::size_t i = 0; i < N; ++i)
			address_ += read(address_, t[i]);

		return *this;
	}
		
	template<class T>
	std::size_t EEStream::read(address_type address, T& value)
	{
		EEPROM.get(address, value);

		return sizeof(value);
	}

	std::size_t EEStream::read(address_type address, char* value)
	{
		// C-strings are read as individual chars including the trailing NULL.

		address_type first = address;

		while ((*value++ = static_cast<char>(EEPROM.read(address++))));
		*value = '\0';

		return address - first;
	}

	std::size_t EEStream::read(address_type address, unsigned char* value)
	{
		return read(address, reinterpret_cast<char*>(value));
	}

	std::size_t EEStream::read(address_type address, signed char* value)
	{
		return read(address, reinterpret_cast<char*>(value));
	}

	std::size_t EEStream::read(address_type address, String& value)
	{
		// String objects are read as C-strings.

		address_type first = address;
		char c = '\0';

		while ((c = EEPROM.read(address++)))
			value += c;

		return address - first;
	}

	template<class T>
	std::size_t EEStream::write(address_type address, const T& value)
	{
		// Arduino update() only works byte-at-a-time, so we use this workaround.
		// T must be equal comparable.

		if (update_)
		{
			T t = EEPROM.get(address, t);

			if(!(t == value))
				(void)EEPROM.put(address, value);
		}
		else 
			(void)EEPROM.put(address, value);

		return sizeof(value);
	}

	std::size_t EEStream::write(address_type address, const char* value)
	{
		// C-strings are written as individual chars including the trailing NULL.

		address_type first = address;

		while (*value)
			(update_) ? EEPROM.update(address++, *value++) : EEPROM.write(address++, *value++);
		(update_) ? EEPROM.update(address++, '\0') : EEPROM.write(address++, '\0');

		return address - first;
	}


	std::size_t EEStream::write(address_type address, const unsigned char* value)
	{
		return write(address, reinterpret_cast<const char*>(value));
	}

	std::size_t EEStream::write(address_type address, const signed char* value)
	{
		return write(address, reinterpret_cast<const char*>(value));
	}

	std::size_t EEStream::write(address_type address, const String& value)
	{
		// String objects are written as C-strings.

		return write(address, value.c_str());
	}

	std::size_t EEStream::write(address_type address, update)
	{
		update_ = true;

		return 0;
	}

	std::size_t EEStream::write(address_type address, noupdate)
	{
		update_ = false;

		return 0;
	}

#pragma endregion
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES

#endif // !defined __PG_EESTREAM_H
