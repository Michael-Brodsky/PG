// Part of the Pg test suite for <complex>

#include <pg.h>
#include <complex>

template <class T>
void print_complex(const std::complex<T>& z) {Print(z.real()); Print(", "); PrintLn(z.imag()); }

bool _complex()
{
  bool result = true;
  std::complex<double> first(2.0, 2.0);
  std::complex<double> second(first);
  std::complex<long double> third(second);
  result = result && third.imag() == 2.0 && third.real() == 2.0;

  return result;
}

bool _complex_members()
{
  bool result = true;
  std::complex<double> mycomplex, complex2;
  mycomplex.real(42.0);
  mycomplex.imag(23.0);
  complex2 = mycomplex;
  result = result && complex2 == mycomplex;
  mycomplex = 10.0;   // 10
  mycomplex += 2.0;   // 12
  result = result && mycomplex == std::complex<double>(12.0);
  mycomplex = std::complex<double>(10.0, 1.0);  // 10+i
  mycomplex = mycomplex + 10.0;                 // 20+i;
  result = result && mycomplex == std::complex<double>(20.0, 1.0);

  return result;
}

bool _complex_nonmembers()
{
  bool result = true;
  std::complex<double> mycomplex(3.0, 4.0), complex2 = mycomplex;
  result = result && std::conj(mycomplex) == std::complex<double>(3.0, -4.0);
  result = result && std::abs(mycomplex) == 5.0;
  result = result && mycomplex * 2.0 == std::complex<double>(6.0, 8.0);
  mycomplex *= 2.0;
  result = result && mycomplex == std::complex<double>(6.0, 8.0) && mycomplex != complex2 && mycomplex == complex2 * 2.0;
  complex2 = mycomplex / 4.0;
  mycomplex /= 4.0;
  result = result && mycomplex == complex2;
  std::complex<double> another = mycomplex * complex2;
  result = result && another == std::complex<double>(-1.75, 6.0);
  result = result && (another -= std::complex<double>(-1.75, 6.0)) == std::complex<double>();

  return result;
}

void setup() 
{
  Serial.begin(9600);
  bool b1 = _complex(), b2 = _complex_members(), b3 = _complex_nonmembers();
  Serial.print("_complex() = "); Serial.println(b1 ? "OK" : "FAIL");
  Serial.print("_members() = "); Serial.println(b2 ? "OK" : "FAIL");
  Serial.print("_nonmembers() = "); Serial.println(b3 ? "OK" : "FAIL");
}

void loop() 
{

}
