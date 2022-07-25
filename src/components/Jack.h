/*
 *	This files defines a remote control and data acquisition class.
 *
 *	***************************************************************************
 *
 *	File: Jack.h
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
 *	Description:
 * 
 *	The `Jack' class allows clients to operate a device remotely, over a 
 *	network connection, using a simple messaging protocol. Jack also supports 
 *	EEPROM storage and recall of its current connection and configuration 
 *	information. Network communications are handled by the `Connection' type 
 *	(see <Connection.h>); messaging and command protocols are defined by the 
 *	`Interpreter' type, which also handles message decoding and command 
 *	execution (see <Interpreter.h>). On supported devices, Jack can store and 
 *	execute programs locally, on the device itself. 
 *
 *	Jack defines a collection of built-in command objects that, when executed, 
 *	call one of the member methods declared by its public interface (see 
 *	below). Command objects are defined by a unique key string, parameter list 
 *	and delegate, which can be ether a free-standing function or object method.
 *	Jack listens to a network connection and forwards any messages received 
 *	to the `Interpreter' which decodes and executes any valid messages. Any 
 *	parameters encoded in the message are passed as arguments to the delegate.
 *	Jack also supports user-defined command objects that can be appended to the 
 *	built-in collection. This allows developers to expand an application’s 
 *	capabilities with custom function/method calls.
 *
 *	Messages are plain text strings that have the general form: 
 *
 *		key[=arg0,arg1,…,argN], 
 *
 *	where key is the command identifier (name) optionally followed by a comma-
 *	separated list of arguments (values) separated by an equal sign (=). 
 *	Messages are terminated with a newline character ‘\n’. 
 *	
 *	To reduce network traffic and execution time, Jack does not implement any 
 *	message handshaking or transactions, other than acknowledging receipt of 
 *	"write" messages (those which normally do not send a reply). This can be 
 *	enabled/disabled by clients. Otherwise clients must implement any message 
 *	error detection/correction schemes, including retransmitting messages where 
 *	a reply was expected but not received.
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
 *	erratic behavior. See `invalidateEeprom()' in the initialize() function. 
 *	Enable the __PG_FORMAT_EEPROM conditional, compile and upload the program. 
 *	Once intitialized, the program should be recompiled and uploaded with the 
 *	conditional disabled. If not, the eeprom memory will be overwritten each 
 *	time the device is reset or	power-cycled and lose any saved information.
 * 
 *****************************************************************************/

#if !defined __PG_JACK_H
# define __PG_JACK_H 20220725L

# include <cassert>
# include <cstdio>
# include <cstring>
# include <valarray>
# include <system/boards.h>
# include <system/utils.h>
# include <interfaces/iclockable.h>
# include <interfaces/icomponent.h>
# include <utilities/EEStream.h>
# include <utilities/Timer.h>
# include <utilities/Interpreter.h>
# include <utilities/Connection.h>
# if !defined __PG_NO_PROGRAM
#  include <utilities/Program.h>	
# endif
# if !defined __PG_NO_CHECKSUM 
#  include <lib/crc.h>
# endif

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	using std::chrono::milliseconds;

	class Jack;
	static Jack* __jack_isr = nullptr; // External pointer to Jack instance used by static isr methods.

	// Remote control and data acquisition class.
# if defined __PG_PROGRAM_H
	class Jack : public iclockable, public icomponent, public iprogram 
# else 
	class Jack : public iclockable, public icomponent
