/*
 *	This files defines WiFi types and header sources for various board 
 *	architectures.
 *
 *	***************************************************************************
 *
 *	File: wifi.h
 *	Date: April 20, 2022
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

#if !defined __PG_WIFI_H
# define __PG_WIFI_H 20220420L

# include <SPI.h>
# if defined ARDUINO_ARCH_MEGAAVR	// megaavr uses WiFiNINA:
#  include <WiFiNINA.h>
#  include <WiFiUdp.h>
using ssid_t = const char*;			//	WiFiNINA ssid param is type const char*.
# else								//	everything else uses whatever these are, 
#  include <WiFi.h>					//	and they must be included in this order:
#  include <WiFiClient.h>
#  include <WiFiServer.h>
#  include <WiFiUdp.h>
using ssid_t = char*;				//	everything else ssid param is type char*.
# endif // defined ARDUINO_ARCH_MEGAAVR

#endif // !defined __PG_WIFI_H
