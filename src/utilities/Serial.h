/*
 *	This file defines constants used to control serial port operations and
 *	types to easily manipulate them.
 *
 *	***************************************************************************
 *
 *	File: Serial.h
 *	Date: December 26, 2021
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
 *	***************************************************************************/

#if !defined __PG_SERIAL_H
# define __PG_SERIAL_H 20211218L

# include "cstring"				// C string cmp functions.
# include "array"				// Fixed-size array types.
# include "utilities/Values.h"	// DisplayValue type.

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	struct serial
	{
		using baud_type = unsigned long;
		using timeout_type = unsigned long;
		using protocol_value_type = char;
		using protocol_display_type = const char*;
		using protocol_type = DisplayValue<protocol_value_type, protocol_display_type>;
#  if defined __PG_SERIAL_PROTOCOLS_ALL
		static const std::size_t DfltProto = 3;
#  else 
		static const std::size_t DfltProto = 0;
#  endif
		static const std::ArrayWrapper<const protocol_type> supported_protocols;
		static const std::ArrayWrapper<const baud_type> supported_baud_rates;
		static const baud_type DefaultBaudRate() { return supported_baud_rates[5]; }
		static const protocol_type DefaultProtocol() { return supported_protocols[DfltProto]; }
		static const timeout_type DefaultTimeout() { return 500UL; }
		static const protocol_display_type find(const protocol_value_type& value)  
		{
			protocol_display_type display_value = nullptr;

			for (auto& i : supported_protocols)
			{
				if (value == i.value())
				{
					display_value = i.display_value();
					break;
				}
			}
			
			return display_value;
		}

		static const protocol_value_type find(const protocol_display_type& display_value)
		{
			protocol_value_type value = '\0';

			for (auto& i : supported_protocols)
			{
				if (!std::strncmp(display_value, i.display_value(), std::strlen(i.display_value())))
				{
					value = i.value();
					break;
				}
			}

			return value;
		}

	private:
		static const baud_type SupportedBaudRates[];
		static const protocol_type SupportedProtocols[];
	};

	const serial::baud_type serial::SupportedBaudRates[] =
	{
		300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200
	};

	const serial::protocol_type serial::SupportedProtocols[] =
	{
#  if defined __PG_SERIAL_PROTOCOLS_ALL
		serial::protocol_type(SERIAL_5N1, "5N1"),
		serial::protocol_type(SERIAL_6N1, "6N1"),
		serial::protocol_type(SERIAL_7N1, "7N1"),
#  endif
		serial::protocol_type(SERIAL_8N1, "8N1"),
#  if defined __PG_SERIAL_PROTOCOLS_ALL
		serial::protocol_type(SERIAL_5N2, "5N2"),
		serial::protocol_type(SERIAL_6N2, "6N2"),
		serial::protocol_type(SERIAL_7N2, "7N2"),
		serial::protocol_type(SERIAL_8N2, "8N2"),
		serial::protocol_type(SERIAL_5E1, "5E1"),
		serial::protocol_type(SERIAL_6E1, "6E1"),
#  endif
		serial::protocol_type(SERIAL_7E1, "7E1"), 
#  if defined __PG_SERIAL_PROTOCOLS_ALL
		serial::protocol_type(SERIAL_8E1, "8E1"),
		serial::protocol_type(SERIAL_5E2, "5E2"),
		serial::protocol_type(SERIAL_6E2, "6E2"),
		serial::protocol_type(SERIAL_7E2, "7E2"),
		serial::protocol_type(SERIAL_8E2, "8E2"),
		serial::protocol_type(SERIAL_5O1, "5O1"),
		serial::protocol_type(SERIAL_6O1, "6O1"),
		serial::protocol_type(SERIAL_7O1, "7O1"),
		serial::protocol_type(SERIAL_8O1, "8O1"),
		serial::protocol_type(SERIAL_5O2, "5O2"),
		serial::protocol_type(SERIAL_6O2, "6O2"),
		serial::protocol_type(SERIAL_7O2, "7O2"),
		serial::protocol_type(SERIAL_8O2, "8O2") 
#  endif
	};

	const std::ArrayWrapper<const serial::protocol_type> serial::supported_protocols(serial::SupportedProtocols);
	const std::ArrayWrapper<const serial::baud_type> serial::supported_baud_rates(serial::SupportedBaudRates);
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_SERIAL_H
