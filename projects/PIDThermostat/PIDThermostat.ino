/*
 *	This program demonstrates the implementation of a programmable PID
 *  controlled thermostat.
 *
 *	***************************************************************************
 *
 *	File: PIDThermostat.ino
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

#include <LiquidCrystal.h>  // Arduino LCD display api.
#include <pg.h>             // Pg library header.
#include "Thermostat.h"     // Program type defs.
#include "config.h"         // Program configuration constants.

using namespace pg;
using namespace std::chrono;

#pragma region Short Descriptive Typedefs

using ScheduledTask = Scheduler::Task;
using TaskState = ScheduledTask::State;
using SensorAref = Settings::sensor_aref_type;
using SensorArefSource = ArefSource;
using AlarmEnable = Settings::enable_type;
using AlarmCompare = Settings::alarm_compare_type;
using SetpointEnable = Settings::enable_type;
using DisplayUnit = Settings::unit_type;
using Temperature = Settings::display_type;
using PidCoefficient = Pid::value_type;
using SensorValue = Settings::adc_type;
using Interval = Settings::duration_type;
using DutyCycle = uint8_t;

#pragma endregion
#pragma region Function Declarations

void keyPress(const Keypad::Button*);
void keyRelease(const Keypad::Button*);
void keyLongpress(const Keypad::Button*);
void keyRepeat(bool);
void keypadCallback(const Keypad::Button*, Keypad::Event);
void displayCallback();
void sensorCallback();
void initSensor();
void checkAlarm(Temperature);
void scrollField(const Keypad::Button*);
void menuSelect(const Display::Field*);
void setMode(ThermostatMode);
void adjustSettings(const Display::Field*, const Keypad::Button*);
void updateSettings(ThermostatMode);
void readSettings();
void writeSettings();
void adjustSetpoint(const Display::Field*, Adjustment::Direction);
void adjustPid(const Display::Field*, Adjustment::Direction);
void adjustAlarm(const Display::Field*, Adjustment::Direction);
void adjustSensor(const Display::Field*, Adjustment::Direction);
void adjustDisplay(const Display::Field*, Adjustment::Direction);
void switchUnits(Settings&, DisplayUnit);
void formatPidScreen();
Temperature getTemperature(SensorValue);

#pragma endregion
#pragma region Thermostat Settings Objects 

const SensorAref SensorArefInternal{ ArefSource::Internal,InternalSymbol };
const SensorAref SensorArefExternal{ ArefSource::External,ExternalSymbol };
const AlarmEnable AlarmDisabled{ false,NoSymbol };
const AlarmEnable AlarmEnabled{ true,YesSymbol };
const AlarmCompare AlarmCmpLess{ alarm_lt,LessSymbol };
const AlarmCompare AlarmCmpGreater{ alarm_gt,GreaterSymbol };
const SetpointEnable SetpointEnabled{ true,EnabledSymbol };
const SetpointEnable SetpointDisabled{ false,DisabledSymbol };
const DisplayUnit DegreesKelvin{ temperature<units::kelvin>,KelvinSymbol };
const DisplayUnit DegreesCelsius{ temperature<units::celsius>,CelsiusSymbol };
const DisplayUnit DegreesFahrenheit{ temperature<units::fahrenheit>,FarenheitSymbol };

#pragma endregion
#pragma region Settings Defaults 

const DisplayUnit TemperatureUnits = DegreesCelsius;
const Temperature TemperatureRangeLow = 0.0;
const Temperature TemperatureRangeHigh = 100.0;
const SetpointEnable SetpointStatus = SetpointDisabled;
const Temperature SetPointValue = TemperatureRangeLow;
const AlarmEnable AlarmStatus = AlarmDisabled;
const AlarmCompare AlarmCmp = AlarmCmpGreater;
const Temperature AlarmSet = TemperatureRangeLow;
const PidCoefficient PidProp = 1.0;
const PidCoefficient PidInteg = 0.0;
const PidCoefficient PidDeriv = 0.0;
const PidCoefficient PidGain = 1.0;
const milliseconds SensorPollingInterval = seconds(1);
const SensorAref SensorReferenceSource = SensorArefInternal;

#pragma endregion
#pragma region Keypad Objects

Keypad::Button right_button(RightButtonTriggerLevel);
Keypad::Button up_button(UpButtonTriggerLevel);
Keypad::Button down_button(DownButtonTriggerLevel);
Keypad::Button left_button(LeftButtonTriggerLevel);
Keypad::Button select_button(SelectButtonTriggerLevel);
Keypad keypad(KeypadInput, keypadCallback,
    { &right_button,&up_button,&down_button,&left_button,&select_button },
    KeypadLongPressMode, KeypadLongPressInterval);
auto isNavButton = [](const Keypad::Button* button)
{ return button->id() == right_button.id() || button->id() == left_button.id(); };
auto isAdjustButton = [](const Keypad::Button* button)
{ return button->id() == up_button.id() || button->id() == down_button.id(); };
auto isSelectButton = [](const Keypad::Button* button)
{ return button->id() == select_button.id(); };

#pragma endregion
#pragma region Display Management Objects

Display::Field pvval_field = { PvValueCol,PvValueRow,PvValueLab,PvValueFmt,PvValueVis,PvValueEdit };
Display::Field pvsym_field = { PvSymbolCol,PvSymbolRow,PvSymbolLab,PvSymbolFmt,PvSymbolVis,PvSymbolEdit };
Display::Field pvunit_field = { PvUnitCol,PvUnitRow,PvUnitLab,PvUnitFmt,PvUnitVis,PvUnitEdit };
Display::Field spval_field = { SpValueCol,SpValueRow,SpValueLab,SpValueFmt,SpValueVis,SpValueEdit };
Display::Field spen_field = { SpEnblCol,SpEnblRow,SpEnblLab,SpEnblFmt,SpEnblVis,SpEnblEdit };
Display::Field alrmqen_field = { AlrmQEnblCol,AlrmQEnblRow,AlrmQEnblLab,AlrmQEnblFmt,AlrmQEnblVis,AlrmQEnblEdit };
Display::Screen run_screen({ &pvval_field,&pvsym_field,&pvunit_field,&spval_field,&spen_field,&alrmqen_field }, 
    RunScreenLab);

Display::Field menu_run_field = { MenuRunCol,MenuRunRow,MenuRunLab,MenuRunFmt,MenuRunVis,MenuRunEdit };
Display::Field menu_pid_field = { MenuPidCol,MenuPidRow,MenuPidLab,MenuPidFmt,MenuPidVis,MenuPidEdit };
Display::Field menu_alarm_field = { MenuAlarmCol,MenuAlarmRow,MenuAlarmLab,MenuAlarmFmt,MenuAlarmVis,MenuAlarmEdit };
Display::Field menu_sensor_field = { MenuSensorCol,MenuSensorRow,MenuSensorLab,MenuSensorFmt,MenuSensorVis,MenuSensorEdit };
Display::Field menu_display_field = { MenuDisplayCol,MenuDisplayRow,MenuDisplayLab,MenuDisplayFmt,MenuDisplayVis,MenuDisplayEdit };
Display::Screen menu_screen({ &menu_run_field,&menu_pid_field,&menu_alarm_field,&menu_sensor_field,&menu_display_field },
    MenuScreenLab);

Display::Field pid_prop_field = { PidPropCol,PidPropRow,PidPropLab,PidPropFmt,PidPropVis,PidPropEdit };
Display::Field pid_integ_field = { PidIntegCol,PidIntegRow,PidIntegLab,PidIntegFmt,PidIntegVis,PidIntegEdit };
Display::Field pid_deriv_field = { PidDerivCol,PidDerivRow,PidDerivLab,PidDerivFmt,PidDerivVis,PidDerivEdit };
Display::Field pid_gain_field = { PidGainCol,PidGainRow,PidGainLab,PidGainFmt,PidGainVis,PidGainEdit };
Display::Field pid_duty_field = { PidDutyCol,PidDutyRow,PidDutyLab,PidDutyFmt,PidDutyVis,PidDutyEdit };
Display::Screen pid_screen({ &pid_prop_field,&pid_integ_field,&pid_deriv_field,&pid_gain_field,&pid_duty_field }, 
    PidScreenLab);

Display::Field alarm_enbl_field = { AlarmEnblCol,AlarmEnblRow,AlarmEnblLab,AlarmEnblFmt,AlarmEnblVis,AlarmEnblEdit };
Display::Field alarm_cmp_field = { AlarmCmpCol,AlarmCmpRow,AlarmCmpLab,AlarmCmpFmt,AlarmCmpVis,AlarmCmpEdit };
Display::Field alarm_set_field = { AlarmSetCol,AlarmSetRow,AlarmSetLab,AlarmSetFmt,AlarmSetVis,AlarmSetEdit };
Display::Screen alarm_screen({ &alarm_enbl_field,&alarm_set_field,&alarm_cmp_field }, AlarmScreenLab);

Display::Field sensor_aref_field = { SensorArefCol,SensorArefRow,SensorArefLab,SensorArefFmt,SensorArefVis,SensorArefEdit };
Display::Field sensor_poll_field = { SensorPollCol,SensorPollRow,SensorPollLab,SensorPollFmt,SensorPollVis,SensorPollEdit };
Display::Screen sensor_screen({ &sensor_aref_field,&sensor_poll_field }, SensorScreenLab);

Display::Field display_low_field = { DisplayLowCol,DisplayLowRow,DisplayLowLab,DisplayLowFmt,DisplayLowVis,DisplayLowEdit };
Display::Field display_hi_field = { DisplayHighCol,DisplayHighRow,DisplayHighLab,DisplayHighFmt,DisplayHighVis,DisplayHighEdit };
Display::Field display_unit_field = { DisplayUnitCol,DisplayUnitRow,DisplayUnitLab,DisplayUnitFmt,DisplayUnitVis,DisplayUnitEdit };
Display::Screen display_screen({ &display_low_field,&display_hi_field,&display_unit_field }, DisplayScreenLab);

LiquidCrystal lcd(LcdRs, LcdEn, LcdD4, LcdD5, LcdD6, LcdD7);
Display display(&lcd, displayCallback, &run_screen);

#pragma endregion
#pragma region Thermostat Objects

Settings settings{
    TemperatureRangeLow,TemperatureRangeHigh,TemperatureUnits,
    PidProp,PidInteg,PidDeriv,PidGain,
    SetpointStatus,SetPointValue,
    AlarmStatus,AlarmCmp,AlarmSet,
    SensorReferenceSource,SensorPollingInterval
}, settings_copy;
TemperatureSensor temp_sensor(SensorInput, sensorCallback);
InputFilter temp_filter;
Pid pid(settings.setpointValue(), PidProp, PidInteg, PidDeriv, PidGain);
Pwm pwm(PwmOut);
Temperature Tsense = Temperature();
DutyCycle dc_pct = DutyCycle();

#pragma endregion
#pragma region Misc. Program Objects

Adjustment adjustment(AdjustmentMultiplyMax);
ThermostatMode op_mode = ThermostatMode::Init;
EEStream eeprom;

#pragma endregion
#pragma region Task Scheduling Objects

ClockCommand keypad_clock(&keypad);
ClockCommand display_clock(&display);
ClockCommand sensor_clock(&temp_sensor);
ClockCommand adjustment_clock(&adjustment);
ScheduledTask keypad_task(KeypadPollingInterval, &keypad_clock, TaskState::Active);
ScheduledTask display_task(DisplayRefreshInterval, &display_clock, TaskState::Active);
ScheduledTask sensor_task(SensorPollingInterval, &sensor_clock, TaskState::Active);
ScheduledTask adjustment_task(AdjustmentMultiplyInterval, &adjustment_clock, TaskState::Idle);
Scheduler scheduler{ &keypad_task,&display_task,&sensor_task,&adjustment_task };

#pragma endregion

void setup() 
{
    lcd.begin(Display::cols(), Display::rows());
    lcd.clear();
    lcd.print("Initializing");
    alarmAttach(AlarmOutput);
    alarmSilence(AlarmOutput, true);
    readSettings();
    initSensor();
    setMode(ThermostatMode::Run);
    scheduler.start();
    auto i = std::abs(3);
}

void loop() 
{
    scheduler.tick();
}

void keyPress(const Keypad::Button* button)
{
    switch (op_mode)
    {
    case ThermostatMode::Setpoint:
    case ThermostatMode::Pid:
    case ThermostatMode::Alarm:
    case ThermostatMode::Sensor:
    case ThermostatMode::Display:
        if (isAdjustButton(button))
            adjustSettings(display.screen()->active_field(), button);
        break;
    default:
        break;
    }
}

void keyRelease(const Keypad::Button* button)
{
    keyRepeat(false);
    switch (op_mode)
    {
    case ThermostatMode::Run:
        if (!alarmSilence(AlarmOutput)) // Any key silences the alarm in RUN mode.
            alarmSilence(AlarmOutput, true);
        else if (isNavButton(button))   // Setpoint & alarm settings adjustable in RUN mode.
        {
            setMode(ThermostatMode::Setpoint);
            run_screen.active_field(button == &right_button ? &spval_field : &alrmqen_field);
            display.update();
        }
        break;
    case ThermostatMode::Menu:
        if (isSelectButton(button))
            menuSelect(display.screen()->active_field());
        else if (isNavButton(button))
            scrollField(button);
        break;
    case ThermostatMode::Pid:
    case ThermostatMode::Setpoint:
    case ThermostatMode::Alarm:
    case ThermostatMode::Sensor:
    case ThermostatMode::Display:
        if (isSelectButton(button))     // Short press/release cancels edits & returns to RUN mode.
            setMode(ThermostatMode::Run);
        else if (isNavButton(button))
            scrollField(button);
        break;
    default:
        break;
    }
}

void keyLongpress(const Keypad::Button* button)
{
    if (isSelectButton(button))
    {
        switch (op_mode)
        {
        case ThermostatMode::Run:
            setMode(ThermostatMode::Menu);  // Opens MENU screen.
            break;
        case ThermostatMode::Pid:
        case ThermostatMode::Setpoint:
        case ThermostatMode::Alarm:
        case ThermostatMode::Sensor:
        case ThermostatMode::Display:
            updateSettings(op_mode);        // Saves edits.
            writeSettings();
        case ThermostatMode::Menu:
            setMode(ThermostatMode::Run);   // Returns to RUN mode.
            break;
        default:
            break;
        }
    }
    else if (isAdjustButton(button) && op_mode != ThermostatMode::Run)
        keyRepeat(true);
}

void keyRepeat(bool enabled)
{
    // Disable button repeat and reset adjustment factor.
    keypad.repeat(enabled);
    adjustment.reset();
    adjustment_task.state(enabled ? TaskState::Active : TaskState::Idle);
    adjustment_task.reset();
}

void keypadCallback(const Keypad::Button* button, Keypad::Event event)
{
    static bool release = true; // Ignores release event after long-press event.

    switch (event)
    {
    case Keypad::Event::Press:
        keyPress(button);
        break;
    case Keypad::Event::Longpress:
        keyLongpress(button);
        release = false;
        break;
    case Keypad::Event::Release:
        keyRepeat(false);
        if (!release)
            release = true;
        else
            keyRelease(button);
        break;
    default:
        break;
    }
}

void displayCallback()
{
    // Display values for the current screen.
    switch (op_mode)
    {
    case ThermostatMode::Setpoint:
        display.refresh(clamp(Tsense, settings.tempLow(), settings.tempHigh()), DegreeSymbol, settings.unitSymbol(),
            settings_copy.setpointValue(), settings_copy.setpointSymbol(), settings_copy.alarmEnableSymbol());
        break;
    case ThermostatMode::Run:
        display.refresh(clamp(Tsense, settings.tempLow(), settings.tempHigh()), DegreeSymbol, settings.unitSymbol(),
            settings.setpointValue(), settings.setpointSymbol(), settings.alarmEnableSymbol());
        break;
    case ThermostatMode::Menu:
        display.refresh(MenuItemRun, MenuItemPid, MenuItemAlarm, MenuItemSensor, MenuItemDisplay);
        break;
    case ThermostatMode::Pid:
        display.refresh(settings_copy.pidProportional(), settings_copy.pidIntegral(),
            settings_copy.pidDerivative(), settings_copy.pidGain(), dc_pct);
        display.update(); // Always update the pwm output duty cycle.
        break;
    case ThermostatMode::Alarm:
        display.refresh(settings_copy.alarmEnableSymbol(), settings_copy.alarmSet(),
            settings_copy.alarmCompareSymbol());
        break;
    case ThermostatMode::Sensor:
        display.refresh(settings_copy.sensorArefString(), settings_copy.sensorPollIntvl());
        break;
    case ThermostatMode::Display:
        display.refresh(settings_copy.tempLow(), settings_copy.tempHigh(), settings_copy.unitSymbol());
        break;
    default:
        break;
    }
}

void sensorCallback()
{
    // Save the sensed temperature, check if it triggers an alarm and output the process control pwm signal.
    checkAlarm((Tsense = getTemperature(temp_filter.out(temp_sensor.value()))));
    PidCoefficient measured_value = clamp(Tsense, settings.tempLow(), settings.tempHigh());
    PidCoefficient control_value = clamp(pid.loop(measured_value), 0.0f, settings.tempHigh() - settings.tempLow());
    PidCoefficient output_value = norm(control_value, 0.0f, settings.tempHigh() - settings.tempLow(), 0.0f, 1.0f);
    pwm.duty_cycle(output_value);
    dc_pct = (uint8_t)(output_value * 100); // Round to [0,100].

}

void initSensor()
{
    delay(SensorInitDelay.count());     // Wait a sec for sensor/ADC to stabilize.
    temp_filter.seed(temp_sensor());    // Seed the input filter with the current sensor reading.
    Tsense = getTemperature(temp_filter.out(temp_sensor.value())); // Save the current temperature.
    pid.start(steady_clock::now());     // Start the pid controller at the current time.
}

void checkAlarm(Temperature value)
{
    static bool alarm_active = false;
    bool alarm_now = settings.alarmCompare()(value, settings.alarmSet());
    // Alarm only sounds if it crosses the setpoint, stays silent after being manually silenced.
    if (settings.alarmEnabled() && !alarm_active && alarm_now)
        alarmSilence(AlarmOutput, false);
    alarm_active = alarm_now && settings.alarmEnabled();
}

void scrollField(const Keypad::Button* button)
{
    // Advance to next/prev field, skipping any non-editable fields.
    do
    {
        button == &left_button
            ? display.prev()
            : display.next();
    } while (!display.screen()->active_field()->editable_);
}

void menuSelect(const Display::Field* field)
{
    if (field == &menu_run_field)
        setMode(ThermostatMode::Run);
    if (field == &menu_pid_field)
        setMode(ThermostatMode::Pid);
    else if (field == &menu_alarm_field)
        setMode(ThermostatMode::Alarm);
    else if (field == &menu_sensor_field)
        setMode(ThermostatMode::Sensor);
    else if (field == &menu_display_field)
        setMode(ThermostatMode::Display);
}

void setMode(ThermostatMode mode)
{
    if (mode != op_mode)
    {
        // Activate the screen corresponding to the given mode, adding 
        // any visual effects and make a copy of the current program 
        // settings for editing.
        Display::Cursor display_cursor = Display::Cursor::Block;
        Display::Mode display_mode = Display::Mode::Edit;
        switch (mode)
        {
        case ThermostatMode::Run:
            display.screen(&run_screen);
            display_cursor = Display::Cursor::None;
            display_mode = Display::Mode::Normal;
            break;
        case ThermostatMode::Setpoint:
            break;
        case ThermostatMode::Pid:
            // This is redundant, but fits the settings copy/update scheme.
            settings.pidProportional() = pid.proportional();
            settings.pidIntegral() = pid.integral();
            settings.pidDerivative() = pid.derivative();
            settings.pidGain() = pid.gain();
            formatPidScreen();
            display.screen(&pid_screen);
            break;
        case ThermostatMode::Menu:
            display.screen(&menu_screen);
            break;
        case ThermostatMode::Alarm:
            display.screen(&alarm_screen);
            break;
        case ThermostatMode::Sensor:
            display.screen(&sensor_screen);
            break;
        case ThermostatMode::Display:
            display.screen(&display_screen);
            break;
        default:
            break;
        };
        if (display_mode == Display::Mode::Edit)
            settings.copy(settings, settings_copy);
        display.cursor(display_cursor);
        display.mode(display_mode);
        display.clear();
        op_mode = mode;
    }
}

void adjustSettings(const Display::Field* field, const Keypad::Button* button)
{
    Adjustment::Direction dir = button == &up_button
        ? Adjustment::Direction::Up
        : Adjustment::Direction::Down;

    // Adjust the value of the active field on the current screen.
    // Active field determined by cursor position.
    switch (op_mode)
    {
    case ThermostatMode::Setpoint:
        adjustSetpoint(field, dir);
        break;
    case ThermostatMode::Pid:
        adjustPid(field, dir);
        break;
    case ThermostatMode::Alarm:
        adjustAlarm(field, dir);
        break;
    case ThermostatMode::Sensor:
        adjustSensor(field, dir);
        break;
    case ThermostatMode::Display:
        adjustDisplay(field, dir);
        break;
    default:
        break;
    }
    display.update(); // Automatic display updates suspended in edit modes.
}

void updateSettings(ThermostatMode mode)
{
    // Make the editted (or eeprom, if initializing) settings active.
    if (mode != ThermostatMode::Init)
        settings.update(settings_copy, settings);
    // Update the program from the active settings.
    sensor_task.interval(settings.sensorPollIntvl());
    sensorSetAref(settings.sensorArefSource());
    pid.set_point(settings.setpointValue());
    pid.proportional(settings.pidProportional());
    pid.integral(settings.pidIntegral());
    pid.derivative(settings.pidDerivative());
    pid.gain(settings.pidGain());
    pid.set_point(settings.setpointValue());
    pwm.enabled(settings.setpointEnabled());
    Tsense = getTemperature(temp_filter.out(temp_sensor.value()));
}

void readSettings()
{
    long eeprom_id;

    eeprom.reset();
    eeprom >> eeprom_id;
    if (eeprom_id == EepromID) // Check if EEPROM data exists and is valid.
    {
        settings.deserialize(eeprom);
        settings.unitConvert() = settings.unitSymbol() == FarenheitSymbol
            ? (typename Settings::unit_convert_func)temperature<units::fahrenheit>
            : settings.unitSymbol() == CelsiusSymbol
            ? (typename Settings::unit_convert_func)temperature<units::celsius>
            : (typename Settings::unit_convert_func)temperature<units::kelvin>;
        settings.alarmEnabled() = settings.alarmEnableSymbol() == YesSymbol
            ? true
            : false;
        settings.alarmCompare() = settings.alarmCompareSymbol() == LessSymbol
            ? (typename Settings::alarm_cmp_func)alarm_lt
            : (typename Settings::alarm_cmp_func)alarm_gt;
        settings.sensorArefString() = settings.sensorArefSource() == ArefSource::Internal
            ? InternalSymbol
            : ExternalSymbol;
        settings.setpointEnabled() = settings.setpointSymbol() == EnabledSymbol
            ? true
            : false;
    }
    else // If not then save the current settings.
        writeSettings();
    updateSettings(op_mode);
}

void writeSettings()
{
    eeprom.reset();
    eeprom << EepromID; // Save the eeprom id so we know settings exist and are valid.
    settings.serialize(eeprom);
}

void adjustSetpoint(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &spval_field)
    {
        auto inc = adjustment.value(DecimalAdjustmentFactor, dir);

        settings_copy.setpointValue() = wrap<Temperature, Temperature>(
            settings_copy.setpointValue(), inc, settings.tempLow(), settings.tempHigh());
    }
    else if (field == &spen_field)
        settings_copy.setpointEnType() = (settings_copy.setpointEnabled()) 
        ? SetpointDisabled
        : SetpointEnabled;
    if (field == &alrmqen_field)
        settings_copy.alarmEnType() = (settings_copy.alarmEnabled()) 
        ? AlarmDisabled
        : AlarmEnabled;
}

void adjustPid(const Display::Field* field, Adjustment::Direction dir)
{
    // Pid coeff fields change display format according to their current value.
    // Pid coeff range is [0, 100].
    // value > 10 || value == 10 && dir == Down: format = [0.0,9.9], adjust = 0.1
    // else: format = [10,100], adjust = 1.0.

    // Select coeff to adjust according to active field.
    Temperature& coeff = field == &pid_prop_field
        ? settings_copy.pidProportional()
        : field == &pid_integ_field
        ? settings_copy.pidIntegral()
        : field == &pid_deriv_field
        ? settings_copy.pidDerivative()
        : settings_copy.pidGain();
    // Set adjustment factor depending on the coeff's current value.
    auto inc = adjustment.value(
        coeff < PidCoeffThreshold || (coeff < (PidCoeffThreshold + 1.0f) && dir == Adjustment::Direction::Down)
        ? DecimalAdjustmentFactor : UnitAdjustmentFactor, dir);
    // Adjust coeff.
    coeff = wrap<Temperature, Temperature>(
        const_cast<const Temperature &>(coeff), inc, PidCoeffMin, PidCoeffMax);
    // Format field according to the adjusted value.
    const_cast<Display::Field*>(field)->fmt_ = coeff < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
}

void adjustAlarm(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &alarm_enbl_field)
        settings_copy.alarmEnType() = (settings_copy.alarmEnabled())
        ? AlarmDisabled
        : AlarmEnabled;
    else if (field == &alarm_cmp_field)
        settings_copy.alarmCmpType() = settings_copy.alarmCompareSymbol() == LessSymbol 
        ? AlarmCmpGreater
        : AlarmCmpLess;
    else if (field == &alarm_set_field)
    {
        auto inc = adjustment.value(DecimalAdjustmentFactor, dir);

        settings_copy.alarmSet() = wrap<Temperature, Temperature>(
            settings_copy.alarmSet(), inc, settings.tempLow(), settings.tempHigh());
    }
}

void adjustSensor(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &sensor_poll_field)
    {
         auto inc = adjustment.value(SensorAdjustmentFactor, dir);

         settings_copy.sensorPollIntvl() = Interval(
              wrap<SensorValue, std::make_signed<SensorValue>::type>(
                  settings_copy.sensorPollIntvl().count(), inc, SensorPollingMin.count(), SensorPollingMax.count()));
    }
    else if (field == &sensor_aref_field)
        settings_copy.sensorArefType() = settings_copy.sensorArefSource() == ArefSource::External 
            ? SensorArefInternal
            : SensorArefExternal;
}

void adjustDisplay(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &display_unit_field)
    {
        auto to_units = settings_copy.unitType() == DegreesCelsius
            ? DegreesFahrenheit
            : settings_copy.unitType() == DegreesFahrenheit
            ? DegreesKelvin
            : DegreesCelsius;

        switchUnits(settings_copy, to_units);
    }
    else
    {
        auto inc = adjustment.value(DecimalAdjustmentFactor, dir);
        Temperature& value = field == &display_low_field
            ? settings_copy.tempLow()
            : settings_copy.tempHigh();

        value = wrap<Temperature, Temperature>(
            const_cast<const Temperature&>(value), inc, TemperatureMin, TemperatureMax);
    }
}

void switchUnits(Settings& settings, DisplayUnit to_units)
{
    typename Settings::unit_convert_func cv = nullptr;

    if (settings.unitType() == DegreesFahrenheit)
        cv = units::convert<units::fahrenheit, units::kelvin>;
    else if (settings.unitType() == DegreesCelsius)
        cv = units::convert<units::celsius, units::kelvin>;
    if (cv)
    {
        // Convert to Kelvin, ...
        settings.tempLow() = cv(settings.tempLow());
        settings.tempHigh() = cv(settings.tempHigh());
        settings.alarmSet() = cv(settings.alarmSet());
        settings.setpointValue() = cv(settings.setpointValue());
    }
    // ... then to_units.
    settings.unitType() = to_units;
    settings.tempLow() = settings.unitConvert()(settings.tempLow());
    settings.tempHigh() = settings.unitConvert()(settings.tempHigh());
    settings.alarmSet() = settings.unitConvert()(settings.alarmSet());
    settings.setpointValue() = settings.unitConvert()(settings.setpointValue());
}

void formatPidScreen()
{
    // Format all coeff fields when first switching to pid screen.
    pid_prop_field.fmt_ = pid.proportional() < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
    pid_integ_field.fmt_ = pid.integral() < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
    pid_deriv_field.fmt_ = pid.derivative() < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
    pid_gain_field.fmt_ = pid.gain() < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
}

Temperature getTemperature(SensorValue sense_out)
{
    Temperature Tk = tsense(sense_out, AnalogMax<board_type>(), R, Vss, Vbe, Ka, Kb, Kc);

    return settings.unitConvert()(Tk); // Convert Kelvin to current temperature units.
}
