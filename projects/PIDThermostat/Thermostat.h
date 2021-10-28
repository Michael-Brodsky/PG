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

// Enumerates valid sensor analog reference sources.
enum class ArefSource
{
	Internal = 0,	// Aref source is internal.
	External		// Aref source is external.
};

// Type that aggregates serializable settings and performs copy/update and 
// serialize/deserialize functions.
template<class DisplayType, class ADCType, class Duration>
class SettingsType : public iserializable 
{
public:
	using display_type = DisplayType;
	using adc_type = ADCType;
	using duration_type = Duration;
	using bool_type = bool;
	using symbol_type = char;
	using string_type = const char*;

	using unit_convert_func = typename callback<display_type, void, display_type>::type; // Unit conversion function signature.
	using alarm_cmp_func = typename callback<bool_type, void, display_type, display_type>::type; // Alarm compare function signature.
	using enable_type = std::pair<bool_type, symbol_type>; // Pairs a boolean value with a display character.
	using unit_type = std::pair<unit_convert_func, symbol_type>; // Pairs a conversion function with a display character.
	using alarm_compare_type = std::pair<alarm_cmp_func, symbol_type>; // Pairs a compare function with a display character.
	using sensor_aref_type = std::pair<ArefSource, string_type>; // Pairs an ArefSource with a display string.

public:	/* Ctors */
	SettingsType() = default;
	SettingsType(display_type temp_low, display_type temp_high, unit_type temp_units, 
		display_type pid_p, display_type pid_i, display_type pid_d, display_type pid_a,
		enable_type sp_enabled, display_type sp_value, 
		enable_type al_enabled, alarm_compare_type al_cmp, display_type al_setpoint, 
		sensor_aref_type sn_aref, duration_type sn_tpoll) :
		temp_low_(temp_low), temp_high_(temp_high), temp_units_(temp_units), 
		pid_p_(pid_p), pid_i_(pid_i), pid_d_(pid_d), pid_a_(pid_a), 
		sp_enabled_(sp_enabled), sp_value_(sp_value), 
		al_enabled_(al_enabled), al_cmp_(al_cmp), al_setpoint_(al_setpoint), 
		sn_aref_(sn_aref), sn_tpoll_(sn_tpoll)
	{}

public: /* Setters and getters. */
	unit_type& unitType() { return temp_units_; }
	const unit_type& unitType() const { return temp_units_; }
	unit_convert_func& unitConvert() { return temp_units_.first; }
	const unit_convert_func& unitConvert() const { return temp_units_.first; }
	symbol_type& unitSymbol() { return temp_units_.second; }
	const symbol_type& unitSymbol() const { return temp_units_.second; }
	display_type& tempLow() { return temp_low_; }
	const display_type& tempLow() const { return temp_low_; }
	display_type& tempHigh() { return temp_high_; }
	const display_type& tempHigh() const { return temp_high_; }
	display_type& pidProportional() { return pid_p_; }
	const display_type& pidProportional() const { return pid_p_; }
	display_type& pidIntegral() { return pid_i_; }
	const display_type& pidIntegral() const { return pid_i_; }
	display_type& pidDerivative() { return pid_d_; }
	const display_type& pidDerivative() const { return pid_d_; }
	display_type& pidGain() { return pid_a_; }
	const display_type& pidGain() const { return pid_a_; }
	enable_type& setpointEnType() { return sp_enabled_; }
	const enable_type& setpointEnType() const { return sp_enabled_; }
	bool_type& setpointEnabled() { return sp_enabled_.first; }
	const bool_type& setpointEnabled() const { return sp_enabled_.first; }
	symbol_type& setpointSymbol() { return sp_enabled_.second; }
	const symbol_type& setpointSymbol() const { return sp_enabled_.second; }
	display_type& setpointValue() { return sp_value_; }
	const display_type& setpointValue() const { return sp_value_; }
	enable_type& alarmEnType() { return al_enabled_; }
	const enable_type& alarmEnType() const { return al_enabled_; }
	bool_type& alarmEnabled() { return al_enabled_.first; }
	const bool_type& alarmEnabled() const { return al_enabled_.first; }
	symbol_type& alarmEnableSymbol() { return al_enabled_.second; }
	const symbol_type& alarmEnableSymbol() const { return al_enabled_.second; }
	alarm_compare_type& alarmCmpType() { return al_cmp_; }
	const alarm_compare_type& alarmCmpType() const { return al_cmp_; }
	alarm_cmp_func& alarmCompare() { return al_cmp_.first; }
	const alarm_cmp_func& alarmCompare() const { return al_cmp_.first; }
	symbol_type& alarmCompareSymbol() { return al_cmp_.second; }
	const symbol_type& alarmCompareSymbol() const { return al_cmp_.second; }
	display_type& alarmSetpoint() { return al_setpoint_; }
	const display_type& alarmSetpoint() const { return al_setpoint_; }
	sensor_aref_type& sensorArefType() { return sn_aref_; }
	const sensor_aref_type& sensorArefType() const { return sn_aref_; }
	ArefSource& sensorArefSource() { return sn_aref_.first; }
	const ArefSource& sensorArefSource() const { return sn_aref_.first; }
	string_type& sensorArefString() { return sn_aref_.second; }
	const string_type& sensorArefString() const { return sn_aref_.second; }
	duration_type& sensorPollIntvl() { return sn_tpoll_; }
	const duration_type& sensorPollIntvl() const { return sn_tpoll_; }

