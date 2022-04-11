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
 *	**************************************************************************/

#if !defined __PG_CONNECTION_H
# define __PG_CONNECTION_H 20220401

# include "cstring"
# include "system/boards.h"
# include "utilities/ValueWrappers.h"
# include "system/wifi.h"

namespace pg
{
	// Connection abstract interface class.
	struct Connection
	{
		enum class ConnectionType
		{
			None = 0,
			Serial = 1,
			Udp = 2
		};

		static constexpr const char ArgsDelimiterChar = ',';

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
		static constexpr const char EndOfMessageChar = '\n';

	public:
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
		const char* ssid() const;
		unsigned int port() const;
		IPAddress remoteIP() const;
		void remoteIP(IPAddress);
		IPAddress localIP() const;

	private:
		void parseParams(const char* params);

	private:
		String			ssid_;		// Network ssid.
		String			pw_;		// Network passphrase.
		int				status_;	// Network connection status.
		WiFiUDP			udp_;		// Arduino UDP api.
		IPAddress		local_ip_;	// The current remote address.
		IPAddress		remote_ip_;	// The current remote address.
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
	std::size_t eot = hardware_.readBytesUntil(EndOfMessageChar, buf_, sizeof(buf_));

	buf_[eot] = '\0';

	return buf_;
}

void SerialConnection::parseParams(const char* params)
{
	String str = params;
	auto n = str.indexOf(ArgsDelimiterChar);

	if (n > 0)
	{
		baud_ = str.substring(0, n).toInt();
		str = str.substring(n + 1);
	}
	if ((n = str.indexOf(ArgsDelimiterChar)) > 0)
	{
		String frame = str.substring(0, n);

		for (auto i : SupportedFrames)
		{
			if (frame == i.string())
			{
				frame_ = i.value();
				break;
			}
		}
		str = str.substring(n + 1);
		if (str.length() > 0)
			timeout_ = str.toInt();
	}
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
	ssid_(), pw_(), status_(WL_IDLE_STATUS), udp_(), remote_ip_(), local_ip_(), port_(), buf_()
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
			status_ = WiFi.begin(const_cast<ssid_t>(ssid_.c_str()), pw_.c_str());
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
		udp_.beginPacket(remote_ip_, port_);
		udp_.write(message);
		udp_.endPacket();
	}
}

const char* UdpConnection::receive()
{
	std::size_t n = 0;

	if (open())
	{
		if (udp_.parsePacket())
		{
			n = udp_.read(buf_, sizeof(buf_));
			remote_ip_ = udp_.remoteIP();
			udp_.endPacket();
		}
	}
	buf_[n] = '\0';

	return buf_;
}

void UdpConnection::parseParams(const char* params)
{
	String str = params;
	auto n = str.indexOf(',');

	if (n > 0)
	{
		ssid_ = str.substring(0, n);
		str = str.substring(n+1);
	}
	if ((n = str.indexOf(ArgsDelimiterChar)) > 0)
	{
		pw_ = str.substring(0, n);
		str = str.substring(n+1);
		port_ = str.toInt();
	}
}

WiFiClass& UdpConnection::hardware()
{
	return WiFi;
}

const char* UdpConnection::ssid() const
{
	return ssid_.c_str();
}

unsigned int UdpConnection::port() const
{
	return port_;
}

IPAddress UdpConnection::remoteIP() const
{
	return remote_ip_;
}

void UdpConnection::remoteIP(IPAddress ip)
{
	remote_ip_ = ip;
}

IPAddress UdpConnection::localIP() const
{
	return local_ip_;
}

#pragma endregion

} // namespace pg

#endif // __PG_CONNECTION_H
