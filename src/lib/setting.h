/*
 *	This file defines a type for storing a program setting and displayable 
 *	value as a pair.
 *
 *	***************************************************************************
 *
 *	File: setting.h
 *	Date: November 11, 2021
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

#if !defined __PG_SETTING_H 
# define __PG_SETTING_H 20211110L

#include <utility>	// std::pair

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	template<class T, class DT>
	struct setting
	{
		using value_type = T;
		using display_type = DT;
		using setting_type = std::pair<value_type, display_type>;

		setting_type setting_;

		value_type& value() { return setting_.first; }
		const value_type& value() const { return setting_.first; }
		display_type& display_value() { return setting_.second; }
		const display_type& display_value() const { return setting_.second; }
		bool operator==(const setting& other) { return setting_ == other.setting_; }
		bool operator!=(const setting& other) { return !(*this == other); }
	};
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_SETTING_H 