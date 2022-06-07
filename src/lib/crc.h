/*
 *	This file defines a library of cyclic-redundancy-check (CRC) and checksum 
 *	algorithms.
 *
 *	***************************************************************************
 *
 *	File: crc.h
 *	Date: October 19, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky
 *
 *	***************************************************************************
 *
 *  This file is part of `Pretty Good' (Pg). `Pg' is free software:
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
 *	The crc library is a collection of common CRC and checksum algorithms 
 *	useful for validating transmission and storage streams.
 *
 *	Library Functions:
 *
 *	checksum(): generates an inverted checksum from an input stream.
 *	     crc(): generates a crc remainder from an input byte stream.
 *	 crc_lut(): generates a crc lookup table for improved performance.
 * 
 *	Standard Parameterized Algorithms:
 * 
 *	This file also defines several types that encapsulate parameters from  
 *	industry standard algorithms and can be passed as arguments to the crc() 
 *	functions. Type names have the form crc_xx_name, where xx is the width 
 *	of the generator polynomial/remainder in bits, and name is the standard 
 *	algorithm name or one of its commonly accepted aliases:
 * 
 *	crc_8
 *	crc_8_bluetooth
 *	crc_8_ccitt
 *	crc_8_cdma2000
 *	crc_8_dallas_1_wire
 *	crc_8_darc
 *	crc_8_dvb_s2
 *	crc_8_gsm_a
 *	crc_8_gsm_b
 *	crc_8_i_432_1
 *	crc_8_itu 
 *  crc_8_lte
 *  crc_8_wcdma
 *  crc_16
 *	crc_16_acorn
 *	crc_16_ansi
 *	crc_16_arc
 *	crc_16_aug_ccitt
 *	crc_16_autosar
 *	crc_16_ccitt
 *	crc_16_ccitt_false
 *	crc_16_ccitt_true
 *	crc_16_cdma2000
 *	crc_16_crc_b
 *  crc_16_dect_r
 *	crc_16_dect_x
 *	crc_16_genibus
 *  crc_16_ibm
 *	crc_16_ibm_3740
 *	crc_16_ibm_sdlc
 *	crc_16_iso_hdlc
 *	crc_16_iso_iec_14443_3b
 *	crc_16_kermit
 *	crc_16_lha
 *	crc_16_lte
 *	crc_16_modbus
 *	crc_16_r_crc;
 *	crc_16_spi_fujitsu
 *	crc_16_usb
 *	crc_16_v41_msb
 *	crc_16_x_crc
 *  crc_16_x_25
 *	crc_16_xmodem
 *	crc_16_zmodem
 *	crc_32
 *	crc_32_adccp
 *	crc_32_bzip2
 *	crc_32_cksum
 *	crc_32_iso_hdlc
 *	crc_32_mpeg_2
 *	crc_32_pkzip
 *	crc_32_posix
 *	crc_32_v42
 *	crc_32_xz
 *
 *	The checksum() functions generate a checksum value from an input stream 
 *	(the message). Messages are verified by comparing checksums. If they are 
 *	equal, the messages are identical, else they are not. The checksum() 
 *	functions operate on streams of any unsigned type and return a checksum 
 *	of the same type as the input stream. Input streams can be either arrays 
 *	or ranges:
 * 
 *		unsigned char msg[] = {"123456789"};
 *		unsigned char sum1 = checksum(msg); 
 *		unsigned char sum2 = checksum(msg, msg + 9); 
 *		unsigned char sum3 = checksum(std::begin(msg), std::end(msg)); 
 *		bool result = sum1 == sum2;
 * 
 *	The crc() functions generate a crc remainder from an input byte stream,  
 *	generator polynomial and optional CRC parameters, which can be passed 
 *	individually or as one of the standard parameterized algorithm types. 
 *	Messages are verified by appending the remainder to the original message. 
 *	If the remainder from the appended message is zero, the messages are 
 *	identical, else they are not. Messages must be padded at the end with 
 *	a number of bytes equal to the size of the polynomial (as returned by the 
 *	sizeof() operator). 
 * 
 *		uint8_t msg[] = { 1,2,3,4,5,6,7,8,9,0x00 }; // message padded w/ byte
 *		uint8_t poly = 0x07;
 *		uint8_t remainder = crc(msg, msg + 9, poly);
 *		msg[9] = remainder;						    // remainder appended to msg
 *		bool result = crc(msg, msg + 10, poly) == 0;// check if appended msg 
 *                                                  // returns 0.
 * 
 *	The crc() function overloads take optional lookup table parameters. CRC 
 *	algorithms are significantly faster when used with lookup tables, but at 
 *	the cost of extra memory occupied by the table. The crc_lut() function 
 *	generates a lookup table of the same type as a given polynomial, that 
 *	can be passed to the crc() functions. crc_lut() will generate tables of 
 *	any size, however only tables with 256 entries are supported by the crc() 
 *	functions. A table size of 256 allows the crc() functions to process 
 *	messages a byte at a time instead of a bit at a time.
 *
 *		uint8_t lut[256];
 *		uint8_t poly = 0x07;
 *		crc_lut(lut, lut + 256, poly); // tables must be passed as ranges.
 * 
 *	Lookup tables can be passed to crc() functions as an array or range:
 * 
 *		crc(msg, msg + 9, lut, lut + 256, poly); // or ...
 *		crc(msg, msg + 9, lut, poly);
 * 
 *	The optional CRC parameters can be passed as individual arguments or as 
 *	one of the standard parameterized algorithm types:
 * 
 *		uint8_t msg[] = { 1,2,3,4,5,6,7,8,9,0x00 };
 *		uint8_t poly = 0x07;
 *		uint8_t xorin = 0x00, xorout = 0x00;
 *		bool refin = false, refout = false;
 *		crc(msg, msg + 9, poly, xorin, xorout, refin, refout);
 *		crc(msg, msg + 9, crc_8);		// no lookup table.
 *		crc(msg, msg + 9, lut, crc_8);	// with lookup table.
 * 
 *	Notes:
 *
 * 	The checksum() functions operate on streams of any unsigned type, crc() 
 *	functions only operate on unsigned byte streams (unsigned char or 
 *	uint8_t). Generator polynomials must be unsigned types whose widths 
 *	are multiples of 8 bits, i.e. 8, 16, 32, etc., Lookup tables must be of 
 *	the same type as the polynomial. Messages must be padded at the end with 
 *	a number of bytes equal to the size of the polynomial (as returned by the 
 *	sizeof() operator). The minimum message "payload" (length of the unpadded 
 *	message) must be greater than or equal to the size of the polynomial, e.g. 
 *	a 32-bit polynomial has a minimum payload of four bytes). Programs 
 *	attempting to call these functions with other parameters or types are 
 *	ill-formed. 
 *
 *	**************************************************************************/

