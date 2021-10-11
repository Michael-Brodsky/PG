/*
 *	Config file for Thermometer.ino.
 *
 *	***************************************************************************
 *
 *	File: Thermometer.h
 *	Date: October 9, 2021
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
# define __PG_THERMOMETER_H 20211005L

#include <lib/fmath.h>
#include <lib/units.h>
#include <utilities/Timer.h>
#include <components/AnalogKeypad.h>
#include <components/LCDDisplay.h>
#include <components/AnalogInput.h>
#include <utilities/TaskScheduler.h>
#include <utilities/MovingAverage.h>

using namespace pg;
using namespace std::chrono;

// Alarm compare functions.

template<class T>
bool alarm_lt(T lhs, T rhs) { return lhs < rhs; }
template<class T>
bool alarm_gt(T lhs, T rhs) { return lhs > rhs; }

// Type that maps ADC values to display values.
template<class From, class To>
struct datamap_t
{
	static To map(From x, From xmin, From xmax, To ymin, To ymax)
	{
		assert(xmax != xmin);

		return (To)((ymax - ymin) / (static_cast<To>(xmax) - static_cast<To>(xmin)) *
			(static_cast<To>(x) - static_cast<To>(xmax)) + ymax);
	}
};

// Type that stores sensor settings.
template<class T, class Duration>
struct sensor_t
{
	using value_type = T;
	using duration_type = Duration;

	value_type		low_;	// Lowest sensor ADC value (see below).
	value_type		high_;	// Highest sensor ADC value (see below).
	duration_type	tmin_;	// Minimum sensor polling interval.
	duration_type	tmax_;	// Maximum sensor polling interval.
	duration_type	tpoll_;	// Current sensor polling interval.
};

// Type that stores display settings.
template<class T>
struct display_t
{
	using value_type = T;

	value_type	min_;	// Absolute minimum displayable temperature.
	value_type	max_;	// Absolute maximum displayable temperature.
	value_type	low_;	// Displayed temperature at sensor_t::low_ (see below).
	value_type	high_;	// Displayed temperature at sensor_t::high_ (see below).
	char		unit_;	// Temparature display unit.
};

// Temperature Data Mapping
// 
// The range of temperatures that can be sensed and displayed is set by the 
// sensor_t::low_, sensor_t::high_ and display_t::low_, display_t::high_ 
// values, and need to be calibrated for the type of sensor being used.
// The sensed temperature is converted to an integer value by the ADC and 
// then mapped to a corresponding floating-point value for display. For 
// instance if a particular sensor can output ADC values from 100 to 1000 
// over its usable range and these values correspond to temperatures of 
// 0.0 and 100.0 degrees respectively, then any sensor value will be 
// mapped to the corresponding temperature thus: 
//
//	Sensor	Display
//	100		0.0
//	550		50.0
//	1000	100.0

// Type that stores alarm settings.
template<class DispT, class SenseT>
struct alarm_t
{
	using display_type = DispT;
	using sensor_type = SenseT;
	using enabled_type = std::pair<bool, char>;
	using compare_type = std::pair<typename callback<bool, void, sensor_type, sensor_type>::type, char>;

	pin_t			pin_;			// Pin number that outputs the alarm signal.
	enabled_type	enabled_;		// Flag indicating whether the alarm is currently enabled.
	compare_type	cmp_;			// Alarm compare function.
	sensor_type		set_point_;		// Alarm set point, ADC value (see above).
	display_type	display_value_;	// Alarm set point, temperature value.
	bool			active_;		// Flag indicating whether the alarm is currently active.
	void			silence(bool value) { digitalWrite(pin_, !value); } // Outputs the alarm control signal.
};

// Type used to calibrate the sensor and set the ADC reference source.
template<class T>
struct calibrate_t
{
	enum class ArefSource
	{
		Internal = 0,
		External
	};
	using aref_type = std::pair<ArefSource, const char*>;
	using value_type = T;

	value_type	value_;		// The currently polled sensor ADC value.
	bool		set_low_;	// Flag indicating whether the sensor_t::low_ value is updated.
	bool		set_high_;	// Flag indicating whether the sensor_t::high_ value is updated.
	aref_type	aref_;		// The current analog voltage reference source.
};
 
// Multiplies the adjustment factor for faster scrolling. When the <Up> or <Down> 
// buttons are pressed and held down in one of the editing modes, the values 
// will rapidly increase/decrease as long as the button stays pressed. The 
// adjustment_factor type multiplies the rate of increase/decrease by a factor 
// of 11 each time it is clocked, up to a limit. This allows large adjustment 
// ranges to be scrolled through both finely and coarsely, without skipping values 
// or having to wait forever.
template<class DisplayType, class SensorType>
class adjustment_factor : public iclockable 
{
public:
	enum class Direction { Up = 0, Down };
	using display_type = DisplayType;
	using sensor_type = SensorType;
	using displaydiff_type = typename std::make_signed<display_type>::type;
	using sensordiff_type = typename std::make_signed<sensor_type>::type;

public:
	adjustment_factor(sensor_type sval, display_type dval, unsigned mulmax) :
		sval_(sval), dval_(dval), mul_(1), mulmax_(mulmax) {}

public:
	void				reset() { mul_ = 1; }
	displaydiff_type	value(display_type, Direction dir) 
	{ displaydiff_type value_ = dval_ * mul_; return dir == Direction::Up ? value_ : -value_; }
	sensordiff_type		value(sensor_type, Direction dir) 
	{ sensordiff_type value_ = sval_ * mul_; return dir == Direction::Up ? value_ : -value_; }

private:
	void	clock() override { mul_ *= mul_ < mulmax_ ? 11 : 1; } 

private:
	sensor_type		sval_;		// The smallest (fine) adjustment factor for ADC types.
	display_type	dval_;		// The smallest (fine) adjustment factor for display types.
	unsigned		mul_;		// The factor multiplier.
	unsigned		mulmax_;	// The maximum multiplication factor.
};

// Enumerates valid thermometer operating modes.
enum class OpMode
{
	Init = 0,	// Initial state.
	Run,		// Displays current temperature and alarm settings.
	Menu,		// Presents a menu of operating mode choices. 
	Alarm,		// Displays/edits alarm settings.
	Sensor,		// Displays/edits sensor settings.
	Calibrate,	// Displays/edits calibration settings.
	Display		// Displays/edits display settings.
};

// 
// Program type aliases.
//
using Display = LCDDisplay<16, 2>;
using Scheduler = TaskScheduler<>;
using TempSensor = AnalogInput;
using SensorSettings = sensor_t<analog_t, milliseconds>;
using TempFilter = MovingAverage<SensorSettings::value_type, 4>;
using DisplaySettings = display_t<float>;
using AlarmSettings = alarm_t<DisplaySettings::value_type, SensorSettings::value_type>;
using CalibrationSettings = calibrate_t<SensorSettings::value_type>;
using MapToSensor = datamap_t<DisplaySettings::value_type, SensorSettings::value_type>;
using MapToDisplay = datamap_t<SensorSettings::value_type, DisplaySettings::value_type>;
using Adjustment = adjustment_factor<DisplaySettings::value_type, SensorSettings::value_type>;

//
// Hardware interfacing constants
//
const pin_t KeypadInputPin = 0;
const pin_t SensorInputPin = A7;
const pin_t AlarmOutputPin = 53;
const pin_t LcdRs = 8;
const pin_t LcdEn = 9;
const pin_t LcdD4 = 4;
const pin_t LcdD5 = 5;
const pin_t LcdD6 = 6;
const pin_t LcdD7 = 7;

//
// Thermometer settings default values.
//
const Keypad::LongPress KeypadLongPressMode = Keypad::LongPress::Hold;
const milliseconds KeypadLongPressInterval = seconds(1);
const analog_t RightButtonTriggerLevel = 60;
const analog_t UpButtonTriggerLevel = 200;
const analog_t DownButtonTriggerLevel = 400;
const analog_t LeftButtonTriggerLevel = 600;
const analog_t SelectButtonTriggerLevel = 800;

const milliseconds SensorPollingMin = milliseconds(100);
const milliseconds SensorPollingMax = milliseconds(9999);
const milliseconds SensorPollingInterval = seconds(1);
const milliseconds KeypadPollingInterval = milliseconds(100);
const milliseconds DisplayRefreshInterval = milliseconds(100);
const milliseconds AdjustmentMultiplyInterval = seconds(4);

const unsigned AdjustmentMultiplyMax = 100;
const DisplaySettings::value_type DisplayAdjustmentFactor = 0.1;
const SensorSettings::value_type SensorAdjustmentFactor = 1;
const DisplaySettings::value_type DisplayValueMin = -999.9;
const DisplaySettings::value_type DisplayValueMax = +999.9;
const DisplaySettings::value_type DisplayRangeLow = 0.0;
const DisplaySettings::value_type DisplayRangeHigh = 100.0;
const SensorSettings::value_type SensorRangeLow = 214;
const SensorSettings::value_type SensorRangeHigh = 1023;
const AlarmSettings::sensor_type SensorAlarmSetPoint = 0;

//
// Display fields and screen formatting constants.
//

// RUN screen formatting

/******************
 * Temp:  -nnn.m*F*
 *Alarm: <  -nnn.m*		
 ******************/
