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

#include <LiquidCrystal.h>
#include <pg.h>
#include "Thermostat.h"
#include "config.h"

using namespace pg;
using namespace std::chrono;

void setDefaults(Settings&);
void keyPress(const Keypad::Button*);
void keyRelease(const Keypad::Button*);
void keyLongpress(const Keypad::Button*);
void keyRepeat(bool);
void keypadCallback(const Keypad::Button*, Keypad::Event);
void displayCallback();
void sensorCallback();
void initSensor();
void checkAlarm(data_t);
void scrollField(const Keypad::Button*);
void menuSelect(const Display::Field*);
void setMode(mode_t);
void adjustSettings(const Display::Field*, const Keypad::Button*);
void updateSettings(bool);
void readSettings();
void writeSettings();
void adjustSetpoint(const Display::Field*, Adjustment::Direction);
void adjustPid(const Display::Field*, Adjustment::Direction);
void adjustPwm(const Display::Field*, Adjustment::Direction);
void adjustAlarm(const Display::Field*, Adjustment::Direction);
void adjustSensor(const Display::Field*, Adjustment::Direction);
void adjustDisplay(const Display::Field*, Adjustment::Direction);
void switchUnits(temp_units_t);
void formatPidScreen();
data_t getTemperature(sensor_t);

#pragma region Settings Defaults 

const temp_units_t TemperatureUnits = DegreesCelsius;
const data_t TemperatureRangeLow = 0.0;
const data_t TemperatureRangeHigh = 100.0;
const sp_enable_t SetpointStatus = SetpointDisabled;
const data_t SetPointValue = TemperatureRangeLow;
const alarm_enable_t AlarmStatus = AlarmDisabled;
const alarm_compare_t AlarmCompare = AlarmCmpLess;
const data_t AlarmSetpoint = TemperatureRangeLow;
const Pwm::Range PwmRange = Pwm::Range();
const pid_t PidProportional = 1.0;
const pid_t PidIntegral = 0.0;
const pid_t PidDerivative = 0.0;
const pid_t PidGain = 1.0;
const milliseconds SensorPollingInterval = seconds(1);
const sensor_aref_t SensorReferenceSource = SensorArefInternal;

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
Adjustment adjustment(AdjustmentMultiplyMax);
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
Display::Field menu_pwm_field = { MenuPwmCol,MenuPwmRow,MenuPwmLab,MenuPwmFmt,MenuPwmVis,MenuPwmEdit };
Display::Field menu_alarm_field = { MenuAlarmCol,MenuAlarmRow,MenuAlarmLab,MenuAlarmFmt,MenuAlarmVis,MenuAlarmEdit };
Display::Field menu_sensor_field = { MenuSensorCol,MenuSensorRow,MenuSensorLab,MenuSensorFmt,MenuSensorVis,MenuSensorEdit };
Display::Field menu_display_field = { MenuDisplayCol,MenuDisplayRow,MenuDisplayLab,MenuDisplayFmt,MenuDisplayVis,MenuDisplayEdit };
Display::Screen menu_screen({ &menu_run_field,&menu_pid_field,&menu_pwm_field,&menu_alarm_field,&menu_sensor_field,
	&menu_display_field }, MenuScreenLab);

Display::Field pid_prop_field = { PidPropCol,PidPropRow,PidPropLab,PidPropFmt,PidPropVis,PidPropEdit };
Display::Field pid_integ_field = { PidIntegCol,PidIntegRow,PidIntegLab,PidIntegFmt,PidIntegVis,PidIntegEdit };
Display::Field pid_deriv_field = { PidDerivCol,PidDerivRow,PidDerivLab,PidDerivFmt,PidDerivVis,PidDerivEdit };
Display::Field pid_gain_field = { PidGainCol,PidGainRow,PidGainLab,PidGainFmt,PidGainVis,PidGainEdit };
Display::Screen pid_screen({ &pid_prop_field,&pid_integ_field,&pid_deriv_field,&pid_gain_field },
	PidScreenLab);

Display::Field pwm_duty_field = { PwmDutyCol,PwmDutyRow,PwmDutyLab,PwmDutyFmt,PwmDutyVis,PwmDutyEdit };
Display::Field pwm_low_field = { PwmLowCol,PwmLowRow,PwmLowLab,PwmLowFmt,PwmLowVis,PwmLowEdit };
Display::Field pwm_high_field = { PwmHighCol,PwmHighRow,PwmHighLab,PwmHighFmt,PwmHighVis,PwmHighEdit };
Display::Field pwm_bracket_field = { PwmBracketCol,PwmBracketRow,PwmBracketLab,PwmBracketFmt,PwmBracketVis,PwmBracketEdit };
Display::Screen pwm_screen({ &pwm_duty_field,&pwm_low_field,&pwm_high_field,&pwm_bracket_field }, PwmScreenLab);

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