	// Creates a copy of the current settings for editing.
	void copy(const SettingsType& original, SettingsType& copy)
	{
		copy = original;
	}

	// Updates the current settings from a copy.
	void update(const SettingsType& copy, SettingsType& original)
	{
		original = copy;
		if (original.alarmSetpoint() < original.tempLow())
			original.alarmSetpoint() = original.tempLow();
		else if (original.alarmSetpoint() > original.tempHigh())
			original.alarmSetpoint() = original.tempHigh();
		if (original.setpointValue() < original.tempLow())
			original.setpointValue() = original.tempLow();
		else if (original.setpointValue() > original.tempHigh())
			original.setpointValue() = original.tempHigh();
	}

	// Writes settings to EEPROM.
	void serialize(EEStream& e) const override 
	{  
		e << tempLow(); e << tempHigh(), e << unitSymbol();
		e << pidProportional(); e << pidIntegral(); e << pidDerivative(); e << pidGain();
		e << setpointSymbol(); e << setpointValue();
		e << alarmEnableSymbol(); e << alarmCompareSymbol(); e << alarmSetpoint();
		e << sensorArefSource(); e << sensorPollIntvl().count();
	}

	// Reads settings from EEPROM.
	void deserialize(EEStream& e) override 
	{  
		e >> tempLow(); e >> tempHigh(), e >> unitSymbol();
		e >> pidProportional(); e >> pidIntegral(); e >> pidDerivative(); e >> pidGain();
		e >> setpointSymbol(); e >> setpointValue();
		e >> alarmEnableSymbol(); e >> alarmCompareSymbol(); e >> alarmSetpoint();
		e >> sensorArefSource();
		typename duration_type::rep r; e >> r;	sensorPollIntvl() = duration_type(r);
	}

private:	/* Editable program settings. */
	display_type			temp_low_;		// Temperature display range low.
	display_type			temp_high_;		// Temperature display range high,
	unit_type				temp_units_;	// Temperature units convert func/display symbol pair.
	display_type			pid_p_;			// PID proportional coeff.
	display_type			pid_i_;			// PID integral coeff.
	display_type			pid_d_;			// PID derivative coeff.
	display_type			pid_a_;			// PID gain coeff.
	enable_type				sp_enabled_;	// Set point enabled value/display symbol pair.
	display_type			sp_value_;		// Set point temperature.
	enable_type				al_enabled_;	// Alarm enabled value/display symbol pair.
	alarm_compare_type		al_cmp_;		// Alarm cmp function/display symbol pair.
	display_type			al_setpoint_;	// Alarm temperature set point.
	sensor_aref_type		sn_aref_;		// Sensor Aref source/display string pair.
	duration_type			sn_tpoll_;		// Sensor polling interval.
};

/* Arduino API interface functions. */

void alarmAttach(pin_t pin) { pinMode(pin, OUTPUT); }
void alarmSilence(pin_t pin, bool value) { digitalWrite(pin, !value); }
bool alarmSilence(pin_t pin) { return !digitalRead(pin); }
void sensorSetAref(const ArefSource src) { analogReference(src == ArefSource::Internal ? DEFAULT : EXTERNAL); }

#endif // !defined __PG_THERMOSTAT_H
