/*
 *	This file defines several types for implementing the C++ "Command" design 
 *	pattern. 
 *
 *	***************************************************************************
 *
 *	File: icommand.h
 *	Date: July 17, 2021
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
 *	**************************************************************************/

#if !defined ICOMMAND_H__
# define ICOMMAND_H__ 20210717L

# include "../lib/callback.h"	// `Callback' interfaces.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Command design pattern abstract interface class.
	struct icommand
	{
		virtual ~icommand() = default;

		virtual void execute() = 0;
	};

	// Constant indicating no command specified.
	const nullptr_t NoCommand = nullptr;

	// Null command type.
	struct NullCommand : public icommand
	{
		NullCommand() = default;
		~NullCommand() = default;

		void execute() override {}
	};

	// Concrete Command type for member functions with arguments.
	template <class Ret, class Obj = void, class Arg = void>
	class Command : public icommand
	{
	public:
		// Callback type signature.
		using callback_type = typename pg::callback<Ret, Obj, Arg>::type;

	public:
		Command(Obj* receiver, callback_type callback, Arg arg) :
			icommand(), arg_(arg), receiver_(receiver), callback_(callback) {}
		~Command() {};

	public:
		// Command execute method.
		void execute() override { (void)(receiver_->*callback_)(arg_); }

	protected:
		Obj*			receiver_;	// Command receiver.
		callback_type	callback_;	// Receiver::method.
		Arg				arg_;		// Method argument.
	};

	// Concrete Command type for free-standing functions with arguments.
	template <class Ret, class Arg>
	class Command<Ret, void, Arg> : public icommand
	{
	public:
		// Callback type signature.
		using callback_type = typename pg::callback<Ret, void, Arg>::type;

	public:
		Command(callback_type callback, Arg arg) :
			icommand(), arg_(arg), callback_(callback) {}
		~Command() {}

	public:
		// Command execute method.
		void execute() override { (void)(*callback_)(arg_); }

	protected:
		callback_type	callback_;	// Command callback.
		Arg				arg_;		// Callback argument.
	};

	// Concrete Command type for member functions without arguments.
	template <class Ret, class Obj>
	class Command<Ret, Obj, void> : public icommand
	{
	public:
		// Callback type signature.
		using callback_type = typename pg::callback<Ret, Obj, void>::type;

	public:
		Command(Obj* object, callback_type callback) :
			icommand(), receiver_(object), callback_(callback) {}
		~Command() {}

	public:
		// Command execute method.
		void execute() override { (void)(receiver_->*callback_)(); }

	protected:
		Obj*			receiver_;	// Command receiver.
		callback_type	callback_;	// Receiver::method.
	};

	// Concrete Command type for free-standing functions without arguments.
	template <class Ret>
	class Command<Ret, void, void> : public icommand
	{
	public:
		// Callback type signature.
		using callback_type = typename pg::callback<Ret, void, void>::type;

	public:
		Command(callback_type callback) :
			icommand(), callback_(callback) {}
		~Command() {}

	public:
		// Command execute method.
		void execute() override { (void)(*callback_)(); }

	protected:
		callback_type callback_;	// Command callback.
	};

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES

#endif // !defined ICOMMAND_H__