const char* RunScreenLab = " Temp:";

const uint8_t TempCol = 8;
const uint8_t TempRow = 0;
const char* TempLab = "";
const char* TempFmt = "%6.1f";

const uint8_t SymbolCol = 14;
const uint8_t SymbolRow = 0;
const char* SymbolLab = "";
const char* SymbolFmt = "%c";

const uint8_t UnitCol = 15;
const uint8_t UnitRow = 0;
const char* UnitLab = "";
const char* UnitFmt = "%c";

const uint8_t Alarm1Col = 0;
const uint8_t Alarm1Row = 1;
const char* Alarm1Lab = "Alarm:";
const char* Alarm1Fmt = "%c";

const uint8_t Alarm2Col = 8;
const uint8_t Alarm2Row = 1;
const char* Alarm2Lab = "";
const char* Alarm2Fmt = "%6.1f";

// MENU screen formatting

/******************
 *MENU: Run Alarm *
 *Sense Cal Disply*
 ******************/
const char* MenuScreenLab = "MENU";

const char* MenuItemRun = "Run";
const char* MenuItemAlarm = "Alarm";
const char* MenuItemSensor = "Sense";
const char* MenuItemCalibrate = "Cal";
const char* MenuItemDisplay = "Disply";

const uint8_t MenuRunCol = 6;
const uint8_t MenuRunRow = 0;
const char* MenuRunLab = "";
const char* MenuRunFmt = "%s";

