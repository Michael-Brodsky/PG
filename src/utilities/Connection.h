/*
 *	This files defines several types for managing network connections using a 
 *	common interface.
 *
 *	***************************************************************************
 *
 *	File: Connection.h
 *	Date: July 25, 2022
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
 *	UPDATES:
 * 
 *	Fixed bugs in WiFiConnection & EthernetConnection: removed 
 *	udp_.endPacket() in receive() methods, which only apply to sending.
 * 
 *	TODO:
 * 
 *	Make async/concurrent version `AsyncConnection' with buffered i/o, clock()
 *	method.
 *
 *	**************************************************************************/

#if !defined __PG_CONNECTION_H
# define __PG_CONNECTION_H 20220625L

# include <cstdio>
# include <cstdlib>
# include <lib/strtok.h>
# include <system/boards.h>
# include <interfaces/iclockable.h>
# include <utilities/ValueWrappers.h>
# if !defined __PG_NO_ETHERNET_CONNECTION
#  include <system/ethernet.h>
# endif
# if !defined __PG_NO_WIFI_CONNECTION
#  include <system/wifi.h>
# endif

namespace pg
{
	// Network connection abstract interface class.
	class Connection : public iclockable
	{
	public:
		enum Maintain : uint8_t
		{
			NothingHappened = 0, 
			RenewFailed = 1, 
			RenewSuccess = 2, 
			RebindFailed = 3, 
			RebindSuccess = 4
		};
		enum class Type
		{
			 Serial = 0,
			 Ethernet = 1,
			 WiFi = 2,
			 Invalid = 3
		};
		using size_type = uint8_t;

		static constexpr const char* ParamsDelimiterChar = ",";
		static constexpr size_type size() 
		{ 
			return SERIAL_RX_BUFFER_SIZE < SERIAL_TX_BUFFER_SIZE 
				? SERIAL_RX_BUFFER_SIZE 
				: SERIAL_TX_BUFFER_SIZE; 
		}

	public:
		virtual ~Connection() = default;

	public:
		virtual void open(const char*) = 0;
		virtual bool open() = 0;
		virtual void close() = 0;
		virtual void flush() = 0;
		virtual size_type send(const char*) = 0;
		virtual const char* params(char*) = 0;
		virtual Maintain maintainConnection() = 0;
		Type type() const { return type_; }
		const char* receive()
		{
			char* msg = next_;

			if (*msg)
				next_ += (strlen(next_) + 1);

			return msg;
		}

	protected:
		explicit Connection(Type type) : type_(type) {}

	protected:
		size_type remaining(char* ptr, char* buf) { return size() - (ptr - buf); }

	protected:
		char* next_;

	private:
		Type type_;
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
		void open(const char*) override;
		bool open() override;
		void close() override;
		void flush() override;
		size_type send(const char*) override;
		const char* params(char*) override;
		HardwareSerial& hardware();
		baud_type baud() const;
		frame_type frame() const;
		timeout_type timeout() const;
		void clock() override;
		Maintain maintainConnection() override;

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
# if defined __PG_ETHERNET_H
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
		size_type send(const char*) override;
		const char* params(char*) override;
		EthernetClass& hardware();
		const mac_type& mac() const;
		uint16_t port() const;
		void remoteIP(IPAddress);
		IPAddress remoteIP() const;
		IPAddress localIP() const;
		void clock() override;
		Maintain maintainConnection() override;

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
# endif
# if defined __PG_WIFI_H
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
		size_type send(const char*) override;
		const char* params(char*) override;
		WiFiClass& hardware();
		const char* ssid() const;
		uint16_t port() const;
		void remoteIP(IPAddress);
		IPAddress remoteIP() const;
		IPAddress localIP() const;
		void clock() override;
		Maintain maintainConnection() override;

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
# endif
#pragma region SerialConnection

	SerialConnection::SerialConnection(HardwareSerial& hs, const char* params) : 
		Connection(Type::Serial), hardware_(hs), baud_(), frame_(DefaultFrame),
		timeout_(DefaultTimeout), is_open_(), buf_() 
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

