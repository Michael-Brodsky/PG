/*
 *	This files defines several types for managing pulse-width-modulated (PWM) 
 *	outputs.
 *
 *	***************************************************************************
 *
 *	File: PWMOutput.h
 *	Date: November 7, 2021
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
 *	**************************************************************************
 *
 *	Description:
 *
 *	The `PWMOutput' class provides a simple interface for controlling PWM 
 *	outputs using natural units, frequencies in Hertz and pulse widths as 
 *	percentage of duty cycle. This eliminates any client overhead in having to 
 *	convert natural units to binary register values. PWMOutput objects can be  
 *	attached to any valid pwm output pin and modify the output frequency, 
 *	duty cycle, enabled state and range with single lines of code. The range 
 *	method provides clients the ability to narrow or widen the duty cycle 
 *	range, with wide ranges allowing fine duty cycle adjustments and narrow 
 *	ranges providing ever coarser adjustments down to bang-bang behavior 
 *  (full on/off). Duty cycle ranges are encapsulated in the nested 
 *	PWMOutput::Range type.
 * 
 *	The `duty_cycle' type is a PWMOutput helper class used for compile-time 
 *	evaluations and conversions, but is also accessible to clients for 
 *	duty cycle-related computations.
 * 
 *	Notes:
 * 
 *	This code relies on proper board identification to get certain values 
 *	needed for computing duty cycles and frequencies. The data was obtained 
 *	from publicly available sources, however no claims are made as to its 
 *	accuracy. See <system/boards.h> for details.
 *
 *	**************************************************************************/

#if !defined __PG_PWMOUTPUT_H
# define __PG_PWMOUTPUT_H 20211005L

