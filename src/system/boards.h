/*
 *	This file is part of the Pg hardware support library.
 *
 *	***************************************************************************
 *
 *	File: boards.h
 *	Date: October 2, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2021 Michael Brodsky
 *
 *	***************************************************************************
 *
 *  This file is part of "Pretty Good" (Pg). "Pg" is free software:
 *	you can redistribute it and/or modify it under the terms of the
 *	GNU General Public License as published by the Free Software Foundation,
 *	either version 3 of the License, or (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *	WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *	along with this file. If not, see <http://www.gnu.org/licenses/>.
 *
 *	**************************************************************************
 *
 *	Description:
 *
 *	This file defines a collection of types that identify Arduino-compatible 
 *	boards and their hardware traits.
 *
 *	***************************************************************************/

#if !defined __PG_BOARDS_H
# define __PG_BOARDS_H 20211002L

# include "system/types.h"

# if defined __PG_HAS_NAMESPACES

namespace pg
{
#pragma region boards

	//
	//		Tag types that identify known boards:
	//

	struct Adafruit_32u4_Breakout {};
	struct Adafruit_BlueFruit {};
	struct Adafruit_Feather {};
	struct Adafruit_Flora {};
	struct Adafruit_Metro {};
	struct Adafruit_ProTrinket_3 {};
	struct Adafruit_ProTrinket_5 {};
	struct Adafruit_ProTrinket3_Ftdi {};
	struct Adafruit_ProTrinket5_Ftdi {};
	struct Adafruit_Trinket_3 {};
	struct Adafruit_Trinket_5 {};
	struct Arduboy {};
	struct Arduboy_Devkit {};
	struct Arduino_101 {};
	struct Arduino_BT {};
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
	struct Arduino_Circuit_Play_Expr {};
	struct Arduino_Due {};
	struct Arduino_Linino_One {};
	struct Arduino_Mega {};
	struct Arduino_MegaAdk {};
	struct Arduino_Mega_2560 {};
	struct Arduino_Micro {};
	struct Arduino_Mini {};
	struct Arduino_MKR_1000 {};
	struct Arduino_MKR_1200 {};
	struct Arduino_MKR_1300 {};
	struct Arduino_MKR_1400 {};
	struct Arduino_MKR_Zero {};
	struct Arduino_Nano {};
	struct Arduino_Nano_32 {};
	struct Arduino_Nano_Ble {};
	struct Arduino_Nano_Ble_2 {};
	struct Arduino_Nano_Every {};
	struct Arduino_NG {};
	struct Arduino_Portenta_H7 {};
	struct Arduino_Pro {};
	struct Arduino_Robot_Ctrl {};
	struct Arduino_Robot_Motor {};
	struct Arduino_Tian {};
	struct Arduino_Uno {};
	struct Arduino_Uno_Wifi {};
	struct Arduino_Yun {};
	struct Arduino_Yun_Mini {};
	struct Arduino_Zero {};
	struct Digispark {};
	struct Digispark_Pro {};
	struct Teensy_2_0 {};
	struct Teensy_plusplus_2_0 {};
	struct Teensy_3_0 {};
	struct Teensy_LC {};
	struct Teensy_3_2 {}; // 3.1/3.2
	struct Teensy_3_5 {};
	struct Teensy_3_6 {};
	struct Teensy_4_0 {};
	struct Teensy_4_1 {};

#pragma endregion
#pragma region board_traits

	//
	// 	   Board hardware traits types. 
	// 
		    
	// Primary template for board hardware traits.
	template<class T>
	struct board_traits
	{
		static constexpr uint8_t adc_digits = T::adc_digits;
		static constexpr frequency_t pwm_frequency(pin_t pin) { return T::pwm_frequency(pin); }
		static constexpr uint8_t pwm_timer(pin_t pin) { return T::pwm_timer(pin); }
		static constexpr frequency_t clock_frequency = T::clock_frequency;
	};

