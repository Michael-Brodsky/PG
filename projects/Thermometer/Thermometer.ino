/*
 *	This files demonstrates the implementation of a digital thermometer 
 *  program using the Pg Libraries.
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
 *  displayed on an LCD display, which can be edited from an attached keypad.
 *  The program is designed for an Arduino board attached to a compatible 
 *  LCD/Keypad Shield, although this is not a strict requirement. 
 *  Configuration settings and program-specific types are defined in 
 *  <Thermometer.h>. Settings, such as pin assignments, must set to appropriate 
 *  values for the board used.
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
 *  appears. To undo edits, quickly press and release the <Select> key. 
 *  Pressing and holding the <Up> and <Down> keys while editing will cause 
 *  the values to rapidly scroll at an increasing rate.
 * 
 *  Alarm settings are edited in the ALARM screen. The alarm can be enabled or 
 *  disabled, set to a specific temperature and activated if the sensed 
 *  temperature goes above or below the value. If activated, the alarm can be 
 *  silenced from the run screen by pressing any key. 
 * 
 *  Sensor settings are edited in the SENSOR screen. The sensor minimum and 
 *  maximum values (as returned by the analog to digital converter) can be set 
 *  along with the sensor polling interval in milliseconds. The minimum and 
 *  maximum values must be calibrated to the sensor being used. Incorrect 
 *  settings will cause erroneous temperatures to be displayed.
 * 
 *  The sensor can be calibrated in the CALIBRATE screen. The current ADC 
 *  value returned from the sensor is displayed at the top left along with the  
 *  Lo and Hi range. Navigating to the Lo or Hi fields and pressing either the 
 *  <Up> or <Down> keys will save the current ADC value to that field. 
 *  Pressing the <Up> or <Down> keys in the same field will reset the value  
 *  back to the ADC minimum or maximum value respectively. This cycle can be 
 *  repeated until stable values are found. One way to calibrate the sensor is 
 *  to expose it to the lowest temperature in the thermometer's display range 
 *  and set the Lo field. Then expose the sensor to the highest temperature in 
 *  the thermometer's display range and set the Hi field. Then set the Lo and 
 *  Hi display ranges on the DISPLAY screen to the temperatures that the Lo  
 *  and Hi sensor values represent. This will map the sensor ADC values to the 
 *  correct temperature display values. The Aref field sets whether the ADC 
 *  reference voltage is generated internaly (IN) by the board or by an 
 *  external source (EX).
 * 
 *  Display settings are edited in the DISPLAY screen. The minimum and maximum 
 *  displayed temperatures and the temperature units can be set. The sensor  
 *  ADC value is mapped to a corresponding display value within the minimum 
 *  and maximum range. These values must be chosen carefully and set to the 
 *  actual temperatures that correspond to the sensor's minimum and maximum 
 *  ADC values. Incorrect settings will cause erroneous temperatures to be 
 *  displayed.
 * 
 *  See the comments in <Thermometer.h> for more details.
 * 
 *	**************************************************************************/

#include <LiquidCrystal.h>  // Arduino LCD api.
#include <pg.h>             // Pg environment.
#include "Thermometer.h"    // Thermometer config file.

//
// Function declarations.
//
void initCallback(pin_t, SensorSettings::value_type, const TempSensor::Range*);
void displayCallback();
void sensorCallback(pin_t, SensorSettings::value_type, const TempSensor::Range*);
void keypadCallback(const Keypad::Button*, Keypad::Event); 
void keyPress(const Keypad::Button*);
void keyRelease(const Keypad::Button*);
void keyLongpress(const Keypad::Button*);
void keyRepeat(bool);
void checkAlarm(SensorSettings::value_type);
void scrollField(const Keypad::Button*);
void menuSelect(const Display::Field*);
void setMode(OpMode);
void adjustSettings(const Display::Field*, const Keypad::Button*);
void copySettings(OpMode);
void updateSettings(OpMode);
void restoreSettings(OpMode);
void getSettings(OpMode);
void adjustAlarm(const Display::Field*, Adjustment::Direction);
void adjustSensor(const Display::Field*, Adjustment::Direction);
void adjustCalibration(const Display::Field*, Adjustment::Direction);
void adjustDisplay(const Display::Field*, Adjustment::Direction);

