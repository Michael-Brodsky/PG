#include <pg.h>
#include <components/SerialControl.h>

using namespace pg;
using SerialController = SerialControl<8>;

void f1() { Serial.println("f1"); }
void f2() { Serial.println("f2"); }

struct Cmd1:public icommand { void execute() override { f1(); } } cmd1;
struct Cmd2:public icommand { void execute() override { f2(); } } cmd2;
SerialController::value_type buf[32] = {'\0'};
SerialController::Command commands[] = 
{
  SerialController::Command{{"cmd1",&cmd1}}, 
  SerialController::Command{{"cmd2",&cmd2}}, 
};

SerialController sc(commands, true);

void setup() 
{
  Serial.begin(9600);
  Serial.println("Send command strings to the serial port.");
  Serial.flush();
}

void loop() 
{
  sc.poll();
}
