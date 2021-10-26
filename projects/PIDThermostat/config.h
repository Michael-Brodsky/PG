/*
 *	Config file for <PIDThermostat.ino>, defines constant values used by the 
 *	program.
 *
 *	***************************************************************************
 *
 *	File: config.h
 *	Date: October 25, 2021
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

#if !defined __PG_THERMOMETER_CONFIG_H
# define __PG_THERMOMETER_CONFIG_H 20211025L

using namespace pg;
using namespace std::chrono;

#pragma region Program Component Types

using Settings = SettingsType<float, analog_t, milliseconds>;
using TemperatureSensor = AnalogInput<Settings::adc_type>;
using InputFilter = MovingAverage<TemperatureSensor::value_type, 4>;
using Pid = PIDController<Settings::display_type>;
using Pwm = PWMOutput<Settings::display_type>;
using Display = LCDDisplay<16, 2>;
using Keypad = AnalogKeypad<Settings::adc_type>;
using Scheduler = TaskScheduler<>;
using Adjustment = Keypad::Multiplier;

#pragma endregion
#pragma region Hardware Properties and Interfacing

const Settings::display_type Ka = 1.125e-3;	// Steinhart-Hart thermistor eqn coeff a.
const Settings::display_type Kb = 2.347e-4;	// Steinhart-Hart thermistor eqn coeff b.
const Settings::display_type Kc = 8.566e-8;	// Steinhart-Hart thermistor eqn coeff c.

const Settings::display_type R = 10030.0;	// Thermistor voltage divider resistor R (Ohms).
const Settings::display_type Vss = 4.97;	// Thermistor voltage divider supply voltage (Volts).
const Settings::display_type Vbe = 0.6;		// Thermistor voltage divider amplifier voltage drop (Volts).

/* These pins are for an ATMega 2560 */

const pin_t KeypadInput = A0;
const pin_t SensorInput = A7;
const pin_t AlarmOutput = 53;
const pin_t PwmOut = 46;
const pin_t LcdRs = 8;
const pin_t LcdEn = 9;
const pin_t LcdD4 = 4;
const pin_t LcdD5 = 5;
const pin_t LcdD6 = 6;
const pin_t LcdD7 = 7;

/* Published values for D1 Robot LCD/Keypad Shield. */

const Keypad::value_type RightButtonTriggerLevel = 60;
const Keypad::value_type UpButtonTriggerLevel = 200;
const Keypad::value_type DownButtonTriggerLevel = 400;
const Keypad::value_type LeftButtonTriggerLevel = 600;
const Keypad::value_type SelectButtonTriggerLevel = 800;

const TemperatureSensor::value_type SensorOutMax = AnalogMax<board_type>();

const long EepromID = 20211010L;	// Value used to validate EEPROM data.

#pragma endregion
#pragma region Program Timing Constants.

const milliseconds SensorInitDelay = seconds(1);
const milliseconds SensorPollingMin = milliseconds(100);
const milliseconds SensorPollingMax = milliseconds(9999);
const milliseconds SensorPollingInterval = seconds(1);
const milliseconds KeypadPollingInterval = milliseconds(100);
const milliseconds KeypadLongPressInterval = seconds(1);
const milliseconds DisplayRefreshInterval = milliseconds(100);
const milliseconds AdjustmentMultiplyInterval = seconds(4);

#pragma endregion 
#pragma region Program Behavior Constants.

const Keypad::LongPress KeypadLongPressMode = Keypad::LongPress::Hold;
const Adjustment::factor_type AdjustmentMultiplyMax = 100;
const Settings::display_type DisplayAdjustmentFactor = 0.1;
const TemperatureSensor::value_type SensorAdjustmentFactor = 1;
const Settings::display_type DisplayValueMin = -999.9; // Smallest value that fits temperature display fields. 
const Settings::display_type DisplayValueMax = +999.9;	// Largest value that fits temperature display fields.
const Pid::value_type PidValueMin = 0.0;		// Min pid coeff value.
const Pid::value_type PidValueMax= 9.9;			// Max pid coeff value.
const Pid::value_type PidProportional = 1.0;	// Default Kp.
const Pid::value_type PidIntegral = 0.0;		// Default Ki.
const Pid::value_type PidDerivative = 0.0;		// Default Kd.
const Pid::value_type PidGain = 0.1;			// Default Ka.
const Settings::display_type DisplayRangeLow = 0.0;				// Default value.
const Settings::display_type DisplayRangeHigh = 100.0;			// Default value.
const Settings::display_type AlarmSetPoint = DisplayRangeLow;	// Default value.

