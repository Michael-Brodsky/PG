/*
 *	Typedefs file for <Thermometer.ino>
 *
 *	***************************************************************************
 *
 *	File: Thermometer.h
 *	Date: October 15, 2021
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

#if !defined __PG_THERMOMETER_H
# define __PG_THERMOMETER_H 20211015L

# include <lib/thermo.h>				// Thermometer temperature maths.
# include <interfaces/iserializable.h>
# include <components/AnalogKeypad.h>	// Async keypad polling.
# include <components/LCDDisplay.h>		// Async display manager.
# include <components/AnalogInput.h>	// Async analog input polling.
# include <utilities/MovingAverage.h>	// Moving average filter type.
# include <utilities/EEStream.h>		// EEPROM streaming support.
# include <utilities/TaskScheduler.h>	// Async task scheduling support.

using namespace pg;
using namespace std::chrono;

// Thermometer display settings type.
template<class T>
class DisplayType : public iserializable
{
public:
	using value_type = T;
	using convert_type = typename callback<value_type, void, value_type>::type;
	using symbol_type = char;
	using unit_type = std::pair<convert_type, symbol_type>;

	value_type		low;	// Temperature display range low.
	value_type		high;	// Temperature display range high,
	unit_type		units;	// Temperature display units convert/symbol.

	DisplayType() = default;
	DisplayType(value_type lo, value_type hi, unit_type un) : low(lo), high(hi), units(un) {}

	convert_type& unitConvert() { return units.first; }
	const convert_type& unitConvert() const { return units.first; }
	symbol_type& unitSymbol() { return units.second; }
	const symbol_type& unitSymbol() const { return units.second; }

	void serialize(EEStream& e) const override { e << low; e << high, e << unitSymbol(); }
	void deserialize(EEStream& e) override { e >> low; e >> high; e >> unitSymbol(); }
};

// Thermometer alarm settings type.
template<class T>
class AlarmType : public iserializable
{
public:
	using value_type = T;
	using symcmp_type = char;
	using symen_type = char;
	using return_type = bool;
	using function_type = typename callback<return_type, void, value_type, value_type>::type;
	using enable_type = std::pair<return_type, symen_type>;
	using compare_type = std::pair<function_type, symcmp_type>;

	pin_t			pin;		// Alarm signal output pin.
	enable_type		enabled;	// Alarm enabled/disabled.
	compare_type	comp;		// Alarm compare < or >.
	value_type		setpoint;	// Alarm temperature set point.

	AlarmType() = default;
	AlarmType(pin_t p, enable_type en, compare_type cmp, value_type sp) :
		pin(p), enabled(en), comp(cmp), setpoint(sp) {}

	return_type& enable() { return enabled.first; }
	const return_type& enable() const { return enabled.first; }
	function_type& compare() { return comp.first; }
	const function_type& compare() const { return comp.first; }
	symen_type& enableSymbol() { return enabled.second; }
	const symen_type& enableSymbol() const { return enabled.second; }
	symcmp_type& cmpSymbol() { return comp.second; }
	const symcmp_type& cmpSymbol() const { return comp.second; }

	void attach() { pinMode(pin, OUTPUT); }
	void silence(bool value) { digitalWrite(pin, !value); }
	void serialize(EEStream& e) const override { e << enableSymbol(); e << cmpSymbol(); e << setpoint; }
	void deserialize(EEStream& e) override { e >> enableSymbol(); e >> cmpSymbol(); e >> setpoint; }
};

// Temperature sensor settings type.
template<class Duration>
class SensorType : public iserializable
{
public:
	enum class ArefSource
	{
		Internal = 0,	// Aref sources is internal.
		External		// Aref sources is external.
	};
	using duration = Duration;
	using symbol_type = const char*;
	using aref_type = std::pair<ArefSource, symbol_type>;

	aref_type		aref;	// Aref source/symbol. 
	duration		tpoll;	// Sensor polling interval.

	SensorType() = default;
	SensorType(aref_type a, duration d) : aref(a), tpoll(d) {}

	ArefSource& arefSource() { return aref.first; }
	const ArefSource& arefSource() const { return aref.first; }
	symbol_type& arefSymbol() { return aref.second; }
	symbol_type& arefSymbol() const { return aref.second; }

	void setAref() { analogReference(aref.first == ArefSource::Internal ? DEFAULT : EXTERNAL); }
	void serialize(EEStream& e) const override { e << arefSource(); e << tpoll.count(); }
	void deserialize(EEStream& e) override
	{
		e >> arefSource();
		typename duration::rep r;
		e >> r;
		tpoll = duration(r);
	}
};

// Type that aggregates display, alarm and sensor settings.
template<class T, class Duration>
struct ThermometerType
{
	using value_type = T;
	using duration = Duration;
	using display_type = DisplayType<value_type>;
	using alarm_type = AlarmType<value_type>;
	using sensor_type = SensorType<duration>;

	display_type	display;		// Active display settings.
	alarm_type		alarm;			// Active alarm settings.
	sensor_type		sensor;			// Active sensor settings.
	display_type	display_copy;	// Display settings copy for editting.
	alarm_type		alarm_copy;		// Alarm settings copy for editting.
	sensor_type		sensor_copy;	// Sensor settings copy for editting.

	void copySettings()
	{
		display_copy = display;
		alarm_copy = alarm;
		sensor_copy = sensor;
	}

	void updateSettings()
	{
		if (alarm_copy.setpoint < display_copy.low)
			alarm_copy.setpoint = display_copy.low;
		else if (alarm_copy.setpoint > display_copy.high)
			alarm_copy.setpoint = display_copy.high;
		display = display_copy;
		alarm = alarm_copy;
		sensor = sensor_copy;
	}
};

// Multiplies the adjustment factor for faster scrolling. When the <Up> or <Down> 
// buttons are pressed and held down in one of the editing modes, the values 
// will rapidly increase/decrease as long as the button remains pressed. The 
// `AdjustmentType' type multiplies the rate of increase/decrease by a factor 
// of 11 each time it is clocked, up to a limit. This allows large adjustment 
// ranges to be scrolled through both finely and coarsely, without skipping values 
// or having to wait forever.
class AdjustmentType : public iclockable
{
public:
	enum class Direction { Up = 0, Down };
	using factor_type = unsigned;

public:
	AdjustmentType(unsigned mulmax) : mul_(1), mulmax_(mulmax) {}

public:
	void reset() { mul_ = 1; }
	template<class T>
	typename std::make_signed<T>::type value(T v, Direction dir)
	{
		typename std::make_signed<T>::type val = v * mul_;

		return dir == Direction::Up ? val : -val;
	}
	
private:
	void clock() override { mul_ *= mul_ < mulmax_ ? 11 : 1; }

private:
	factor_type	mul_;		// The adjustment factor multiplier.
	factor_type	mulmax_;	// The maximum multiplication factor.
};

#endif // !defined __PG_THERMOMETER_H