	template<>
	struct board_traits<Arduino_Uno>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin) 
		{ 
			return pin == 5 || pin == 6
				? 980.392
				: pin == 3 || pin == 9 || pin == 10 || pin == 11
					? 490.196
					: 0;
		}
		static constexpr uint8_t pwm_timer(pin_t pin) 
		{ 
			return pin == 5 || pin == 6 
				? 0 
				: pin == 9 || pin == 10
					? 1 
					: pin == 3 || pin == 11 
						? 2 
						: InvalidPin;
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Nano>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin) 
		{	
			return board_traits<Arduino_Uno>::pwm_frequency(pin); 
		}
		static constexpr uint8_t pwm_timer(pin_t pin)
		{
			return board_traits<Arduino_Uno>::pwm_timer(pin);
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Mini>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin) 
		{ 
			return board_traits<Arduino_Uno>::pwm_frequency(pin); 
		}
		static constexpr uint8_t pwm_timer(pin_t pin)
		{
			return board_traits<Arduino_Uno>::pwm_timer(pin);
		}
		static constexpr frequency_t clock_frequency = 16000000; // 8MHz for 3.3V, detection undefined.
	};

	template<>
	struct board_traits<Arduino_Pro>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return board_traits<Arduino_Mini>::pwm_frequency(pin);
		}
		static constexpr uint8_t pwm_timer(pin_t pin)
		{
			return board_traits<Arduino_Mini>::pwm_timer(pin);
		}
		static constexpr frequency_t clock_frequency = 16000000; // 8MHz for 3.3V, detection undefined.
	};

	template<>
	struct board_traits<Arduino_Mega>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return pin == 4 || pin == 13
				? 980.392
				: (pin >= 2 && pin < 13) || (pin >= 44 && pin <= 46)
					? 490.196
					: 0;
		}
		static constexpr uint8_t pwm_timer(pin_t pin)
		{ 
			return pin == 4 || pin == 13
				? 0
				: pin == 11 || pin == 12
					? 1
					: pin == 9 || pin == 10
						? 2
						: pin == 2 || pin == 3 || pin == 5
							? 3
							: (pin >= 6 && pin < 8)
								? 4
								: (pin >= 44 && pin <= 46)
									? 5
									: InvalidPin;
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Mega_2560>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin) 
		{ 
			return board_traits<Arduino_Mega>::pwm_frequency(pin); 
		}
		static constexpr uint8_t pwm_timer(pin_t pin)
		{
			return board_traits<Arduino_Mega>::pwm_timer(pin);
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Leonardo>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return pin == 3 || pin == 11
				? 980.392
				: pin == 5 || pin == 6 || pin == 9 || pin == 10 || pin == 13
					? 490.196
					: 0;
		}
		static constexpr uint8_t pwm_timer(pin_t pin)
		{
			return pin == 3 || pin == 11
				? 0
				: pin == 9 || pin == 10
					? 1
					: pin == 5 
						? 3
						: pin == 6 || pin == 13
							? 4
							: InvalidPin;
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Micro>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return board_traits<Arduino_Leonardo>::pwm_frequency(pin);
		}
		static constexpr frequency_t pwm_timer(pin_t pin)
		{
			return board_traits<Arduino_Leonardo>::pwm_timer(pin);
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Yun>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return board_traits<Arduino_Leonardo>::pwm_frequency(pin);
		}
		static constexpr frequency_t pwm_timer(pin_t pin)
		{
			return board_traits<Arduino_Leonardo>::pwm_timer(pin);
		}

		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Uno_Wifi>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10
				? 976.5625
				: 0;
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Duemilanove>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10 || pin == 11
				? 976.5625
				: 0;
		}
		static constexpr uint8_t pwm_timer(pin_t pin)
		{
			return board_traits<Arduino_Uno>::pwm_timer(pin);
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Arduino_Nano_Every>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return board_traits<Arduino_Uno_Wifi>::pwm_frequency(pin);
		}
	};
	
	template<>
	struct board_traits<Arduino_101>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return pin == 5 || pin == 6
				? 980.392
				: pin == 3 || pin == 9 
				? 490.196
				: 0;
		}
		static constexpr frequency_t clock_frequency = 32000000;
	}; 

	template<>
	struct board_traits<Arduino_Due>
	{
		static constexpr uint8_t adc_digits = 12;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return pin >= 2 && pin <= 13
				? 1000.
				: 0;
		}
		static constexpr frequency_t clock_frequency = 84000000;
	};

	template<>
	struct board_traits<Arduino_Zero>
	{
		static constexpr uint8_t adc_digits = 12;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return (pin >= 3 && pin <= 13) 
				? 732.
				: 0;
		}
		static constexpr frequency_t clock_frequency = 48000000;
	};
	template<>
	struct board_traits<Arduino_MKR_1000>
	{
		static constexpr uint8_t adc_digits = 12;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return (pin >= 0 && pin <= 8) || pin == 10 || pin == 11 || pin == A3 || pin == A4 
				? 732.
				: 0;
		}
		static constexpr frequency_t clock_frequency = 48000000;
	};

	template<>
	struct board_traits<Arduino_MKR_Zero>
	{
		static constexpr uint8_t adc_digits = 12;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return (pin >= 0 && pin <= 8) || pin == 10 
				? 732.
				: 0;
		}
		static constexpr frequency_t clock_frequency = 48000000;
	};

	template<>
	struct board_traits<Arduino_MKR_1200>
	{
		static constexpr uint8_t adc_digits = 12;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return board_traits<Arduino_MKR_Zero>::pwm_frequency(pin);
		}
		static constexpr frequency_t clock_frequency = 48000000;
	};

	template<>
	struct board_traits<Arduino_MKR_1300>
	{
		static constexpr uint8_t adc_digits = 12;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return board_traits<Arduino_MKR_Zero>::pwm_frequency(pin);
		}
		static constexpr frequency_t clock_frequency = 48000000;
	};

	template<>
	struct board_traits<Arduino_MKR_1400>
	{
		static constexpr uint8_t adc_digits = 12;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return board_traits<Arduino_MKR_Zero>::pwm_frequency(pin);
		}
		static constexpr frequency_t clock_frequency = 48000000;
	};

	template<>
	struct board_traits<Teensy_2_0>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return  pin == 4 || pin == 9 || pin == 10 || pin == 12 || pin == 14 || pin == 15
				? 3921.57
				: pin == 5 
					? 976.56
					: 0;
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Teensy_plusplus_2_0>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return  pin == 1 || pin == 14 || pin == 15 || pin == 16 || (pin >= 24 && pin <= 27)
				? 3921.57
				: pin == 0
					? 976.56
					: 0;
		}
		static constexpr frequency_t clock_frequency = 16000000;
	};

	template<>
	struct board_traits<Teensy_3_0>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return  pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10 || (pin >= 20 && pin <= 23)
				? 488.28
				: 0;
		}
		static constexpr frequency_t clock_frequency = 48000000;
	};

	template<>
	struct board_traits<Teensy_LC>
	{
		static constexpr uint8_t adc_digits = 12;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return  pin == 3 || pin == 4 || pin == 6 || pin == 9 || pin == 10 || 
				pin == 16 || pin == 17 || pin == 20 || pin == 22 || pin == 23
				? 488.28
				: 0;
		}
		static constexpr frequency_t clock_frequency = 48000000;
	};

	template<>
	struct board_traits<Teensy_3_2>
	{
		static constexpr uint8_t adc_digits = 13;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return  pin == 3 || pin == 4 || pin == 5 || pin == 6 || pin == 9 || pin == 10 || 
				(pin >= 20 && pin <= 23) || pin == 25 || pin == 32
				? 488.28
				: 0;
		}
		static constexpr frequency_t clock_frequency = 72000000;
	};

	template<>
	struct board_traits<Teensy_3_5>
	{
		static constexpr uint8_t adc_digits = 13;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return  (pin >= 2 && pin <= 10) || pin == 14 || (pin >= 20 && pin <= 23) || pin == 29 || pin == 30 ||
				(pin >= 35 && pin <= 38) 
				? 488.28
				: 0;
		}
		static constexpr frequency_t clock_frequency = 120000000;
	};

	template<>
	struct board_traits<Teensy_3_6>
	{
		static constexpr uint8_t adc_digits = 13;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return  (pin >= 2 && pin <= 10) || pin == 14 || pin == 16 || pin == 17 || 
				(pin >= 20 && pin <= 23) || pin == 29 || pin == 30 || (pin >= 35 && pin <= 38)
				? 488.28
				: 0;
		}
		static constexpr frequency_t clock_frequency = 180000000;
	};

	template<>
	struct board_traits<Teensy_4_0>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return (pin >= 10 && pin <= 15) || pin == 18 || pin == 19   
				? 3611.
				: (pin >= 0 && pin <= 9) || (pin >= 22 && pin <= 25) || pin == 28 || pin == 29 || (pin >= 33 && pin <= 39) 
					? 4482.
					: 0;
		}
		static constexpr frequency_t clock_frequency = 600000000;
	};

	template<>
	struct board_traits<Teensy_4_1>
	{
		static constexpr uint8_t adc_digits = 10;
		static constexpr frequency_t pwm_frequency(pin_t pin)
		{
			return  (pin >= 10 && pin <= 15) || pin == 18 || pin == 19  
				? 3611. 
				: (pin >= 22 && pin <= 25) || pin == 28 || pin == 29 || pin == 33 || pin == 36 || pin == 37 || 
				(pin >= 42 && pin <= 47) || pin == 51 || pin == 54 
					? 4482.
					: 0;
		}
		static constexpr frequency_t clock_frequency = 600000000;
	};

