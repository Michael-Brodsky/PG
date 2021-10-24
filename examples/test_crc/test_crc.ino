// This program validates all of the crc Standard Parameterized Algorithms
// defined in <lib/crc.h>.

#include <pg.h>
#include <lib/crc.h>

template<class T>
void crcCheck(pg::crc_traits<T> crc, const char* name = "")
{
  unsigned char* str = (unsigned char*)"123456789XXXX";
  typename pg::crc_traits<T>::value_type lut[256];
  pg::crc_lut(lut, lut + 256, crc.poly);
  auto r_poly = pg::crc(str, str + 9, crc);
  auto r_lut = pg::crc(str, str + 9, lut, crc);

  Serial.print(name); Serial.print(":\t");
  Serial.println(r_poly == r_lut && r_lut == crc.check ? "OK" : "FAIL");
}
void setup() 
{
  Serial.begin(115200);
  crcCheck(pg::crc_8_dallas_1_wire(),"crc_8_dallas_1_wire"); 
  crcCheck(pg::crc_8_ccitt(),"crc_8_ccitt");
  crcCheck(pg::crc_16(),"crc_16");
  crcCheck(pg::crc_16_usb(),"crc_16_usb");
  crcCheck(pg::crc_16_modbus(),"crc_16_modbus");
  crcCheck(pg::crc_16_genibus(),"crc_16_genibus");
  crcCheck(pg::crc_16_r_crc(),"crc_16_r_crc");
  crcCheck(pg::crc_16_kermit(),"crc_16_kermit");
  crcCheck(pg::crc_16_x_25(), "crc_16_x_25");
  crcCheck(pg::crc_32(), "crc_32");
  crcCheck(pg::crc_8(), "crc_8");
  crcCheck(pg::crc_8_bluetooth(), "crc_8_bluetooth"); 
  crcCheck(pg::crc_8_cdma2000(), "crc_8_cdma2000"); 
  crcCheck(pg::crc_8_darc(), "crc_8_darc"); 
  crcCheck(pg::crc_8_dvb_s2(), "crc_8_dvb_s2"); 
  crcCheck(pg::crc_8_gsm_a(), "crc_8_gsm_a");
  crcCheck(pg::crc_8_gsm_b(), "crc_8_gsm_b");
  crcCheck(pg::crc_8_wcdma(), "crc_8_wcdma");
  crcCheck(pg::crc_16_cdma2000(), "crc_16_cdma2000");
  crcCheck(pg::crc_16_x_crc(), "crc_16_x_crc"); 
  crcCheck(pg::crc_16_ibm_3740(), "crc_16_ibm_3740"); 
  crcCheck(pg::crc_16_aug_ccitt(), "crc_16_aug_ccitt");
  crcCheck(pg::crc_16_xmodem(), "crc_16_xmodem");
  crcCheck(pg::crc_32_bzip2(), "crc_32_bzip2");
  crcCheck(pg::crc_32_mpeg_2(), "crc_32_mpeg_2");
  crcCheck(pg::crc_32_posix(), "crc_32_posix"); 
  crcCheck(pg::crc_8_lte(), "crc_8_lte"); 
  crcCheck(pg::crc_8_itu(), "crc_8_itu"); 
}

void loop() 
{

}
