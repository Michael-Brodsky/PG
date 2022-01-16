/*
 *	This files defines a class that remotely controls gpio pins using a serial  
 *	port.
 *
 *	***************************************************************************
 *
 *	File: Jack.h
 *	Date: January 16, 2022
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
# define __PG_JACK_H 20220116L

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
	using std::chrono::milliseconds;
	using pg::usart::serial;
	using pg::usart::hardware_serial;

	// Facilitates remote control of gpio pins over a serial connection.
	class Jack : public iclockable, public icomponent
	{
	public:
		static const uint8_t GpioCount = NUM_DIGITAL_PINS;					// Total number of gpio pins of any type.
		static const uint8_t AnalogInCount = NUM_ANALOG_INPUTS;				// Total number of gpio pins with analog input capability.
		static const uint8_t LedPinNumber = LED_BUILTIN;					// Built-in LED pin number.
		static const std::size_t CommandsMaxCount = 32;						// Maximum number of storable remote commands.
		static const std::size_t TimersCount = 4;							// Number of event timers.
		static constexpr const unsigned long DeviceId() { return 20211226UL; }	// Unique id used to validate eeprom.

		struct GpioPin;	// Forward decl.
		using baud_t = usart::baud_type;									// Port baud rate storage type. 
		using frame_t = usart::frame_type;									// Port frame storage type.  
		using timeout_t = usart::timeout_type;								// Port timeout storage type.
		using frame_v = usart::frame_map_type;								// Port frame to string mapping type.
		using command_type = RemoteControl::command_type;					// Jack command type.
		using cmdkey_t = typename command_type::key_type;					// Command key storage type.
		using ilist_type = std::initializer_list<command_type*>;			// Commands initializer list type.
		using callback_type = typename callback<void>::type;				// Client callback signature type.
		using timer_type = Timer<milliseconds>;								// Event timer type.
		using Commands = typename std::valarray<command_type*, CommandsMaxCount>;	// Commands collection type (valarray has resize()).
		using Timers = typename std::array<timer_type, TimersCount>;		// Event timers collection type.
		using Pins = std::array<GpioPin, GpioCount>;						// GpioPins collection type.
		template<class... Ts>
		using JackCommand = typename RemoteControl::Command<void, Jack, Ts...>; // Type alias for RemoteControl::Command.

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

		// Aggregates i/o pin info.
		struct GpioPin
		{
			gpio_type	type_;		// Pin i/o type.
			gpio_mode	mode_;		// Pin i/o mode.
			timer_type*	timer_;		// Assigned event timer, if any.
		};

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

#pragma region built-in command key strings

		static constexpr const cmdkey_t KeyDevReset = "rst";
		static constexpr const cmdkey_t KeyDevInfo = "inf";
		static constexpr const cmdkey_t KeyCommsStatus = "com";
		static constexpr const cmdkey_t KeyPinMode = "pin";
		static constexpr const cmdkey_t KeyAnalogRead = "ard";
		static constexpr const cmdkey_t KeyDigitalRead = "drd";
		static constexpr const cmdkey_t KeyReadPin = "rdp";
		static constexpr const cmdkey_t KeyAnalogWrite = "awr";
		static constexpr const cmdkey_t KeyDigitalWrite = "dwr";
		static constexpr const cmdkey_t KeyTimerStatus = "tms";
		static constexpr const cmdkey_t KeyTimerAttach = "atm";

#pragma endregion
#pragma region command reply print format strings

		static constexpr const char* FmtGetPinMode = "%s=%u,%u,%u";
		static constexpr const char* FmtGetComms = "%s=%lu,%s,%lu";
		static constexpr const char* FmtDevInfo = "%s=%lu,%s,%s,%.1f";
		static constexpr const char* FmtSendPin = "%u=%u";

#pragma endregion

	public:
		Jack(hardware_serial&, EEStream&, callback_type = nullptr, ilist_type = ilist_type());

	public:
		void poll();
		const Commands& commands() const;
		Commands& commands();
		hardware_serial& comms();
		void initialize();
		void devReset();
		void devInfo();
		void commsStatus(baud_t, frame_t, timeout_t);
		void commsStatus();
		void gpioMode(pin_t, uint8_t);
		void gpioMode(pin_t);
		void gpioMode();
		void readAnalog(pin_t);
		void readAnalog();
		void readDigital(pin_t);
		void readDigital();
		void readPin(timer_type*);
		void readPin(pin_t);
		void readPin();
		void writeAnalog(pin_t, analog_t);
		void writeDigital(pin_t, bool);
		void timerStatus(uint8_t, time_t);
		void timerStatus(uint8_t);
		void timerStatus();
		void timerAttach(uint8_t, const char*);

	private:
		void clock() override;
		void initCommands(ilist_type&);
		void initComms(Settings&);
		void initPins(Pins&);
		void loadSettings(EEStream&, Settings&);
		void sendPin(pin_t, uint16_t);
		void setPin(pin_t, uint8_t);
		void storeSettings(EEStream&, const Settings&);

	private:
		JackCommand<void> cmd_devreset_{ KeyDevReset, *this, &Jack::devReset };
		JackCommand<void> cmd_devinfo_{ KeyDevInfo, *this, &Jack::devInfo };
		JackCommand<baud_t, frame_t, timeout_t> cmd_setcomms_{ KeyCommsStatus, *this, &Jack::commsStatus };
		JackCommand<void> cmd_getcomms_{ KeyCommsStatus, *this, &Jack::commsStatus };
		JackCommand<pin_t, uint8_t> cmd_setpin_{ KeyPinMode, *this, &Jack::gpioMode };
		JackCommand<pin_t> cmd_getpin_{ KeyPinMode, *this, &Jack::gpioMode };
		JackCommand<void> cmd_getallpins_{ KeyPinMode, *this, &Jack::gpioMode };
		JackCommand<pin_t> cmd_aread_{ KeyAnalogRead, *this, &Jack::readAnalog };
		JackCommand<void> cmd_areadall_{ KeyAnalogRead, *this, &Jack::readAnalog };
		JackCommand<pin_t> cmd_dread_{ KeyDigitalRead, *this, &Jack::readDigital };
		JackCommand<void> cmd_dreadall_{ KeyDigitalRead, *this, &Jack::readDigital };
		JackCommand<pin_t> cmd_readpin_{ KeyReadPin, *this, &Jack::readPin };
		JackCommand<void> cmd_readallpins_{ KeyReadPin, *this, &Jack::readPin };
		JackCommand<pin_t, analog_t> cmd_awrite_{ KeyAnalogWrite, *this, &Jack::writeAnalog };
		JackCommand<pin_t, bool> cmd_dwrite_{ KeyDigitalWrite, *this, &Jack::writeDigital };
		JackCommand<uint8_t, time_t> cmd_settimer_{ KeyTimerStatus, *this, &Jack::timerStatus };
		JackCommand<uint8_t> cmd_gettimer_{ KeyTimerStatus, *this, &Jack::timerStatus };
		JackCommand<void> cmd_getalltimers_{ KeyTimerStatus, *this, &Jack::timerStatus };
		JackCommand<uint8_t, const char*> cmd_attachtimer_{ KeyTimerAttach, *this, &Jack::timerAttach };

		hardware_serial&	hs_;		// Serial port hardware.
		EEStream&			eeprom_;	// EEPROM data stream.
		callback_type		callback_;	// Client callback function.
		Pins				pins_;		// GPIO pins collection.
		Timers				timers_;	// Event timers collection.
		Commands			commands_;	// Remote commands collection.
		RemoteControl		rc_;		// Remote command processor.
	};

	Jack::Jack(hardware_serial& hs, EEStream& eeprom, callback_type callback, ilist_type commands) : 
		hs_(hs), eeprom_(eeprom), callback_(callback), pins_(), timers_(), 
		commands_({ &cmd_aread_, &cmd_areadall_, &cmd_dread_, &cmd_dreadall_, &cmd_readpin_, &cmd_readallpins_,
			&cmd_awrite_, &cmd_dwrite_, &cmd_settimer_, &cmd_gettimer_, &cmd_getalltimers_, &cmd_attachtimer_,
			&cmd_setpin_, &cmd_getpin_, &cmd_getallpins_, &cmd_setcomms_, &cmd_getcomms_, &cmd_devinfo_, &cmd_devreset_ }),
		rc_(nullptr, nullptr, hs_.hardware())
	{
		initPins(pins_);
		initCommands(commands);
	}

#pragma region public methods

	void Jack::poll()
	{
		// Check for new commands received by serial port.
		rc_.poll();
		// Check for any expired timers.
		for (std::size_t i = 0; i < timers_.size(); ++i)
		{
			if (timers_[i].expired())
			{
				readPin(&timers_[i]);	// Read pins attached to expired timer.
				timers_[i].reset();
			}
		}
	}

	const typename Jack::Commands& Jack::commands() const
	{
		return commands_;
	}

	typename Jack::Commands& Jack::commands()
	{
		return commands_;
	}

	hardware_serial& Jack::comms()
	{
		return hs_;
	}

	void Jack::initialize()
	{
		Settings settings;

		rc_.commands(std::begin(commands_), std::end(commands_));
		loadSettings(eeprom_, settings);
		initComms(settings);
	}

	void Jack::devReset()
	{
		resetFunc();
	}

	void Jack::devInfo()
	{
		char buf[64];

		hs_.printFmt(buf, FmtDevInfo, cmd_devinfo_.key(), DeviceId(),
			board_traits<board_type>::board, board_traits<board_type>::mcu,
			board_traits<board_type>::clock_frequency);
	}

	void Jack::commsStatus(baud_t baud, frame_t frame, timeout_t timeout)
	{
		Settings settings(baud, frame, timeout);

		storeSettings(eeprom_, settings);
		initComms(settings);
		if (callback_)
			(*callback_)();
	}

	void Jack::commsStatus()
	{
		char buf[32];

		hs_.printFmt(buf, FmtGetComms, cmd_getcomms_.key(), hs_.baud(),
			std::find(hs_.SupportedFrames.begin(), hs_.SupportedFrames.end(), hs_.frame())->string(),
			hs_.getTimeout());
	}

	void Jack::gpioMode(pin_t pin, uint8_t mode)
	{
		if (pin < GpioCount)
		{
			setPin(pin, mode);
			pins_[pin].mode_ = static_cast<gpio_mode>(mode);
		}
	}

	void Jack::gpioMode(pin_t pin)
	{
		if (pin < GpioCount)
		{
			char buf[16];

			hs_.printFmt(buf, FmtGetPinMode, cmd_getpin_.key(), pin, pins_[pin].type_, pins_[pin].mode_);
		}
	}

	void Jack::gpioMode()
	{
		for (std::size_t i = 0; i < pins_.size(); ++i)
			gpioMode(i);
	}

	void Jack::readAnalog(pin_t pin)
	{
		sendPin(pin, analogRead(pin));
	}

	void Jack::readAnalog()
	{
		for (std::size_t i = 0; i < pins_.size(); ++i)
			if (pins_[i].type_ == gpio_type::Analog)
				readAnalog(i);
	}

	void Jack::readDigital(pin_t pin)
	{
		sendPin(pin, digitalRead(pin));
	}

	void Jack::readDigital()
	{
		for (std::size_t i = 0; i < pins_.size(); ++i)
			if (pins_[i].type_ == gpio_type::Digital)
				readDigital(i);
	}

	void Jack::readPin(timer_type* timer)
	{
		assert(timer);

		for (std::size_t i = 0; i < pins_.size(); ++i)
			if (pins_[i].timer_ == timer && pins_[i].mode_ != gpio_mode::Disabled)
				readPin(i);
	}

	void Jack::readPin(pin_t pin)
	{
		GpioPin& gpio = pins_[pin];

		switch (gpio.type_)
		{
		case gpio_type::Analog:
			readAnalog(pin);
			break;
		case gpio_type::Pwm:
		case gpio_type::Digital:
			readDigital(pin);
			break;
		default:
			break;
		}
	}

	void Jack::readPin()
	{
		for (std::size_t i = 0; i < pins_.size(); ++i)
			readPin(i);
	}

	void Jack::writeAnalog(pin_t pin, analog_t value)
	{
		analogWrite(pin, value);
	}

	void Jack::writeDigital(pin_t pin, bool value)
	{
		digitalWrite(pin, value);
	}

	void Jack::timerStatus(uint8_t tid, time_t interval)
	{
		timer_type* timer = tid < TimersCount ? &timers_[tid] : nullptr;

		if (timer)
		{
			timer->stop();
			timer->interval(milliseconds(interval));
			if (interval != 0)
				timer->start();
		}
	}

	void Jack::timerStatus(uint8_t tid)
	{
		timer_type* timer = tid < TimersCount ? &timers_[tid] : nullptr;

		if (timer)
		{
			char buf[64];

			std::sprintf(buf, "%s=%u,%lu", KeyTimerStatus, tid, timer->interval().count());
			for (std::size_t i = 0; i < pins_.size(); ++i)
				if (pins_[i].timer_ == timer)
					std::sprintf(buf + std::strlen(buf), ",%u", i);
			hs_.println(buf);
		}
	}

	void Jack::timerStatus()
	{
		for (std::size_t i = 0; i < timers_.size(); ++i)
			timerStatus(i);
	}

	void Jack::timerAttach(uint8_t tid, const char* pins)
	{
		timer_type* timer = tid < TimersCount ? &timers_[tid] : nullptr;

		if (timer)
		{
			char* tok = std::strtok((char*)pins, ".");

			// Detach all attached pins from the timer.
			for (auto& i : pins_)
				if (i.timer_ == timer)
					i.timer_ = nullptr;
			// Attach all pins in list to the timer.
			while (tok)
			{
				pin_t pin = std::atoi(tok);

				if (pin <= GpioCount && (pin > 0 || pins[0] == '0'))
					pins_[std::atoi(tok)].timer_ = timer;
				tok = std::strtok(nullptr, ".");
			}
		}
	}

#pragma endregion
#pragma region private methods

	void Jack::clock()
	{
		poll();
	}

	void Jack::initCommands(ilist_type& il)
	{
		if (il.size())
		{
			auto list_end = std::end(commands_);
			auto first = const_cast<command_type**>(il.begin());
			auto last = const_cast<command_type**>(il.begin()) + il.size();

			// Append commamnds in list to the current collection.
			commands_.resize(commands_.size() + il.size());
			std::copy(first, last, list_end);
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
			pin.timer_ = nullptr;
		}
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

	void Jack::sendPin(pin_t pin, uint16_t value)
	{
		char buf[16];

		hs_.printFmt(buf, FmtSendPin, pin, value);
	}

	void Jack::setPin(pin_t pin, uint8_t mode)
	{
		if (mode == gpio_mode::Disabled)
			mode = gpio_mode::Input;
		pinMode(pin, mode);
		if (pins_[pin].type_ == gpio_type::Analog)
			(void)analogRead(pin);
	}

	void Jack::storeSettings(EEStream& eeprom, const Settings& settings)
	{
		eeprom.reset();
		eeprom << DeviceId();
		settings.serialize(eeprom);
	}

#pragma endregion
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_JACK_H 

