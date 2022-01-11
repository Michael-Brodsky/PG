/*
 *	This files defines several types that extend the capabilities of the 
 *	Arduino HardwareSerial and Serial objects.
 *
 *	***************************************************************************
 *
 *	File: Serial.h
 *	Date: January 10, 2022
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
 *		The `hardware_serial' class is a wrapper class for the Arduino API's 
 *		serial port objects (Serial,Serial1,Serial2,Serial3) that extends 
 *		their capabilities by holding state information, such as whether the 
 *		port is open, the current baud rate and framing and, by exposing 
 *		additional methods such as formatted printing.
 * 
 *		The `hardware_serial' class is a base class that is instantiated 
 *		using one of the indexed `serial<N>' templates, where N is an index 
 *		corresponding to one of the Arduino's serial port objects:
 * 
 *			serial<0> = Serial,
 *			serial<1> = Serial1,
 *			serial<2> = Serial2,
 *			serial<3> = Serial3,
 * 
 *		Serial port availability is determined by the type of board being 
 *		used.
 * 
 *		Each serial<N> object exposes the same methods as the wrapped  
 *		Arduino object and the following additional methods: 
 *		
 *			hardware(): returns a reference to the wrapped Arduino object,
 *			baud(): returns the current serial baud rate,
 *			frame(): returns the current serial frame,
 *			isOpen(): returns true after a succesful call to begin() until a 
 *					  successful call to end(), otherwide returns false.
 *			loopback(): copies the contents of the hardware read buffer to the 
 *						write buffer and returns the number of bytes written.
 *			printFmt(): prints formatted text to the hardware write buffer. 
 *						(see std::sprintf() for print formatting specs).
 *			isSupported(baud_type): checks whether a given baud rate is valid.
 *			isSupported(frame_type): checks whether a given frame is valid.
 * 
 *		To save memory, not all baud_rates and frames are supported. See 
 *		`supported_baud_rates_', `supported_frames_' and 
 *		`__PG_USE_ALL_SERIAL_FRAMES' below.
 * 
 *		The hardware_serial::begin() method differs from the Arduino begin() 
 *		method in that is fully defaulted. Calls to hardware_serial::begin() 
 *		with no arguments uses both the default baud rate and frame (see 
 *		`DefaultBaudRate' and `DefaultFrame' below).
 * 
 *		The `frame_map_type' maps Arduino frame values to their human-readable 
 *		counterpart: 
 * 
 *			SERIAL_8N1 maps to "8N1" 
 * 
 *		Clients can lookup the corresponding values using the std::find
 *		method:
 * 
 *			auto i = std::find(hardware_serial::SupportedFrames.begin(),
 *			  hardware_serial::SupportedFrames.end(), 
 *			  FRAME);
 * 
 *			where FRAME is a valid Arduino frame constant (e.g. SERIAL_8N1).
 * 
 *			i->string() will return the human readable value (e.g. "8N1").
 *			If an unsupported Arduino frame constant is given, std::find will 
 *			return hardware_serial::SupportedFrames.end() which is not 
 *			dereferencable. Therefore clients should check the return value 
 *			before attempting to use it.
 * 
 *	Usage:
 * 
 *		using namespace pg::usart;
 * 
 *		serial<0> s0; 
 *		s0.begin();
 *		if(s0.isOpen()) { 
 *			char buf[20];
 *			s0.printFmt(buf, "%s=%lu", "baud rate", s0.baud());
 *		}
 *		
 *	**************************************************************************/

#if !defined __PG_SERIAL_H
# define __PG_SERIAL_H 20220110L

