/*
 *	This files defines several types for managing connections to remote hosts 
 *	using a common interface.
 *
 *	***************************************************************************
 *
 *	File: Connection.h
 *	Date: April 1, 2022
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
 *		This file defines two connection types:
 *		
 *			SerialConnection: manages serial connections, 
 *			UdpConnection: manages UDP/IP connections on a WiFi network.
 * 
 *		Both of these types inherit from a common abstract class `Connection' 
 *		that declares a common interface.
 * 
 *	Usage:
 * 
 *		UdpConnection objects are default constructable, with optional 
 *		connection parameters (see open() method below). For example: 
 *
 *			UdpConnection udp;
 *			UdpConnection udp("MyNetworkSSID,MyPassword,192.168.128.1,5000");
 * 
 *		SerialConnection objects are constructed by passing one of the Pg 
 *		hardware_serial objects to the constructor and specifying the port 
 *		to use. The hardware_serial types are are wrappers for the Arduino's 
 *		HardwareSerial objects, Serial, Serial1, Serial2 or Serial3, but are 
 *		named serial<0>, serial<1>, serial<2> and serial<3> respectively. 
 *		They extend the capabilities of the Arduino HardwareSerial types but 
 *		otherwise behave identically (see <utilities/Serial.h>). For example: 
 * 
 *			SerialConnection ser(serial<0>);
 *			SerialConnection ser(serial<1>,"9600,8N1");
 * 
 *		Connections must be opened in the constructor or with the open() method.
 *		The method takes connection parameters in the form of a comma separated 
 *		string list ("param1,param2,...,paramN").
 * 
 *		SerialConnection.open(baud,frame[,timeout]); where:
 * 
 *			baud = any valid baud rate, 
 *			frame = any valid frame, 
 *			timeout = (optional) any valid timeout in milliseconds. 
 * 
 *		UdpConnection.open("ssid,pw,ipa,port"); where:
 * 
 *			ssid = the network ssid, 
 *			pw = the network password,
 *			ipa = the broadcast address, 
 *			port = the port to listen on.
 * 
 *		For example:
 * 
 *			ser.open("9600,8N1,1000");
 *			udp.open("MyNetworkSSID,MyPassword,192.168.128.1,5000");
 * 
 *		Messages are sent to and received from remote hosts as character 
 *		strings using the send() and receive() methods. The receive() method 
 *		returns a std::nullptr_t if no packets were received.
 * 
 *	Notes:
 * 
 *		UdpConnection only supports unicast send and receive, as it's intended 
 *		to communicate with a specific host at a specific ip on a specific 
 *		port.
 *
 *	**************************************************************************/

#if !defined __PG_CONNECTION_H
#define __PG_CONNECTION_H 20220401

#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <utilities/Serial.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

using namespace pg::usart;

namespace pg
{
	// Connection abstract interface class.
	struct Connection
	{
		static const char* const ParamArgsDelimiter;
		static const char* const IpAddressDelimiter;

		virtual ~Connection() = default;

		virtual void open(const char* params) = 0;
		virtual bool open() = 0;
		virtual void close() = 0;
		virtual std::size_t send(const char* message) = 0;
		virtual const char* receive() = 0;
	};

	const char* const Connection::ParamArgsDelimiter = ",";
	const char* const Connection::IpAddressDelimiter = ".";

	// Connects a remote host using a serial port (usart).
	class SerialConnection : public Connection
	{
	public:
		SerialConnection(hardware_serial&, const char* = nullptr);
		~SerialConnection() override;

	public:
		void open(const char* params) override;
		bool open() override;
		void close() override;
		std::size_t send(const char* message) override;
		const char* receive() override;
		hardware_serial& hardware();

	private:
		void parseParams(const char* params);

	private:
		hardware_serial&	hardware_;				// References the port hardware layer.
		baud_type			baud_;					// The current baud rate.
		frame_type			frame_;					// The current frame.
		timeout_type		timeout_;				// The current write timeout in milliseconds.
		char buf_[hardware_serial::RxBufferSize];	// Receive buffer. 
	};

	// Connects a remote host using UDP/IP (WiFi).
	class UdpConnection : public Connection
	{
	public: 
		static const std::size_t UdpMaxPacketSize = 255;
		static const uint32_t WifiWaitConnect = 2000;
		static const uint32_t WifiMaxWaitTime = 10000;

	public:
		UdpConnection(const char* params = nullptr);
		~UdpConnection() override;

	public:
		void open(const char* params) override;
		bool open() override;
		void close() override;
		std::size_t send(const char* message) override;
		const char* receive() override;
		WiFiClass& hardware();

