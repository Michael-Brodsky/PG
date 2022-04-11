#if !defined __PG_WIFI_H
# define __PG_WIFI_H 20220406L
# include <SPI.h>
# if defined ARDUINO_AVR_UNO_WIFI_REV2
#  include <WiFiNINA.h>
#  include <WiFiUdp.h>
using ssid_t = const char*;
# else 
#  include <WiFiUdp.h>
#  include <WiFiServer.h>
#  include <WiFiClient.h>
#  include <WiFi.h>
using ssid_t = char*;
# endif // defined __MEGAAVR_ATmega4809__
#endif // !defined __PG_WIFI_H
