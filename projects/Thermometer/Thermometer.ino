/*
 *	This files demonstrates the implementation of a programmable digital 
 *  thermometer.
 *
 *	***************************************************************************
 *
 *	File: Thermometer.ino
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
 *	**************************************************************************
 *
 *	Description:
 *
 *  This file demonstrates a programmable digital thermometer built using the 
 *  Pg Libraries. It is built with common components found in many industrial 
 *  automation applications: data acquisition, process control and user 
 *  interfaces. Temperature is acquired by polling a sensor attached to an 
 *  analog input, the program controls an alarm that sounds if the temperature 
 *  exceeds a specified value, and the temperature and program settings are 
 *  displayed on an LCD display, which can be edited from an attached keypad 
 *  and stored/recalled from EEPROM.
 * 
 *  The program is designed for an Arduino board attached to a compatible 
 *  LCD/Keypad Shield, although this is not a strict requirement. The 
 *  The temperature sensing algorithms are based on a standard 10K thermistor 
 *  attached to one of the Arduino's analog inputs. Configuration and hardware 
 *  settings are defined in <config.h> Settings, such as pin assignments, must 
 *  be set to appropriate values for the board used.
 * 
 *  The program has a normal "run" mode that displays the current temperature 
 *  and several editing modes used to edit the program settings. On power-up, 
 *  the program enters the run mode. The keypad is used to open a menu and  
 *  select one of the editting modes from run mode. To open the menu screen, 
 *  press and hold the <Select> key. Press the <Left> and <Right> keys to 
 *  navigate menu items and press the <Select> key to select an item. Once the 
 *  editing screen opens, press the <Left> and <Right> keys to navigate
 *  the fields and the <Up> and <Down> keys to change the settings. To save 
 *  the settings, press and hold the <Select> key until the "run" screen 
 *  appears. To undo edits, quickly press and release the <Select> key. This 
 *  returns the program to run mode without saving any changes. Pressing and 
 *  holding the <Up> and <Down> keys while editing will cause the values to 
 *  rapidly scroll at an increasing rate.
 * 
 *  Alarm settings are edited in the ALARM screen. The alarm can be enabled or 
 *  disabled, set to a specific temperature and activated if the sensed 
 *  temperature goes above or below the value. If activated, the alarm can be 
 *  silenced from the run screen by pressing any key. 
 * 
 *  Sensor settings are edited in the SENSOR screen. The Aref field sets 
 *  whether the analog reference voltage is generated internaly (IN) by the 
 *  board or by an external source (EX). The sensor polling interval, in 
 *  milliseconds, can be adjusted up or down between a fixed minimum and 
 *  maximum value.
 * 
 *  Display settings are edited in the DISPL screen. The temperature display 
 *  range and units can be set. The display units field cycles between 
 *  Celsius, Fahrenheit and Kelvin, and effects the displayed temperature, 
 *  alarm set point and display range. The display range Lo and Hi fields 
 *  limit the displayed temperature and alarm set point between those values, 
 *  regardless of the sensed temperature.
 * 
 *  See the comments in <Thermometer.h> for more details.
 * 
 *	**************************************************************************/

#include <LiquidCrystal.h>  // Arduino LCD api.
#include <pg.h>             // Pg environment.
#include "config.h"         // Thermometer types, constants and defaults.

//
// Function declarations.
//

void initSensor();
void displayCallback();
void sensorCallback();
void keypadCallback(const Keypad::Button*, Keypad::Event); 
void keyPress(const Keypad::Button*);
void keyRelease(const Keypad::Button*);
void keyLongpress(const Keypad::Button*);
void keyRepeat(bool);
void checkAlarm(Thermometer::value_type);
void scrollField(const Keypad::Button*);
void menuSelect(const Display::Field*);
void setMode(ThermometerMode);
void adjustSettings(const Display::Field*, const Keypad::Button*);
void updateSettings(ThermometerMode);
void readSettings();
void writeSettings();
void adjustAlarm(const Display::Field*, Adjustment::Direction);
void adjustSensor(const Display::Field*, Adjustment::Direction);
void adjustDisplay(const Display::Field*, Adjustment::Direction);
void switchUnits(ThermometerAlarm&, ThermometerDisplay&, ThermometerDisplayUnit);
Thermometer::value_type getTemperature();

