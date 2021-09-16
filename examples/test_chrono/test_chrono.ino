// Part of the Pg test suite for <chrono>.
#include <pg.h>
#include <chrono>

bool _duration()
{
  std::chrono::hours h(1); // one hour
  std::chrono::milliseconds ms{ 3 }; // 3 milliseconds
  std::chrono::duration<int, std::kilo> ks(3); // 3000 seconds
  std::chrono::duration<double, std::ratio<1, 30>> hz30(3.5);
  std::chrono::microseconds us = ms;
  std::chrono::microseconds us2 = 2 * ms;
  std::chrono::duration<int, std::kilo> d3(3.5); // should be error, rep=int, whole nums only.
  std::chrono::milliseconds ms2 = us; // should be error, 1/1000000 is not divisible by 1/1000
  bool result = h.count() == 1 && ms.count() == 3 && ks.count() == 3 && us.count() == 3000 && 
    us2.count() == 6000 && hz30.count() == 3.5 && ms2.count() == 3;

  return result;
}

bool _duration_values()
{
  static_assert(
    (std::chrono::hours::zero() == std::chrono::nanoseconds::zero()) && 
    (std::chrono::duration<short, std::nano>::zero() ==
      std::chrono::duration<int, std::deci>::zero())
    && (std::chrono::duration<short, std::deca>::zero() ==
      std::chrono::duration<long, std::giga>::zero()) &&
    (std::chrono::duration<long, std::nano>::zero().count() ==
      std::chrono::duration<float, std::giga>::zero().count())
    ,
    "std::chrono::duration_values failed."
    );

  return true;
}

template <typename T, typename S>
auto durationDiff(const T& t, const S& s)  -> typename std::common_type<T, S>::type
{
  typedef typename std::common_type<T, S>::type Common;
  return Common(t) - Common(s);
}

bool _duration_arithmetic()
{
  constexpr std::chrono::seconds s = std::chrono::hours(1)
    + 2 * std::chrono::minutes(10)
    + std::chrono::seconds(70) / 10;
  std::chrono::seconds s2 = std::chrono::hours(1);
  s2 %= 3;
  //s3 %= 3; // OK, %= disabled for float types.
  std::chrono::duration<float, std::ratio<12>> s3 = std::chrono::hours(1);
  static_assert(
    s.count() == 4807 && 
    s / std::chrono::minutes(2) == 40 && 
    (s / 2).count() == 2403 && 
    std::chrono::duration_cast<std::chrono::hours>(s).count() == 1 && 
    std::chrono::duration_cast<std::chrono::minutes>(s % std::chrono::hours(1)).count() == 20 && 
    std::chrono::duration_cast<std::chrono::seconds>(s % std::chrono::minutes(1)).count() == 7,
    ""
    );
  
  return true;
}

bool _dur_unary_plus_minus()
{
  constexpr std::chrono::seconds s1(10);
  constexpr std::chrono::seconds s2 = -s1;

  static_assert(
    s2.count() == -10,
    ""
    );

  return true;
}

bool _duration_inc_dec()
{
  std::chrono::hours h(1);
  std::chrono::minutes m = ++h;
  m--;

  return m.count() == 119;
}

bool _duration_cmp()
{
  static_assert(
    std::chrono::seconds(2) == std::chrono::milliseconds(2000) && 
    std::chrono::seconds(61) > std::chrono::minutes(1) && 
    std::chrono::milliseconds(1) < std::chrono::minutes(1),
    ""
    );

  return true;
}

void f_duration_cast()
{
  delay(1000);
}

bool _duration_cast()
{
  auto t1 = std::chrono::steady_clock::now();
  f_duration_cast();
  auto t2 = std::chrono::steady_clock::now();
  auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
  std::chrono::duration<long, std::micro> int_usec = int_ms;
  bool result = (fp_ms.count() > 999.5 && fp_ms.count() < 1000.5) && int_ms.count() == 1000 && int_usec.count() == 1000000;

  return result; // needs result
}

