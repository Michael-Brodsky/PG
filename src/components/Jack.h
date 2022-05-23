/*
 *	This files defines a remote control and data acquisition class.
 *
 *	***************************************************************************
 *
 *	File: Jack.h
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
 *	**************************************************************************
 * 
 *	Description:
 * 
 *	The `Jack' class allows clients to operate a device remotely, over a 
 *	network connection using a simple messaging protocol. Clients can 
 *	configure, read/write values to/from the device’s gpio pins, and count/ 
 *	time external events. Jack also supports EEPROM storage and recall of its 
 *	current connection and configuration information. Network communications 
 *	are handled by the `Connection' type (see <Connection.h>); the messaging 
 *	protocol is defined by the `RemoteControl' type, which also handles 
 *	message decoding and command execution (see <RemoteControl.h>).
 *
 *	Jack defines a collection of built-in command objects that, when executed, 
 *	call one of the member methods declared by its public interface (see 
 *	below). Command objects are defined by a key string and a parameter list. 
 *	`RemoteControl' listens for inbound messages and executes matching 
 *	commands, passing any parameters encoded in the message as method 
 *	arguments. `Jack' also supports user-defined commands that can be appended 
 *	to the built-in collection. This allows developers to expand an 
 *	application’s capabilities with custom remote function/method calls.
 *
 *	Messages are plain text strings that have the general form: 
 *
 *		key[=arg0,arg1,…,argN], 
 *
 *	where key is the command identifier (name) optionally followed by a comma-
 *	separated list of arguments (values). Messages are terminated with a 
 *	newline character ‘\n’. For example: 
 *
 *		rst - resets the device
 *		inf - sends device hardware info
 *		pin - sends pin type info for all pins
 *		pin=n - sends pin type info for pin n
 *		pmd=n - sends pin mode info for pin n
 *		pmd=n,m - sets pin n mode to m
 *		rdp=n - reads the state of input pin n
 *		wrp=n,v writes value v to output pin n
 *
 *	Read commands reply with a message in similar fashion. For example, rdp=n 
 *	replies with: 
 *
 *		n=v 
 *
 *	where v is the state of input pin n. Clients can decode replies and take 
 *	appropriate action. To reduce network traffic and execution time, Jack 
 *	does not implement any message handshaking or transactions, other than 
 *	acknowledging receipt of write messages (those which normally do not send 
 *	a reply). This can be enabled/disabled with the `ack' command. Otherwise 
 *	clients must implement any message error detection/correction schemes, 
 *	including retransmitting messages where a reply was expected but not 
 *	received.
 *
 *	Messages can be sent consecutively at network speeds, however Jack 
 *	currently does not implement any message queueing. Messages are decoded, 
 *	executed and replies sent as they are received. Therefore, message sizes 
 *	and “queues” are limited by the device hardware buffer sizes. For serial 
 *	connections this is the hardware receive and transmit buffer sizes, and 
 *	for UDP connections, the UDP packet size.

 *	**************************************************************************/