# endif
	{
	public:
		template<class... Ts>											// Jack command template.
		using Command = typename Interpreter::Command<void, Jack, Ts...>;
		template<class Ret, class Obj = void, class... Ts>
		using UsrCommand = Interpreter::Command <Ret, Obj, Ts...>;		// Client command template.
		using address_type = EEStream::address_type;					// EEPROM address storage type.
		using connection_type = Connection::Type;						// Network connection storage type.
		using devid_type = unsigned long;								// Device id storage type.
		using fmt_type = const char*;									// Reply message format specifier type.
		using isr_type = pg::callback<void>::type;						// Isr function call type.
		using command_type = Interpreter::CommandBase;					// Remote command interface type.
		using cmdlist_type = std::initializer_list<command_type*>;		// Remote command interface list type.
		using key_type = typename command_type::key_type;				// Command key storage type.
		using size_type = uint8_t;										// Type that can hold the size of any Jack object.
		using value_type = uint16_t;									// Type that can hold any pin state.
		using ct_type = CounterTimer<milliseconds, value_type>;			// Event counter/timer type.
		using counter_tag = ct_type::counter_tag;						// Tag type for dispatching counter methods.
		using timer_tag = ct_type::timer_tag;							// Tag type for dispatching timer methods.
		using timer_t = uint8_t;										// Timer index type alias.

# if defined __PG_PROGRAM_H
		static constexpr size_type CommandsMaxCount = 64;				// Maximum number of storable remote commands.
# elif defined __PG_NO_USR_COMMANDS 
		static constexpr size_type CommandsMaxCount = 31;				// Maximum number of storable remote commands.
# else
		static constexpr size_type CommandsMaxCount = 40;				// Maximum number of storable remote commands.
# endif
		static constexpr size_type TimersMaxCount = 16;					// Maximum number of event counters/timers.
		static constexpr size_type InterruptsCount =					// Number of pins with hardware interrupts.
			countInterrupts<GpioCount>();
		static constexpr size_type TimersCount = 						// Number of instantiated event counters/timers, 
			InterruptsCount > TimersMaxCount							// TimersMinCount <= TimersCount <= TimersMaxCount.
			? TimersMaxCount
			: InterruptsCount;
		static constexpr devid_type DeviceId = 20220430UL;				// Value used to validate eeprom.
		static constexpr pin_t PowerOnDefaultsPin = 2;					// Power-on/reset defaults pin.
		static constexpr connection_type DefaultConnectionType = connection_type::Serial;
		static constexpr const char* DefaultConnectionParams = "9600,8N1";
		static constexpr const char* ListDelimiterChars = ".";			// Delimiter char for lists encoded as a single Command arg.
		static constexpr const char* CheckSumDelimiterChars = ":";		// Delimiter char for check values appended to messages.
		static constexpr size_type ListSize = GpioCount > TimersCount
			? GpioCount
			: TimersCount;

		struct GpioPin;													// Forward decl (see below).
		struct TimerCounter;											// Forward decl (see below).
		using Commands = typename std::valarray<command_type*, CommandsMaxCount>;	// Remote commands collection type.
		using Timers = typename std::array<TimerCounter, TimersCount>;	// Event counters/timers collection type.
		using Pins = std::array<GpioPin, GpioCount>;					// GpioPins collection type.
		using Isrs = std::array<isr_type, TimersCount>;					// ISRs collection type.
		using List = std::valarray<uint8_t, ListSize>;					// Type that holds Command argument lists.

		// Aggregates information about an event counter/timer. 
		struct TimerCounter
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
				Stop = 0,		// Starts the counter/timer.
				Start = 1,		// Stops the counter/timer.
				Resume = 2,		// Resumes the counter/timer with the current count/elapsed time.
				Reset = 3		// Resets the current count/elapsed time.
			};

			// Enumerates counter/timer timing modes. 
			enum Timing : uint8_t
			{
				Continuous = 0,	// Timer is toggled on each trigger event.
				OneShot = 1,	// Timer starts then stops when triggered once.
			};

			pin_t			pin_;		// The currently attached pin, if any.
			Mode			mode_;		// The current operating mode.
			PinStatus		trigger_;	// The current event trigger.
			Timing			timing_;	// The current timing mode.
			bool			enabled_;	// Flag used to disable one-shot timer after trigger.
			bool			instant_;	// Flag indicating whether to report the instantaneous or triggered value;
			uint32_t		value_;		// The current timer reporting value.
			ct_type			object_;	// The counter/timer object.

			bool isAttached() const { return pin_ != InvalidPin; }
		};

		using timer_action = TimerCounter::Action;
		using timer_mode = TimerCounter::Mode;
		using timing_mode = TimerCounter::Timing;

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
				Reserved = 4	// Reserved for system use.
			};

			Type			type_;		// Pin i/o type.
			Mode			mode_;		// Pin i/o mode.
			bool			int_;		// Pin has hardware interrupt.

			// Checks whether the pin is currently configured as an input.
			bool isInput() const { return mode_ == Mode::Input || mode_ == Mode::Pullup; }
			// Checks whether the pin is currently configured as an output.
			bool isOutput() const { return mode_ == Mode::Output || mode_ == Mode::PwmOut; }
			// Checks whether the pin is an analog input.
			bool isAnalog() const { return type_ == Type::Analog; }
			// Checks whether the pin is a digital input/ouput.
			bool isDigital() const { return !isAnalog(); } 
			// Checks whether the pin has interrupt capability.
			bool hasInterrupt() const { return int_; }
			// Checks whether the pin is available as a gpio pin.
			bool isAvailable() const { return mode_ < Mode::Reserved; }
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
			(GpioCount * sizeof(decltype(GpioPin::mode_))) +							// Pins Config saves mode_
			Jack::TimersCount * (sizeof(decltype(TimerCounter::pin_)) +					// Timers Config saves pin_
				sizeof(decltype(TimerCounter::mode_)) +									// mode_
				sizeof(decltype(TimerCounter::trigger_)) +								// trigger_ and 
				sizeof(decltype(TimerCounter::timing_)) +								// operation
				sizeof(decltype(TimerCounter::instant_)));								// instant

#pragma region strings

		/*
		 *  Command/instruction keys and reply string format specifiers.
		 */

		static constexpr key_type KeyDevReset = "rst";			// Device reset:					rst
		static constexpr key_type KeyDevInfo = "inf";			// Device info:						inf
		static constexpr key_type KeyGetAck = "ack";			// Get write acknowledge:			ack
		static constexpr key_type KeySetAck = "sck";			// Set write acknowledge:			sck=0|1
		static constexpr key_type KeyGetPinInfo = "pin";		// Get pin info:					pin=p
		static constexpr key_type KeyGetPinInfoAll = "pna";		// Get all pins info:				pna
		static constexpr key_type KeyGetPinMode = "pmd";		// Get pin mode:					pmd=p
		static constexpr key_type KeyGetPinModeAll = "pma";		// Get all pins mode:				pma
		static constexpr key_type KeyGetPinModeList = "pml";	// Get pin list mode:				pml=p0[.p1. ... .pN]
		static constexpr key_type KeySetPinMode = "spm";		// Set pin mode:					spm=p,m
		static constexpr key_type KeySetPinModeAll = "spa";		// Set all pins mode:				spa=m
		static constexpr key_type KeyGetElapsedTime = "tim";	// Get system elapsed time:			tim=0|1
		static constexpr key_type KeyReadPin = "rdp";			// Read pin:						rdp=p
		static constexpr key_type KeyReadPinAll = "rda";		// Read all pins:					rda
		static constexpr key_type KeyReadPinList = "rdl";		// Read pin list:					rdl=p0[.p1. ... .pN]
		static constexpr key_type KeyWritePin = "wrp";			// Write pin:						wrp=p,v
		static constexpr key_type KeyGetTimerStatus = "tms";	// Get timer state:					tms=t
		static constexpr key_type KeyGetTimerStatusAll = "tma";	// Get all timers state:			tma
		static constexpr key_type KeyGetTimerStatusList = "tml";// Get timer list state:			tml=t0[.t1. ... .tN]
		static constexpr key_type KeySetTimerStatus = "stm";	// Set timer state:					stm=t,s
		static constexpr key_type KeyTimerDetach = "dtc";		// Detach timer:					dtc=t
		static constexpr key_type KeyTimerGetAttach = "tcm";	// Get timer attach info:			tcm=t
		static constexpr key_type KeyTimerGetAttachAll = "tca";	// Get all timers attach info:		tca
		static constexpr key_type KeyTimerGetAttachList = "tcl";// Get timer list attach info:		tcl=t0[.t1. ... .tN]
		static constexpr key_type KeyTimerAttach = "atc";		// Attach timer to pin.				atc=t,p,m,e,n
		static constexpr key_type KeyGetConnection = "net";		// Get network connection:			net
		static constexpr key_type KeySetConnection = "snt";		// Set network connection:			snt=c,arg0,arg1,arg2
		static constexpr key_type KeyLoadConfig = "lda";		// Load device config from eeprom:	lda
		static constexpr key_type KeyStoreConfig = "sto";		// Store device config to eeprom:	sto
		static constexpr key_type KeySetTimerStatusAll = "sta";	// Set all timers state:			sta=s
		static constexpr key_type KeyTimerDetachAll = "dta";	// Detach all timers:				dta

		static constexpr fmt_type FmtAcknowledge = "%s=%u";				// ack=0|1
		static constexpr fmt_type FmtConnectionGet = "%s=%u,%s";		// net=type,arg0,arg1,arg2
		static constexpr fmt_type FmtConnectionSet = "%s,%s,%s";		// "arg0,arg1,arg2"
		static constexpr fmt_type FmtDevInfo = "%s=%lu,%s,%s,%u,%u,%u";	// inf=id,type,mcutype,mcuspd,#pins,#timers
		static constexpr fmt_type FmtElapsedTime = "%s=%u,%lu";			// tim=unit,time
		static constexpr fmt_type FmtPinInfo = "%s=%u,%u,%u,%u";		// pin=p#,type,int,mode
		static constexpr fmt_type FmtPinMode = "%s=%u,%u";				// pmd=p#,mode
		static constexpr fmt_type FmtReadPin = "%u=%u";					// p#=value
		static constexpr fmt_type FmtTimerAttach = "%s=%u,%u,%u,%u,%u,%u";	// atc=t#,p#,mode,trigger,timing
		static constexpr fmt_type FmtTimerStatus = "%s=%u,%u,%lu";		// tms=t#,active,value
		static constexpr fmt_type FmtChecksum = ":%u";					// Message check value. 
