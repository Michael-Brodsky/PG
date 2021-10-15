/*
 *	This files defines a class that asynchronously controls a pulse-width-
 *	modulated (PWM) output.
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
 *	**************************************************************************/

#if !defined __PG_PWMOUTPUT_H
# define __PG_PWMOUTPUT_H 20211004L

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
	class Pwm
	{
	public:
		using duty_cycle_t = frequency_t;	// Fractional duty cycle type.
		using dc_type = uint8_t;			// Integral duty cycle type suitable for analogWrite() function.

	public:
		// Constructs an uninitialized pwm output.
		Pwm();
		// Constructs a pwm output attached to a digital output pin with a given duty cycle and state.
		Pwm(pin_t, duty_cycle_t = 0, bool = false);
		// Move constructor.
		Pwm(Pwm&&) = default;
		// No copy constructor.
		Pwm(const Pwm&) = delete;
		// No copy assignment operator.
		Pwm& operator=(const Pwm&) = delete;

	public:
		// Attaches the pwm output to a digital output pin.
		void attach(pin_t);
		// Returns the currently attached digital output pin.
		pin_t attach() const;
		// Returns the current pwm output frequency in Hz.
		frequency_t frequency() const;
		// Sets the pwm duty cycle.
		void duty_cycle(duty_cycle_t);
		// Returns the current pwm duty cycle.
		duty_cycle_t duty_cycle() const;
		// Sets the pwm output state.
		void enabled(bool);
		// Returns the current pwm output state.
		bool enabled() const;

	private:
		frequency_t set_frequency(frequency_t);
		void set_output();

	private:
		pin_t			pin_;			// The attached output pin number.
		frequency_t		frequency_;		// Current output frequency.
		duty_cycle_t	duty_cycle_;	// Current output duty cycle.
		bool			enabled_;		// Flag indicating whether the pwm output is currently enabled.
	};

	Pwm::Pwm() :
		pin_(InvalidPin), duty_cycle_(), frequency_(), enabled_()
	{

	}

	Pwm::Pwm(pin_t pin, duty_cycle_t duty_cycle, bool enabled) :
		pin_(board_traits<board_type>::pwm_frequency(pin) != 0 ? pin : InvalidPin),
		duty_cycle_(duty_cycle), frequency_(set_frequency(0)), enabled_(enabled)
	{
		set_output();
	}


	void Pwm::attach(pin_t pin)
	{
		pin_ = pin;
		frequency_ = set_frequency(0);
		set_output();
	}

	pin_t Pwm::attach() const
	{
		return pin_;
	}

	frequency_t Pwm::frequency() const
	{
		return frequency_;
	}

	void Pwm::duty_cycle(duty_cycle_t value)
	{
		const duty_cycle_t min = pg::duty_cycle<duty_cycle_t, dc_type>::frac_min, 
			max = pg::duty_cycle<duty_cycle_t, dc_type>::frac_max;
		duty_cycle_ = clamp(value, min, max);
		set_output();
	}

	Pwm::duty_cycle_t Pwm::duty_cycle() const
	{
		return duty_cycle_;
	}

	void Pwm::enabled(bool value)
	{
		enabled_ = value;
		set_output();
	}

	bool Pwm::enabled() const
	{
		return enabled_;
	}

	frequency_t Pwm::set_frequency(frequency_t ideal)
	{
		assert(ideal < board_traits<board_type>::clock_frequency);
		return pin_ != InvalidPin ? board_traits<board_type>::pwm_frequency(pin_) : 0;
	}

	void Pwm::set_output()
	{
		if (pin_ != InvalidPin)
			analogWrite(pin_, (enabled_) ? pg::duty_cycle<duty_cycle_t, dc_type>::frac_to_uint(duty_cycle_) : 0);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_PWMOUTPUT_H
