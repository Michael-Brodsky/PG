/*
 *	This file defines a class that manages a set of executable instructions.
 *
 *	***************************************************************************
 *
 *	File: Program.h
 *	Date: June 3, 2022
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
 *	along with this file. If fnot, see <http://www.gnu.org/licenses/>.
 *
 *	**************************************************************************/

#if !defined __PG_PROGRAM_H
# define __PG_PROGRAM_H 20220603L

# include <cstdint>
# include <cstring>
# include <cstdlib>
# include <stack>
# include <utilities/Timer.h>

namespace pg
{
	// Type that manages a set of executable instructions.
	class Program
	{
	public:
		// Enumerates valid program actions.
		enum Action	: uint8_t 
		{
			End = 0,	// End loading program.
			Begin = 1,	// Begin loading new program.
			Run = 2,	// Run current program.
			Halt = 3,	// Halt current program.
			Reset = 4,	// Reset current program to first instruction.
			Size = 5,	// Current program size in characters.
			Active = 6, // Current program status.
			Verify = 7,	// Verifies the current program.
			List = 8,	// Lists the current program text.
		};
		using size_type = uint16_t;	// Type that can hold the size of any program object.
		using value_type = int32_t;	// Type that can hold the value of any program object. 
		using timer_type = Timer<std::chrono::milliseconds>; // Program sleep timer type.

		static constexpr size_type CharsMax = 1024;	// Maximum size of program text in characters.
		static constexpr size_type StackSize = 32;	// Maximum size of program stack.

		using stack_type = std::stack<value_type, StackSize>; // Program stack.

	public:
		Program();

	public: /* Program control methods. */
		bool active() const;					// Checks whether the program is currently running.
		void begin();							// Begins loading a new program.
		void end();								// Ends loading a new program.
		void halt();							// Stops a running program.
		void instruction(const char*);			// Adds an instruction to a new program.
		const char* instruction();				// Returns the current program instruction and advances to the next one.
		bool loading() const;					// Checks whether a new program is currently loading.
		void reset();							// Resets the program to the first instruction.		
		void run();								// Marks the current program as active.
		size_type size() const;					// Returns the current program size in characters.
		void sleep(std::time_t);				// Puts the program execution to sleep for a given interval.
		const char* text() const;				// Returns a pointer to the beginning of the program text.

	public:	/* Program execution methods. */
		void add(const char*, const char*);
		void call(size_type);
		void compare(value_type, value_type);
		void compare(const char*, value_type);
		void compare(value_type, const char*);
		void compare(const char*, const char*);	
		void decrement(const char*);
		void divide(const char*, const char*);
		void increment(const char*);
		void jump(size_type);
		void jumpEqual(size_type);
		void jumpGreater(size_type);
		void jumpGreaterEqual(size_type);
		void jumpLess(size_type);
		void jumpLessEqual(size_type);
		void jumpNotEqual(size_type);
		void jumpNotSign(size_type);
		void jumpNotZero(size_type);
		void jumpSign(size_type);
		void jumpZero(size_type);
		void logicalAnd(const char*, const char*);
		void logicalOr(const char*, const char*);
		void logicalTest(const char*, const char*);
		void logicalXor(const char*, const char*);
		void logicalNot(const char*);
		void loop(size_type);
		void modulo(const char*, const char*);
		void move(const char*, const char*);
		void move(const char*, value_type);
		void multiply(const char*, const char*);
		void negate(const char*);
		void pop(const char*);
		void push(const char*);
		value_type regValue(const char*);
		void ret();
		void ret(const char*);
		void subtract(const char*, const char*);

	private:
		value_type argValue(const char* arg);
		size_type next() const;		
		value_type* regAddress(const char*);
		bool sleeping();			

	private:
		bool		loading_;			// Flag indicating whether a new program is loading.
		bool		active_;			// Flag indicating whether the current program is active.
		char		text_[CharsMax];	// Program text buffer.
		char*		ptr_;				// Pointer to the current program instruction.
		char*		end_;				// Pointer to one past the last program instruction.
		timer_type	sleep_;				// Program sleep timer.
		value_type	ax_;				
		value_type	bx_;
		value_type	cx_;
		value_type	dx_;
		value_type	sr_;
		stack_type	stack_;
	};

#pragma region public program control methods

	Program::Program() :
		loading_(), active_(), text_{}, ptr_(text_), end_(ptr_), sleep_(),
		ax_(), bx_(), cx_(), dx_(), sr_(), stack_() 
	{

	}

	bool Program::active() const
	{
		return active_;
	}

	void Program::begin()
	{
		if (!active_)
		{
			loading_ = true;
			ptr_ = end_ = text_;
			*ptr_ = '\0';
		}
	}

	void Program::end()
	{
		if (loading_)
		{
			loading_ = false;
			*ptr_ = '\0';	// Mark one past the last instruction.
			ptr_ = text_;
		}
	}

	void Program::halt()
	{
		active_ = false;
	}

	void Program::instruction(const char* line)
	{
		if (loading_)
		{
			std::strncpy(ptr_, line, CharsMax - size());
			end_ += std::strlen(++end_);
			ptr_ = end_ + sizeof(char);
		}
	}

	const char* Program::instruction()
	{
		const char* cmd = ((active_ &= ptr_ < end_) && !sleeping()) ? ptr_ : nullptr;

		if (cmd)
			ptr_ += next();

		return cmd;
	}

	bool Program::loading() const
	{
		return loading_;
	}
	
	Program::value_type Program::regValue(const char* arg)
	{
		value_type* reg = regAddress(arg);

		return reg ? *reg : value_type();
	}

	void Program::reset()
	{
		if (!loading_)
			ptr_ = text_;
	}