# if defined __PG_PROGRAM_H
		static constexpr key_type KeyProgram = "pgm";					// Get/set program state:	pgm=a
		static constexpr fmt_type FmtProgramStatus = "%s=%u,%u";		// pgm=action,status
																		// #pin value, %timer elapsed, +counter count, 
																		// *tc active, $time elapsed.
# endif

#pragma endregion
#pragma region interrupt service routines

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
		// Constructs a Jack object with an optional list of client-defined commands.
		explicit Jack(cmdlist_type = cmdlist_type());

	public:
		void clock() override;
		void cmdAckGet();
		void cmdAckSet(bool);
		void cmdConnectionGet();
		void cmdConnectionSet(uint8_t, const char*, const char*, const char*);
		void cmdDevInfo();
		void cmdDevReset();
		void cmdElapsedTime(uint8_t);
		void cmdLoadConfig();
		void cmdPinInfoGet(pin_t);
		void cmdPinInfoGetAll();
		void cmdPinModeGet(pin_t);
		void cmdPinModeGetAll();
		void cmdPinModeGetList(char*);
		void cmdPinModeSet(pin_t, uint8_t);
		void cmdPinModeSetAll(uint8_t);
		void cmdReadPin(pin_t);
		void cmdReadPinAll();
		void cmdReadPinList(char*);
		void cmdStoreConfig();
		void cmdTimerAttachGet(timer_t);
		void cmdTimerAttachGetAll();
		void cmdTimerAttachGetList(char*);
		void cmdTimerAttachSet(timer_t, pin_t, uint8_t, uint8_t, uint8_t, bool);
		void cmdTimerDetach(timer_t);
		void cmdTimerDetachAll();
		void cmdTimerStatusGet(timer_t);
		void cmdTimerStatusGetAll();
		void cmdTimerStatusGetList(char*);
		void cmdTimerStatusSet(timer_t, uint8_t);
		void cmdTimerStatusSetAll(uint8_t);
		void cmdWritePin(pin_t, value_type);
		Commands commands() const;
		Connection* connection() const;
		bool exec(const char*);
		void initialize(pin_t = PowerOnDefaultsPin);
		void isrHandler(timer_t);

	private:
		template<class ForwardIt>
		void addCommands(Commands&, ForwardIt, ForwardIt);
		void attachTimer(timer_t, pin_t, uint8_t, uint8_t, uint8_t, bool);
		bool check(char*);
		void closeConnection(Connection*&);
		void detachTimer(timer_t, bool = true);
		std::time_t elapsedTime(uint8_t);
		void endTimer(timer_t);
		bool eepromValid(EEStream&, devid_type);
		template<class... Ts>
		size_type fmtMessage(char*, const char*, Ts...);
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
# if !defined __PG_NO_ETHERNET_DHCP
		void maintainConnection();
# endif
		void makeList(char*, uint8_t);
		Connection* openConnection(connection_type, const char*);
		bool powerOnDefaults(pin_t);
		value_type readPin(pin_t);
		template<class... Ts>
		void sendMessage(const char*, Ts...);
		void sendPinInfo(pin_t);
		void sendPinMode(pin_t);
		void sendPinValue(pin_t, value_type);
		void sendTimerInfo(timer_t);
		void sendTimerStatus(timer_t);
		void setConnection(Connection*);
		void setPinMode(pin_t, uint8_t);
		void setTimerStatus(timer_t, TimerCounter::Action);
		void storeConfig(EEStream&, const Pins&, const Timers&);
		void storeConnection(EEStream&, connection_type, const char*);
		void writePin(pin_t, value_type);
# if defined __PG_PROGRAM_H
		void list();
		void process(const char*);
		void program(uint8_t);
		void sendProgramStatus(Program::Action, value_type);
		iprogram::value_type sys_get(const char*) override;
		void sys_set(const char*, iprogram::value_type) override;
		bool verify();
