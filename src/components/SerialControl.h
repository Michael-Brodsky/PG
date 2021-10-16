/*
 *	This files defines a class that asynchronously executes Command objects 
 *	in response to key strings received by the serial port..
 *
 *	***************************************************************************
 *
 *	File: SerialControl.h
 *	Date: October 14, 2021
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

#if !defined __PG_SERIALCONTROL_H 
# define __PG_SERIALCONTROL_H 20211014L

# include "cstring"					// String functions.
# include "array"					// ArrayWrapper type.
# include "interfaces/icomponent.h"	// icomponent interface.
# include "interfaces/iclockable.h"	// iclockable & icommand interfaces.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Type that executes Command objects over the serial port.
	template<std::size_t N, class T = char>
	class SerialControl : public iclockable, public icomponent 
	{
	public:
		// Serial command type stores a key string and command object pair.
		struct Command
		{
			using value_type = T;
			using key_type = const T*;
			using program_type = icommand*;
			using command_type = std::pair<key_type, program_type>; // Key/Command pair.

			command_type command_;

			const key_type& key() const { return command_.first; }
			const program_type& program() const { return command_.second; }

			// Check if this key matches another.
			bool operator==(const Command& other) const { return !std::strncmp(key(), other.key(), std::strlen(key())); }
		};
		using value_type = typename Command::value_type;
		using command_type = typename Command::command_type;
		using container_type = std::ArrayWrapper<Command>;
		using iterator = typename container_type::iterator;
		using pointer = value_type*;

		static const value_type EndOfTextChar = '\n';	// Character indicating end of text.

	public:
		template<std::size_t Size>
		SerialControl(Command (&)[Size], bool = false);
		SerialControl(Command [], std::size_t, bool = false);
		explicit SerialControl(Command* = nullptr, Command* = nullptr, bool = false);
		SerialControl(std::initializer_list<Command*>, bool = false);

	public:
		// Sets/clears the echo flag.
		void		echo(bool);
		// Returns the current state of the echo flag.
		bool		echo() const;
		// Returns a mutable pointer to the input buffer.
		T*			buf();
		// Returns an immutable pointer to the input buffer.
		const T*	buf() const;
		// Polls the input buffer for matching keys.
		void		poll();

	private:
		void clock() override;

	private:
		value_type		buf_[N];	// Input buffer.
		pointer			data_;		// Input buffer iterator.
		container_type	commands_;	// The current collection of key/command pairs.
		iterator		current_;	// The currently matched pair, if any.
		bool			echo_;		// Flag indicating whether to echo matched keys to the serial port.
	};

	template<std::size_t N, class T>
	template<std::size_t Size>
	SerialControl<N, T>::SerialControl(Command (&commands)[Size], bool echo) :
		buf_(), data_(buf_), commands_(commands), current_(std::begin(commands_)), echo_(echo)
	{
		
	}

	template<std::size_t N, class T>
	SerialControl<N, T>::SerialControl(Command commands[], std::size_t sz, bool echo) :
		buf_(), data_(buf_), commands_(commands, sz), current_(std::begin(commands_)), echo_(echo)
	{

	}

	template<std::size_t N, class T>
	SerialControl<N, T>::SerialControl(Command* first, Command* last, bool echo) :
		buf_(), data_(buf_), commands_(first, last), current_(std::begin(commands_)), echo_(echo)
	{
		
	}

	template<std::size_t N, class T>
	SerialControl<N, T>::SerialControl(std::initializer_list<Command*> il, bool echo) :
		buf_(), data_(buf_), commands_(const_cast<Command*>(*il.begin()), il.size()), 
		current_(std::begin(commands_)), echo_(echo)
	{

	}

	template<std::size_t N, class T>
	void SerialControl<N, T>::echo(bool value)
	{
		echo_ = value;
	}

	template<std::size_t N, class T>
	bool SerialControl<N, T>::echo() const
	{
		return echo_;
	}

	template<std::size_t N, class T>
	T* SerialControl<N, T>::buf()
	{
		return buf_;
	}

	template<std::size_t N, class T>
	const T* SerialControl<N, T>::buf() const
	{
		return buf_;
	}

	template<std::size_t N, class T>
	void SerialControl<N, T>::poll()
	{
		{
			data_ += Serial.readBytes(data_, std::distance(data_, std::end(buf_)));
			if (*(data_ - 1) == EndOfTextChar || data_ == std::end(buf_))
			{
				*(--data_) = '\0';
				current_ = std::find(std::begin(commands_), std::end(commands_), Command{ {buf_,nullptr} });
				if (current_ != std::end(commands_))
				{
					current_->program()->execute();
					if (echo_)
						Serial.print(buf_);
				}
				data_ = std::begin(buf_);
			}
		}
	}

	template<std::size_t N, class T>
	void SerialControl<N, T>::clock()
	{
		poll();
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 


#endif // !defined __PG_SERIALCONTROL_H 
