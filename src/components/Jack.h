/*
 *	This files defines a remote control and data acquisition class.
 *
 *	***************************************************************************
 *
 *	File: Jack.h
 *	Date: May 25, 2022
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
 *	protocol is defined by the `Interpreter' type, which also handles 
 *	message decoding and command execution (see <Interpreter.h>).
 *
 *	Jack defines a collection of built-in command objects that, when executed, 
 *	call one of the member methods declared by its public interface (see 
 *	below). Command objects are defined by a key string and a parameter list. 
 *	Jack listens to the network connection and forwards any messages received 
 *	to the `Interpreter' which decodes and executes any valid commands.
 *	Any parameters encoded in the message are passed as arguments to the 
 *	delegate function/method. `Jack' also supports user-defined commands that 
 *	can be appended to the built-in collection. This allows developers to 
 *	expand an application’s capabilities with custom function/method calls.
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
 *		pmd=n,m - sets pin n to mode m
 *		rdp=n - sends the state of input pin n
 *		wrp=n,v - writes value v to output pin n
 *		net - sends the current connection parameters
 *		net=t,a,b,c - opens a new connection of type t with parameters a,b,c 
 *		lda - loads the device configuration from eeprom
 *		sto - stores the current device configuration to eeprom
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
 *
 *  Jack also defines a function that allows users to force the device to 
 *	use the default connection at power-up. It checks a digital input pin 
 *	and, if the pin is in the LOW state, opens the default connection 
 *	regardles any settings saved in eeprom.
 *	
 *	The eeprom memory needs to be intitialized the first time the program is
 *	run, otherwise the program may crash, reboot the device or exhibit
 *	erratic behavior. This is done by uncommenting the line
 *	`invalidateEeprom()' in the initialize() function, then compiling and 
 *	running the program. Once intitialized, the program should be recompiled 
 *	and uploaded with the `invalidateEeprom()' line commented out. If not,
 *	the eeprom memory will be overwritten each time the device is reset or
 *	power-cycled and you will lose any saved information.
 *
 *	**************************************************************************/

#if !defined __PG_JACK_H
# define __PG_JACK_H 20220525L