#pragma region Thermostat Objects

TemperatureSensor temp_sensor(SensorInput, sensorCallback);
InputFilter temp_filter;
Pid pid;
Pwm pwm(PwmOutput);
EEStream eeprom;
Settings settings, settings_copy;
data_t Tsense = data_t();
uint8_t dc_pct = 0;
mode_t op_mode = mode_t::Init;

#pragma endregion
#pragma region Task Scheduling Objects

ClockCommand keypad_clock(&keypad);
ClockCommand display_clock(&display);
ClockCommand sensor_clock(&temp_sensor);
ClockCommand adjustment_clock(&adjustment);
ScheduledTask keypad_task(KeypadPollingInterval, &keypad_clock, task_state_t::Active);
ScheduledTask display_task(DisplayRefreshInterval, &display_clock, task_state_t::Active);
ScheduledTask sensor_task(SensorPollingInterval, &sensor_clock, task_state_t::Active);
ScheduledTask adjustment_task(AdjustmentMultiplyInterval, &adjustment_clock, task_state_t::Idle);
Scheduler scheduler{ &keypad_task,&display_task,&sensor_task,&adjustment_task };

#pragma endregion

void setup() 
{
#if defined _LOG_VALUES
	Serial.begin(9600);
	Serial.println();
	Serial.println("MV\t\tCV\t\tDCin\t\tDCout");
#endif
	lcd.begin(Display::cols(), Display::rows());
	lcd.clear();
	lcd.print("Initializing");
	alarmAttach(AlarmOutput);
	alarmSilence(AlarmOutput, true);
    setDefaults();
	readSettings();
	initSensor();
	setMode(mode_t::Run);
	scheduler.start();
}

void loop() 
{
	scheduler.tick();
}

void setDefaults()
{
	settings.temperatureLow() = TemperatureRangeLow;
    settings.temperatureHigh() = TemperatureRangeHigh;
    settings.temperatureUnits() = TemperatureUnits;
    settings.setpointEnable() = SetpointStatus;
    settings.setpointValue() = SetPointValue;
    settings.alarmCompare() = AlarmCompare;
    settings.alarmEnable() = AlarmStatus;
    settings.alarmSetpoint() = AlarmSetpoint;
    settings.pwmRange() = PwmRange;
    settings.pidProportional() = PidProportional;
    settings.pidIntegral() = PidIntegral;
    settings.pidDerivative() = PidDerivative;
    settings.pidGain() = PidGain;
    settings.sensorAref() = SensorReferenceSource;
    settings.sensorPollIntvl() = SensorPollingInterval;
}

void keyPress(const Keypad::Button* button)
{
    switch (op_mode)
    {
    case mode_t::Setpoint:
    case mode_t::Pid:
    case mode_t::Pwm:
    case mode_t::Alarm:
    case mode_t::Sensor:
    case mode_t::Display:
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
    case mode_t::Run:
        if (!alarmSilence(AlarmOutput)) // Any key silences the alarm in RUN mode.
            alarmSilence(AlarmOutput, true);
        else if (isNavButton(button))   // Setpoint & alarm settings adjustable in RUN mode.
        {
            setMode(mode_t::Setpoint);
            run_screen.active_field(button == &right_button ? &spval_field : &alrmqen_field);
            display.update();
        }
        break;
    case mode_t::Menu:
        if (isSelectButton(button))
            menuSelect(display.screen()->active_field());
        else if (isNavButton(button))
            scrollField(button);
        break;
    case mode_t::Pid:
    case mode_t::Pwm:
    case mode_t::Setpoint:
    case mode_t::Alarm:
    case mode_t::Sensor:
    case mode_t::Display:
        if (isSelectButton(button))     // Short press/release cancels edits & returns to RUN mode.
            setMode(mode_t::Run);
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
        case mode_t::Run:
            setMode(mode_t::Menu);  // Opens MENU screen.
            break;
        case mode_t::Pid:
        case mode_t::Pwm:
        case mode_t::Setpoint:
        case mode_t::Alarm:
        case mode_t::Sensor:
        case mode_t::Display:
            updateSettings(true);        // Saves edits.
            writeSettings();
        case mode_t::Menu:
            setMode(mode_t::Run);   // Returns to RUN mode.
            break;
        default:
            break;
        }
    }
    else if (isAdjustButton(button) && op_mode != mode_t::Run)
        keyRepeat(true);
}

