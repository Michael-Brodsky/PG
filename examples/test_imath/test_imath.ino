// This is part of the Pg test suite for <lib/imath.h>
#include <pg.h>
#include <lib/imath.h>

bool _imath_isgn()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min();

  return pg::isgn(a) == 0 && pg::isgn(b) == 0 && pg::isgn(c) == -1;
}

bool _imath_isign()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min();
  
  return pg::isign(a) == +1 && pg::isign(b) == +1 && pg::isign(c) == -1;
}

bool _imath_isignof()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min();

  return pg::isignof(a) == +1 && pg::isignof(b) == 0 && pg::isign(c) == -1;
}

bool _imath_iabs()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min() + 1;

  return pg::iabs(a) == a && pg::iabs(b) == b && pg::iabs(c) == -c;
}

bool _imath_max()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min();

  return pg::imax(a, b) == a && pg::imax(b, c) == b && pg::imax(a, c) == a;
}

bool _imath_min()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min();

  return pg::imin(a, b) == b && pg::imin(b, c) == c && pg::imin(a, c) == c;
}

bool _imath_iseven()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min();
  return !pg::iseven(a) && pg::iseven(b) && pg::iseven(c);
}

bool _imath_isodd()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min();

  return pg::isodd(a) && !pg::isodd(b) && !pg::isodd(c);
}

bool _imath_issignne()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min();

  return !pg::issignne(a, b) && pg::issignne(b, c) && pg::issignne(a, c);
}

bool _imath_ispow2()
{
  using Ta = uintmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = 1;

  return !pg::ispow2(a) && !pg::ispow2(b) && pg::ispow2(c);
}

bool _imath_inegateif()
{
  using Ta = intmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  const Ta b = 0;
  const Ta c = std::numeric_limits<Ta>::min() + 1;

  return pg::inegateif(a, false) == a && pg::inegateif(a, true) == -a && 
    pg::inegateif(b, false) == b && pg::inegateif(b, true) == -b && 
    pg::inegateif(c, false) == c && pg::inegateif(c, true) == -c;
}

bool _imath_swap()
{
  using Ta = intmax_t;
  bool result = true;
  Ta a = std::numeric_limits<Ta>::max(), a_ = a;
  Ta b = 0, b_ = b;
  Ta c = std::numeric_limits<Ta>::min(), c_ = c;
  pg::iswap(a, b);
  result = result && a == b_ && b == a_;
  pg::iswap(b, c);
  result = result && b == c_ && c == a_;
  pg::iswap(a, c);
  result = result && a == a_ && c == b_;

  return result;
}

bool _imath_idiv2()
{
  using Ta = uintmax_t;
  bool result = true;
  Ta vals[] = { 
    18446744073709551615ULL,
    9223372036854775807,
    4611686018427387903,
    2305843009213693951,
    1152921504606846975,
    576460752303423487,
    288230376151711743,
    144115188075855871,
    72057594037927935,
    36028797018963967,
    18014398509481983,
    9007199254740991,
    4503599627370495,
    2251799813685247,
    1125899906842623,
    562949953421311,
    281474976710655,
    140737488355327,
    70368744177663,
    35184372088831,
    17592186044415,
    8796093022207,
    4398046511103,
    2199023255551,
    1099511627775,
    549755813887,
    274877906943,
    137438953471,
    68719476735,
    34359738367,
    17179869183,
    8589934591,
    4294967295,
    2147483647,
    1073741823,
    536870911,
    268435455,
    134217727,
    67108863,
    33554431,
    16777215,
    8388607,
    4194303,
    2097151,
    1048575,
    524287,
    262143,
    131071,
    65535,
    32767,
    16383,
    8191,
    4095,
    2047,
    1023,
    511,
    255,
    127,
    63,
    31,
    15,
    7,
    3,
    1
  };
  const Ta x = std::numeric_limits<Ta>::max();
  for (unsigned i = 0; i < 64; ++i)
  {
    result = result && pg::idiv2(x, i) == vals[i];
  }

  return result;
}

bool _imath_ipow2()
{
  using Ta = uintmax_t;
  bool result = true;
  Ta vals[] = {
    1,
    2,
    4,
    8,
    16,
    32,
    64,
    128,
    256,
    512,
    1024,
    2048,
    4096,
    8192,
    16384,
    32768,
    65536,
    131072,
    262144,
    524288,
    1048576,
    2097152,
    4194304,
    8388608,
    16777216,
    33554432,
    67108864,
    134217728,
    268435456,
    536870912,
    1073741824,
    2147483648,
    4294967296,
    8589934592,
    17179869184,
    34359738368,
    68719476736,
    137438953472,
    274877906944,
    549755813888,
    1099511627776,
    2199023255552,
    4398046511104,
    8796093022208,
    17592186044416,
    35184372088832,
    70368744177664,
    140737488355328,
    281474976710656,
    562949953421312,
    1125899906842624,
    2251799813685248,
    4503599627370496,
    9007199254740992,
    18014398509481984,
    36028797018963968,
    72057594037927936,
    144115188075855872,
    288230376151711744,
    576460752303423488,
    1152921504606846976,
    2305843009213693952,
    4611686018427387904,
    9223372036854775808ULL,
  };
  for (uint8_t i = 0; i < 64; ++i)
  {
    result = result && pg::ipow2<Ta>(i) == vals[i];
  }

  return result;
}