# endif
	private: 
		Command<void> cmd_devinfo_{ KeyDevInfo, *this, &Jack::cmdDevInfo };	// inf
		Command<void> cmd_devreset_{ KeyDevReset, *this, &Jack::cmdDevReset };	// rst
		Command<void> cmd_ackget_{ KeyGetAck, *this, &Jack::cmdAckGet }; // ack
		Command<bool> cmd_ackset_{ KeySetAck, *this, &Jack::cmdAckSet };	// sck=[0|1]
		Command<uint8_t> cmd_elapsed_{ KeyGetElapsedTime,*this,&Jack::cmdElapsedTime };	// rde=0|1
		Command<pin_t> cmd_pininfoget_{ KeyGetPinInfo, *this, &Jack::cmdPinInfoGet }; // pin=p
		Command<void> cmd_pininfogetall_{ KeyGetPinInfoAll, *this, &Jack::cmdPinInfoGetAll };	// pna
		Command<pin_t> cmd_pinmodeget_{ KeyGetPinMode, *this, &Jack::cmdPinModeGet };	// pmd=p
		Command<void> cmd_pinmodegetall_{ KeyGetPinModeAll, *this, &Jack::cmdPinModeGetAll };	// pma
		Command<char*> cmd_pinmodegetlist_{ KeyGetPinModeList, *this, &Jack::cmdPinModeGetList };	// pml
		Command<pin_t, uint8_t> cmd_pinmodeset_{ KeySetPinMode, *this, &Jack::cmdPinModeSet };	// spm=p,m
		Command<uint8_t> cmd_pinmodesetall_{ KeySetPinModeAll, *this, &Jack::cmdPinModeSetAll };	// spa=m
		Command<timer_t> cmd_timerstatusget_{ KeyGetTimerStatus, *this, &Jack::cmdTimerStatusGet };	// tms=t
		Command<void> cmd_timerstatusgetall_{ KeyGetTimerStatusAll, *this, &Jack::cmdTimerStatusGetAll }; // tma
		Command<char*> cmd_timerstatusgetlist_{ KeyGetTimerStatusList, *this, &Jack::cmdTimerStatusGetList }; // tml
		Command<timer_t, uint8_t> cmd_timerstatusset_{ KeySetTimerStatus, *this, &Jack::cmdTimerStatusSet }; // stm=t,a
		Command<pin_t> cmd_readpin_{ KeyReadPin, *this, &Jack::cmdReadPin };	// rdp=p
		Command<void> cmd_readpinall_{ KeyReadPinAll, *this, &Jack::cmdReadPinAll };	// rda
		Command<char*> cmd_readpinlist_{ KeyReadPinList, *this, &Jack::cmdReadPinList };	// rdl="p0.p1.p2. ... .pN"
		Command<timer_t> cmd_timerattachget_{ KeyTimerGetAttach, *this, &Jack::cmdTimerAttachGet };	// tcm=t
		Command<void> cmd_timerattachgetall_{ KeyTimerGetAttachAll, *this, &Jack::cmdTimerAttachGetAll };	// tca
		Command<char*> cmd_timerattachgetlist_{ KeyTimerGetAttachList, *this, &Jack::cmdTimerAttachGetList };	// tcl
		Command<timer_t, pin_t, uint8_t, uint8_t, uint8_t, bool> cmd_timerattachset_{ KeyTimerAttach, *this, &Jack::cmdTimerAttachSet };	// atc=t,p,m,s,o,i
		Command<timer_t> cmd_timerdetach_{ KeyTimerDetach, *this, &Jack::cmdTimerDetach };	// dtc=t
		Command<pin_t, value_type> cmd_writepin_{ KeyWritePin, *this, &Jack::cmdWritePin };	// wrp=p,v
		Command<void> cmd_connectionget_{ KeyGetConnection, *this, &Jack::cmdConnectionGet };	// net
		Command<uint8_t, const char*, const char*, const char*> cmd_connectionset_{ KeySetConnection, *this, &Jack::cmdConnectionSet };	// snt=n,a,b,c
		Command<void> cmd_ldaconfig_{ KeyLoadConfig, *this, &Jack::cmdLoadConfig };	// lda
		Command<void> cmd_stoconfig_{ KeyStoreConfig, *this, &Jack::cmdStoreConfig };	// sto
		Command<void> cmd_timerdetachall_{ KeyTimerDetachAll, *this, &Jack::cmdTimerDetachAll };	// dta
		Command<uint8_t> cmd_timerstatussetall_{ KeySetTimerStatusAll, *this, &Jack::cmdTimerStatusSetAll }; // sta=t,a

		Connection*		connection_;	// Current network connection.
		Interpreter		interp_;		// Command interpreter.
		EEStream		eeprom_;		// EEPROM streaming object.
		Pins			pins_;			// Gpio pins collection.
		Timers			timers_;		// Event counters/timers collection.
		Commands		commands_;		// Remote commands collection.
		Isrs			isrs_;			// Interrupt service routines collection.
		bool			ack_;			// Command acknowledge flag.
		List			list_;			// Command argument list buffer.
# if defined __PG_PROGRAM_H
		Command<uint8_t> cmd_program_{ KeyProgram, *this, &Jack::program };	// Program command object.
		Program			program_;		// Program manager/executor.
# endif
# if defined __PG_CRC_H 
		bool checksum_;					// Flag indicating whether a received message included a checksum.
#endif
	};
} // namespace pg

namespace pg
{
#pragma region public methods

# if defined __PG_PROGRAM_H
	Jack::Jack(cmdlist_type commands) :
		connection_(), interp_(), eeprom_(), pins_(), timers_(), isrs_(), ack_(), program_(*this), list_(),
		commands_({ & cmd_devinfo_ , & cmd_devreset_, & cmd_ackget_, & cmd_ackset_, & cmd_pininfoget_, & cmd_pininfogetall_,
			& cmd_pinmodeget_, & cmd_pinmodegetall_, & cmd_pinmodeset_, & cmd_pinmodesetall_, & cmd_timerstatusget_,
			& cmd_timerstatusgetall_, & cmd_timerstatusset_, & cmd_timerstatussetall_, & cmd_readpin_, & cmd_readpinall_, 
			& cmd_readpinlist_, & cmd_timerattachget_, & cmd_timerattachgetall_, & cmd_timerattachset_, & cmd_timerdetach_,
			& cmd_timerdetachall_, & cmd_connectionget_, & cmd_connectionset_, & cmd_ldaconfig_, & cmd_stoconfig_, 
			& cmd_elapsed_,& cmd_writepin_, & cmd_program_, & cmd_pinmodegetlist_, & cmd_timerstatusgetlist_, 
			& cmd_timerattachgetlist_ })
# else
	Jack::Jack(cmdlist_type commands) :
		connection_(), interp_(), eeprom_(), pins_(), timers_(), isrs_(), ack_(), list_(), 
		commands_({ &cmd_devinfo_ , &cmd_devreset_, &cmd_ackget_, &cmd_ackset_, &cmd_pininfoget_, &cmd_pininfogetall_,
			&cmd_pinmodeget_, &cmd_pinmodegetall_, &cmd_pinmodeset_, &cmd_pinmodesetall_, &cmd_timerstatusget_,
			&cmd_timerstatusgetall_, &cmd_timerstatusset_, &cmd_timerstatussetall_, &cmd_readpin_, &cmd_readpinall_,
			&cmd_readpinlist_, &cmd_timerattachget_, &cmd_timerattachgetall_, &cmd_timerattachset_, &cmd_timerdetach_,
			&cmd_timerdetachall_, &cmd_connectionget_, &cmd_connectionset_, &cmd_ldaconfig_, &cmd_stoconfig_,
			&cmd_elapsed_, & cmd_writepin_,& cmd_pinmodegetlist_,& cmd_timerstatusgetlist_,	& cmd_timerattachgetlist_ })
# endif
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
			const char* msg;

