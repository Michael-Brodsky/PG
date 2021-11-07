/*
 *	Config file for <PIDThermostat.ino>, defines immutable constant used by 
 *	the program.
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

#if !defined __PG_THERMOSTAT_CONFIG_H
# define __PG_THERMOSTAT_CONFIG_H 20211025L

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
const pin_t SensorInput = A15;
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

const long EepromID = 20211010L;	// Value used to determine existence and validity of EEPROM data.

#pragma endregion
#pragma region Program Timing Constants.

const milliseconds SensorInitDelay = seconds(1);
const milliseconds SensorPollingMin = milliseconds(100);
const milliseconds SensorPollingMax = milliseconds(9999);
const milliseconds KeypadPollingInterval = milliseconds(100);
const milliseconds KeypadLongPressInterval = seconds(1);
const milliseconds DisplayRefreshInterval = milliseconds(100);
const milliseconds AdjustmentMultiplyInterval = seconds(4);

#pragma endregion 
#pragma region Program Behavior Constants.

const Keypad::LongPress KeypadLongPressMode = Keypad::LongPress::Hold;
const Adjustment::factor_type AdjustmentMultiplyMax = 100;
const Settings::display_type DecimalAdjustmentFactor = 0.1;
const Settings::display_type UnitAdjustmentFactor = 1.0;
const TemperatureSensor::value_type SensorAdjustmentFactor = 1;
const Settings::display_type TemperatureMin = -999.9;	// Smallest value that fits temperature display fields. 
const Settings::display_type TemperatureMax = +999.9;	// Largest value that fits temperature display fields.
const Pid::value_type PidCoeffMin = 0.0;				// Min pid coeff value.
const Pid::value_type PidCoeffMax = 100.0;				// Max pid coeff value.
const Pid::value_type PidCoeffThreshold = 10.0;			// Value at which pid display format changes.

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
const char* const InternalSymbol = "IN";
const char* const ExternalSymbol = "EX";

const char* const TemperatureDisplayFormat = "%6.1f";	// [-999.9,+999.9]
const char* const TimingDisplayFormat = "%4u";			// [0,9999]
const char* const PidDecimalFormat = "%3.1f";			// [0.0, +9.9]
const char* const PidUnitFormat = "%3.0f";				// [+10, +100]

#pragma region RUN Screen

/******************
 *PV:-nnn.m*F     *
 *SP:-nnn.m*  AL:Y*
 ******************/
const char* RunScreenLab = "";

const uint8_t PvValueCol = 0;
const uint8_t PvValueRow = 0;
const char* const PvValueLab = "PV:";
const char* const PvValueFmt = TemperatureDisplayFormat;
const bool PvValueVis = true;
const bool PvValueEdit = false;

const uint8_t PvSymbolCol = 9;
const uint8_t PvSymbolRow = 0;
const char* const PvSymbolLab = "";
const char* const PvSymbolFmt = "%c";	// Degrees symbol.
const bool PvSymbolVis = true;
const bool PvSymbolEdit = false;

const uint8_t PvUnitCol = 10;
const uint8_t PvUnitRow = 0;
const char* const PvUnitLab = "";
const char* const PvUnitFmt = "%c";		// 'C', 'F' or 'K'.
const bool PvUnitVis = true;
const bool PvUnitEdit = false;

const uint8_t SpValueCol = 0;
const uint8_t SpValueRow = 1;
const char* const SpValueLab = "SP:";
const char* const SpValueFmt = TemperatureDisplayFormat;
const bool SpValueVis = true;
const bool SpValueEdit = true;

const uint8_t SpEnblCol = 9;
const uint8_t SpEnblRow = 1;
const char* const SpEnblLab = "";
const char* const SpEnblFmt = "%c";		// '*' or ' '
const bool SpEnblVis = true;
const bool SpEnblEdit = true;

const uint8_t AlrmQEnblCol = 12;
const uint8_t AlrmQEnblRow = 1;
const char* const AlrmQEnblLab = "AL:";
const char* const AlrmQEnblFmt = "%c";	// 'Y' or 'N'
const bool AlrmQEnblVis = true;
const bool AlrmQEnblEdit = true;

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
const bool MenuRunVis = true;
const bool MenuRunEdit = true;

const uint8_t MenuPidCol = 11;
const uint8_t MenuPidRow = 0;
const char* const MenuPidLab = "";
const char* const MenuPidFmt = "%s";
const bool MenuPidVis = true;
const bool MenuPidEdit = true;

const uint8_t MenuAlarmCol = 0;
const uint8_t MenuAlarmRow = 1;
const char* const MenuAlarmLab = "";
const char* const MenuAlarmFmt = "%s";
const bool MenuAlarmVis = true;
const bool MenuAlarmEdit = true;