#pragma endregion
#pragma region Display Symbols and Formatting

const char DegreeSymbol = 0xDF;
const char FarenheitSymbol = 'F';
const char CelsiusSymbol = 'C';
const char KelvinSymbol = 'K';
const char YesSymbol = 'Y';
const char NoSymbol = 'N';
const char EnabledSymbol = '*';
const char DisabledSymbol = ' ';
const char LessSymbol = '<';
const char GreaterSymbol = '>';
const char* InternalSymbol = "IN";
const char* ExternalSymbol = "EX";

const char* const TemperatureDisplayFormat = "%6.1f";	// Supports upto +/- 999.9
const char* const TimingDisplayFormat = "%4u";			// Supports upto 9999 ms (SensorPollingMax).
const char* const PidDisplayFormat = "%3.1f";			// Supports [0.0, +9.9]

#pragma region RUN Screen

/******************
 *PV:-nnn.m*F     *
 *SP:-nnn.m*  AL:Y*
 ******************/
const char* RunScreenLab = "";

const uint8_t PvValCol = 0;
const uint8_t PvValRow = 0;
const char* const PvValLab = "PV:";
const char* const PvValFmt = TemperatureDisplayFormat;

const uint8_t PvSymCol = 9;
const uint8_t PvSymRow = 0;
const char* const PvSymLab = "";
const char* const PvSymFmt = "%c";		// Supports degrees symbol.

const uint8_t PvUnitCol = 10;
const uint8_t PvUnitRow = 0;
const char* const PvUnitLab = "";
const char* const PvUnitFmt = "%c";		// Supports 'C', 'F' or 'K'.

const uint8_t SpValCol = 0;
const uint8_t SpValRow = 1;
const char* const SpValLab = "SP:";
const char* const SpValFmt = TemperatureDisplayFormat;

const uint8_t SpEnCol = 9;
const uint8_t SpEnRow = 1;
const char* const SpEnLab = "";
const char* const SpEnFmt = "%c";			// Supports '*' or ''

const uint8_t AlrmEnCol = 12;
const uint8_t AlrmEnRow = 1;
const char* const AlrmEnLab = "AL:";
const char* const AlrmEnFmt = "%c";		// Supports '*' or ' '

#pragma endregion
#pragma region MENU Screen

/******************
 *MENU: Run  Pid  *
 *Alrm Sense Displ*
 ******************/
const char* const MenuScreenLab = "MENU";

const char* const MenuItemRun = "Run";
const char* const MenuItemPid = "Pid";
const char* const MenuItemAlarm = "Alrm";
const char* const MenuItemSensor = "Sense";
const char* const MenuItemDisplay = "Displ";

const uint8_t MenuRunCol = 6;
const uint8_t MenuRunRow = 0;
const char* const MenuRunLab = "";
const char* const MenuRunFmt = "%s";

const uint8_t MenuPidCol = 11;
const uint8_t MenuPidRow = 0;
const char* const MenuPidLab = "";
const char* const MenuPidFmt = "%s";

const uint8_t MenuAlarmCol = 0;
const uint8_t MenuAlarmRow = 1;
const char* const MenuAlarmLab = "";
const char* const MenuAlarmFmt = "%s";

const uint8_t MenuSensorCol = 5;
const uint8_t MenuSensorRow = 1;
const char* const MenuSensorLab = "";
const char* const MenuSensorFmt = "%s";

