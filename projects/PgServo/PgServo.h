/*
 *	This is the config file for the PgServo.ino application.
 *
 *	***************************************************************************
 *
 *	File: PgServo.h
 *	Date: March 26, 2022
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

#if !defined __PGSERVO_H
# define __PGSERVO_H 20220326

# include <pg.h>
# include <utilities/TaskScheduler.h>
# include <components/SweepServo.h>
# include <components/RemoteControl.h>

using namespace pg;
using namespace pg::servos;
using namespace pg::usart;

/* Type Aliases */

template<class... Ts>
using RemoteCommand = RemoteControl::Command<void, void, Ts...>;
using Scheduler = TaskScheduler<>;
using Task = Scheduler::Task;
// SweepServo<T> declares the servo object. The template parameter is the servo `type' 
// that defines a servo's performance parameters and limits, and is required. Known types 
// are defined in <lib/servos.h>. If your servo is not defined, you'll have to create 
// a definition for it.
using MyServo = SweepServo<hiwonder_ld20mg>; // The type of servo hardware being used.
using servo_type = MyServo::servo_type;
using angle_type = MyServo::angle_type;
using speed_type = MyServo::speed_type;
using baud_t = usart::baud_type;									
using frame_t = usart::frame_type;									

/* Application Constants */

const pin_t ServoControlPin = 3;							// Servo control signal pin.
const milliseconds ServoClockInterval = milliseconds(25);	// Servo clock interval
const milliseconds RCClockInterval = milliseconds(100);		// Remote control polling interval.
const unsigned long SerialBaudRate = 9600L;					// Default serial baud rate.
const char* RCKeySweep = "swp";								// Remote servo `sweep' command key.
const char* RCKeySpeed = "spd";								// Remote servo `speed' command key.
const char* RCKeyComms = "com";								// Remote comms command key.

#endif