#if !defined __PG_JACK_H
# define __PG_JACK_H 20220504L

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

	// Remote control and data acquisition class.
	class Jack : public iclockable, public icomponent
	{
	public:
		struct GpioPin;															// Forward decl (see below).
		struct CounterTimer;													// Forward decl (see below).
		using command_type = RemoteControl::command_type;						// Remote command type.
		using cmdlist_type = std::initializer_list<command_type*>;				// Remote command initializer list type.
		using devid_type = unsigned long;										// Device id storage class type.
		using fmt_type = const char*;											// Reply message formatting type.
		using isr_type = pg::callback<void>::type;								// Isr function call type.
		template<class... Ts>													// Jack remote command template.
		using JackCommand = typename RemoteControl::Command<void, Jack, Ts...>; 
		using key_type = typename command_type::key_type;						// Remote command key storage type.
		using size_type = uint8_t;												// Type that can hold the size of any object.
		using timer_type = Timer<milliseconds>;									// Event timer type.
		using value_type = uint16_t;											// Type that can hold any pin input/output value.
		using counter_type = Counter<value_type>;								// Event counter type.
		using timer_t = uint8_t;												// Timer index type alias.

		static const devid_type DeviceId = 20220430UL;				// Identifier used to verify eeprom contents.
		//static const size_type GpioCount = NUM_DIGITAL_PINS;		// Total number of gpio pins of any type.
		//static const size_type AnalogInCount = NUM_ANALOG_INPUTS;	// Total number of gpio pins with analog input capability.
		//static const size_type LedPinNumber = LED_BUILTIN;			// Built-in LED pin number.
		static const size_type CommandsMaxCount = 32;				// Maximum number of storable remote commands.
		static const size_type TimersMaxCount = 16;					// Maximum number of event counters/timers.
		static const size_type InterruptsCount =					// Number of available hardware interrupts.
			countInterrupts<GpioCount>();
		static const size_type TimersCount =						// Number of event counters/timers.
			InterruptsCount < TimersMaxCount 
			? InterruptsCount 
			: TimersMaxCount;

		using Commands = typename std::valarray<command_type*, CommandsMaxCount>;	// Remote commands collection type.
		using Timers = typename std::array<CounterTimer, TimersCount>;				// Event counters/timers collection type.
		using Pins = std::array<GpioPin, GpioCount>;								// GpioPins collection type.

		// Aggregates counters and timers into a single object. CounterTimers
		// can be attached to a gpio pin and triggered by hardware interrupts.
		struct CounterTimer
		{
			// Enumerates valid CounterTimer operating modes.
			enum Mode : uint8_t
			{
				Counter = 0,	// Counter mode.
				Timer = 1		// Timer mode.
			};

			// Enumerates valid CounterTimer actions.
			enum Action : uint8_t
			{
				Start = 0,		// Starts the CounterTimer.
				Stop = 1,		// Stops the CounterTimer.
				Resume = 2,		// Resumes the CounterTimer with the current count/elapsed time.
				Reset = 3		// Resets the current count/elapsed time.
			};

			// Enumerates CounterTimer timing modes. 
			enum Timing : uint8_t
			{
				Immediate = 0,	// Timer starts when attached, stops when triggered.
				OneShot = 1,	// Timer starts then stops when triggered once.
				Continuous = 2	// Timer is toggled on each trigger event.
			};

			pin_t			pin_;		// The currently attached pin, if any.
			Mode			mode_;		// The current operating mode.
			PinStatus		trigger_;	// The current event trigger.
			Timing			timing_;	// The current timing mode.
			timer_type		timer_;		// Timer object.
			counter_type	counter_;	// Counter object.
		};

		using timer_action = CounterTimer::Action;
		using timer_mode = CounterTimer::Mode;
		using timing_mode = CounterTimer::Timing;

		// Aggregates information about a gpio pin.
		struct GpioPin
		{
			// Enumerates valid gpio pin types.
			enum Type : uint8_t
			{
				Digital = 0,	// Digital input/output.								
				Analog = 1,		// Analog input.
				Pwm = 2 		// Pwm output.
			};

			// Enumerates valid gpio pin modes.
			enum Mode : uint8_t
			{
				Input = 0,		// Input pin.
				Output = 1,		// Output pin.
				Pullup = 2,		// Input pin w/ internal pullup resistor enabled.
				PwmOut = 3,		// Pwm output pin.
				Invalid = 4
			};

			Type			type_;		// Pin i/o type.
			Mode			mode_;		// Pin i/o mode.
			bool			int_;		// Pin has hardware interrupt.

			// Checks whether the pin is currently configured as an input.
			bool isInput() const { return mode_ == gpio_mode::Input || mode_ == gpio_mode::Pullup; }
			// Checks whether the pin is currently configured as an output.
			bool isOutput() const { return mode_ == gpio_mode::Output || mode_ == gpio_mode::PwmOut; }
			// Checks whether the pin is an analog input.
			bool isAnalog() const { return type_ == gpio_type::Analog; }
			// Checks whether the pin is a digital input/ouput.
			bool isDigital() const { return !isAnalog(); }
		};

		using gpio_type = GpioPin::Type;
		using gpio_mode = GpioPin::Mode;

#pragma region strings

		/*
		 *  Command key and reply string format specifiers.
		 */

		static constexpr key_type KeyDevReset = "rst";		// Device reset.
		static constexpr key_type KeyDevInfo = "inf";		// Device info.
		static constexpr key_type KeyAcknowledge = "ack";	// Write acknowledge.
		static constexpr key_type KeyPinInfo = "pin";		// Get pin info.
		static constexpr key_type KeyPinMode = "pmd";		// Get/set pin mode.
		static constexpr key_type KeyReadPin = "rdp";		// Read pin (type dependent).
		static constexpr key_type KeyWritePin = "wrp";		// Write pin (type dependent).
		static constexpr key_type KeyTimerStatus = "tcs";	// Get/set timer state.
		static constexpr key_type KeyTimerAttach = "atc";	// Attach/detach timer, get timer info.

		static constexpr fmt_type FmtDevInfo = "%s=%lu,%s,%s,%u,%u,%u";	// inf=id,type,mcutype,mcuspd,#pins,#timers
		static constexpr fmt_type FmtPinInfo = "%s=%u,%u,%u,%u";		// pin=p#,type,int,mode
		static constexpr fmt_type FmtPinMode = "%s=%u,%u";				// pmd=p#,mode
		static constexpr fmt_type FmtReadPin = "%u=%u";					// p#=value
		static constexpr fmt_type FmtAcknowledge = "%s=%u";				// ack=0||1
		static constexpr fmt_type FmtTimerAttach = "%s=%u,%u,%u,%u,%u";	// tmr=t#,p#,mode,trigger,timing
		static constexpr fmt_type FmtTimerStatus = "%s=%u,%u,%lu";		// tmr=t#,active,value

#pragma endregion
#pragma region interrupt service routines

		static void isrTimer0();
		static void isrTimer1();
		static void isrTimer2();
		static void isrTimer3();

#pragma endregion

	public:
		explicit Jack(Connection* = nullptr, cmdlist_type = cmdlist_type());

	public:
		void clock() override;
		void cmdAcknowledge(bool);
		void cmdAcknowledge();
		void cmdDevInfo();
		void cmdDevReset();
		void cmdPinInfo(pin_t);
		void cmdPinInfo();
		void cmdPinMode(pin_t, uint8_t);
		void cmdPinMode(pin_t);
		void cmdReadPin(pin_t);
		void cmdReadPin();
		void cmdTimerAttach(timer_t, pin_t, uint8_t, uint8_t, uint8_t);
		void cmdTimerDetach(timer_t, pin_t); // Rename cmdTimerAttach
		void cmdTimerInfo(timer_t); // Rename cmdTimerAttach
		void cmdTimerInfo(); // Rename cmdTimerAttach
		void cmdTimerStatus(timer_t, uint8_t);
		void cmdTimerStatus(timer_t);
		void cmdTimerStatus();
		void cmdWritePin(pin_t, value_type);
		void connection(Connection*);
		Connection* connection() const;
		void isrHandler(timer_t);
		Pins& pins();
		const Pins& pins() const;
		template<class... Ts>
		void sendMessage(const char*, Ts...);
		Timers& timers();
		const Timers& timers() const;

	private:
		void attachTimer(timer_t, pin_t, uint8_t, uint8_t, uint8_t);
		void detachTimer(timer_t);
		void endTimer(timer_t);
		isr_type getIsr(timer_t);
		void initialize(cmdlist_type&);
		void initialize(Pins&);
		void initialize(Timers&);
		void readPin(pin_t);
		void readAnalog(pin_t);
		void readDigital(pin_t);
		void sendPinInfo(pin_t);
		void sendPinMode(pin_t);
		void sendPinValue(pin_t, value_type);
		void sendTimerInfo(timer_t);
		void sendTimerStatus(timer_t);
		void writePin(pin_t, value_type);

	private:
		/* Built-in remote commands*/

		JackCommand<void> cmd_devinfo_{ KeyDevInfo, *this, &Jack::cmdDevInfo };
		JackCommand<void> cmd_devreset_{ KeyDevReset, *this, &Jack::cmdDevReset };
		JackCommand<void> cmd_getack_{ KeyAcknowledge, *this, &Jack::cmdAcknowledge };
		JackCommand<pin_t> cmd_getpininfo_{ KeyPinInfo, *this, &Jack::cmdPinInfo };
		JackCommand<void> cmd_getpinsinfo_{ KeyPinInfo, *this, &Jack::cmdPinInfo };
		JackCommand<pin_t> cmd_getpinmode_{ KeyPinMode, *this, &Jack::cmdPinMode };
		JackCommand<timer_t, uint8_t> cmd_setimerstatus{ KeyTimerStatus, *this, &Jack::cmdTimerStatus };
		JackCommand<timer_t> cmd_getimerstatus{ KeyTimerStatus, *this, &Jack::cmdTimerStatus };
		JackCommand<void> cmd_getimersstatus{ KeyTimerStatus, *this, &Jack::cmdTimerStatus };
		JackCommand<pin_t> cmd_readpin_{ KeyReadPin, *this, &Jack::cmdReadPin };
		JackCommand<void> cmd_readpins_{ KeyReadPin, *this, &Jack::cmdReadPin };
		JackCommand<bool> cmd_setack_{ KeyAcknowledge, *this, &Jack::cmdAcknowledge };
		JackCommand<pin_t, uint8_t> cmd_setpinmode_{ KeyPinMode, *this, &Jack::cmdPinMode };
		JackCommand<timer_t, pin_t, uint8_t, uint8_t, uint8_t> cmd_timerattach_{ KeyTimerAttach, *this, &Jack::cmdTimerAttach };
		JackCommand<timer_t, pin_t> cmd_timerdetach_{ KeyTimerAttach, *this, &Jack::cmdTimerDetach };
		JackCommand<timer_t> cmd_timerinfo_{ KeyTimerAttach, *this, &Jack::cmdTimerInfo };
		JackCommand<void> cmd_timersinfo_{ KeyTimerAttach, *this, &Jack::cmdTimerInfo };
		JackCommand<pin_t, value_type> cmd_writepin_{ KeyWritePin, *this, &Jack::cmdWritePin };

		/* Private class members */

		Connection*		connection_;	// Current network connection.
		RemoteControl	remote_;		// Remote command processor.
		Pins			pins_;			// Gpio pins collection.
		Timers			timers_;		// Event counter/timers collection.
		Commands		commands_;		// Remote commands collection.
		bool			ack_;			// Write command acknowledge flag.
	};
} // namespace pg

