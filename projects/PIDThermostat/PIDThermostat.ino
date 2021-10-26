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

#pragma region Short Typedefs

using ScheduledTask = Scheduler::Task;
using TaskState = ScheduledTask::State;
using SensorAref = Settings::sensor_aref_type;
using SensorArefSource = ArefSource;
using AlarmEnable = Settings::enable_type;
using AlarmCompare = Settings::alarm_compare_type;
using SetpointEnable = Settings::enable_type;
using DisplayUnit = Settings::unit_type;

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
void checkAlarm(Settings::display_type);
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
Settings::display_type getTemperature(Settings::adc_type);

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

#pragma endregion
#pragma region Display Management Objects

Display::Field pvval_field = { PvValCol,PvValRow,PvValLab,PvValFmt,true };
Display::Field pvsym_field = { PvSymCol,PvSymRow,PvSymLab,PvSymFmt,true };
Display::Field pvunit_field = { PvUnitCol,PvUnitRow,PvUnitLab,PvUnitFmt,true };
Display::Field spval_field = { SpValCol,SpValRow,SpValLab,SpValFmt,true };
Display::Field spen_field = { SpEnCol,SpEnRow,SpEnLab,SpEnFmt,true };
Display::Field alrmen_field = { AlrmEnCol,AlrmEnRow,AlrmEnLab,AlrmEnFmt,true };
Display::Screen run_screen({ &pvval_field,&pvsym_field,&pvunit_field,&spval_field,&spen_field,&alrmen_field }, RunScreenLab);

Display::Field menu_run_field = { MenuRunCol,MenuRunRow,MenuRunLab,MenuRunFmt,true };
Display::Field menu_pid_field = { MenuPidCol,MenuPidRow,MenuPidLab,MenuPidFmt,true };
Display::Field menu_alarm_field = { MenuAlarmCol,MenuAlarmRow,MenuAlarmLab,MenuAlarmFmt,true };
Display::Field menu_sensor_field = { MenuSensorCol,MenuSensorRow,MenuSensorLab,MenuSensorFmt,true };
Display::Field menu_display_field = { MenuDisplayCol,MenuDisplayRow,MenuDisplayLab,MenuDisplayFmt,true };
Display::Screen menu_screen({ &menu_run_field,&menu_pid_field,&menu_alarm_field,&menu_sensor_field,&menu_display_field },
    MenuScreenLab);

Display::Field pid_p_field = { PCol,PRow,PLab,PFmt,true };
Display::Field pid_i_field = { ICol,IRow,ILab,IFmt,true };
Display::Field pid_d_field = { DCol,DRow,DLab,DFmt,true };
Display::Field pid_a_field = { ACol,ARow,ALab,AFmt,true };
Display::Screen pid_screen({ &pid_p_field,&pid_i_field,&pid_d_field,&pid_a_field },
    PidScreenLab);

Display::Field alarm_enable_field = { AlarmEnableCol,AlarmEnableRow,AlarmEnableLab,AlarmEnableFmt,true };
Display::Field alarm_cmp_field = { AlarmCmpCol,AlarmCmpRow,AlarmCmpLab,AlarmCmpFmt,true };
Display::Field alarm_setpoint_field = { AlarmSetPointCol,AlarmSetPointRow,AlarmSetPointLab,AlarmSetPointFmt,true };
Display::Screen alarm_screen({ &alarm_enable_field,&alarm_setpoint_field,&alarm_cmp_field }, AlarmScreenLab);

Display::Field sensor_aref_field = { SensorArefCol,SensorArefRow,SensorArefLab,SensorArefFmt,true };
Display::Field sensor_poll_field = { SensorPollCol,SensorPollRow,SensorPollLab,SensorPollFmt,true };
Display::Screen sensor_screen({ &sensor_aref_field,&sensor_poll_field }, SensorScreenLab);

Display::Field display_low_field = { DisplayLowCol,DisplayLowRow,DisplayLowLab,DisplayLowFmt,true };
Display::Field display_hi_field = { DisplayHighCol,DisplayHighRow,DisplayHighLab,DisplayHighFmt,true };
Display::Field display_unit_field = { DisplayUnitCol,DisplayUnitRow,DisplayUnitLab,DisplayUnitFmt,true };
Display::Screen display_screen({ &display_low_field,&display_hi_field,&display_unit_field }, DisplayScreenLab);