///////////////////////////////////////////////////////
// Thermometer settings objects.                     //
//                                                   //
// These are pairs of program and display values.    //
///////////////////////////////////////////////////////

const SensorAref SensorArefInternal{ SensorArefSource::Internal,InternalSymbol };
const SensorAref SensorArefExternal{ SensorArefSource::External,ExternalSymbol };
const AlarmEnable AlarmDisabled{ false,NoSymbol };
const AlarmEnable AlarmEnabled{ true,YesSymbol };
const AlarmCompare AlarmCmpLess{ alarm_lt,LessSymbol };
const AlarmCompare AlarmCmpGreater{ alarm_gt,GreaterSymbol };
const ThermometerDisplayUnit DegreesKelvin{ temperature<units::kelvin>,KelvinSymbol };
const ThermometerDisplayUnit DegreesCelsius{ temperature<units::celsius>,CelsiusSymbol };
const ThermometerDisplayUnit DegreesFahrenheit{ temperature<units::fahrenheit>,FarenheitSymbol };

//
// Keypad objects.
//

Keypad::Button right_button(RightButtonTriggerLevel);
Keypad::Button up_button(UpButtonTriggerLevel);
Keypad::Button down_button(DownButtonTriggerLevel);
Keypad::Button left_button(LeftButtonTriggerLevel);
Keypad::Button select_button(SelectButtonTriggerLevel);
Keypad keypad(KeypadInputPin, keypadCallback, 
    { &right_button,&up_button,&down_button,&left_button,&select_button },
    KeypadLongPressMode, KeypadLongPressInterval);
auto isNavButton = [](const Keypad::Button* button) 
{ return button->id() == right_button.id() || button->id() == left_button.id(); };
auto isAdjustButton = [](const Keypad::Button* button) 
{ return button->id() == up_button.id() || button->id() == down_button.id(); };

// 
// Display objects
//

Display::Field temp_field = { TempCol,TempRow,TempLab,TempFmt,true };
Display::Field symbol_field = { SymbolCol,SymbolRow,SymbolLab,SymbolFmt,true };
Display::Field unit_field = { UnitCol,UnitRow,UnitLab,UnitFmt,true };
Display::Field alarm1_field = { Alarm1Col,Alarm1Row,Alarm1Lab,Alarm1Fmt,false };
Display::Field alarm2_field = { Alarm2Col,Alarm2Row,Alarm2Lab,Alarm2Fmt,false };
Display::Screen run_screen({ &temp_field,&symbol_field,&unit_field,&alarm1_field,&alarm2_field }, RunScreenLab);

Display::Field menu_run_field = { MenuRunCol,MenuRunRow,MenuRunLab,MenuRunFmt,true };
Display::Field menu_alarm_field = { MenuAlarmCol,MenuAlarmRow,MenuAlarmLab,MenuAlarmFmt,true };
Display::Field menu_sensor_field = { MenuSensorCol,MenuSensorRow,MenuSensorLab,MenuSensorFmt,true };
Display::Field menu_display_field = { MenuDisplayCol,MenuDisplayRow,MenuDisplayLab,MenuDisplayFmt,true };
Display::Screen menu_screen({ &menu_run_field,&menu_alarm_field,&menu_sensor_field,&menu_display_field }, 
    MenuScreenLab);

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

// 
// Thermometer objects.
//

