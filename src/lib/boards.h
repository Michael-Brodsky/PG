#if !defined __PG_BOARDS_H
# define __PG_BOARDS_H 20211002L

# include "cstdint"
# include "pg.h"

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	struct Teensy_2_0 {};
	struct Teensy_plusplus_2_0 {};
	struct Teensy_3_0 {};
	struct Teensy_LC {};
	struct Teensy_3_2 {};
	struct Teensy_3_5 {};
	struct Teensy_3_6 {};
	struct Teensy_4_0 {};
	struct Teensy_4_1 {};
	struct Arduino_MegaAdk {};
	struct Arduino_Bluetooth {};
	struct Arduino_Duemilanove {};
	struct Arduino_Esplora {};
	struct Arduino_Ethernet {};
	struct Arduino_Fio {};
	struct Arduino_Gemma {};
	struct Arduino_Leonardo {};
	struct Arduino_Leonardo_Eth {};
	struct Arduino_Lilypad {};
	struct Arduino_Lilypad_Usb {};
	struct Arduino_Circuit_Play {};
	struct Arduino_Mega {};
	struct Arduino_Mega_2560 {};
	struct Arduino_Micro {};
	struct Arduino_Mini {};
	struct Arduino_Nano {};
	struct Arduino_Nano_32 {};
	struct Arduino_Nano_Ble {};
	struct Arduino_Nano_Ble_2 {};
	struct Arduino_NG {};
	struct Arduino_Pro {};
	struct Arduino_Robot_Ctrl {};
	struct Arduino_Robot_Motor {};
	struct Arduino_Uno {};
	struct Arduino_Uno_Wifi {};
	struct Arduino_Yun {};
	struct Arduino_Yun_Mini {};
	struct Arduino_Due {};
	struct Arduino_Zero {};
	struct Arduino_Linino_One {};
	struct Arduino_101 {};
	struct Arduino_MKR_1000 {};
	struct Arduino_MKR_1200 {};
	struct Arduino_MKR_1300 {};
	struct Arduino_MKR_1400 {};
	struct Arduino_MKR_Zero {};
	struct Arduino_Nano33 {};
	struct Arduino_Portenta_H7 {};

	template<class T>
	struct board_traits
	{
		static constexpr uint8_t adc_digits = 10;
	};

	template<>
	struct board_traits<Teensy_LC>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Teensy_3_2>
	{
		static constexpr uint8_t adc_digits = 13;
	};

	template<>
	struct board_traits<Teensy_3_5>
	{
		static constexpr uint8_t adc_digits = 13;
	};

	template<>
	struct board_traits<Teensy_3_6>
	{
		static constexpr uint8_t adc_digits = 13;
	};

	template<>
	struct board_traits<Arduino_Zero>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_Due>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_MKR_1000>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_MKR_1200>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_MKR_1300>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_MKR_1400>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_MKR_Zero>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_Nano_32>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_Nano_Ble>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_Nano_Ble_2>
	{
		static constexpr uint8_t adc_digits = 12;
	};

	template<>
	struct board_traits<Arduino_Portenta_H7>
	{
		static constexpr uint8_t adc_digits = 16;
	};

# if defined ARDUINO
#  if defined(TEENSYDUINO)	// --- Teensy ---
#   if defined(__AVR_ATmega32U4__)
	using board_type = Teensy_2_0;
#   elif defined(__AVR_AT90USB1286__) 
	using board_type = Teensy_plusplus_2_0;
#   elif defined(__MK20DX128__) 
	using board_type = Teensy_3_0;
#   elif defined(__MK20DX256__)    
	using board_type = Teensy_3_2;
#   elif defined(__MKL26Z64__) 
	using board_type = Teensy_LC;
#   elif defined(__MK64FX512__)
	using board_type = Teensy_3_5;
#   elif defined(__MK66FX1M0__)
	using board_type = Teensy_3_6;
#   elif (defined(__IMXRT1052__) || defined(__IMXRT1062__))
	using board_type = Teensy_4_0;
#   endif
#  else						// --- Arduino ---
#   if defined(ARDUINO_AVR_ADK)       
	using board_type = Arduino_MegaAdk;
#   elif defined(ARDUINO_AVR_BT)
	using board_type = Arduino_Bluetooth;