//
// Keypad objects.
//
Keypad::Button right_button(RightButtonTriggerLevel);
Keypad::Button up_button(UpButtonTriggerLevel);
Keypad::Button down_button(DownButtonTriggerLevel);
Keypad::Button left_button(LeftButtonTriggerLevel);
Keypad::Button select_button(SelectButtonTriggerLevel);
Keypad keypad({ &right_button,&up_button,&down_button,&left_button,&select_button },
    KeypadInputPin, keypadCallback, KeypadLongPressMode, KeypadLongPressInterval);

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
Display::Field menu_cal_field = { MenuCalCol,MenuCalRow,MenuCalLab,MenuCalFmt,true };
Display::Field menu_display_field = { MenuDisplayCol,MenuDisplayRow,MenuDisplayLab,MenuDisplayFmt,true };
Display::Screen menu_screen({ &menu_run_field,&menu_alarm_field,&menu_sensor_field,&menu_cal_field,&menu_display_field }, 
    MenuScreenLab);

Display::Field alarm_enable_field = { AlarmEnableCol,AlarmEnableRow,AlarmEnableLab,AlarmEnableFmt,true };
Display::Field alarm_cmp_field = { AlarmCmpCol,AlarmCmpRow,AlarmCmpLab,AlarmCmpFmt,true };
Display::Field alarm_setpoint_field = { AlarmSetPointCol,AlarmSetPointRow,AlarmSetPointLab,AlarmSetPointFmt,true };
Display::Screen alarm_screen({ &alarm_enable_field,&alarm_setpoint_field,&alarm_cmp_field }, AlarmScreenLab);

Display::Field sensor_low_field = { SensorLowCol,SensorLowRow,SensorLowLab,SensorLowFmt,true };
Display::Field sensor_hi_field = { SensorHighCol,SensorHighRow,SensorHighLab,SensorHighFmt,true };
Display::Field sensor_poll_field = { SensorPollCol,SensorPollRow,SensorPollLab,SensorPollFmt,true };
Display::Screen sensor_screen({ &sensor_low_field,&sensor_hi_field,&sensor_poll_field }, SensorScreenLab); 

Display::Field cal_sensor_field = { CalSenseCol,CalSenseRow,CalSenseLab,CalSenseFmt,true };
Display::Field cal_low_field = { CalLowCol,CalLowRow,CalLowLab,CalLowFmt,true };
Display::Field cal_aref_field = { CalArefCol,CalArefRow,CalArefLab,CalArefFmt,true };
Display::Field cal_high_field = { CalHighCol,CalHighRow,CalHighLab,CalHighFmt,true };
Display::Screen calibrate_screen({ &cal_sensor_field,&cal_low_field,&cal_high_field,&cal_aref_field }, CalScreenLab);

Display::Field display_low_field = { DisplayLowCol,DisplayLowRow,DisplayLowLab,DisplayLowFmt,true };
Display::Field display_hi_field = { DisplayHighCol,DisplayHighRow,DisplayHighLab,DisplayHighFmt,true };
Display::Field display_unit_field = { DisplayUnitCol,DisplayUnitRow,DisplayUnitLab,DisplayUnitFmt,true };
Display::Screen display_screen({ &display_low_field,&display_hi_field,&display_unit_field }, DisplayScreenLab);

LiquidCrystal lcd(LcdRs, LcdEn, LcdD4, LcdD5, LcdD6, LcdD7);
Display display(&lcd, displayCallback, &run_screen);

// 
// Temperature sensor objects.
//
TempSensor sensor(SensorInputPin, initCallback); // Set sensor callback for initialization. 
TempFilter filter; // Sensor output is filtered through a moving average filter.

//
// Thermometer settings objects.
// 
SensorSettings::value_type sensor_in = 0;   // Stores the currently polled ADC value from the sensor input.
SensorSettings sensor_settings
    { SensorRangeLow,SensorRangeHigh,SensorPollingMin,SensorPollingMax,SensorPollingInterval }, sensor_copy;
DisplaySettings display_settings
    { DisplayValueMin,DisplayValueMax,DisplayRangeLow,DisplayRangeHigh,CelsiusSymbol }, display_copy;
AlarmSettings alarm_settings
    { AlarmOutputPin,AlarmDisabled,AlarmCmpGreater,SensorAlarmSetPoint,DisplayRangeLow }, alarm_copy;
CalibrationSettings cal_settings{ 0, false, false, ArefInternal };
Adjustment adjustment(SensorAdjustmentFactor, DisplayAdjustmentFactor, AdjustmentMultiplyMax);
OpMode op_mode = OpMode::Init;