LiquidCrystal lcd(LcdRs, LcdEn, LcdD4, LcdD5, LcdD6, LcdD7);
Display display(&lcd, displayCallback, &run_screen);

#pragma endregion
#pragma region Thermostat Objects

Settings settings{
    DisplayRangeLow,DisplayRangeHigh,DegreesCelsius,
    PidProportional,PidIntegral,PidDerivative,PidGain,
    SetpointDisabled,DisplayRangeLow,
    AlarmDisabled,AlarmCmpGreater,AlarmSetPoint,
    SensorArefInternal,SensorPollingInterval
}, settings_copy;
TemperatureSensor temp_sensor(SensorInput, sensorCallback);
InputFilter temp_filter;
Pid pid(settings.setpointValue(), PidProportional, PidIntegral, PidDerivative, PidGain);
Pwm pwm(PwmOut);
Settings::display_type Tsense = 0;

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
        if (!alarmSilence(AlarmOutput))
            alarmSilence(AlarmOutput, true);
        else if (isNavButton(button)) // SP & AL settings adjustable in RUN mode.
        {
            setMode(ThermostatMode::Setpoint);
            run_screen.active_field(button == &right_button ? &spval_field : &alrmen_field);
            display.update();
        }
        break;
    case ThermostatMode::Menu:
        if (button->id() == select_button.id())
            menuSelect(display.screen()->active_field());
        else if (isNavButton(button))
            scrollField(button);
        break;
    case ThermostatMode::Pid:
    case ThermostatMode::Setpoint:
    case ThermostatMode::Alarm:
    case ThermostatMode::Sensor:
    case ThermostatMode::Display:
        if (button->id() == select_button.id())
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
    if (button->id() == select_button.id())
    {
        switch (op_mode)
        {
        case ThermostatMode::Run:
            setMode(ThermostatMode::Menu);
            break;
        case ThermostatMode::Menu:
            setMode(ThermostatMode::Run);
            break;
        case ThermostatMode::Pid:
        case ThermostatMode::Setpoint:
        case ThermostatMode::Alarm:
        case ThermostatMode::Sensor:
        case ThermostatMode::Display:
            updateSettings(op_mode);
            writeSettings();
            setMode(ThermostatMode::Run);
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
    static bool release = true; // Suspends release event after long-press event.

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
    // Send display values for the current screen.
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
            settings_copy.pidDerivative(), settings_copy.pidGain());
        break;
    case ThermostatMode::Alarm:
        display.refresh(settings_copy.alarmEnableSymbol(), settings_copy.alarmSetpoint(),
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
    checkAlarm((Tsense = getTemperature(temp_filter.avg(temp_sensor.value()))));
    Pid::value_type measured_value = clamp(Tsense, settings.tempLow(), settings.tempHigh());
    Pid::value_type control_value = clamp(pid.loop(measured_value), 0.0f, settings.tempHigh() - settings.tempLow());
    Pid::value_type output_value = norm(control_value, 0.0f, settings.tempHigh() - settings.tempLow(), 0.0f, 1.0f);
    pwm.duty_cycle(output_value);

}

void initSensor()
{
    delay(SensorInitDelay.count()); // Wait a sec for sensor/ADC to stabilize.
    temp_filter.seed(temp_sensor());
    Tsense = getTemperature(temp_filter.avg(temp_sensor.value()));
    pid.start(steady_clock::now());
}

void checkAlarm(Settings::display_type value)
{
    static bool alarm_active = false;
    bool alarm_now = settings.alarmCompare()(value, settings.alarmSetpoint());

    if (settings.alarmEnabled() && !alarm_active && alarm_now)
        alarmSilence(AlarmOutput, false);
    alarm_active = alarm_now && settings.alarmEnabled();
}

void scrollField(const Keypad::Button* button)
{
    button == &left_button
        ? display.prev()
        : display.next();
    switch (op_mode)
    {
    case ThermostatMode::Setpoint:
    {
        // Skip PV fields.
        const Display::Field* field = (run_screen.active_field() == &pvval_field)
            ? &spval_field
            : run_screen.active_field() == &pvunit_field
            ? &alrmen_field
            : run_screen.active_field();
        run_screen.active_field(field);
        break;
    }
    default:
        break;
    }
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
            // This is redundant, but fits the copy/update settings scheme.
            settings.pidProportional() = pid.proportional();
            settings.pidIntegral() = pid.integral();
            settings.pidDerivative() = pid.derivative();
            settings.pidGain() = pid.gain();
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
    // Active field determined by display cursor position.
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
    // Make the editted settings active.
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
    Tsense = getTemperature(temp_filter.avg(temp_sensor.value()));
}

void readSettings()
{
    long eeprom_id;

    eeprom.reset();
    eeprom >> eeprom_id;
    if (eeprom_id == EepromID) // Check if EEPROM data is valid.
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
    eeprom << EepromID; // Save the eeprom id so we know it's valid when retrieved.
    settings.serialize(eeprom);
}

void adjustSetpoint(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &spval_field)
    {
        auto inc = adjustment.value(DisplayAdjustmentFactor, dir);

        settings_copy.setpointValue() = wrap<Settings::display_type, Settings::display_type>(
            settings_copy.setpointValue(), inc, settings.tempLow(), settings.tempHigh());
    }
    else if (field == &spen_field)
        settings_copy.setpointEnType() = settings_copy.setpointEnabled() 
        ? SetpointDisabled
        : SetpointEnabled;
    if (field == &alrmen_field)
        settings_copy.alarmEnType() = settings_copy.alarmEnabled() 
        ? AlarmDisabled
        : AlarmEnabled;
}

