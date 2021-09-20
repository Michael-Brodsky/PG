/*
 *	This file defines a base class for types that need a unique identifier,
 *
 *	**************************************************************************/

/*	File: Unique.h
 *	Date: April 7, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2021 Michael Brodsky */

/*	***************************************************************************
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

#if !defined __PG_UNIQUE_H
# define __PG_UNIQUE_H 20210407L

# include "cstdint"	// `uint8_t' type.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// Base class for all types having a unique identifier.
	class Unique
	{
	public:
		using uniq_t = uint8_t;	// uniq_t type alias, max of 256 unique objects.

	protected:
		Unique();				// Not directly constructable, copyable or assignable.			
		explicit Unique(const Unique& other);
		Unique& operator=(const Unique& other);

	public:
		uniq_t id() const;		// Returns the instance's unique identifier.

	protected:
		uniq_t id_;				// This instance's unique identifier.

	private:
		static uniq_t next_;	// Holds the next unique identifier to be assigned.
	};

	Unique::uniq_t Unique::next_ = 0;	// Initialize the unique identifier to zero.

	Unique::Unique() :
		id_(next_++)
	{

	}

	Unique::Unique(const Unique& other) :
		id_(next_++)
	{

	}

	Unique& Unique::operator=(const Unique& other)
	{
		// `id_' member isn't copied, derived types retain their original unique id.
		return *this;
	}

	Unique::uniq_t Unique::id() const
	{
		return id_;
	};
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_UNIQUE_H
