// This is part of the Pg test suite for <lib/bits.h>
#include <pg.h>
#include <lib/bits.h>

template<class T>
T _bits()
{
  T b = pg::bit<T>(pg::widthof(b) - 1);

  return b;
}

bool _bits_bit()
{
  bool result = _bits<uint8_t>() == 128 && _bits<uint16_t>() == 32768 &&
    _bits<uint32_t>() == 2147483648 && _bits<uint64_t>() == _bits<uintmax_t>();

  return result;
}

bool _bits_bitset()
{
  bool result = true;
  uint8_t u8 = 0;
  uint16_t u16 = 0;
  uint32_t u32 = 0;
  uint64_t u64 = 0;
  result = result && pg::bitset(u8, 0) == 1 && pg::bitset(u16, 0) == 1 && 
    pg::bitset(u32, 0) == 1 && pg::bitset(u64, 0) == 1;

  return result;
}

bool _bits_bitclr()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && pg::bitclr(u8, 1) == 40 && pg::bitclr(u16, 1) == 40 &&
    pg::bitclr(u32, 1) == 40 && pg::bitclr(u64, 1) == 40;

  return result;
}

bool _bits_bitflip()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && pg::bitflip(u8, 0) == 43 && pg::bitflip(u16, 0) == 43 &&
    pg::bitflip(u32, 0) == 43 && pg::bitflip(u64, 0) == 43;

  return result;
}

bool _bits_bitchg()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && pg::bitchg(u8, 0, 1) == 43 && pg::bitchg(u16, 0, 1) == 43 &&
    pg::bitchg(u32, 0, 1) == 43 && pg::bitchg(u64, 0, 1) == 43;

  return result;
}

bool _bits_bitmerge()
{
  bool result = true;
  uint8_t u8 = 0xff, v8 = 0, m8 = 0xf0;
  uint16_t u16 = 0xffff, v16 = 0, m16 = 0xf0f0;
  uint32_t u32 = 0xffffffff, v32 = 0, m32 = 0xf0f0f0f0;
  uint64_t u64 = 0xffffffffffffffff, v64 = 0, m64 = 0xf0f0f0f0f0f0f0f0;
  result = result && pg::bitmerge(u8, v8, m8) == 15 && pg::bitmerge(u16, v16, m16) == 3855 &&
    pg::bitmerge(u32, v32, m32) == 252645135 && pg::bitmerge(u64, v64, m64) == 1085102592571150095;

  return result;
}

bool _bits_bitrev()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && pg::bitrev(u8) == 84 && pg::bitrev(u16) == 21504 &&
    pg::bitrev(u32) == 1409286144 && pg::bitrev(u64) == 6052837899185946624;

  return result;
}

bool _bits_bitsetm()
{
  bool result = true;
  uint8_t u8 = 0, m8 = 0xff;
  uint16_t u16 = 0, m16 = 0xff;
  uint32_t u32 = 0, m32 = 0xff;
  uint64_t u64 = 0, m64 = 0xff;
  result = result && pg::bitsetm(u8, m8) == m8 && pg::bitsetm(u16, m16) == m16 &&
    pg::bitsetm(u32, m32) == m32 && pg::bitsetm(u64, m64) == m64;

  return result;
}

bool _bits_bitclrm()
{
  bool result = true;
  uint8_t u8 = 0, m8 = 0xff;
  uint16_t u16 = 0, m16 = 0xffff;
  uint32_t u32 = 0, m32 = 0xffffffff;
  uint64_t u64 = 0, m64 = 0xffffffffffffffff;
  result = result && pg::bitclrm(u8, m8) == 0 && pg::bitclrm(u16, m16) == 0 &&
    pg::bitclrm(u32, m32) == 0 && pg::bitclrm(u64, m64) == 0;

  return result;
}

bool _bits_bitchgm()
{
  bool result = true;
  uint8_t u8 = 0, m8 = 0xff;
  uint16_t u16 = 0, m16 = 0xffff;
  uint32_t u32 = 0, m32 = 0xffffffff;
  uint64_t u64 = 0, m64 = 0xffffffffffffffff;
  result = result && pg::bitchgm(u8, m8, 1) == m8 && pg::bitchgm(u16, m16, 1) == m16 &&
    pg::bitchgm(u32, m32, 1) == m32 && pg::bitchgm(u64, m64, 1) == m64;

  return result;
}

bool _bits_bitflipm()
{
  bool result = true;
  uint8_t u8 = 42, m8 = 0x1;
  uint16_t u16 = 42, m16 = 0x1;
  uint32_t u32 = 42, m32 = 0x1;
  uint64_t u64 = 42, m64 = 0x1;
  result = result && pg::bitflipm(u8, m8) == 43 && pg::bitflipm(u16, m16) == 43 &&
    pg::bitflipm(u32, m32) == 43 && pg::bitflipm(u64, m64) == 43;

  return result;
}

bool _bits_bitisset()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && pg::bitisset(u8, 1) && pg::bitisset(u16, 1) &&
    pg::bitisset(u32, 1) && pg::bitisset(u64, 1);

  return result;
}

