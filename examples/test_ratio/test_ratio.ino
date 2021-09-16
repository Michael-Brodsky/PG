// Part of the Pg test suite for <ratio>.
#include <pg.h>
#include <chrono>
#include <ratio>

bool _ratio()
{
  constexpr intmax_t i = std::numeric_limits<intmax_t>::max();
  static_assert(
    i == std::numeric_limits<intmax_t>::max(),
    "WTF?"
    );
  
  return true;
}

bool _ratio_arithmetic()
{
  using two_third = std::ratio<2, 3>;
  using one_sixth = std::ratio<1, 6>;
  using sum = std::ratio_add<two_third, one_sixth>;
  using diff = std::ratio_subtract<two_third, one_sixth>;
  using prod = std::ratio_multiply<two_third, one_sixth>;
  using quot = std::ratio_divide<two_third, one_sixth>;

  static_assert(
    sum::num == 5 && 
    sum::den == 6 && 
    diff::num == 1 && 
    diff::den == 2 && 
    prod::num == 1 &&
    prod::den == 9 &&
    quot::num == 4 &&
    quot::den == 1,
    "std::ratio arithmetic failed."
    );

  return true;
}

bool _ratio_comparison()
{
  static_assert(
    std::ratio_equal<std::ratio<2, 3>, std::ratio<4, 6>>::value && 
    std::ratio_not_equal<std::ratio<2, 3>, std::ratio<1, 3>>::value && 
    std::ratio_less<std::ratio<23, 37>, std::ratio<57, 90>>::value && 
    std::ratio_less_equal<std::ratio<1, 2>, std::ratio<3, 4>>::value && 
    std::ratio_less_equal<std::ratio<10, 11>, std::ratio<11, 12>>::value && 
    std::ratio_less_equal<std::ratio<10, 11>, std::ratio<11, 12>>::value && 
    std::ratio_greater<std::ratio<3, 4>, std::ratio<1, 2>>::value && 
    std::ratio_greater<std::ratio<11, 12>, std::ratio<10, 11>>::value && 
    std::ratio_greater<std::ratio<12, 13>, std::ratio<11, 12>>::value && 
    std::ratio_greater_equal<std::ratio<2, 3>, std::ratio<2, 3>>::value && 
    std::ratio_greater_equal<std::ratio<2, 1>, std::ratio<1, 2>>::value && 
    std::ratio_greater_equal<std::ratio<1, 2>, std::ratio<1, 2>>::value && 
    std::ratio_greater_equal<std::ratio<999999, 1000000>, std::ratio<999998, 999999>>::value,
    "std::ratio comparison failed."
    );

  return true;
}

bool _ratio_common_type()
{
  using milliseconds = std::chrono::milliseconds;
  using microseconds = std::chrono::microseconds;
  auto ms = milliseconds(30);
  auto us = microseconds(1100);
  std::common_type<milliseconds, microseconds>::type ctype(30);
  static_assert(
    std::is_same<std::common_type<milliseconds, microseconds>::type, 
    std::chrono::microseconds>::value, 
    "chrono common_type failed."
    );

  return ctype.count() == 30;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _ratio(), b2 = _ratio_arithmetic(), b3 = _ratio_comparison(), 
    b4 = _ratio_common_type();
  Serial.print("_ratio() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_arithmetic() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_comparison() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_common_type() = "); Serial.println(b4 ? "OK" : "FAIL");
}

void loop() 
{

}
