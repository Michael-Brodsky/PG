#include <pg.h>
#include <random>

bool _random_lce()
{
  std::array<std::minstd_rand0::result_type, 5> 
    minstd_rand0_results{ 1861954555,738487229,1446856012,1351647381,1055503923 };
  std::array<std::minstd_rand::result_type, 5> 
    minstd_rand_results{ 1226992363,549342533,185324939,1550736550,766486715 };
  bool result = true;
  std::minstd_rand lcg(42);
  std::minstd_rand0 lcg0;
  lcg0.seed(lcg());
  for (auto& i : minstd_rand0_results)
    result = result && i == lcg0();
  for (auto& i : minstd_rand_results)
    result = result && i == lcg();
  result = result && lcg == lcg && lcg0 == lcg0;
  result = result && lcg.min() == 0 && lcg.max() == 2147483647 - 1;

  return result;
}

bool _random_swce()
{
  bool result = true;
  std::array<std::ranlux24_base::result_type, 5>
    ranlux24_base_results{ 5446415,4972016,6109456,16424943,7575311 };
  std::ranlux24_base ranlux24_0, ranlux24_1;
  for (auto i : ranlux24_base_results)
  {
    result = result && i == ranlux24_0(), ranlux24_1();
    result = result && ranlux24_0 == ranlux24_1;
  }
  return result;
}

bool _random_xor()
{
  bool result = true;
  std::array<std::xorshift128_engine<unsigned long>::result_type, 5> 
    xorshift128_results{ 3701687786,458299110,2500872618,3633119408,516391518 };
  std::xorshift128_engine<unsigned long> xorg0, xorg1, xorg2;
  for (auto& i : xorshift128_results)
  {
    result = result && i == xorg0() && i == xorg1() && i == xorg2();
    result = result && xorg0 == xorg1 && xorg0 == xorg2;
  }

  for (auto& i : xorshift128_results)
    xorg2.seed(xorg1());
  for (auto& i : xorshift128_results)
    result == result && xorg0() != xorg2();
  std::xorshift128_engine<unsigned long> xorg3(42);
  std::experimental::xorshift128_engine<unsigned long> xorgex(42);
  for (auto& i : xorshift128_results)
    result = result && xorg3() == xorgex();
  return result;
}

bool _random_dbe()
{
  bool result = true;
  std::array<std::xorshift128_engine<unsigned long>::result_type, 3>
    ranlux24_results{ 5446415,6109456,7575311 };
  std::discard_block_engine<std::ranlux24_base, 2, 1> ranlux24;
  for (auto& i : ranlux24_results)
  {
    result = result && i == ranlux24();
  }

  return result;
}

bool _random_exp()
{
  bool result = true;
  std::experimental::xorshift128_engine<unsigned long> xorg0;
  std::xorshift128_engine<unsigned long> xorg1;
  for (auto& i : { 1,2,3,4,5 })
  {
    result = result && xorg0() == xorg1();
  }
  return result;
}

bool _random_ibe()
{
  bool result = true;
  
  std::independent_bits_engine<std::minstd_rand, 16, std::minstd_rand::result_type> 
    ibe, ibe2(42), ibe3(std::minstd_rand(42)), ibe4, ibe5(ibe4());
  std::minstd_rand lcg;
  ibe5.discard(64);
  for (int i = 0; i < 5; ++i)
  {
    std::minstd_rand::result_type r = lcg();
    r >>= 16;
    result = result && static_cast<std::minstd_rand::result_type>(r) == ibe();
    result = result && ibe2() == ibe3() && ibe2 == ibe3;
  }
  for (int i = 0; i < 5; ++i)
    Serial.println(ibe5());
  ibe5.seed(ibe4());
  for (int i = 0; i < 5; ++i)
    Serial.println(ibe5());

  return result;
}

void setup() 
{
  Serial.begin(9600);
  bool b0 = _random_lce(), b1 = _random_xor(), b2 = _random_dbe(), b3 = _random_exp(), b4 = _random_ibe();
  Serial.print("_random_lce() = "); Serial.println(b0 ? "OK" : "FAIL");
  Serial.print("_random_xor() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_random_dbe() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_random_exp() = "); Serial.println(b3 ? "OK" : "FAIL");
  Serial.print("_random_ibe() = "); Serial.println(b4 ? "OK" : "FAIL");
}

void loop() 
{

}
