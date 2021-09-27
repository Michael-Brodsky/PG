/*
 *	This file defines serializable communication protocols storage and 
 *	manipulation class.
 *
 *	***************************************************************************
 *
 *	File: SerialProtocols.h
 *	Date: August 10, 2021
 *	Version: 0.99
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
 *	**************************************************************************
 * 
 *	Description:
 * 
 *		The SerialProtocols class stores and manipulates serial communications 
 *		protocol settings: baud rate and protocol (8N1, 7E1, etc.), and stores 
 *		and retrives these to/from the onboard EEPROM using single lines of 
 *		code. This makes it useful for situations where users are able to edit 
 *		these settings, save them and have them restored after power-cycling. 
 *		It also has an "undo" feature that can restore the previous settings 
 *		after they've been changed.
 * 
 *		Supported baud rates and protocols are stored in static arrays (see 
 *		implementation below). The current values can be advanced, forward or 
 *		backward with the prev() and next() methods. The select() method 
 *		selects which parameter to advance (baud or protocol). The copy() 
 *		method makes a copy of the current settings, which can be restored 
 *		with the restore() method. The serialize() and deserialize() methods 
 *		write and read the current settings to/from the onboard EEPROM 
 *		respectively.
 * 
 *		In a typical application, the class methods can be triggered by 
 *		a keypad allowing users to modify the settings in real-time. Baud 
 *		rates are stored as their actual numeric values, protocols are stored 
 *		as key/value pairs with the first field being a human-readable string 
 *		and the second the corresponding Arduino constant #defined by the 
 *		Serial class.
 *
 *	**************************************************************************/

#if !defined __PG_SERIALPROTOCOLS_H
# define __PG_SERIALPROTOCOLS_H 20210810L

# include "array"		// STL container/iterator support.
# include "EEStream.h"	// EEPROM serialize/deserialize support.

namespace pg
{
	// Serializable communication protocols type.
	class SerialProtocols : public iserializable
	{
	public:
		using baud_type = unsigned long;	// Baud rate type.
		using protocol_type = char;			// Arduino #defined protocols type.
		using map_type = std_pair<const char*, const protocol_type>; // Type that maps Arduino protocol #defines to human-readable strings. 
		using baud_container_type = ArrayWrapper<const baud_type>;
		using protocol_container_type = ArrayWrapper<const map_type>;
		using baud_iterator_type = baud_container_type::const_iterator;
		using protocol_iterator_type = protocol_container_type::const_iterator;
		enum class Advance { Baud = 0, Protocol };	// Determines which value (baud or protocol) is advanced by prev() and next() methods.

		static const baud_type SupportedBaudRates[];
		static const map_type SupportedProtocols[];
		static const baud_type DefaultBaudRate = 9600;
		static const protocol_type DefaultProtocol = SERIAL_8N1;

	public:
		// Default constructor.
		SerialProtocols();

	public:
		// Sets the current protocol from a given value.
		void protocol(const protocol_type&);
		// Sets the current protocol from a given key-value pair.
		void protocol(const map_type&);
		// Returns the current protocol as a key-value pair.
		const map_type& protocol() const;
		// Sets the current baud rate.
		void baud(const baud_type&);
		// Returns the current baud rate.
		const baud_type& baud() const;
		// Selects the `Advance' mode.
		void select(Advance);
		// Advances to the next value (baud/protocol) in a list according to the `Advance' mode.
		void next();
		// Advances to the previous value (baud/protocol) in a list according to the `Advance' mode.
		void prev();
		// Writes the currently stored baud/protocol values to EEPROM.
		void serialize(EEPROMStream&) const override;
		// Reads baud/protocol values from the EEPROM and stores them.
		void deserialize(EEPROMStream&) override;
		// Makes a copy of the current baud/protocol values.
		void copy();
		// Restores the current baud/protocol values from a copy.
		void restore();

	private:
		// Finds a given baud rate in a collection.
		baud_iterator_type match(const baud_type baud) const;
		// Finds a given protocol in a collection.
		protocol_iterator_type match(const protocol_type protocol) const;

	private:
		baud_iterator_type baud_rate_;			// Stores the current baud rate.
		protocol_iterator_type protocol_;		// Stores the current protocol value.
		baud_iterator_type baud_rate_copy_;		// Stores a copy of the current baud rate.
		protocol_iterator_type protocol_copy_;	// Stores a copy of the current protocol value.
		Advance selection_;						// The current `Advance' mode.
	};

	// List of supported baud rates.
	const SerialProtocols::baud_type SerialProtocols::SupportedBaudRates[] =
	{
		300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
	};

	// Map of supported protocols, first field is a human-readable string, 
	// for display, second is the Arduino constant #defined in the Serial 
	// class. Note, Only the 8N1 and 7E1 are protocols are ever used in 
	// most implementations. Uncommenting the remaining protocols will 
	// alllow for more choices but may consume a significant amount of memory.