void keyRepeat(bool enabled)
{
    // Disable button repeat and reset adjustment factor.
    keypad.repeat(enabled);
    adjustment.reset();
    adjustment_task.state(enabled ? task_state_t::Active : task_state_t::Idle);
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
    case mode_t::Setpoint:
        display.refresh(clamp(Tsense, settings.temperatureLow(), settings.temperatureHigh()), 
            DegreeSymbol, settings.temperatureUnits().display_value(), settings_copy.setpointValue(), 
            settings_copy.setpointEnable().display_value(), settings_copy.alarmEnable().display_value());
        break;
    case mode_t::Run:
        display.refresh(clamp(Tsense, settings.temperatureLow(), settings.temperatureHigh()), 
            DegreeSymbol, settings.temperatureUnits().display_value(), settings.setpointValue(), 
            settings.setpointEnable().display_value(), settings.alarmEnable().display_value());
        break;
    case mode_t::Menu:
        display.refresh(MenuItemRun, MenuItemPid, MenuItemPwm, MenuItemAlarm, MenuItemSensor, MenuItemDisplay);
        break;
    case mode_t::Pid:
        display.refresh(settings_copy.pidProportional(), settings_copy.pidIntegral(),
            settings_copy.pidDerivative(), settings_copy.pidGain());
        break;
    case mode_t::Pwm:
        display.refresh(dc_pct, settings_copy.pwmRange().low(), settings_copy.pwmRange().high(), ']');
        display.update(); // Always update the pwm output duty cycle.
        break;
    case mode_t::Alarm:
        display.refresh(settings_copy.alarmEnable().display_value(), settings_copy.alarmSetpoint(),
            settings_copy.alarmCompare().display_value());
        break;
    case mode_t::Sensor:
        display.refresh(settings_copy.sensorAref().display_value(), settings_copy.sensorPollIntvl());
        break;
    case mode_t::Display:
        display.refresh(settings_copy.temperatureLow(), settings_copy.temperatureHigh(),
            settings_copy.temperatureUnits().display_value());
        break;
    default:
        break;
    }
}

void sensorCallback()
{
    // Save the sensed temperature, check if it triggers an alarm and output the process control signal.
    checkAlarm((Tsense = getTemperature(temp_filter.out(temp_sensor.value()))));
    pid_t measured_value = clamp(Tsense, settings.temperatureLow(), settings.temperatureHigh());
    pid_t control_value = clamp(pid.loop(measured_value), 0.0f, settings.temperatureHigh() - settings.temperatureLow());
    pid_t output_value = norm(control_value, 0.0f, settings.temperatureHigh() - settings.temperatureLow(), 
        duty_cycle_t::min, duty_cycle_t::max);
    pwm.dutyCycle(output_value);
    pid_t dc_value = pwm.dutyCycle();
    dc_pct = (uint8_t)(dc_value * 100); // Round to [0,100].
#if defined _LOG_VALUES
    Serial.print(measured_value, 5); Serial.print(",\t");
    Serial.print(control_value, 5); Serial.print(",\t");
    Serial.print(output_value, 5); Serial.print(",\t");
    Serial.println(dc_value, 5);
#endif

}

void initSensor()
{
    delay(SensorInitDelay.count());     // Wait a sec for sensor/ADC to stabilize.
    temp_filter.seed(temp_sensor());    // Seed the input filter with the current sensor reading.
    Tsense = getTemperature(temp_filter.out(temp_sensor.value())); // Save the current temperature.
    pid.start(steady_clock::now());     // Start the pid controller at the current time.
}

void checkAlarm(data_t value)
{
    static bool alarm_active = false;
    bool alarm_now = settings.alarmCompare().value()(value, settings.alarmSetpoint());
    // Alarm only sounds if it crosses the setpoint, stays silent when manually silenced.
    if (settings.alarmEnable().value() && !alarm_active && alarm_now)
        alarmSilence(AlarmOutput, false);
    alarm_active = alarm_now && settings.alarmEnable().value();
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
        setMode(mode_t::Run);
    else if (field == &menu_pid_field)
        setMode(mode_t::Pid);
    else if (field == &menu_pwm_field)
        setMode(mode_t::Pwm);
    else if (field == &menu_alarm_field)
        setMode(mode_t::Alarm);
    else if (field == &menu_sensor_field)
        setMode(mode_t::Sensor);
    else if (field == &menu_display_field)
        setMode(mode_t::Display);
}