#if !defined __PG_CRC_H
# define __PG_CRC_H 20211019L

#include <array>		// iterator_traits.
#include <limits>		// numeric_limits.
#include <lib/imath.h>	// is_unsigned type.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	// The following sections contain type definitions and parameters 
	// for Standard parameterized CRC algorithms.

#pragma region forward_decls

	template<class T>
	struct crc_traits;

#pragma endregion
#pragma region crc_algorithm_tags

	// Algorithm traits tag		// Standard CRC algorithm name aliases.

	struct crc8 {};				using crc_8 = crc_traits<crc8>;
								using crc_8_ccitt = crc_8;
	struct crc8_bluetooth {};	using crc_8_bluetooth = crc_traits<crc8_bluetooth>;
	struct crc8_cdma2000 {};	using crc_8_cdma2000 = crc_traits<crc8_cdma2000>;
	struct crc8_dallas1w {};	using crc_8_dallas_1_wire = crc_traits<crc8_dallas1w>;
	struct crc8_darc {};		using crc_8_darc = crc_traits<crc8_darc>;
	struct crc8_dvbs2 {};		using crc_8_dvb_s2 = crc_traits<crc8_dvbs2>;
	struct crc8_gsma {};		using crc_8_gsm_a = crc_traits<crc8_gsma>;
	struct crc8_gsmb {};		using crc_8_gsm_b = crc_traits<crc8_gsmb>;
	struct crc8_i4321 {};		using crc_8_i_432_1 = crc_traits<crc8_i4321>;
								using crc_8_itu = crc_8_i_432_1;
	struct crc8_lte {};			using crc_8_lte = crc_traits<crc8_lte>;
	struct crc8_wcdma {};		using crc_8_wcdma = crc_traits<crc8_wcdma>;
	struct crc16 {};			using crc_16 = crc_traits<crc16>;
								using crc_16_ibm = crc_16;
								using crc_16_ansi = crc_16;
								using crc_16_arc = crc_16;
								using crc_16_lha = crc_16;
	struct crc16_cdma2000 {};	using crc_16_cdma2000 = crc_traits<crc16_cdma2000>;
	struct crc16_dectr {};		using crc_16_dect_r = crc_traits<crc16_dectr>;
								using crc_16_r_crc = crc_16_dect_r;
	struct crc16_dectx {};		using crc_16_dect_x = crc_traits<crc16_dectx>;
								using crc_16_x_crc = crc_16_dect_x;
	struct crc16_genibus {};	using crc_16_genibus = crc_traits<crc16_genibus>;
	struct crc16_ibm3740 {};	using crc_16_ibm_3740 = crc_traits<crc16_ibm3740>;
								using crc_16_ccitt_false = crc_16_ibm_3740;
								using crc_16_autosar = crc_16_ibm_3740;
	struct crc16_kermit {};		using crc_16_kermit = crc_traits<crc16_kermit>;
								using crc_16_ccitt = crc16_kermit;
								using crc_16_ccitt_true = crc16_kermit;
	struct crc16_modbus {};		using crc_16_modbus = crc_traits<crc16_modbus>;
	struct crc16_spifujitsu {};	using crc_16_spi_fujitsu = crc_traits<crc16_spifujitsu>;
								using crc_16_aug_ccitt = crc_16_spi_fujitsu;
	struct crc16_usb {};		using crc_16_usb = crc_traits<crc16_usb>;
	struct crc16_ibmsdlc {};	using crc_16_ibm_sdlc = crc_traits<crc16_ibmsdlc>;
								using crc_16_x_25 = crc_16_ibm_sdlc;
								using crc_16_iso_hdlc = crc_16_ibm_sdlc;
								using crc_16_iso_iec_14443_3b = crc_16_ibm_sdlc;
								using crc_16_crc_b = crc_16_ibm_sdlc;
	struct crc16_xmodem {};		using crc_16_xmodem = crc_traits<crc16_xmodem>;
								using crc_16_zmodem = crc_16_xmodem;
								using crc_16_acorn = crc_16_xmodem;
								using crc_16_lte = crc_16_xmodem;
								using crc_16_v41_msb = crc_16_xmodem;
	struct crc32 {};			using crc_32 = crc_traits<crc32>;
								using crc_32_iso_hdlc = crc_32;
								using crc_32_adccp = crc_32;
								using crc_32_v42 = crc_32;
								using crc_32_xz = crc_32;
								using crc_32_pkzip = crc_32;
	struct crc32_bzip2 {};		using crc_32_bzip2 = crc_traits<crc32_bzip2>;
	struct crc32_mpeg2 {};		using crc_32_mpeg_2 = crc_traits<crc32_mpeg2>;
	struct crc32_posix {};		using crc_32_posix = crc_traits<crc32_posix>;
								using crc_32_cksum = crc_32_posix;