#   elif defined(ARDUINO_AVR_DUEMILANOVE)       
	using board_type = Arduino_Duemilanove;
#   elif defined(ARDUINO_AVR_ESPLORA)       
	using board_type = Arduino_Esplora;
#   elif defined(ARDUINO_AVR_ETHERNET)       
	using board_type = Arduino_Ethernet;
#   elif defined(ARDUINO_AVR_FIO)       
	using board_type = Arduino_Fio;
#   elif defined(ARDUINO_AVR_GEMMA)
	using board_type = Arduino_Gemma;
#   elif defined(ARDUINO_AVR_LEONARDO)       
	using board_type = Arduino_Leonardo;
#   elif defined(ARDUINO_AVR_LEONARDO_ETH)
	using board_type = Arduino_Leonardo_Eth;
#   elif defined(ARDUINO_AVR_LILYPAD)
	using board_type = Arduino_Lilypad;
#   elif defined(ARDUINO_AVR_LILYPAD_USB)
	using board_type = Arduino_Lilypad_Usb;
#   elif defined(ARDUINO_AVR_CIRCUITPLAY)
	using board_type = Arduino_Circuit_Play;
#   elif defined(ARDUINO_AVR_MEGA)       
	using board_type = Arduino_Mega;
#   elif defined(ARDUINO_AVR_MEGA2560)       
	using board_type = Arduino_Mega_2560;
#   elif defined(ARDUINO_AVR_MICRO)       
	using board_type = Arduino_Micro;
#   elif defined(ARDUINO_AVR_MINI)       
	using board_type = Arduino_Mini;
#   elif defined(ARDUINO_AVR_NANO)       
	using board_type = Arduino_Nano;
#   elif defined(ARDUINO_NANO32)
	using board_type = Arduino_Nano_32;
#   elif defined(ARDUINO_RB_BLE_NANO_2)
	using board_type = Arduino_Nano_Ble_2;
#   elif defined(ARDUINO_BLE_NANO)
	using board_type = Arduino_Nano_Ble;
#   elif defined(ARDUINO_AVR_NG)       
	using board_type = Arduino_NG;
#   elif defined(ARDUINO_AVR_PRO)       
	using board_type = Arduino_Pro;
#   elif defined(ARDUINO_AVR_ROBOT_CONTROL)       
	using board_type = Arduino_Robot_Ctrl;
#   elif defined(ARDUINO_AVR_ROBOT_MOTOR)       
	using board_type = Arduino_Robot_Motor;
#   elif defined(ARDUINO_AVR_UNO)  
	using board_type = Arduino_Uno;
#   elif defined(ARDUINO_AVR_UNO_WIFI_DEV_ED)
	using board_type = Arduino_Uno_Wifi;
#   elif defined(ARDUINO_AVR_YUN)       
	using board_type = Arduino_Yun;
#   elif defined(ARDUINO_AVR_YUNMINI)
	using board_type = Arduino_Yun_Mini;
#   elif defined(ARDUINO_AVR_LININO_ONE)
	using board_type = Arduino_Linino_One;
#   elif defined(ARDUINO_SAM_DUE)       
	using board_type = Arduino_Due;
#   elif defined(ARDUINO_SAMD_ZERO)       
	using board_type = Arduino_Zero;
#   elif defined(ARDUINO_SAMD_MKR1000)
	using board_type = Arduino_MKR_1000;
#   elif defined(ARDUINO_SAMD_MKRZERO)
	using board_type = Arduino_MKR_Zero;
#   elif defined(ARDUINO_SAMD_MKRFox1200)
	using board_type = Arduino_MKR_1200;
#   elif defined(ARDUINO_SAMD_MKRGSM1400)
	using board_type = Arduino_MKR_1300;
#   elif defined(ARDUINO_SAMD_MKRWAN1300)
	using board_type = Arduino_MKR_1400;
#   elif defined(ARDUINO_AVR_INDUSTRIAL101)
	using board_type = Arduino_101;
#   elif defined(ARDUINO_PORTENTA_H7_M7)
	using board_type = Arduino_Portenta_H7;
#   endif
#  endif
# endif // defined ARDUINO

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_BOARDS_H