extern pg::Jack _jack;	// Needed by static isr methods.

namespace pg
{
#pragma region public methods

	Jack::Jack(Connection* connection, cmdlist_type commands) :
		connection_(connection), pins_(), timers_(), ack_(), remote_(connection),
		commands_({ &cmd_readpin_ , &cmd_readpins_, &cmd_writepin_ , &cmd_setpinmode_, &cmd_getpinmode_,
			&cmd_timerattach_, &cmd_timerdetach_, &cmd_timerinfo_, &cmd_timersinfo_, &cmd_setimerstatus, 
			&cmd_getimerstatus, &cmd_getimersstatus, &cmd_getpininfo_, &cmd_getpinsinfo_,
			&cmd_devinfo_, &cmd_devreset_, &cmd_setack_, &cmd_getack_ })
	{
		initialize(pins_);
		initialize(timers_);
		initialize(commands);
	}

	void Jack::clock()
	{
		remote_.poll();
	}

	void Jack::cmdAcknowledge(bool value)
	{
		if ((ack_ = value))
			cmdAcknowledge();
	}

	void Jack::cmdAcknowledge()
	{
		sendMessage(FmtAcknowledge, KeyAcknowledge, ack_);
	}

	void Jack::cmdDevInfo()
	{
		sendMessage(FmtDevInfo, cmd_devinfo_.key(), DeviceId,
			board_traits<board_type>::board, board_traits<board_type>::mcu,
			static_cast<unsigned>(board_traits<board_type>::clock_frequency / 1000000),
			static_cast<size_type>(pins_.size()), static_cast<size_type>(timers_.size()));
	}

