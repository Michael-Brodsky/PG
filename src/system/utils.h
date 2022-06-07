/*
 *	This file defines several convenience functions and function-like macros.
 *
 *	***************************************************************************
 *
 *	File: utils.h
 *	Date: December 18, 2021
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

#if !defined __PG_UTILS_H
# define __PG_UTILS_H 20210717L

# include <cassert>
# include <cstring>

# if !defined AssertMsg
#  define AssertMsg(x, msg) assert(((void) msg, (x))) // Prints `msg' on assertion failure of `x'.
# endif

# define NoTimer0Int TIMSK0 &= ~_BV(OCIE0A);					// Disable Timer0 cmp interrupt.
# define Timer0Int(val) OCR0A = (val); TIMSK0 |= _BV(OCIE0A);	// Sets Timer0 cmp interrupt to trigger on `val'.

namespace pg
{
	void sbegin(unsigned long baud, uint8_t frame = SERIAL_8N1) { Serial.begin(baud, frame); }

	void send() { Serial.end(); }

	void sflush() { Serial.flush(); }

	template<class T>
	unsigned sprint(const T& t) { return Serial.print(t); }

	template<class T>
	unsigned sprintln(const T& t) { return Serial.println(t); }

	unsigned sprintln() { return Serial.println(); }
} // namespace pg

# if defined ARDUINO_ARCH_MEGAAVR
void resetFunc()			// Reboots the Arduino device.
{
	CPU_CCP = 0xD8;
	WDT.CTRLA = 0x4;
	while (true);
}
# else
void(*resetFunc)(void) = 0;	// Reboots the Arduino device.
# endif

# if defined __arm__
// should use unistd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
# else  // __ARM__
extern char* __brkval;
# endif  // __arm__

// Returns the amount of free heap memory.
int freeMemory() 
{
	char top;
# if defined __arm__
	return &top - reinterpret_cast<char*>(sbrk(0));
# elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	return &top - __brkval;
# else  // __arm__
	return __brkval ? &top - __brkval : &top - __malloc_heap_start;
# endif  // __arm__
}

// `strcat()' for chars.
void charcat(char* buf, char c)
{
	char* ptr = buf + strlen(buf);

	*ptr = c;
	*(++ptr) = '\0';
}

#endif // !defined UTILS_H__