const uint8_t MenuSensorCol = 5;
const uint8_t MenuSensorRow = 1;
const char* const MenuSensorLab = "";
const char* const MenuSensorFmt = "%s";
const bool MenuSensorVis = true;
const bool MenuSensorEdit = true;

const uint8_t MenuDisplayCol = 11;
const uint8_t MenuDisplayRow = 1;
const char* const MenuDisplayLab = "";
const char* const MenuDisplayFmt = "%s";
const bool MenuDisplayVis = true;
const bool MenuDisplayEdit = true;

#pragma endregion
#pragma region PID Screen

/******************
 *PID  p:n.n i:n.n*
 *100  d:n.n A:n.n*
 ******************/
const char* const PidScreenLab = "PID";

const uint8_t PidPropCol = 5;
const uint8_t PidPropRow = 0;
const char* const PidPropLab = "p:";
const char* const PidPropFmt = PidDecimalFormat;
const bool PidPropVis = true;
const bool PidPropEdit = true;

const uint8_t PidIntegCol = 11;
const uint8_t PidIntegRow = 0;
const char* const PidIntegLab = "i:";
const char* const PidIntegFmt = PidDecimalFormat;
const bool PidIntegVis = true;
const bool PidIntegEdit = true;

const uint8_t PidDerivCol = 5;
const uint8_t PidDerivRow = 1;
const char* const PidDerivLab = "d:";
const char* const PidDerivFmt = PidDecimalFormat;
const bool PidDerivVis = true;
const bool PidDerivEdit = true;

const uint8_t PidGainCol = 11;
const uint8_t PidGainRow = 1;
const char* const PidGainLab = "A:";
const char* const PidGainFmt = PidDecimalFormat;
const bool PidGainVis = true;
const bool PidGainEdit = true;

const uint8_t PidDutyCol = 0;
const uint8_t PidDutyRow = 1;
const char* const PidDutyLab = "";
const char* const PidDutyFmt = "%3u%%";	// [0,100] %
const bool PidDutyVis = true;
const bool PidDutyEdit = false;

#pragma endregion
#pragma region ALARM Screen

/******************
 *ALARM  En:Y     *
 *Cmp:<  Sp:-nnn.m*
 ******************/
const char* const AlarmScreenLab = "ALARM";

const uint8_t AlarmEnblCol = 7;
const uint8_t AlarmEnblRow = 0;
const char* const AlarmEnblLab = "En:";
const char* const AlarmEnblFmt = "%c";	// 'Y' or 'N'
const bool AlarmEnblVis = true;
const bool AlarmEnblEdit = true;

const uint8_t AlarmCmpCol = 0;
const uint8_t AlarmCmpRow = 1;
const char* const AlarmCmpLab = "Cmp:";
const char* const AlarmCmpFmt = "%c";	// '<' or '>'
const bool AlarmCmpVis = true;
const bool AlarmCmpEdit = true;

const uint8_t AlarmSetCol = 7;
const uint8_t AlarmSetRow = 1;
const char* const AlarmSetLab = "Sp:";
const char* const AlarmSetFmt = TemperatureDisplayFormat;
const bool AlarmSetVis = true;
const bool AlarmSetEdit = true;

#pragma endregion
#pragma region SENSOR Screen

/******************
 *SENSOR   Aref:IN*
 *         Tp:nnnn*
 ******************/
const char* const SensorScreenLab = "SENSOR";

const uint8_t SensorArefCol = 9;
const uint8_t SensorArefRow = 0;
const char* const SensorArefLab = "Aref:";
const char* const SensorArefFmt = "%2s";	// "EX" or "IN"
const bool SensorArefVis = true;
const bool SensorArefEdit = true;

const uint8_t SensorPollCol = 9;
const uint8_t SensorPollRow = 1;
const char* const SensorPollLab = "Tp:";
const char* const SensorPollFmt = TimingDisplayFormat;
const bool SensorPollVis = true;
const bool SensorPollEdit = true;

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
const bool DisplayLowVis = true;
const bool DisplayLowEdit = true;

const uint8_t DisplayHighCol = 7;
const uint8_t DisplayHighRow = 1;
const char* const DisplayHighLab = "Hi:";
const char* const DisplayHighFmt = TemperatureDisplayFormat;
const bool DisplayHighVis = true;
const bool DisplayHighEdit = true;

const uint8_t DisplayUnitCol = 0;
const uint8_t DisplayUnitRow = 1;
const char* const DisplayUnitLab = "Un:";
const char* const DisplayUnitFmt = "%c";	// 'C', 'F' or 'K'.
const bool DisplayUnitVis = true;
const bool DisplayUnitEdit = true;

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

#endif // !defined __PG_THERMOSTAT_CONFIG_H