//
// Task scheduling objects.
//
ClockCommand keypad_clock(&keypad);
ClockCommand display_clock(&display);
ClockCommand sensor_clock(&sensor);
ClockCommand adjustment_clock(&adjustment);
Scheduler::Task keypad_task(KeypadPollingInterval, &keypad_clock, Scheduler::Task::State::Active);
Scheduler::Task display_task(DisplayRefreshInterval, &display_clock, Scheduler::Task::State::Idle);
Scheduler::Task sensor_task(SensorPollingInterval, &sensor_clock, Scheduler::Task::State::Active);
Scheduler::Task adjustment_task(AdjustmentMultiplyInterval, &adjustment_clock, Scheduler::Task::State::Idle);
Scheduler scheduler{ &keypad_task,&display_task,&sensor_task,&adjustment_task };

void setup() 
{
    lcd.begin(Display::cols(), Display::rows());
    lcd.clear();
    lcd.print("Initializing");
    scheduler.start();
}

void loop() 
{
    scheduler.tick();
}

void initCallback(pin_t, SensorSettings::value_type, const TempSensor::Range*)
{
    pinMode(AlarmOutputPin, OUTPUT);
    alarm_settings.silence(true);
    setMode(OpMode::Run);
    filter.seed(sensor());
    sensor_in = filter.avg();
    display_task.state(Scheduler::Task::State::Active);
    sensor.callback(sensorCallback); // Now set sensor callback for normal ops.
}

void displayCallback()
{
    // Call display.refresh() with values for the current screen/mode.
    switch (op_mode)
    {
    case OpMode::Run:
        display.refresh(
            MapToDisplay::map(
                sensor_in, sensor_settings.low_, sensor_settings.high_, display_settings.low_, display_settings.high_),
                DegreeSymbol, display_settings.unit_, alarm_settings.cmp_.second, alarm_settings.display_value_);
        break;
    case OpMode::Menu:
        display.refresh(MenuItemRun, MenuItemAlarm, MenuItemSensor, MenuItemCalibrate, MenuItemDisplay);
        break;
    case OpMode::Alarm:
        display.refresh(alarm_copy.enabled_.second, alarm_copy.cmp_.second, alarm_copy.display_value_);
        break;
    case OpMode::Sensor:
        display.refresh(sensor_copy.low_, sensor_copy.high_, sensor_copy.tpoll_);
        break;
    case OpMode::Calibrate:
        display.refresh((cal_settings.value_ = filter.avg()), sensor_settings.low_, cal_settings.aref_.second, sensor_settings.high_);
        display.update();
        break;
    case OpMode::Display:
        display.refresh(display_copy.low_, display_copy.high_, display_copy.unit_);
        break;
    default:
        break;
    }
}

void sensorCallback(pin_t pin, SensorSettings::value_type value, const TempSensor::Range* range)
{
    checkAlarm((sensor_in = clamp(filter.avg(value), sensor_settings.low_, sensor_settings.high_)));
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
    case OpMode::Run:
        break;
    case OpMode::Menu:
        break;
    case OpMode::Alarm:
    case OpMode::Sensor:
    case OpMode::Calibrate:
    case OpMode::Display:
    {
        if (button->id() == up_button.id() || button->id() == down_button.id())
            adjustSettings(display.screen()->active_field(), button);
        break;
    }
    default:
        break;
    }
}

void keyRelease(const Keypad::Button* button)
{
    keyRepeat(false);
    switch (op_mode)
    {
    case OpMode::Init:
        break;
    case OpMode::Run:
        alarm_settings.silence(true);
        break;
    case OpMode::Menu:
        if (button->id() == select_button.id())
            menuSelect(display.screen()->active_field());
        else if (button->id() == left_button.id() || button->id() == right_button.id())
            scrollField(button);
        break;
    case OpMode::Calibrate:
        // Sensor settings must be restored on cancel edit event, since we editted them directly.
        if (button->id() == select_button.id())
            restoreSettings(OpMode::Calibrate);
    case OpMode::Alarm:
    case OpMode::Sensor:
    case OpMode::Display:
        if (button->id() == select_button.id())
            setMode(OpMode::Run);
        if (button->id() == left_button.id() || button->id() == right_button.id())
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
        case OpMode::Run:
            setMode(OpMode::Menu);
            break;
        case OpMode::Menu:
            setMode(OpMode::Run);
            break;
        case OpMode::Alarm:
        case OpMode::Sensor:
        case OpMode::Calibrate:
        case OpMode::Display:
            updateSettings(op_mode);
            setMode(OpMode::Run);
            break;
        default:
            break;
        }
    }
    else if ((button->id() == down_button.id() || button->id() == up_button.id()) && op_mode != OpMode::Run)
        keyRepeat(true);
}

