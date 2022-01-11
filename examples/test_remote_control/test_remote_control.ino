#include <pg.h>
#include <components/RemoteControl.h>

using pg::RemoteControl;

void foo(int);
struct Foo 
{
  void foo(int);
};

Foo f;
RemoteControl::Command<void, void, int> bar("foo", &foo);
RemoteControl::Command<void, Foo, int> bar_none("bar", f, &Foo::foo);
RemoteControl rc({ &bar,&bar_none }, Serial);

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  rc.poll();
}

void foo(int i) 
{
  Serial.print("foo("); Serial.print(i); Serial.println(")");
}

void Foo::foo(int i) 
{ 
  Serial.print("Foo::foo("); Serial.print(i); Serial.println(")");
}