bool _imath_ilog2()
{
  using Ta = uintmax_t;
  bool result = true;
  unsigned j = 0;
  for (unsigned i = 0; i < 64; i *= 2)
  {
    result = result && pg::ilog2(++i) == j++;
  }

  return result;
}

bool _imath_ilog10()
{
  using Ta = uintmax_t;
  bool result = true;
  unsigned j = 0;
  for (Ta i = 1; i < 1000000000; i *= 10)
  {
    result = result && pg::ilog10(i) == j++;
  }

  return result;
}

bool _imath_imod2()
{
  using Ta = uintmax_t;
  bool result = true;
  Ta a = std::numeric_limits<Ta>::max();
  unsigned j = 0;
  for (Ta i = 1; i < 64; i *= 2)
  {
    result = result && pg::imod2(a, i) == pg::ipow2<Ta>(j++) - 1;
  }

  return result;
}

bool _imath_imod2m()
{
  using Ta = uintmax_t;
  bool result = true;
  Ta a = std::numeric_limits<Ta>::max();
  uint8_t j = 0;
  for (Ta i = 1; i < 64; i *= 2)
  {
    result = result && pg::imod2m(a, i - 1) == pg::ipow2<Ta>(j++) - 1;
  }

  return result;
}

bool _imath_ipow2ge()
{
  using Ta = uintmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  for (Ta i = 1; i < a / 2; i *= 2)
  {
    result = result && pg::ipow2ge(i) == i;
  }

  return result;
}

bool _imath_ipow2le()
{
  using Ta = uintmax_t;
  bool result = true;
  const Ta a = std::numeric_limits<Ta>::max();
  for (Ta i = 1; i < a / 2; i *= 2)
  {
    result = result && pg::ipow2le(i) == i;
  }

  return result;
}

bool _imath_igcd()
{
  using Ta = uintmax_t;
  bool result = true;
  constexpr unsigned p{ 2 * 2 * 3 };
  constexpr unsigned q{ 2 * 3 * 3 };
  const unsigned r = pg::igcd(p, q);
  const unsigned u = 0;
  result = 2 * 3 == r && r == pg::igcd(q, p) && pg::igcd(p, 0U) == p;

  return result;
}

bool _imath_ilcm()
{
  using Ta = uintmax_t;
  bool result = true;
  constexpr unsigned p{ 2 * 2 * 3 };
  constexpr unsigned q{ 2 * 3 * 3 };
  const unsigned r = pg::ilcm(p, q);
  result = 2 * 2 * 3 * 3  == r;

  return result;
}

bool _imath_iscoprime()
{
  return pg::iscoprime(15U, 8U) && !pg::iscoprime(15U, 9U);
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _imath_isgn(), b2 = _imath_isign(), b3 = _imath_isignof(), 
    b4 = _imath_iabs(), b5 = _imath_max(), b6 = _imath_min(), 
    b7 = _imath_iseven(), b8 = _imath_isodd(), b9 = _imath_issignne(), 
    b10 = _imath_ispow2(), b11 = _imath_inegateif(), b12 = _imath_swap(), 
    b13 = _imath_idiv2(), b14 = _imath_ipow2(), b15 = _imath_ilog2(), 
    b16 = _imath_ilog10(), b17 = _imath_imod2(), b18 = _imath_imod2m(), 
    b19 = _imath_ipow2ge(), b20 = _imath_ipow2le(), b21 = _imath_igcd();
  Serial.print("isgn() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("isign() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("isignof() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("iabs() = "); Serial.println(b4 ? "OK" : "FAIL");
  Serial.print("imax() = "); Serial.println(b5 ? "OK" : "FAIL");
  Serial.print("imin() = "); Serial.println(b6 ? "OK" : "FAIL");
  Serial.print("iseven() = "); Serial.println(b7 ? "OK" : "FAIL");
  Serial.print("isodd() = "); Serial.println(b8 ? "OK" : "FAIL");
  Serial.print("issignne() = "); Serial.println(b9 ? "OK" : "FAIL");
  Serial.print("ispow2() = "); Serial.println(b10 ? "OK" : "FAIL");
  Serial.print("inegateif() = "); Serial.println(b11 ? "OK" : "FAIL");
  Serial.print("iswap() = "); Serial.println(b12 ? "OK" : "FAIL");
  Serial.print("idiv2() = "); Serial.println(b13 ? "OK" : "FAIL");
  Serial.print("ipow2() = "); Serial.println(b14 ? "OK" : "FAIL");
  Serial.print("ilog2() = "); Serial.println(b15 ? "OK" : "FAIL");
  Serial.print("ilog10() = "); Serial.println(b16 ? "OK" : "FAIL");
  Serial.print("imod2() = "); Serial.println(b17 ? "OK" : "FAIL");
  Serial.print("imod2m() = "); Serial.println(b18 ? "OK" : "FAIL");
  Serial.print("ipow2ge() = "); Serial.println(b19 ? "OK" : "FAIL");
  Serial.print("ipow2le() = "); Serial.println(b20 ? "OK" : "FAIL");
  Serial.print("igcd() = "); Serial.println(b21 ? "OK" : "FAIL");
}

void loop() 
{

}
