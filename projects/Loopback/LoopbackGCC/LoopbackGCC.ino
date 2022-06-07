#include <WiFiNINA.h>
#include <pg.h>
#include <utilities/Connection.h>

using namespace pg;

SerialConnection serial(Serial);
UdpConnection udp;

void setup() 
{
	serial.open("9600,8N1");
	if (serial.open())
		serial.send("Serial open");
	udp.open("Drowning_Ophelia,Chinab13!!,5002");
	if (udp.open())
		serial.send("Udp connected");
	else
		serial.send("Udp connect failed");
}

void loop() 
{
	if (serial.open())
		serial.send(serial.receive());
	if (udp.open())
		udp.send(udp.receive());
}
