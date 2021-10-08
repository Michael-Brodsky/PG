/*
 *	This files declares a timer class that can execute commands at specified 
 *	intervals.
 *
 *	***************************************************************************
 *
 *	File: CommandTimer.h
 *	Date: April 9, 2021
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
 *	**************************************************************************
 *
 *	Description:
 *
 *		The `CommandTimer' class can be used to execute `Command' objects 
 *		(see ICommand.h) at specified intervals. `CommandTimer' extends 
 *		the base class 'Timer' by adding three methods: `command()', `repeats() 
 *		and `tick()', otherwise it behaves identically except when the current  
 *		interval expires, the timer is automatically stopped or reset based on 
 *		the parameter passed to the `repeats()' method. (see <Timer.h>). The 
 *		timer is advanced by the client using the `tick()' method. Each call to 
 *		`tick()' checks time elapsed since it was last started or resumed and, 
 *		if the elapsed time is equal to or greater than the interval time, 
 *		executes the current `Command', which the client provides using the 
 *		`command()' method. The `repeats()' method determines whether the timer 
 *		is reset and continues running, or is stopped when the current interval 
 *		expires. If the current interval does not repeat, the timer must be 
 *		manually restarted. Clients can manually call the `expired()' method, 
 *		however, this is only useful if the current interval does not repeat 
 *		because otherwise any call to `expired()' always return `false'. 
 *		
 *	**************************************************************************/

#if !defined __PG_COMMANDTIMER_H
# define __PG_COMMANDTIMER_H 20210403L

# include "interfaces/icommand.h"	// `icommand' interface.
# include "Timer.h"						// `Timer' class.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// Executes commands at timed intervals.
	template<class T>
	class CommandTimer : public Timer<T>
	{
	public:
		using timer_type = Timer<T>;
		using duration = typename timer_type::duration;

	public:
		CommandTimer() = default;
		CommandTimer(duration, icommand* = nullptr, bool = false);

	public:
		// Sets the timer command.
		void		command(icommand*);
		// Returns the current timer command.
		icommand*	command() const;
		// Sets the interval repeat mode.
		void		repeats(bool);
		// Returns the current interval's repeat mode.
		bool		repeats() const;
		// Checks the current interval's elapsed time and executes a `Command' if expired. 
		void		tick();

	private:
		// Executes the current `Command' object and either resets or stops the timer.
		void		execute();

	private:
		icommand*	command_;	// The `Command' object to execute when the current interval expires. 
		bool		repeats_;	// Flag indicating whether the current interval repeats when expired.
	};

	template<class T>
	CommandTimer<T>::CommandTimer(duration intvl, icommand* cmd, bool repeats) : 
		timer_type(intvl), command_(cmd), repeats_(repeats)
	{

	}

	template<class T>
	void CommandTimer<T>::command(icommand* cmd)
	{
		command_ = cmd;
	}

	template<class T>
	icommand* CommandTimer<T>::command() const
	{
		return command_;
	}

	template<class T>
	void CommandTimer<T>::repeats(bool repeats)
	{
		repeats_ = repeats;
	}

	template<class T>
	bool CommandTimer<T>::repeats() const
	{
		return repeats_;
	}

	template<class T>
	void CommandTimer<T>::tick()
	{
		if (timer_type::expired())
			execute();
	}

	template<class T>
	void CommandTimer<T>::execute()
	{
		if (command_)
			command_->execute();
		(repeats_) ? timer_type::reset() : timer_type::stop();
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_COMMANDTIMER_H


