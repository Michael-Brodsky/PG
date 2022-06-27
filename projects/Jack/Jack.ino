/*
 *	This program demonstrates use of the pg::Jack class as a remote control,
 *	data acquisition (DAQ) and IoT platform (see <Jack.h>). 
 *
 *	***************************************************************************
 *
 *	File: Jack.ino
 *	Date: June 17, 2022
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

/* Compilation conditionals used to exclude certain features. */

//#define __PG_FORMAT_EEPROM 1			// Define this to format EEPROM memory, undefine & reupload for normal operation.
#define __PG_RESERVE_SPI_PINS			// Define this to set SPI pin modes to Reserved, e.g when using shields.
//#define __PG_NO_ETHERNET_CONNECTION 1	// Define this to exclude Ethernet connectivity.
//#define __PG_NO_WIFI_CONNECTION 1		// Define this to exclude WiFi connectivity.
//#define __PG_NO_ETHERNET_DHCP 1		// Define this to exclude Ethernet DHCP. 
//#define __PG_NO_PROGRAM 1				// Define this to exclude remote programming capability.
//#define __PG_NO_CHECKSUM 1			// Define this to exclude message checksum validation.
//#define __PG_NO_USR_COMMANDS 1		// Define this to exclude client-defined commands.

#if (defined FLASHEND) 
# if ((FLASHEND) < 0x4000)				// ATmega168 boards not supported.
#  error device not supported	
# endif
# if ((FLASHEND) < 0x8000)				// Low-end boards, like Uno, don't have enough memory for everything.
#  define __PG_NO_ETHERNET_CONNECTION 1
#  define __PG_NO_PROGRAM 1
# endif
# if ((FLASHEND) < 0xC000)				// DHCP really needs 64K+ of program memory.
#  define __PG_NO_ETHERNET_DHCP 1
# endif
#endif
#include <pg.h>
#include <components/Jack.h>

using namespace pg;

// In addition to those defined by Jack, clients can define their 
// own remotely callable functions.
void doSomething();

// Jack devices can be booted into a known state by holding one of 
// the pins LOW on power-up or reset. This allows communication with 
// the device if a network connection becomes unusable or is unknown 
// to the user. 
constexpr pin_t PowerOnDefaultsPin = 2;

// Clients must define "command" objects to execute client-defined
// functions. Command types are templates that take three parameters: 
// the return type, object type (or void for free-standing functions) 
// and a parameter list of argument type(s) (or void for functions 
// taking no arguments). Commands constructors require the command 
// "key" (a unique string that identifies the command), an object  
// reference (ommitted for commands that execute free-standing functions) 
// and the function address. Whenever Jack receives a message over the  
// network, it executes the command object with a matching key.
Jack::UsrCommand<void, void, void> usr_cmd{ "usr",&doSomething };

// Clients must pass a list of any client-defined commands to Jack's 
// constructor, or use the default constructor if no client commands 
// are defined.
Jack jack({ &usr_cmd });

void setup()
{
	// Jack's initialize() method must be called prior to use. This 
	// configures the device and opens a network connection.
	jack.initialize(PowerOnDefaultsPin);
}

void loop()
{
	// Jack processes any pending tasks in its clock() method. Clients 
	// should call this method regularly and without delay to achieve 
	// best performance.
	jack.clock();
}

void doSomething()
{
	jack.connection()->send("I did it!");
}
