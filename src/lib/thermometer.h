/*
 *	This file defines some standard algorithms for computing temperatures.
 *
 *	***************************************************************************
 *
 *	File: thermometer.h
 *	Date: October 14, 2021
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

#if !defined __THERMOMETER_H 
# define __THERMOMETER_H 20211014L

# include "fmath.h"
# include "units.h"

namespace pg
{
	namespace details
	{
		// Primary temperature conversion template.
		template<class T, class Units>
		struct temperature_impl;

		template<class T>
		struct temperature_impl<T, units::kelvin>
		{
			T operator()(T value) { return value; }
		};

		template<class T>
		struct temperature_impl<T, units::celsius>
		{
			T operator()(T value) { return units::convert<units::kelvin, units::celsius>(value); }
		};

		template<class T>
		struct temperature_impl<T, units::fahrenheit>
		{
			T operator()(T value) { return units::convert<units::kelvin, units::fahrenheit>(value); }
		};
	}

	// Returns value in degrees Kelvin converted to Units.
	template<class Units, class T>
	inline T temperature(T value)
	{
		return details::temperature_impl<T, Units>()(value);
	};

	// Returns the sensed thermistor temperature, in degrees K, using the Steinhart-Hart eqn.
	template<class T, class ADCType>
	T tsense(ADCType adc_out, ADCType adc_max, T r, T vss, T dc, T a, T b, T c)
	{
		const T vin = vsense(adc_out, adc_max, vss, dc);

		return thermistor(rsense(vin, vss, r), a, b, c);
	}

	// Returns the sensed thermistor temperature, in degrees K, using the beta-parameter eqn.
	template<class T, class ADCType>
	T tsense(ADCType adc_out, ADCType adc_max, T r, T vss, T dc, T rinf, T beta)
	{
		const T vin = vsense(adc_out, adc_max, vss, dc);

		return thermistor(rsense(vin, vss, r), rinf, beta);
	}

	// Temperature alarm compare less function.
	template<class T>
	bool alarm_lt(T lhs, T rhs) { return lhs < rhs; }

	// Temperature alarm compare greater function.
	template<class T>
	bool alarm_gt(T lhs, T rhs) { return lhs > rhs; }

} // namespace pg

#endif // !defined __THERMOMETER_H 
