#include <pg.h>
#include <utilities/EEStream.h>
using namespace pg;

// A serializable class.
class Foo : public iserializable
{
public:
  Foo() = default;
  Foo(int arg1, float arg2) : iserializable(), arg1_(arg1), arg2_(arg2) {}
public:
  void serialize(EEStream& stream) const override { stream << arg1_; stream << arg2_; }
  void deserialize(EEStream& stream) override { stream >> arg1_; stream >> arg2_; }
public:
  bool operator==(const Foo& other) { return arg1_ == other.arg1_ && arg2_ == other.arg2_; }
public:
  int arg1_;    // Member to serialize.
  float arg2_;  // Another member to serialize. 
};

Foo foo(42, 23.0f), bar;
EEStream eeprom;
  
void setup() 
{
  Serial.begin(9600);
  eeprom.store(foo);  // Write foo to eeprom @ addr=0.
  eeprom.reset();     // Reset eeprom to addr=0.
  eeprom.load(bar);   // Read bar from eeprom, now foo == bar.
  Serial.print("foo == bar: ");
  Serial.println(foo == bar ? "OK" : "FAIL");
}

void loop() 
{

}