	void Jack::cmdDevReset()
	{
		sendMessage(KeyDevReset);	// Send reset ack to host.
		delay(50);					// Wait for Tx buf to empty.
		resetFunc();				// Reset the device.
	}
	void Jack::cmdPinInfo(pin_t n)
	{
		if (n < GpioCount)
			sendPinInfo(n);
	}

	void Jack::cmdPinInfo()
	{
		for (size_type i = 0; i < pins_.size(); ++i)
			sendPinInfo(i);
	}

	void Jack::cmdPinMode(pin_t n, uint8_t mode)
	{
		if (n < GpioCount && mode < gpio_mode::Invalid)
		{
			pinMode(n, mode);
			pins_[n].mode_ = static_cast<gpio_mode>(mode);
			if (ack_)
				sendPinMode(n);
		}
	}

	void Jack::cmdPinMode(pin_t n)
	{
		if (n < GpioCount)
			sendPinMode(n);
	}

	void Jack::cmdReadPin(pin_t n)
	{
		if (n < GpioCount)
			readPin(n);
	}

	void Jack::cmdReadPin()
	{
		for (size_type i = 0; i < GpioCount; ++i)
			readPin(i);
	}

	void Jack::cmdTimerAttach(timer_t t, pin_t p, uint8_t mode, uint8_t trigger, uint8_t timing = timing_mode::Immediate)
	{

		if (t < TimersCount && (p < GpioCount || p == InvalidPin))
		{
			pin_t attached_pin = timers_[t].pin_;

			if (attached_pin != InvalidPin)
				detachTimer(t);
			if (p != InvalidPin)
			{
				isr_type isr = getIsr(t);

				if (isr)
					attachInterrupt(digitalPinToInterrupt(p), isr, static_cast<PinStatus>(trigger));
				else
					p = InvalidPin;
			}
			attachTimer(t, p, mode, trigger, timing);
		}
	}

