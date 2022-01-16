/*
 *	This files defines a class that asynchronously executes client code in
 *	response to commands received via the serial port.
 *
 *	***************************************************************************
 *
 *	File: RemoteControl.h
 *	Date: January 15, 2022
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky
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
# define __PG_REMOTECONTROL_H 20220115L

# include "cstdlib"					// atoi(), atol(), atof().
# include "tuple"					// std::tuple, std::apply
# include "array"					// std::ArrayWrapper
# include "interfaces/icomponent.h"	// icomponent interface.
# include "interfaces/iclockable.h"	// iclockable interface.
# include "utilities/Serial.h"		// pg::usart::serial

# if defined __PG_HAS_NAMESPACES

using pg::usart::serial;
using pg::usart::hardware_serial;

namespace pg
{
	namespace detail
	{
		void getArg(const char* str, const char*& arg)
		{
			arg = str;
		}

		void getArg(const char* str, bool& arg)
		{
			arg = static_cast<bool>(std::atoi(str));
		}

		void getArg(const char* str, char& arg)
		{
			arg = static_cast<char>(std::atoi(str));
		}

		void getArg(const char* str, unsigned char& arg)
		{
			arg = static_cast<unsigned char>(std::atoi(str));
		}

		void getArg(const char* str, signed char& arg)
		{
			arg = static_cast<signed char>(std::atoi(str));
		}

		void getArg(const char* str, short& arg)
		{
			arg = static_cast<short>(std::atoi(str));
		}

		void getArg(const char* str, unsigned short& arg)
		{
			arg = static_cast<unsigned short>(std::atoi(str));
		}

		void getArg(const char* str, int& arg)
		{
			arg = std::atoi(str);
		}

		void getArg(const char* str, unsigned int& arg)
		{
			arg = static_cast<unsigned int>(std::atoi(str));
		}

		void getArg(const char* str, long& arg)
		{
			arg = std::atol(str);
		}

		void getArg(const char* str, unsigned long& arg)
		{
			arg = static_cast<unsigned long>(std::atoi(str));
		}

		void getArg(const char* str, long long& arg)
		{
			arg = static_cast<long long>(std::atoi(str));
		}

		void getArg(const char* str, unsigned long long& arg)
		{
			arg = static_cast<unsigned long long>(std::atoi(str));
		}

		void getArg(const char* str, float& arg)
		{
			arg = static_cast<float>(std::atof(str));
		}

		void getArg(const char* str, double& arg)
		{
			arg = std::atof(str);
		}

		void getArg(const char* str, long double& arg)
		{
			arg = static_cast<long double>(std::atof(str));
		}

		template<std::size_t I = 0, class...Ts>
		inline typename std::enable_if<I == sizeof...(Ts), std::size_t>::type
			getCmdArgs(const char* delim, std::tuple<Ts...>& t)
		{
			return I;
		}

		template<std::size_t I = 0, class...Ts>
		inline typename std::enable_if < I < sizeof...(Ts), std::size_t>::type
			getCmdArgs(const char* delim, std::tuple<Ts...>& t)
		{
			char* tok = nullptr;
			std::size_t i = 0;

			if ((tok = std::strtok(nullptr, delim)))
			{
				detail::getArg(tok, std::get<I>(t));
				i = getCmdArgs<I + 1, Ts...>(delim, t);
			}

			return i; // Returns sizeof...(Ts) if no. of parsed tokens matches, else returns 0.
		}
	} // namespace detail

	class RemoteControl : public iclockable, public icomponent
	{
	public:
		// Remote command base type.
		struct ICommand
		{
			using key_type = char const*;

			ICommand(const key_type& key) : key_(const_cast<key_type>(key)) {}
			virtual ~ICommand() = default;

			virtual bool execute(char*) = 0;	// virtual bool execute(char*) = 0;
			key_type key() { return key_; }
			const key_type key() const { return key_; }

			friend bool operator==(const ICommand& other, const key_type& str) { return !strncmp(other.key(), str, strlen(other.key())); }
			friend bool operator!=(const ICommand& other, const key_type& str) { return !(other == str); }

		private:
			key_type key_;
		};

		// Remote command type for member methods taking arguments.
		template<class Ret, class Obj = void, class...Args>
		class Command : public ICommand
		{
		public:
			using key_type = typename ICommand::key_type;
			using object_type = Obj;
			using delegate_type = typename callback<Ret, Obj, Args...>::type;
			using args_type = std::tuple<Args...>; 

		public:
			Command(const key_type& key, object_type& object, delegate_type del) :
				ICommand(key), object_(object), delegate_(del) {}

		public:
			bool execute(char* str) override 
			{
				// To support "variadic" commands, command strings must have the 
				// number of arguments equal to the tuple size -> args_.size().

				bool result = false;

				if((result = RemoteControl::parseCommand(str, args_) == args_.size()))
					std::experimental::apply(&object_, delegate_, args_);

				return result;
				//RemoteControl::parseCommand(str, args_);
				//std::experimental::apply(&object_, delegate_, args_);
			}
			const object_type& object() const { return object_; }
			const delegate_type& delegate() const { return delegate_; }
			const args_type& args() const { return args_; }

		private:
			object_type&	object_;
			delegate_type	delegate_;
			args_type		args_;
		};

		// Remote command type for free-standing functions taking arguments.
		template<class Ret, class...Args>
		class Command<Ret, void, Args...> : public ICommand
		{
		public:
			using key_type = typename ICommand::key_type;
			using delegate_type = typename callback<Ret, void, Args...>::type;
			using args_type = std::tuple<Args...>; 

		public:
			Command(const key_type& key, delegate_type del) :
				ICommand(key), delegate_(del) {}

		public:
			bool execute(char* str) override
			{
				bool result = false;

				if((result = RemoteControl::parseCommand(str, args_) == args_.size()))
					std::experimental::apply(delegate_, args_);

				return result;
				//RemoteControl::parseCommand(str, args_);
				//std::experimental::apply(delegate_, args_);
			}
			const delegate_type& delegate() const { return delegate_; }
			const args_type& args() const { return args_; }

		private:
			delegate_type	delegate_;
			args_type		args_;
		};

		// Remote command type for member methods without arguments.
		template <class Ret, class Obj>
		class Command<Ret, Obj, void> : public ICommand
		{
		public:
			using key_type = typename ICommand::key_type;
			using object_type = Obj;
			using delegate_type = typename callback<Ret, Obj>::type;

		public:
			Command(const key_type& key, object_type& object, delegate_type del) :
				ICommand(key), delegate_(del), object_(object) {}
		public:
			bool execute(char* str) override { (object_.*delegate_)(); return true; }
			const object_type& object() const { return object_; }
			const delegate_type& delegate() const { return delegate_; }

		private:
			object_type&	object_;
			delegate_type	delegate_;
		};

		// Remote command type for free-standing functions without arguments.
		template<class Ret>
		class Command<Ret, void, void> : public ICommand
		{
		public:
			using key_type = typename ICommand::key_type;
			using delegate_type = typename callback<Ret>::type; 

		public:
			Command(key_type& key, delegate_type del) :
				ICommand(key), delegate_(del) {}

		public:
			bool execute(key_type str) override { (*delegate_)(); return true; }
			const delegate_type& delegate() const { return delegate_; }

		private:
			delegate_type delegate_;
		};

		using command_type = ICommand;
		using container_type = std::ArrayWrapper<command_type*>;

		static constexpr const char DefaultEotChar = '\n';	// Default end of transmission char.
		static constexpr const char* DfltCmdDelimStr = "=";	// Default command/arguments delimiter char.
		static constexpr const char* DfltArgDelimStr = ",";	// Default arguments list delimiter char.

		// Command strings are transmitted to the client over one of the onboard serial ports.
		// Command strings have the following format:
		// key[=arg0,arg1,...]\n
		//
		// Command objects are instantiated as follows:
		// RemoteControl::Command<Ret, Obj=void, Args...> cmd(key, [obj,] &[Obj::]function);
		// depending on their signature.
		// 
		// If a Command string key matches a Command object key, the Command object's
		// execute() method is called with the contents of the receive buffer. The 
		// object parses the buffer for tokens, converts them to a collection of typed values and calls 
		// ([obj::]*function)(args...) expanding the collection into the function arguments, if any.
		// 
		// For example:
		// 
		// void foo(int); // Client's callable function prototype.
		//
		// RemoteControl::Command<void, void, int> bar("foo", &foo); // Instantiates a command object with key "foo"
		//															 // That calls free-standing function foo() with 
		//															 // one arg of type int.
		// 
		// RemoteControl rc({&bar}, Serial);	// Instantiates a RemoteControl with one command (bar) and listens for 
		//										// Command strings using the Arduino `Serial' object.
		// 
		// Then in the `loop()' function:
		// void loop() { rc.poll(); }
		// 
		// The command string "foo=42" calls foo(42)
		// "foo=-42" calls foo(-42)
		// "bar=99" is ignored because no command with key "bar" has been defined.
		// 
		// Member method commands work the same as free-standing commands except they require an instance of the object 
		// at time of construction:
		//
		// struct Foo { void foo(int i) {} };
		// Foo f;
		// RemoteControl::Command<void, Foo, int> bar_none("bar", f, &Foo::foo);
		// RemoteControl rc({&bar_none}, Serial);
		//
		// Now the command string "bar=42" calls f.foo(42), and so on...

	public:
		template<std::size_t Size>
		RemoteControl(command_type* (&)[Size], HardwareSerial& = Serial, char = DefaultEotChar, bool = false);
		RemoteControl(command_type* [], std::size_t, HardwareSerial& = Serial, char = DefaultEotChar, bool = false);
		explicit RemoteControl(command_type** = nullptr, command_type** = nullptr, HardwareSerial& = Serial, char = DefaultEotChar, bool = false);
		RemoteControl(std::initializer_list<command_type*>, HardwareSerial& = Serial, char = DefaultEotChar, bool = false);

	public:
		void commands(command_type** first, command_type** last) { commands_ = container_type(first, last); }
		const container_type& commands() const { return commands_; }
		void eot(char);
		char eot() const;
		void echo(bool);
		bool echo() const;
		void poll();
		HardwareSerial& hardware();
		template<class...Ts>
		static std::size_t parseCommand(char*, std::tuple<Ts...>&, const char* = DfltCmdDelimStr, const char* = DfltArgDelimStr);

	private:
		void clock() override;

	private:
		HardwareSerial& hardware_;
		container_type	commands_;
		char			eot_;
		bool			echo_;
	};

	template<std::size_t Size>
	RemoteControl::RemoteControl(command_type* (&commands)[Size], HardwareSerial& hs, char eot, bool echo) :
		hardware_(hs), commands_(commands), eot_(eot), echo_(echo)
	{

	}

	RemoteControl::RemoteControl(command_type* commands[], std::size_t sz, HardwareSerial& hs, char eot, bool echo) :
		hardware_(hs), commands_(commands, sz), eot_(eot), echo_(echo)
	{

	}

	RemoteControl::RemoteControl(command_type** first, command_type** last, HardwareSerial& hs, char eot, bool echo) :
		hardware_(hs), commands_(first, last), eot_(eot), echo_(echo)
	{

	}

	RemoteControl::RemoteControl(std::initializer_list<command_type*> il, HardwareSerial& serial, char eot, bool echo) :
		hardware_(serial), commands_(const_cast<ICommand**>(il.begin()), il.size()), eot_(eot), echo_(echo) 
	{

	}

	void RemoteControl::eot(char value)
	{
		eot_ = value;
	}

	char RemoteControl::eot() const 
	{
		return eot_;
	}

	void RemoteControl::echo(bool value)
	{
		echo_ = value;
	}

	bool RemoteControl::echo() const
	{
		return echo_;
	}

	HardwareSerial& RemoteControl::hardware()
	{
		return hardware_;
	}

	void RemoteControl::poll()
	{
		char buf[hardware_serial::RxBufferSize] = { '\0' }; // Receive buffer.

		if (hardware_.available())
		{
			hardware_.readBytesUntil(eot_, buf, sizeof(buf));
			for (auto i : commands_)
			{
				if (*i == buf)
				{
					if (i->execute(buf))
					{
						if (echo_)
							hardware_.println(buf);
						break;
					}
				}
			}
		}
	}

	void RemoteControl::clock()
	{
		poll();
	}

	template<class...Ts>
	std::size_t RemoteControl::parseCommand(char* cmd, std::tuple<Ts...>& args, const char* cmd_delim, const char* args_delim)
	{
		char tmp[hardware_serial::RxBufferSize];
		char* tok = nullptr;
		std::size_t n = 0;

		std::strncpy(tmp, cmd, hardware_serial::RxBufferSize);
		if ((tok = std::strtok(tmp, cmd_delim)))
			n = detail::getCmdArgs(args_delim, args);

		return n;
	}
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_REMOTECONTROL_H 