	// Map of supported protocols.
	const SerialProtocols::map_type SerialProtocols::SupportedProtocols[] =
	{
		//SerialProtocols::map_type("5N1", SERIAL_5N1),
		//SerialProtocols::map_type("6N1", SERIAL_6N1),
		//SerialProtocols::map_type("7N1", SERIAL_7N1),
		SerialProtocols::map_type("8N1", SERIAL_8N1),
		//SerialProtocols::map_type("5N2", SERIAL_5N2),
		//SerialProtocols::map_type("6N2", SERIAL_6N2),
		//SerialProtocols::map_type("7N2", SERIAL_7N2),
		//SerialProtocols::map_type("8N2", SERIAL_8N2),
		//SerialProtocols::map_type("5E1", SERIAL_5E1),
		//SerialProtocols::map_type("6E1", SERIAL_6E1),
		SerialProtocols::map_type("7E1", SERIAL_7E1),
		//SerialProtocols::map_type("8E1", SERIAL_8E1),
		//SerialProtocols::map_type("5E2", SERIAL_5E2),
		//SerialProtocols::map_type("6E2", SERIAL_6E2),
		//SerialProtocols::map_type("7E2", SERIAL_7E2),
		//SerialProtocols::map_type("8E2", SERIAL_8E2),
		//SerialProtocols::map_type("5O1", SERIAL_5O1),
		//SerialProtocols::map_type("6O1", SERIAL_6O1),
		//SerialProtocols::map_type("7O1", SERIAL_7O1),
		//SerialProtocols::map_type("8O1", SERIAL_8O1),
		//SerialProtocols::map_type("5O2", SERIAL_5O2),
		//SerialProtocols::map_type("6O2", SERIAL_6O2),
		//SerialProtocols::map_type("7O2", SERIAL_7O2),
		//SerialProtocols::map_type("8O2", SERIAL_8O2)
	};

	SerialProtocols::SerialProtocols() :
		baud_rate_(match(DefaultBaudRate)), baud_rate_copy_(baud_rate_),
		protocol_(match(DefaultProtocol)), protocol_copy_(protocol_),
		selection_(Advance::Baud)
	{

	}

	void SerialProtocols::protocol(const protocol_type& value)
	{
		protocol_iterator_type it = match(value);

		protocol_ = it != std::end(SupportedProtocols) ? it : match(DefaultProtocol);
	}

	void SerialProtocols::protocol(const SerialProtocols::map_type& key_value)
	{
		protocol(key_value.second);
	}

	const SerialProtocols::map_type& SerialProtocols::protocol() const
	{
		return *protocol_;
	}

	void SerialProtocols::baud(const baud_type& value)
	{
		baud_iterator_type it = match(value);

		baud_rate_ = it != std::end(SupportedBaudRates) ? it : match(DefaultBaudRate);
	}

	const SerialProtocols::baud_type& SerialProtocols::baud() const
	{
		return *baud_rate_;
	}

	void SerialProtocols::select(Advance selection)
	{
		selection_ = selection;
	}

	void SerialProtocols::next()
	{
		switch (selection_)
		{
		case SerialProtocols::Advance::Baud:
			if (++baud_rate_ == std::end(SupportedBaudRates))
				baud_rate_ = std::begin(SupportedBaudRates);
			break;
		case SerialProtocols::Advance::Protocol:
			if (++protocol_ == std::end(SupportedProtocols))
				protocol_ = std::begin(SupportedProtocols);
			break;
		default:
			break;
		}
	}

	void SerialProtocols::prev()
	{
		switch (selection_)
		{
		case SerialProtocols::Advance::Baud:
			if (baud_rate_ == std::begin(SupportedBaudRates))
				baud_rate_ = std::end(SupportedBaudRates);
			--baud_rate_;
			break;
		case SerialProtocols::Advance::Protocol:
			if (protocol_ == std::begin(SupportedProtocols))
				protocol_ = std::end(SupportedProtocols);
			--protocol_;
			break;
		default:
			break;
		}
	}

	void SerialProtocols::serialize(EEPROMStream& s) const
	{
		s << baud();
		s << protocol().second;
	}

	void SerialProtocols::deserialize(EEPROMStream& s)
	{
		baud_type b;
		protocol_type p;

		s >> b;
		s >> p;
		baud(b);
		protocol(p);
	}

	void SerialProtocols::copy()
	{
		baud_rate_copy_ = baud_rate_;
		protocol_copy_ = protocol_;
	}

	void SerialProtocols::restore()
	{
		baud_rate_ = baud_rate_copy_;
		protocol_ = protocol_copy_;
	}

	SerialProtocols::baud_iterator_type SerialProtocols::match(const baud_type baud) const
	{
		return std::find(std::begin(SupportedBaudRates), std::end(SupportedBaudRates), baud);
	}

	SerialProtocols::protocol_iterator_type SerialProtocols::match(const protocol_type protocol) const
	{
		for (auto& it : SupportedProtocols)
		{
			if (it.second == protocol)
				return &it;
		}

		return std::end(SupportedProtocols);
	}
}

#endif // !defined __PG_SERIALPROTOCOLS_H