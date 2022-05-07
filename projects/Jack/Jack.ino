/*
 *	This program demonstrates use of the pg::Jack class to turn an Arduino 
 *	into a remote control and data acquisition (DAQ) platform. Remote hosts 
 *	can establish a network connection and send commands to control the 
 *	board's pins, read and write values to/from the pins and time/count 
 *	pin state changes (see <Jack.h>).
 *
 *	This main program file defines functions that handle network connections 
 *	and the eeprom memory; device control and data acquisition functions are 
 *	handled by Jack. This file also defines a function that allows users to 
 *	force the device to use the default connection at power-up. It checks a 
 *	user-defined digital input pin and, if the pin is in the LOW state, opens 
 *	the default connection regardless (see <config.h>).
 * 
 *	The eeprom memory needs to be intitialized the first time the program is 
 *	run, otherwise the program may crash, reboot the device or exhibit  
 *	erratic behavior. This is done by uncommenting the line  
 *	`invalidateEeprom()' in the setup() function, and compiling and running 
 *	the program. Once intitialized, the program should be recompiled and 
 *	uploaded with the `invalidateEeprom()' line commented out. If not, 
 *	the eeprom memory will be overwritten each time the device is reset or 
 *	power-cycled and you will lose any saved information.
 *
 *	***************************************************************************
 *
 *	File: Jack.ino
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

#include <pg.h>
#include "config.h"	// Dependencies, typedefs and constants.

/*
 * Function Declarations
 */

void cmdConnection();
void cmdConnection(uint8_t, const char*, const char*, const char*);
void cmdLoadConfig();
void cmdStoreConfig();
bool eepromValid(EEStream&, const devid_type);
void closeConnection(Connection*&);
void intializeEeprom(EEStream&, devid_type, network_type, const char*, const Jack::Pins&, const Jack::Timers&);
void invalidateEeprom(EEStream&);
void loadConfig(EEStream&, Jack::Pins&, Jack::Timers&);
Connection* loadConnection(EEStream&, char*);
Connection* openConnection(network_type, const char*);
void setConnection(Connection*);
void storeConfig(EEStream&, const Jack::Pins&, const Jack::Timers&);
void storeConnection(EEStream&, network_type, const char*);
bool useDefaultConnection(pin_t);

/*
 * Application Objects
 */

RemoteCommand<uint8_t, const char*, const char*, const char*> _cmd_setconnection{ KeyConnection, &cmdConnection };
RemoteCommand<void> _cmd_getconnection{ KeyConnection, &cmdConnection };
RemoteCommand<void> _cmd_ldaconfig{ KeyLoadConfig, &cmdLoadConfig };
RemoteCommand<void> _cmd_stoconfig{ KeyStoreConfig, &cmdStoreConfig };

Connection* _connection = nullptr; 
EEStream _eeprom; 
Jack _jack(_connection, { &_cmd_setconnection,&_cmd_getconnection,&_cmd_ldaconfig,&_cmd_stoconfig }); 

void setup() 
{
	char params_buf[Connection::size()] = { '\0' };
	bool use_dflt = useDefaultConnection(DefaultConnectionPin);

	_eeprom << EEStream::update(); // Stay Home, Save Lives, Protect the EEPROM.
	//invalidateEeprom(_eeprom); // Uncomment this and upload to reinitialize eeprom. Comment out and upload for normal ops.
	if (use_dflt || !eepromValid(_eeprom, Jack::DeviceId))
	{
		_connection = openConnection(DefaultConnectionType, DefaultConnectionParams);
		if(!use_dflt)
			intializeEeprom(_eeprom, Jack::DeviceId, DefaultConnectionType, DefaultConnectionParams, 
				_jack.pins(), _jack.timers());
	}
	else
		_connection = loadConnection(_eeprom, params_buf);
	setConnection(_connection);
}

void loop() 
{
	_jack.clock();
}

void cmdConnection()
{
	char params[Connection::size()] = { '\0' };
	const char* fmt = nullptr;

	switch (_connection->type())
	{
	case network_type::Serial:
		fmt = FmtSerialConnection;
		break;
	case network_type::WiFi:
		fmt = FmtWifiConnection;
		break;
	case network_type::Ethernet:
		break;
	default: 
		break;
	}

	_jack.sendMessage(fmt, KeyConnection, _connection->type(), _connection->params(params));
}

