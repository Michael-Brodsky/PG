//#include <system/types.h>
#include <pg.h>
#include <system/utils.h>
//#include <iterator>
#include <utilities/Connection.h>
#include <utilities/EEStream.h>
#include <utilities/Interpreter.h>
#include <utilities/Program.h>
//#include "ICommand.h"

using namespace pg;
using namespace pg::utils;
using ICmd = Interpreter::ICommand;
template<class T, class U, class...V>
using Cmd = typename Interpreter::Command<T,U,V...>;

void process(const char*);
void program(uint8_t);
void rdp();
void rdp(pin_t);
void wrp(pin_t, uint16_t);
void stopgm();
void ldapgm();
void eefree();
//const char* cmds[] = { "rdp=54","rdp=55","rdp=56","rdp=57" };

Program _pgm;
Interpreter _interp;
EEStream _eeprom;
Connection* _connection = new SerialConnection(Serial);

Cmd<void, void, pin_t> rdp1("rdp", &rdp);
Cmd<void, void, void> rdp2("rdp", &rdp);
Cmd<void, void, pin_t, uint16_t> wrp1("wrp", &wrp);
Cmd<void, void, uint8_t> pgm1("pgm", &program);
Cmd<void, Program, uint8_t> jmp1("jmp", _pgm, &Program::jump);
Cmd<void, Program, std::time_t> dly1("dly", _pgm, &Program::sleep);
Cmd<void, void, void> eep1("eep", &eefree);
ICmd* _commands[] = { &rdp1,&rdp2,&wrp1,&pgm1,&jmp1,&dly1,&eep1 };

void setup() 
{
	//sbegin(115200);
	_connection->open("9600,8N1,10");
	//_connection->send("ppp");
	if (_connection->open())
		sprintln(1);
	else
		sprintln(0);
	//sprintln(_connection->open());
	//int i = 0;
	_interp.commands(std::begin(_commands), std::end(_commands));
	//sprintln(_pgm.size());
	//for (uint8_t k = 0; k < x.args().size(); ++k)
	//	sprintln(std::get<0>(x.args()));
	//_pgm.begin();
	//_pgm.add("rdp=54");
	//_pgm.add("rdp=55");
	//_pgm.add("rdp=56");
	//_pgm.add("rdp=57");
	//_pgm.end();
	//exes = new icommand* [_pgm.size()];
	//for (auto cmd : cmds)
	//{
	//	(_interp.execute(cmd));
	//}
	//for (uint8_t j = 0; j < i; ++j)
	//	exes[j]->execute();
	//sprint("size="); sprintln(_pgm.size());
	//_pgm.run();
	//const char* cmd;
	//while((cmd = _pgm.exec()))
	//	sprintln(cmd);
	//sprint("cmds="); sprintln(_interp.commands().size());
	//sprintln(std::strcmp("cmdf", "c"));
	//sprintln(std::strcmp("cmdf", "cmdf"));
	//sprintln(std::strcmp("cmdf", "cmdfff"));
	//std::strncpy(buf, "cmdf", 64);
	//_interp.execute(buf);
	//std::strncpy(buf, "cmdg=42", 64);
	//_interp.execute(buf);
}

void loop() 
{
	process(_connection->receive());
	if (_pgm.active())
		_interp.execute(_pgm.command());
}

void process(const char* msg)
{
	if (*msg)
	{
		if (_pgm.loading())
		{
			icommand* cmd = _interp.interpret(msg);

			if (cmd == static_cast<icommand*>(&pgm1))
				cmd->execute();
			else
				_pgm.add(msg);
		}
		else
			_interp.execute(msg);
	}
}

void program(uint8_t value)
{
	Program::Action action = static_cast<Program::Action>(value);

	switch (action)
	{
	case Program::Action::Begin:
		_pgm.begin();

		break;
	case Program::Action::End:
		_pgm.end();

		break;
	case Program::Action::Halt:
		_pgm.halt();
		break;
	case Program::Action::Reset:
		_pgm.reset();
		break;
	case Program::Action::Run:
		_pgm.run();
		break;
	case Program::Action::Size:
	{
		char buf[64] = { '\0' };

		std::sprintf(buf, "%s=%u=%u", pgm1.key(), value, _pgm.size());
		_connection->send(buf);
	}
		break;
	case Program::Action::Status:
	{
		char buf[64] = { '\0' };
		uint8_t status = _pgm.loading() == true 
			? Program::Action::Begin
			: _pgm.active() == true 
			? Program::Action::Run
			: Program::Action::Halt;

		std::sprintf(buf, "%s=%u=%u", pgm1.key(), value, status);
		_connection->send(buf);
	}
		break;
	case Program::Action::Store:
		stopgm();
		break;
	case Program::Action::Load:
		ldapgm();
		break;
	default:
		break;
	}
}

void rdp()
{
	pin_t pin = 255;

}

void rdp(pin_t pin)
{
	char buf[64] = { '\0' };

	std::sprintf(buf, "%u=%u", pin, analogRead(pin));
	_connection->send(buf);
}

void wrp(pin_t pin, uint16_t value)
{

}

void stopgm()
{
	const char* text = _pgm.text(), * next = text;
	const char* end = text + Program::CharsMax;

	_eeprom.reset();
	while (*next)
	{
		_eeprom << next;
		next = (const char*)std::memchr(next, 0, std::distance(next, end)) + 1;
	}
	_eeprom << "";
}

void ldapgm()
{
	char buf[64] = { '\0' };
	char* cmd = buf;

	_pgm.begin();
	_eeprom.reset();
	_eeprom >> cmd;
	while (*cmd)
	{
		_pgm.add(cmd);
		_eeprom >> cmd;
	}
	_pgm.end();
}

void eefree()
{
	char buf[64] = { '\0' };
	std::size_t n = EEPROM.length() - _eeprom.address();

	std::sprintf(buf, "%s=%u", "len", EEPROM.length());
	_connection->send(buf);
	std::sprintf(buf, "%s=%u", "addr", _eeprom.address());
	_connection->send(buf);
}
