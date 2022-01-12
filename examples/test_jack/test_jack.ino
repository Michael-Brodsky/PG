// This program demonstrates usage of the pg::Jack class to remotely read/write values 
// and get/set the status of the Arduino's gpio pins. See <components/Jack.h> for list 
// of built-in Jack commands.

#include <pg.h>
#include <components/Jack.h>

using pg::Jack;
using pg::EEStream;
using pg::RemoteControl;

void jackCallback();      // Client callback function, prints the current serial settings.
void jackCommand(int);      // Remotely callable, user-defined function 1.
void jackCommand(int, long);  // Remotely callable, user-defined function 2.
void jackCommand(float);    // Remotely callable, user-defined function 3.

serial<0> serial0;        // Serial port object with extended capabilities.
EEStream eeprom;        // EEPROM streaming object.
RemoteControl::Command<void, void, int> cmd1("cmd1", &jackCommand);     // User-defined remote command calls function 1.
RemoteControl::Command<void, void, int, long> cmd2("cmd2", &jackCommand); // User-defined remote command calls function 2.
RemoteControl::Command<void, void, float> cmd3("cmd3", &jackCommand);   // User-defined remote command calls function 3.
Jack jack(serial0, eeprom, jackCallback, { &cmd1,&cmd2,&cmd3 });      // Jack object.

void setup() 
{
  jack.initialize();  // Retrieves settings from EEPROM & opens comm port.
  jackCallback();   // Print the current comms settings.
}

void loop()
{
  jack.poll();    // Execute any new commands received by the serial port.
}

void jackCallback()
{
  char buf[17];
  hardware_serial& comms = jack.commsHardware();
  auto it = std::find(hardware_serial::SupportedFrames.begin(), hardware_serial::SupportedFrames.end(), comms.frame());

  comms.printFmt(buf, "%s", pg::board_traits<pg::board_type>::mcu);
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
  char buf[64];

  serial0.printFmt(buf, "jackCommand(%.2f)", f);
}