	void Jack::cmdTimerDetach(timer_t n, pin_t dummy)
	{
		detachTimer(n);
	}

	void Jack::cmdTimerInfo(timer_t n)
	{
		if (n < TimersCount)
			sendTimerInfo(n);
	}

	void Jack::cmdTimerInfo()
	{
		for (size_type i = 0; i < TimersCount; ++i)
			sendTimerInfo(i);
	}

	void Jack::cmdTimerStatus(timer_t n, uint8_t action)
	{
		if (n < TimersCount)
		{
			timer_action act = static_cast<timer_action>(action);
			CounterTimer& timer = timers_[n];

			switch (timer.mode_)
			{
			case timer_mode::Counter:
				switch (act)
				{
				case timer_action::Start:
					timer.counter_.start();
					break;
				case timer_action::Stop:
					timer.counter_.stop();
					break;
				case timer_action::Resume:
					timer.counter_.resume();
					break;
				case timer_action::Reset:
					timer.counter_.reset();
					break;
				default:
					break;
				}
				break;
			case timer_mode::Timer:
				switch (act)
				{
				case timer_action::Start:
					timer.timer_.start();
					break;
				case timer_action::Stop:
					timer.timer_.stop();
					break;
				case timer_action::Resume:
					timer.timer_.resume();
					break;
				case timer_action::Reset:
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

	void Jack::cmdTimerStatus(timer_t n)
	{
		if (n < TimersCount)
			sendTimerStatus(n);
	}

	void Jack::cmdTimerStatus()
	{
		for (size_type i = 0; i < TimersCount; ++i)
			sendTimerStatus(i);
	}

	void Jack::cmdWritePin(pin_t n, value_type value)
	{
		if (n < GpioCount)
			writePin(n, value);
	}

	void Jack::connection(Connection* cn)
	{
		remote_.connection(cn);
		connection_ = cn;
	}

	Connection* Jack::connection() const
	{
		return connection_;
	}

	void Jack::isrHandler(timer_t n)
	{
		CounterTimer& timer = timers_[n];

		switch (timer.mode_)
		{
		case timer_mode::Counter:
			timer.counter_++;
			break;
		case timer_mode::Timer:
			switch (timer.timing_)
			{
			case timing_mode::OneShot:
				if (!timer.timer_.active())
					timer.timer_.start();
				else
					endTimer(n);
				break;
			case timing_mode::Immediate:
				endTimer(n);
				break;
			case timing_mode::Continuous:
				if (timer.timer_.active())
					timer.timer_.stop();
				else
					timer.timer_.start();
				break;
			default:
				break;
			}
		default:
			break;
		}
	}

	Jack::Pins& Jack::pins()
	{
		return pins_;
	}

	const Jack::Pins& Jack::pins() const
	{
		return pins_;
	}

	template<class... Ts>
	void Jack::sendMessage(const char* fmt, Ts... args)
	{
		char msg[64] = { '\0' };

		std::sprintf(msg, fmt, args...);
		connection_->send(msg);
	}

	Jack::Timers& Jack::timers()
	{
		return timers_;
	}

	const Jack::Timers& Jack::timers() const
	{
		return timers_;
	}

#pragma endregion
#pragma region private methods

	void Jack::attachTimer(timer_t t, pin_t p, uint8_t mode, uint8_t trigger, uint8_t timing)
	{
		CounterTimer& timer = timers_[t];

		timer.pin_ = p;
		timer.mode_ = static_cast<timer_mode>(mode);
		timer.trigger_ = static_cast<PinStatus>(trigger);
		timer.timing_ = static_cast<timing_mode>(timing);
		switch (timer.mode_)
		{
		case timer_mode::Counter:
			timer.counter_.start();
			break;
		case timer_mode::Timer:
			switch (timer.timing_)
			{
			case timing_mode::Immediate:
				timer.timer_.start();
				break;
			case timing_mode::OneShot:
			case timing_mode::Continuous:
				timer.timer_.stop();
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

	void Jack::detachTimer(timer_t n)
	{
		CounterTimer& timer = timers_[n];

		switch (timer.mode_)
		{
		case timer_mode::Counter:
			timer.counter_.stop();
			break;
		case timer_mode::Timer:
			timer.timer_.stop();
			break;
		default:
			break;
		}
		detachInterrupt(digitalPinToInterrupt(timer.pin_));
		timer.pin_ = InvalidPin;
	}

	void Jack::endTimer(timer_t n)
	{
		timers_[n].timer_.stop();
		detachTimer(n);
	}

	Jack::isr_type Jack::getIsr(timer_t n)
	{
		isr_type isr = nullptr;

		switch (n)
		{
		case 0:
			isr = &Jack::isrTimer0;
			break;
		case 1:
			isr = &Jack::isrTimer1;
			break;
		case 2:
			isr = &Jack::isrTimer2;
			break;
		case 3:
			isr = &Jack::isrTimer3;
			break;
		default:
			break;
		}

		return isr;
	}

	void Jack::initialize(cmdlist_type& user_cmds)
	{
		if (user_cmds.size())
		{
			auto list_end = std::end(commands_);
			auto first = const_cast<command_type**>(user_cmds.begin());
			auto last = const_cast<command_type**>(user_cmds.begin()) + user_cmds.size();

			commands_.resize(commands_.size() + user_cmds.size());
			std::copy(first, last, list_end);
		}
		remote_.commands(std::begin(commands_), std::end(commands_));
	}

	void Jack::initialize(Pins& pins)
	{
		for (size_type i = 0; i < pins.size(); ++i)
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
			pin.int_ = pin.isDigital() && digitalPinToInterrupt(i) != NOT_AN_INTERRUPT;
			pin.mode_ = i == LedPinNumber ? gpio_mode::Output : gpio_mode::Input;
			//gpioMode(i, pin.mode_); // doesn't work on megaavr boards
		}

	}

	void Jack::initialize(Timers& timers)
	{
		for (size_type i = 0; i < timers.size(); ++i)
		{
			CounterTimer& timer = timers[i];

			timer.pin_ = InvalidPin;
			timer.mode_ = timer_mode::Counter;
			timer.trigger_ = FALLING;
			timer.timing_ = timing_mode::Immediate;
		}
	}

	void Jack::readAnalog(pin_t n)
	{
		sendPinValue(n, analogRead(n));
	}

	void Jack::readDigital(pin_t n)
	{
		sendPinValue(n, digitalRead(n));
	}

	void Jack::readPin(pin_t n)
	{
		GpioPin& pin = pins_[n];

		if (pin.isInput())
		{
			switch (pin.type_)
			{
			case gpio_type::Analog:
				readAnalog(n);
				break;
			case gpio_type::Pwm:
			case gpio_type::Digital:
				readDigital(n);
				break;
			default:
				break;
			}
		}
	}

	void Jack::sendPinInfo(pin_t n)
	{
		const GpioPin& pin = pins_[n];

		sendMessage(FmtPinInfo, KeyPinInfo, n, pin.type_, pin.int_, pin.mode_);
	}

	void Jack::sendPinMode(pin_t n)
	{
		const GpioPin& pin = pins_[n];

		sendMessage(FmtPinMode, KeyPinMode, n, pin.mode_);
	}

	void Jack::sendPinValue(pin_t n, value_type value)
	{
		sendMessage(FmtReadPin, n, value);
	}

	void Jack::sendTimerInfo(timer_t n)
	{
		const CounterTimer& timer = timers_[n];

		sendMessage(FmtTimerAttach, KeyTimerAttach, n, timer.pin_, timer.mode_, timer.trigger_, timer.timing_);
	}

	void Jack::sendTimerStatus(timer_t n)
	{
		CounterTimer& timer = timers_[n];
		bool active = false;
		unsigned long value = 0;

		switch (timer.mode_)
		{
		case timer_mode::Counter:
			active = timer.counter_.active();
			value = timer.counter_.count();
			break;
		case timer_mode::Timer:
			active = timer.timer_.active();
			value = timer.timer_.elapsed().count();
			break;
		default:
			break;
		}
		sendMessage(FmtTimerStatus, KeyTimerStatus, n, active, value);
	}

	void Jack::writePin(pin_t n, value_type value)
	{
		GpioPin& pin = pins_[n];
		value_type write_value = value;

		switch (pin.type_)
		{
		case gpio_type::Pwm:
			if (pin.mode_ == gpio_mode::PwmOut)
				analogWrite(n, write_value);
			else
			{
				write_value &= 0x1;
				digitalWrite(n, write_value);
			}
			break;
		case gpio_type::Analog:	// Analog pins are digital out only.
		case gpio_type::Digital:
			write_value &= 0x1;
			digitalWrite(n, write_value);
			break;
		default:
			break;
		}
		if (ack_)
			sendPinValue(n, write_value);
	}

#pragma endregion
#pragma region static methods

	void Jack::isrTimer0() { _jack.isrHandler(0); }
	void Jack::isrTimer1() { _jack.isrHandler(1); }
	void Jack::isrTimer2() { _jack.isrHandler(2); }
	void Jack::isrTimer3() { _jack.isrHandler(3); }

#pragma endregion

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 
#endif // !defined __PG_JACK_H