#pragma endregion
#pragma region board_ident 

#  if defined ARDUINO
#   if defined(TEENSYDUINO)	// --- Teensy ---
#    if defined(__AVR_ATmega32U4__)
		using board_type = Teensy_2_0;
#    elif defined(__AVR_AT90USB1286__) 
		using board_type = Teensy_plusplus_2_0;
#    elif defined(__MK20DX128__) 
		using board_type = Teensy_3_0;
#    elif defined(__MK20DX256__)    
		using board_type = Teensy_3_2;
#    elif defined(__MKL26Z64__) 
		using board_type = Teensy_LC;
#    elif defined(__MK64FX512__)
		using board_type = Teensy_3_5;
#    elif defined(__MK66FX1M0__)
		using board_type = Teensy_3_6;
#    elif (defined(__IMXRT1052__) || defined(__IMXRT1062__))
		using board_type = Teensy_4_0;
#    endif // defined(__AVR_ATmega32U4__)				
#   else					// --- Arduino ---
#    if defined(ARDUINO_AVR_ADK)       
		using board_type = Arduino_MegaAdk;
#    elif defined(ARDUINO_AVR_ADAFRUIT32U4)       
		using board_type = Adafruit_32u4_Breakout;
#    elif defined(ARDUINO_AVR_ARDUBOY)       
		using board_type = Arduboy;