bool _bits_bitisclr()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && !pg::bitisset(u8, 0) && !pg::bitisset(u16, 0) &&
    !pg::bitisset(u32, 0) && !pg::bitisset(u64, 0);

  return result;
}

bool _bits_bitissetm()
{
  bool result = true;
  uint8_t u8 = 42, m8 = 0x1;
  uint16_t u16 = 42, m16 = 0x1;
  uint32_t u32 = 42, m32 = 0x1;
  uint64_t u64 = 42, m64 = 0x1;
  result = result && !pg::bitissetm(u8, m8) && !pg::bitissetm(u16, m16) &&
    !pg::bitissetm(u32, m32) && !pg::bitissetm(u64, m64);

  return result;
}

bool _bits_bitlsbset()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && pg::bitlsbset(u8) == 2 && pg::bitlsbset(u16) == 2 &&
    pg::bitlsbset(u32) == 2 && pg::bitlsbset(u64) == 2;

  return result;
}

bool _bits_bitlsbclrm()
{
  bool result = true;
  uint8_t u8 = 42, m8 = 0x1;
  uint16_t u16 = 42, m16 = 0x1;
  uint32_t u32 = 42, m32 = 0x1;
  uint64_t u64 = 42, m64 = 0x1;
  result = result && pg::bitmlsbclr(u8) == m8 && pg::bitmlsbclr(u16) == m16 &&
    pg::bitmlsbclr(u32) == m32 && pg::bitmlsbclr(u64) == m64;

  return result;
}

bool _bits_bitnset()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && pg::bitnset(u8) == 3 && pg::bitnset(u16) == 3 &&
    pg::bitnset(u32) == 3 && pg::bitnset(u64) == 3;

  return result;
}


bool _bits_bitnlsbclr()
{
  bool result = true;
  uint8_t u8 = 42, m8 = 0x1;
  uint16_t u16 = 42, m16 = 0x1;
  uint32_t u32 = 42, m32 = 0x1;
  uint64_t u64 = 42, m64 = 0x1;
  result = result && pg::bitnlsbclr(u8) == m8 && pg::bitnlsbclr(u16) == m16 &&
    pg::bitnlsbclr(u32) == m32 && pg::bitnlsbclr(u64) == m64;

  return result;
}

bool _bits_bitparity()
{
  bool result = true;
  uint8_t u8 = 42;
  uint16_t u16 = 42;
  uint32_t u32 = 42;
  uint64_t u64 = 42;
  result = result && pg::bitparity(u8) && pg::bitparity(u16) &&
    pg::bitparity(u32) && pg::bitparity(u64);

  return result;
}

bool _bits_swap()
{
  bool result = true;
  uint8_t u8 = 42, m8 = 0x1;
  uint16_t u16 = 42, m16 = 0x1;
  uint32_t u32 = 42, m32 = 0x1;
  uint64_t u64 = 42, m64 = 0x1;
  pg::bitswap(u8, m8);
  pg::bitswap(u16, m16);
  pg::bitswap(u32, m32);
  pg::bitswap(u64, m64);
  result = result && u8 == 1 && m8 == 42 && u16 == 1 && m16 == 42 && 
    u32 == 1 && m32 == 42 && u64 == 1 && m64 == 42;

  return result;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _bits_bit(), b2 = _bits_bitset(), b3 = _bits_bitclr(), 
    b4 = _bits_bitflip(), b5 = _bits_bitchg(), b6 = _bits_bitmerge(), 
    b7 = _bits_bitrev(), b8 = _bits_bitsetm(), b9 = _bits_bitclrm(), 
    b10 = _bits_bitchgm(), b11 = _bits_bitflipm(), b12 = _bits_bitisset(), 
    b13 = _bits_bitisclr(), b14 = _bits_bitissetm(), b15 = _bits_bitlsbclrm();
  Serial.print("bit() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("bitset() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("bitclr() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("bitflip() = "); Serial.println(b4 ? "OK" : "FAIL");
  Serial.print("bitchg() = "); Serial.println(b5 ? "OK" : "FAIL");
  Serial.print("bitmerge() = "); Serial.println(b6 ? "OK" : "FAIL");
  Serial.print("bitrev() = "); Serial.println(b7 ? "OK" : "FAIL");
  Serial.print("bitsetm() = "); Serial.println(b8 ? "OK" : "FAIL");
  Serial.print("bitclrm() = "); Serial.println(b9 ? "OK" : "FAIL");
  Serial.print("bitchgm() = "); Serial.println(b10 ? "OK" : "FAIL");
  Serial.print("bitflipm() = "); Serial.println(b11 ? "OK" : "FAIL");
  Serial.print("bitisset() = "); Serial.println(b12 ? "OK" : "FAIL");
  Serial.print("bitisclr() = "); Serial.println(b13 ? "OK" : "FAIL");
  Serial.print("bitissetm() = "); Serial.println(b14 ? "OK" : "FAIL");
  Serial.print("bitmlsbclr() = "); Serial.println(b15 ? "OK" : "FAIL");
}

void loop() 
{

}
