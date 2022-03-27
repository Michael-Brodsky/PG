/*
 *	This application demonstrates how a servo motor can be controlled remotely 
 *	over a serial connection.
 *
 *	***************************************************************************
 *
 *	File: PgServo.ino
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
 *	**************************************************************************
 * 
 *	Description:
 * 
 *	The application remotely controls a servo motor attached one of the 
 *	board's pins using commands sent over a serial connection. Commands can be 
 *	sent using the Arduino IDE's Serial Monitor or any other application that 
 *	supports serial communications. The app requires the `Pg' libraries to be 
 *	installed on your computer (see https://github.com/Michael-Brodsky/pg).
 * 
 *	Commands are character strings using a key=value format:
 * 
 *		key[=param1,param2,...]      (params are comma separated numeric values.)
 * 
 *	Command keys:
 *
 *		swp: gets/sets the servo sweep angle, 
 *		spd: gets/sets the servo rotation speed, 
 *		com: gets the serial comms params.
 * 
 *	Examples:
 * 
 *		swp=90.0	(Rotates the servo to 90.0 degrees).
 *		swp			(Prints the servo's current rotation angle).
 *		spd=60,250	(Sets the rotation speed in degrees/milliseconds).
 *		spd=90.0	(Sets the speed in degrees using the current interval).
 *		com			(Prints the current communications parameters).
 * 
 *	Note that rotation angles can be either integer or decimal values. Servos 
 *	have limits and parameters must fall within those limits. A servo cannot 
 *	rotate at infinite speeds or beyond its min/max angles. Be sensible! 
 * 
 *	Defaults are defined in <PgServo.h> and may require modification based on 
 *	your implementation.
 * 
 *	**************************************************************************/

#include <pg.h>
#include "PgServo.h"

using namespace pg;
using namespace std::chrono;

/* Function Declarations */

void servoCallback(MyServo::State);		// Servo state change callback.
void servoSweep(angle_type);			// Sets the servo sweep angle.
void servoSweep();						// Gets the current servo sweep angle.
void servoSpeed(angle_type, time_t);	// Sets the servo speed in degrees/ms.
void servoSpeed(angle_type);			// Sets the servo speed in degrees (using current interval).
void servoSpeed();						// Gets the current servo speed in degrees/ms.
void commsStatus();						// Gets the current serial comms params.

/* Application Objects */

serial<0> serial0;	// Serial hardware to use, serial<0> refers to Serial, serial<1> = Serial1, etc.
RemoteCommand<angle_type> rc_set_sweep(RCKeySweep, &servoSweep);
RemoteCommand<angle_type, time_t> rc_set_speed2(RCKeySpeed, &servoSpeed);
RemoteCommand<angle_type> rc_set_speed(RCKeySpeed, &servoSpeed);
RemoteCommand<void> rc_get_sweep(RCKeySweep, &servoSweep);
RemoteCommand<void> rc_get_speed(RCKeySpeed, &servoSpeed);
RemoteCommand<void> rc_get_comms(RCKeyComms, &commsStatus);
RemoteControl rc({ &rc_set_speed2,&rc_set_sweep,&rc_set_speed,&rc_get_sweep,&rc_get_speed,&rc_get_comms }, serial0.hardware());
MyServo servo;	// See declaration in PgServo.h
ClockCommand clk_servo(&servo);
ClockCommand clk_rc(&rc);
Task tsk_servo(ServoClockInterval, &clk_servo, Task::State::Active);
Task tsk_rc(RCClockInterval, &clk_rc, Task::State::Active);
Scheduler scheduler({ &tsk_servo,&tsk_rc });

void setup() 
{
	serial0.begin(SerialBaudRate);
	Serial.println();
	servo.attach(ServoControlPin);
	servo.speed(speed_type(servo_type::LowRotationSpeed));
	servo.initialize(angle_type(servo_type::MinControlAngle));
	if (servo.attach() == ServoControlPin)
	{
		scheduler.start();
	}
	else
	{
		Serial.println("Error attaching servo.");
	}
}

void loop() 
{
	scheduler.tick();
}

void servoCallback(MyServo::State state)
{
	Serial.print("servo=");
	switch (state)
	{
	case MyServo::State::Active:
		Serial.println("actv");
		break;
	case MyServo::State::Idle:
		Serial.println("idle");
		Serial.print("sweep="); Serial.println(servo.sweep());
		break;
	default:
		break;
	}
}

void servoSweep(angle_type angle)
{
	Serial.print("servoSweep="); Serial.println(angle);
	servo.sweep(angle_type(angle));
}

void servoSpeed(angle_type degrees, time_t interval)
{
	Serial.print("servoSpeed="); Serial.println(degrees); Serial.print("/"); Serial.println(interval);
	servo.speed(speed_type(milliseconds(interval), angle_type(degrees)));
}

void servoSpeed(angle_type degrees)
{
	Serial.print("servoSpeed="); Serial.println(degrees);
	servo.speed(speed_type(milliseconds(250), angle_type(degrees)));
}

void servoSweep()
{
	Serial.print("sweep="); Serial.println(servo.sweep());
}

void servoSpeed()
{
	Serial.print("speed="); Serial.print(servo.speed().angle); Serial.print("/"); Serial.println(servo.speed().interval.count());
}

void commsStatus()
{
	static constexpr const char* FmtGetComms = "%s=%lu,%s";
	char buf[32];

	serial0.printFmt(buf, FmtGetComms, RCKeyComms, serial0.baud(),
		std::find(serial0.SupportedFrames.begin(), serial0.SupportedFrames.end(), serial0.frame())->string());
}