#    elif defined(ARDUINO_AVR_ARDUBOY_DEVKIT)       
		 board_type = Arduboy_Devkit;
#    elif defined(ARDUINO_AVR_BLUEFRUITMICRO)       
		using board_type = Adafruit_BlueFruit;
#    elif defined(ARDUINO_AVR_BT)
		using board_type = Arduino_BT;
#    elif defined(ARDUINO_AVR_CIRCUITPLAY)
		using board_type = Arduino_Circuit_Play;
#    elif defined(ARDUINO_AVR_DIGISPARK)
		using board_type = Digispark;
#    elif defined(ARDUINO_AVR_DIGISPARKPRO)
	using board_type = Digispark_Pro;
#    elif defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
		using board_type = Arduino_Circuit_Play_Expr;
#    elif defined(ARDUINO_AVR_DUEMILANOVE)       
		using board_type = Arduino_Duemilanove;
#    elif defined(ARDUINO_AVR_ESPLORA)       
		using board_type = Arduino_Esplora;
#    elif defined(ARDUINO_AVR_ETHERNET)       
		using board_type = Arduino_Ethernet;
#    elif defined(ARDUINO_AVR_FEATHER32U4)       
		using board_type = Adafruit_Feather;
#    elif defined(ARDUINO_AVR_FIO)       
		using board_type = Arduino_Fio;
#    elif defined(ARDUINO_AVR_FLORA8)       
		using board_type = Adafruit_Flora;
#    elif defined(ARDUINO_AVR_GEMMA)
		using board_type = Arduino_Gemma;
#    elif defined(ARDUINO_AVR_LEONARDO)       
		using board_type = Arduino_Leonardo;
#    elif defined(ARDUINO_AVR_LEONARDO_ETH)
		using board_type = Arduino_Leonardo_Eth;
#    elif defined(ARDUINO_AVR_LILYPAD)
		using board_type = Arduino_Lilypad;
#    elif defined(ARDUINO_AVR_LILYPAD_USB)
		using board_type = Arduino_Lilypad_Usb;
#    elif defined(ARDUINO_AVR_MEGA)       
		using board_type = Arduino_Mega;
#    elif defined(ARDUINO_AVR_MEGA2560)       
		using board_type = Arduino_Mega_2560;