TemperatureSensor temp_sensor(SensorInputPin, sensorCallback);
InputFilter temp_filter;
Thermometer thermometer{
    ThermometerDisplay(DisplayRangeLow,DisplayRangeHigh,DegreesCelsius),
    ThermometerAlarm(AlarmOutputPin,AlarmDisabled,AlarmCmpGreater,AlarmSetPoint),
    Thermometer::sensor_type(SensorArefInternal,SensorPollingInterval)
};
Thermometer::value_type Tsense = 0;

//
// Misc. program objects.
//

Adjustment adjustment(SensorAdjustmentFactor, DisplayAdjustmentFactor, AdjustmentMultiplyMax);
ThermometerMode op_mode = ThermometerMode::Init;
EEStream eeprom;

//
// Task scheduling objects.
//

ClockCommand keypad_clock(&keypad);
ClockCommand display_clock(&display);
ClockCommand sensor_clock(&temp_sensor);
ClockCommand adjustment_clock(&adjustment);
ScheduledTask keypad_task(KeypadPollingInterval, &keypad_clock, TaskState::Active);
ScheduledTask display_task(DisplayRefreshInterval, &display_clock, TaskState::Active);
ScheduledTask sensor_task(SensorPollingInterval, &sensor_clock, TaskState::Active);
ScheduledTask adjustment_task(AdjustmentMultiplyInterval, &adjustment_clock, TaskState::Idle);
Scheduler scheduler{ &keypad_task,&display_task,&sensor_task,&adjustment_task };

void setup() 
{
    lcd.begin(Display::cols(), Display::rows());
    lcd.clear();
    lcd.print("Initializing");
    thermometer.alarm.attach();
    thermometer.alarm.silence(true);
    readSettings();
    initSensor();
    setMode(ThermometerMode::Run);
    scheduler.start();
}

void loop() 
{
    scheduler.tick();
}

void initSensor()
{
    delay(SensorInitDelay.count()); // Wait a sec for sensor/ADC to stabilize.
    temp_filter.seed(temp_sensor());
    Tsense = getTemperature();
}

void displayCallback()
{
    // Call display.refresh() with values for the current screen.
    switch (op_mode)
    {
    case ThermometerMode::Run:
        display.refresh(Tsense, DegreeSymbol, thermometer.display.units.second, 
            thermometer.alarm.compare.second, thermometer.alarm.setpoint);
        break;
    case ThermometerMode::Menu:
        display.refresh(MenuItemRun, MenuItemAlarm, MenuItemSensor, MenuItemDisplay);
        break;
    case ThermometerMode::Alarm:
        display.refresh(thermometer.alarm_copy.enabled.second, thermometer.alarm_copy.setpoint, 
            thermometer.alarm_copy.compare.second);
        break;
    case ThermometerMode::Sensor:
        display.refresh(thermometer.sensor_copy.aref.second, thermometer.sensor_copy.tpoll);
        break;
    case ThermometerMode::Display:
        display.refresh(thermometer.display_copy.low, thermometer.display_copy.high, thermometer.display_copy.units.second);
        break;
    default:
        break;
    }
}

void sensorCallback()
{
    checkAlarm((Tsense = getTemperature()));
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

void keyPress(const Keypad::Button* button)
{
    switch (op_mode)
    {
    case ThermometerMode::Alarm:
    case ThermometerMode::Sensor:
    case ThermometerMode::Display:
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
    case ThermometerMode::Run:
        thermometer.alarm.silence(true);
        break;
    case ThermometerMode::Menu:
        if (button->id() == select_button.id())
            menuSelect(display.screen()->active_field());
        else if (isNavButton(button))
            scrollField(button);
        break;
    case ThermometerMode::Alarm:
    case ThermometerMode::Sensor:
    case ThermometerMode::Display:
        if (button->id() == select_button.id())
            setMode(ThermometerMode::Run);
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
        case ThermometerMode::Run:
            setMode(ThermometerMode::Menu);
            break;
        case ThermometerMode::Menu:
            setMode(ThermometerMode::Run);
            break;
        case ThermometerMode::Alarm:
        case ThermometerMode::Sensor:
        case ThermometerMode::Display:
            updateSettings(op_mode);
            writeSettings();
            setMode(ThermometerMode::Run);
            break;
        default:
            break;
        }
    }
    else if (isAdjustButton(button) && op_mode != ThermometerMode::Run)
        keyRepeat(true);
}

