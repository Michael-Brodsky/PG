/*
 *  This file exposes the Arduino API.
 *
 *	***************************************************************************
 *
 *	File: library.h
 *	Date: March 10, 2021
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
 *  Description:
 *
 *	This file should be included in any program that makes calls to Arduino 
 *	api functions.
 * 
 *  This file may include the following header files:
 *
 *      Arduino.h or WProgram.h: Arduino API declarations, depending on the 
 *                               Arduino IDE implementation being used.
 * 
 *	**************************************************************************/

#if !defined __PG_API_H
# define __PG_API_H 20210310L

# if defined ARDUINO
#  if ARDUINO >= 100 
#   include <Arduino.h>
#  else
#   include <WProgram.h>
#  endif // ARDUINO >= 100 
# endif // defined ARDUINO

#endif // !defined __PG_API_H