const uint8_t MenuSensorCol = 0;
const uint8_t MenuSensorRow = 1;
const char* MenuSensorLab = "";
const char* MenuSensorFmt = "%s";

const uint8_t MenuAlarmCol = 10;
const uint8_t MenuAlarmRow = 0;
const char* MenuAlarmLab = "";
const char* MenuAlarmFmt = "%s";

const uint8_t MenuCalCol = 6;
const uint8_t MenuCalRow = 1;
const char* MenuCalLab = "";
const char* MenuCalFmt = "%s";

const uint8_t MenuDisplayCol = 10;
const uint8_t MenuDisplayRow = 1;
const char* MenuDisplayLab = "";
const char* MenuDisplayFmt = "%s";

// ALARM screen formatting

/******************
 *ALARM  En:Y     *
 *Cmp:<  Sp:-nnn.m*
 ******************/
const char* AlarmScreenLab = "ALARM";

const uint8_t AlarmEnableCol = 7;
const uint8_t AlarmEnableRow = 0;
const char* AlarmEnableLab = "En:";
const char* AlarmEnableFmt = "%c";

const uint8_t AlarmCmpCol = 0;
const uint8_t AlarmCmpRow = 1;
const char* AlarmCmpLab = "Cmp:";
const char* AlarmCmpFmt = "%c";

