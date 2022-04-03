/*
 *	This files defines a class that remotely controls and queries gpio pins.
 *
 *	***************************************************************************
 *
 *	File: Jack.h
 *	Date: March 31, 2022
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
# define __PG_JACK_H 20220331L

# include <Arduino.h>
# include <cstdio>
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

	// Facilitates remote control of gpio pins data acquisition.
	class Jack : public iclockable, public icomponent
	{
	public:
		static const uint8_t GpioCount = NUM_DIGITAL_PINS;					// Total number of gpio pins of any type.
		static const uint8_t AnalogInCount = NUM_ANALOG_INPUTS;				// Total number of gpio pins with analog input capability.
		static const uint8_t LedPinNumber = LED_BUILTIN;					// Built-in LED pin number.
		static const std::size_t CommandsMaxCount = 32;						// Maximum number of storable remote commands.
		static const std::size_t TimersCount = 4;							// Number of event timers.
		static const int8_t NoTimer = -1;									// EEPROM storable constant indicating pin has no timer.
		static constexpr const unsigned long DeviceId() { return 20211226UL; }	// Unique id used to validate eeprom contents.

		struct GpioPin;	// Forward decl.
		using command_type = RemoteControl::command_type;					// Jack command type.
		using cmdkey_t = typename command_type::key_type;					// Command key storage type.
		using ilist_type = std::initializer_list<command_type*>;			// Commands initializer list type.
		using callback_type = typename callback<void>::type;				// Client callback signature type.
		using timer_type = Timer<milliseconds>;								// Event timer type.
		using address_t = EEStream::address_type;							// EEPROM addressing tpe.
		using Commands = typename std::valarray<command_type*, CommandsMaxCount>;	// Commands collection type (valarray has resize()).
		using Timers = typename std::array<timer_type, TimersCount>;		// Event timers collection type.
		using Pins = std::array<GpioPin, GpioCount>;						// GpioPins collection type.
		template<class... Ts>
		using JackCommand = typename RemoteControl::Command<void, Jack, Ts...>; // Type alias for RemoteControl::Command.

		// Enumerates valid i/o pin types.
		enum gpio_type : uint8_t
		{
			Digital = 0,	// Digital i/o								
			Analog = 1,		// Analog in
			Pwm = 2 		// Pwm out
		};

		// Enumerates valid pin i/o modes.
		enum gpio_mode : uint8_t
		{
			Input = 0,		// Input pin.
			Output = 1,		// Output pin.
			Pullup = 2,		// Input pin w/ internal pullup resistor.
			PwmOut = 3,		// Pwm output pin.
			Disabled = 4	// Pin is disabled.
		};

		// Aggregates i/o pin info.
		struct GpioPin
		{
			gpio_type	type_;		// Pin i/o type.
			gpio_mode	mode_;		// Pin i/o mode.
			timer_type*	timer_;		// Attached event timer, if any.
		};

#pragma region built-in command key strings

		static constexpr const cmdkey_t KeyDevReset = "rst";					// Device reset.
		static constexpr const cmdkey_t KeyDevInfo = "inf";						// Device info.
		static constexpr const cmdkey_t KeyAcknowledge = "ack";					// Device acknowledge.
		static constexpr const cmdkey_t KeyPinMode = "pin";						// Pin type and mode.
		static constexpr const cmdkey_t KeyAnalogRead = "ard";					// Analog read pin.
		static constexpr const cmdkey_t KeyDigitalRead = "drd";					// Digital read pin.
		static constexpr const cmdkey_t KeyReadPin = "rdp";						// Read pin (depends on type).
		static constexpr const cmdkey_t KeyAnalogWrite = "awr";					// Analog write pin.
		static constexpr const cmdkey_t KeyDigitalWrite = "dwr";				// Digital write pin.
		static constexpr const cmdkey_t KeyWritePin = "wrp";					// Write pin (depends on type).
		static constexpr const cmdkey_t KeyTimerStatus = "tms";					// Timer interval and attached pins.
		static constexpr const cmdkey_t KeyTimerAttach = "atm";					// Timer attach pin.
		static constexpr const cmdkey_t KeyLoadConfig = "lda";					// Load device configuration from eeprom.
		static constexpr const cmdkey_t KeyStoreConfig = "sto";					// Store device configuration to eeprom.

#pragma endregion
#pragma region command reply print format strings

		static constexpr const char* FmtGetPinMode = "%s=%u,%u,%u";				// pin=p#,type,mode
		static constexpr const char* FmtDevInfo = "%s=%lu,%s,%s,%.1f,%u,%u";	// inf=id,type,mcu,clkspd,#pins,#timers
		static constexpr const char* FmtSendPin = "%u=%u";						// p#=value
		static constexpr const char* FmtAcknowledge = "%s=%u";					// ack=0/1
		static constexpr const char* FmtTimerStatus = "%s=%u,%lu";				// tmr=t#,intvl,[p0],...,[pn]    

#pragma endregion

	public:
		Jack(EEStream&, callback_type = nullptr, ilist_type = ilist_type());
		Jack(Connection*, EEStream&, callback_type = nullptr, ilist_type = ilist_type());

	public:
		void poll();
		const Commands& commands() const;
		Commands& commands();
		void connection(Connection*);
		const Connection* connection() const;
		void initialize();
		void ack(bool); 
		void ack();
		void configLoad();
		void configStore();
		void devReset();
		void devInfo();
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
		void writePin(pin_t, analog_t);
		void timerStatus(uint8_t, time_t);
		void timerStatus(uint8_t);
		void timerStatus();
		void timerAttach(uint8_t, const char*);
		void timerAttach(uint8_t);

	private:
		void timerDetach(timer_type*);
		timer_type* getTimer(uint8_t);
		void clock() override;
		void initCommands(ilist_type&);
		void initPins(Pins&);
		void loadConfig(EEStream&, const address_t);
		void sendPin(pin_t, uint16_t);
		void setPin(pin_t, uint8_t);
		void storeConfig(EEStream&, const address_t);
		template<class... Args>
		void reply(char*, const char*, Args...);

	private:
		// Jack built-in command objects.

		JackCommand<void> cmd_devreset_{ KeyDevReset, *this, &Jack::devReset };
		JackCommand<void> cmd_devinfo_{ KeyDevInfo, *this, &Jack::devInfo };
		JackCommand<bool> cmd_setack_{ KeyAcknowledge, *this, &Jack::ack };
		JackCommand<void> cmd_getack_{ KeyAcknowledge, *this, &Jack::ack };
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
		JackCommand<pin_t, analog_t> cmd_writepin_{ KeyWritePin, *this, &Jack::writePin };
		JackCommand<uint8_t, time_t> cmd_settimer_{ KeyTimerStatus, *this, &Jack::timerStatus };
		JackCommand<uint8_t> cmd_gettimer_{ KeyTimerStatus, *this, &Jack::timerStatus };
		JackCommand<void> cmd_getalltimers_{ KeyTimerStatus, *this, &Jack::timerStatus };
		JackCommand<uint8_t, const char*> cmd_attachtimer_{ KeyTimerAttach, *this, &Jack::timerAttach };
		JackCommand<uint8_t> cmd_detachtimer_{ KeyTimerAttach, *this, &Jack::timerAttach };
		JackCommand<void> cmd_ldaconfig_{ KeyLoadConfig, *this, &Jack::configLoad };
		JackCommand<void> cmd_stoconfig_{ KeyStoreConfig, *this, &Jack::configStore };

		Connection*			connection_;// Communications connection.
		EEStream&			eeprom_;	// EEPROM data stream.
		callback_type		callback_;	// Client callback function.
		Pins				pins_;		// GPIO pins collection.
		Timers				timers_;	// Event timers collection.
		bool				ack_;		// Flag indicating whether to acknowledge a "write" command.
		Commands			commands_;	// Remote commands collection.
		RemoteControl		rc_;		// Remote command processor.
		address_t			addr_cfg_;	// EEPROM address of device configuration.
	};

	Jack::Jack(EEStream& eeprom, callback_type callback, ilist_type commands) :
		connection_(), eeprom_(eeprom), callback_(callback), pins_(), timers_(), ack_(), rc_(), addr_cfg_(eeprom_.address()), 
		commands_({ &cmd_aread_, &cmd_areadall_, &cmd_dread_, &cmd_dreadall_, &cmd_readpin_, &cmd_readallpins_,
			&cmd_awrite_, &cmd_dwrite_, &cmd_writepin_, &cmd_settimer_, &cmd_gettimer_, &cmd_getalltimers_,
			&cmd_attachtimer_, &cmd_setpin_, &cmd_getpin_, &cmd_getallpins_, &cmd_detachtimer_, 
			&cmd_setack_, &cmd_getack_, &cmd_devinfo_, &cmd_devreset_, &cmd_ldaconfig_, &cmd_stoconfig_ })
		
	{
		initPins(pins_);
		initCommands(commands);
	}

	Jack::Jack(Connection* connection, EEStream& eeprom, callback_type callback, ilist_type commands) : 
		connection_(), eeprom_(eeprom), callback_(callback), pins_(), timers_(), ack_(), rc_(), addr_cfg_(eeprom_.address()),
		commands_({ &cmd_aread_, &cmd_areadall_, &cmd_dread_, &cmd_dreadall_, &cmd_readpin_, &cmd_readallpins_,
			&cmd_awrite_, &cmd_dwrite_, &cmd_writepin_, &cmd_settimer_, &cmd_gettimer_, &cmd_getalltimers_,
			&cmd_attachtimer_, &cmd_setpin_, &cmd_getpin_, &cmd_getallpins_, &cmd_detachtimer_,
			&cmd_setack_, &cmd_getack_, &cmd_devinfo_, &cmd_devreset_, &cmd_ldaconfig_, &cmd_stoconfig_ })
	{
		initPins(pins_);
		initCommands(commands);
	}

#pragma region public methods

	void Jack::poll()
	{
		// Check for new messages.
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

	void Jack::connection(Connection* cn)
	{
		connection_ = cn;
	}

	const Connection* Jack::connection() const 
	{
		return connection_;
	}

	void Jack::initialize()
	{
		rc_.connection(connection_);
		rc_.commands(std::begin(commands_), std::end(commands_));
		if (callback_)
			(*callback_)();
	}

	void Jack::ack(bool value)
	{
		bool old_value = ack_;

		if((ack_ = value) != old_value)
			ack();
	}

	void Jack::ack() 
	{
		char buf[8];

		reply(buf, FmtAcknowledge, KeyAcknowledge, ack_);
	}

	void Jack::configLoad()
	{
		loadConfig(eeprom_, addr_cfg_);
	}

	void Jack::configStore()
	{
		storeConfig(eeprom_, addr_cfg_);
	}

	void Jack::devReset()
	{
		if (ack_) 
		{
			connection_->send(KeyDevReset);
			delay(50);	// Wait for write buf to empty.
		}
		resetFunc();
	}

	void Jack::devInfo()
	{
		char buf[64];

		reply(buf, FmtDevInfo, cmd_devinfo_.key(), DeviceId(),
			board_traits<board_type>::board, board_traits<board_type>::mcu,
			board_traits<board_type>::clock_frequency / 1000000, (unsigned)pins_.size(), (unsigned)timers_.size());
	}

	void Jack::gpioMode(pin_t pin, uint8_t mode)
	{
		if (pin < GpioCount && mode <= gpio_mode::Disabled)
		{
			setPin(pin, mode);
			pins_[pin].mode_ = static_cast<gpio_mode>(mode);
			if (ack_)
				gpioMode(pin);
		}
	}

	void Jack::gpioMode(pin_t pin)
	{
		if (pin < GpioCount)
		{
			char buf[16];

			reply(buf, FmtGetPinMode, cmd_getpin_.key(), pin, pins_[pin].type_, pins_[pin].mode_);
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

	void Jack::writePin(pin_t pin, analog_t value)
	{
		GpioPin& gpio = pins_[pin];

		switch (gpio.type_)
		{
		case gpio_type::Pwm:
			if(gpio.mode_ == gpio_mode::PwmOut)
				writeAnalog(pin, value);
			else
				writeDigital(pin, value);
			break;
		case gpio_type::Analog:	// Analog pins are digital out only.
		case gpio_type::Digital:
			writeDigital(pin, value);
			break;
		default:
			break;
		}
	}

	Jack::timer_type* Jack::getTimer(uint8_t tid)
	{
		return tid < TimersCount ? &timers_[tid] : nullptr;
	}

	void Jack::timerStatus(uint8_t tid, time_t interval)
	{
		timer_type* timer = getTimer(tid);

		if (timer)
		{
			timer->stop();
			timer->interval(milliseconds(interval));
			if (ack_)
				timerStatus(tid);
			if (interval != 0)
				timer->start();
		}
	}

	void Jack::timerStatus(uint8_t tid)
	{
		timer_type* timer = getTimer(tid);

		if (timer)
		{
			char buf[64];

			std::sprintf(buf, FmtTimerStatus, KeyTimerStatus, tid, timer->interval().count());
			for (std::size_t i = 0; i < pins_.size(); ++i)
				if (pins_[i].timer_ == timer)
					std::sprintf(buf + std::strlen(buf), ",%u", i);
			connection_->send(buf);
		}
	}

	void Jack::timerStatus()
	{
		for (std::size_t i = 0; i < timers_.size(); ++i)
			timerStatus(i);
	}

	void Jack::timerDetach(timer_type* timer)
	{
		for (auto& i : pins_)
			if (i.timer_ == timer)
				i.timer_ = nullptr;
	}

	void Jack::timerAttach(uint8_t tid)
	{
		timer_type* timer = getTimer(tid);

		if (timer)
			timerDetach(timer);
	}

	void Jack::timerAttach(uint8_t tid, const char* pins)
	{
		timer_type* timer = getTimer(tid);

		if (timer)
		{
			char* tok = std::strtok((char*)pins, ".");

			// Detach all attached pins from the timer.
			timerDetach(timer);
			// Attach all pins in list to the timer.
			while (tok)
			{
				pin_t pin = std::atoi(tok);

				if (pin <= GpioCount && (pin > 0 || pins[0] == '0'))
					pins_[std::atoi(tok)].timer_ = timer;
				tok = std::strtok(nullptr, ".");
			}
			if (ack_)
				timerStatus(tid);
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

	void Jack::initPins(Pins& pins)
	{
		for (uint8_t i = 0; i < pins.size(); ++i)
		{
			GpioPin& pin = pins[i];

			// Digital pins are in [0, GpioCount - AnalogInCount), 
			// Analog pins are in [GpioCount - AnalogInCount, GpioCount), 
			// digitalPinHasPWM(i) expands to non-zero value if pin i supports PWM output.

			pin.type_ = digitalPinHasPWM(i)
				? gpio_type::Pwm
				: i < GpioCount - AnalogInCount
				? gpio_type::Digital
				: analogInputToDigitalPin(GpioCount - i - 1) == -1
				? gpio_type::Digital
				: gpio_type::Analog;
			pin.mode_ = i == LedPinNumber ? gpio_mode::Output : gpio_mode::Input;
			setPin(i, pin.mode_);
			pin.timer_ = nullptr;
		}
	}

	void Jack::loadConfig(EEStream& eeprom, const address_t addr)
	{
		eeprom.address() = addr;
		for (std::size_t i = 0; i < pins_.size(); ++i)
		{
			int8_t timer_id = NoTimer;
			uint8_t pin_mode = 0;

			eeprom >> pin_mode;
			eeprom >> timer_id;
			gpioMode(i, pin_mode);
			if (timer_id != NoTimer)
				pins_[i].timer_ = &timers_[timer_id];
		}
		for (std::size_t i = 0; i < timers_.size(); ++i)
		{
			time_t intvl = 0;

			eeprom >> intvl;
			timerStatus(i, intvl);
		}
	}

	void Jack::sendPin(pin_t pin, uint16_t value)
	{
		char buf[16];

		reply(buf, FmtSendPin, pin, value);
	}

	void Jack::setPin(pin_t pin, uint8_t mode)
	{
		if (mode == gpio_mode::Disabled)
			mode = gpio_mode::Input;
		pinMode(pin, mode);
		if (pins_[pin].type_ == gpio_type::Analog)
			(void)analogRead(pin);
	}

	void Jack::storeConfig(EEStream& eeprom, const address_t addr)
	{
		eeprom.address() = addr;
		for (auto i : pins_)
		{
			int8_t timer_id = NoTimer;

			eeprom << i.mode_;
			if (i.timer_)
			{
				// Timers need an id field so we dont have to loop through the collection.
				for (timer_id = 0; timer_id < TimersCount; ++timer_id)
					if (i.timer_ == &timers_[timer_id])
						break;
			}
			eeprom << timer_id;
		}
		for (auto i : timers_)
			eeprom << i.interval().count();
	}

	template<class... Args>
	void Jack::reply(char* buf, const char* fmt, Args... args)
	{
		std::sprintf(buf, fmt, args...);
		connection_->send(buf);
	}

#pragma endregion
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_JACK_H 