			connection_->clock();
			while(*(msg = connection_->receive()))
# if defined __PG_CRC_H
				if(check(const_cast<char*>(msg))) // Look for trailing check value and compare to checksum.
# endif
# if defined __PG_PROGRAM_H
					process(msg);		// Check for program control messages before executing command.
				if (program_.active())	// If current program is active, execute next instruction.
				{
					const char* instruction = program_.instruction();
					// Programs can execute both remote commands and program instructions.
					interp_.execute(std::begin(commands_), std::end(commands_), instruction) || 
						interp_.execute(std::begin(program_.instructions()), std::end(program_.instructions()), instruction);
				}
# else 
					interp_.execute(std::begin(commands_), std::end(commands_), msg);
# endif
# if !defined __PG_NO_ETHERNET_DHCP
			maintainConnection();		// Maintain ethernet dhcp lease.
# endif
		}
	}

	bool Jack::exec(const char* command)
	{
		return interp_.execute(std::begin(commands_), std::end(commands_), command);
	}

	void Jack::cmdAckGet()
	{
		sendMessage(FmtAcknowledge, KeyGetAck, ack_);
	}

	void Jack::cmdAckSet(bool value)
	{
		if ((ack_ = value))
			sendMessage(FmtAcknowledge, KeyGetAck, ack_);
	}

	void Jack::cmdConnectionGet()
	{
		char params[Connection::size()] = { '\0' };

		sendMessage(FmtConnectionGet, KeyGetConnection, connection_->type(), connection_->params(params));
	}

	void Jack::cmdConnectionSet(uint8_t type, const char* arg0, const char* arg1, const char* arg2)
	{
		connection_type net_type = static_cast<connection_type>(type);
		char params[Connection::size()] = { '\0' };
		Connection* connection = nullptr;

		// Convert arg0, arg1 and arg2 into a comma-delimited string: "arg0,arg1,arg2".
		(void)fmtMessage(params, FmtConnectionSet, arg0, arg1, arg2);
		sendMessage(FmtConnectionGet, KeySetConnection, type, params);
		closeConnection(connection_);
		setConnection(nullptr);
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

	void Jack::cmdElapsedTime(uint8_t s)
	{
		sendMessage(FmtElapsedTime, KeyGetElapsedTime, s, elapsedTime(s));
	}

	void Jack::cmdLoadConfig()
	{
		loadConfig(eeprom_, pins_, timers_);
	}

	void Jack::cmdStoreConfig()
	{
		storeConfig(eeprom_, pins_, timers_);
	}

	void Jack::cmdPinInfoGet(pin_t p)
	{
		if (p < GpioCount)
			sendPinInfo(p);
	}

	void Jack::cmdPinInfoGetAll()
	{
		for (size_type p = 0; p < pins_.size(); ++p)
			sendPinInfo(p);
	}

	void Jack::cmdPinModeGet(pin_t p)
	{
		if (p < GpioCount)
			sendPinMode(p);
	}

	void Jack::cmdPinModeGetAll()
	{
		for(size_type p = 0; p < GpioCount; ++p)
			sendPinMode(p);
	}

	void Jack::cmdPinModeGetList(char* list)
	{
		makeList(list, GpioCount);
		for (auto p : list_)
			if (p < GpioCount)
				sendPinMode(p);
	}

	void Jack::cmdPinModeSet(pin_t p, uint8_t mode)
	{
		if (p < GpioCount && mode < gpio_mode::Reserved)
			setPinMode(p, mode);
	}

	void Jack::cmdPinModeSetAll(uint8_t mode)
	{
		if(mode < gpio_mode::Reserved)
			for (size_type p = 0; p < pins_.size(); ++p)
				setPinMode(p, mode);
	}

	void Jack::cmdReadPin(pin_t p)
	{
		if (p < GpioCount)
			sendPinValue(p, readPin(p));
	}

	void Jack::cmdReadPinAll()
	{
		for (size_type p = 0; p < GpioCount; ++p)
			sendPinValue(p, readPin(p));
	}

	void Jack::cmdReadPinList(char* list)
	{
		makeList(list, GpioCount);
		for (auto p : list_)
			if (p < GpioCount)
				sendPinValue(p, readPin(p));
	}

	void Jack::cmdTimerAttachGet(timer_t t)
	{
		if (t < TimersCount)
			sendTimerInfo(t);
	}

	void Jack::cmdTimerAttachGetAll()
	{
		for (size_type t = 0; t < TimersCount; ++t)
			sendTimerInfo(t);
	}

	void Jack::cmdTimerAttachGetList(char* list)
	{
		makeList(list, TimersCount);
		for (auto t : list_)
			if (t < TimersCount)
				sendTimerInfo(t);
	}

	void Jack::cmdTimerAttachSet(timer_t t, pin_t p, uint8_t mode, uint8_t trigger, uint8_t timing, bool instant)
	{
		// Hardware interrupts have frequency limits. Triggering interrupts faster than they can be 
		// handled will cause them to become unresponsive.
		
		// If t & p are valid, ...
		if (t < TimersCount && ((p < GpioCount && pins_[p].isAvailable() && pins_[p].hasInterrupt()) || p == InvalidPin))
		{
			// If timer attached to another pin, detach it first.
			if (timers_[t].isAttached())
				detachTimer(t, false);
			// If not manual operation, detach any other timer attached to same pin and attach to pin to interrupt.
			if (p != InvalidPin)
			{
				for(size_type i = 0; i < TimersCount; ++i)
					if (timers_[i].pin_ == p)
						detachTimer(i, false);
				// Using trigger LOW and floating input may cause attachInterrupt() to not return, so set as pullup.
				if (trigger == LOW && pins_[p].mode_ != gpio_mode::Pullup)
					setPinMode(p, gpio_mode::Pullup);
				attachInterrupt(digitalPinToInterrupt(p), getIsr(t), static_cast<PinStatus>(trigger));
			}
			// Attach timer to pin, p == InvalidPin allows user to operate timer manually with commands.
			attachTimer(t, p, mode, trigger, timing, instant);
		}
	}

	void Jack::cmdTimerDetach(timer_t t)
	{
		if (t < TimersCount)
			detachTimer(t);
	}

	void Jack::cmdTimerDetachAll()
	{
		for (size_type t = 0; t < TimersCount; ++t)
			detachTimer(t);
	}

	void Jack::cmdTimerStatusGet(timer_t t)
	{
		if (t < TimersCount)
			sendTimerStatus(t);
	}

	void Jack::cmdTimerStatusGetList(char* list)
	{
		makeList(list, TimersCount);
		for (auto t : list_)
			if (t < TimersCount)
				sendTimerStatus(t);
	}

	void Jack::cmdTimerStatusGetAll()
	{
		for (size_type t = 0; t < TimersCount; ++t)
			sendTimerStatus(t);
	}

	void Jack::cmdTimerStatusSet(timer_t t, uint8_t action)
	{
		if (t < TimersCount)
			setTimerStatus(t, static_cast<timer_action>(action));
	}

	void Jack::cmdTimerStatusSetAll(uint8_t action)
	{
		for (size_type t = 0; t < TimersCount; ++t)
			setTimerStatus(t, static_cast<timer_action>(action));
	}

	void Jack::cmdWritePin(pin_t p, value_type value)
	{
		if (p < GpioCount)
			writePin(p, value);
	}

	typename Jack::Commands Jack::commands() const
	{ 
		return commands_; 
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
# if defined __PG_FORMAT_EEPROM
		invalidateEeprom(eeprom_); // Reinitialize eeprom. 
# endif
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

	void Jack::isrHandler(timer_t t)
	{
		// This method updates the attached event counter/timer 
		// in response to hardware interrupts.
		TimerCounter& timer = timers_[t];

		if (timer.enabled_)
		{
			switch (timer.mode_)
			{
			case timer_mode::Counter:
				timer.object_++;
				break;
			case timer_mode::Timer:
				if (timer.object_.active(timer_tag{}))
				{
					if (timer.timing_ == timing_mode::OneShot)
						timer.enabled_ = false;
					timer.object_.stop(timer_tag{});
					timer.value_ = timer.object_.elapsed().count();
				}
				else
					timer.object_.start(timer_tag{});
				break;
			default:
				break;
			}
		}
	}

#pragma endregion
#pragma region private methods

	template<class ForwardIt>
	void Jack::addCommands(Commands& commands, ForwardIt first, ForwardIt last)
	{
		auto end = std::end(commands);

		commands_.resize(commands.size() + std::distance(first, last));
		std::copy(first, last, end);
	}

	void Jack::attachTimer(timer_t t, pin_t p, uint8_t mode, uint8_t trigger, uint8_t timing, bool instant)
	{
		TimerCounter& timer = timers_[t];

		timer.pin_ = p;
		timer.mode_ = static_cast<timer_mode>(mode);
		timer.trigger_ = static_cast<PinStatus>(trigger);
		timer.timing_ = timer.pin_ == InvalidPin || timer.mode_ == timer_mode::Counter ? timing_mode::Continuous : static_cast<timing_mode>(timing);
		timer.instant_ = timer.pin_ == InvalidPin || timer.mode_ == timer_mode::Counter ? true : instant;
		timer.enabled_ = true;
		if (ack_)
			sendTimerInfo(t);
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

	void Jack::detachTimer(timer_t t, bool ack)
	{
		TimerCounter& timer = timers_[t];

		timer.object_.stop();
		detachInterrupt(digitalPinToInterrupt(timer.pin_));
		timer.pin_ = InvalidPin;
		if (ack_ && ack)
			sendTimerInfo(t);
	}

	bool Jack::eepromValid(EEStream& eeprom, devid_type device_id)
	{
		devid_type eeprom_id = 0;

		eeprom.reset();
		eeprom >> eeprom_id;

		return eeprom_id == device_id;
	}

	std::time_t Jack::elapsedTime(uint8_t s)
	{
		std::time_t elapsed = 0;

		switch (s)
		{
		case 0:
			elapsed = micros();
			break;
		case 1:
			elapsed = millis();
			break;
		default:
			break;
		}

		return elapsed;
	}

	void Jack::endTimer(timer_t t)
	{
		timers_[t].object_.stop();
		detachTimer(t);
	}

	template<class... Ts>
	Jack::size_type Jack::fmtMessage(char* buf, const char* fmt, Ts... args)
	{
		return std::sprintf(buf, fmt, args...);
	}

	Jack::isr_type Jack::getIsr(timer_t n)
	{
		return isrs_[n];
	}

	void Jack::initialize(cmdlist_type& user_cmds)
	{
		// This method appends any client-defined commands to the built-in collection.
# if !defined __PG_NO_USR_COMMANDS
		addCommands(commands_, user_cmds.begin(), user_cmds.end());
# endif
		// Command keys must be unique, uncomment this if you need to check:
		//assert(std::unique(std::begin(commands_), std::end(commands_)) == std::end(commands_));
		
		// Interpreter uses binary search algo, collections must be sorted.
		std::sort(std::begin(commands_), std::end(commands_), cbcomp);
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

		__jack_isr = this;		// Set our external pointer to this.
	}

	void Jack::initialize(Pins& pins)
	{
		// Set SPI pins mode to Reserved. The pin indexes may be out of 
		// range on some boards, but doesn't seem to be a problem.
# if defined __PG_RESERVE_SPI_PINS
		pins[SS].mode_ = gpio_mode::Reserved;
		pins[MOSI].mode_ = gpio_mode::Reserved;
		pins[MISO].mode_ = gpio_mode::Reserved;
		pins[SCK].mode_ = gpio_mode::Reserved;
# endif
		for (size_type i = 0; i < pins.size(); ++i)
		{
			GpioPin& pin = pins[i];

			// Only set non-reserved pins.
			if (pin.mode_ != gpio_mode::Reserved)
			{
				if (isPwmPin(i))
					pin.type_ = gpio_type::Pwm;
				else if (isAnalogPin(i))
					pin.type_ = gpio_type::Analog;
				pin.int_ = isInterruptPin(i);
				// Set LED pin as output, all others as inputs.
				pin.mode_ = i != LedPinNumber ? gpio_mode::Input : isPwmPin(i) ? gpio_mode::PwmOut : gpio_mode::Output;
				//pinMode(i, static_cast<int>(pin.mode_)); // doesn't work on megaavr boards
			}
		}
	}

	void Jack::initialize(Timers& timers)
	{
		for (size_type i = 0; i < timers.size(); ++i)
		{
			TimerCounter& timer = timers[i];

			timer.pin_ = InvalidPin;
			timer.mode_ = timer_mode::Counter;
			timer.trigger_ = FALLING;
			timer.timing_ = timing_mode::Continuous;
			timer.enabled_ = false;
			timer.instant_ = false;
			timer.value_ = 0;
			timer.object_.reset();	// megaavr boards don't intialize timers objects correctly.
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
		for (size_type i = 0; i < GpioCount; ++i)
		{
			uint8_t mode = 0;

			eeprom >> mode;
			cmdPinModeSet(i, mode);
		}
		for (size_type i = 0; i < TimersCount; ++i)
		{
			uint8_t pin = InvalidPin;
			uint8_t mode = 0;
			uint8_t trigger = 0;
			uint8_t timing = 0;
			bool instant = false;

			eeprom >> pin;
			eeprom >> mode;
			eeprom >> trigger;
			eeprom >> timing; 
			eeprom >> instant;
			cmdTimerAttachSet(i, pin, mode, trigger, timing, instant);
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
# if !defined __PG_NO_ETHERNET_DHCP
	void Jack::maintainConnection()
	{
		if (connection_->type() == Connection::Type::Ethernet)
		{
			char buf[Connection::size()] = { '\0' };

			switch (static_cast<EthernetConnection*>(connection_)->hardware().maintain())
			{
			case EthernetConnection::Maintain::NothingHappened:
				break;
			case EthernetConnection::Maintain::RenewFailed:
			case EthernetConnection::Maintain::RebindFailed:	// Try reopening using original params.
				connection_->close();
				connection_->open(connection_->params(buf));
				break;
			case EthernetConnection::Maintain::RenewSuccess:
				break;
			case EthernetConnection::Maintain::RebindSuccess:	// Notify host of new ip.
				cmdConnectionGet();
				break;
			default:
				break;
			}
		}
	}
# endif
	void Jack::makeList(char* list, uint8_t last)
	{
		// Convert dot-separated list into an array of indexes.
		// Lists elements can be individual indexes or ranges 
		// separated by hyphens '-'. Ranges without a trailing 
		// index imply "to last"

		char* tok = std::strtok(list, ListDelimiterChars);

		list_.resize(0);
		while (tok && list_.size() < ListSize)
		{
			pin_t from = std::atoi(tok), to = from;
			char* range = std::strchr(tok, '-');

			if (range)
			{
				*range = '\0';
				to = *++range ? std::atoi(range) : ListSize;
			}
			for (; from <= to && list_.size() < ListSize; ++from)
			{
				list_[list_.size()] = from;
				list_.resize(list_.size() + 1);
			}
			tok = std::strtok(nullptr, ListDelimiterChars);
		}
	}

	Connection* Jack::openConnection(connection_type type, const char* params)
	{
		Connection* connection = nullptr;

		switch (type)
		{
		case connection_type::Serial:
			connection = new pg::SerialConnection(Serial, params);
			break;
# if !defined __PG_NO_WIFI_CONNECTION
		case connection_type::WiFi:
			connection = new pg::WiFiConnection(params);
			break;
# endif
# if !defined __PG_NO_ETHERNET_CONNECTION
		case connection_type::Ethernet:
			connection = new pg::EthernetConnection(params);
			break;
# endif
		default:
			break;
		}

		return connection;
	}

	bool Jack::powerOnDefaults(pin_t pin)
	{
		gpio_mode pin_mode =pins_[pin].mode_;	// Save current pin mode.
		bool result = false;

		pinMode(pin, gpio_mode::Pullup);
		if (!digitalRead(pin))		// Check if pin state = LOW.
			result = true;
		pinMode(pin, pin_mode);		// Return pin to its original mode.

		return result;
	}

	typename Jack::value_type Jack::readPin(pin_t p)
	{
		GpioPin& pin = pins_[p];
		value_type value = value_type();

		if (pin.isAvailable())
		{
			switch (pin.type_)
			{
			case gpio_type::Pwm:
				if (pin.mode_ != gpio_mode::PwmOut)
					value = digitalRead(p);
				break;
			case gpio_type::Analog:
				value = analogRead(p);
				break;
			case gpio_type::Digital:
				value = digitalRead(p);
				break;
			default:
				break;
			}
		}

		return value;
	}

	template<class... Ts>
	void Jack::sendMessage(const char* fmt, Ts... args)
	{
		char msg[Connection::size()] = { '\0' };

		(void)fmtMessage(msg, fmt, args...);
# if defined __PG_CRC_H
		if (checksum_)	// If we received a msg with a checksum then set our reply flag.
			(void)fmtMessage(msg + std::strlen(msg), FmtChecksum, checksum(static_cast<char*>(msg)));
# endif
		connection_->send(msg);
	}

	void Jack::sendPinInfo(pin_t n)
	{
		const GpioPin& pin = pins_[n];

		sendMessage(FmtPinInfo, KeyGetPinInfo, n, pin.type_, pin.int_, pin.mode_);
	}

	void Jack::sendPinMode(pin_t n)
	{
		const GpioPin& pin = pins_[n];

		sendMessage(FmtPinMode, KeyGetPinMode, n, pin.mode_);
	}

	void Jack::sendPinValue(pin_t n, value_type value)
	{
		sendMessage(FmtReadPin, n, value);
	}

	void Jack::sendTimerInfo(timer_t n)
	{
		const TimerCounter& timer = timers_[n];

		sendMessage(FmtTimerAttach, KeyTimerAttach, n, timer.pin_, timer.mode_, timer.trigger_, timer.timing_, timer.instant_);
	}

	void Jack::sendTimerStatus(timer_t n)
	{
		TimerCounter& timer = timers_[n];
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
			value = timer.instant_ ? timer.object_.elapsed().count() : timer.value_;
			break;
		default:
			break;
		}
		sendMessage(FmtTimerStatus, KeyGetTimerStatus, n, active, value);
	}

	void Jack::setConnection(Connection* connection)
	{
		connection_ = connection;
		digitalWrite(pg::LedPinNumber, connection && connection->open());
	}

	void Jack::setPinMode(pin_t p, uint8_t mode)
	{
		GpioPin& pin = pins_[p];

		if (pin.isAvailable())
		{
			// If pin attached to interrupt w/ active LOW trigger it should only be used in pullup mode.  
			pinMode(p, mode);
			pin.mode_ = static_cast<gpio_mode>(mode);
			if (ack_)
				sendPinMode(p);
		}
	}

	void Jack::setTimerStatus(timer_t t, TimerCounter::Action action)
	{
		TimerCounter& timer = timers_[t];

		switch (timer.mode_)
		{
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
				timer.value_ = 0;
				break;
			default:
				break;
			}
			break;
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
				timer.value_ = 0;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		if (ack_)
			sendTimerStatus(t);
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
			eeprom << static_cast<bool>(timer.instant_);
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

		if (pin.isAvailable())
		{
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
	}
# if defined __PG_CRC_H
	bool Jack::check(char* msg)
	{
		(void)std::strtok(msg, CheckSumDelimiterChars); // look for msg w/ trailing ":", 
		char* chk_val = std::strtok(nullptr, CheckSumDelimiterChars); // followed by a check value. 
		checksum_ = chk_val;	// set our checksum reply flag.
		// result is either no check value found or check value == checksum().
		return !chk_val || checksum(msg) == static_cast<unsigned char>(std::atoi(chk_val));
	}
# endif
# if defined __PG_PROGRAM_H
	void Jack::list()
	{
		// Send current program loisting.
		const char* text = program_.text();

		while (*text)
		{
			connection_->send(text);
			text += program_.next(text);
		}
	}

	void Jack::process(const char* msg)
	{
		// Message processor only executes remote commands, not program instructions.
		command_type* cmd = static_cast<command_type*>(interp_.interpret(std::begin(commands_), std::end(commands_), msg));

		// When loading a program, execute any program control commands else forward message to program for storage.
		if (program_.loading())
		{
			if (cmd && *cmd == KeyProgram)
				cmd->execute();
			else
				program_.instruction(msg);
		}
		else if (cmd)
			cmd->execute();
	}

	void Jack::program(uint8_t a)
	{
		// This method handles program change of state commands ("pgm").

		Program::Action action = static_cast<Program::Action>(a);
		bool send = false;
		bool invalid = false;
		value_type status = 0;

		switch (action)
		{
		case Program::Action::Active:
			status = program_.active();
			send = true;
			break;
		case Program::Action::Begin:
			program_.begin();
			action = Program::Action::Size;
			status = program_.size();
			break;
		case Program::Action::End:
			program_.end();
			action = Program::Action::Size;
			status = program_.size();
			break;
		case Program::Action::Halt:
			program_.halt();
			action = Program::Action::Active;
			status = program_.active();
			break;
		case Program::Action::List:
			list();
			break;
		case Program::Action::Reset:
			program_.reset();
			break;
		case Program::Action::Run:
			program_.run();
			action = Program::Action::Active;
			status = program_.active();
			break;
		case Program::Action::Size:
			status = program_.size();
			send = true;
			break;
		case Program::Action::Verify:
			status = verify();
			send = true;
			break;
		default:
			invalid = true;
			break;
		}
		if (!invalid && (ack_ || send))
			sendProgramStatus(action, status);
	}

	void Jack::sendProgramStatus(Program::Action action, value_type status)
	{
		sendMessage(FmtProgramStatus, KeyProgram, action, status);
	}

	iprogram::value_type Jack::sys_get(const char* str)
	{
		iprogram::value_type value = 0;
		size_type n = std::atoi(str + 1);

		switch (*str)
		{
		case '#':
			value = readPin(n);
			break;
		case '%':
			value = timers_[n].object_.elapsed().count();
			break;
		case '+':
			value = timers_[n].object_.count();
			break;
		case '*':
			value = timers_[n].object_.active();
			break;
		case '$':
			value = elapsedTime(n);
			break;
		default:
			value = std::atol(str);
			break;
		}

		return value;
	}

	void Jack::sys_set(const char* str, iprogram::value_type value)
	{
		size_type n = std::atoi(str + 1);

		switch (*str)
		{
		case '#':
			cmdWritePin(n, static_cast<value_type>(value));
			break;
		case '*':
			setTimerStatus(n, static_cast<timer_action>(value));
			break;
		case '+':
			break;
		default:
			break;
		}
	}

	bool Jack::verify()
	{
		// This method verifies a program by attempting to interpret each line in the text.

		bool result = false;

		if (!(program_.active() || program_.loading()))
		{
			const char* text = program_.text();

			if (*text)
			{
				while (*text)
				{
					if (!(interp_.interpret(std::begin(commands_), std::end(commands_), text) || 
						interp_.interpret(std::begin(program_.instructions()), std::end(program_.instructions()), text)))
						break;
					text += program_.next(text);
				}
				result = !(*text);	// Program is valid if end of text reached.
			}
		}

		return result;
	}
# endif

#pragma endregion
#pragma region static isr methods

	void Jack::isrTimer0() { __jack_isr->isrHandler(0); }
	void Jack::isrTimer1() { __jack_isr->isrHandler(1); }
	void Jack::isrTimer2() { __jack_isr->isrHandler(2); }
	void Jack::isrTimer3() { __jack_isr->isrHandler(3); }
	void Jack::isrTimer4() { __jack_isr->isrHandler(4); }
	void Jack::isrTimer5() { __jack_isr->isrHandler(5); }
	void Jack::isrTimer6() { __jack_isr->isrHandler(6); }
	void Jack::isrTimer7() { __jack_isr->isrHandler(7); }
	void Jack::isrTimer8() { __jack_isr->isrHandler(8); }
	void Jack::isrTimer9() { __jack_isr->isrHandler(9); }
	void Jack::isrTimer10() { __jack_isr->isrHandler(10); }
	void Jack::isrTimer11() { __jack_isr->isrHandler(11); }
	void Jack::isrTimer12() { __jack_isr->isrHandler(12); }
	void Jack::isrTimer13() { __jack_isr->isrHandler(13); }
	void Jack::isrTimer14() { __jack_isr->isrHandler(14); }
	void Jack::isrTimer15() { __jack_isr->isrHandler(15); }

#pragma endregion

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 
#endif // !defined __PG_JACK_H