# include "cstdio"
# include "cstring"
# include "valarray"
# include "system/boards.h"
# include "system/utils.h"
# include "interfaces/iclockable.h"
# include "interfaces/icomponent.h"
# include "utilities/EEStream.h"
# include "utilities/Timer.h"
# include "utilities/Interpreter.h"
# include "utilities/Connection.h"

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	using std::chrono::milliseconds;

	// Remote control and data acquisition class.
	class Jack : public iclockable, public icomponent
	{
	public:
		struct GpioPin;															// Forward decl (see below).
		struct JackTimer;														// Forward decl (see below).
		using address_type = EEStream::address_type;							// EEPROM address storage type.
		using command_type = Interpreter::CommandBase;							// Remote command type.
		using cmdlist_type = std::initializer_list<command_type*>;				// Remote command initializer list type.
		using connection_type = Connection::Type;								// Network connection storage type.
		using devid_type = unsigned long;										// Device id storage class type.
		using fmt_type = const char*;											// Reply message formatting type.
		using isr_type = pg::callback<void>::type;								// Isr function call type.
		template<class... Ts>													// Jack command template.
		using Command = typename Interpreter::Command<void, Jack, Ts...>;		
		using key_type = typename command_type::key_type;						// Command key storage type.
		using size_type = uint8_t;												// Type that can hold the size of any object.
		using value_type = uint16_t;											// Type that can hold any pin input/output value.
		using ct_type = CounterTimer<milliseconds, value_type>;					// Aggregate counter/timer type.
		using counter_tag = ct_type::counter_tag;								// Tag type for dispatching counter methods.
		using timer_tag = ct_type::timer_tag;									// Tag type for dispatching timer methods.
		using timer_t = uint8_t;												// Timer index type alias.

		static constexpr devid_type DeviceId = 20220430UL;				// Identifier used to verify eeprom contents.
		static constexpr size_type CommandsMaxCount = 32;				// Maximum number of storable remote commands.
		static constexpr size_type TimersMaxCount = 16;					// Maximum number of event counters/timers.
		static constexpr size_type InterruptsCount =					// Number of available hardware interrupts.
			countInterrupts<GpioCount>();
		static constexpr size_type TimersCount = 						// Number of event counters/timers.
			InterruptsCount < TimersMaxCount 
			? InterruptsCount 
			: TimersMaxCount;

		static constexpr pin_t PowerOnDefaultsPin = 2;
		static constexpr connection_type DefaultConnectionType = connection_type::Serial;
		static constexpr const char* DefaultConnectionParams = "9600,8N1";

		using Commands = typename std::valarray<command_type*, CommandsMaxCount>;	// Remote commands collection type.
		using Timers = typename std::array<JackTimer, TimersCount>;					// Event counters/timers collection type.
		using Pins = std::array<GpioPin, GpioCount>;								// GpioPins collection type.
		using Isrs = std::array<isr_type, TimersCount>;								// ISRs collection type.

		/* TODO: Reimplement the timed pin read scheme. */

		// Aggregates information about an event counter/timer. 
		struct JackTimer
		{
			// Enumerates valid counter/timer operating modes.
			enum Mode : uint8_t
			{
				Counter = 0,	// Counter mode.
				Timer = 1		// Timer mode.
			};

			// Enumerates valid counter/timer actions.
			enum Action : uint8_t
			{
				Start = 0,		// Starts the counter/timer.
				Stop = 1,		// Stops the counter/timer.
				Resume = 2,		// Resumes the counter/timer with the current count/elapsed time.
				Reset = 3		// Resets the current count/elapsed time.
			};

			// Enumerates JackTimer timing modes. 
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
			ct_type			object_;	// The counter/timer object.
		};

		using timer_action = JackTimer::Action;
		using timer_mode = JackTimer::Mode;
		using timing_mode = JackTimer::Timing;

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

		// EEPROM Memory Map
		// 
		//  |          |             |               |                   |
		//	| DeviceId | Pins Config | Timers Config | Connection Params |
		//  |          |             |               |                   |
		//	0		   A                             B

		static constexpr address_type ConfigurationEepromAddress = sizeof(devid_type);	// A
		static constexpr address_type ConnectionEepromAddress =							// B
			ConfigurationEepromAddress +
			(GpioCount * sizeof(decltype(GpioPin::mode_))) +
			Jack::TimersCount * (sizeof(decltype(JackTimer::pin_)) +
				sizeof(decltype(JackTimer::mode_)) +
				sizeof(decltype(JackTimer::trigger_)) +
				sizeof(decltype(JackTimer::timing_)));

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
		static constexpr key_type KeyReadPinList = "rdl";	// Read pin list (type dependent).
		static constexpr key_type KeyWritePin = "wrp";		// Write pin (type dependent).
		static constexpr key_type KeyTimerStatus = "tcs";	// Get/set timer state.
		static constexpr key_type KeyTimerAttach = "atc";	// Attach/detach and get timer info.
		static constexpr key_type KeyConnection = "net";	// Get/set network connection.
		static constexpr key_type KeyLoadConfig = "lda";	// Load device config from eeprom.
		static constexpr key_type KeyStoreConfig = "sto";	// Store device config to eeprom.

		static constexpr fmt_type FmtConnectionGet = "%s=%u,%s";		// net=type,arg0,arg1,arg2
		static constexpr fmt_type FmtConnectionSet = "%s,%s,%s";		// "arg0,arg1,arg2"
		static constexpr fmt_type FmtDevInfo = "%s=%lu,%s,%s,%u,%u,%u";	// inf=id,type,mcutype,mcuspd,#pins,#timers
		static constexpr fmt_type FmtPinInfo = "%s=%u,%u,%u,%u";		// pin=p#,type,int,mode
		static constexpr fmt_type FmtPinMode = "%s=%u,%u";				// pmd=p#,mode
		static constexpr fmt_type FmtReadPin = "%u=%u";					// p#=value
		static constexpr fmt_type FmtAcknowledge = "%s=%u";				// ack=0|1
		static constexpr fmt_type FmtTimerAttach = "%s=%u,%u,%u,%u,%u";	// tmr=t#,p#,mode,trigger,timing
		static constexpr fmt_type FmtTimerStatus = "%s=%u,%u,%lu";		// tmr=t#,active,value

#pragma endregion
#pragma region interrupt service routines
		//
		// These are called by the board when a hardware interrupt occurs.
		// They in turn call our handler: isrHandler(t) passing the event 
		// counter/timer index that handles the interrupt.

		static void isrTimer0();
		static void isrTimer1();
		static void isrTimer2();
		static void isrTimer3();
		static void isrTimer4();
		static void isrTimer5();
		static void isrTimer6();
		static void isrTimer7();
		static void isrTimer8();
		static void isrTimer9();
		static void isrTimer10();
		static void isrTimer11();
		static void isrTimer12();
		static void isrTimer13();
		static void isrTimer14();
		static void isrTimer15();

#pragma endregion

	public:
		// Constructs a Jack object with an optional list of user-defined commands.
		explicit Jack(cmdlist_type = cmdlist_type());

	public:
		void clock() override;
		void cmdAcknowledge(bool);
		void cmdAcknowledge();
		void cmdConnection();
		void cmdConnection(uint8_t, const char*, const char*, const char*);
		void cmdDevInfo();
		void cmdDevReset();
		void cmdLoadConfig();
		void cmdPinInfo(pin_t);
		void cmdPinInfo();
		void cmdPinMode(pin_t, uint8_t);
		void cmdPinMode(pin_t);
		void cmdReadPin(pin_t);
		void cmdReadPin();
		void cmdReadPinList(char*);
		void cmdStoreConfig();
		void cmdTimerAttach(timer_t, pin_t, uint8_t, uint8_t, uint8_t);
		void cmdTimerDetach(timer_t, pin_t);
		void cmdTimerInfo(timer_t);
		void cmdTimerInfo();
		void cmdTimerStatus(timer_t, uint8_t);
		void cmdTimerStatus(timer_t);
		void cmdTimerStatus();
		void cmdWritePin(pin_t, value_type);
		void connection(Connection*);
		Connection* connection() const;
		void initialize(pin_t = PowerOnDefaultsPin);
		Interpreter& interpreter();
		void isrHandler(timer_t);
		Pins& pins();
		const Pins& pins() const;
		Timers& timers();
		const Timers& timers() const;

	private:
		void attachTimer(JackTimer&, pin_t, uint8_t, uint8_t, uint8_t);
		void closeConnection(Connection*&);
		void detachTimer(JackTimer&);
		void endTimer(timer_t);
		bool eepromValid(EEStream&, devid_type);
		inline isr_type getIsr(timer_t);
		void initialize(cmdlist_type&);
		template<size_type>
		void initialize(Isrs&);
		void initialize(Pins&);
		void initialize(Timers&);
		void intializeEeprom(EEStream&, devid_type, connection_type, const char*, const Pins&, const Timers&);
		void invalidateEeprom(EEStream&);
		void loadConfig(EEStream&, Jack::Pins&, Jack::Timers&);
		Connection* loadConnection(EEStream&, char*);
		Connection* openConnection(connection_type, const char*);
		bool powerOnDefaults(pin_t);
		void readPin(pin_t);
		void readAnalog(pin_t);
		void readDigital(pin_t);
		template<class... Ts>
		void sendMessage(const char*, Ts...);
		void sendPinInfo(pin_t);
		void sendPinMode(pin_t);
		void sendPinValue(pin_t, value_type);
		void sendTimerInfo(timer_t);
		void sendTimerStatus(timer_t);
		void setConnection(Connection*);
		void setTimerStatus(JackTimer&, JackTimer::Action);
		void storeConfig(EEStream&, const Pins&, const Timers&);
		void storeConnection(EEStream&, connection_type, const char*);
		void writePin(pin_t, value_type);

	private:
		/* Built-in commands*/

		Command<void> cmd_devinfo_{ KeyDevInfo, *this, &Jack::cmdDevInfo };	// inf
		Command<void> cmd_devreset_{ KeyDevReset, *this, &Jack::cmdDevReset };	// rst
		Command<void> cmd_getack_{ KeyAcknowledge, *this, &Jack::cmdAcknowledge }; // ack
		Command<pin_t> cmd_getpininfo_{ KeyPinInfo, *this, &Jack::cmdPinInfo }; // pin=p
		Command<void> cmd_getpinsinfo_{ KeyPinInfo, *this, &Jack::cmdPinInfo };	// pin
		Command<pin_t> cmd_getpinmode_{ KeyPinMode, *this, &Jack::cmdPinMode };	// pmd=p
		Command<timer_t, uint8_t> cmd_settimerstatus{ KeyTimerStatus, *this, &Jack::cmdTimerStatus }; // tcs=t,a
		Command<timer_t> cmd_gettimerstatus{ KeyTimerStatus, *this, &Jack::cmdTimerStatus };	// tcs=t
		Command<void> cmd_gettimersstatus{ KeyTimerStatus, *this, &Jack::cmdTimerStatus }; // tcs
		Command<pin_t> cmd_readpin_{ KeyReadPin, *this, &Jack::cmdReadPin };	// rdp=p
		Command<char*> cmd_readpinlist_{ KeyReadPinList, *this, &Jack::cmdReadPinList };	// rdl="p0.p1.p2. ... .pN"
		Command<void> cmd_readpins_{ KeyReadPin, *this, &Jack::cmdReadPin };	// rdp
		Command<bool> cmd_setack_{ KeyAcknowledge, *this, &Jack::cmdAcknowledge };	// ack=[0|1]
		Command<pin_t, uint8_t> cmd_setpinmode_{ KeyPinMode, *this, &Jack::cmdPinMode };	// pmd=p,m
		Command<timer_t, pin_t, uint8_t, uint8_t, uint8_t> cmd_timerattach_{ KeyTimerAttach, *this, &Jack::cmdTimerAttach };	// atc=t,p,m,s,i
		Command<timer_t, pin_t> cmd_timerdetach_{ KeyTimerAttach, *this, &Jack::cmdTimerDetach };	// atc=t,n
		Command<timer_t> cmd_timerinfo_{ KeyTimerAttach, *this, &Jack::cmdTimerInfo };	// atc=t
		Command<void> cmd_timersinfo_{ KeyTimerAttach, *this, &Jack::cmdTimerInfo };	// atc
		Command<pin_t, value_type> cmd_writepin_{ KeyWritePin, *this, &Jack::cmdWritePin };	// wrp=p,v
		Command<uint8_t, const char*, const char*, const char*> cmd_setconnection_{ KeyConnection, *this, &Jack::cmdConnection };	// net=n,a,b,c
		Command<void> cmd_getconnection_{ KeyConnection, *this, &Jack::cmdConnection };	// net
		Command<void> cmd_ldaconfig_{ KeyLoadConfig, *this, &Jack::cmdLoadConfig };	// lda
		Command<void> cmd_stoconfig_{ KeyStoreConfig, *this, &Jack::cmdStoreConfig };	// sto

		/* Private class members */

		Connection*		connection_;	// Current network connection.
		Interpreter		interp_;		// Command interpreter.
		EEStream		eeprom_;		// EEPROM streaming access.
		Pins			pins_;			// Gpio pins collection.
		Timers			timers_;		// Event counter/timers collection.
		Commands		commands_;		// Commands collection.
		Isrs			isrs_;			// Interrupt service routines collection.
		bool			ack_;			// Write command acknowledge flag.
	};
} // namespace pg