	void Program::run()
	{
		active_ = !(loading_ || size() == 0);
	}

	typename Program::size_type Program::size() const
	{
		return std::distance(const_cast<char* const>(text_), end_);
	}

	void Program::sleep(std::time_t duration)
	{
		sleep_.interval(std::chrono::milliseconds(duration));
		sleep_.start();
	}

	const char* Program::text() const
	{
		return text_;
	}

#pragma endregion
#pragma region private

	Program::value_type Program::argValue(const char* arg)
	{
		value_type* reg = regAddress(arg);

		return reg ? *reg : std::atol(arg);
	}

	Program::size_type Program::next() const
	{
		return std::strlen(ptr_) + sizeof(char);
	}

	Program::value_type* Program::regAddress(const char* arg)
	{
		value_type* reg = nullptr;
		char c = 'x';

		switch (*arg)
		{
		case 'a':
			reg = &ax_;
			break;
		case 'b':
			reg = &bx_;
			break;
		case 'c':
			reg = &cx_;
			break;
		case 'd':
			reg = &dx_;
			break;
		case 'p':
			reg = reinterpret_cast<value_type*>(&ptr_);
			c = 'c';
			break;
		case 's':
			reg = &sr_;
			c = 'r';
			break;
		default:
			break;
		}

		return reg && *(arg + 1) == c && *(arg + 2) == '\0' ? reg : nullptr;
	}

	bool Program::sleeping()
	{
		// timer may roll over if run for long time, might want to stop when expired.
		return sleep_.active() && !sleep_.expired();
	}

#pragma endregion
#pragma region instructions

	void Program::add(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg += argValue(arg2);
	}

	void Program::call(size_type address)
	{
		push("pc");
		push("sr");
		jump(address);
	}

	void Program::compare(value_type a, value_type b)
	{
		sr_ = a - b;
	}

	void Program::compare(const char* arg, value_type v)
	{
		sr_ = argValue(arg) - v;
	}

	void Program::compare(value_type v, const char* arg)
	{
		sr_ = v - argValue(arg);
	}

	void Program::compare(const char* arg1, const char* arg2)
	{
		sr_ = argValue(arg1) - argValue(arg2);
	}

	void Program::decrement(const char* arg)
	{
		value_type* reg = regAddress(arg);

		if (reg)
			sr_ = --(*reg);
	}

	void Program::divide(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg /= argValue(arg2);
	}

	void Program::increment(const char* arg)
	{
		value_type* reg = regAddress(arg);

		if (reg)
			sr_ = ++(*reg);
	}

	void Program::jump(size_type n)
	{
		// This could be slow in large programs.
		ptr_ = text_;
		while (n--)
			ptr_ += next();
	}

	void Program::jumpEqual(size_type address)
	{
		if (sr_ == 0)
			jump(address);
	}

	void Program::jumpGreater(size_type address)
	{
		if (sr_ > 0)
			jump(address);
	}

	void Program::jumpGreaterEqual(size_type address)
	{
		if (sr_ >= 0)
			jump(address);
	}

	void Program::jumpLess(size_type address)
	{
		if (sr_ < 0)
			jump(address);
	}

	void Program::jumpLessEqual(size_type address)
	{
		if (sr_ <= 0)
			jump(address);
	}

	void Program::jumpNotEqual(size_type address)
	{
		if (sr_ != 0)
			jump(address);
	}

	void Program::jumpNotSign(size_type address)
	{
		jumpGreaterEqual(address);
	}

	void Program::jumpNotZero(size_type address)
	{
		jumpNotEqual(address);
	}

	void Program::jumpSign(size_type address)
	{
		jumpLess(address);
	}

	void Program::jumpZero(size_type address)
	{
		jumpEqual(address);
	}

	void Program::logicalAnd(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg &= argValue(arg2);
	}

	void Program::logicalOr(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg |= argValue(arg2);
	}

	void Program::logicalTest(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg & argValue(arg2);
	}

	void Program::logicalXor(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg ^= argValue(arg2);
	}

	void Program::logicalNot(const char* arg)
	{
		value_type* reg = regAddress(arg);

		if (reg)
			sr_ = *reg = ~(*reg);
	}

	void Program::loop(size_type address)
	{
		if (--cx_ > 0)
		{
			jump(address);
			//--cx_;
		}
	}

	void Program::modulo(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg %= argValue(arg2);
	}

	void Program::multiply(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg *= argValue(arg2);
	}

	void Program::move(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg = argValue(arg2);
	}

	void Program::move(const char* arg, value_type v)
	{
		value_type* reg = regAddress(arg);

		if (reg)
			sr_ = *reg = v;
	}

	void Program::negate(const char* arg)
	{
		value_type* reg = regAddress(arg);

		if (reg)
			sr_ = *reg = -(*reg);
	}

	void Program::pop(const char* r)
	{
		value_type* reg = regAddress(r);

		if (reg)
		{
			if (reg == reinterpret_cast<value_type*>(&ptr_))
				ptr_ = reinterpret_cast<char*>(stack_.top());
			else
				*reg = stack_.top();
			stack_.pop();
		}
	}

	void Program::push(const char* arg)
	{
		stack_.push(argValue(arg));
	}

	void Program::ret()
	{
		pop("sr");
		pop("pc");
	}

	void Program::ret(const char* arg)
	{
		ret();
		push(arg);
	}

	void Program::subtract(const char* arg1, const char* arg2)
	{
		value_type* reg = regAddress(arg1);

		if (reg)
			sr_ = *reg -= argValue(arg2);
	}

#pragma endregion
} // namespace pg

#endif // !defined __PG_PROGRAM_H

