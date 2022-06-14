/*
 *	This file defines a class that manages a set of executable instructions.
 *
 *	***************************************************************************
 *
 *	File: Program.h
 *	Date: June 10, 2022
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
# define __PG_PROGRAM_H 20220610L

# include <cstdint>
# include <cstring>
# include <cstdlib>
# include <stack>
# include <interfaces/isystem.h>
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
		using value_type = isystem::value_type;	// Type that can hold the value of any program object. 
		using timer_type = Timer<std::chrono::milliseconds>; // Program sleep timer type.
		using key_type = const char*;

		static constexpr size_type CharsMax = 1024;	// Maximum size of program text in characters.
		static constexpr size_type StackSize = 32;	// Maximum size of program stack.

		static constexpr key_type KeyAdd = "add";				// Add two values.
		static constexpr key_type KeyCall = "call";				// Call subroutine.
		static constexpr key_type KeyCompare = "cmp";			// Compare two values.
		static constexpr key_type KeyDecrement = "dec";			// Decrement a value.
		static constexpr key_type KeyDivide = "div";			// Divide a value by another.
		static constexpr key_type KeyIncrement = "inc";			// Increment a value.
		static constexpr key_type KeyJump = "jmp";				// Unconditional jump.
		static constexpr key_type KeyJumpEqual = "je";			// Jump on equal.
		static constexpr key_type KeyJumpNotEqual = "jne";		// Jump on not equal.
		static constexpr key_type KeyJumpGreater = "jgt";		// Jump on greater than.
		static constexpr key_type KeyJumpGreaterEqual = "jge";	// Jump on greater than or equal.
		static constexpr key_type KeyJumpLess = "jlt";			// Jump on less than.
		static constexpr key_type KeyJumpLessEqual = "jle";		// Jump on less than or equal.
		static constexpr key_type KeyJumpNotSign = "jns";		// Jump on not negative.
		static constexpr key_type KeyJumpSign = "js";			// Jump on negative.
		static constexpr key_type KeyJumpNotZero = "jnz";		// Jump on not zero.
		static constexpr key_type KeyJumpZero = "jz";			// Jump on zero.
		static constexpr key_type KeyLogicalAnd = "and";		// Logical and two values.
		static constexpr key_type KeyLogicalNot = "not";		// Logical complement a value.
		static constexpr key_type KeyLogicalOr = "or";			// Logical or two values.
		static constexpr key_type KeyLogicalTest = "tst";		// Logical and two values without saving result.
		static constexpr key_type KeyLogicalXor = "xor";		// Logical exclusive or two values.
		static constexpr key_type KeyLoop = "loop";				// Loop while cx not zero.
		static constexpr key_type KeyModulo = "mod";			// Modulo operator.
		static constexpr key_type KeyMove = "mov";				// Move value to register.
		static constexpr key_type KeyMultiply = "mul";			// Multiply two values.
		static constexpr key_type KeyNegate = "neg";			// Negate a value.
		static constexpr key_type KeyPop = "pop";				// Pop from stack to register.
		static constexpr key_type KeyPush = "push";				// Push value to stack.
		static constexpr key_type KeyReturn = "ret";			// Return from subroutine call.
		static constexpr key_type KeyReturnValue = "rets";		// Return from subroutine call with value on stack.
		static constexpr key_type KeySleep = "dly";				// Sleep.
		static constexpr key_type KeySubtract = "sub";			// Subtract two values.

		static constexpr const char* const SystemCallChars = "#%+*$";

		using stack_type = std::stack<value_type, StackSize>; // Program stack.

	public:
		Program(isystem&);

	public: /* Program control methods. */
		bool active() const;					// Checks whether the program is currently running.
		void begin();							// Begins loading a new program.
		void end();								// Ends loading a new program.
		value_type get(const char*);			// Returns the value of a register.
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
		void multiply(const char*, const char*);
		void negate(const char*);
		void pop(const char*);
		void push(const char*);
		void ret();
		void ret(const char*);
		void subtract(const char*, const char*);

	private:
		value_type* getDest(const char*);
		bool isSysCall(const char*);
		void moveValue(const char*, value_type);
		size_type next() const;		
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
		isystem&	system_;
	};

