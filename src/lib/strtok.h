/*
 *	This files defines a specialized version of the C Standard Library 
 *	function strtok() that operates on a copy of the source string.
 *
 *	***************************************************************************
 *
 *	File: strtok.h
 *	Date: June 5, 2021
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

#if !defined __PG_STRTOK_H
# define __PG_STRTOK_H 20220605L

# include <cstring>

namespace pg
{
	// Makes a copy of the original string and parses the copy, preserving the original.
	char* strtok(char* cpy, const char* orig, const char* delim, std::size_t len)
	{
		(void)std::strncpy(cpy, orig, len);

		return  std::strtok(cpy, delim);
	}
} // namespace pg

#endif // !defined __PG_STRTOK_H
