/*
 *	This program demonstrates the use of the pg::Jack class to turn an Arduino 
 *	into a remote control and data acquisition (DAQ) platform. Remote hosts 
 *	can establish a network connection and send the commands to control the 
 *	board's pins, or read and write values to/from the pins.
 *
 *	***************************************************************************
 *
 *	File: Jack.ino
 *	Date: May 4, 2022
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
#include <components/Jack.h>

using pg::Connection;
using pg::EEStream;
using pg::Jack;
using pg::RemoteControl;
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

void cmdConnection();
void cmdConnection(uint8_t, const char*);
void cmdLoadConfig();
void cmdStoreConfig();
bool eepromValid(EEStream&, const devid_type);
void closeConnection(Connection*&);
void intializeEeprom(EEStream&, const devid_type, network_type, const char*, const Jack::Pins&, const Jack::Timers&);
void loadConfig(EEStream&, Jack::Pins&, Jack::Timers&);
Connection* loadConnection(EEStream&, char*);
Connection* openConnection(const network_type, const char*);
void storeConfig(EEStream&, const Jack::Pins&, const Jack::Timers&);
void storeConnection(EEStream&, network_type, const char*);

network_type DefaultConnectionType = network_type::Serial;
const char* DefaultConnectionParams = "9600,8N1";
const address_type ConfigurationEepromAddress = sizeof(devid_type);
const address_type ConnectionEepromAddress = 
	ConfigurationEepromAddress +
	(Jack::GpioCount * sizeof(decltype(Jack::GpioPin::mode_))) + 
	Jack::TimersCount * (sizeof(decltype(Jack::CounterTimer::pin_)) + 
		sizeof(decltype(Jack::CounterTimer::mode_)) + 
		sizeof(decltype(Jack::CounterTimer::trigger_)));

key_type KeyConnection = "net";	
key_type KeyLoadConfig = "lda";	
key_type KeyStoreConfig = "sto";

fmt_type FmtConnection = "%s=%u,%s"; 

RemoteCommand<void> _cmd_getconnection{ KeyConnection, &cmdConnection };
RemoteCommand<uint8_t, const char*> _cmd_setconnection{ KeyConnection, &cmdConnection };
RemoteCommand<void> _cmd_ldaconfig{ KeyLoadConfig, &cmdLoadConfig };
RemoteCommand<void> _cmd_stoconfig{ KeyStoreConfig, &cmdStoreConfig };

Connection* _connection = nullptr; 
EEStream _eeprom; 
Jack _jack(_connection, { &_cmd_setconnection,&_cmd_getconnection,&_cmd_ldaconfig,&_cmd_stoconfig }); 

void setup() 
{
	char params_buf[Connection::size()] = { '\0' };

	_eeprom << EEStream::update();
	//_eeprom << 42UL; // Uncomment this and upload to reinitialize eeprom. Then comment out and upload for normal ops.
	if (!eepromValid(_eeprom, Jack::DeviceId))
	{
		_connection = openConnection(DefaultConnectionType, DefaultConnectionParams);
		intializeEeprom(_eeprom, Jack::DeviceId, DefaultConnectionType, DefaultConnectionParams, 
			_jack.pins(), _jack.timers());
	}
	else
		_connection = loadConnection(_eeprom, params_buf);
	_jack.connection(_connection);
	digitalWrite(Jack::LedPinNumber, _connection->open());
}

void loop() 
{
	_jack.clock();
}

void cmdConnection()
{
	char params[Connection::size()] = { '\0' };

	_jack.sendMessage(FmtConnection, KeyConnection, _connection->type(), _connection->params(params));
}

void cmdConnection(uint8_t type, const char* params)
{
	closeConnection(_connection);
	_connection = openConnection(static_cast<network_type>(type), params);
	storeConnection(_eeprom, static_cast<network_type>(type), params);
	_jack.connection(_connection);
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

bool eepromValid(EEStream& eeprom, const devid_type device_id)
{
	devid_type eeprom_id = 0;

	eeprom.reset();
	eeprom >> eeprom_id;

	return eeprom_id == device_id;
}

void intializeEeprom(EEStream& eeprom, const devid_type eeprom_id, network_type type, const char* params,
	const Jack::Pins& pins, const Jack::Timers& timers)
{
	eeprom.reset();
	eeprom << eeprom_id;
	storeConfig(eeprom, pins, timers);
	storeConnection(eeprom, type, params);
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

Connection* openConnection(const network_type type, const char* params)
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
