/*
 *	This file defines an abstract interface class and one concrete type for 
 *	implementing asynchronous, event-driven objects.
 *
 *	***************************************************************************
 *
 *	File: iclockable.h
 *	Date: April 9, 2021
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
 *	**************************************************************************
 *
 *	Description:
 * 
 *	The `iclockable' type is an abstract interface class with one purely 
 *	virtual method `clock()' which must be implemented by derived types. It 
 *	is useful when several tasks, encapsulated in `iclockable' objects, need 
 *	be executed asynchronously by the client, such as in multitasking or 
 *	time-sensitive applications. 
 * 
 *	The `ClockCommand' type is a concrete type derived from `icommand' (see 
 *	<icommand.h>) that takes a reference to an `iclockable' object and, when 
 *	executed, calls its `clock()' method. It's provided because it is often 
 *	convenient to implement scheduled tasks as `iclockable' objects, and then 
 *	create a collection of `ClockCommands' to execute them at specified 
 *	intervals. Concrete `iclockable' objects should hide their clock() methods 
 *	by declaring them private thus making them accessible only through the 
 *	interface.
 * 
 *****************************************************************************/

#if !defined __PG_ICLOCKABLE_H 
# define __PG_ICLOCKABLE_H 20210409L

# include "cassert"
# include "../interfaces/icommand.h"	// `icommand' interface.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// Abstract interface class for clockable objects.
	struct iclockable
	{
		virtual void clock() = 0;

		virtual ~iclockable() = default;
	};

	// Command to call `clock()' method of iclockable objects.
	class ClockCommand : public icommand
	{
	public:
		explicit ClockCommand(iclockable* receiver) : receiver_(receiver) { assert(receiver); }

	private:
		void execute() override { receiver_->clock(); } // Only accessible through the interface.

	private:
		iclockable* receiver_;
	};

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_ICLOCKABLE_H 