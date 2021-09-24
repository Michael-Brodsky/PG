#include <pg.h>
#include <cstdint>
#include <limits>

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