# include <cassert>
# include <limits>
# include <utility>
# include <system/boards.h>
# include <lib/fmath.h>

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	template<class T, class CtrlType>
	struct duty_cycle
	{
		// Absolute minimum fractional duty cycle ~0%.
		static constexpr T min = T(0.0);
		// Absolute maximum fractional duty cycle ~100%.
		static constexpr T max = T(1.0);		
		// Returns the minimum fractional duty cycle resolution for CtrlType. 
		static constexpr T res(T low = min, T high = max)
		{
			return (high - low) / static_cast<T>(std::numeric_limits<CtrlType>::max() - std::numeric_limits<CtrlType>::min());
		}
		// Converts a fractional duty cycle to a control value.
		static constexpr CtrlType dc_to_ctrl(T dc, T low = min, T high = max)
		{
			return norm(clamp(dc, low, high), low, high, std::numeric_limits<CtrlType>::min(), std::numeric_limits<CtrlType>::max());
		}
		// Converts a control value to a fractional duty cycle.
		static constexpr T ctrl_to_dc(CtrlType value)
		{
			return norm(value, std::numeric_limits<CtrlType>::min(), std::numeric_limits<CtrlType>::max(), min, max);
		}
	};

	// PWM output controller. (T = fractional duty cycle type, CtrlType = interger control register type.)
	template<class T = float, class CtrlType = uint8_t>
	class PWMOutput
	{
	public:
		using value_type = T;
		using control_type = CtrlType;
		using range_type = std::pair<value_type, value_type>;
		using dc_type = duty_cycle<value_type, control_type>;
		// Encapsulates information about duty cycle ranges.
		class Range
		{
			// A duty cycle is expressed as a fractional value in [0.0, 1.0], 0% and 100%  
			// respectively. A Range object has two members: low and high. Commanded duty 
			// cycles in [low,high] are normalized to [0.0, 1.0] (0% - 100%). Those less 
			// than low always evaluate to 0.0 while those greater than high evaluate to 
			// 1.0. This allows clients to squeeze or stretch the entire duty cycle into 
			// narrower or wider bands, acting as a sort of gain factor reflected about 
			// the range's median value. A Range of [0.0, 1.0] will result in linear    
			// behavior: output duty cycle = commanded duty cycle. Extremely small 
			// Ranges such as [0.0, 0.00001] will result in bang-bang behavior where 
			// a commanded duty cycle of 0.0 will produce a 0% output and any positive 
			// value above that will produce a 100% output (subject to floating precision 
			// and epsilons).
		public:
			// Constructs a default Range of [0.0, 1.0].
			Range() : range_({ dc_type::min,dc_type::max }) {}
			// Constructs a Range in [low, high], low < high. 
			Range(value_type low, value_type high) : range_({ low,high }) { assert(low < high); }

		public:
			range_type& range() { return range_; }
			const range_type& range() const { return range_; }
			value_type& low() { return range_.first; }
			const value_type& low() const { return range_.first; }
			value_type& high() { return range_.second; }
			const value_type& high() const { return range_.second; }

		private:
			range_type range_;	// The current range.
		};

	public:
		// Constructs an uninitialized PWMOutput.
		PWMOutput();
		// Constructs a PWMOutput attached to an output pin with a given duty cycle, range and state.
		PWMOutput(pin_t, value_type = 0, bool = false, Range = Range());

	public:
		// Attaches to a specified pwm output pin.
		void attach(pin_t);
		// Returns the currently attached pwm output pin.
		pin_t attach() const;
		// Returns the current pwm output frequency in Hz.
		const value_type& frequency() const;
		// Sets the output duty cycle.
		void dutyCycle(const value_type&);
		// Returns the current output duty cycle.
		const value_type& dutyCycle() const;
		// Sets the output duty cycle range.
		void range(const Range&);
		// Returns the current output duty cycle range.
		const Range& range() const;
		// Sets the pwm output state.
		void enabled(bool);
		// Returns the current pwm output state.
		bool enabled() const;

	private:
		// Sets and returns the closest possible frequency to a given ideal frequency.
		value_type setFrequency(value_type = value_type());
		// Sets and returns the closest possible output duty cycle to a given ideal duty cycle.
		value_type setOutput(value_type = duty_cycle<value_type, control_type>::min);

	private:
		pin_t		pin_;			// The attached pwm output pin.
		value_type	frequency_;		// The current pwm output frequency.
		Range		range_;			// The current duty cycle range.
		bool		enabled_;		// Flag indicating whether the pwm output is currently enabled.
		value_type	duty_cycle_;	// The current output duty cycle.
	};

	template<class T, class CtrlType>
	PWMOutput<T, CtrlType>::PWMOutput() :
		pin_(InvalidPin), frequency_(setFrequency()), range_(), enabled_(), duty_cycle_(setOutput())
	{

	}

	template<class T, class CtrlType>
	PWMOutput<T, CtrlType>::PWMOutput(pin_t pin, value_type duty_cycle, bool enabled, Range range) : 
		pin_(board_traits<board_type>::pwm_frequency(pin) != 0 ? pin : InvalidPin), 
		frequency_(setFrequency(board_traits<board_type>::pwm_frequency(pin))), 
		range_(range), enabled_(enabled), duty_cycle_(setOutput(duty_cycle))
	{

	}

	template<class T, class CtrlType>
	void PWMOutput<T, CtrlType>::attach(pin_t pin)
	{
		pin_ = pin;
		frequency_ = setFrequency(board_traits<board_type>::pwm_frequency(pin));
		duty_cycle_ = setOutput(duty_cycle_);
	}

	template<class T, class CtrlType>
	pin_t PWMOutput<T, CtrlType>::attach() const
	{
		return pin_;
	}

	template<class T, class CtrlType>
	const typename PWMOutput<T, CtrlType>::value_type& PWMOutput<T, CtrlType>::frequency() const
	{
		return frequency_;
	}

	template<class T, class CtrlType>
	void PWMOutput<T, CtrlType>::dutyCycle(const value_type& dc)
	{
		duty_cycle_ = setOutput(dc);
	}

	template<class T, class CtrlType>
	const typename PWMOutput<T, CtrlType>::value_type& PWMOutput<T, CtrlType>::dutyCycle() const
	{
		return duty_cycle_;
	}

	template<class T, class CtrlType>
	void PWMOutput<T, CtrlType>::range(const Range& rng)
	{
		range_ = rng;
		duty_cycle_ = setOutput(duty_cycle_);
	}

	template<class T, class CtrlType>
	const typename PWMOutput<T, CtrlType>::Range& PWMOutput<T, CtrlType>::range() const
	{
		return range_;
	}

	template<class T, class CtrlType>
	void PWMOutput<T, CtrlType>::enabled(bool value)
	{
		enabled_ = value;
		duty_cycle_ = setOutput(duty_cycle_);
	}

	template<class T, class CtrlType>
	bool PWMOutput<T, CtrlType>::enabled() const
	{
		return enabled_;
	}

	template<class T, class CtrlType>
	typename PWMOutput<T, CtrlType>::value_type PWMOutput<T, CtrlType>::setFrequency(value_type f)
	{
		return f;
	}

	template<class T, class CtrlType>
	typename PWMOutput<T, CtrlType>::value_type PWMOutput<T, CtrlType>::setOutput(value_type dc)
	{
		control_type ctrl = duty_cycle<value_type, control_type>::dc_to_ctrl(dc, range_.low(), range_.high());

		analogWrite(pin_, !(pin_ == InvalidPin || enabled_ == false) ? ctrl : std::numeric_limits<CtrlType>::min());
		
		return duty_cycle<value_type, control_type>::ctrl_to_dc(ctrl);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and named namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_PWMOUTPUT_H