#pragma endregion
#pragma region crc_algorithm_traits

	// Primary template for CRC algorithm traits.
	template<class T>
	struct crc_traits
	{
		using type = crc_traits<T>;
		using value_type = typename T::value_type;

		static constexpr value_type width = T::width;
		static constexpr value_type poly = T::poly;
		static constexpr value_type xorin = T::xorin;
		static constexpr bool refin = T::refin;
		static constexpr bool refout = T::refout;
		static constexpr value_type xorout = T::xorout;
		static constexpr value_type check = T::check;
	};

	template<>
	struct crc_traits<crc8>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x07;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0xF4;
	};

	template<>
	struct crc_traits<crc8_bluetooth>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0xA7;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0x26;
	};

	template<>
	struct crc_traits<crc8_cdma2000>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x9B;
		static constexpr value_type xorin = 0xFF;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0xDA;
	};

	template<>
	struct crc_traits<crc8_darc>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x39;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0x15;
	};

	template<>
	struct crc_traits<crc8_dvbs2>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0xD5;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0xBC;
	};

	template<>
	struct crc_traits<crc8_dallas1w>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x31;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0xA1;
	};

	template<>
	struct crc_traits<crc8_gsma>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x1D;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0x37;
	};

	template<>
	struct crc_traits<crc8_gsmb>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x49;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0xFF;
		static constexpr value_type check = 0x94;
	};

	template<>
	struct crc_traits<crc8_i4321>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x07;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x55;
		static constexpr value_type check = 0xA1;
	};

	template<>
	struct crc_traits<crc8_lte>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x9B;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0xEA;
	};

	template<>
	struct crc_traits<crc8_wcdma>
	{
		using value_type = uint8_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x9B;
		static constexpr value_type xorin = 0x00;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0x00;
		static constexpr value_type check = 0x25;
	};

	template<>
	struct crc_traits<crc16>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x8005;
		static constexpr value_type xorin = 0x0000;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0x0000;
		static constexpr value_type check = 0xBB3D;
	};

	template<>
	struct crc_traits<crc16_cdma2000>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0xC867;
		static constexpr value_type xorin = 0xFFFF;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x0000;
		static constexpr value_type check = 0x4C06;
	};

	template<>
	struct crc_traits<crc16_dectr> 
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x0589;
		static constexpr value_type xorin = 0x0000;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x0001;
		static constexpr value_type check = 0x007E;
	};

	template<>
	struct crc_traits<crc16_dectx>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x0589;
		static constexpr value_type xorin = 0x0000;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x0000;
		static constexpr value_type check = 0x007F;
	};

	template<>
	struct crc_traits<crc16_genibus>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x1021;
		static constexpr value_type xorin = 0xFFFF;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0xFFFF;
		static constexpr value_type check = 0xD64E;
	};

	template<>
	struct crc_traits<crc16_ibm3740>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x1021;
		static constexpr value_type xorin = 0xFFFF;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x0000;
		static constexpr value_type check = 0x29B1;
	};

	template<>
	struct crc_traits<crc16_kermit>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x1021;
		static constexpr value_type xorin = 0x0000;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0x0000;
		static constexpr value_type check = 0x2189;
	};

	template<>
	struct crc_traits<crc16_modbus>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x8005;
		static constexpr value_type xorin = 0xFFFF;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0x0000;
		static constexpr value_type check = 0x4B37;
	};

	template<>
	struct crc_traits<crc16_spifujitsu>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x1021;
		static constexpr value_type xorin = 0x1D0F;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x0000;
		static constexpr value_type check = 0xE5CC;
	};

	template<>
	struct crc_traits<crc16_usb>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x8005;
		static constexpr value_type xorin = 0xFFFF;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0xFFFF;
		static constexpr value_type check = 0xB4C8;
	};

	template<>
	struct crc_traits<crc16_ibmsdlc>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x1021;
		static constexpr value_type xorin = 0xFFFF;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0xFFFF;
		static constexpr value_type check = 0x906E;
	};

	template<>
	struct crc_traits<crc16_xmodem>
	{
		using value_type = uint16_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x1021;
		static constexpr value_type xorin = 0x0000;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x0000;
		static constexpr value_type check = 0x31C3;
	};

	template<>
	struct crc_traits<crc32>
	{
		using value_type = uint32_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x04C11DB7;
		static constexpr value_type xorin = 0xFFFFFFFF;
		static constexpr bool refin = true;
		static constexpr bool refout = true;
		static constexpr value_type xorout = 0xFFFFFFFF;
		static constexpr value_type check = 0xCBF43926;
	};

	template<>
	struct crc_traits<crc32_bzip2>
	{
		using value_type = uint32_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x04C11DB7;
		static constexpr value_type xorin = 0xFFFFFFFF;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0xFFFFFFFF;
		static constexpr value_type check = 0xFC891918;
	};

	template<>
	struct crc_traits<crc32_mpeg2>
	{
		using value_type = uint32_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x04C11DB7;
		static constexpr value_type xorin = 0xFFFFFFFF;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0x00000000;
		static constexpr value_type check = 0x0376E6E7;
	};

	template<>
	struct crc_traits<crc32_posix>
	{
		using value_type = uint32_t;

		static constexpr value_type width = std::numeric_limits<value_type>::digits;
		static constexpr value_type poly = 0x04C11DB7;
		static constexpr value_type xorin = 0x00000000;
		static constexpr bool refin = false;
		static constexpr bool refout = false;
		static constexpr value_type xorout = 0xFFFFFFFF;
		static constexpr value_type check = 0x765E7680;
	};

