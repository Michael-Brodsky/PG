/*
 *	This files defines a class that controls a pulse-width-modulated (PWM) 
 *	output.
 *
 *	***************************************************************************
 *
 *	File: PWMOutput.h
 *	Date: October 5, 2021
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
 *	Description:
 * 
 *	The `PWMOutput' class provides a simple way of a managing a pulse-width-
 *	modulated (PWM) signal at on a digital output, usually with single lines 
 *	of code. Duty cycles are expressable in more natural fractional formats 
 *	(e.g. a percentage) before being converted to the appropriate type and 
 *	value for the analogWrite() function.
 *
 *	PWMOutput objects must be attached to a valid GPIO output pin that 
 *	supports PWM outputs. The duty cycle is set by the duty_cycle() method and 
 *	the output can be enabled or disabled with the enabled() method.
 * 
 *	Notes:
 * 
 *	Currently, only the board's native PWM frequencies are supported. If 
 *	available, they are found by calling pwm_frequency<board_type>(pin) 
 *	function for the type of board an attached pin (see <lib/boards.h>). 
 * 
 *	**************************************************************************/

#if !defined __PG_PWMOUTPUT_H
# define __PG_PWMOUTPUT_H 20211005L

# include "cassert"
# include "limits"
# include "system/boards.h"
# include "lib/fmath.h"

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// Converts fractional duty cycle values to unsinged integer values and vice-versa.
	template<class FracType, class UIntType>
	struct duty_cycle
	{
		static constexpr FracType frac_min = FracType(0);		// Minimum range of fractional values.
		static constexpr FracType frac_max = FracType(1.0);		// Maximum range of fractional values.
		static constexpr UIntType frac_to_uint(FracType frac)
		{
			return (UIntType)(clamp(frac, frac_min, frac_max) * FracType(std::numeric_limits<UIntType>::max()));
		}
		static constexpr FracType uint_to_frac(UIntType uint)
		{
			return (FracType)(FracType(uint) / FracType(std::numeric_limits<UIntType>::max()));
		}
	};

	// Type that outputs a pwm wave with a specified duty cycle.
	template<class T = float>
	class PWMOutput
	{
	public:
		using value_type = T;
		using dc_type = uint8_t;	// Integral duty cycle type suitable for analogWrite() function.

	public:
		// Constructs an uninitialized pwm output.
		PWMOutput();
		// Constructs a pwm output attached to a digital output pin with a given duty cycle and state.
		PWMOutput(pin_t, value_type = 0, bool = false);
		// Move constructor.
		PWMOutput(PWMOutput&&) = default;
		// No copy constructor.
		PWMOutput(const PWMOutput&) = delete;
		// No copy assignment operator.
		PWMOutput& operator=(const PWMOutput&) = delete;

	public:
		// Attaches the pwm output to a digital output pin.
		void attach(pin_t);
		// Returns the currently attached digital output pin.
		pin_t attach() const;
		// Returns the current pwm output frequency in Hz.
		value_type frequency() const;
		// Sets the pwm duty cycle.
		void duty_cycle(value_type);
		// Returns the current pwm duty cycle.
		value_type duty_cycle() const;
		// Sets the pwm output state.
		void enabled(bool);
		// Returns the current pwm output state.
		bool enabled() const;

	private:
		value_type set_frequency(value_type);
		void set_output();

	private:
		pin_t		pin_;			// The attached output pin number.
		value_type	frequency_;		// Current output frequency.
		value_type	duty_cycle_;	// Current output duty cycle.
		bool		enabled_;		// Flag indicating whether the pwm output is currently enabled.
	};

	template<class T>
	PWMOutput<T>::PWMOutput() :
		pin_(InvalidPin), duty_cycle_(), frequency_(), enabled_()
	{

	}

	template<class T>
	PWMOutput<T>::PWMOutput(pin_t pin, value_type duty_cycle, bool enabled) :
		pin_(board_traits<board_type>::pwm_frequency(pin) != 0 ? pin : InvalidPin),
		duty_cycle_(duty_cycle), frequency_(set_frequency(0)), enabled_(enabled)
	{
		set_output();
	}

	template<class T>
	void PWMOutput<T>::attach(pin_t pin)
	{
		pin_ = pin;
		frequency_ = set_frequency(0);
		set_output();
	}

	template<class T>
	pin_t PWMOutput<T>::attach() const
	{
		return pin_;
	}

	template<class T>
	typename PWMOutput<T>::value_type PWMOutput<T>::frequency() const
	{
		return frequency_;
	}

	template<class T>
	void PWMOutput<T>::duty_cycle(value_type value)
	{
		const value_type min = pg::duty_cycle<value_type, dc_type>::frac_min,
			max = pg::duty_cycle<value_type, dc_type>::frac_max;
		duty_cycle_ = clamp(value, min, max);
		set_output();
	}

	template<class T>
	typename PWMOutput<T>::value_type PWMOutput<T>::duty_cycle() const
	{
		return duty_cycle_;
	}

	template<class T>
	void PWMOutput<T>::enabled(bool value)
	{
		enabled_ = value;
		set_output();
	}

	template<class T>
	bool PWMOutput<T>::enabled() const
	{
		return enabled_;
	}

	template<class T>
	typename PWMOutput<T>::value_type PWMOutput<T>::set_frequency(value_type ideal)
	{
		assert(ideal < board_traits<board_type>::clock_frequency);
		return pin_ != InvalidPin ? board_traits<board_type>::pwm_frequency(pin_) : 0;
	}

	template<class T>
	void PWMOutput<T>::set_output()
	{
		if (pin_ != InvalidPin)
			analogWrite(pin_, (enabled_) ? pg::duty_cycle<value_type, dc_type>::frac_to_uint(duty_cycle_) : 0);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_PWMOUTPUT_H
