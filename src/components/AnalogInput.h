/*
 *	This files defines a class that asynchronously polls a GPIO analog input.
 *
 *	***************************************************************************
 *
 *	File: AnalogInput.h
 *	Date: October 2, 2021
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
 *	The `AnalogInput' class is used to poll analog input pins and issue client 
 *	callbacks if the value read from the attached input falls within a 
 *	specified range. The nested `Range' type encapsulates information about  
 *	input ranges. Input values are read by the Arduino analogRead() API 
 *	function.
 * 
 *	AnalogInput objects must be attached to a valid GPIO analog input, either 
 *	in the constructor or the attach() method. If range checking and callbacks 
 *	are used, they must also be specified in the constructor, or the ranges() 
 *	and callback() methods respectively. If ranges and callbacks are not used, 
 *	these parameters should be omitted.
 * 
 *	Range objects contain two fields, `range_' as std:pair<analog_t, analog_t> 
 *	and `tag_' which is an enumerated type that is used to uniquely identify 
 *	ranges. Clients must define an AnalogInput::Range::Tag object which must 
 *	be visible to the AnalogInput class:
 * 
 *		enum class pg::AnalogInput::Range::Tag
 *		{ 
 *			LowRange = 0, 
 *			HighRange, 
 *			OtherRange 
 *		};
 * 
 *		pg::AnalogInput::Range range1 = { 
 *			AnalogInput::Range::range_type(0,60), 
 *			pg::AnalogInput::Range::Tag::LowRange
 *		};
 * 
 *	AnalogInput objects can be polled synchronously with `operator()' or 
 *	asynchronously using the clock() method. `operator()' simply returns the 
 *	value returned by the Arduino analogRead() function. The clock() method, 
 *	in addition to reading the input value, also checks whether the value 
 *	falls within a specified range and, if so, executes a callback.
 *
 *	AnalogInput also specializes six non-member comparison functions which can 
 *	be used to compare two AnalogInput objects for equality. The functions 
 *	compare on the last value read from the attached input. If clients need to 
 *	read a new value immediately before comparison, operator() should be used 
 *	with the comparison functions:
 * 
 *		AnalogInput in1(0), in2(1);
 *		bool b1 = in1 == in2;		// Compares the last known input values.
 *		bool b2 = in1() == in2();	// Reads and compares the current values.
 * 
 *	AnalogInput objects are not copyable or assignable as this would lead to 
 *	multiple instances attached to the same analog input, which is redundant.
 *
 *	**************************************************************************/

#if !defined __PG_ANALOG_INPUT_H
#define __PG_ANALOG_INPUT_H 20211002L

#include "utility"					// std::pair type.
#include "array"					// Fixed-size array types.
#include "system/types.h"			// pin_t and analog_t types.
#include "interfaces/icomponent.h"	// icomponent interface.
#include "interfaces/iclockable.h"	// iclockable interface.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Asyncronous analog input polling class.
	class AnalogInput : public icomponent, public iclockable 
	{
	public:
		// Encapsulates information about a range of analog input values.
		struct Range
		{
			enum class Tag; // Must be defined by clients.
			using range_type = std::pair<analog_t, analog_t>; // first is low, second is high.

			range_type	range_;	// The input range, low, high.
			Tag			tag_;	// Range unique identifier.

			// Checks whether value is within the range.
			bool in_range(analog_t value) { return value >= range_.first && value <= range_.second; }
		};

		using callback_type = typename callback<void, void, pin_t, analog_t, Range*>::type;
		using container_type = std::ArrayWrapper<Range*>;
		using iterator = typename container_type::iterator;

	public:
		// Constructs an uninitialized AnalogInput.
		AnalogInput();
		// Constructs an AnalogInput attached to the given analog input pin.
		explicit AnalogInput(pin_t);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified in an array.
		template<std::size_t N>
		AnalogInput(pin_t, callback_type, Range* (&)[N]);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified by a pointer and size.
		AnalogInput(pin_t, callback_type, Range* [], std::size_t);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified by a range.
		AnalogInput(pin_t, callback_type, Range**, Range**);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified by a list.
		AnalogInput(pin_t, callback_type, std::initializer_list<Range*>);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified by a container.
		AnalogInput(pin_t, callback_type, container_type&);
		// No copy constructor.
		AnalogInput(const AnalogInput&) = delete;
		// No copy assignment operator.
		AnalogInput& operator=(const AnalogInput&) = delete;

	public:
		// Attaches to the specified analog input pin.
		void attach(pin_t);
		// Returns the attached analog input pin, or pg::InvalidPin if not attached.
		pin_t attach() const;
		// Sets the input ranges from an array.
		template<std::size_t N>
		void ranges(Range* (&)[N]);
		// Sets the input ranges from a pointer and size.
		void ranges(Range* [], std::size_t);
		// Sets the input ranges from a range.
		void ranges(Range**, Range**);
		// Sets the input ranges from a list.
		void ranges(std::initializer_list<Range*>);
		// Sets the input ranges from a container.
		void ranges(container_type&);
		// Returns the current input ranges.
		const container_type& ranges() const;
		// Sets the client callback.
		void callback(callback_type);
		// Returns the last read input value.
		analog_t value() const;
		// Reads and returns the current input value.
		analog_t operator()();
		// Polls the input and executes a callback if any input ranges were matched.
		void			poll();

	private:
		// Reads the current input value and returns the matched range, if any.
		iterator read_input();
		// Polls the input and executes a callback if any input ranges were matched.
		void clock() override;

	private:
		pin_t			pin_;		// The attached GPIO analog input pin.
		analog_t		value_;		// The last value read from the input.
		container_type	ranges_;	// The collection of input ranges.
		iterator		current_;	// The last range matched by the last input value, if any.
		callback_type	callback_;	// The client callback.
	};

