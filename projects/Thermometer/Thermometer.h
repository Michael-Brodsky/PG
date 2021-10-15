/*
 *	Type definitions file for <Thermometer.ino>
 *
 *	***************************************************************************
 *
 *	File: Thermometer.h
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

# include <lib/fmath.h>
# include <lib/units.h>
# include <components/AnalogKeypad.h>
# include <components/LCDDisplay.h>
# include <components/AnalogInput.h>
# include <utilities/EEStream.h>
# include <utilities/TaskScheduler.h>
# include <utilities/MovingAverage.h>

using namespace pg;
using namespace std::chrono;

// Returns the temperature, in degrees K, represented by a voltage present 
// at an analog input.
template<class T, class U>
T tsense(U ain, U amax)
{
	// The following assume a 10K thermistor forming a voltage divider with 
	// known resistor R, connected to a common-emitter amplifier having a 
	// base/emitter voltage drop Vbe. Vss is the supply voltage. Vsense is 
	// the voltage divider node voltage, computed from the amplifier output 
	// voltage as converted to ain by the analog-to-digital converter (ADC). 
	// The temperature in degrees Kelvin is found by evaluating the Steinhart-
	// Hart equation for the thermistor's resistance in the voltage divider  
	// network rsense. If a current amplifier is not used, set Vbe = 0.
	const T a = 1.125e-3, b = 2.347e-4, c = 8.566e-8;	// Steinhart-Hart coefficients for 10K thermistor.
	const T R = 10030.0, Vss = 4.97, Vbe = 0.6;			// Measured voltage divider parameters.
	const T Vsense = static_cast<T>(ain) / static_cast<T>(amax) * Vss + Vbe; // Voltage represented by ADC value ain.

	return hart(rsense(Vsense, Vss, R), a, b, c);
}

//
// Alarm compare functions.
//

template<class T>
bool alarm_lt(T lhs, T rhs) { return lhs < rhs; }
template<class T>
bool alarm_gt(T lhs, T rhs) { return lhs > rhs; }

//
// Temperature conversion objects.
//

namespace
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
	return temperature_impl<T, Units>()(value);
};

//
// Display symbols constants.
//

const char DegreeSymbol = 0xDF;
const char FarenheitSymbol = 'F';
const char CelsiusSymbol = 'C';
const char KelvinSymbol = 'K';
const char YesSymbol = 'Y';
const char NoSymbol = 'N';
const char LessSymbol = '<';
const char GreaterSymbol = '>';
const char* InternalSymbol = "IN";
const char* ExternalSymbol = "EX";

//
// Editable/serializable thermometer settings types.
//

// Thermometer display settings.
template<class T>
class DisplayType : public iserializable
{
public:
	using value_type = T;
	using unit_type = std::pair<typename callback<value_type, void, value_type>::type, char>;

	value_type		low;	// Temperature display range low.
	value_type		high;	// Temperature display range high,
	unit_type		units;	// Temperature display units (C,F,K).

	DisplayType() = default;
	DisplayType(value_type lo, value_type hi, unit_type un) : low(lo), high(hi), units(un) {}

	void serialize(EEStream& e) const override { e << low; e << high, e << units.second; }
	void deserialize(EEStream& e) override
	{
		e >> low; e >> high; e >> units.second;
		units.first = units.second == FarenheitSymbol
			? (typename unit_type::first_type)temperature<units::fahrenheit>
			: units.second == CelsiusSymbol
			? (typename unit_type::first_type)temperature<units::celsius>
			: (typename unit_type::first_type)temperature<units::kelvin>;
	}
};

// Thermometer alarm settings.
template<class T>
class AlarmType : public iserializable
{
public:
	using value_type = T;
	using enable_type = std::pair<bool, char>;
	using compare_type = std::pair<typename callback<bool, void, value_type, value_type>::type, char>;

	pin_t			pin;		// Alarm signal output pin.
	enable_type		enabled;	// Alarm enabled/disabled.
	compare_type	compare;	// Alarm compare < or >.
	value_type		setpoint;	// Alarm temperature set point.

	AlarmType() = default;
	AlarmType(pin_t p, enable_type en, compare_type cmp, value_type sp) :
		pin(p), enabled(en), compare(cmp), setpoint(sp) {}

	void attach() { pinMode(pin, OUTPUT); }
	void silence(bool value) { digitalWrite(pin, !value); }
	void serialize(EEStream& e) const override { e << enabled.second; e << compare.second; e << setpoint; }
	void deserialize(EEStream& e) override
	{
		e >> enabled.second; e >> compare.second; e >> setpoint;
		enabled.first = enabled.second == YesSymbol
			? true
			: false;
		compare.first = compare.second == LessSymbol
			? (typename compare_type::first_type)alarm_lt
			: (typename compare_type::first_type)alarm_gt;
	}
};

// Temperature sensor settings.
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
	using aref_type = std::pair<ArefSource, const char*>;

	aref_type		aref;	// Analog reference voltage source.
	duration		tpoll;	// Sensor polling interval.

	SensorType() = default;
	SensorType(aref_type a, duration d) : aref(a), tpoll(d) {}

	void setAref() { analogReference(aref.first == ArefSource::Internal ? DEFAULT : EXTERNAL); }
	void serialize(EEStream& e) const override { e << aref.first; e << tpoll.count(); }
	void deserialize(EEStream& e) override
	{
		e >> aref.first;
		aref.second = aref.first == ArefSource::Internal
			? InternalSymbol
			: ExternalSymbol;
		typename duration::rep r;
		e >> r;
		tpoll = duration(r);
	}
};

// Aggregates display, alarm and sensor settings.
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
template<class DisplayType, class SensorType>
class AdjustmentType : public iclockable
{
public:
	enum class Direction { Up = 0, Down };
	using display_type = DisplayType;
	using sensor_type = SensorType;
	using factor_type = unsigned;
	using displaydiff_type = typename std::make_signed<display_type>::type;
	using sensordiff_type = typename std::make_signed<sensor_type>::type;

public:
	AdjustmentType(sensor_type sval, display_type dval, unsigned mulmax) :
		sval_(sval), dval_(dval), mul_(1), mulmax_(mulmax) {}

public:
	void				reset() { mul_ = 1; }
	displaydiff_type	value(display_type, Direction dir)
	{
		displaydiff_type value_ = dval_ * mul_; return dir == Direction::Up ? value_ : -value_;
	}
	sensordiff_type		value(sensor_type, Direction dir)
	{
		sensordiff_type value_ = sval_ * mul_; return dir == Direction::Up ? value_ : -value_;
	}

private:
	void	clock() override { mul_ *= mul_ < mulmax_ ? 11 : 1; }

private:
	sensor_type		sval_;		// The smallest (fine) adjustment factor for integer types.
	display_type	dval_;		// The smallest (fine) adjustment factor for float types.
	factor_type		mul_;		// The adjustment factor multiplier.
	factor_type		mulmax_;	// The maximum multiplication factor.
};

// Enumerates valid thermometer operating modes.
enum class ThermometerMode
{
	Init = 0,	// Initial state.
	Run,		// Displays current temperature and alarm settings.
	Menu,		// Presents a menu of operating mode choices. 
	Alarm,		// Displays/edits alarm settings.
	Sensor,		// Displays/edits sensor settings.
	Display		// Displays/edits display settings.
};

#endif // !defined __THERMOMETER_H 