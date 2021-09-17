/*
 *	This file defines several template typedefs for implementing callbacks. 
 *
 *	***************************************************************************
 *
 *	File: callback.h
 *	Date: July 17, 2021
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
 *	This file includes several templated typedefs for defining callback 
 *	function signatures with template parameters. The templates support both 
 *	member method or free-standing function callbacks, with or without 
 *	arguments.
 *
 *	Examples:
 *
 *		struct Object { int doSomething(int i) { ... } };
 *		void doSomething() { ... }
 *		Object obj;
 *
 *		...
 *
 *		callback<int, Object, int>::type cb1 = &Object::doSomething;
 *		callback<void>::type cb2 = &doSomething;
 *
 *		(obj.*cb1)(42);	// Calls obj.doSomething(42);
 *		cb2();			// Calls doSomething();
 * 
 *	***************************************************************************/

#if !defined __PG_CALLBACK_H
# define __PG_CALLBACK_H 20210717L

# include "pg.h"	// Pg environment.

# if defined __PG_HAS_NAMESPACES

namespace pg
{

# if defined __PG_HAS_VARIADIC_TEMPLATES	// Requires C++11 or later.

	template <class Ret, class Obj = void, class ... Args>
	struct callback
	{
		typedef Ret(Obj::* type)(Args ...);
	};

	template <class Ret, class ... Args>
	struct callback<Ret, void, Args ...>
	{
		typedef Ret(*type)(Args ...);
	};

# else // !defined __PG_HAS_VARIADIC_TEMPLATES

	template <class Ret, class Obj = void, class Arg = void>
	struct callback
	{
		typedef Ret(Obj::* type)(Arg);
	};

	template <class Ret, class Arg>
	struct callback<Ret, void, Arg>
	{
		typedef Ret(*type)(Arg);
	};

# endif // defined __PG_HAS_VARIADIC_TEMPLATES 

	template <class Ret, class Obj>
	struct callback<Ret, Obj, void>
	{
		typedef Ret(Obj::* type)();
	};

	template <class Ret>
	struct callback<Ret, void, void>
	{
		typedef Ret(*type)();
	};

	// Constant indicating no callback.
	static const nullptr_t NoCallback = nullptr;

} // namespace pg

# else // !defined __HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __HAS_NAMESPACES

#endif // !defined __PG_CALLBACK_H

