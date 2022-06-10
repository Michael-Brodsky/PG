/*
 * This files defines an abstract interface class that facilitates "system 
 * calls" back to clients.
 *
 *	***************************************************************************
 *
 *	File: isystem.h
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
 *	along with this file. If not, see <http://www.gnu.org/licenses/>.
 *
 *	**************************************************************************/ 

#if !defined __PG_ISYSTEM_H
# define __PG_ISYSTEM_H 20220610L

#include <cstdint>	// int32_t type.

namespace pg
{
	struct isystem
	{
		using value_type = int32_t;

		virtual value_type sys_get(const char*) = 0;
	};
}

#endif 