void setMode(mode_t mode)
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
        case mode_t::Run:
            display.screen(&run_screen);
            display_cursor = Display::Cursor::None;
            display_mode = Display::Mode::Normal;
            break;
        case mode_t::Setpoint:
            break;
        case mode_t::Pid:
            // This is redundant, but fits the settings copy/update scheme.
            settings.pidProportional() = pid.proportional();
            settings.pidIntegral() = pid.integral();
            settings.pidDerivative() = pid.derivative();
            settings.pidGain() = pid.gain();
            formatPidScreen();
            display.screen(&pid_screen);
            break;
        case mode_t::Pwm:
            // This is redundant, but fits the settings copy/update scheme.
            settings.pwmRange() = pwm.range();
            display.screen(&pwm_screen);
            pwm_screen.active_field(&pwm_low_field);
            break;
        case mode_t::Menu:
            display.screen(&menu_screen);
            break;
        case mode_t::Alarm:
            display.screen(&alarm_screen);
            break;
        case mode_t::Sensor:
            display.screen(&sensor_screen);
            break;
        case mode_t::Display:
            display.screen(&display_screen);
            break;
        default:
            break;
        };
        if (display_mode == Display::Mode::Edit)
            settings.copy(settings_copy);
        display.cursor(display_cursor);
        display.mode(display_mode);
        display.clear();
        op_mode = mode;
    }
}
void updateSettings(bool copy)
{
    if(copy)
        settings.update(settings_copy);
	sensor_task.interval(settings.sensorPollIntvl());
	sensorSetAref(settings.sensorAref().value());
	pid.set_point(settings.setpointValue());
	pid.proportional(settings.pidProportional());
	pid.integral(settings.pidIntegral());
	pid.derivative(settings.pidDerivative());
	pid.gain(settings.pidGain());
	pid.set_point(settings.setpointValue());
	pwm.range(settings.pwmRange());
	pwm.enabled(settings.setpointEnable().value());
	Tsense = getTemperature(temp_filter.out(temp_sensor.value()));
}

void writeSettings()
{
	eeprom.reset();
	eeprom << EepromID; // Save the eeprom id so we know settings exist and are valid.
	settings.serialize(eeprom);
}

void readSettings()
{
	long eeprom_id;

	eeprom.reset();
	eeprom >> eeprom_id;
	if (eeprom_id == EepromID)
		settings.deserialize(eeprom);
	else 
		writeSettings();
	updateSettings(false);
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
    case mode_t::Setpoint:
        adjustSetpoint(field, dir);
        break;
    case mode_t::Pid:
        adjustPid(field, dir);
        break;
    case mode_t::Pwm:
        adjustPwm(field, dir);
        break;
    case mode_t::Alarm:
        adjustAlarm(field, dir);
        break;
    case mode_t::Sensor:
        adjustSensor(field, dir);
        break;
    case mode_t::Display:
        adjustDisplay(field, dir);
        break;
    default:
        break;
    }
    display.update(); // Automatic display updates suspended in edit modes.
}

void adjustSetpoint(const Display::Field* field, Adjustment::Direction dir)
{
    // Adjusts set point and alarm from RUN screen.
    if (field == &spval_field)
    {
        auto inc = adjustment.value(DecimalAdjustmentFactor, dir);

        settings_copy.setpointValue() = wrap<data_t, data_t>(
            settings_copy.setpointValue(), inc, settings.temperatureLow(), settings.temperatureHigh());
    }
    else if (field == &spen_field)
        settings_copy.setpointEnable() = (settings_copy.setpointEnable().value())
        ? SetpointDisabled
        : SetpointEnabled;
    if (field == &alrmqen_field)
        settings_copy.alarmEnable() = (settings_copy.alarmEnable().value())
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
    data_t& coeff = field == &pid_prop_field
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
    coeff = wrap<data_t, data_t>(
        const_cast<const data_t&>(coeff), inc, PidCoeffMin, PidCoeffMax);
    // Format field according to the adjusted value.
    const_cast<Display::Field*>(field)->fmt_ = coeff < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
}

