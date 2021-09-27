#include <pg.h>
#include <cstdint>
#include <limits>
// This program tests 64-bit printing capability. It requires that the 
// <Print.h> and <Print.cpp> files in the Arduino libraries folder be 
// replaced with the modified ones here in Pg/hardware/avr. The Arduino 
// library files are usually located somewhere abouts:
//
//    Arduino\hardware\arduino\avr\cores\arduino, 
//
// on your local machine, depending on where the Arduino IDE is installed. 
// Make a backup of the <Print.h> and <Print.cpp> found there and replace 
// them with the ones here, in Pg/hardware/avr.

int64_t i64 = std::numeric_limits<int64_t>::max();
uint64_t u64 = std::numeric_limits<uint64_t>::max();
float f = 3.4028235E+09;
double d = 3.4028235E+09;
long double dd = 3.4028235E+09;

void setup() 
{
  Serial.begin(9600);
  Serial.println(i64);
  Serial.println(u64);
  Serial.println(f);
  Serial.println(d);
  Serial.println(dd);
}

void loop() 
{

}
