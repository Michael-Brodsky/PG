/*
 *	This files defines a class that asynchronously executes function calls in  
 *	response to command strings received over the serial port.
 *
 *	***************************************************************************
 *
 *	File: RemoteControl.h
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

#if !defined __PG_REMOTECONTROL_H 
# define __PG_REMOTECONTROL_H 20211014L

# include "cstdio"					// std::sprintf
# include "cstring"					// C string cmp functions.
# include "cstdlib"					// C string convert functions.
# include "array"					// ArrayWrapper type.
# include "utility"					// std::tuple & std::apply
# include "interfaces/icomponent.h"	// icomponent interface.
# include "interfaces/iclockable.h"	// iclockable interface.
# include "utilities/Values.h"		// KeyValue type.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	void convertTo(const char* str, const char*& arg)
	{
		arg = str;
	}

	void convertTo(const char* str, char& arg)
	{
		arg = str[0];
	}

	void convertTo(const char* str, unsigned char& arg)
	{
		convertTo(str, reinterpret_cast<char&>(arg));
	}

	void convertTo(const char* str, int& arg)
	{
		arg = std::atoi(str);
	}

	void convertTo(const char* str, bool& arg)
	{
		arg = static_cast<bool>(std::atoi(str));
	}

	void convertTo(const char* str, short& arg)
	{
		convertTo(str, reinterpret_cast<int&>(arg));
	}

	void convertTo(const char* str, unsigned short& arg)
	{
		convertTo(str, reinterpret_cast<int&>(arg));
	}

	void convertTo(const char* str, unsigned int& arg)
	{
		convertTo(str, reinterpret_cast<int&>(arg));
	}

	void convertTo(const char* str, long& arg)
	{
		arg = std::atol(str);
	}

	void convertTo(const char* str, unsigned long& arg)
	{
		convertTo(str, reinterpret_cast<long&>(arg));
	}

	void convertTo(const char* str, double& arg)
	{
		arg = std::atof(str);
	}

	void convertTo(const char* str, float& arg)
	{
		convertTo(str, reinterpret_cast<double&>(arg));
	}
	
	void convertTo(const char* str, long double& arg)
	{
		convertTo(str, reinterpret_cast<double&>(arg));
	}

	template<std::size_t I = 0, class...Ts>
	inline typename std::enable_if<I == sizeof...(Ts), std::size_t>::type
		getArgs(const char* delim, std::tuple<Ts...>& t)
	{
		return I;
	}

	template<std::size_t I = 0, class...Ts>
	inline typename std::enable_if < I < sizeof...(Ts), std::size_t>::type
		getArgs(const char* delim, std::tuple<Ts...>& t)
	{
		char* tok = nullptr;
		std::size_t i = 0;

		if ((tok = std::strtok(nullptr, delim)))
		{
			convertTo(tok, std::get<I>(t));
			i = getArgs<I + 1, Ts...>(delim, t);
		}

		return i; // Returns sizeof...(Ts) if no. of tokens matches sizeof...(Ts) else returns 0.
	}

	template<class...Ts>
	std::size_t parseMessage(char* msg, std::tuple<Ts...>& args, const char* delim1 = "=", const char* delim2 = ",")
	{
		char* tok = nullptr;
		std::size_t n = 0;

		if ((tok = std::strtok(msg, delim1)))
			n = getArgs(delim2, args);

		return n;
	}

	// Type that reads, executes and writes remote commands to and from the serial port.
	template<std::size_t N>
	class RemoteControl : public iclockable, public icomponent 
	{
	private:
		// Remote command interface type.
		struct ICommand
		{
			virtual ~ICommand() = default;

			virtual bool execute(char*) = 0;
		};

	public:
		// Remote command type taking one or more arguments.
		template<class Ret, class...Args>
		class Command : public ICommand
		{
		public:
			using delegate_type = typename callback<Ret, void, Args...>::type;
			using args_type = std::tuple<Args...>;
			using command_type = KeyValue<const char*, delegate_type>;

		public:
			Command(const char* key, delegate_type del) : command_(key, del) {}

		public:
			bool execute(char* str) override
			{
				bool result = *this == str;

				if (result)
				{
					parseMessage(str, args());
					std::apply(delegate(), args());
				}

				return result;
			}
			const char* key() const { return command_.key(); }
			const delegate_type& delegate() const { return command_.value(); }
			const args_type& args() const { return args_; }

		public:
			bool operator==(const char* str) const { return !std::strncmp(key(), str, std::strlen(key())); }
			bool operator!=(const char* str) const { return !(*this == str); }
			bool operator==(const Command& other) const { return !std::strncmp(command_.key(), other.command_.key(), std::strlen(command_.key())); }
			bool operator!=(const Command& other) const { return !(*this == other); }

		private:
			args_type& args() { return args_; }

		private:
			command_type	command_;
			args_type		args_;
		};

		template<class Ret>
		class Command<Ret, void> : public ICommand
		{
		public:
			using delegate_type = typename callback<Ret>::type;
			using command_type = KeyValue<const char*, delegate_type>;

		public:
			Command(const char* key, delegate_type del) : command_(key, del) {}

		public:
			bool execute(char* str) override
			{
				bool result = *this == str;

				if (result)
					(*delegate())();

				return result;
			}
			const char* key() const { return command_.key(); }
			const delegate_type& delegate() const { return command_.value(); }

		public:
			bool operator==(const char* str) const { return !std::strncmp(key(), str, std::strlen(key())); }
			bool operator!=(const char* str) const { return !(*this == str); }
			bool operator==(const Command& other) const { return !std::strncmp(command_.key(), other.command_.key(), std::strlen(command_.key())); }
			bool operator!=(const Command& other) const { return !(*this == other); }

		private:
			command_type command_;
		};

		using command_type = ICommand;
		using container_type = std::ArrayWrapper<command_type*>;

		static const char DefaultEndOfTextChar = '\n';

	public:
		template<std::size_t Size>
		RemoteControl(ICommand* (&)[Size], char = DefaultEndOfTextChar, bool = false);
		RemoteControl(ICommand* [], std::size_t, char = DefaultEndOfTextChar, bool = false);
		explicit RemoteControl(ICommand** = nullptr, ICommand** = nullptr, char = DefaultEndOfTextChar, bool = false);
		RemoteControl(std::initializer_list<ICommand*>, char = DefaultEndOfTextChar, bool = false);

	public:
		// Sets/clears the echo flag.
		void		echo(bool);
		// Returns the current state of the echo flag.
		bool		echo() const;
		// Returns a mutable reference to the end of text character.
		char&		eot();
		// Returns an immutable reference to the end of text character.
		const char&	eot() const;
		// Returns a mutable pointer to the input buffer.
		char*		buf();
		// Returns an immutable pointer to the input buffer.
		const char*	buf() const;
		// Polls the input buffer for matching keys.
		void		poll();
		// Returns the message buffer size at compile time.
		static constexpr std::size_t size() { return N; }
		// Sends a formatted message to the serial port.
		template<class ...Args>
		static void sendMessage(char*, const char*, Args...);

	private:
		void clock() override;

	private:
		char			buf_[N];	// Input buffer.
		char*			data_;		// Input buffer iterator.
		char			eot_;		// The current end of text character.
		container_type	commands_;	// The current collection of remote commands.
		bool			echo_;		// Flag indicating whether to echo matched command strings to the serial port.
	};

	template<std::size_t N>
	template<std::size_t Size>
	RemoteControl<N>::RemoteControl(ICommand* (&commands)[Size], char eot, bool echo) :
		buf_(), data_(buf_), commands_(commands), eot_(eot), echo_(echo)
	{
		
	}

	template<std::size_t N>
	RemoteControl<N>::RemoteControl(ICommand* commands[], std::size_t sz, char eot, bool echo) :
		buf_(), data_(buf_), commands_(commands, sz), eot_(eot), echo_(echo)
	{

	}

	template<std::size_t N>
	RemoteControl<N>::RemoteControl(ICommand** first, ICommand** last, char eot, bool echo) :
		buf_(), data_(buf_), commands_(first, last), eot_(eot), echo_(echo)
	{
		
	}

	template<std::size_t N>
	RemoteControl<N>::RemoteControl(std::initializer_list<ICommand*> il, char eot, bool echo) :
		buf_(), data_(buf_), commands_(const_cast<ICommand**>(il.begin()), il.size()), 
		eot_(eot), echo_(echo)
	{

	}

	template<std::size_t N>
	void RemoteControl<N>::echo(bool value)
	{
		echo_ = value;
	}

	template<std::size_t N>
	bool RemoteControl<N>::echo() const
	{
		return echo_;
	}

	template<std::size_t N>
	char& RemoteControl<N>::eot() 
	{
		return eot_;
	}

	template<std::size_t N>
	const char& RemoteControl<N>::eot() const
	{
		return eot_;
	}

	template<std::size_t N>
	char* RemoteControl<N>::buf()
	{
		return buf_;
	}

	template<std::size_t N>
	const char* RemoteControl<N>::buf() const
	{
		return buf_;
	}

	template<std::size_t N>
	void RemoteControl<N>::poll()
	{
		{
			data_ += Serial.readBytes(data_, std::distance(data_, std::end(buf_)));
			if (*(data_ - 1) == eot() || data_ == std::end(buf_))
			{
				*(--data_) = '\0';
				for (auto i : commands_)
				{
					if (i->execute(buf()))
					{
						if (echo_)
							Serial.println(buf());
						break;
					}
				}
				data_ = std::begin(buf_);
			}
		}
	}

	template<std::size_t N>
	void RemoteControl<N>::clock()
	{
		poll();
	}

	template<std::size_t N>
	template<class ...Args>
	void RemoteControl<N>::sendMessage(char* buf, const char* fmt, Args... args)
	{
		std::sprintf(buf, fmt, args...);
		Serial.println(buf);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_REMOTECONTROL_H 
