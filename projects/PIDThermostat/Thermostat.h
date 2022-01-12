/*
 *	Typedefs file for <PIDThermostat.ino>
 *
 *	***************************************************************************
 *
 *	File: Thermostat.h
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
//Program size : 27, 202 bytes(used 11 % of a 253, 952 byte maximum) (4.21 secs)
//Minimum Memory Usage : 1199 bytes(15 % of a 8192 byte maximum)
#if !defined __PG_THERMOSTAT_H
# define __PG_THERMOSTAT_H 20211015L

# include <lib/thermo.h>				// Temperature maths.
# include <interfaces/iserializable.h>	// iserializable interface.
# include <components/AnalogKeypad.h>	// Async keypad polling.
# include <components/LCDDisplay.h>		// Async display manager.
# include <components/AnalogInput.h>	// Async analog input polling.
# include <utilities/PIDController.h>	// Process pid-controller.
# include <utilities/PWMOutput.h>		// Process control signal generator.
# include <utilities/MovingAverage.h>	// Moving average filter.
# include <utilities/EEStream.h>		// EEPROM streaming support.
# include <utilities/TaskScheduler.h>	// Async task scheduling.

using namespace pg;
using namespace std::chrono;

#pragma region Program Type Definitions

using temperature_t = float; // Thermostat floating point value type.
using TemperatureSensor = AnalogInput<analog_t>;
using InputFilter = MovingAverage<TemperatureSensor::value_type, 4>;
using Display = LCDDisplay<16, 2>;
using Pwm = PWMOutput<float>;
using Pid = PIDController<float>;
using Keypad = AnalogKeypad<analog_t>;
using Scheduler = TaskScheduler<>;
using ScheduledTask = Scheduler::Task;
using Adjustment = Keypad::Multiplier;

using task_state_t = ScheduledTask::State;
using sensor_t = TemperatureSensor::value_type;
using factor_t = Adjustment::factor_type;
using pid_t = Pid::value_type;
using pwm_t = Pwm::value_type;
using control_t = Pwm::control_type;
using duty_cycle_t = duty_cycle<pwm_t, control_t>;
using longpress_t = Keypad::LongPress;

enum class ArefSource
{
	Internal = 0,	// Aref source is internal.
	External		// Aref source is external.
};

// Program settings/display value types.

using temperature_v = ValueWrapper<temperature_t>;
using pid_v = ValueWrapper<pid_t>;
using sensor_poll_v = ValueWrapper<milliseconds>;
using temp_units_t = StringValueWrapper<
	typename callback<temperature_t, void, temperature_t>::type,
	char>;
using alarm_cmp_t = StringValueWrapper<
	typename callback<bool, void, temperature_t, temperature_t>::type,
	char>;
using alarm_en_t = StringValueWrapper<bool, char>;
using setpoint_en_t = StringValueWrapper<bool, char>;
using sensor_aref_t = StringValueWrapper<ArefSource, const char*>;


#pragma endregion 
#pragma region Program Character and String Constants

const char DegreeSymbol = 0xDF;
const char FarenheitSymbol = 'F';
const char CelsiusSymbol = 'C';
const char KelvinSymbol = 'K';
const char EnabledSymbol = '*';
const char DisabledSymbol = ' ';
const char LessSymbol = '<';
const char GreaterSymbol = '>';
const char* const InternalSymbol = "IN";
const char* const ExternalSymbol = "EX";

const char* const TemperatureDisplayFormat = "%6.1f";	// [-999.9,+999.9]
const char* const TimingDisplayFormat = "%4u";			// [0,9999]
const char* const PidDecimalFormat = "%3.1f";			// [0.0, +9.9]
const char* const PidUnitFormat = "%3.0f";				// [+10, +100]

#pragma endregion

struct Settings : public iserializable
{
	temperature_v	temperatureLow;
	temperature_v	temperatureHigh;
	temp_units_t	temperatureUnits;
	setpoint_en_t	setpointEnable;
	temperature_v	setpointValue;
	alarm_cmp_t		alarmCompare;
	alarm_en_t		alarmEnable;
	temperature_v	alarmSetpoint;
	Pwm::Range		pwmRange;
	pid_v			pidProportional;
	pid_v			pidIntegral;
	pid_v			pidDerivative;
	pid_v			pidGain;
	sensor_aref_t	sensorAref;
	sensor_poll_v	sensorPollIntvl;

	Settings() = default;
	Settings(temperature_v temperature_low, temperature_v temperature_high, temp_units_t temperature_units,
		setpoint_en_t setpoint_enable, temperature_v setpoint_value,
		alarm_cmp_t	alarm_compare, alarm_en_t alarm_enable, temperature_v alarm_setpoint,
		Pwm::Range pwm_range, pid_v pid_proportional, pid_v	pid_integral, pid_v pid_derivative, pid_v pid_gain,
		sensor_aref_t sensor_aref, sensor_poll_v sensor_poll_intvl) : iserializable(),
		temperatureLow(temperature_low), temperatureHigh(temperature_high), temperatureUnits(temperature_units),
		setpointEnable(setpoint_enable), setpointValue(setpoint_value),
		alarmCompare(alarm_compare), alarmEnable(alarm_enable), alarmSetpoint(alarm_setpoint),
		pwmRange(pwm_range),
		pidProportional(pid_proportional), pidIntegral(pid_integral), pidDerivative(pid_derivative), pidGain(pid_gain),
		sensorAref(sensor_aref), sensorPollIntvl(sensor_poll_intvl)
	{}

	void copy(Settings& copy)
	{
		copy = *this;
	}

	void update(const Settings& copy)
	{
		*this = copy;
		if (alarmSetpoint() < temperatureLow())
			alarmSetpoint() = temperatureLow();
		else if (alarmSetpoint() > temperatureHigh())
			alarmSetpoint() = temperatureHigh();
		if (setpointValue() < temperatureLow())
			setpointValue() = temperatureLow();
		else if (setpointValue() > temperatureHigh())
			setpointValue() = temperatureHigh();
	}

	void serialize(EEStream& e) const override
	{
		e << temperatureLow(); e << temperatureHigh(), e << temperatureUnits.string();
		e << pidProportional(); e << pidIntegral(); e << pidDerivative(); e << pidGain();
		e << setpointEnable.string(); e << setpointValue();
		e << alarmEnable.string();	e << alarmCompare.string(); e << alarmSetpoint();
		e << sensorAref.value(); e << sensorPollIntvl().count();
		e << pwmRange.low(); e << pwmRange.high();
	}

	void deserialize(EEStream& e) override
	{
		e >> temperatureLow(); e >> temperatureHigh(), e >> temperatureUnits.string();
		e >> pidProportional(); e >> pidIntegral(); e >> pidDerivative(); e >> pidGain();
		e >> setpointEnable.string(); e >> setpointValue();
		e >> alarmEnable.string();	e >> alarmCompare.string(); e >> alarmSetpoint();
		e >> sensorAref.value(); typename milliseconds::rep r; e >> r; sensorPollIntvl() = milliseconds(r);
		e >> pwmRange.low(); e >> pwmRange.high();

		temperatureUnits.value() = temperatureUnits.string() == FarenheitSymbol
			? static_cast<typename temp_units_t::value_type>(temperature<units::fahrenheit>)
			: temperatureUnits.string() == CelsiusSymbol
			? static_cast<typename temp_units_t::value_type>(temperature<units::celsius>)
			: static_cast<typename temp_units_t::value_type>(temperature<units::kelvin>);
		alarmEnable.value() = alarmEnable.string() == EnabledSymbol
			? true
			: false;
		alarmCompare.value() = alarmCompare.string() == LessSymbol
			? static_cast<typename alarm_cmp_t::value_type>(alarm_lt)
			: static_cast<typename alarm_cmp_t::value_type>(alarm_gt);
		sensorAref.string() = sensorAref.value() == ArefSource::Internal
			? InternalSymbol
			: ExternalSymbol;
		setpointEnable.value() = setpointEnable.string() == EnabledSymbol
			? true
			: false;
	}
};

// Enumerates valid thermostat operating modes.
enum class mode_t
{
	Init = 0,	// Initial state.
	Run,		// Displays current temperature, set point and alarm settings.
	Setpoint,	// Allows setpoint/alarm enable edits in Run mode.
	Menu,		// Presents a menu of operating mode choices.
	Pid,		// Displays/edits pid settings.
	Pwm,		// Displays/edits pwm settings.
	Alarm,		// Displays/edits alarm settings.
	Sensor,		// Displays/edits sensor settings.
	Display		// Displays/edits display settings.
};

#pragma endregion
#pragma region Program Settings Constants

const sensor_aref_t SensorArefInternal(ArefSource::Internal, InternalSymbol);
const sensor_aref_t SensorArefExternal(ArefSource::External, ExternalSymbol);
const alarm_en_t	AlarmDisabled(false, DisabledSymbol);
const alarm_en_t	AlarmEnabled(true, EnabledSymbol);
const alarm_cmp_t	AlarmCmpLess(alarm_lt, LessSymbol);
const alarm_cmp_t	AlarmCmpGreater(alarm_gt, GreaterSymbol);
const setpoint_en_t SetpointEnabled(true, EnabledSymbol);
const setpoint_en_t SetpointDisabled(false, DisabledSymbol);
const temp_units_t	DegreesKelvin(temperature<units::kelvin>, KelvinSymbol);
const temp_units_t	DegreesCelsius(temperature<units::celsius>, CelsiusSymbol);
const temp_units_t	DegreesFahrenheit(temperature<units::fahrenheit>, FarenheitSymbol);

#pragma endregion
#pragma region Arduino API Interface

void alarmAttach(pin_t pin) { pinMode(pin, OUTPUT); }
void alarmSilence(pin_t pin, bool value) { digitalWrite(pin, !value); }
bool alarmSilence(pin_t pin) { return !digitalRead(pin); }
void sensorSetAref(const ArefSource src) { analogReference(src == ArefSource::Internal ? DEFAULT : EXTERNAL); }

#pragma endregion

#endif // !defined __PG_THERMOSTAT_H