	private:
		void parseParams(const char* params);

	private:
		char*			ssid_;		// Network ssid.
		const char*		pw_;		// Network passphrase.
		int				status_;	// Network connection status.
		WiFiUDP			udp_;		// Arduino UDP api.
		IPAddress		ipa_;		// The current broadcast address.
		unsigned int	port_;		// The current UDP port.
		char			buf_[UdpMaxPacketSize];	// Receive buffer.
	};


#pragma region SerialConnection

SerialConnection::SerialConnection(hardware_serial& hs, const char* params) : 
	hardware_(hs), baud_(hardware_serial::DefaultBaudRate), frame_(hardware_serial::DefaultFrame), 
	timeout_(hardware_serial::DefaultTimeout), buf_() 
{
	if (params)
		open(params);
}

SerialConnection::~SerialConnection()
{
	close();
}

void SerialConnection::open(const char* params)
{
#if !defined _DEBUG 
	parseParams(params);
	hardware_.begin(baud_, frame_);
	hardware_.setTimeout(timeout_);
#endif // !defined _DEBUG 
}

bool SerialConnection::open()
{
	return hardware_.isOpen();
}

void SerialConnection::close()
{
	hardware_.end();
}

std::size_t SerialConnection::send(const char* message)
{
	std::size_t n = 0;

	if(hardware_.availableForWrite())
		n = hardware_.println(message);

	return n;
}

const char* SerialConnection::receive()
{
	std::size_t eot = 0;

	if(hardware_.available())
		eot = hardware_.readBytesUntil('\n', buf_, sizeof(buf_));
	buf_[eot] = '\0';

	return buf_;
}

void SerialConnection::parseParams(const char* params)
{
	char* baud = std::strtok(const_cast<char*>(params), Connection::ParamArgsDelimiter);
	char* frame = nullptr, * timeout = nullptr;

	if (baud)
		baud_ = std::atol(baud);
	if ((frame = std::strtok(nullptr, Connection::ParamArgsDelimiter)))
	{
		for (auto i : hardware_serial::SupportedFrames)
		{
			if (!std::strncmp(frame, i.string(), std::strlen(frame)))
			{
				frame_ = i.value();
				break;
			}
		}
	}
	if ((timeout = std::strtok(nullptr, Connection::ParamArgsDelimiter)))
		timeout_ = std::atol(timeout);
}

hardware_serial& SerialConnection::hardware()
{
	return hardware_;
}

#pragma endregion
#pragma region UdpConnection

UdpConnection::UdpConnection(const char* params) : 
	ssid_(), pw_(), status_(WL_IDLE_STATUS), udp_(), ipa_(), port_(), buf_() 
{
	
}

UdpConnection::~UdpConnection()
{
	close();
}

void UdpConnection::open(const char* params)
{
	if (WiFi.status() != WL_NO_SHIELD)
	{
		uint32_t waited = 0;

		parseParams(params);
		while (status_ != WL_CONNECTED && waited < WifiMaxWaitTime)
		{
			status_ = WiFi.begin(ssid_, pw_);
			delay(WifiWaitConnect);
			waited += WifiWaitConnect;
		}
		udp_.begin(port_);
	}
}

bool UdpConnection::open()
{
	return status_ == WL_CONNECTED;
}

void UdpConnection::close()
{
	WiFi.disconnect();
}

std::size_t UdpConnection::send(const char* message)
{
	if (open())
	{
		udp_.beginPacket(ipa_, port_);
		udp_.write(message);
		udp_.endPacket();
	}
}

const char* UdpConnection::receive()
{
	std::size_t n = 0;

	if (open())
	{
		if (udp_.parsePacket() && udp_.remoteIP() == ipa_)
		{
			n = udp_.read(buf_, sizeof(buf_));
			udp_.endPacket();
		}
	}
	buf_[n] = '\0';

	return buf_;
}

void UdpConnection::parseParams(const char* params)
{
	char* port = nullptr, * ip = nullptr;

	ssid_ = std::strtok(const_cast<char*>(params), Connection::ParamArgsDelimiter);
	pw_ = std::strtok(nullptr, Connection::ParamArgsDelimiter);
	if ((ip = std::strtok(nullptr, Connection::ParamArgsDelimiter)))
		ipa_.fromString(const_cast<const char*>(ip));
	if ((port = std::strtok(nullptr, Connection::ParamArgsDelimiter)))
		port_ = std::atoi(port);
}

WiFiClass& UdpConnection::hardware()
{
	return WiFi;
}

#pragma endregion

} // namespace pg

#endif // __PG_CONNECTION_H
