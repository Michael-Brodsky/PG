/*
 *	This files defines a class that asynchronously polls a GPIO analog input.
 *
 *	***************************************************************************
 *
 *	File: AnalogInput.h
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
 *	**************************************************************************
 * 
 *	Description:
 * 
 *	The `AnalogInput' class is used to poll analog input pins and issue client 
 *	callbacks if the value read from the attached input falls within a 
 *	specified range. The nested `Range' type encapsulates information about  
 *	input ranges. Ranges are compared to values returned by the Arduino 
 *	analogRead() function from the attached input pin. The template parameter 
 *	`T' determines the data type and should be the same as that returned by 
 *	analogRead().
 * 
 *	AnalogInput objects must be attached to a valid GPIO analog input, either 
 *	in the constructor or the attach() method. If range checking and callbacks 
 *	are used, they must also be specified in the constructor, or the ranges() 
 *	and callback() methods respectively. If ranges and callbacks are not used, 
 *	these parameters should be omitted.
 * 
 *	Range objects have a `range_' field that specifies a range of input 
 *	values, and inherit the `id()' method from the base class `Unique', which 
 *	can be use to to uniquely identify Range instances. The `range_' field is 
 *	of type std:pair<value_type, value_type>, where `first; is the range 
 *	minimum and `second' the range maximum value. A client callback is 
 *	triggered on each call to the `poll()' method if the `match_any_' flag is 
 *	set or if the polled value falls within [first, second]. The match_any_ 
 *	flag is set in the constructor or the `matchAny()' method and is always 
 *	set if no ranges are specified. The`clock()' method can be used to poll 
 *	inputs asynchronously. 
 *
 *	The function call operator `operator()' returns the current input value.
 *	AnalogInput also specializes six non-member comparison functions which can 
 *	be used to compare two AnalogInput objects for equality. The functions 
 *	compare on the last value read from the attached input. If clients need to 
 *	read fresh values before comparison, operator() must be used with the 
 *	comparison functions:
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
# define __PG_ANALOG_INPUT_H 20211005L

# include "utility"					// std::pair type.
# include "array"					// Fixed-size array types.
# include "system/types.h"			// pin_t and analog_t types.
# include "interfaces/icomponent.h"	// icomponent interface.
# include "interfaces/iclockable.h"	// iclockable interface.
# include "utilities/ValueWrappers.h"
# include "utilities/Unique.h"		// Unique base class.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Asyncronous analog input polling class.
	template<class T = analog_t>
	class AnalogInput : public icomponent, public iclockable 
	{
	public:
		using value_type = T;

		// Encapsulates information about a range of analog input values.
		struct Range : public RangeValueWrapper<T>, public Unique
		{
			using base_type = RangeValueWrapper<T>;
			using data_type = typename base_type::data_type;

			Range() = default;
			Range(const value_type& value) : base_type(data_type{ value,value }) {}
			Range(const value_type& low, const value_type & high) : base_type(data_type{ low,high }) {}
			Range(const data_type& data) : base_type(data) {}
		};

		using callback_type = typename callback<void>::type;
		using container_type = std::ArrayWrapper<Range*>;
		using iterator = typename container_type::iterator;

	public:
		// Constructs an uninitialized AnalogInput.
		AnalogInput();
		// Constructs an AnalogInput attached to the given analog input pin.
		explicit AnalogInput(pin_t);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified by a range.
		AnalogInput(pin_t, callback_type, Range** = nullptr, Range** = nullptr, bool = false);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified in an array.
		template<std::size_t N>
		AnalogInput(pin_t, callback_type, Range* (&)[N], bool = false);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified by a pointer and size.
		AnalogInput(pin_t, callback_type, Range* [], std::size_t, bool = false);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified by a list.
		AnalogInput(pin_t, callback_type, std::initializer_list<Range*>, bool = false);
		// Constructs an AnalogInput attached to the given pin and with input ranges specified by a container.
		AnalogInput(pin_t, callback_type, container_type&, bool = false);
		// Move constructor.
		AnalogInput(AnalogInput&&) = default;
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
		// Sets whether a callback is triggered on any range match.
		void matchAny(bool);
		// Sets whether a callback is triggered on any range match.
		bool matchAny() const;
		// Sets the client callback.
		void callback(callback_type);
		// Reads and returns the current input value.
		value_type operator()();
		// Returns the last read input value.
		value_type value() const;
		// Returns the last matched range, if any.
		Range* range() const;
		// Polls the input and executes a callback if any input ranges were matched.
		void poll();

	private:
		// Sets the pin mode to an analog input.
		void setPinMode(pin_t);
		// Reads the current input value and returns the matched range, if any.
		iterator read_input();
		// Executes the current client callback, if any.
		void doCallback();
		// Polls the input and executes a callback if any input ranges were matched.
		void clock() override;

	private:
		pin_t			pin_;		// The attached GPIO analog input pin.
		value_type		value_;		// The last value read from the input.
		container_type	ranges_;	// The collection of input ranges.
		iterator		current_;	// The last range matched by the last input value, if any.
		bool			match_any_;	// Flag indicating whether any matched range triggers a callback.
		callback_type	callback_;	// The client callback.
	};

#pragma region member_funcs

	template<class T>
	AnalogInput<T>::AnalogInput() :
		pin_(InvalidPin), value_(), callback_(), ranges_(), current_(), match_any_(ranges_.size() == 0)
	{

	}

	template<class T>
	AnalogInput<T>::AnalogInput(pin_t pin) :
		pin_(pin), value_(), callback_(), ranges_(), current_(), match_any_(ranges_.size() == 0)
	{
		setPinMode(pin);
	}

	template<class T>
	template<std::size_t N>
	AnalogInput<T>::AnalogInput(pin_t pin, callback_type callback, Range* (&ranges)[N], bool match_any) :
		pin_(pin), value_(), callback_(callback), ranges_(ranges), 
		current_(std::end(ranges_)), match_any_(ranges_.size() == 0 || match_any)
	{
		setPinMode(pin);
	}

	template<class T>
	AnalogInput<T>::AnalogInput(pin_t pin, callback_type callback, Range* ranges[], std::size_t n, bool match_any) :
		pin_(pin), value_(), callback_(callback), ranges_(ranges, n), 
		current_(std::end(ranges_)), match_any_(ranges_.size() == 0 || match_any)
	{
		setPinMode(pin);
	}

	template<class T>
	AnalogInput<T>::AnalogInput(pin_t pin, callback_type callback, Range** first, Range** last, bool match_any) :
		pin_(pin), value_(), callback_(callback), ranges_(first, last), 
		current_(std::end(ranges_)), match_any_(ranges_.size() == 0 || match_any)
	{
		setPinMode(pin);
	}

	template<class T>
	AnalogInput<T>::AnalogInput(pin_t pin, callback_type callback, container_type& ranges, bool match_any) :
		pin_(pin), value_(), callback_(callback), 
		ranges_(ranges), current_(std::end(ranges_)), match_any_(ranges_.size() == 0 || match_any)
	{
		setPinMode(pin);
	}

	template<class T>
	AnalogInput<T>::AnalogInput(pin_t pin, callback_type callback, std::initializer_list<Range*> il, bool match_any) :
		pin_(pin), value_(), callback_(callback),
		ranges_(const_cast<Range**>(il.begin()), il.size()), 
		current_(std::end(ranges_)), match_any_(ranges_.size() == 0 || match_any)
	{
		std::size_t i = 0;

		for (auto j : il)
			*ranges_[i++] = *j;
	}

	template<class T>
	void AnalogInput<T>::attach(pin_t pin)
	{
		setPinMode(pin);
	}

	template<class T>
	pin_t AnalogInput<T>::attach() const
	{
		return pin_;
	}

	template<class T>
	template<std::size_t N>
	void AnalogInput<T>::ranges(Range* (&ranges)[N])
	{
		ranges_ = container_type(ranges);
		current_ = std::end(ranges_);
		matchAny(match_any_);
	}

	template<class T>
	void AnalogInput<T>::ranges(Range* ranges[], std::size_t n)
	{
		ranges_ = container_type(ranges, n);
		current_ = std::end(ranges_);
		matchAny(match_any_);
	}

	template<class T>
	void AnalogInput<T>::ranges(Range** first, Range** last)
	{
		ranges_ = container_type(first, last);
		current_ = std::end(ranges_);
		matchAny(match_any_);
	}

	template<class T>
	void AnalogInput<T>::ranges(std::initializer_list<Range*> il)
	{
		ranges_ = container_type(const_cast<Range**>(il.begin()), il.size());
		std::size_t i = 0;

		for (auto j : il)
			*ranges_[i++] = *j;
		current_ = std::end(ranges_);
		matchAny(match_any_);
	}

	template<class T>
	void AnalogInput<T>::ranges(container_type& ranges)
	{
		ranges_ = ranges;
		current_ = std::end(ranges_);
		matchAny(match_any_);
	}

	template<class T>
	const typename AnalogInput<T>::container_type& AnalogInput<T>::ranges() const
	{
		return ranges_;
	}

	template<class T>
	void AnalogInput<T>::matchAny(bool value)
	{
		match_any_ = ranges_.size() == 0 || value;
	}

	template<class T>
	bool AnalogInput<T>::matchAny() const
	{
		return match_any_;
	}

	template<class T>
	void AnalogInput<T>::callback(callback_type callback)
	{
		callback_ = callback;
	}

	template<class T>
	typename AnalogInput<T>::value_type AnalogInput<T>::operator()()
	{
		return (value_ = analogRead(pin_));
	}

	template<class T>
	typename AnalogInput<T>::value_type AnalogInput<T>::value() const
	{
		return value_;
	}

	template<class T>
	typename AnalogInput<T>::Range* AnalogInput<T>::range() const
	{
		return *current_;
	}

	template<class T>
	void AnalogInput<T>::poll()
	{
		iterator i = read_input();
		Serial.print("poll");
		if (match_any_ || i != current_) 
		{
			current_ = i;
			doCallback(); // Callback if match any set or new match.
		}
	}

	template<class T>
	void AnalogInput<T>::setPinMode(pin_t pin)
	{
		pinMode(pin, INPUT);
		pin_ = pin;
	}

	template<class T>
	typename AnalogInput<T>::iterator AnalogInput<T>::read_input()
	{
		iterator i = std::end(ranges_);

		operator()();
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

	template<class T>
	void AnalogInput<T>::doCallback()
	{
		if (callback_)
			(*callback_)();
	}

	template<class T>
	void AnalogInput<T>::clock()
	{
		poll();
	}

#pragma endregion
#pragma region non-member_funcs

	template<class T>
	inline bool operator==(const AnalogInput<T>& lhs, const AnalogInput<T>& rhs)
	{
		return lhs.value() == rhs.value();
	}

	template<class T>
	inline bool operator!=(const AnalogInput<T>& lhs, const AnalogInput<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class T>
	inline bool operator<(const AnalogInput<T>& lhs, const AnalogInput<T>& rhs)
	{
		return lhs.value() < rhs.value();
	}

	template<class T>
	inline bool operator>(const AnalogInput<T>& lhs, const AnalogInput<T>& rhs)
	{
		return (rhs < lhs);
	}

	template<class T>
	inline bool operator<=(const AnalogInput<T>& lhs, const AnalogInput<T>& rhs)
	{
		return !(rhs < lhs);
	}

	template<class T>
	inline bool operator>=(const AnalogInput<T>& lhs, const AnalogInput<T>& rhs)
	{
		return !(lhs < rhs);
	}

#pragma endregion
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_ANALOG_INPUT_H
