/*
 *	This files defines a class for implementing the C++ "Mediator" design 
 *  pattern for component types.
 *
 *	***************************************************************************
 *
 *	File: icomponent.h
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
 *	**************************************************************************/

#if !defined ICOMPONENT_H__ 
#define ICOMPONENT_H__ 20210717L

#include "imediator.h"

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Component interface class.
	struct icomponent
	{
		icomponent(imediator* mediator = nullptr) : mediator_(mediator) {}

		virtual ~icomponent() = default;

		void mediator(imediator* mediator) { mediator_ = mediator; }

	protected:
		imediator* mediator_;
	};

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES

#endif // !defined ICOMPONENT_H__ 