void keyRepeat(bool enabled)
{
    keypad.repeat(enabled);
    adjustment.reset();
    adjustment_task.state(enabled ? Scheduler::Task::State::Active : Scheduler::Task::State::Idle);
    adjustment_task.reset();
}

void checkAlarm(SensorSettings::value_type value)
{
    // Activate the alarm if it's not currently active.
    bool alarm_now = alarm_settings.cmp_.first(value, alarm_settings.set_point_);

    if (alarm_settings.enabled_.first && !alarm_settings.active_ && alarm_now)
        alarm_settings.silence(false);
    alarm_settings.active_ = alarm_now;
}

void scrollField(const Keypad::Button* button)
{
    button == &left_button
        ? display.prev()
        : display.next();
}

void menuSelect(const Display::Field* field)
{
    // Set the operating mode according to the cursor position/active field.
    if (field == &menu_run_field)
        setMode(OpMode::Run);
    else if(field == &menu_alarm_field)
        setMode(OpMode::Alarm);
    else if (field == &menu_sensor_field)
        setMode(OpMode::Sensor);
    else if (field == &menu_cal_field)
        setMode(OpMode::Calibrate);
    else if (field == &menu_display_field)
        setMode(OpMode::Display);
}

void setMode(OpMode mode)
{
    // Set the operating mode/active screen if it differs from the current operating mode.
    if (mode != op_mode)
    {
        Display::Cursor display_cursor = Display::Cursor::Block;
        Display::Mode display_mode = Display::Mode::Edit;
        switch (mode)
        {
        case OpMode::Run:
        {
            display.screen(&run_screen);
            display_cursor = Display::Cursor::None;
            display_mode = Display::Mode::Normal;
            break;
        }
        case OpMode::Menu:
            display.screen(&menu_screen);
            break;
        case OpMode::Alarm:
            display.screen(&alarm_screen);
            break;
        case OpMode::Sensor:
            display.screen(&sensor_screen);
            break;
        case OpMode::Calibrate:
            cal_settings.set_low_ = cal_settings.set_high_ = false;
            display.screen(&calibrate_screen);
            break;
        case OpMode::Display:
            display.screen(&display_screen);
            break;
        default:
            break;
        };
        if (display_mode == Display::Mode::Edit)
            copySettings(mode);
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

    // Adjust one of the current screen/mode settings.
    switch (op_mode)
    {
    case OpMode::Run:
        break;
    case OpMode::Alarm:
        adjustAlarm(field, dir);
        break;
    case OpMode::Sensor:
        adjustSensor(field, dir);
        break;
    case OpMode::Calibrate:
        adjustCalibration(field, dir);
        break;
    case OpMode::Display:
        adjustDisplay(field, dir);
        break;
    default:
        break;
    }
    display.update(); // Automatic display updates suspended in edit modes.
}

void copySettings(OpMode mode)
{
    // Create a copy of the current screen/mode settings for editing.
    switch (mode)
    {
    case OpMode::Run:
        break;
    case OpMode::Alarm:
        alarm_copy = alarm_settings;
        break;
    case OpMode::Sensor:
        sensor_copy = sensor_settings;
        break;
    case OpMode::Calibrate:
        sensor_copy = sensor_settings;
        sensor_settings.low_ = 0;       // Set sensor range [0,AnalogMax] so it's not limited to its current range.
        sensor_settings.high_ = AnalogMax<board_type>();
        break;
    case OpMode::Display:
        display_copy = display_settings;
        break;
    default:
        break;
    }
}

void updateSettings(OpMode mode)
{
    // Update the settings for the current screen/mode from the copy.
    switch (mode)
    {
    case OpMode::Run:
        break;
    case OpMode::Alarm:
        alarm_settings = alarm_copy;
        alarm_settings.set_point_ = MapToSensor::map(
            alarm_settings.display_value_, display_settings.low_, display_settings.high_,
            sensor_settings.low_, sensor_settings.high_); // Alarm set point edited using display values, map back to sensor values.
        alarm1_field.visible_ = alarm2_field.visible_ = alarm_settings.enabled_.first; // Alarm settings only visible if alarm enabled.
        break;
    case OpMode::Sensor:
        sensor_settings = sensor_copy;
        break;
    case OpMode::Calibrate:
        // Restore sensor settings not set in calibration mode.
        if (!cal_settings.set_high_)
            sensor_settings.high_ = sensor_copy.high_;
        if (!cal_settings.set_low_)
            sensor_settings.low_ = sensor_copy.low_;
        // Set the aref source.
        analogReference(cal_settings.aref_.first == CalibrationSettings::ArefSource::Internal
            ? DEFAULT : EXTERNAL);
        break;
    case OpMode::Display:
        display_settings = display_copy;
        break;
    default:
        break;
    }
}

void restoreSettings(OpMode mode)
{
    switch (mode)
    {
    case OpMode::Run:
        break;
    case OpMode::Alarm:
        break;
    case OpMode::Sensor:
        break;
    case OpMode::Calibrate:
        sensor_settings = sensor_copy;
        break;
    case OpMode::Display:
        break;
    default:
        break;
    }
}

void getSettings(OpMode mode)
{
    // Retrieve settings from eeprom.
    switch (mode)
    {
    case OpMode::Run:
        break;
    case OpMode::Alarm:
        break;
    case OpMode::Sensor:
        break;
    case OpMode::Calibrate:
        break;
    case OpMode::Display:
        break;
    default:
        break;
    }
}

void adjustAlarm(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &alarm_enable_field)
        alarm_copy.enabled_ = alarm_copy.enabled_ == AlarmDisabled
        ? AlarmEnabled
        : AlarmDisabled;
    else if (field == &alarm_cmp_field)
        alarm_copy.cmp_ = alarm_copy.cmp_ == AlarmCmpLess
            ? AlarmCmpGreater
            : AlarmCmpLess;
    else if (field == &alarm_setpoint_field)
    {
        auto inc = adjustment.value(Adjustment::display_type(), dir);

        alarm_copy.display_value_ = wrap<DisplaySettings::value_type, DisplaySettings::value_type>(
            alarm_copy.display_value_, inc, display_settings.low_, display_settings.high_);
    }
}