void adjustPwm(const Display::Field* field, Adjustment::Direction dir)
{
    pwm_t& value = field == &pwm_low_field
        ? settings_copy.pwmRange().low() 
        : settings_copy.pwmRange().high();
    auto inc = adjustment.value(PwmAdjustmentFactor, dir);

    value = wrap<pwm_t, pwm_t>(
        const_cast<const pwm_t&>(value), inc, duty_cycle_t::min, duty_cycle_t::max);
}

void adjustAlarm(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &alarm_enbl_field)
        settings_copy.alarmEnable() = (settings_copy.alarmEnable().value())
        ? AlarmDisabled
        : AlarmEnabled;
    else if (field == &alarm_cmp_field)
        settings_copy.alarmCompare() = settings_copy.alarmCompare().display_value() == LessSymbol
        ? AlarmCmpGreater
        : AlarmCmpLess;
    else if (field == &alarm_set_field)
    {
        auto inc = adjustment.value(DecimalAdjustmentFactor, dir);

        settings_copy.alarmSetpoint() = wrap<data_t, data_t>(
            settings_copy.alarmSetpoint(), inc, settings.temperatureLow(), settings.temperatureHigh());
    }
}

void adjustSensor(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &sensor_poll_field)
    {
        auto inc = adjustment.value(SensorAdjustmentFactor, dir);

        settings_copy.sensorPollIntvl() = milliseconds(
            wrap<sensor_t, std::make_signed<sensor_t>::type>(
                settings_copy.sensorPollIntvl().count(), inc, SensorPollingMin.count(), SensorPollingMax.count()));
    }
    else if (field == &sensor_aref_field)
        settings_copy.sensorAref() = settings_copy.sensorAref().value() == ArefSource::External
        ? SensorArefInternal
        : SensorArefExternal;
}

void adjustDisplay(const Display::Field* field, Adjustment::Direction dir)
{
    if (field == &display_unit_field)
    {
        auto to_units = (settings_copy.temperatureUnits() == DegreesCelsius)
            ? DegreesFahrenheit
            : (settings_copy.temperatureUnits() == DegreesFahrenheit)
            ? DegreesKelvin
            : DegreesCelsius;

        switchUnits(to_units);
    }
    else
    {
        auto inc = adjustment.value(DecimalAdjustmentFactor, dir);
        data_t& value = field == &display_low_field
            ? settings_copy.temperatureLow()
            : settings_copy.temperatureHigh();

        value = wrap<data_t, data_t>(
            const_cast<const data_t&>(value), inc, TemperatureMin, TemperatureMax);
    }
}

void switchUnits(temp_units_t to_units)
{
    typename temp_units_t::value_type cv = nullptr;

    if (settings_copy.temperatureUnits() == DegreesFahrenheit)
        cv = units::convert<units::fahrenheit, units::kelvin>;
    else if (settings_copy.temperatureUnits() == DegreesCelsius)
        cv = units::convert<units::celsius, units::kelvin>;
    if (cv)
    {
        // Convert to Kelvin, ...
        settings_copy.temperatureLow() = cv(settings_copy.temperatureLow());
        settings_copy.temperatureHigh() = cv(settings_copy.temperatureHigh());
        settings_copy.alarmSetpoint() = cv(settings_copy.alarmSetpoint());
        settings_copy.setpointValue() = cv(settings_copy.setpointValue());
    }
    // ... then to_units.
    settings_copy.temperatureUnits() = to_units;
    settings_copy.temperatureLow() = settings_copy.temperatureUnits().value()(settings_copy.temperatureLow());
    settings_copy.temperatureHigh() = settings_copy.temperatureUnits().value()(settings_copy.temperatureHigh());
    settings_copy.alarmSetpoint() = settings_copy.temperatureUnits().value()(settings_copy.alarmSetpoint());
    settings_copy.setpointValue() = settings_copy.temperatureUnits().value()(settings_copy.setpointValue());
}

void formatPidScreen()
{
    // Format all coeff fields when first switching to pid screen.
    pid_prop_field.fmt_ = pid.proportional() < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
    pid_integ_field.fmt_ = pid.integral() < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
    pid_deriv_field.fmt_ = pid.derivative() < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
    pid_gain_field.fmt_ = pid.gain() < PidCoeffThreshold ? PidDecimalFormat : PidUnitFormat;
}

data_t getTemperature(sensor_t value)
{
	data_t Tk = tsense(value, SensorOutMax, R, Vss, Vbe, Ka, Kb, Kc);

	return settings.temperatureUnits().value()(Tk); // Convert Kelvin to current temperature units.
}