bool _duration_types()
{
  constexpr auto year = 31556952LL;

  using shakes = std::chrono::duration<long, std::ratio<1, 100000000L>>;
  using jiffies = std::chrono::duration<long, std::centi>;
  using microfortnights = std::chrono::duration<float, std::ratio<14L * 24L * 60L * 60L, 1000000L>>; // Literals must be fully specified, else doesn't eval corectly.
  using nanocenturies = std::chrono::duration<float, std::ratio<100LL * year, 1000000000LL>>; // ditto

  std::chrono::seconds sec(1);
  bool result = std::chrono::microseconds(sec).count() == 1000000 && 
    shakes(sec).count() == 100000000 && jiffies(sec).count() == 100 && 
    std::chrono::duration_cast<std::chrono::minutes>(sec).count() == 0 && 
    (microfortnights(sec).count() < 0.82672f && microfortnights(sec).count() >= 0.82671f) && 
    (nanocenturies(sec).count() < 0.316888 && nanocenturies(sec).count() >= 0.316887);

  return result;
}

using TestClock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<TestClock>;
using Ms = std::chrono::milliseconds;
bool check_ms(const TimePoint& point, const intmax_t cnt)
{
  const TestClock::duration since_epoch = point.time_since_epoch();

  return std::chrono::duration_cast<Ms>(since_epoch).count() >= cnt;
}

bool _time_point()
{
  const TimePoint default_value = TimePoint(); // dflt ctor broke.
  const TestClock::duration duration_4_seconds = std::chrono::seconds(4);
  const TimePoint time_point_4_seconds(duration_4_seconds);
  
  bool result = check_ms(default_value, 0) && check_ms(time_point_4_seconds, 4000);

  const TimePoint time_point_now = TestClock::now();
  result = result && check_ms(time_point_now, 4001);

  return result;
}

bool _time_point_cmp()
{
  const TimePoint time_point_now = TestClock::now(); 
  const TimePoint tp2(time_point_now);
  const TimePoint tpz;
  bool result = time_point_now == tp2 && tpz != time_point_now && tpz < tp2&& time_point_now > tpz &&
    time_point_now >= tp2 && tpz <= tp2;

  return result;
}

using Sec = std::chrono::seconds;
template<class Duration>
using TimePoint2 = std::chrono::time_point<TestClock, Duration>;
inline bool check_ms2(const TimePoint2<Ms>& time_point, Ms ms)
{
  return time_point.time_since_epoch().count() == ms.count();
}

bool _time_point_cast()
{
  TimePoint2<Sec> time_point_sec(Sec(4));
  TimePoint2<Ms> time_point_ms(time_point_sec);
  bool result1 = check_ms2(time_point_ms, Ms(4000));
  time_point_ms = TimePoint2<Ms>(Ms(5756));
  time_point_sec = std::chrono::time_point_cast<Sec>(time_point_ms);
  bool result2 = check_ms2(time_point_sec, Ms(5000));

  return result1 && result2;
}
void setup() 
{
  Serial.begin(9600);
  bool b1 = _duration(), b2 = _duration_values(), b3 = _duration_arithmetic(), 
    b4 = _dur_unary_plus_minus(), b5 = _duration_inc_dec(), b6 = _duration_cmp(), 
    b7 = _duration_cast(), b8 = _duration_types(), b9 = _time_point(), b10 = _time_point_cmp(), 
    b11 = _time_point_cast();
  Serial.print("_duration() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_duration_values() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_duration_arithmetic() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_dur_unary_plus_minus() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_duration_inc_dec() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_duration_cmp() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_duration_cast() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_duration_types() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_nonmembers() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_time_point() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_time_point_cmp() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_time_point_cast() = "); Serial.println(b3 ? "OK" : "FAIL");
}

void loop() 
{

}