#pragma endregion

	namespace details
	{
		// Helper class ensures message and polynomial types are unsigned and message types are 8-bit.
		template<class InputIt, class Poly, class T = Poly>
		struct is_crc
		{
			typedef typename std::enable_if<
				(std::is_same<InputIt, uint8_t*>::value || std::is_same<InputIt, unsigned char*>::value) &&
				std::is_unsigned<Poly>::value, T>::type type;
		};

		template<class Poly>
		struct crc_shift
		{
			// Returns the width in bits of type Poly.
			static constexpr Poly width() { return std::numeric_limits<Poly>::digits; }
			// Returns 2**(w-1), where w is the width in bits of type Poly.
			static constexpr Poly top() { return (Poly(1) << (width() - 1)); }
		};

		// Returns the bit-wise reflection of value.
		template<class T>
		T crc_reflect(T value)
		{
			T result = 0;

			for (uint8_t i = 0; i < std::numeric_limits<T>::digits; ++i, value >>= 1)
				result = (result << 1) | (value & 0x01);

			return result;
		}

		// Returns the CRC remainder for byte stream [first,last) using 
		// CRC parameters poly, xorin, xorout, refin and refout.
		template<class InputIt, class Poly>
		typename is_crc<InputIt, Poly>::type
			crc_impl(InputIt first, InputIt last, Poly poly, Poly xorin,
				Poly xorout, bool refin, bool refout)
		{
			// Message length must be >= size of generator polynomial, in bytes.
			assert(std::distance(first, last) >= sizeof(Poly));
			using msg_type = typename std::iterator_traits<InputIt>::value_type;
			using poly_type = Poly;
			using shift_type = crc_shift<Poly>;

			poly_type rem = xorin;	// crc remainder.

			for (; first != last; ++first)
			{
				msg_type in = refin ? crc_reflect(*first) : *first;	// input byte.

				rem ^= (static_cast<poly_type>(in) << (shift_type::width() - CHAR_BIT));
				for (uint8_t j = 0; j < CHAR_BIT; ++j)
					rem = rem & shift_type::top() 
					? (rem << 1) ^ poly
					: (rem << 1);
			}

			return xorout ^ (refout ? crc_reflect(rem) : rem);
		}

		// Returns the CRC remainder for byte stream [first,last) using 
		// table in [lutfirst,lutlast) and CRC parameters poly, xorin, xorout, refin and refout.
		template<class InputIt, class InputIt2>
		typename is_crc<InputIt, typename std::iterator_traits<InputIt2>::value_type>::type
			crc_lut_impl(InputIt first, InputIt last, InputIt2 lutfirst, InputIt2 lutlast,
				typename std::iterator_traits<InputIt2>::value_type poly,
				typename std::iterator_traits<InputIt2>::value_type xorin,
				typename std::iterator_traits<InputIt2>::value_type xorout,
				bool refin, bool refout)
		{
			using msg_type = typename std::iterator_traits<InputIt>::value_type;
			using poly_type = typename std::iterator_traits<InputIt2>::value_type;
			using shift_type = crc_shift<poly_type>;
			// Message length must be >= size of generator polynomial in bytes, and table size must = 256.
			assert(std::distance(first, last) >= sizeof(poly_type) && std::distance(lutfirst, lutlast) == 256);

			poly_type rem = xorin;	// crc remainder.
			uint8_t i;				// lookup table index.

			for (; first != last; ++first)
			{
				msg_type in = refin ? crc_reflect(*first) : *first;	// input byte.

				i = in ^ static_cast<uint8_t>(rem >> (shift_type::width() - CHAR_BIT)); // cast to lut index type.
				rem = lutfirst[i] ^ (rem << CHAR_BIT);
			}

			return xorout ^ (refout ? crc_reflect(rem) : rem);
		}


	} // namespace details

	// Appends r, byte at a time, beginning at first upto one before last, preserving endianness. 
	template<class InputIt, class Poly>
	typename details::is_crc<InputIt, Poly, InputIt>::type
		crc_append(InputIt first, InputIt last, Poly r)
	{
		using value_type = typename std::iterator_traits<InputIt>::value_type;

		for (uint8_t n = sizeof(Poly) - 1; first != last; ++first, --n)
			*first = static_cast<value_type>(r >> (CHAR_BIT * n)) & 0xff; // cast to byte
		return last;
	}

	// Generates a CRC lookup table in range [first,last) using polynomial poly.
	template<class InputIt>
	typename details::is_unsigned<typename std::iterator_traits<InputIt>::value_type>::type
		crc_lut(InputIt first, InputIt last, typename std::iterator_traits<InputIt>::value_type poly)
	{
		using poly_type = typename std::iterator_traits<InputIt>::value_type;
		using shift_type = details::crc_shift<poly_type>;
		std::size_t n = std::distance(first, last);
		std::size_t i = 1;
		poly_type rem;

		first[0] = poly_type();
		do
		{
			// Only need to directly compute elements whose index is a power of 2, 
			// since tbl[i] ^ tbl[j] == tbl[i ^ j].
			rem = (static_cast<poly_type>(i) << (shift_type::width() - CHAR_BIT)); // cast to table element type.
			for (uint8_t j = 0; j < CHAR_BIT; ++j)
				first[i] = (rem = rem & shift_type::top()
					? (rem << 1) ^ poly
					: (rem << 1));
			for (std::size_t j = 0; j < i; ++j)
				first[i + j] = rem ^ first[j];
			i <<= 1;
		} while (i < n);
	}

	// Returns the CRC remainder for byte stream [first,last) using 
	// polynomial poly and CRC parameters xorin, xorout, refin, refout.
	template<class InputIt, class Poly> inline
	Poly crc(InputIt first, InputIt last, Poly poly, Poly xorin = 0,
		Poly xorout = 0, bool refin = false, bool refout = false)
	{
		return details::crc_impl(first, last, poly, xorin, xorout, refin, refout);
	}

	// Returns the CRC remainder for byte stream [first,last) using CRC parameters crc.
	template<class InputIt, class T> inline
	typename crc_traits<T>::value_type crc(InputIt first, InputIt last, crc_traits<T> crc)
	{
		using crc_type = crc_traits<T>;

		return details::crc_impl(first, last, crc_type::poly, crc_type::xorin,
			crc_type::xorout, crc_type::refin, crc_type::refout);
	}

	// Returns the CRC remainder for byte stream [first,last) using table in [lutfirst,lutlast), 
	// polynomial poly, and CRC parameters xorin, xorout, refin and refout.
	template<class InputIt, class InputIt2> inline
		typename std::iterator_traits<InputIt2>::value_type 
		crc(InputIt first, InputIt last, InputIt2 lutfirst, InputIt2 lutlast, 
			typename std::iterator_traits<InputIt2>::value_type poly, 
			typename std::iterator_traits<InputIt2>::value_type xorin = 0,
			typename std::iterator_traits<InputIt2>::value_type xorout = 0, 
			bool refin = false, bool refout = false)
	{
		return details::crc_lut_impl(first, last, lutfirst, lutlast, poly, xorin, xorout, refin, refout);
	}

	// Returns the CRC remainder for byte stream [first,last) using table in array lut, 
	// polynomial poly and CRC parameters xorin, xorout, refin and refout.
	template<class InputIt, class Poly, std::size_t N> inline
		Poly crc(InputIt first, InputIt last, Poly (&lut)[N], Poly poly, 
			Poly xorin = 0, Poly xorout = 0, bool refin = false, bool refout = false)
	{
		return details::crc_lut_impl(first, last, lut, lut + N, poly, xorin, xorout, refin, refout);
	}

	// Returns the CRC remainder for byte stream [first,last) using 
	// table in [lutfirst,lutlast) and CRC parameters crc.
	template<class InputIt, class InputIt2, class T> inline
		typename std::iterator_traits<InputIt2>::value_type
		crc(InputIt first, InputIt last, InputIt2 lutfirst, InputIt2 lutlast, crc_traits<T> crc)
	{
		using crc_type = crc_traits<T>;

		return details::crc_lut_impl(first, last, lutfirst, lutlast,
			crc_type::poly, crc_type::xorin, crc_type::xorout, crc_type::refin, crc_type::refout);
	}

	// Returns the CRC remainder for byte stream [first,last) using 
	// table in array lut and CRC parameters crc.
	template<class InputIt, class Poly, std::size_t N, class T> inline
	Poly crc(InputIt first, InputIt last, Poly (&lut)[N], crc_traits<T> crc)
	{
		using crc_type = crc_traits<T>;

		return details::crc_lut_impl(first, last, lut, lut + N,
			crc_type::poly, crc_type::xorin, crc_type::xorout, crc_type::refin, crc_type::refout);
	}

	// Returns the checksum for stream range [first, last).
	template<class InputIt> 
	typename details::is_unsigned<typename std::iterator_traits<InputIt>::value_type>::type
		checksum(InputIt first, InputIt last)
	{
		typename std::iterator_traits<InputIt>::value_type cs = 0;

		for (; first != last; ++first)
			cs += *first;

		return ~cs;
	}

	// Returns the checksum for stream array arr.
	template<class T, std::size_t N> inline 
	typename details::is_unsigned<T>::type checksum(T(&arr)[N])
	{
		return checksum(arr, arr + N);
	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_CRC_H


