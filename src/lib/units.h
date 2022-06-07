/*
 *	This file defines a library of units of measure types and conversion 
 *	functions.
 *
 *	***************************************************************************
 *
 *	File: units.h
 *	Date: October 9, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky
 *
 *	***************************************************************************
 *
 *  This file is part of `Pretty Good' (Pg). `Pg' is free software:
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
 *	The measurements library is a collection of types that define common 
 *	units of measure and functions to convert one unit to another. Unit types 
 *	are defined in the pg::units namespace and use the common unit name, e.g. 
 *	kelvin, celsius, fahrenheit, etc. 
 * 
 *	Conversion functions templates have three parameters, the "from" type, 
 *	"to" type and the data type. The from and to parameters are required, the 
 *	data type can be found by the compiler using template argument deduction.
 *
 *	**************************************************************************/

#if !defined __PG_UNITS_H
# define __PG_UNITS_H 20211009L

# include <numbers>	// Numeric constants.
# include <ratio>	// std::ratio type.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	namespace units
	{
		struct celsius {};
		struct fahrenheit {};
		struct kelvin {};
		struct radians {};
		struct degrees {};


		/*
		 * Usinge these until namespace units is fully operational.
		 */

		namespace details
		{
			// Primary template for conversion implementations.
			template<class T, class FromUnits, class ToUnits>
			struct convert_impl;

			template<class T>
			struct convert_impl<T, units::celsius, units::fahrenheit>
			{
				T operator()(T value) { return value * 9 / 5 + 32; }
			};

			template<class T>
			struct convert_impl<T, units::fahrenheit, units::celsius>
			{
				T operator()(T value) { return (value - 32) * 5 / 9; }
			};

			template<class T>
			struct convert_impl<T, units::celsius, units::kelvin>
			{
				T operator()(T value) { return value + 273.15; }
			};

			template<class T>
			struct convert_impl<T, units::kelvin, units::celsius>
			{
				T operator()(T value) { return value - 273.15; }
			};

			template<class T>
			struct convert_impl<T, units::fahrenheit, units::kelvin>
			{
				T operator()(T value)
				{
					return convert_impl<T, units::celsius, units::kelvin>()(
						convert_impl<T, units::fahrenheit, units::celsius>()(value)
						);
				}
			};

			template<class T>
			struct convert_impl<T, units::kelvin, units::fahrenheit>
			{
				T operator()(T value)
				{
					return convert_impl<T, units::celsius, units::fahrenheit>()(
						convert_impl<T, units::kelvin, units::celsius>()(value)
						);
				}
			};

			template<class T>
			struct convert_impl<T, units::degrees, units::radians>
			{
				T operator()(T value) { return value / 180 * std::numbers::pi; }
			};

			template<class T>
			struct convert_impl<T, units::radians, units::degrees>
			{
				T operator()(T value) { return value / std::numbers::pi * 180; }
			};

			// lbs -> g = 453.59237

		} // namespace details

		// Converts a value of type T in FromUnits to a value of type T in ToUnits.
		template<class FromUnits, class ToUnits, class T>
		inline T convert(T value)
		{
			return details::convert_impl<T, FromUnits, ToUnits>()(value);
		};
	} // namespace units
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_UNITS_H