void keyRepeat(bool enabled)
{
    keypad.repeat(enabled);
    adjustment.reset();
    adjustment_task.state(enabled ? TaskState::Active : TaskState::Idle);
    adjustment_task.reset();
}

void checkAlarm(Thermometer::value_type value)
{
    static bool alarm_active = false;
    bool alarm_now = thermometer.alarm.compare.first(value, thermometer.alarm.setpoint);

    if (thermometer.alarm.enabled.first && !alarm_active && alarm_now)
        thermometer.alarm.silence(false);
    alarm_active = alarm_now;
}

void scrollField(const Keypad::Button* button)
{
    button == &left_button
        ? display.prev()
        : display.next();
}

void menuSelect(const Display::Field* field)
{
    // Set the operating mode according to the active field.
    // Active field determined by display cursor location.
    if (field == &menu_run_field)
        setMode(ThermometerMode::Run);
    else if(field == &menu_alarm_field)
        setMode(ThermometerMode::Alarm);
    else if (field == &menu_sensor_field)
        setMode(ThermometerMode::Sensor);
    else if (field == &menu_display_field)
        setMode(ThermometerMode::Display);
}

void setMode(ThermometerMode mode)
{
    // Set the operating mode/active screen if it differs from the current one.
    if (mode != op_mode)
    {
        Display::Cursor display_cursor = Display::Cursor::Block;
        Display::Mode display_mode = Display::Mode::Edit;
        switch (mode)
        {
        case ThermometerMode::Run:
            display.screen(&run_screen);
            display_cursor = Display::Cursor::None;
            display_mode = Display::Mode::Normal;
            break;
        case ThermometerMode::Menu:
            display.screen(&menu_screen);
            break;
        case ThermometerMode::Alarm:
            display.screen(&alarm_screen);
            break;
        case ThermometerMode::Sensor:
            display.screen(&sensor_screen);
            break;
        case ThermometerMode::Display:
            display.screen(&display_screen);
            break;
        default:
            break;
        };
        if (display_mode == Display::Mode::Edit)
            thermometer.copySettings();
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
    // Active field determined by display cursor location.
    switch (op_mode)
    {
    case ThermometerMode::Alarm:
        adjustAlarm(field, dir);
        break;
    case ThermometerMode::Sensor:
        adjustSensor(field, dir);
        break;
    case ThermometerMode::Display:
        adjustDisplay(field, dir);
        break;
    default:
        break;
    }
    display.update(); // Automatic display updates suspended in edit modes.
}

void updateSettings(ThermometerMode mode)
{
    // Make the editted settings active.
    if (mode != ThermometerMode::Init)
        thermometer.updateSettings();
    // Update the program from the active settings.
    switch (mode)
    {
    case ThermometerMode::Init:
        sensor_task.interval(thermometer.sensor.tpoll);
        thermometer.sensor.setAref();
    case ThermometerMode::Alarm:
        alarm1_field.visible_ = alarm2_field.visible_ = thermometer.alarm.enabled.first;
        break;
    case ThermometerMode::Sensor:
        sensor_task.interval(thermometer.sensor.tpoll);
        thermometer.sensor.setAref();
        break;
    case ThermometerMode::Display:
        Tsense = getTemperature();
        break;
    default:
        break;
    }
}

void readSettings()
{
    // Retrieve settings from eeprom.
    long eeprom_id;

    eeprom.reset();
    eeprom >> eeprom_id;
    if (eeprom_id == EepromID)
    {
        thermometer.display.deserialize(eeprom);
        thermometer.alarm.deserialize(eeprom);
        thermometer.sensor.deserialize(eeprom);
    }
    else
        writeSettings();
    updateSettings(op_mode); 
}

void writeSettings()
{
    // Save settings to eeprom.
    eeprom.reset();
    eeprom << EepromID;
    thermometer.display.serialize(eeprom);
    thermometer.alarm.serialize(eeprom);
    thermometer.sensor.serialize(eeprom);
}

void adjustAlarm(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &alarm_enable_field)
        thermometer.alarm_copy.enabled = thermometer.alarm_copy.enabled == AlarmDisabled
        ? AlarmEnabled
        : AlarmDisabled;
    else if (field == &alarm_cmp_field)
        thermometer.alarm_copy.compare = thermometer.alarm_copy.compare == AlarmCmpLess
        ? AlarmCmpGreater
        : AlarmCmpLess;
    else if (field == &alarm_setpoint_field)
    {
        auto inc = adjustment.value(Thermometer::value_type(), dir);

        thermometer.alarm_copy.setpoint = wrap<Thermometer::value_type, Thermometer::value_type>(
            thermometer.alarm_copy.setpoint, inc, thermometer.display.low, thermometer.display.high);
    }
}

