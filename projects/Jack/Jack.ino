// This program demonstrates the use of the pg::Jack class to turn an Arduino into a 
// remote data acquisition (DAQ) platform.

#include <pg.h>
#include <components/Jack.h>

using pg::Connection;
using pg::SerialConnection;
using pg::UdpConnection;
using pg::Jack;
using pg::EEStream;
using pg::RemoteControl;
template<class... Ts>
using RemoteCommand = RemoteControl::Command<void, void, Ts...>;

void jackCallback();			// Client callback function, prints the current connection settings.
void sendConnection(SerialConnection*, char*);
void sendConnection(UdpConnection*, char*);
void jackCommand(int);			// Remotely callable, user-defined function.
void jackCommand(int, long);	// Remotely callable, user-defined function.
void jackCommand(float);		// Remotely callable, user-defined function.

Connection* connect = nullptr;
EEStream eeprom;				// EEPROM streaming object.
RemoteCommand<int> cmd1("cmd1", &jackCommand);			// User-defined remote command calls jackCommand<int>.
RemoteCommand<int, long> cmd2("cmd2", &jackCommand);	// User-defined remote command calls jackCommand<int, long>.
RemoteCommand<float> cmd3("cmd3", &jackCommand);		// User-defined remote command calls jackCommand<float>.
Jack jack(eeprom, & jackCallback, { &cmd1,&cmd2,&cmd3 });

void setup() 
{
	//connect = new SerialConnection(Serial);
	//connect->open("56000,8N1");
	connect = new UdpConnection();
	connect->open("Drowning_Ophelia,Chinab13!!,5002");
	if (connect->open())
	{
		connect->flush();
		jack.connection(connect);
		jack.initialize();	// This must be called any time the connection or command list changes.
		digitalWrite(LED_BUILTIN, true);
	}
}

void loop()
{
	jack.poll();		// Execute any new commands received.
}

void jackCallback()
{
	char buf[32];

	switch (jack.connection()->type())
	{
	case Connection::ConnectionType::Serial:
		sendConnection(reinterpret_cast<SerialConnection*>(const_cast<Connection*>(jack.connection())), buf);
		break;
	case Connection::ConnectionType::Udp:
		sendConnection(reinterpret_cast<UdpConnection*>(const_cast<Connection*>(jack.connection())), buf);
		break;
	default:
		break;
	}
}
 
void sendConnection(SerialConnection* connection, char* buf)
{
	std::sprintf(buf, "serial=%lu,%3s,%lu", connection->baud(), "8N1", connection->timeout());
	connection->send(buf);
}

void sendConnection(UdpConnection* connection, char* buf)
{
	IPAddress ipa = connection->localIP();
	std::sprintf(buf, "udp=%u.%u.%u.%u,%u", ipa[0],ipa[1],ipa[2],ipa[3], connection->port());
	connection->send(buf);
}

void jackCommand(int i)
{
	char buf[64];

	std::sprintf(buf, "jackCommand(%d)", i);
	connect->send(buf);
}

void jackCommand(int i, long l)
{
	char buf[64];

	std::sprintf(buf, "jackCommand(%d, %ld)", i, l);
	connect->send(buf);
}

void jackCommand(float f)
{
	// Requires floating point support for sprintf() function.
	// Edit platform.txt file and add/change this line to:
	// compiler.c.elf.extra_flags=-Wl,-u,vfprintf -lprintf_flt -lm

	char buf[64];

	std::sprintf(buf, "jackCommand(%.2f)", f);
	connect->send(buf);
}