void adjustPid(const Display::Field* field, Adjustment::Direction dir)
{
    auto inc = adjustment.value(DisplayAdjustmentFactor, dir);

    Settings::display_type& coeff = field == &pid_p_field
        ? settings_copy.pidProportional()
        : field == &pid_i_field
        ? settings_copy.pidIntegral()
        : field == &pid_d_field
        ? settings_copy.pidDerivative()
        : settings_copy.pidGain();

    coeff = wrap<Settings::display_type, Settings::display_type>(
        (const Settings::display_type &)coeff, inc, PidValueMin, PidValueMax);
}

void adjustAlarm(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &alarm_enable_field)
        settings_copy.alarmEnType() = settings_copy.alarmEnabled()
        ? AlarmDisabled
        : AlarmEnabled;
    else if (field == &alarm_cmp_field)
        settings_copy.alarmCmpType() = settings_copy.alarmCompareSymbol() == LessSymbol 
        ? AlarmCmpGreater
        : AlarmCmpLess;
    else if (field == &alarm_setpoint_field)
    {
        auto inc = adjustment.value(DisplayAdjustmentFactor, dir);

        settings_copy.alarmSetpoint() = wrap< Settings::display_type, Settings::display_type>(
            settings_copy.alarmSetpoint(), inc, settings.tempLow(), settings.tempHigh());
    }
}

void adjustSensor(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &sensor_poll_field)
    {
         auto inc = adjustment.value(SensorAdjustmentFactor, dir);

         settings_copy.sensorPollIntvl() = Settings::duration_type(
              wrap<Settings::adc_type, std::make_signed<Settings::adc_type>::type>(
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
        auto inc = adjustment.value(DisplayAdjustmentFactor, dir);
        Settings::display_type& value = field == &display_low_field
            ? settings_copy.tempLow()
            : settings_copy.tempHigh();

        value = wrap< Settings::display_type, Settings::display_type>(
            (const Settings::display_type &)value, inc, DisplayValueMin, DisplayValueMax);
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
        settings.alarmSetpoint() = cv(settings.alarmSetpoint());
        settings.setpointValue() = cv(settings.setpointValue());
    }
    // ... then to_units.
    settings.unitType() = to_units;
    settings.tempLow() = settings.unitConvert()(settings.tempLow());
    settings.tempHigh() = settings.unitConvert()(settings.tempHigh());
    settings.alarmSetpoint() = settings.unitConvert()(settings.alarmSetpoint());
    settings.setpointValue() = settings.unitConvert()(settings.setpointValue());
}

Settings::display_type getTemperature(Settings::adc_type sense_out)
{
    Settings::display_type Tk = tsense(sense_out, AnalogMax<board_type>(), R, Vss, Vbe, Ka, Kb, Kc);

    return settings.unitConvert()(Tk);
}