void adjustSensor(const Display::Field* field, Adjustment::Direction dir)
{
    auto inc = adjustment.value(Adjustment::sensor_type(), dir);

    if (field == &sensor_poll_field)
        thermometer.sensor_copy.tpoll = Thermometer::duration(
            wrap<TemperatureSensor::value_type, Adjustment::sensordiff_type>(
                thermometer.sensor_copy.tpoll.count(), inc, SensorPollingMin.count(), SensorPollingMax.count()));
    else if (field == &sensor_aref_field)
        thermometer.sensor_copy.aref = thermometer.sensor_copy.aref == SensorArefExternal
            ? SensorArefInternal
            : SensorArefExternal;
}

void adjustDisplay(const Display::Field* field, Adjustment::Direction dir)
{

    if (field == &display_unit_field)
    {
        auto to_units = thermometer.display_copy.units == DegreesCelsius
            ? DegreesFahrenheit
            : thermometer.display_copy.units == DegreesFahrenheit
            ? DegreesKelvin
            : DegreesCelsius;

        switchUnits(thermometer.alarm_copy, thermometer.display_copy, to_units);
    }
    else
    {
        auto inc = adjustment.value(Thermometer::value_type(), dir);

        if (field == &display_low_field)
            thermometer.display_copy.low = wrap<Thermometer::value_type, Thermometer::value_type>(
                thermometer.display_copy.low, inc, DisplayValueMin, DisplayValueMax);
        else if (field == &display_hi_field)
            thermometer.display_copy.high = wrap<Thermometer::value_type, Thermometer::value_type>(
                thermometer.display_copy.high, inc, DisplayValueMin, DisplayValueMax);

    }
}

void switchUnits(ThermometerAlarm& alarm, ThermometerDisplay& display, ThermometerDisplayUnit to_units)
{
    typename ThermometerDisplayUnit::first_type cv = nullptr;

    if (display.units == DegreesFahrenheit)
        cv = units::convert<units::fahrenheit, units::kelvin>;
    else if (display.units == DegreesCelsius)
        cv = units::convert<units::celsius, units::kelvin>;
    if (cv)
    {
        // Convert to Kelvin, ...
        display.low = cv(display.low);
        display.high = cv(display.high);
        alarm.setpoint = cv(alarm.setpoint);
    }
    // ... then to_units.
    display.units = to_units;
    display.low = display.units.first(display.low);
    display.high = display.units.first(display.high);
    alarm.setpoint = display.units.first(alarm.setpoint);
}

Thermometer::value_type getTemperature()
{
    // Convert degrees K to display units.
    return thermometer.display.units.first(tsense<Thermometer::value_type, TemperatureSensor::value_type>(
        temp_filter.avg(temp_sensor.value()), SensorADCMax));
}