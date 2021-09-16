// Part of the Pg test suite for <limits>.
#include <pg.h>
#include <limits>

bool _numeric_limits()
{
  // C/C++ Standards stipulate the following: 
  static_assert(
    std::numeric_limits<short>::max() >= std::numeric_limits<char>::max() && 
    std::numeric_limits<int>::max() >= std::numeric_limits<short>::max() && 
    std::numeric_limits<long>::max() >= std::numeric_limits<int>::max() && 
    std::numeric_limits<long long>::max() >= std::numeric_limits<long>::max() && 
    std::numeric_limits<double>::max() >= std::numeric_limits<float>::max() && 
    std::numeric_limits<long double>::max() >= std::numeric_limits<double>::max(),
    ""
    );

  return true;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _numeric_limits();
  Serial.print("_numeric_limits() = "); Serial.println(b1 ? "OK" : "FAIL");
}

void loop() 
{

}