#pragma region member_funcs

	AnalogInput::AnalogInput() :
		pin_(InvalidPin), value_(), callback_(), ranges_(), current_()
	{

	}

	AnalogInput::AnalogInput(pin_t pin) : 
		pin_(pin), value_(), callback_(), ranges_(), current_() 
	{
		
	}

	template<std::size_t N>
	AnalogInput::AnalogInput(pin_t pin, callback_type callback, Range* (&ranges)[N]) : 
		pin_(pin), value_(), callback_(callback),
		ranges_(ranges), current_(std::end(ranges_))
	{

	}

	AnalogInput::AnalogInput(pin_t pin, callback_type callback, Range* ranges[], std::size_t n) :
		pin_(pin), value_(), callback_(callback),
		ranges_(ranges, n), current_(std::end(ranges_))
	{

	}

	AnalogInput::AnalogInput(pin_t pin, callback_type callback, Range** first, Range** last) :
		pin_(pin), value_(), callback_(callback),
		ranges_(first, last), current_(std::end(ranges_))
	{

	}

	AnalogInput::AnalogInput(pin_t pin, callback_type callback, container_type& ranges) : 
		pin_(pin), value_(), callback_(callback), 
		ranges_(ranges), current_(std::end(ranges_))
	{
		
	}

	AnalogInput::AnalogInput(pin_t pin, callback_type callback, std::initializer_list<Range*> il) :
		pin_(pin), value_(), callback_(callback),
		ranges_(const_cast<Range**>(il.begin()), il.size()), current_(std::end(ranges_))
	{
		std::size_t i = 0;

		for (auto j : il)
			*ranges_[i++] = *j;
	}

	void AnalogInput::attach(pin_t pin)
	{
		pin_ = pin;
	}

	pin_t AnalogInput::attach() const
	{
		return pin_;
	}

	template<std::size_t N>
	void AnalogInput::ranges(Range* (&ranges)[N])
	{
		ranges_ = container_type(ranges);
		current_ = std::end(ranges_);
	}

	void AnalogInput::ranges(Range* ranges[], std::size_t n)
	{
		ranges_ = container_type(ranges, n);
		current_ = std::end(ranges_);
	}

	void AnalogInput::ranges(Range** first, Range** last)
	{
		ranges_ = container_type(first, last);
		current_ = std::end(ranges_);
	}

	void AnalogInput::ranges(std::initializer_list<Range*> il)
	{
		ranges_ = container_type(const_cast<Range**>(il.begin()), il.size());
		std::size_t i = 0;

		for (auto j : il)
			*ranges_[i++] = *j;
		current_ = std::end(ranges_);
	}

	void AnalogInput::ranges(container_type& ranges)
	{
		ranges_ = ranges;
		current_ = std::end(ranges_);
	}
	const typename AnalogInput::container_type& AnalogInput::ranges() const
	{
		return ranges_;
	}

	void AnalogInput::callback(callback_type callback)
	{
		callback_ = callback;
	}

	analog_t AnalogInput::operator()()
	{
		return (value_ = analogRead(pin_));
	}

	analog_t AnalogInput::value() const
	{
		return value_;
	}

	void AnalogInput::poll()
	{
		iterator i = read_input();

		if (i != current_)
		{
			if (i != std::end(ranges_))
				(*callback_)(pin_, value_, *i);
			current_ = i;
		}
	}

	typename AnalogInput::iterator AnalogInput::read_input()
	{
		iterator i = std::end(ranges_);

		(void)operator()();
		for (auto j : ranges_)
		{
			if (j->in_range(value_))
			{
				i = &j;
				break;
			}
		}

		return i;
	}

	void AnalogInput::clock()
	{
		poll();
	}

#pragma endregion
#pragma region non-member_funcs

	inline bool operator==(const AnalogInput& lhs, const AnalogInput& rhs)
	{
		return lhs.value() == rhs.value();
	}

	inline bool operator!=(const AnalogInput& lhs, const AnalogInput& rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator<(const AnalogInput& lhs, const AnalogInput& rhs)
	{
		return lhs.value() < rhs.value();
	}

	inline bool operator>(const AnalogInput& lhs, const AnalogInput& rhs)
	{
		return (rhs < lhs);
	}

	inline bool operator<=(const AnalogInput& lhs, const AnalogInput& rhs)
	{
		return !(rhs < lhs);
	}

	inline bool operator>=(const AnalogInput& lhs, const AnalogInput& rhs)
	{
		return !(lhs < rhs);
	}

#pragma endregion
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_ANALOG_INPUT_H
