/*
 *	This file defines a class for reading and writing values to and from, and 
 *	getting and setting the status of gpio pins remotely using the serial port.
 * 
 *	***************************************************************************
 *
 *	File: Jack.h
 *	Date: January 11, 2022
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

#if !defined __PG_JACK_H
# define __PG_JACK_H 20220111L

# include <Arduino.h>
# include <valarray>
# include <system/boards.h>
# include <system/utils.h>
# include <interfaces/iserializable.h>
# include <utilities/EEStream.h>
# include <utilities/Timer.h>
# include <components/RemoteControl.h>

# if defined __PG_HAS_NAMESPACES  

namespace pg
{
	using pg::usart::serial;
	using pg::usart::hardware_serial;
	using pg::RemoteControl;
	using std::chrono::milliseconds;

	class Jack : public iclockable, public icomponent
	{
	public:
		static constexpr const uint8_t GpioCount = NUM_DIGITAL_PINS;
		static constexpr const uint8_t AnalogInCount = NUM_ANALOG_INPUTS;
		static constexpr const uint8_t LedPinNumber = LED_BUILTIN;
		static constexpr const unsigned long DeviceId() { return 20211226UL; }
		static constexpr const std::size_t MaxCommands = 32;
		static constexpr const std::size_t TimersCount = 4;
		static constexpr const int8_t NoTimer = -1;

		// Enumerates valid i/o pin types.
		enum gpio_type : uint8_t 
		{ 
			Digital = 4,	// Digital i/o
			Analog = 5,		// Analog in
			Pwm = 6 		// Pwm out
		};

		// Enumerates valid pin i/o modes.
		enum gpio_mode : uint8_t 
		{ 
			Input = 0,		// Input pin.
			Output = 1,		// Output pin.
			Pullup = 2,		// Input pin w/ internal pullup resistor.
			Disabled = 3	// Pin is disabled.
		};

		// Aggregates i/o pin type and mode info.
		struct GpioPin 
		{ 
			gpio_type	type_;		// Pin i/o type.
			gpio_mode	mode_;		// Pin i/o mode.
			bool		enabled_;	// Pin enabled flag.
			int8_t		timer_;		// Event timer number, -1 if not assigned.
		};
		
		using Pins = std::array<GpioPin, GpioCount>;						// GpioPin collection container type.
		using baud_t = usart::baud_type;									// Port baud rate storage type. 
		using frame_t = usart::frame_type;									// Port frame storage type.  
		using timeout_t = usart::timeout_type;								// Port timeout storage type.
		using frame_v = usart::frame_map_type;								// Port frame to string mapping type.
		using command_type = RemoteControl::command_type;					// Jack command type.
		using cmdkey_t = typename command_type::key_type;					// Command key storage type.
		using ilist_type = std::initializer_list<command_type*>;			// Commands initializer list type.
		using callback_type = typename callback<void>::type;				// Client callback signature type.
		using timer_type = Timer<milliseconds>;								// Event timer type.
		using timer_cmd_type = Command<void, Jack, int8_t>;
		using commands_cont = typename std::valarray<command_type*, MaxCommands>;	// Commands collection container type.
		using timers_cont = typename std::array<timer_type, TimersCount>;	// Event timers collection container type.

		// Serializable type that stores current program settings.
		class Settings : public iserializable 
		{
		public:
			Settings(baud_t baud = hardware_serial::DefaultBaudRate, 
				frame_t frame = hardware_serial::DefaultFrame, 
				timeout_t timeout = hardware_serial::DefaultTimeout) :
				baud_(baud), frame_(frame), timeout_(timeout)
			{}
		public:
			baud_t& baud() { return baud_; }
			const baud_t& baud() const { return baud_; }
			frame_t& frame() { return frame_; }
			const frame_t& frame() const { return frame_; }
			timeout_t& timeout() { return timeout_; }
			const timeout_t& timeout() const { return timeout_; }
			void serialize(EEStream& e) const override { e << baud_; e << frame_; e << timeout_; }
			void deserialize(EEStream& e) override { e >> baud_; e >> frame_; e >> timeout_; }

		private:
			baud_t		baud_;
			frame_t		frame_;
			timeout_t	timeout_;
		};

		//
		// Command key strings.
		//

		static constexpr const cmdkey_t KeyDevReset = "rst";
		static constexpr const cmdkey_t KeyDevPing = "png";
		static constexpr const cmdkey_t KeyDevInfo = "inf";
		static constexpr const cmdkey_t KeyGetComms = "gcm";
		static constexpr const cmdkey_t KeySetComms = "scm";
		static constexpr const cmdkey_t KeyGetPins = "pns";
		static constexpr const cmdkey_t KeyGetPinMode = "gpm";
		static constexpr const cmdkey_t KeySetPinMode = "spm";
		static constexpr const cmdkey_t KeyAnalogRead = "ard";
		static constexpr const cmdkey_t KeyAnalogWrite = "awr";
		static constexpr const cmdkey_t KeyDigitalRead = "drd";
		static constexpr const cmdkey_t KeyDigitalWrite = "dwr";
		static constexpr const cmdkey_t KeyReadAllPins = "all";
		static constexpr const cmdkey_t KeySetTimer = "stm";
		static constexpr const cmdkey_t KeyGetTimer = "gtm";

		// 
		// Command reply formatting strings.
		//

		static constexpr const char* FmtGetPinMode = "%s=%u,%u,%u";
		static constexpr const char* FmtGetComms = "%s=%lu,%s,%lu";
		static constexpr const char* FmtDevInfo = "%s=%lu,%s,%s,%.1f";
		static constexpr const char* FmtDevPing = "%s=%lu";
		static constexpr const char* FmtSendPin = "%u=%u";

	public: // Class Constructors
		explicit Jack(hardware_serial, EEStream&, callback_type = nullptr, ilist_type = ilist_type());

	public: // Class Methods
		void poll();
		const commands_cont& commands() const;
		commands_cont& commands();
		hardware_serial& commsHardware();
		void initialize();
		void readDigital(pin_t);
		void readAnalog(pin_t);
		void readAllPins();
		void writeDigital(pin_t, bool);
		void writeAnalog(pin_t, analog_t);
		void setPinMode(pin_t, uint8_t);
		void getPinMode(pin_t);
		void getPins();
		void setComms(baud_t, frame_t, timeout_t);
		void getComms();
		void setTimer(uint8_t, time_t, const char*);
		void getTimer(uint8_t);
		void devInfo();
		void devReset();
		void devPing();

	private:
		void addCommands(ilist_type&);
		void loadSettings(EEStream&, Settings&);
		void storeSettings(EEStream&, const Settings&);
		void clock() override;
		void initPins(Pins&);
		void initComms(Settings&);
		void readAll(int8_t);
		void readPin(pin_t);
		void sendPin(pin_t, uint16_t);
		void setPin(pin_t, uint8_t);

	private: // Class Members
		RemoteControl::Command<void, Jack, void> cmd_devreset_{ KeyDevReset, *this, &Jack::devReset };
		RemoteControl::Command<void, Jack, void> cmd_devinfo_{ KeyDevInfo, *this, &Jack::devInfo };
		RemoteControl::Command<void, Jack, void> cmd_devping_{ KeyDevPing, *this, &Jack::devPing };
		RemoteControl::Command<void, Jack, pin_t> cmd_dread_{ KeyDigitalRead, *this, &Jack::readDigital };
		RemoteControl::Command<void, Jack, pin_t> cmd_aread_{ KeyAnalogRead, *this, &Jack::readAnalog };
		RemoteControl::Command<void, Jack, void> cmd_readall_{ KeyReadAllPins, *this, &Jack::readAllPins };
		RemoteControl::Command<void, Jack, pin_t, bool> cmd_dwrite_{ KeyDigitalWrite, *this, &Jack::writeDigital };
		RemoteControl::Command<void, Jack, pin_t, analog_t> cmd_awrite_{ KeyAnalogWrite, *this, &Jack::writeAnalog };
		RemoteControl::Command<void, Jack, pin_t, uint8_t> cmd_setmode_{ KeySetPinMode, *this, &Jack::setPinMode };
		RemoteControl::Command<void, Jack, pin_t> cmd_getmode_{ KeyGetPinMode, *this, &Jack::getPinMode };
		RemoteControl::Command<void, Jack, void> cmd_getpins_{ KeyGetPins, *this, &Jack::getPins };
		RemoteControl::Command<void, Jack, baud_t, frame_t, timeout_t> cmd_setcomms_{ KeySetComms, *this, &Jack::setComms };
		RemoteControl::Command<void, Jack, void> cmd_getcomms_{ KeyGetComms, *this, &Jack::getComms };
		RemoteControl::Command<void, Jack, uint8_t, time_t, const char*> cmd_settimer_{ KeySetTimer, *this, &Jack::setTimer };
		RemoteControl::Command<void, Jack, uint8_t> cmd_gettimer_{ KeyGetTimer, *this, &Jack::getTimer };
		hardware_serial hs_;
		RemoteControl	rc_;
		Pins			pins_;
		commands_cont	commands_;
		timers_cont		timers_;
		EEStream&		eeprom_;
		callback_type	callback_;
	};

	Jack::Jack(hardware_serial hs, EEStream& eeprom, callback_type callback, ilist_type commands) :
		commands_({ &cmd_devreset_, &cmd_devinfo_, &cmd_devping_, &cmd_dread_, &cmd_aread_, &cmd_readall_, &cmd_dwrite_, 
			& cmd_awrite_, & cmd_setmode_, & cmd_getmode_, & cmd_getpins_, & cmd_setcomms_, & cmd_getcomms_,
			& cmd_settimer_, & cmd_gettimer_ }),
		timers_(), hs_(hs), rc_(nullptr, nullptr, hs_.hardware()), eeprom_(eeprom), callback_(callback)
	{
		initPins(pins_); 
		addCommands(commands);
	}

	void Jack::poll()
	{
		rc_.poll();
		for (std::size_t i = 0; i < timers_.size(); ++i)
		{
			if (timers_[i].expired())
			{
				readAll(i);
				timers_[i].reset();
			}
		}
	}

	const typename Jack::commands_cont& Jack::commands() const
	{ 
		return commands_; 
	}

	typename Jack::commands_cont& Jack::commands()
	{ 
		return commands_; 
	}

	hardware_serial& Jack::commsHardware()
	{
		return hs_;
	}

	void Jack::initPins(Pins& pins)
	{
		for (uint8_t i = 0; i < pins.size(); ++i)
		{
			GpioPin& pin = pins[i];

			// Digital pins are in [0, GpioCount - AnalogInCount), 
			// Analog pins are in [GpioCount - AnalogInCount, GpioCount), 
			// digitalPinHasPWM(i) expands to non-zero value if i supports PWM output.

			pin.type_ = digitalPinHasPWM(i)
				? gpio_type::Pwm 
				: i < GpioCount - AnalogInCount 
				? gpio_type::Digital 
				: gpio_type::Analog;
			pin.mode_ = i == LedPinNumber ? gpio_mode::Output : gpio_mode::Input;
			setPin(i, pin.mode_);
			pin.enabled_ = true;
			pin.timer_ = NoTimer;
		}
	}

	void Jack::initComms(Settings& settings)
	{
# if !defined _DEBUG
		hs_.begin(settings.baud(), settings.frame());
		hs_.flush();
		hs_.setTimeout(settings.timeout());
# endif // !defined _DEBUG
	}

	void Jack::initialize()
	{
		Settings settings;

		rc_.commands(std::begin(commands_), std::end(commands_));
		loadSettings(eeprom_, settings);
		initComms(settings);
	}

	void Jack::loadSettings(EEStream& eeprom, Settings& settings)
	{
		unsigned long device_id = 0;

		eeprom.reset();
		eeprom >> device_id;
		if (device_id == DeviceId())
			settings.deserialize(eeprom);
		else
			storeSettings(eeprom, settings);
	}

	void Jack::storeSettings(EEStream& eeprom, const Settings& settings)
	{
		eeprom.reset();
		eeprom << DeviceId();
		settings.serialize(eeprom);
	}

	void Jack::writeDigital(pin_t pin, bool value)
	{
		digitalWrite(pin, value);
	}

	void Jack::writeAnalog(pin_t pin, analog_t value)
	{
		analogWrite(pin, value);
	}

	void Jack::setPinMode(pin_t pin, uint8_t mode)
	{
		setPin(pin, mode);
		pins_[pin].mode_ = static_cast<gpio_mode>(mode);
	}

	void Jack::setPin(pin_t pin, uint8_t mode)
	{
		if (mode == gpio_mode::Disabled)
			mode = gpio_mode::Input;
		pinMode(pin, mode); 
		if (pins_[pin].type_ == gpio_type::Analog)
			(void)analogRead(pin);
	}

	void Jack::setComms(baud_t baud, frame_t frame, timeout_t timeout)
	{
		Settings settings(baud, frame, timeout);

		storeSettings(eeprom_, settings);
		initComms(settings);
		if (callback_)
			(*callback_)();
	}

	void Jack::setTimer(uint8_t n, time_t duration, const char* pins)
	{
		timer_type& timer = timers_[n];
		char* tok = std::strtok((char*)pins, ".");

		timer.stop();
		timer.interval(milliseconds(duration));
		while (tok)
		{
			pin_t pin = std::atoi(tok);

			if(pin <= GpioCount && (pin > 0 || pins[0] == '0'))
				pins_[std::atoi(tok)].timer_ = n;
			tok = std::strtok(nullptr, ".");
		}
		if(duration != 0) 
			timer.start();
	}

	void Jack::getTimer(uint8_t n)
	{
		char buf[64];

		std::sprintf(buf, "%s=%u,%lu", KeyGetTimer, n, timers_[n].interval().count());
		for (std::size_t i = 0; i < pins_.size(); ++i)
		{
			if (pins_[i].timer_ == n)
				std::sprintf(buf + std::strlen(buf), ",%u", i);
		}
		hs_.println(buf);
	}


	void Jack::devReset()
	{
		resetFunc();
	}

	void Jack::readPin(pin_t p)
	{
		GpioPin pin = pins_[p]; 

		switch (pin.type_)
		{
		case gpio_type::Analog:
			readAnalog(p);
			break;
		case gpio_type::Pwm:
		case gpio_type::Digital:
			readDigital(p);
			break;
		default:
			break;
		}
	}

	void Jack::readDigital(pin_t pin)
	{
		sendPin(pin, digitalRead(pin));
	}

	void Jack::readAnalog(pin_t pin)
	{
		sendPin(pin, analogRead(pin));
	}

	void Jack::readAllPins()
	{
		readAll(NoTimer);
	}

	void Jack::readAll(int8_t timer)
	{
		pin_t pin = 0;

		for (auto& i : pins_)
		{
			if ((i.mode_ == gpio_mode::Input || i.mode_ == gpio_mode::Pullup) && i.enabled_ && (timer == NoTimer || i.timer_ == timer))
				readPin(pin);
			++pin;
		}
	}

	void Jack::getPinMode(pin_t pin)
	{
		char buf[16];

		hs_.printFmt(buf, FmtGetPinMode, cmd_getmode_.key(), pin, pins_[pin].type_, pins_[pin].mode_);
	}

	void Jack::getPins()
	{
		for (std::size_t i = 0; i < pins_.size(); ++i)
			getPinMode(i);
	}

	void Jack::getComms()
	{
		char buf[32];
		
		hs_.printFmt(buf, FmtGetComms, cmd_getcomms_.key(), hs_.baud(),
			std::find(hardware_serial::SupportedFrames.begin(), hardware_serial::SupportedFrames.end(), hs_.frame())->string(), hs_.getTimeout());
	}

	void Jack::devInfo()
	{
		char buf[hardware_serial::TxBufferSize];

		hs_.printFmt(buf, FmtDevInfo, cmd_devinfo_.key(), DeviceId(),
			board_traits<board_type>::board, board_traits<board_type>::mcu,
			board_traits<board_type>::clock_frequency);
	}

	void Jack::devPing()
	{
		char buf[16];

		hs_.printFmt(buf, FmtDevPing, cmd_devping_.key(), DeviceId());
	}

	void Jack::addCommands(ilist_type& il)
	{
		if (il.size())
		{
			auto list_end = std::end(commands_);
			auto first = const_cast<command_type**>(il.begin());
			auto last = const_cast<command_type**>(il.begin()) + il.size();

			commands_.resize(commands_.size() + il.size());
			std::copy(first, last, list_end);
		}
	}

	void Jack::sendPin(pin_t pin, uint16_t value)
	{
		char buf[16];

		hs_.printFmt(buf, FmtSendPin, pin, value);
	}

	void Jack::clock()
	{
		poll();
	}
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_JACK_H 
