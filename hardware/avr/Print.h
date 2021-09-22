/*
 *	This file extends capabilities of the Arduino "Print" API.
 *
 *	***************************************************************************
 *
 *	File: Print.h
 *	Date: September 17, 2021
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
 *	***************************************************************************
 *
 *	Description:
 *
 *		This file extends the <Print.h> file bundled with the Arduino IDE 
 *      (avr-libc) by adding support for printing 64-bit integer types and 
 *      <long double> types. 
 *
 *	Notes:
 *
 *      This file adds the following functions and function overloads to the 
 *      `Print' class:
 * 
 *          size_t print64(unsigned long long, uint8_t);
 *          size_t print(long long, int = DEC);
 *          size_t print(unsigned long long, int = DEC);
 *          size_t print(long double, int = 2);
 *          size_t println(long long, int = DEC);
 *          size_t println(unsigned long long, int = DEC);
 *          size_t println(long double, int = 2);
 *          
 *  Credits:
 *
 *		This file is derived from <Print.h>, "Base class that provides print() 
 *      and println()", Copyright (C) 2008 David A. Mellis and modified 2015 
 *      by Chuck Todd.
 *
 *	**************************************************************************/

#ifndef Print_h
#define Print_h

#include <inttypes.h>
#include <stdio.h> // for size_t

#include "WString.h"
#include "Printable.h"

#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BIN
#endif
#define BIN 2

class Print
{
  private:
    int write_error;
    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(double, uint8_t);

    // Implements 64-bit integer printing, added 8/16/2021
    size_t print64(unsigned long long, uint8_t);

  protected:
    void setWriteError(int err = 1) { write_error = err; }
  public:
    Print() : write_error(0) {}
  
    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }
  
    virtual size_t write(uint8_t) = 0;
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *buffer, size_t size) {
      return write((const uint8_t *)buffer, size);
    }

    // default to zero, meaning "a single write may block"
    // should be overriden by subclasses with buffering
    virtual int availableForWrite() { return 0; }

    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    // Overload of print for <long long signed int> type. Added 08/16/2021.
    size_t print(long long, int = DEC);
    // Overload of print for <long long unsigned int> type. Added 08/16/2021.
    size_t print(unsigned long long, int = DEC);
    size_t print(double, int = 2);
    // Overload of print for <long double> type. Added 09/16/2021.
    size_t print(long double, int = 2);
    size_t print(const Printable&);

    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    // Overload of println for <long long signed int> type. Added 08/16/2021.
    size_t println(long long, int = DEC);
    // Overload of println for <long long unsigned int> type. Added 08/16/2021.
    size_t println(unsigned long long, int = DEC);
    size_t println(double, int = 2);
    // Overload of println for <long double> type. Added 09/16/2021.
    size_t println(long double, int = 2);
    size_t println(const Printable&);
    size_t println(void);

    virtual void flush() { /* Empty implementation for backward compatibility */ }
};

#endif