# include "array"						// std::ArrayWrapper
# include "cstring"						// Cstring compare functions.
# include "cstdio"						// std::sprintf
# include "utilities/ValueWrappers.h"	// pg::StringValueWrapper type.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	namespace usart
	{
		using baud_type = unsigned long;
		using frame_type = uint8_t;
		using timeout_type = unsigned long;
		using string_type = const char*;
		using frame_map_type = StringValueWrapper<frame_type, string_type>;

		// Wrapper for Arduino HardwareSerial class that maintains state variables and exposes additional methods.
		class hardware_serial
		{
		public:
			static constexpr const baud_type DefaultBaudRate = 9600;
			static constexpr const frame_type DefaultFrame = SERIAL_8N1;
			static constexpr const timeout_type DefaultTimeout = 1000;
			static constexpr const std::size_t TxBufferSize = SERIAL_TX_BUFFER_SIZE;
			static constexpr const std::size_t RxBufferSize = SERIAL_RX_BUFFER_SIZE;
			static const std::ArrayWrapper<const baud_type> SupportedBaudRates;
			static const std::ArrayWrapper<const frame_map_type> SupportedFrames;

		public:
			hardware_serial(HardwareSerial&);
			hardware_serial(const hardware_serial&) = default;
			hardware_serial(hardware_serial&&) = default;
			hardware_serial& operator=(const hardware_serial&) = default;

		public:
			void begin(baud_type = DefaultBaudRate, frame_type = DefaultFrame);
			void end();
			timeout_type getTimeout() const;
			void setTimeout(timeout_type);
			std::size_t available();
			std::size_t availableForWrite();
			void clearWriteError();
			void flush();
			int8_t peek();
			int8_t read();
			std::size_t readBytes(char*, std::size_t);
			std::size_t readBytes(uint8_t*, std::size_t);
			std::size_t readBytesUntil(char, char*, std::size_t);
			std::size_t readBytesUntil(char, uint8_t*, std::size_t);
			std::size_t write(const char*);
			std::size_t write(const uint8_t*, std::size_t);
			std::size_t write(const char*, std::size_t);
			std::size_t write(int);
			std::size_t write(unsigned int);
			template<class T>
			std::size_t print(const T&);
			template<class T>
			std::size_t println(const T&);
			template<class ...Args>
			std::size_t printFmt(char*, const char*, Args...);
			HardwareSerial& hardware();
			baud_type baud() const;
			frame_type frame() const;
			bool isSupported(baud_type) const;
			bool isSupported(frame_type) const;
			bool isOpen() const;
			std::size_t loopBack();

		private:
			HardwareSerial&				hardware_;					// The wrapped Arduino HardwareSerial object.
			baud_type					baud_rate_;					// The current hardware baud rate.
			frame_type					frame_;						// The current hardware frame.
			bool						is_open_;					// Flag indicating whether the hardware `begin()' method has been called.
			static const baud_type		supported_baud_rates_[];	// Collection of supported hardware baud rates.
			static const frame_map_type	supported_frames_[];		// Collection of supported hardware frames.
		};

		// Primary template for derived hardware_serial objects.
		template<std::size_t N>
		class serial : public hardware_serial {};

#if defined HAVE_HWSERIAL0
		// Specialization for Arduino Serial.
		template<> class serial<0> : public hardware_serial
		{
		public: 
			serial() : hardware_serial(Serial) {}
		};
#endif
#if defined HAVE_HWSERIAL1
		// Specialization for Arduino Serial1.
		template<> class serial<1> : public hardware_serial
		{
		public: 
			serial() : hardware_serial(Serial1) {}
		};
#endif
#if defined HAVE_HWSERIAL2
		// Specialization for Arduino Seria2.
		template<> class serial<2> : public hardware_serial
		{
		public: 
			serial() : hardware_serial(Serial2) {}
		};
#endif
#if defined HAVE_HWSERIAL3
		// Specialization for Arduino Serial3.
		template<> class serial<3> : public hardware_serial
		{
		public: 
			serial() : hardware_serial(Serial3) {} 
		};
#endif

		const baud_type hardware_serial::supported_baud_rates_[] =
		{
			300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 56000, 57600, 74880, 115200
		};

		const frame_map_type hardware_serial::supported_frames_[] =
		{
			frame_map_type(SERIAL_8N1, "8N1"),
			frame_map_type(SERIAL_7E1, "7E1"),
#  if defined __PG_USE_ALL_SERIAL_FRAMES		// Clients must define this before including `Serial.h'
			frame_map_type(SERIAL_5N1, "5N1"),
			frame_map_type(SERIAL_6N1, "6N1"),
			frame_map_type(SERIAL_7N1, "7N1"),
			frame_map_type(SERIAL_5N2, "5N2"),
			frame_map_type(SERIAL_6N2, "6N2"),
			frame_map_type(SERIAL_7N2, "7N2"),
			frame_map_type(SERIAL_8N2, "8N2"),
			frame_map_type(SERIAL_5E1, "5E1"),
			frame_map_type(SERIAL_6E1, "6E1"),
			frame_map_type(SERIAL_8E1, "8E1"),
			frame_map_type(SERIAL_5E2, "5E2"),
			frame_map_type(SERIAL_6E2, "6E2"),
			frame_map_type(SERIAL_7E2, "7E2"),
			frame_map_type(SERIAL_8E2, "8E2"),
			frame_map_type(SERIAL_5O1, "5O1"),
			frame_map_type(SERIAL_6O1, "6O1"),
			frame_map_type(SERIAL_7O1, "7O1"),
			frame_map_type(SERIAL_8O1, "8O1"),
			frame_map_type(SERIAL_5O2, "5O2"),
			frame_map_type(SERIAL_6O2, "6O2"),
			frame_map_type(SERIAL_7O2, "7O2"),
			frame_map_type(SERIAL_8O2, "8O2")
#  endif
		};

		const std::ArrayWrapper<const baud_type> hardware_serial::SupportedBaudRates(hardware_serial::supported_baud_rates_);
		const std::ArrayWrapper<const frame_map_type> hardware_serial::SupportedFrames(hardware_serial::supported_frames_);

		hardware_serial::hardware_serial(HardwareSerial& hardware) :
			hardware_(hardware), baud_rate_(DefaultBaudRate), frame_(DefaultFrame), is_open_()
		{

		}

		void hardware_serial::begin(baud_type baud, frame_type frame)
		{
			hardware_.begin(baud, frame);
			baud_rate_ = baud;
			frame_ = frame;
			is_open_ = true;
		}

		void hardware_serial::end()
		{
			hardware_.end();
			is_open_ = false;
		}

		timeout_type hardware_serial::getTimeout() const
		{
			return hardware_.getTimeout();
		}

		void hardware_serial::setTimeout(timeout_type value)
		{
			hardware_.setTimeout(value); 
		}

		std::size_t hardware_serial::available()
		{
			return hardware_.available();
		}

		std::size_t hardware_serial::availableForWrite()
		{
			return hardware_.availableForWrite();
		}

		void hardware_serial::clearWriteError()
		{
			hardware_.clearWriteError();
		}

		void hardware_serial::flush()
		{
			hardware_.flush();
		}

		int8_t hardware_serial::peek()
		{
			return hardware_.peek();
		}

		int8_t hardware_serial::read()
		{
			return hardware_.read();
		}

		std::size_t hardware_serial::readBytes(char* buf, std::size_t n)
		{
			return hardware_.readBytes(buf, n);
		}

		std::size_t hardware_serial::readBytes(uint8_t* buf, std::size_t n)
		{
			return hardware_.readBytes(buf, n);
		}

		std::size_t hardware_serial::readBytesUntil(char until, char* buf, std::size_t n)
		{
			return hardware_.readBytesUntil(until, buf, n);
		}

		std::size_t hardware_serial::readBytesUntil(char until, uint8_t* buf, std::size_t n)
		{
			return hardware_.readBytesUntil(until, buf, n);
		}

		std::size_t hardware_serial::write(const char* str)
		{
			return hardware_.write(str);
		}

		std::size_t hardware_serial::write(const uint8_t* buf, std::size_t n)
		{
			return hardware_.write(buf, n);
		}

		std::size_t hardware_serial::write(const char* buf, std::size_t n)
		{
			return hardware_.write(buf, n); 
		}

		std::size_t hardware_serial::write(int value)
		{
			return hardware_.write(value);
		}

		std::size_t hardware_serial::write(unsigned int value)
		{
			return hardware_.write(value);
		}

		template<class T>
		std::size_t hardware_serial::print(const T& value)
		{
			return hardware_.print(value);
		}

		template<class T>
		std::size_t hardware_serial::println(const T& value)
		{
			return hardware_.println(value);
		}

		template<class ...Args>
		std::size_t hardware_serial::printFmt(char* buf, const char* fmt, Args... args)
		{
			std::sprintf(buf, fmt, args...);
			println(buf);
		}

		HardwareSerial& hardware_serial::hardware()
		{
			return hardware_;
		}

		baud_type hardware_serial::baud() const
		{
			return baud_rate_;
		}

		frame_type hardware_serial::frame() const
		{
			return frame_;
		}

		bool hardware_serial::isSupported(baud_type value) const
		{
			return std::find(SupportedBaudRates.begin(), SupportedBaudRates.end(), value) != SupportedBaudRates.end();
		}

		bool hardware_serial::isSupported(frame_type value) const
		{
			return std::find(SupportedFrames.begin(), SupportedFrames.end(), value) != SupportedFrames.end();
		}

		bool hardware_serial::isOpen() const
		{
			return is_open_;
		}

		std::size_t hardware_serial::loopBack()
		{
			const std::size_t BufferSize = std::min(RxBufferSize, TxBufferSize);
			char buf[BufferSize] = { '\0' };
			std::size_t n = 0;

			if (available())
			{
				readBytes(buf, BufferSize);
				n = print(buf);
			}

			return n;
		}
	} // namespace usart
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_SERIAL_H
