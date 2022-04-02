// This program demonstrates the use of the pg::Jack class to turn an Arduino into a 
// remote data acquisition (DAQ) platform.

#include <pg.h>
#include <components/Jack.h>

using pg::Jack;
using pg::EEStream;
using pg::RemoteControl;
template<class... Ts>
using RemoteCommand = RemoteControl::Command<void, void, Ts...>;

void jackCallback();			// Client callback function, prints the current serial settings.
void jackCommand(int);			// Remotely callable, user-defined function.
void jackCommand(int, long);	// Remotely callable, user-defined function.
void jackCommand(float);		// Remotely callable, user-defined function.

serial<0> serial0;				// HardwareSerial object with extended capabilities (serial<0> references Serial).
EEStream eeprom;				// EEPROM streaming object.
RemoteCommand<int> cmd1("cmd1", &jackCommand);			// User-defined remote command calls jackCommand<int>.
RemoteCommand<int, long> cmd2("cmd2", &jackCommand);	// User-defined remote command calls jackCommand<int, long>.
RemoteCommand<float> cmd3("cmd3", &jackCommand);		// User-defined remote command calls jackCommand<float>.
//Jack jack(serial0, eeprom, jackCallback, { &cmd1,&cmd2,&cmd3 });	// Jack object.
Jack jack(serial0, eeprom, jackCallback);

void setup() 
{
	jack.initialize();	// Retrieve settings from EEPROM & open Serial port.
	jackCallback();		// Print the current Serial settings.
}

void loop()
{
	jack.poll();		// Execute any new commands received by the serial port.
}

void jackCallback()
{
	char buf[17];
	hardware_serial& comms = jack.comms();
	auto it = std::find(hardware_serial::SupportedFrames.begin(), hardware_serial::SupportedFrames.end(), comms.frame());

	comms.printFmt(buf, "%lu,%s,%lu%c", comms.baud(), it->string(), comms.getTimeout(), (comms.isOpen()) ? '*' : ' ');
}

void jackCommand(int i)
{
	char buf[64];

	serial0.printFmt(buf, "jackCommand(%d)", i);
}

void jackCommand(int i, long l)
{
	char buf[64];

	serial0.printFmt(buf, "jackCommand(%d, %ld)", i, l);
}

void jackCommand(float f)
{
	// Requires floating point support for sprintf() function.
	// Edit platform.txt file and add/change this line to:
	// compiler.c.elf.extra_flags=-Wl,-u,vfprintf -lprintf_flt -lm

	char buf[64];

	serial0.printFmt(buf, "jackCommand(%.2f)", f);
}