void cmdConnection(uint8_t type, const char* arg0, const char* arg1, const char* arg2)
{
	network_type net_type = static_cast<network_type>(type);
	char params[Connection::size()] = { '\0' };
	const char* fmt = "%s,%s,%s";;
	Connection* connection = nullptr;

	(void)std::sprintf(params, fmt, arg0, arg1, arg2);
	digitalWrite(Jack::LedPinNumber, false);
	closeConnection(_connection);
	storeConnection(_eeprom, net_type, params);
	_connection = openConnection(net_type, params);
	setConnection(_connection);
}

void cmdLoadConfig()
{
	loadConfig(_eeprom, _jack.pins(), _jack.timers());
}

void cmdStoreConfig()
{
	storeConfig(_eeprom, _jack.pins(), _jack.timers());
}

void closeConnection(Connection*& connection)
{
	if (connection)
	{
		connection->close();
		delete connection;
		connection = nullptr;
	}
}

bool eepromValid(EEStream& eeprom, devid_type device_id)
{
	devid_type eeprom_id = 0;

	eeprom.reset();
	eeprom >> eeprom_id;

	return eeprom_id == device_id;
}

void intializeEeprom(EEStream& eeprom, devid_type eeprom_id, network_type type, const char* params,
	const Jack::Pins& pins, const Jack::Timers& timers)
{
	eeprom.reset();
	eeprom << eeprom_id;
	storeConfig(eeprom, pins, timers);
	storeConnection(eeprom, type, params);
}

void invalidateEeprom(EEStream& eeprom)
{
	eeprom << 42UL;
}

void loadConfig(EEStream& eeprom, Jack::Pins& pins, Jack::Timers& timers)
{
	eeprom.address() = ConfigurationEepromAddress;
	for (auto& pin : pins)
	{
		uint8_t tmp = 0;

		eeprom >> tmp;
		pin.mode_ = static_cast<gpio_mode>(tmp);
	}
	for (auto& timer : timers)
	{
		uint8_t tmp = 0;

		eeprom >> timer.pin_;
		eeprom >> tmp;
		timer.mode_ = static_cast<timer_mode>(tmp);
		eeprom >> tmp;
		timer.trigger_ = static_cast<PinStatus>(tmp);
	}
}

Connection* loadConnection(EEStream& eeprom, char* params)
{
	uint8_t tmp = 0;

	eeprom.address() = ConnectionEepromAddress;
	eeprom >> tmp;
	eeprom >> params;

	return openConnection(static_cast<network_type>(tmp), params);
}

Connection* openConnection(network_type type, const char* params)
{
	Connection* connection = nullptr;

	switch (type)
	{
	case network_type::Serial:
		connection = new pg::SerialConnection(Serial, params);
		break;
	case network_type::WiFi:
		connection = new pg::WiFiConnection(params);
		break;
	default:
		break;
	}

	return connection;
}

void setConnection(Connection* connection)
{
	_jack.connection(connection);
	digitalWrite(Jack::LedPinNumber, connection->open());
}

void storeConfig(EEStream& eeprom, const Jack::Pins& pins, const Jack::Timers& timers)
{
	eeprom.address() = ConfigurationEepromAddress;
	for (auto& pin : pins)
		eeprom << static_cast<uint8_t>(pin.mode_);
	for (auto& timer : timers)
	{
		eeprom << timer.pin_;
		eeprom << static_cast<uint8_t>(timer.mode_);
		eeprom << static_cast<uint8_t>(timer.trigger_);
	}
}

void storeConnection(EEStream& eeprom, network_type type, const char* params)
{
	eeprom.address() = ConnectionEepromAddress;
	eeprom << static_cast<uint8_t>(type);
	eeprom << params; 
}

bool useDefaultConnection(pin_t pin)
{
	gpio_mode pin_mode = _jack.pins()[pin].mode_;
	bool result = false;

	pinMode(pin, gpio_mode::Pullup);
	if (digitalRead(pin) == false)
		result = true;
	pinMode(pin, pin_mode);

	return result;
}