#pragma region public program control methods

	Program::Program(isystem& system) :
		loading_(), active_(), text_{}, ptr_(text_), end_(ptr_), sleep_(),
		ax_(), bx_(), cx_(), dx_(), sr_(), stack_(), system_(system) 
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

	Program::value_type Program::get(const char* arg)
	{
		Program::value_type value = 0;
		value_type* dest = nullptr;

		if (isSysCall(arg))
			value = system_.sys_get(arg);
		else if ((dest = getDest(arg)))
			value = *dest;
		else
			value = std::atol(arg);

		return value;
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

	Program::value_type* Program::getDest(const char* arg)
	{
		value_type* dest = nullptr;
		char c = 'x';

		switch (*arg)
		{
		case 'a':
			dest = &ax_;
			break;
		case 'b':
			dest = &bx_;
			break;
		case 'c':
			dest = &cx_;
			break;
		case 'd':
			dest = &dx_;
			break;
		case 'p':
			dest = reinterpret_cast<value_type*>(&ptr_);
			c = 'c';
			break;
		case 's':
			dest = &sr_;
			c = 'r';
			break;
		default:
			break;
		}

		return dest && *(arg + 1) == c && *(arg + 2) == '\0' ? dest : nullptr;
	}

	bool Program::isSysCall(const char* arg)
	{
		return std::strpbrk(arg, SystemCallChars) == arg;
	}

	void Program::moveValue(const char* arg, value_type value)
	{
		value_type* dest = getDest(arg);

		if(dest)
			*dest = value;
	}

	Program::size_type Program::next() const
	{
		return std::strlen(ptr_) + sizeof(char);
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
		moveValue(arg1, (sr_ = get(arg1) + get(arg2)));
	}

	void Program::call(size_type address)
	{
		push("pc");
		push("sr");
		jump(address);
	}

	void Program::compare(const char* arg1, const char* arg2)
	{
		sr_ = get(arg1) - get(arg2);
	}

	void Program::decrement(const char* arg)
	{
		moveValue(arg, (sr_ = get(arg) - 1));
	}

	void Program::divide(const char* arg1, const char* arg2)
	{
		moveValue(arg1, (sr_ = get(arg1) / get(arg2)));
	}

	void Program::increment(const char* arg)
	{
		moveValue(arg, (sr_ = get(arg) + 1));
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
		moveValue(arg1, (sr_ = get(arg1) & get(arg2)));
	}

	void Program::logicalOr(const char* arg1, const char* arg2)
	{
		moveValue(arg1, (sr_ = get(arg1) | get(arg2)));
	}

	void Program::logicalTest(const char* arg1, const char* arg2)
	{
		sr_ = get(arg1) & get(arg2);
	}

	void Program::logicalXor(const char* arg1, const char* arg2)
	{
		moveValue(arg1, (sr_ = get(arg1) ^ get(arg2)));
	}

	void Program::logicalNot(const char* arg)
	{
		moveValue(arg, (sr_ = ~get(arg)));
	}

	void Program::loop(size_type address)
	{
		if (--cx_ > 0)
			jump(address);
	}

	void Program::modulo(const char* arg1, const char* arg2)
	{
		moveValue(arg1, (sr_ = get(arg1) % get(arg2)));
	}

	void Program::multiply(const char* arg1, const char* arg2)
	{
		moveValue(arg1, (sr_ = get(arg1) * get(arg2)));
	}

	void Program::move(const char* arg1, const char* arg2)
	{
		moveValue(arg1, get(arg2));
	}

	void Program::negate(const char* arg)
	{
		moveValue(arg, -get(arg));
	}

	void Program::pop(const char* arg)
	{
		value_type* dest = getDest(arg);

		if (dest)
		{
			if (dest == reinterpret_cast<value_type*>(&ptr_))
				ptr_ = reinterpret_cast<char*>(stack_.top());
			else
				*dest = stack_.top();
			stack_.pop();
		}
	}

	void Program::push(const char* arg)
	{
		stack_.push(get(arg));
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
		moveValue(arg1, (sr_ = get(arg1) - get(arg2)));
	}

#pragma endregion
} // namespace pg

#endif // !defined __PG_PROGRAM_H

