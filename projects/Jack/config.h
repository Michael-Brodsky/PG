/*
 *	This file defines types and constants used by the Jack demo application 
 *	(Jack.ino).
 *
 *	***************************************************************************
 *
 *	File: config.h
 *	Date: May 6, 2022
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky
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

#if !defined __PG_JACK_CONFIG_H
# define __PG_JACK_CONFIG_H 20220506L

# include <components/Jack.h>

/*
 * Type aliases 
 */

using pg::Connection;
using pg::EEStream;
using pg::Jack;
using pg::RemoteControl;
using pg::pin_t;
using address_type = EEStream::address_type;
using devid_type = Jack::devid_type;
using fmt_type = Jack::fmt_type;
using gpio_mode = Jack::gpio_mode;
using gpio_type = Jack::gpio_type;
using key_type = Jack::key_type;
using network_type = Connection::Type;
template<class... Ts>
using RemoteCommand = typename RemoteControl::Command<void, void, Ts...>;
using size_type = Jack::size_type;
using timer_mode = Jack::timer_mode;

/*
 * Constants 
 */

network_type DefaultConnectionType = network_type::Serial;
const char* DefaultConnectionParams = "9600,8N1";
const pin_t DefaultConnectionPin = 2;

// Eeprom memory map:
//
// | Device_ID | Pins Config | Timers Config | Connection |
//
// Contents:
//
// Device_ID: application-specific magic number of type unsigned long, 
// Connection: type, params,
// Pins: mode, 
// Timers: pin,mode,trigger.

const address_type ConfigurationEepromAddress = sizeof(devid_type);
const address_type ConnectionEepromAddress =
ConfigurationEepromAddress +
(Jack::GpioCount * sizeof(decltype(Jack::GpioPin::mode_))) +
Jack::TimersCount * (sizeof(decltype(Jack::CounterTimer::pin_)) +
	sizeof(decltype(Jack::CounterTimer::mode_)) +
	sizeof(decltype(Jack::CounterTimer::trigger_)));

// Remote command strings:

key_type KeyConnection = "net";			// Get/set network connection.
key_type KeyLoadConfig = "lda";			// Load device config from eeprom.
key_type KeyStoreConfig = "sto";		// Store device config to eeprom.

fmt_type FmtSerialConnection = "%s=%u,%s";	// Serial connection reply format specifier.
fmt_type FmtWifiConnection = "%s=%u,%s";	// WiFi connection reply format specifier.

#endif // !defined __PG_JACK_CONFIG_H