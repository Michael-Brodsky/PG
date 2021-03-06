/*
 *	This file defines a class that interprets messages and generates and   
 *	executes command objects.
 *
 *	***************************************************************************
 *
 *	File: Interpreter.h
 *	Date: May 25, 2022
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

#if !defined __PG_INTERPRETER_H
# define __PG_INTERPRETER_H 20220525L

# include <cstdlib>					// atoi(), atol(), atof()
# include <array>					// Fixed-size sequence containers.
# include <tuple>					// Command args storage class.
# include <lib/strtok.h>			// Non-mangling strtok().
# include <lib/imath.h>				// isignof()
# include <interfaces/icommand.h>	// Command pattern interface.

namespace pg
{
	// Binary search algorithm, returns iterator to matching object or nullptr if not found.
	template<class RandomIt, class T, class BinaryPredicate>
	RandomIt bsearch(RandomIt first, RandomIt last, T value, BinaryPredicate pred)
	{
		RandomIt result = nullptr;
		bool found = false;
		RandomIt middle;

		while (!found && first < last)
		{
			middle = first + std::distance(first, last) / 2;
			switch (isignof(pred(*middle, value)))	// pred must return <, = and > results, constrain result in [-1,1].
			{
			case 0:
				result = middle;
				found = true;
				break;
			case +1:
				last = middle;
				break;
			case -1:
				first = middle + 1;
				break;
			default:
				return nullptr; // Invalid result from pred().
				break;
			}
		}

		return result;
	}

	namespace details
	{
		void getArg(const char* tok, char*& arg)
		{
			arg = const_cast<char*>(tok);
		}

		void getArg(const char* tok, const char*& arg)
		{
			arg = tok;
		}

		void getArg(const char* tok, bool& arg)
		{
			arg = static_cast<bool>(std::atoi(tok));
		}

		void getArg(const char* tok, char& arg)
		{
			arg = static_cast<char>(std::atoi(tok));
		}

		void getArg(const char* tok, unsigned char& arg)
		{
			arg = static_cast<unsigned char>(std::atoi(tok));
		}

		void getArg(const char* tok, signed char& arg)
		{
			arg = static_cast<signed char>(std::atoi(tok));
		}

		void getArg(const char* tok, short& arg)
		{
			arg = static_cast<short>(std::atoi(tok));
		}

		void getArg(const char* tok, unsigned short& arg)
		{
			arg = static_cast<unsigned short>(std::atoi(tok));
		}

		void getArg(const char* tok, int& arg)
		{
			arg = std::atoi(tok);
		}

		void getArg(const char* tok, unsigned int& arg)
		{
			arg = static_cast<unsigned int>(std::atoi(tok));
		}

		void getArg(const char* tok, long& arg)
		{
			arg = std::atol(tok);
		}

		void getArg(const char* tok, unsigned long& arg)
		{
			arg = static_cast<unsigned long>(std::atol(tok));
		}

		void getArg(const char* tok, long long& arg)
		{
			arg = static_cast<long long>(std::atol(tok));
		}

		void getArg(const char* tok, unsigned long long& arg)
		{
			arg = static_cast<unsigned long long>(std::atol(tok));
		}

		void getArg(const char* tok, float& arg)
		{
			arg = static_cast<float>(std::atof(tok));
		}

		void getArg(const char* tok, double& arg)
		{
			arg = std::atof(tok);
		}

		void getArg(const char* tok, long double& arg)
		{
			arg = static_cast<long double>(std::atof(tok));
		}

		template<std::size_t I = 0, class...Ts>
		typename std::enable_if<I == sizeof...(Ts), uint8_t>::type
			getArgs(const char* delim, std::tuple<Ts...>& args)
		{
			return I;	// End case, all args found.
		}

		template<std::size_t I = 0, class...Ts>
		typename std::enable_if <I < sizeof...(Ts), uint8_t>::type
			getArgs(const char* delim, std::tuple<Ts...>& args)
		{
			uint8_t i = 0;	// Prob should be i = I
			char* tok = std::strtok(nullptr, delim);

			if (tok)
			{
				getArg(tok, std::get<I>(args));
				i = getArgs<I + 1, Ts...>(delim, args);	// Recurse until all args found or no more to parse.
			}

			return i;
		}
	} // namespace details

	// Type that interprets string messages and executes matching command objects.
	class Interpreter
	{
	public:
		class CommandBase : public icommand
		{
		public:
			using key_type = char const*;
		public:
			CommandBase(const key_type& key) : key_(key) {}
		public:
			bool operator==(const key_type& key) { return std::strcmp(key_, key) == 0; }
			bool operator!=(const key_type& key) { return !(*this == key); }
			bool operator<(const key_type& key) { return std::strcmp(key_, key) < 0; }
			bool operator>(const key_type& key) { return std::strcmp(key_, key) > 0; }
			bool operator>=(const key_type& key) { return !(*this < key); }
			bool operator<=(const key_type& key) { return !(*this > key); }
			bool operator==(const CommandBase& other) { return *this == other.key_; }
			bool operator!=(const CommandBase& other) { return *this != other.key_; }
			bool operator<(const CommandBase& other) { return *this < other.key_; }
			bool operator>(const CommandBase& other) { return *this > other.key_; }
			bool operator>=(const CommandBase& other) { return *this >= other.key_; }
			bool operator<=(const CommandBase& other) { return *this <= other.key_; }
		public:
			virtual bool assemble(char*) = 0;
			const key_type& key() { return key_; }
		private:
			key_type key_;
		};

		template <class Ret, class Obj = void, class...Args>
		class Command : public CommandBase
		{
		public:
			using key_type = typename CommandBase::key_type;
			using object_type = Obj;
			using delegate_type = typename callback<Ret, Obj, Args...>::type;
			using args_type = std::tuple<Args...>;
			using return_type = Ret;
		public:
			Command(const key_type& key, object_type& obj, delegate_type del) :
				CommandBase(key), obj_(obj), del_(del), args_() {}
		public:
			void execute() override { std::experimental::apply(&obj_, del_, args_); }
		private:
			bool assemble(char* line) override { return parse(line, args_) == args_.size(); }
		private:
			object_type&	obj_;
			delegate_type	del_;
			args_type		args_;
		};

		template <class Ret, class...Args>
		class Command<Ret, void, Args...> : public CommandBase
		{
		public:
			using key_type = typename CommandBase::key_type;
			using object_type = void;
			using delegate_type = typename callback<Ret, void, Args...>::type;
			using args_type = std::tuple<Args...>;
			using return_type = Ret;
		public:
			Command(const key_type& key, delegate_type del) :
				CommandBase(key), del_(del), args_() {}
		public:
			void execute() override { std::experimental::apply(del_, args_); }
		private:
			bool assemble(char* line) override { return parse(line, args_) == args_.size(); }
		private:
			delegate_type	del_;
			args_type		args_;
		};

		template <class Ret, class Obj>
		class Command<Ret, Obj, void> : public CommandBase
		{
		public:
			using key_type = typename CommandBase::key_type;
			using object_type = Obj;
			using delegate_type = typename callback<Ret, Obj>::type;
			using args_type = void;
			using return_type = Ret;
		public:
			Command(const key_type& key, object_type& obj, delegate_type del) :
				CommandBase(key), obj_(obj), del_(del) {}
		public:
			void execute() override { (obj_.*del_)(); }
		private:
			bool assemble(char* line) override { return true; }
		private:
			object_type&	obj_;
			delegate_type	del_;
		};

		template <class Ret>
		class Command<Ret, void, void> : public CommandBase
		{
		public:
			using key_type = typename CommandBase::key_type;
			using object_type = void;
			using delegate_type = typename callback<Ret>::type;
			using args_type = void;
			using return_type = Ret;
		public:
			Command(const key_type& key, delegate_type del) :
				CommandBase(key), del_(del) {}
		public:
			void execute() override { (*del_)(); }
		private:
			bool assemble(char* line) override { return true; }
		private:
			delegate_type	del_;
		};

		using size_type = uint8_t;

		static constexpr size_type MsgSizeMax = 64;
		static constexpr const char* CmdDelimChars = "= ";
		static constexpr const char* ArgDelimChars = ",";

	public:
		Interpreter();

	public:
		bool execute(CommandBase**, CommandBase**, const char*);
		icommand* interpret(CommandBase**, CommandBase**, const char*);
		template<class...Ts>
		static size_type parse(char*, std::tuple<Ts...>&);

	private:
		char buf_[MsgSizeMax];		// Temporary message buffer.
	};

	Interpreter::Interpreter() :
		buf_{}
	{

	}

	icommand* Interpreter::interpret(CommandBase** first, CommandBase** last, const char* line)
	{
		icommand* result = nullptr;
		char* key = nullptr;

		if (line)
		{
			// Make a copy of the line and look for a command key.
			if ((key = pg::strtok(buf_, line, CmdDelimChars, sizeof(buf_)))) 
			{
				CommandBase** it = bsearch(first, last, key, 
					[](CommandBase* c, CommandBase::key_type k) { return std::strcmp(c->key(), k); });

				// If the line key matches one of the our command keys, ...
				if (it)
				{
					CommandBase* cmd = *it;

					// ... make a copy of the line without the key.
					(void)pg::strtok(buf_, line, CmdDelimChars, sizeof(buf_));
					// If the line contains the number of expected command arguments, ...
					if (cmd->assemble(buf_))
					{
						// ... return the matched command.
						result = cmd;
					}
				}
			}
		}

		return result;
	}

	bool Interpreter::execute(CommandBase** first, CommandBase** last, const char* line)
	{
		icommand* cmd = interpret(first, last, line);
		bool result = cmd != nullptr;

		if (result)
			cmd->execute();

		return result;
	}

	template<class...Ts>
	Interpreter::size_type Interpreter::parse(char* line, std::tuple<Ts...>& args)
	{
		size_type n = line ? details::getArgs(ArgDelimChars, args) : 0;

		return n;
	}

	// Comparator for sorting arrays of Interpreter command objects.
	bool cbcomp(Interpreter::CommandBase* cmd1, Interpreter::CommandBase* cmd2) { return *cmd1 >= *cmd2; } 

} // namespace pg

#endif // !defined __PG_INTERPRETER_H
