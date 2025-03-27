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

bool _var_limits()
{
  Serial.print("<bool> min: "); Serial.print(std::numeric_limits<bool>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<bool>::max());
  Serial.print("<char> min: "); Serial.print((signed char)std::numeric_limits<char>::min()); Serial.print(", max: "); Serial.println((signed char)std::numeric_limits<char>::max());
  Serial.print("<signed char> min: "); Serial.print(std::numeric_limits<signed char>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<signed char>::max());
  Serial.print("<unsigned char> min: "); Serial.print(std::numeric_limits<unsigned char>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<unsigned char>::max());
  Serial.print("<wchar_t> min: "); Serial.print(std::numeric_limits<wchar_t>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<wchar_t>::max());
  Serial.print("<char16_t> min: "); Serial.print(std::numeric_limits<char16_t>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<char16_t>::max());
  Serial.print("<char32_t> min: "); Serial.print(std::numeric_limits<char32_t>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<char32_t>::max());
  Serial.print("<signed short> min: "); Serial.print(std::numeric_limits<signed short>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<signed short>::max());
  Serial.print("<unsigned short> min: "); Serial.print(std::numeric_limits<unsigned short>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<unsigned short>::max());
  Serial.print("<signed int> min: "); Serial.print(std::numeric_limits<signed int>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<signed int>::max());
  Serial.print("<unsigned int> min: "); Serial.print(std::numeric_limits<unsigned int>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<unsigned int>::max());
  Serial.print("<signed long int> min: "); Serial.print(std::numeric_limits<signed long int>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<signed long int>::max());
  Serial.print("<unsigned long int> min: "); Serial.print(std::numeric_limits<unsigned long int>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<unsigned long int>::max());
  
  // Needs <Print.h> that supports 64-bit integer printing.
  // Update 20250327 - we have that now.
  //Serial.print("<signed long long int> min: "); Serial.print(std::numeric_limits<signed long long int>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<signed long long int>::max());
  //Serial.print("<unsigned long long int> min: "); Serial.print(std::numeric_limits<unsigned long long int>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<unsigned long long int>::max());

  // The Arduino API is severely limited when it comes to printing floating point numbers, 
  // so these lines are left commented out.
  // Serial.print("<float> min: "); Serial.print(std::numeric_limits<float>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<float>::max());
  // Serial.print("<double> min: "); Serial.print(std::numeric_limits<double>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<double>::max());
  // Serial.print("<long double> min: "); Serial.print(std::numeric_limits<long double>::min()); Serial.print(", max: "); Serial.println(std::numeric_limits<long double>::max());
  
  return true;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _numeric_limits(), b2 = _var_limits();
  Serial.print("_built_ins() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_numeric_limits() = "); Serial.println(b1 ? "OK" : "FAIL");
}

void loop() 
{

}