const uint8_t AlarmSetPointCol = 7;
const uint8_t AlarmSetPointRow = 1;
const char* AlarmSetPointLab = "Sp:";
const char* AlarmSetPointFmt = "%6.1f";

const AlarmSettings::enabled_type AlarmDisabled{ false,'N' };
const AlarmSettings::enabled_type AlarmEnabled{ true,'Y' };
const AlarmSettings::compare_type AlarmCmpLess{ alarm_lt ,'<' };
const AlarmSettings::compare_type AlarmCmpGreater{ alarm_gt ,'>' };

// SENSOR screen formatting

/******************
 *SENSOR: Lo:nnnnn*
 *Tp:nnnn Hi:nnnnn*
 ******************/
const char* SensorScreenLab = "SENSOR";

const uint8_t SensorLowCol = 8;
const uint8_t SensorLowRow = 0;
const char* SensorLowLab = "Lo:";
const char* SensorLowFmt = "%5u";

const uint8_t SensorHighCol = 8;
const uint8_t SensorHighRow = 1;
const char* SensorHighLab = "Hi:";
const char* SensorHighFmt = "%5u";

const uint8_t SensorPollCol = 0;
const uint8_t SensorPollRow = 1;
const char* SensorPollLab = "Tp:";
const char* SensorPollFmt = "%4u";

// CALIBRATION screen formatting

/******************
 *nnnnn   Lo:nnnnn*
 *Aref:IN Hi:nnnnn*
 ******************/
const char* CalScreenLab = "->";

const uint8_t CalSenseCol = 2;
const uint8_t CalSenseRow = 0;
const char* CalSenseLab = "";
const char* CalSenseFmt = "%5u";

const uint8_t CalLowCol = 8;
const uint8_t CalLowRow = 0;
const char* CalLowLab = "Lo:";
const char* CalLowFmt = "%5u";

const uint8_t CalArefCol = 0;
const uint8_t CalArefRow = 1;
const char* CalArefLab = "Aref:";
const char* CalArefFmt = "%2s";

const uint8_t CalHighCol = 8;
const uint8_t CalHighRow = 1;
const char* CalHighLab = "Hi:";
const char* CalHighFmt = "%5u";

const CalibrationSettings::aref_type ArefInternal{ CalibrationSettings::ArefSource::Internal, "IN" };
const CalibrationSettings::aref_type ArefExternal{ CalibrationSettings::ArefSource::External, "EX" };

// DISPLAY screen formatting

/******************
 *DISPL  Lo:-nnn.m*
 *Un:F   Hi:-nnn.m*
 ******************/
const char* DisplayScreenLab = "DISPL";

const uint8_t DisplayLowCol = 7;
const uint8_t DisplayLowRow = 0;
const char* DisplayLowLab = "Lo:";
const char* DisplayLowFmt = "%6.1f";

const uint8_t DisplayHighCol = 7;
const uint8_t DisplayHighRow = 1;
const char* DisplayHighLab = "Hi:";
const char* DisplayHighFmt = "%6.1f";

const uint8_t DisplayUnitCol = 0;
const uint8_t DisplayUnitRow = 1;
const char* DisplayUnitLab = "Un:";
const char* DisplayUnitFmt = "%c";

//
// Symbols constants
//

const char DegreeSymbol = 0xDF;
const char FarenheitSymbol = 'F';
const char CelsiusSymbol = 'C';
const char KelvinSymbol = 'K';

#endif // !defined __PG_THERMOMETER_H
