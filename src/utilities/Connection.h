/*
 *	This files defines several types for managing network connections using a 
 *	common interface.
 *
 *	***************************************************************************
 *
 *	File: Connection.h
 *	Date: May 4, 2022
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
# define __PG_CONNECTION_H 20220504L

# include <cstdio>
# include <cstdlib>
# include <lib/strtok.h>
# include <system/boards.h>
# include <utilities/ValueWrappers.h>
# include <system/ethernet.h>
# include <system/wifi.h>

namespace pg
{
	// Network connection abstract interface class.
	struct Connection
	{
		// Enumerates valid connection types.
		enum class Type
		{
			 Serial = 0,
			 Ethernet = 1,
			 WiFi = 2,
			 Invalid = 3
		};

		static constexpr const char* ParamsDelimiterChar = ",";
		static constexpr std::size_t size() 
		{ 
			return SERIAL_RX_BUFFER_SIZE < SERIAL_TX_BUFFER_SIZE 
				? SERIAL_RX_BUFFER_SIZE 
				: SERIAL_TX_BUFFER_SIZE; 
		}

		virtual ~Connection() = default;

		virtual Type type() const = 0;
		virtual void open(const char*) = 0;
		virtual bool open() = 0;
		virtual void close() = 0;
		virtual void flush() = 0;
		virtual std::size_t send(const char*) = 0;
		virtual const char* receive() = 0;
		virtual const char* params(char*) = 0;
	};

	// Creates a serial network connection.
	class SerialConnection : public Connection
	{
	public:
		using baud_type = unsigned long;
		using frame_type = uint8_t;
		using timeout_type = unsigned long;
		using frame_map_type = StringValue<frame_type>;
		using frames_type = std::ArrayWrapper<frame_map_type>;

		static const frames_type SupportedFrames;
		static constexpr frame_map_type DefaultFrame = frame_map_type{ SERIAL_8N1, "8N1" };
		static constexpr timeout_type DefaultTimeout = 0;
		static constexpr const char EndOfMessageChar = '\n';

	public:
		SerialConnection(HardwareSerial&, const char* = nullptr);
		~SerialConnection() override;
	
	public:
		Type type() const override;
		void open(const char*) override;
		bool open() override;
		void close() override;
		void flush() override;
		std::size_t send(const char*) override;
		const char* receive() override;
		const char* params(char*) override;
		HardwareSerial& hardware();
		baud_type baud() const;
		frame_type frame() const;
		timeout_type timeout() const;

	private:
		void parseParams(const char* params);

	private:
		HardwareSerial&		hardware_;		// References the port hardware layer.
		baud_type			baud_;			// The current baud rate.
		frame_map_type		frame_;			// The current frame.
		timeout_type		timeout_;		// The current write timeout in milliseconds.
		bool				is_open_;		// Flag indicating whether the connection is currently open.
		char				buf_[size()];	// Receive buffer. 
	};

	namespace detail
	{
		SerialConnection::frame_map_type supported_frames[] =
		{
			{SERIAL_8N1, "8N1"}, 
			{SERIAL_7E1, "7E1"}
		};
	}

	const std::ArrayWrapper<SerialConnection::frame_map_type> SerialConnection::SupportedFrames(detail::supported_frames);

	class EthernetConnection : public Connection
	{
	public:
		static constexpr uint32_t WaitConnect = 2000;
		static constexpr uint32_t MaxWaitTime = 10000;
		static constexpr const char* MacDelimiterChar = " ";

	public:
		EthernetConnection(const char* = nullptr);
		~EthernetConnection() override;

	public:
		Type type() const;
		void open(const char*) override;
		bool open() override;
		void close() override;
		void flush() override;
		std::size_t send(const char*) override;
		const char* receive() override;
		const char* params(char*) override;
		EthernetClass& hardware();
		mac_type mac() const;
		uint16_t port() const;
		IPAddress remoteIP() const;
		IPAddress localIP() const;

	private:
		void parseParams(const char* params);

	private:
		char			buf_[size()];	// Receive buffer.
		EthernetUDP		udp_;			// Arduino UDP api.
		bool			is_open_;		// Flag indicating whether the connection is open.
		IPAddress		local_ip_;		// The current local IP address.
		mac_type		mac_;			// The MAC address.
		unsigned int	port_;			// The current UDP port.
		IPAddress		remote_ip_;		// The current remote address.
	};

	// Creates a WiFi network connection.
	class WiFiConnection : public Connection
	{
	public:
		static constexpr uint32_t WaitConnect = 2000; 
		static constexpr uint32_t MaxWaitTime = 10000;

	public:
		WiFiConnection(const char* = nullptr);
		~WiFiConnection() override;

	public:
		Type type() const;
		void open(const char*) override;
		bool open() override;
		void close() override;
		void flush() override;
		std::size_t send(const char*) override;
		const char* receive() override;
		const char* params(char*) override;
		WiFiClass& hardware();
		const char* ssid() const;
		uint16_t port() const;
		IPAddress remoteIP() const;
		void remoteIP(IPAddress);
		IPAddress localIP() const;

	private:
		void parseParams(const char* params);

	private:
		char*			ssid_;			// Network ssid.
		char*			pw_;			// Network passphrase.
		unsigned int	port_;			// The current UDP port.
		int				status_;		// Network connection status.
		WiFiUDP			udp_;			// Arduino UDP api.
		IPAddress		remote_ip_;		// The current remote address.
		char			buf_[size()];	// Receive buffer.

	};

#pragma region SerialConnection

	SerialConnection::SerialConnection(HardwareSerial& hs, const char* params) : 
		hardware_(hs), baud_(), frame_(DefaultFrame), 
		timeout_(DefaultTimeout), is_open_(), buf_() 
	{
		if (params)
			open(params);
	}

	SerialConnection::~SerialConnection()
	{
		close();
	}

	Connection::Type SerialConnection::type() const
	{
		return Type::Serial;
	}

	void SerialConnection::open(const char* params)
	{
		parseParams(params);
		hardware_.begin(baud_, frame_.value());
		if(timeout_ != DefaultTimeout)
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
		return hardware_.println(message);
	}

	const char* SerialConnection::receive()
	{
		std::size_t len = Serial.available() ? hardware_.readBytesUntil(EndOfMessageChar, buf_, sizeof(buf_)) : 0;

		buf_[len] = '\0';

		return buf_;
	}

	const char* SerialConnection::params(char* buf)
	{
		(void)std::sprintf(buf, "%lu,%3s,%lu", baud_, frame_.string(), timeout_);

		return buf;
	}

	void SerialConnection::parseParams(const char* params)
	{
		char buf[size()] = { '\0' };
		char* tok = nullptr;

		if ((tok = pg::strtok(buf, params, ParamsDelimiterChar, sizeof(buf))))
		{
			baud_ = std::atol(tok);
			if ((tok = std::strtok(nullptr, ParamsDelimiterChar)))
			{
				auto it = std::find(static_cast<frames_type>(SupportedFrames).begin(), 
					static_cast<frames_type>(SupportedFrames).end(), (tok));

				if (it)
					frame_ = *it;
				if((tok = std::strtok(nullptr, ParamsDelimiterChar)))
					timeout_ = std::atol(tok);
			}
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
		return frame_.value();
	}

	SerialConnection::timeout_type SerialConnection::timeout() const 
	{ 
		return timeout_; 
	}

#pragma endregion
#pragma region EthernetConnection

	EthernetConnection::EthernetConnection(const char* params) : 
		buf_(), is_open_(), local_ip_(), mac_(), port_()
	{
		if (params)
			open(params);
	}

	EthernetConnection::~EthernetConnection()
	{
		close();
	}

	Connection::Type EthernetConnection::type() const
	{
		return Type::Ethernet;
	}

	void EthernetConnection::open(const char* params) 
	{
		parseParams(params);
		if (!(Ethernet.hardwareStatus() == EthernetNoHardware) || Ethernet.linkStatus() == LinkOFF)
		{
			Ethernet.begin(mac_.data(), local_ip_);
			is_open_ = udp_.begin(port_);
		}
	}

	bool EthernetConnection::open()
	{
		return is_open_;
	}

	void EthernetConnection::close()
	{
		udp_.stop();
		is_open_ = false;
	}

	void EthernetConnection::flush()
	{
		udp_.flush();
	}

	std::size_t EthernetConnection::send(const char* message)
	{
		if (open())
		{
			udp_.beginPacket(remote_ip_, port_);
			udp_.write(message);
			udp_.endPacket();
		}
	}

	const char* EthernetConnection::receive()
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

	const char* EthernetConnection::params(char* buf)
	{
		if (buf)
		{
			IPAddress ip = localIP();

			(void)std::sprintf(buf, "%d %d %d %d %d %d,%d.%d.%d.%d:%u", 
				mac_[0], mac_[1], mac_[2], mac_[3], mac_[4], mac_[5], ip[0], ip[1], ip[2], ip[3], port_);
		}

		return buf;
	}

	EthernetClass& EthernetConnection::hardware()
	{
		return Ethernet;
	}

	uint16_t EthernetConnection::port() const
	{
		return port_;
	}

	IPAddress EthernetConnection::remoteIP() const
	{
		return remote_ip_;
	}

	IPAddress EthernetConnection::localIP() const
	{
		return Ethernet.localIP();
	}

	void EthernetConnection::parseParams(const char* params)
	{
		char buf[size()] = { '\0' };
		char* ip = nullptr;
		char* mac = nullptr;
		char* port = nullptr;

		if ((mac = pg::strtok(buf, params, ParamsDelimiterChar, sizeof(buf))))
		{
			std::size_t i = 0;

			do
			{
				mac_[i++] = static_cast<typename mac_type::value_type>(std::strtol(mac, nullptr, 16));
			} while ((mac = std::strtok(nullptr, MacDelimiterChar)));
		}
		if ((ip = std::strtok(nullptr, MacDelimiterChar)))	// Might need to be comma.
			local_ip_.fromString(ip);
		if ((port = std::strtok(nullptr, ParamsDelimiterChar)))
			port_ = std::atoi(port);
	}

#pragma endregion
#pragma region WiFiConnection

	WiFiConnection::WiFiConnection(const char* params) : 
		ssid_(), pw_(), status_(WL_IDLE_STATUS), udp_(), remote_ip_(), port_(), buf_()
	{
		if (params)
			open(params);
	}

	WiFiConnection::~WiFiConnection()
	{
		close();
	}

	Connection::Type WiFiConnection::type() const
	{
		return Type::WiFi;
	}

	void WiFiConnection::open(const char* params)
	{
		if (WiFi.status() != WL_NO_SHIELD)
		{
			uint32_t waited = 0;

			parseParams(params);
			while (status_ != WL_CONNECTED && waited < MaxWaitTime)
			{
				status_ = WiFi.begin(const_cast<ssid_t>(ssid_), pw_);
				delay(WaitConnect);
				waited += WaitConnect;
			}
			udp_.begin(port_);
		}
	}

	bool WiFiConnection::open()
	{
		return status_ == WL_CONNECTED;
	}

	void WiFiConnection::close()
	{
		WiFi.disconnect();
	}

	void WiFiConnection::flush()
	{
		udp_.flush();
	}

	std::size_t WiFiConnection::send(const char* message)
	{
		if (open()) 
		{
			udp_.beginPacket(remote_ip_, port_);
			udp_.write(message);
			udp_.endPacket();
		}
	}

	const char* WiFiConnection::receive()
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

	const char* WiFiConnection::params(char* buf)
	{
		if (buf)
		{
			IPAddress ip = localIP();

			(void)std::sprintf(buf, "%s,%d.%d.%d.%d:%u", WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], port_);
		}

		return buf;
	}

	WiFiClass& WiFiConnection::hardware()
	{
		return WiFi;
	}

	const char* WiFiConnection::ssid() const
	{
		return WiFi.SSID();
	}

	uint16_t WiFiConnection::port() const
	{
		return port_;
	}

	IPAddress WiFiConnection::remoteIP() const
	{
		return remote_ip_;
	}

	void WiFiConnection::remoteIP(IPAddress ip)
	{
		remote_ip_ = ip;
	}

	IPAddress WiFiConnection::localIP() const
	{
		return WiFi.localIP();
	}

	void WiFiConnection::parseParams(const char* params)
	{
		char buf[size()] = { '\0' };
		char* port = nullptr;

		if ((ssid_ = pg::strtok(buf, params, ParamsDelimiterChar, sizeof(buf))))
			if ((pw_ = std::strtok(nullptr, ParamsDelimiterChar)))
				if ((port = std::strtok(nullptr, ParamsDelimiterChar)))
					port_ = std::atoi(port);
	}

#pragma endregion

} // namespace pg

#endif // __PG_CONNECTION_H