// Needed by static isr methods. This must be declared by the client and visible to this translation unit.
extern pg::Jack _jack;	

namespace pg
{
#pragma region public methods

	Jack::Jack(cmdlist_type commands) :
		connection_(), interp_(), eeprom_(), pins_(), timers_(), isrs_(), ack_(), 
		commands_({ &cmd_readpin_ , &cmd_readpins_, &cmd_readpinlist_, &cmd_writepin_ , &cmd_setpinmode_, &cmd_getpinmode_,
			&cmd_timerattach_, &cmd_timerdetach_, &cmd_timerinfo_, &cmd_timersinfo_, &cmd_settimerstatus, 
			&cmd_gettimerstatus, &cmd_gettimersstatus, &cmd_getpininfo_, &cmd_getpinsinfo_,
			&cmd_devinfo_, &cmd_devreset_, &cmd_setack_, &cmd_getack_, &cmd_setconnection_, 
			&cmd_getconnection_, &cmd_ldaconfig_, &cmd_stoconfig_ })
	{
		initialize(pins_);
		initialize(timers_);
		initialize<TimersCount>(isrs_);
		initialize(commands);
	}

	void Jack::clock()
	{
		if (connection_ && connection_->open())
		{
			const char* msg = connection_->receive();

			if (*msg)
				interp_.execute(msg);
		}
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

	void Jack::cmdConnection()
	{
		char params[Connection::size()] = { '\0' };

		sendMessage(FmtConnectionGet, KeyConnection, connection_->type(), connection_->params(params));
	}

	void Jack::cmdConnection(uint8_t type, const char* arg0, const char* arg1, const char* arg2)
	{
		connection_type net_type = static_cast<connection_type>(type);
		char params[Connection::size()] = { '\0' };
		Connection* connection = nullptr;

		// Convert arg0, arg1 and arg2 into a comma-delimited string: "arg0,arg1,arg2".
		(void)std::sprintf(params, FmtConnectionSet, arg0, arg1, arg2); 
		closeConnection(connection_);
		storeConnection(eeprom_, net_type, params);
		connection = openConnection(net_type, params);	// openConnection() expects params as "arg0,arg1,arg2"
		setConnection(connection);
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

	void Jack::cmdLoadConfig()
	{
		loadConfig(eeprom_, pins_, timers_);
	}

	void Jack::cmdStoreConfig()
	{
		storeConfig(eeprom_, pins_, timers_);
	}

	void Jack::cmdPinInfo(pin_t p)
	{
		if (p < GpioCount)
			sendPinInfo(p);
	}

	void Jack::cmdPinInfo()
	{
		for (size_type i = 0; i < pins_.size(); ++i)
			sendPinInfo(i);
	}

	void Jack::cmdPinMode(pin_t p, uint8_t mode)
	{
		if (p < GpioCount && mode < gpio_mode::Invalid)
		{
			pinMode(p, mode);
			pins_[p].mode_ = static_cast<gpio_mode>(mode);
			if (ack_)
				sendPinMode(p);
		}
	}

	void Jack::cmdPinMode(pin_t p)
	{
		if (p < GpioCount)
			sendPinMode(p);
	}

	void Jack::cmdReadPin(pin_t p)
	{
		if (p < GpioCount)
			readPin(p);
	}

	void Jack::cmdReadPin()
	{
		for (size_type i = 0; i < GpioCount; ++i)
			readPin(i);
	}

	void Jack::cmdReadPinList(char* list)
	{
		const char* delim = ".";
		char* p = std::strtok(list, delim);

		while (p)
		{
			readPin(std::atoi(p));
			p = std::strtok(nullptr, delim);
		}
	}

	void Jack::cmdTimerAttach(timer_t t, pin_t p, uint8_t mode, uint8_t trigger, uint8_t timing = timing_mode::Immediate)
	{
		// If t & p are valid, ...
		if (t < TimersCount && pins_[p].int_)
		{
			JackTimer& timer = timers_[t];

			// If timer t attached to another pin, detach it and ... 
			if(timer.pin_ != InvalidPin)
				detachTimer(timer);
			// ... attach timer t to pin p.
			attachInterrupt(digitalPinToInterrupt(p), getIsr(t), static_cast<PinStatus>(trigger));
			attachTimer(timer, p, mode, trigger, timing);
			if (ack_)
				sendTimerInfo(t);
		}
	}

	void Jack::cmdTimerDetach(timer_t t, pin_t)
	{
		if (t < TimersCount)
		{
			detachTimer(timers_[t]);
			if (ack_)
				sendTimerInfo(t);
		}
	}

	void Jack::cmdTimerInfo(timer_t t)
	{
		if (t < TimersCount)
			sendTimerInfo(t);
	}

	void Jack::cmdTimerInfo()
	{
		for (size_type i = 0; i < TimersCount; ++i)
			sendTimerInfo(i);
	}

	void Jack::cmdTimerStatus(timer_t t, uint8_t action)
	{
		if (t < TimersCount)
		{
			setTimerStatus(timers_[t], static_cast<timer_action>(action));
			if (ack_)
				sendTimerStatus(t);
		}
	}

	void Jack::cmdTimerStatus(timer_t t)
	{
		if (t < TimersCount)
			sendTimerStatus(t);
	}

	void Jack::cmdTimerStatus()
	{
		for (size_type i = 0; i < TimersCount; ++i)
			sendTimerStatus(i);
	}

	void Jack::cmdWritePin(pin_t p, value_type value)
	{
		if (p < GpioCount)
			writePin(p, value);
	}

	void Jack::connection(Connection* cn)
	{
		connection_ = cn;
	}

	Connection* Jack::connection() const
	{
		return connection_;
	}

	void Jack::initialize(pin_t p)
	{
		char params_buf[Connection::size()] = { '\0' };
		bool use_dflt = powerOnDefaults(p);

		eeprom_ << EEStream::update(); // Stay Home, Save Lives, Protect the EEPROM.
		//invalidateEeprom(eeprom_); // Uncomment this and upload to reinitialize eeprom. Comment out and upload for normal ops.
		if (use_dflt || !eepromValid(eeprom_, DeviceId))
		{
			connection_ = openConnection(DefaultConnectionType, DefaultConnectionParams);
			if (!use_dflt)
				intializeEeprom(eeprom_, DeviceId, DefaultConnectionType, DefaultConnectionParams, pins_, timers_);
		}
		else
			connection_ = loadConnection(eeprom_, params_buf);
		setConnection(connection_);
	}

	Interpreter& Jack::interpreter()
	{
		return interp_;
	}

	void Jack::isrHandler(timer_t t)
	{
		// This method updates the appropriate event counter/timer 
		// in response to hardware interrupts.

		JackTimer& timer = timers_[t];

		switch (timer.mode_)
		{
		case timer_mode::Counter:
			timer.object_++;
			break;
		case timer_mode::Timer:
			switch (timer.timing_)
			{
			case timing_mode::OneShot:
				if (!timer.object_.active(timer_tag{}))
					timer.object_.start(timer_tag{});
				else
					endTimer(t);
				break;
			case timing_mode::Immediate:
				endTimer(t);
				break;
			case timing_mode::Continuous:
				if (timer.object_.active(timer_tag{}))
					timer.object_.stop(timer_tag{});
				else
					timer.object_.start(timer_tag{});
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

	void Jack::attachTimer(JackTimer& timer, pin_t p, uint8_t mode, uint8_t trigger, uint8_t timing)
	{
		timer.pin_ = p;
		timer.mode_ = static_cast<timer_mode>(mode);
		timer.trigger_ = static_cast<PinStatus>(trigger);
		timer.timing_ = static_cast<timing_mode>(timing);
		switch (timer.mode_)
		{
		case timer_mode::Counter:
			timer.object_.start(counter_tag{});
			break;
		case timer_mode::Timer:
			switch (timer.timing_)
			{
			case timing_mode::Immediate:
				timer.object_.start(timer_tag{});
				break;
			case timing_mode::OneShot:
			case timing_mode::Continuous:
				timer.object_.stop(timer_tag{});
				timer.object_.reset(timer_tag{});
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	void Jack::closeConnection(Connection*& connection)
	{
		if (connection)
		{
			connection->close();
			delete connection;
			connection = nullptr;
			digitalWrite(LedPinNumber, false);
		}
	}

	void Jack::detachTimer(JackTimer& timer)
	{
		timer.object_.stop();
		detachInterrupt(digitalPinToInterrupt(timer.pin_));
		timer.pin_ = InvalidPin;
	}

	bool Jack::eepromValid(EEStream& eeprom, devid_type device_id)
	{
		devid_type eeprom_id = 0;

		eeprom.reset();
		eeprom >> eeprom_id;

		return eeprom_id == device_id;
	}

	void Jack::endTimer(timer_t t)
	{
		JackTimer& timer = timers_[t];

		timer.object_.stop();
		detachTimer(timer);
	}

	Jack::isr_type Jack::getIsr(timer_t n)
	{
		return isrs_[n];
	}

	void Jack::initialize(cmdlist_type& user_cmds)
	{
		// This method appends any user-defined commands to the built-in collection 
		// and passes the collection to the interpreter.

		if (user_cmds.size())
		{
			auto list_end = std::end(commands_);
			auto first = const_cast<command_type**>(user_cmds.begin());
			auto last = const_cast<command_type**>(user_cmds.begin()) + user_cmds.size();

			commands_.resize(commands_.size() + user_cmds.size());
			std::copy(first, last, list_end);
		}
		interp_.commands(std::begin(commands_), std::end(commands_));
	}

	template<typename Jack::size_type I>
	void Jack::initialize(Isrs& isrs)
	{
		// Arduino isrs must be free-standing functions or static methods of 
		// type void that take no arguments and have a unique identifier (name).
		// We define enough static methods to support upto the maximum number of 
		// event timers/counters but only compile enough for the number of pins 
		// that actually support hardware interrupts, then store them in a lookup
		// table (member isrs_). The unused static methods are optimized away by 
		// the compiler, thus saving program memory.

		constexpr size_type i = I - 1;
		isr_type isr = nullptr;

		switch (I)
		{
		case 1:
			isr = &Jack::isrTimer0;
			break;
		case 2:
			isr = &Jack::isrTimer1;
			break;
		case 3:
			isr = &Jack::isrTimer2;
			break;
		case 4:
			isr = &Jack::isrTimer3;
			break;
		case 5:
			isr = &Jack::isrTimer4;
			break;
		case 6:
			isr = &Jack::isrTimer5;
			break;
		case 7:
			isr = &Jack::isrTimer6;
			break;
		case 8:
			isr = &Jack::isrTimer7;
			break;
		case 9:
			isr = &Jack::isrTimer8;
			break;
		case 10:
			isr = &Jack::isrTimer9;
			break;
		case 11:
			isr = &Jack::isrTimer10;
			break;
		case 12:
			isr = &Jack::isrTimer11;
			break;
		case 13:
			isr = &Jack::isrTimer12;
			break;
		case 14:
			isr = &Jack::isrTimer13;
			break;
		case 15:
			isr = &Jack::isrTimer14;
			break;
		case 16:
			isr = &Jack::isrTimer15;
			break;
		default:
			break;
		}
		if(isr)
			isrs[i] = isr;
		if(i)
			initialize<i>(isrs); // Recurse until all used isrs are compiled.
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
			pin.mode_ = i != LedPinNumber ? gpio_mode::Input : digitalPinHasPWM(i) ? gpio_mode::PwmOut : gpio_mode::Input;
			//gpioMode(i, pin.mode_); // doesn't work on megaavr boards
		}
	}

	void Jack::initialize(Timers& timers)
	{
		for (size_type i = 0; i < timers.size(); ++i)
		{
			JackTimer& timer = timers[i];

			timer.pin_ = InvalidPin;
			timer.mode_ = timer_mode::Counter;
			timer.trigger_ = FALLING;
			timer.timing_ = timing_mode::Immediate;
		}
	}

	void Jack::intializeEeprom(EEStream& eeprom, devid_type eeprom_id, connection_type type, const char* params,
		const Pins& pins, const Timers& timers)
	{
		eeprom.reset();
		eeprom << eeprom_id;
		storeConfig(eeprom, pins, timers);
		storeConnection(eeprom, type, params);
	}

	void Jack::invalidateEeprom(EEStream& eeprom)
	{
		eeprom << 42UL;
	}

	void Jack::loadConfig(EEStream& eeprom, Pins& pins, Timers& timers)
	{
		eeprom.address() = ConfigurationEepromAddress;
		for (auto& pin : pins)
		{
			uint8_t tmp = 0;

			eeprom >> tmp;
			pin.mode_ = static_cast<gpio_mode>(tmp);
		}
		for (auto& timer : timers)
		{
			uint8_t tmp = 0;

			eeprom >> timer.pin_;
			eeprom >> tmp;
			timer.mode_ = static_cast<timer_mode>(tmp);
			eeprom >> tmp;
			timer.trigger_ = static_cast<PinStatus>(tmp);
			eeprom >> tmp;
			timer.timing_ = static_cast<timing_mode>(tmp);
		}
	}

	Connection* Jack::loadConnection(EEStream& eeprom, char* params)
	{
		uint8_t type = 0;

		eeprom.address() = ConnectionEepromAddress;
		eeprom >> type;
		eeprom >> params;

		return openConnection(static_cast<connection_type>(type), params);
	}

	Connection* Jack::openConnection(connection_type type, const char* params)
	{
		Connection* connection = nullptr;

		switch (type)
		{
		case connection_type::Serial:
			connection = new pg::SerialConnection(Serial, params);
			break;
		case connection_type::WiFi:
			connection = new pg::WiFiConnection(params);
			break;
		case connection_type::Ethernet:
			connection = new pg::EthernetConnection(params);
		default:
			break;
		}

		return connection;
	}

	bool Jack::powerOnDefaults(pin_t pin)
	{
		gpio_mode pin_mode = _jack.pins()[pin].mode_;
		bool result = false;

		pinMode(pin, gpio_mode::Pullup);
		if (digitalRead(pin) == false)
			result = true;
		pinMode(pin, pin_mode);

		return result;
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

	template<class... Ts>
	void Jack::sendMessage(const char* fmt, Ts... args)
	{
		char msg[Connection::size()] = { '\0' };

		std::sprintf(msg, fmt, args...);
		connection_->send(msg);
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
		const JackTimer& timer = timers_[n];

		sendMessage(FmtTimerAttach, KeyTimerAttach, n, timer.pin_, timer.mode_, timer.trigger_, timer.timing_);
	}

	void Jack::sendTimerStatus(timer_t n)
	{
		JackTimer& timer = timers_[n];
		bool active = false;
		unsigned long value = 0;

		switch (timer.mode_)
		{
		case timer_mode::Counter:
			active = timer.object_.active(counter_tag{});
			value = timer.object_.count();
			break;
		case timer_mode::Timer:
			active = timer.object_.active(timer_tag{});
			value = timer.object_.elapsed().count();
			break;
		default:
			break;
		}
		sendMessage(FmtTimerStatus, KeyTimerStatus, n, active, value);
	}

	void Jack::setConnection(Connection* connection)
	{
		connection_ = connection;
		digitalWrite(pg::LedPinNumber, connection->open());
	}

	void Jack::setTimerStatus(JackTimer& timer, JackTimer::Action action)
	{
		switch (timer.mode_)
		{
		case timer_mode::Counter:
			switch (action)
			{
			case timer_action::Start:
				timer.object_.start(counter_tag{});
				break;
			case timer_action::Stop:
				timer.object_.stop(counter_tag{});
				break;
			case timer_action::Resume:
				timer.object_.resume(counter_tag{});
				break;
			case timer_action::Reset:
				timer.object_.reset(counter_tag{});
				break;
			default:
				break;
			}
			break;
		case timer_mode::Timer:
			switch (action)
			{
			case timer_action::Start:
				timer.object_.start(timer_tag{});
				break;
			case timer_action::Stop:
				timer.object_.stop(timer_tag{});
				break;
			case timer_action::Resume:
				timer.object_.resume(timer_tag{});
				break;
			case timer_action::Reset:
				timer.object_.reset(timer_tag{});
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	void Jack::storeConfig(EEStream& eeprom, const Jack::Pins& pins, const Jack::Timers& timers)
	{
		eeprom.address() = ConfigurationEepromAddress;
		for (auto& pin : pins)
			eeprom << static_cast<uint8_t>(pin.mode_);
		for (auto& timer : timers)
		{
			eeprom << timer.pin_;
			eeprom << static_cast<uint8_t>(timer.mode_);
			eeprom << static_cast<uint8_t>(timer.trigger_);
			eeprom << static_cast<uint8_t>(timer.timing_);
		}
	}

	void Jack::storeConnection(EEStream& eeprom, connection_type type, const char* params)
	{
		eeprom.address() = ConnectionEepromAddress;
		eeprom << static_cast<uint8_t>(type);
		eeprom << params;
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
	void Jack::isrTimer4() { _jack.isrHandler(4); }
	void Jack::isrTimer5() { _jack.isrHandler(5); }
	void Jack::isrTimer6() { _jack.isrHandler(6); }
	void Jack::isrTimer7() { _jack.isrHandler(7); }
	void Jack::isrTimer8() { _jack.isrHandler(8); }
	void Jack::isrTimer9() { _jack.isrHandler(9); }
	void Jack::isrTimer10() { _jack.isrHandler(10); }
	void Jack::isrTimer11() { _jack.isrHandler(11); }
	void Jack::isrTimer12() { _jack.isrHandler(12); }
	void Jack::isrTimer13() { _jack.isrHandler(13); }
	void Jack::isrTimer14() { _jack.isrHandler(14); }
	void Jack::isrTimer15() { _jack.isrHandler(15); }

#pragma endregion

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 
#endif // !defined __PG_JACK_H