#    elif defined(ARDUINO_AVR_METRO)       
		using board_type = Adafruit_Metro;
#    elif defined(ARDUINO_AVR_MICRO)       
		using board_type = Arduino_Micro;
#    elif defined(ARDUINO_AVR_MINI)       
		using board_type = Arduino_Mini;
#    elif defined(ARDUINO_AVR_NANO)       
		using board_type = Arduino_Nano;
#    elif defined(ARDUINO_AVR_NANO_EVERY)       
		using board_type = Arduino_Nano_Every;
#    elif defined(ARDUINO_AVR_PROTRINKET3)       
	using board_type = Adafruit_ProTrinket_3;
#    elif defined(ARDUINO_AVR_PROTRINKET5)       
	using board_type = Adafruit_ProTrinket_5;
#    elif defined(ARDUINO_AVR_PROTRINKET3FTDI)       
	using board_type = Adafruit_ProTrinket3_Ftdi;
#    elif defined(ARDUINO_AVR_PROTRINKET5FTDI)       
	using board_type = Adafruit_ProTrinket5_Ftdi;
#    elif defined(ARDUINO_AVR_TRINKET3)       
	using board_type = Adafruit_Trinket_3;
#    elif defined(ARDUINO_AVR_TRINKET5)       
	using board_type = Adafruit_Trinket_5;
#    elif defined(ARDUINO_NANO32)
		using board_type = Arduino_Nano_32;
#    elif defined(ARDUINO_RB_BLE_NANO_2)
		using board_type = Arduino_Nano_Ble_2;
#    elif defined(ARDUINO_BLE_NANO)
		using board_type = Arduino_Nano_Ble;
#    elif defined(ARDUINO_AVR_NG)       
		using board_type = Arduino_NG;
#    elif defined(ARDUINO_AVR_PRO)       
		using board_type = Arduino_Pro;
#    elif defined(ARDUINO_AVR_ROBOT_CONTROL)       
		using board_type = Arduino_Robot_Ctrl;
#    elif defined(ARDUINO_AVR_ROBOT_MOTOR)       
		using board_type = Arduino_Robot_Motor;
#    elif defined(ARDUINO_AVR_UNO)  
		using board_type = Arduino_Uno;
#    elif defined(ARDUINO_AVR_UNO_WIFI_DEV_ED)
		using board_type = Arduino_Uno_Wifi;
#    elif defined(ARDUINO_AVR_YUN)       
		using board_type = Arduino_Yun;
#    elif defined(ARDUINO_AVR_YUNMINI)
		using board_type = Arduino_Yun_Mini;
#    elif defined(ARDUINO_SAMD_TIAN)
		using board_type = Arduino_Tian;
#    elif defined(ARDUINO_AVR_LININO_ONE)
		using board_type = Arduino_Linino_One;
#    elif defined(ARDUINO_SAM_DUE)       
		using board_type = Arduino_Due;
#    elif defined(ARDUINO_SAMD_ZERO)       
		using board_type = Arduino_Zero;
#    elif defined(ARDUINO_SAMD_MKR1000)
		using board_type = Arduino_MKR_1000;
#    elif defined(ARDUINO_SAMD_MKRZERO)
		using board_type = Arduino_MKR_Zero;
#    elif defined(ARDUINO_SAMD_MKRFox1200)
		using board_type = Arduino_MKR_1200;
#    elif defined(ARDUINO_SAMD_MKRGSM1400)
		using board_type = Arduino_MKR_1400;
#    elif defined(ARDUINO_SAMD_MKRWAN1300)
		using board_type = Arduino_MKR_1300;
#    elif defined(ARDUINO_AVR_INDUSTRIAL101)
		using board_type = Arduino_101;
#    elif defined(ARDUINO_PORTENTA_H7_M7)
		using board_type = Arduino_Portenta_H7;
#    endif // defined(ARDUINO_AVR_ADK) 
#   endif // defined TEENSYDUINO
#  endif // defined ARDUINO

#pragma endregion
#pragma region misc_funcs

	template<class T>
	constexpr analog_t AnalogMax() { return (analog_t(1) << board_traits<T>::adc_digits) - 1; }

	template<class T>
	constexpr frequency_t ClockPeriod() { return 1 / board_traits<T>::clock_frequency; }

#pragma endregion

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_BOARDS_H