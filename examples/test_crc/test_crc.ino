#include <pg.h>
#include <lib/crc.h>

uint8_t stream1[] = {9,23,42,99,255,254,86,127,88,43,2,0}; // Last but must be 0 for CRC-8 remainder.
uint8_t stream2[] = {0,0,0,0,0,0}; // Zero block to verify checksum != 0.
uint8_t lut[256]; // Lookup table to run faster CRC-8 algo.
uint8_t poly = 0xD8;  // CRC-8 polynomial.
uint8_t cs = pg::checksum(stream2);

void setup() 
{
  Serial.begin(115200);
  pg::crc_lut(lut, lut + 256, poly);
  stream1[11] = pg::crc8(stream1, stream1 + 12, poly, lut); // Remainder replaces 0 in last byte. Another pass returns 0 if stream OK.
  Serial.print("Checksum for zero block != 0: "); Serial.println(cs != 0 ? "OK" : "FAIL");
  Serial.print("CRC-8 for Stream1: "); Serial.println(pg::crc8(stream1, stream1 + 12, poly, lut) == 0 ? "OK" : "FAIL");
}

void loop() 
{

}
