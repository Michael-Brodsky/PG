/*
 *	This files defines a class that remotely controls and queries gpio pins.
 *
 *	***************************************************************************
 *
 *	File: Jack.h
 *	Date: April 19, 2022
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
# define __PG_JACK_H 20220419L

# include "cstdio"
# include "cstring"
# include "valarray"
# include "system/boards.h"
# include "system/utils.h"
# include "utilities/EEStream.h"
# include "utilities/Timer.h"
# include "components/RemoteControl.h"

# if defined __PG_HAS_NAMESPACES  

namespace pg
{
	using std::chrono::milliseconds;

	// Facilitates remote control of gpio pins and data acquisition.
	class Jack : public iclockable, public icomponent
	{
	public:
		//
		// These function-like macros need to be replaced w/ constexpr funcs.
		//
#  if defined analogInputToDigitalPin
#   define getAnalogPins analogInputToDigitalPin
#  elif defined digitalPinToAnalogInput
#   define getAnalogPins digitalPinToAnalogInput
#  endif
#  if defined digitalPinToInterrupt
#   define getInterruptPins digitalPinToInterrupt
#  else
#   define getInterruptPins(p) (p)
#  endif
		static const uint8_t GpioCount = NUM_DIGITAL_PINS;					// Total number of gpio pins of any type.
		static const uint8_t AnalogInCount = NUM_ANALOG_INPUTS;				// Total number of gpio pins with analog input capability.
		static const uint8_t LedPinNumber = LED_BUILTIN;					// Built-in LED pin number.
		static const std::size_t CommandsMaxCount = 32;						// Maximum number of storable remote commands.
		static const std::size_t TimersCount = 4;							// Number of event counters/timers.
		static constexpr const unsigned long DeviceId() { return 20211226UL; }	// Unique id used to validate eeprom contents.

		struct GpioPin;			// Forward decl.
		struct CounterTimer;	// Forward decl.
		using command_type = RemoteControl::command_type;					// Jack command type.
		using cmdkey_t = typename command_type::key_type;					// Command key storage type.
		using ilist_type = std::initializer_list<command_type*>;			// Commands initializer list type.
		using callback_type = typename callback<void>::type;				// Client callback signature type.
		using timer_type = Timer<milliseconds>;								// Event timer type.
		using counter_type = Counter<uint16_t>;								// Event counter type.
		using address_t = EEStream::address_type;							// EEPROM addressing type.
		using Commands = typename std::valarray<command_type*, CommandsMaxCount>;	// Commands collection type (valarray has resize()).
		using CounterTimers = typename std::array<CounterTimer, TimersCount>;		// Event counters/timers collection type.
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
			Pullup = 2,		// Input pin w/ internal pullup resistor active.
			PwmOut = 3,		// Pwm output pin.
			Disabled = 4	// Pin is disabled.
		};

		// Aggregates info about event timers/counters.
		struct CounterTimer
		{
			// Enumerates valid CounterTimer operating modes.
			enum Mode
			{
				Counter = 0,	// Counter mode.
				Timer = 1		// Timer mode.
			};

			// Enumerates valid CounterTimer actions.
			enum Action
			{
				Start = 0,		// Starts the CounterTimer.
				Stop = 1,		// Stops the CounterTimer.
				Resume = 2,		// Resumes the CounterTimer with the current count/elapsed time.
				Reset = 3		// Resets the current count/elapsed time.
			};

			uint8_t			idx_;		// The counter/timer's index in a collection.
			pin_t			pin_;		// The currently attached pin.
			uint8_t			mode_;		// The current operating mode.
			uint8_t			trigger_;	// The current event trigger.
			timer_type		timer_;		// Timer object.
			counter_type	counter_;	// Counter object.
		};

		// Aggregates i/o pin info.
		struct GpioPin
		{
			gpio_type		type_;		// Pin i/o type.
			gpio_mode		mode_;		// Pin i/o mode.
			bool			int_;		// Pin is interrupt.
			CounterTimer*	timer_;		// Attached event counter/timer, if any.

			bool isInput() const { return mode_ == gpio_mode::Input || mode_ == gpio_mode::Pullup; }
			bool isOutput() const { return mode_ == gpio_mode::Output || mode_ == gpio_mode::PwmOut; }
			bool isDigital() const { return type_ != gpio_type::Analog; }
		};


#pragma region built-in command key strings

		static constexpr const cmdkey_t KeyDevReset = "rst";		// Device reset
		static constexpr const cmdkey_t KeyDevInfo = "inf";			// Device info
		static constexpr const cmdkey_t KeyAcknowledge = "ack";		// Write acknowledge
		static constexpr const cmdkey_t KeyPinInfo = "pin";			// Pin info (all)
		static constexpr const cmdkey_t KeyPinMode = "pmd";			// Pin mode only.
		static constexpr const cmdkey_t KeyAnalogRead = "ard";		// Analog read pin.
		static constexpr const cmdkey_t KeyDigitalRead = "drd";		// Digital read pin.
		static constexpr const cmdkey_t KeyReadPin = "rdp";			// Read pin (type dependent).
		static constexpr const cmdkey_t KeyAnalogWrite = "awr";		// Analog write pin.
		static constexpr const cmdkey_t KeyDigitalWrite = "dwr";	// Digital write pin.
		static constexpr const cmdkey_t KeyWritePin = "wrp";		// Write pin (type dependent).
		static constexpr const cmdkey_t KeyTcStatus = "tcs";		// Set the timer/counter state.
		static constexpr const cmdkey_t KeyTcAttach = "atc";		// Attach timer/counter to pin.
		static constexpr const cmdkey_t KeyLoadConfig = "lda";		// Load device configuration from eeprom.
		static constexpr const cmdkey_t KeyStoreConfig = "sto";		// Store device configuration to eeprom.

#pragma endregion
#pragma region command reply print format strings

		static constexpr const char* FmtDevInfo = "%s=%lu,%s,%s,%u,%u,%u";	// inf=id,type,mcutype,mcuspd,#pins,#timers
		static constexpr const char* FmtPinInfo = "%s=%u,%u,%u,%u";			// pin=p#,type,mode,int
		static constexpr const char* FmtPinMode = "%s=%u,%u";				// pmd=p#,mode
		static constexpr const char* FmtReadPin = "%u=%u";					// p#=value
		static constexpr const char* FmtAcknowledge = "%s=%u";				// ack=0||1
		static constexpr const char* FmtTimerInfo = "%s=%u,%u,%u,%u,%lu";	// tmr=t#,p#,mode,active,value    

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
		void gpioInfo(pin_t);
		void gpioInfo();
		const Pins& pins() const;
		Pins& pins();
		const CounterTimers& timers() const;
		CounterTimers& timers();
		void readAnalog(pin_t);
		void readAnalog();
		void readDigital(pin_t);
		void readDigital();
		void readPin(pin_t);
		void readPin();
		void writeAnalog(pin_t, analog_t);
		void writeDigital(pin_t, bool);
		void writePin(pin_t, analog_t);
		void tcAttach(uint8_t, pin_t, uint8_t, uint8_t);
		void tcAttach(uint8_t, pin_t);
		void tcAttach(uint8_t);
		void tcStatus(uint8_t, uint8_t);

	private:
		void clock() override;
		void initCommands(ilist_type&);
		void initPins(Pins&);
		void initTimers(CounterTimers&);
		void loadConfig(EEStream&, const address_t);
		void sendPinInfo(pin_t);
		void sendPinMode(pin_t);
		void sendReadPin(pin_t, uint16_t);
		void sendReset();
		void sendTimerInfo(uint8_t);
		void setPin(pin_t, uint8_t);
		void storeConfig(EEStream&, const address_t);
		void startTimer(CounterTimer&);
		void stopTimer(CounterTimer&);
		template<class... Args>
		void reply(char*, const char*, Args...);

	private:
		// Jack built-in command objects.

		JackCommand<void> cmd_devreset_{ KeyDevReset, *this, &Jack::devReset };
		JackCommand<void> cmd_devinfo_{ KeyDevInfo, *this, &Jack::devInfo };
		JackCommand<bool> cmd_setack_{ KeyAcknowledge, *this, &Jack::ack };
		JackCommand<void> cmd_getack_{ KeyAcknowledge, *this, &Jack::ack };
		JackCommand<pin_t, uint8_t> cmd_setpinmode_{ KeyPinMode, *this, &Jack::gpioMode };
		JackCommand<pin_t> cmd_getpinmode_{ KeyPinMode, *this, &Jack::gpioMode };
		JackCommand<pin_t> cmd_pininfo_{ KeyPinInfo, *this, &Jack::gpioInfo };
		JackCommand<void> cmd_pininfoall_{ KeyPinInfo, *this, &Jack::gpioInfo };
		JackCommand<pin_t> cmd_aread_{ KeyAnalogRead, *this, &Jack::readAnalog };
		JackCommand<void> cmd_areadall_{ KeyAnalogRead, *this, &Jack::readAnalog };
		JackCommand<pin_t> cmd_dread_{ KeyDigitalRead, *this, &Jack::readDigital };
		JackCommand<void> cmd_dreadall_{ KeyDigitalRead, *this, &Jack::readDigital };
		JackCommand<pin_t> cmd_readpin_{ KeyReadPin, *this, &Jack::readPin };
		JackCommand<void> cmd_readallpins_{ KeyReadPin, *this, &Jack::readPin };
		JackCommand<pin_t, analog_t> cmd_awrite_{ KeyAnalogWrite, *this, &Jack::writeAnalog };
		JackCommand<pin_t, bool> cmd_dwrite_{ KeyDigitalWrite, *this, &Jack::writeDigital };
		JackCommand<pin_t, analog_t> cmd_writepin_{ KeyWritePin, *this, &Jack::writePin };
		JackCommand<uint8_t, uint8_t> cmd_tcstatusset_{ KeyTcStatus, *this, &Jack::tcStatus };
		JackCommand<void> cmd_ldaconfig_{ KeyLoadConfig, *this, &Jack::configLoad };
		JackCommand<void> cmd_stoconfig_{ KeyStoreConfig, *this, &Jack::configStore };

		Connection*			connection_;// Communications connection.
		EEStream&			eeprom_;	// EEPROM data stream.
		callback_type		callback_;	// Client callback function.
		Pins				pins_;		// GPIO pins collection.
		CounterTimers		timers_;	// Event counter/timers collection.
		Commands			commands_;	// Remote commands collection.
		RemoteControl		rc_;		// Remote command processor.
		address_t			addr_cfg_;	// EEPROM address of device configuration.
		bool				ack_;		// Write acknowledge flag.
	};

	Jack::Jack(EEStream& eeprom, callback_type callback, ilist_type commands) :
		connection_(), eeprom_(eeprom), callback_(callback), pins_(), timers_(), ack_(), rc_(), addr_cfg_(eeprom_.address()), 
		commands_({ &cmd_readpin_, &cmd_writepin_, &cmd_setpinmode_, &cmd_getpinmode_, &cmd_pininfo_, &cmd_pininfoall_, 
			&cmd_setack_, &cmd_getack_, &cmd_devinfo_, &cmd_devreset_, &cmd_ldaconfig_, &cmd_stoconfig_, 
			&cmd_tcstatusset_, &cmd_readallpins_, &cmd_aread_, &cmd_areadall_, 
			&cmd_dread_, &cmd_dreadall_, &cmd_awrite_, &cmd_dwrite_ })
		
	{
		initPins(pins_);
		initTimers(timers_);
		initCommands(commands);
	}

	Jack::Jack(Connection* connection, EEStream& eeprom, callback_type callback, ilist_type commands) : 
		connection_(), eeprom_(eeprom), callback_(callback), pins_(), timers_(), ack_(), rc_(), addr_cfg_(eeprom_.address()),
		commands_({ &cmd_readpin_, &cmd_writepin_, &cmd_setpinmode_, &cmd_getpinmode_, &cmd_pininfo_, &cmd_pininfoall_,
			&cmd_setack_, &cmd_getack_, &cmd_devinfo_, &cmd_devreset_, &cmd_ldaconfig_, &cmd_stoconfig_,
			&cmd_tcstatusset_, &cmd_readallpins_, &cmd_aread_, &cmd_areadall_,
			&cmd_dread_, &cmd_dreadall_, &cmd_awrite_, &cmd_dwrite_ })
	{
		initPins(pins_);
		initTimers(timers_);
		initCommands(commands);
	}

#pragma region public methods

	void Jack::poll()
	{
		rc_.poll();
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
		if ((ack_ = value))
			ack();
	}

	void Jack::ack()
	{
		char buf[6];

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
		sendReset();	// Send reset ack to host.
		delay(50);		// Wait for write buf to empty.
		resetFunc();
	}

	void Jack::devInfo()
	{
		char buf[64];

		reply(buf, FmtDevInfo, cmd_devinfo_.key(), DeviceId(),
			board_traits<board_type>::board, board_traits<board_type>::mcu,
			static_cast<unsigned>(board_traits<board_type>::clock_frequency / 1000000), 
			(unsigned)pins_.size(), (unsigned)timers_.size());
	}

	void Jack::gpioMode(pin_t pin, uint8_t mode)
	{
		if (pin < GpioCount && mode <= gpio_mode::Disabled)
		{
			setPin(pin, mode);
			pins_[pin].mode_ = static_cast<gpio_mode>(mode);
			if(ack_)
				sendPinMode(pin);
		}
	}

	void Jack::gpioMode(pin_t pin)
	{
		if (pin < GpioCount)
			sendPinMode(pin);
	}


	void Jack::gpioInfo(pin_t pin)
	{
		if (pin < GpioCount)
			sendPinInfo(pin);
	}

	void Jack::gpioInfo()
	{
		for (std::size_t i = 0; i < pins_.size(); ++i)
			gpioInfo(i);
	}

	const Jack::Pins& Jack::pins() const
	{
		return pins_;
	}

	Jack::Pins& Jack::pins()
	{
		return pins_;
	}

	const Jack::CounterTimers& Jack::timers() const
	{
		return timers_;
	}

	Jack::CounterTimers& Jack::timers()
	{
		return timers_;
	}


	void Jack::readAnalog(pin_t pin)
	{
		sendReadPin(pin, analogRead(pin));
	}

	void Jack::readAnalog()
	{
		for (std::size_t i = 0; i < pins_.size(); ++i)
			if (pins_[i].type_ == gpio_type::Analog)
				readAnalog(i);
	}

	void Jack::readDigital(pin_t pin)
	{
		sendReadPin(pin, digitalRead(pin));
	}

	void Jack::readDigital()
	{
		for (std::size_t i = 0; i < pins_.size(); ++i)
			if (pins_[i].type_ == gpio_type::Digital)
				readDigital(i);
	}

	void Jack::readPin(pin_t pin)
	{
		GpioPin& gpio = pins_[pin];

		if (gpio.mode_ == gpio_mode::Input || gpio.mode_ == gpio_mode::Pullup)
		{
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
		if (ack_)
			sendReadPin(pin, value);
	}

	void Jack::tcAttach(uint8_t ctid)
	{
		//
		// This function sends info about an attached timer only.
		//
		if (ctid < timers_.size())
			sendTimerInfo(ctid);
	}

	void Jack::tcAttach(uint8_t ctid, pin_t pin, uint8_t mode, uint8_t trigger)
	{
		// 
		// This function configures the timer and pin objects only and does not 
		// attach a hardware interrupt to the pin, which must be done in an 
		// external standalone function.
		//
		if (ctid < timers_.size() && (pin < GpioCount || pin == InvalidPin))
		{
			CounterTimer& timer = timers_[ctid];

			pins_[pin].timer_ = pin == InvalidPin 
				? nullptr
				: &timer;
			timer.pin_ = pin;
			timer.mode_ = mode;
			timer.trigger_ = trigger;
			if (pins_[pin].timer_)
				startTimer(timer);
			else
				stopTimer(timer);
		}
	}

	void Jack::tcAttach(uint8_t ctid, pin_t dummy)
	{
		// 
		// This function serves as a convenience to detach a timer 
		// from a pin with a two argument call. It does not detach
		// any hardware interrupts from the pin, which must be done 
		// in an external standalone function.
		//
		CounterTimer& timer = timers_[ctid];
		GpioPin& pin = pins_[timer.pin_];

		timer.pin_ = InvalidPin;
		pin.timer_ = nullptr;
		stopTimer(timer);
	}

	void Jack::tcStatus(uint8_t ctid, uint8_t status)
	{
		//
		// This function sets the timer status only, and does not attach 
		// a pin to an interrupt or a timer. It sets the timer status  
		// regardless of whether a pin is attached. Unattached timers 
		// will not respond to pin events, but can be used as general 
		// purpose timers with this function.
		//

		if (ctid < timers_.size())
		{
			CounterTimer& timer = timers_[ctid];

			switch (timer.mode_)
			{
			case CounterTimer::Mode::Counter:
				switch (status)
				{
				case CounterTimer::Action::Start:
					timer.counter_.start();
					break;
				case CounterTimer::Action::Stop:
					timer.counter_.stop();
					break;
				case CounterTimer::Action::Resume:
					timer.counter_.resume();
					break;
				case CounterTimer::Action::Reset:
					timer.counter_.reset();
					break;
				default:
					break;
				}
				break;
			case CounterTimer::Mode::Timer:
				switch (status)
				{
				case CounterTimer::Action::Start:
					timer.timer_.start();
					break;
				case CounterTimer::Action::Stop:
					timer.timer_.stop();
					break;
				case CounterTimer::Action::Resume:
					timer.timer_.resume();
					break;
				case CounterTimer::Action::Reset:
					timer.timer_.reset();
					break;
				default:
					break;
				}
				break;
			default:
				break;
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
		//
		// This function is used to add a list of external, user-defined
		// commands to the built-in list.
		//
		if (il.size())
		{
			auto list_end = std::end(commands_);
			auto first = const_cast<command_type**>(il.begin());
			auto last = const_cast<command_type**>(il.begin()) + il.size();

			// Resize and append commands in list to the current collection.
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
			// digitalPinToInterrupt(i) expands to non-negative value if pin i supports interrupts.

			if (digitalPinHasPWM(i))
				pin.type_ = gpio_type::Pwm;
			else if (i >= (GpioCount - AnalogInCount) && getAnalogPins(GpioCount - i - 1) != -1)
				pin.type_ = gpio_type::Analog;
			pin.mode_ = i == LedPinNumber ? gpio_mode::Output : gpio_mode::Input;
			pin.int_ = pin.isDigital() && digitalPinToInterrupt(i) != NOT_AN_INTERRUPT;
			//gpioMode(i, pin.mode_); // doesn't work on megaavr boards
			pin.timer_ = nullptr;
		}
	}

	void Jack::initTimers(CounterTimers& timers)
	{
		for (std::size_t i = 0; i < timers.size(); ++i)
		{
			timers[i].idx_ = i;
			timers[i].pin_ = InvalidPin;
			timers[i].mode_ = CounterTimer::Mode::Counter;
			timers[i].trigger_ = FALLING;
		}
	}

	void Jack::loadConfig(EEStream& eeprom, const address_t addr)
	{
		// Need to read & validate eeprom id and, if invalid, 
		// store the id and current config to properly format
		// eeprom memory.

		eeprom.address() = addr;
		for (std::size_t i = 0; i < pins_.size(); ++i)
		{
			gpio_mode pin_mode = gpio_mode::Input;

			eeprom >> pin_mode;
			gpioMode(i, pin_mode);
		}
	}

	void Jack::sendPinMode(pin_t pin)
	{
		char buf[16];

		reply(buf, FmtPinMode, KeyPinMode, pin, pins_[pin].mode_);
	}

	void Jack::sendPinInfo(pin_t pin)
	{
		char buf[16];

		reply(buf, FmtPinInfo, KeyPinInfo, pin, pins_[pin].type_, pins_[pin].mode_, pins_[pin].int_);
	}

	void Jack::sendReadPin(pin_t pin, uint16_t value)
	{
		char buf[16];

		reply(buf, FmtReadPin, pin, value);
	}

	void Jack::sendReset()
	{
		char buf[4];

		reply(buf, KeyDevReset);
	}

	void Jack::sendTimerInfo(uint8_t ctid)
	{
		char buf[32];
		CounterTimer& timer = timers_[ctid];
		bool active = false;
		unsigned long value = 0UL;

		switch (timer.mode_)
		{
		case CounterTimer::Mode::Counter:
			active = timer.counter_.active();
			value = timer.counter_.count();
			break;
		case CounterTimer::Mode::Timer:
			active = timer.timer_.active();
			value = timer.timer_.elapsed().count();
			break;
		default:
			break;
		}
		reply(buf, FmtTimerInfo, KeyTcAttach, ctid, timer.pin_, timer.mode_, active, value);
	}

	void Jack::setPin(pin_t pin, uint8_t mode)
	{
		if (mode == gpio_mode::Disabled)
			mode = gpio_mode::Input;
		pinMode(pin, mode);
	}

	void Jack::startTimer(CounterTimer& timer)
	{
		switch (timer.mode_)
		{
		case CounterTimer::Mode::Counter:
			timer.counter_.start();
			break;
		case CounterTimer::Mode::Timer:
			timer.timer_.start();
			break;
		default:
			break;
		}
	}

	void Jack::stopTimer(CounterTimer& timer)
	{
		switch (timer.mode_)
		{
		case CounterTimer::Mode::Counter:
			timer.counter_.stop();
			break;
		case CounterTimer::Mode::Timer:
			timer.timer_.stop();
			break;
		default:
			break;
		}
	}

	void Jack::storeConfig(EEStream& eeprom, const address_t addr)
	{
		//
		// Need to write the eeprom id first.
		//
		eeprom.address() = addr;
		for (auto i : pins_)
			eeprom << i.mode_;
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

