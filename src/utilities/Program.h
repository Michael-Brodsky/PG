/*
 *	This file defines a class that manages and executes a set of instructions.
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
			Status = 6, // Current program status.
			Store = 7,	// Store the current program to eeprom.
			Load = 8	// Load the current program from eeprom.
		};
		using size_type = uint8_t;	// Type that can hold the size of any program object.
		using timer_type = Timer<std::chrono::milliseconds>; // Program sleep timer type.

		static constexpr size_type CharsMax = 128;	// Maximum size of program text in characters.

	public:
		Program();

	public:
		bool active() const;
		void add(const char*);
		void begin();
		void sleep(std::time_t);
		void end();
		const char* command();
		void halt();
		void jump(uint8_t);
		bool loading() const;
		void reset();
		void run();
		size_type size() const;
		const char* text() const;

	private:
		size_type next() const;
		bool sleeping();

	private:
		bool		new_;				// Flag set when loading new program, else clear.
		bool		active_;			// Flag indicating whether the program is currently running.
		char		text_[CharsMax];	// Program text buffer.
		char*		ptr_;				// Pointer to the current program instruction.
		char*		end_;				// Pointer to after the last program instruction.
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

	void Program::add(const char* cmd)
	{
		if (new_)
		{
			std::strncpy(ptr_, cmd, CharsMax - size());
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
			*ptr_ = '\0';	// Mark after the last instruction.
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