	Connection::size_type SerialConnection::send(const char* message)
	{
		return hardware_.println(message);
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

		if ((tok = pg::strtok(buf, params, ParamsDelimiterChar, size()))) // Parse a copy of params.
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

	void SerialConnection::clock()
	{
		char* p = buf_;

		if (Serial.available())
		{
			do
			{
				p += Serial.readBytesUntil('\n', p, remaining(p, buf_));
				*p++ = '\0';
			} while (Serial.available());
			*p = '\0';
			next_ = buf_;
		}
	}

	Connection::Maintain SerialConnection::maintainConnection()
	{
		return Connection::Maintain::NothingHappened;
	}
#pragma endregion
#pragma region EthernetConnection
# if defined __PG_ETHERNET_H
	EthernetConnection::EthernetConnection(const char* params) : 
		Connection(Type::Ethernet), buf_(), is_open_(), local_ip_(), mac_(), port_()
	{
		if (params)
			open(params);
	}

	EthernetConnection::~EthernetConnection()
	{
		close();
	}

	void EthernetConnection::open(const char* params) 
	{
		parseParams(params);
#  if !defined __PG_NO_ETHERNET_DHCP 
		if (local_ip_ == IPAddress())
			Ethernet.begin(mac_.data());
		else
#  endif
		Ethernet.begin(mac_.data(), local_ip_);
		// hardwareStatus() valid only after begin() called.
		if (!(Ethernet.hardwareStatus() == EthernetNoHardware) || Ethernet.linkStatus() == LinkOFF)
		{
			local_ip_ = localIP();	// Save the actual ip assigned.
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

	Connection::size_type EthernetConnection::send(const char* message)
	{
		size_type n = 0;

		if (open())
		{
			if (udp_.beginPacket(remote_ip_, port_))
			{
				n = udp_.write(message);
				n = udp_.endPacket() ? n : 0;
			}
		}

		return n;
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

	const mac_type& EthernetConnection::mac() const
	{
		return mac_;
	}

	uint16_t EthernetConnection::port() const
	{
		return port_;
	}

	void EthernetConnection::remoteIP(IPAddress ip)
	{
		remote_ip_ = ip;
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
		char buf[size()];
		char* ptr = buf;
		char* tok = pg::strtok(buf, params, ParamsDelimiterChar, sizeof(buf)); // Parse a copy of params.
		size_type n = 0;

		if (tok)
		{
			n = std::strlen(tok);
			if ((tok = std::strtok(tok, MacDelimiterChar)))
			{
				size_type i = 0;

				do
				{
					mac_[i++] = std::strtol(tok, nullptr, 16);
				} while ((tok = std::strtok(nullptr, MacDelimiterChar)));
			}
		}
		ptr += ++n;
		if ((tok = std::strtok(ptr, ParamsDelimiterChar)))
		{
			local_ip_.fromString(tok);
		}
		if (*ptr == ',')
		{
			++ptr;
			tok = std::strtok(ptr, ParamsDelimiterChar);
		}
		else
			tok = std::strtok(nullptr, ParamsDelimiterChar);
		if (tok)
			port_ = std::atoi(tok);
	}

	void EthernetConnection::clock()
	{
		char* p = buf_;

		if (udp_.parsePacket())
		{
			do
			{
				p += udp_.read(p, remaining(p, buf_));
				*p++ = '\0';
			} while (udp_.parsePacket());
			*p = '\0';
			next_ = buf_;
			remote_ip_ = udp_.remoteIP();
		}
	}

	Connection::Maintain EthernetConnection::maintainConnection()
	{
# if !defined __PG_NO_ETHERNET_DHCP
		return static_cast<Maintain>(hardware().maintain());
# else
		return Connection::Maintain::NothingHappened;
# endif
	}


# endif
#pragma endregion
#pragma region WiFiConnection
# if defined __PG_WIFI_H
	WiFiConnection::WiFiConnection(const char* params) : 
		Connection(Type::WiFi), ssid_(), pw_(), status_(WL_IDLE_STATUS), udp_(), remote_ip_(), port_(), buf_()
	{
		if (params)
			open(params);
	}

	WiFiConnection::~WiFiConnection()
	{
		close();
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

	Connection::size_type WiFiConnection::send(const char* message)
	{
		size_type n = 0;

		if (open())
		{
			if (udp_.beginPacket(remote_ip_, port_))
			{
				n = udp_.write(message);
				n = udp_.endPacket() ? n : 0;
			}
		}

		return n;
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

		if ((ssid_ = pg::strtok(buf, params, ParamsDelimiterChar, size()))) // Parse a copy of params.
			if ((pw_ = std::strtok(nullptr, ParamsDelimiterChar)))
				if ((port = std::strtok(nullptr, ParamsDelimiterChar)))
					port_ = std::atoi(port);
	}

	void WiFiConnection::clock()
	{
		char* p = buf_;

		if (udp_.parsePacket())
		{
			do
			{
				p += udp_.read(p, remaining(p, buf_));
				*p++ = '\0';
			} while (udp_.parsePacket());
			*p = '\0';
			next_ = buf_;
			remote_ip_ = udp_.remoteIP();
		}
	} 

	Connection::Maintain WiFiConnection::maintainConnection()
	{
		return Connection::Maintain::NothingHappened;
	}

# endif
#pragma endregion

} // namespace pg

#endif // __PG_CONNECTION_H
/*

*/