void adjustSensor(const Display::Field* field, Adjustment::Direction dir)
{
    auto inc = adjustment.value(Adjustment::sensor_type(), dir);

    if (field == &sensor_low_field)
        sensor_copy.low_ = wrap<SensorSettings::value_type, Adjustment::sensordiff_type>(
            sensor_copy.low_, inc, 0, AnalogMax<board_type>());
    else if (field == &sensor_hi_field)
        sensor_copy.high_ = wrap<SensorSettings::value_type, Adjustment::sensordiff_type>(
            sensor_copy.high_, inc, 0, AnalogMax<board_type>());
    else if (field == &sensor_poll_field)
        sensor_copy.tpoll_ = SensorSettings::duration_type(
            wrap<SensorSettings::value_type, Adjustment::sensordiff_type>(
                sensor_copy.tpoll_.count(), inc, sensor_copy.tmin_.count(), sensor_copy.tmax_.count()));
}

void adjustCalibration(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &cal_aref_field)
        cal_settings.aref_ = cal_settings.aref_ == ArefInternal 
            ? ArefExternal
            : ArefInternal;
    else if (field == &cal_low_field)
    {
        sensor_settings.low_ = (cal_settings.set_low_ = !cal_settings.set_low_)
            ? cal_settings.value_
            : 0;
    }
    else if (field == &cal_high_field)
    {
        sensor_settings.high_ = (cal_settings.set_high_ = !cal_settings.set_high_)
            ? cal_settings.value_
            : AnalogMax<board_type>();
    }
}

void adjustDisplay(const Display::Field* field, Adjustment::Direction dir)
{

    if (field == &display_unit_field)
        display_copy.unit_ = display_copy.unit_ == CelsiusSymbol
            ? FarenheitSymbol
            : display_copy.unit_ == FarenheitSymbol
                ? KelvinSymbol
                : CelsiusSymbol;
    else
    {
        auto inc = adjustment.value(Adjustment::display_type(), dir);

        if (field == &display_low_field)
            display_copy.low_ = wrap<DisplaySettings::value_type, DisplaySettings::value_type>(
                display_copy.low_, inc, display_copy.min_, display_copy.max_);
        else if (field == &display_hi_field)
            display_copy.high_ = wrap<DisplaySettings::value_type, DisplaySettings::value_type>(
                display_copy.high_, inc, display_copy.min_, display_copy.max_);

    }
}