const uint8_t MenuDisplayCol = 11;
const uint8_t MenuDisplayRow = 1;
const char* const MenuDisplayLab = "";
const char* const MenuDisplayFmt = "%s";

#pragma endregion
#pragma region PID Screen

/******************
 *PID  p:n.n i:n.n*
 *     d:n.n A:n.n*
 ******************/
const char* const PidScreenLab = "PID";

const uint8_t PCol = 5;
const uint8_t PRow = 0;
const char* const PLab = "p:";
const char* const PFmt = PidDisplayFormat;

const uint8_t ICol = 11;
const uint8_t IRow = 0;
const char* const ILab = "i:";
const char* const IFmt = PidDisplayFormat;

const uint8_t DCol = 5;
const uint8_t DRow = 1;
const char* const DLab = "d:";
const char* const DFmt = PidDisplayFormat;

const uint8_t ACol = 11;
const uint8_t ARow = 1;
const char* const ALab = "A:";
const char* const AFmt = PidDisplayFormat;

#pragma endregion
#pragma region ALARM Screen

/******************
 *ALARM  En:Y     *
 *Cmp:<  Sp:-nnn.m*
 ******************/
const char* const AlarmScreenLab = "ALARM";

const uint8_t AlarmEnableCol = 7;
const uint8_t AlarmEnableRow = 0;
const char* const AlarmEnableLab = "En:";
const char* const AlarmEnableFmt = "%c";	// Supports 'Y' or 'N'

const uint8_t AlarmCmpCol = 0;
const uint8_t AlarmCmpRow = 1;
const char* const AlarmCmpLab = "Cmp:";
const char* const AlarmCmpFmt = "%c";		// Supports '<' or '>'

const uint8_t AlarmSetPointCol = 7;
const uint8_t AlarmSetPointRow = 1;
const char* const AlarmSetPointLab = "Sp:";
const char* const AlarmSetPointFmt = TemperatureDisplayFormat;
#pragma endregion
#pragma region SENSOR Screen

/******************
 *SENSOR: Lo:nnnnn*
 *Tp:nnnn Hi:nnnnn*
 ******************/
const char* const SensorScreenLab = "SENSOR";

const uint8_t SensorArefCol = 9;
const uint8_t SensorArefRow = 0;
const char* const SensorArefLab = "Aref:";
const char* const SensorArefFmt = "%2s";		// Supports "EX" or "IN"

const uint8_t SensorPollCol = 9;
const uint8_t SensorPollRow = 1;
const char* const SensorPollLab = "Tp:";
const char* const SensorPollFmt = TimingDisplayFormat;

#pragma endregion
#pragma region DISPLAY Screen

/******************
 *DISPL  Lo:-nnn.m*
 *Un:F   Hi:-nnn.m*
 ******************/
const char* const DisplayScreenLab = "DISPL";

const uint8_t DisplayLowCol = 7;
const uint8_t DisplayLowRow = 0;
const char* const DisplayLowLab = "Lo:";
const char* const DisplayLowFmt = TemperatureDisplayFormat;

const uint8_t DisplayHighCol = 7;
const uint8_t DisplayHighRow = 1;
const char* const DisplayHighLab = "Hi:";
const char* const DisplayHighFmt = TemperatureDisplayFormat;

const uint8_t DisplayUnitCol = 0;
const uint8_t DisplayUnitRow = 1;
const char* const DisplayUnitLab = "Un:";
const char* const DisplayUnitFmt = "%c";	// Supports 'C', 'F' or 'K'.

#pragma endregion
#pragma endregion

// Enumerates valid thermostat operating modes.
enum class ThermostatMode
{
	Init = 0,	// Initial state.
	Run,		// Displays current temperature, set point and alarm settings.
	Setpoint,	// Allows setpoint/alarm enable edits in Run mode.
	Menu,		// Presents a menu of operating mode choices.
	Pid,		// Displays/edits pid settings.
	Alarm,		// Displays/edits alarm settings.
	Sensor,		// Displays/edits sensor settings.
	Display		// Displays/edits display settings.
};

#endif // !defined __PG_THERMOMETER_CONFIG_H
