/*
 *	This files defines a class that asynchronously polls a GPIO digital input.
 *
 *	***************************************************************************
 *
 *	File: DigitalInput.h
 *	Date: October 3, 2021
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
 *	**************************************************************************
 *
 *	Description:
 *
 *	The `DigitalInput' class is used to configure and poll digital input pins 
 *	and issue client callbacks if the value read from the attached input 
 *	triggers it. Input triggering can be disable, edge or level triggered, 
 *  active low (0) or active high (1) and configured to enable/disable the 
 *	built-in pullup resistor. Input values are read by the Arduino 
 *	digitalRead() API function and input modes set by the pinMode() function.
 *	
 *	DigitalInput objects must be attached to a valid GPIO digital input, 
 *	either in the constructor or the attach() method. The trigger type and 
 *  input pin mode is set in the constructor or the trigger() and level() 
 *	methods respectively. The nested Trigger type enumerates valid triggers: 
 * 
 *		None - input triggering is disabled, 
 *		Edge - input is edge triggered, 
 *		Level - input is level triggered, 
 * 
 *	and the nested PinMode type enumerates valid pin modes: 
 * 
 *		Input - the input pullup resistor is disabled, 
 *		Input_Pullup - the input pullup resistor is enabled.
 * 
 *	The level parameter indicates whether the input is active high or active 
 *	low:
 * 
 *		false - active low/triggered on falling edge, 
 *		true - active high/triggered on rising edge. 
 * 
 *	The callback parameter sets the client callback function, if any, and is 
 *	set in the constructor or callback() method. If set, a DigitalInput 
 *	object will issue a callback to the client passing the attached pin and 
 *	current input level. The mode, trigger and level parameters are defaulted 
 *	to Input_Pullup, Edge and false in the constructor.
 * 
 *	DigitalInput objects can be operated synchronously with `operator()', 
 *	which reads and returns the current input level, or asynchronously with 
 *	the clock() method. The clock() method, in addition to reading the input 
 *	level, also test if the level satisfies the trigger settings and, if so, 
 *	issues a client callback.
 * 
 *	DigitalInput objects are not copyable or assignable as this would lead to 
 *	multiple instances attached to the same digital input, which is redundant. 
 *	
 *	**************************************************************************/

#if !defined __PG_DIGITAL_INPUT_H
# define __PG_DIGITAL_INPUT_H 20211003L

#include "lib/pgtypes.h"			// pin_t type.
#include "interfaces/icomponent.h"	// icomponent interface.
#include "interfaces/iclockable.h"	// iclockable interface.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Asyncronous digital input polling class.
	class DigitalInput : public icomponent, public iclockable
	{
	public:
		// Enumerates valid input trigger types.
		enum class Trigger
		{
			None = 0, 
			Edge,
			Level
		};

		// Enumerates valid input pin modes.
		enum class PinMode : uint8_t 
		{
			Input = 0,			// Internal pullup resistor is disabled.
			Input_Pullup = 2	// Internal pullup resistor is enabled.
		};

		using callback_type = typename callback<void, void, pin_t, bool>::type;

	public:
		// Constructs an uninitialized DigitalInput.
		DigitalInput();
		// Constructs a DigitalInput attached to the given pin and configured with the specified 
		// mode, trigger, level and callback.
		explicit DigitalInput(pin_t, PinMode = PinMode::Input_Pullup, Trigger = Trigger::Edge, 
			bool = false, callback_type = nullptr);
		// No copy constructor.
		DigitalInput(const DigitalInput&) = delete;
		// No copy assignment operator.
		DigitalInput& operator=(const DigitalInput&) = delete;

	public:
		// Attaches the a digital input pin and sets its pin mode.
		void			attach(pin_t, PinMode = PinMode::Input_Pullup);
		// Returns the currently attached input pin or InvalidPin if not attached.
		pin_t			attach() const;
		// Sets the input trigger type.
		void			trigger(Trigger);
		// Returns the current input trigger type.
		Trigger			trigger() const;
		// Sets the input trigger level.
		void			level(bool);
		// Returns the current input trigger level.
		bool			level() const;
		// Sets the client callback function.
		void			callback(callback_type);
		// Reads and returns the currently attached input level.
		bool			operator()();
		// Returns the last known input level.
		bool			value() const;
		// Polls the input and executes a callback if the input has been triggered.
		void			poll();

	private:
		// Checks whether the attached input has been triggered.
		bool			triggered();
		// Polls the input and executes a callback if the input has been triggered.
		void			clock() override;

	private:
		pin_t			pin_;		// The attached digital input pin.
		PinMode			mode_;		// The current input pin mode.
		bool			value_;		// The last value read from the attached input.
		Trigger			trigger_;	// The current trigger type.
		bool			level_;		// The current trigger level.
		callback_type	callback_;	// Client callback function, if any.
	};

#pragma region member_funcs

	DigitalInput::DigitalInput() : 
		pin_(InvalidPin), mode_(), value_(), trigger_(), level_(), callback_() 
	{

	}

	DigitalInput::DigitalInput(pin_t pin, PinMode mode, Trigger trigger, bool level, callback_type callback) :
		pin_(), mode_(), value_(), trigger_(trigger), level_(level), callback_(callback)
	{
		attach(pin, mode);
	}

	void DigitalInput::attach(pin_t pin, PinMode mode)
	{
		mode_ = mode;
		if ((pin_ = pin) != InvalidPin)
			pinMode(pin, static_cast<uint8_t>(mode));
	}

	pin_t DigitalInput::attach() const
	{
		return pin_;
	}

	void DigitalInput::trigger(Trigger type)
	{
		trigger_ = type;
	}

	DigitalInput::Trigger DigitalInput::trigger() const
	{
		return trigger_;
	}

	void DigitalInput::level(bool value)
	{
		level_ = value;
	}

	bool DigitalInput::level() const
	{
		return level_;
	}

	void DigitalInput::callback(callback_type callback)
	{
		callback_ = callback;
	}

	bool DigitalInput::operator()()
	{
		return (value_ = digitalRead(pin_));
	}

	bool DigitalInput::value() const
	{
		return value_;
	}

	void DigitalInput::poll()
	{
		if (triggered() && callback_)
			(*callback_)(pin_, value_);
	}

	bool DigitalInput::triggered()
	{
		bool result = false;
		bool previous = value_;

		(void)operator()();
		switch (trigger_)
		{
		case Trigger::Level:
			result = value_ == level_;
			break;
		case Trigger::Edge:
			result = value_ == level_ && value_ != previous;
			break;
		default:
			break;
		};

		return result;
	}

	void DigitalInput::clock()
	{
		poll();
	}

#pragma endregion
#pragma region non-member_funcs

	inline bool operator==(const DigitalInput& lhs, const DigitalInput& rhs)
	{
		return lhs.value() == rhs.value();
}

	inline bool operator!=(const DigitalInput& lhs, const DigitalInput& rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator<(const DigitalInput& lhs, const DigitalInput& rhs)
	{
		return lhs.value() < rhs.value();
	}

	inline bool operator>(const DigitalInput& lhs, const DigitalInput& rhs)
	{
		return (rhs < lhs);
	}

	inline bool operator<=(const DigitalInput& lhs, const DigitalInput& rhs)
	{
		return !(rhs < lhs);
	}

	inline bool operator>=(const DigitalInput& lhs, const DigitalInput& rhs)
	{
		return !(lhs < rhs);
	}

#pragma endregion
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

# endif // !defined __PG_DIGITAL_INPUT_H

