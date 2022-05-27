/*
 *	This file defines a class that manages a set of commands that can be 
 *	executed in sequence.
 *
 *	***************************************************************************
 *
 *	File: Program.h
 *	Date: May 22, 2022
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

#if !defined __PG_PROGRAM_H
# define __PG_PROGRAM_H 20220522L

# include "cstdint"
# include "cstring"
# include "utilities/Timer.h"

namespace pg
{
	// Type that manages and executes a set of instructions.
	class Program
	{
	public:
		// Enumerates valid program actions.
		enum Action	
		{
			End = 0,	// End loading program.
			Begin = 1,	// Begin loading new program.
			Run = 2,	// Run current program.
			Halt = 3,	// Halt current program.
			Reset = 4,	// Reset current program to first instruction.
			Size = 5,	// Program size in characters.
			Active = 6, // Current program status.
			Verify = 7	// Verifies the current program.
		};
		using size_type = uint16_t;	// Type that can hold the size of any program object.
		using timer_type = Timer<std::chrono::milliseconds>; // Program sleep timer type.

		static constexpr size_type CharsMax = 1023;	// Maximum size of program text in characters.

	public:
		Program();

	public:
		bool active() const;		// Checks whether the program is currently running.
		void begin();				// Begins loading a new program.
		void command(const char*);	// Adds a command to a new program.
		const char* command();		// Returns the current command in a running program and advances to the next one.
		void end();					// Ends loading a new program.
		void halt();				// Stops a running program.
		void jump(uint8_t);			// Unconditionally jumps to another command in the program.
		bool loading() const;		// Checks whether a new program is currently loading.
		void reset();				// Resets the program to the first command.		
		void run();					// Marks the current program as active.
		size_type size() const;		// Returns the current program size in characters.
		void sleep(std::time_t);	// Puts the program execution to sleep for a given interval.
		const char* text() const;	// Returns a pointer to the beginning of the program text.

	private:
		size_type next() const;		// Returns the offset of the next command in the program.
		bool sleeping();			// Checks whether the program is sleeping.

	private:
		bool		new_;				// Flag indicating whether a new program is loading.
		bool		active_;			// Flag indicating whether the current program is active.
		char		text_[CharsMax];	// Program text buffer.
		char*		ptr_;				// Pointer to the current program command.
		char*		end_;				// Pointer to one past the last program command.
		timer_type	sleep_;				// Program sleep timer.
	};

#pragma region public

	Program::Program() :
		new_(), active_(), text_{}, ptr_(text_), end_(ptr_), sleep_()
	{

	}

	bool Program::active() const
	{
		return active_;
	}

	void Program::command(const char* line)
	{
		if (new_)
		{
			std::strncpy(ptr_, line, CharsMax - size());
			end_ += std::strlen(++end_);
			ptr_ = end_ + sizeof(char);
		}
	}

	void Program::begin()
	{
		new_ = true;
		active_ = false;
		ptr_ = end_ = text_;
		*ptr_ = '\0';
	}

	const char* Program::command()
	{
		const char* cmd = ((active_ &= ptr_ < end_) && !sleeping()) ? ptr_ : nullptr;

		if (cmd)
			ptr_ += next();

		return cmd;
	}

	void Program::sleep(std::time_t duration)
	{
		sleep_.interval(std::chrono::milliseconds(duration));
		sleep_.start();
	}

	void Program::end()
	{
		if (new_)
		{
			new_ = active_ = false;
			*ptr_ = '\0';	// Mark one past the last instruction.
			ptr_ = text_;
		}
	}

	void Program::halt()
	{
		active_ = false;
	}

	void Program::jump(uint8_t n)
	{
		ptr_ = text_;
		while (n--)
			ptr_ += next();
	}

	bool Program::loading() const
	{
		return new_;
	}

	void Program::reset()
	{
		if (!new_)
			ptr_ = text_;
	}

	void Program::run()
	{
		active_ = !new_;
	}

	typename Program::size_type Program::size() const
	{
		return std::distance(const_cast<char* const>(text_), end_);
	}

	const char* Program::text() const
	{
		return text_;
	}

#pragma endregion
#pragma region private

	Program::size_type Program::next() const
	{
		return std::strlen(ptr_) + 1;
	}

	bool Program::sleeping()
	{
		return sleep_.active() && !sleep_.expired();
	}

#pragma endregion
} // namespace pg

#endif // !defined __PG_PROGRAM_H

