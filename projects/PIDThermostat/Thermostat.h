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

#if !defined __PG_THERMOSTAT_H
# define __PG_THERMOSTAT_H 20211015L

# include <lib/thermo.h>				// Temperature maths.
# include <lib/setting.h>				// Program setting pair type.
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

using data_t = float; // Thermostat floating point value type.
using TemperatureSensor = AnalogInput<analog_t>;
using InputFilter = MovingAverage<TemperatureSensor::value_type, 4>;
using Display = LCDDisplay<16, 2>;
using Pwm = PWMOutput<data_t>;
using Pid = PIDController<data_t>;
using Keypad = AnalogKeypad<analog_t>;
using Scheduler = TaskScheduler<>;
using Adjustment = Keypad::Multiplier;

using ScheduledTask = Scheduler::Task;
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

using temp_units_t = setting<
	typename callback<data_t, void, data_t>::type,
	char>;
using alarm_compare_t = setting<
	typename callback<bool, void, data_t, data_t>::type,
	char>;
using alarm_enable_t = setting<bool, char>;
using sp_enable_t = setting<bool, char>;
using sensor_aref_t = setting<ArefSource, const char*>;


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

class Settings : public iserializable
{
public:
	Settings() = default;

public:
	data_t& temperatureLow() { return temp_low_; }
	const data_t& temperatureLow() const { return temp_low_; }
	data_t& temperatureHigh() { return temp_high_; }
	const data_t& temperatureHigh() const { return temp_high_; }
	temp_units_t& temperatureUnits() { return temp_units_; }
	const temp_units_t& temperatureUnits() const { return temp_units_; }
	sp_enable_t& setpointEnable() { return setpoint_enable_; }
	const sp_enable_t& setpointEnable() const { return setpoint_enable_; }
	data_t& setpointValue() { return setpoint_value_; }
	const data_t& setpointValue() const { return setpoint_value_; }
	alarm_compare_t& alarmCompare() { return alarm_compare_; }
	const alarm_compare_t& alarmCompare() const { return alarm_compare_; }
	alarm_enable_t& alarmEnable() { return alarm_enable_; }
	const alarm_enable_t& alarmEnable() const { return alarm_enable_; }
	data_t& alarmSetpoint() { return alarm_setpoint_; }
	const data_t& alarmSetpoint() const { return alarm_setpoint_; }
	Pwm::Range& pwmRange() { return pwm_range_; }
	const Pwm::Range& pwmRange() const { return pwm_range_; }
	Pid::value_type& pidProportional() { return pid_p_; }
	const Pid::value_type& pidProportional() const { return pid_p_; }
	Pid::value_type& pidIntegral() { return pid_i_; }
	const Pid::value_type& pidIntegral() const { return pid_i_; }
	Pid::value_type& pidDerivative() { return pid_d_; }
	const Pid::value_type& pidDerivative() const { return pid_d_; }
	Pid::value_type& pidGain() { return pid_a_; }
	const Pid::value_type& pidGain() const { return pid_a_; }
	sensor_aref_t& sensorAref() { return sensor_aref_; }
	const sensor_aref_t& sensorAref() const { return sensor_aref_; }
	milliseconds& sensorPollIntvl() { return sensor_tpoll_; }
	const milliseconds& sensorPollIntvl() const { return sensor_tpoll_; }

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
		e << temperatureLow(); e << temperatureHigh(), e << temperatureUnits().display_value();
		e << pidProportional(); e << pidIntegral(); e << pidDerivative(); e << pidGain();
		e << setpointEnable().display_value(); e << setpointValue();
		e << alarmEnable().display_value();	e << alarmCompare().display_value(); e << alarmSetpoint();
		e << sensorAref().value(); e << sensorPollIntvl().count();
		e << pwmRange().low(); e << pwmRange().high();
	}

	void deserialize(EEStream& e) override
	{
		e >> temperatureLow(); e >> temperatureHigh(), e >> temperatureUnits().display_value();
		e >> pidProportional(); e >> pidIntegral(); e >> pidDerivative(); e >> pidGain();
		e >> setpointEnable().display_value(); e >> setpointValue();
		e >> alarmEnable().display_value();	e >> alarmCompare().display_value(); e >> alarmSetpoint();
		e >> sensorAref().value(); typename milliseconds::rep r; e >> r; sensorPollIntvl() = milliseconds(r);
		e >> pwmRange().low(); e >> pwmRange().high();

		temperatureUnits().value() = temperatureUnits().display_value() == FarenheitSymbol
			? static_cast<typename temp_units_t::value_type>(temperature<units::fahrenheit>)
			: temperatureUnits().display_value() == CelsiusSymbol
			? static_cast<typename temp_units_t::value_type>(temperature<units::celsius>)
			: static_cast<typename temp_units_t::value_type>(temperature<units::kelvin>);
		alarmEnable().value() = alarmEnable().display_value() == EnabledSymbol
			? true
			: false;
		alarmCompare().value() = alarmCompare().display_value() == LessSymbol
			? static_cast<typename alarm_compare_t::value_type>(alarm_lt)
			: static_cast<typename alarm_compare_t::value_type>(alarm_gt);
		sensorAref().display_value() = sensorAref().value() == ArefSource::Internal
			? "IN"
			: "EX";
		setpointEnable().value() = setpointEnable().display_value() == EnabledSymbol
			? true
			: false;
	}

private:
	data_t			temp_low_;
	data_t			temp_high_;
	temp_units_t	temp_units_;
	sp_enable_t		setpoint_enable_;
	data_t			setpoint_value_;
	alarm_compare_t	alarm_compare_;
	alarm_enable_t	alarm_enable_;
	data_t			alarm_setpoint_;
	Pwm::Range		pwm_range_;
	pid_t			pid_p_;
	pid_t			pid_i_;
	pid_t			pid_d_;
	pid_t			pid_a_;
	sensor_aref_t	sensor_aref_;
	milliseconds	sensor_tpoll_;
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

const sensor_aref_t SensorArefInternal{ {ArefSource::Internal,InternalSymbol} };
const sensor_aref_t SensorArefExternal{ {ArefSource::External,ExternalSymbol} };
const alarm_enable_t AlarmDisabled{ {false,DisabledSymbol} };
const alarm_enable_t AlarmEnabled{ {true,EnabledSymbol} };
const alarm_compare_t AlarmCmpLess{ {alarm_lt,LessSymbol} };
const alarm_compare_t AlarmCmpGreater{ {alarm_gt,GreaterSymbol} };
const sp_enable_t SetpointEnabled{ {true,EnabledSymbol} };
const sp_enable_t SetpointDisabled{ {false,DisabledSymbol} };
const temp_units_t DegreesKelvin{ {temperature<units::kelvin>,KelvinSymbol} };
const temp_units_t DegreesCelsius{ {temperature<units::celsius>,CelsiusSymbol} };
const temp_units_t DegreesFahrenheit{ {temperature<units::fahrenheit>,FarenheitSymbol} };

#pragma endregion
#pragma region Arduino API Interface

void alarmAttach(pin_t pin) { pinMode(pin, OUTPUT); }
void alarmSilence(pin_t pin, bool value) { digitalWrite(pin, !value); }
bool alarmSilence(pin_t pin) { return !digitalRead(pin); }
void sensorSetAref(const ArefSource src) { analogReference(src == ArefSource::Internal ? DEFAULT : EXTERNAL); }

#pragma endregion

#endif // !defined __PG_THERMOSTAT_H