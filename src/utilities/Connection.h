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
 *		SerialConnection objects are constructed by passing one of the  
 *		HardwareSerial objects to the constructor and specifying the port 
 *		to use. For example: 
 * 
 *			
 *			SerialConnection ser(Serial);
 *			SerialConnection ser(Serial1,"9600,8N1");
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
#include <utilities/ValueWrappers.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>

namespace pg
{
	namespace detail
	{

	} // namespace detail

	// Connection abstract interface class.
	struct Connection
	{
		using string_type = const char*;

		enum class ConnectionType
		{
			None = 0,
			Serial = 1,
			Udp = 2
		};

		static constexpr const char* const ParamArgsDelimiter = ",";
		static constexpr const char* const IpAddressDelimiter = ".";

		virtual ~Connection() = default;

		virtual void open(const char* params) = 0;
		virtual bool open() = 0;
		virtual void close() = 0;
		virtual void flush() = 0;
		virtual std::size_t send(const char* message) = 0;
		virtual const char* receive() = 0;
		virtual ConnectionType type() const = 0;
	};

	// Connects a remote host using a serial port (usart).
	class SerialConnection : public Connection
	{
	public:
		using string_type = Connection::string_type;
		using baud_type = unsigned long;
		using frame_type = uint8_t;
		using timeout_type = unsigned long;
		using frame_map_type = StringValue<frame_type>;

	public:
		static constexpr const std::size_t TxBufferSize = SERIAL_TX_BUFFER_SIZE;
		static constexpr const std::size_t RxBufferSize = SERIAL_RX_BUFFER_SIZE;
		static constexpr const baud_type DefaultBaudRate = 9600;
		static constexpr const frame_type DefaultFrame = SERIAL_8N1;
		static constexpr const timeout_type DefaultTimeout = 1000;
		static const std::ArrayWrapper<const baud_type> SupportedBaudRates;
		static const std::ArrayWrapper<const frame_map_type> SupportedFrames;

	public:
		SerialConnection(HardwareSerial&, const char* = nullptr);
		~SerialConnection() override;

	public:
		ConnectionType type() const;
		void open(const char* params) override;
		bool open() override;
		void close() override;
		void flush() override;
		std::size_t send(const char* message) override;
		const char* receive() override;
		HardwareSerial& hardware();
		baud_type baud() const;
		frame_type frame() const;
		timeout_type timeout() const;
	private:
		void parseParams(const char* params);

	private:
		HardwareSerial&		hardware_;				// References the port hardware layer.
		baud_type			baud_;					// The current baud rate.
		frame_type			frame_;					// The current frame.
		timeout_type		timeout_;				// The current write timeout in milliseconds.
		bool				is_open_;				// Flag indicating whether the connection is currently open.
		char				buf_[RxBufferSize];		// Receive buffer. 
	};

	namespace detail
	{
		const SerialConnection::frame_map_type supported_frames[] =
		{
			{SERIAL_8N1, "8N1"}, 
			{SERIAL_7E1, "7E1"}
		};
		const SerialConnection::baud_type supported_baud_rates[] = 
		{
			300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 56000, 57600, 74880, 115200
		};

	}
	const std::ArrayWrapper<const SerialConnection::frame_map_type> SerialConnection::SupportedFrames(detail::supported_frames);
	const std::ArrayWrapper<const SerialConnection::baud_type> SerialConnection::SupportedBaudRates(detail::supported_baud_rates);

	// Connects a remote host using UDP/IP (WiFi).
	class UdpConnection : public Connection
	{
	public:
		using string_type = Connection::string_type;

	public: 
		static constexpr const std::size_t UdpMaxPacketSize = 255;
		static constexpr const uint32_t WifiWaitConnect = 2000;
		static constexpr const uint32_t WifiMaxWaitTime = 10000;

	public:
		UdpConnection(const char* params = nullptr);
		~UdpConnection() override;

	public:
		ConnectionType type() const;
		void open(const char* params) override;
		bool open() override;
		void close() override;
		void flush() override;
		std::size_t send(const char* message) override;
		const char* receive() override;
		WiFiClass& hardware();
		unsigned int port() const;
		IPAddress ip() const;

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

SerialConnection::SerialConnection(HardwareSerial& hs, const char* params) : 
	hardware_(hs), baud_(DefaultBaudRate), frame_(DefaultFrame), 
	timeout_(DefaultTimeout), is_open_(), buf_() 
{
	if (params)
		open(params);
}

SerialConnection::~SerialConnection()
{
	close();
}

Connection::ConnectionType SerialConnection::type() const
{
	return ConnectionType::Serial;
}

void SerialConnection::open(const char* params)
{
	parseParams(params);
	hardware_.begin(baud_, frame_);
	hardware_.setTimeout(timeout_);
	while (!hardware_);
	is_open_ = true;
}

bool SerialConnection::open()
{
	return is_open_;
}

void SerialConnection::close()
{
	hardware_.end();
	is_open_ = false;
}

void SerialConnection::flush()
{
	hardware_.flush();
	while (hardware_.available())
		(void)hardware_.read();
}

std::size_t SerialConnection::send(const char* message)
{
	std::size_t n = 0;

	if (*message)
		n = hardware_.println(message);

	return n;
}

const char* SerialConnection::receive()
{
	std::size_t eot = hardware_.readBytesUntil('\n', buf_, sizeof(buf_));

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
		for (auto i : SupportedFrames)
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

HardwareSerial& SerialConnection::hardware()
{
	return hardware_;
}

SerialConnection::baud_type SerialConnection::baud() const
{ 
	return baud_; 
}

SerialConnection::frame_type SerialConnection::frame() const
{
	return frame_;
}

SerialConnection::timeout_type SerialConnection::timeout() const 
{ 
	return timeout_; 
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


Connection::ConnectionType UdpConnection::type() const
{
	return ConnectionType::Serial;
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

void UdpConnection::flush()
{
	udp_.flush();
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

unsigned int UdpConnection::port() const
{
	return port_;
}

IPAddress UdpConnection::ip() const
{
	return ipa_;
}

#pragma endregion

} // namespace pg

#endif // __PG_CONNECTION_H
