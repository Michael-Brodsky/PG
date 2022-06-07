/*
 *	This file defines a class for increasing the resolution of an analog to 
 *	digital converter (ADC) by oversampling and decimation.
 *
 *	***************************************************************************
 *
 *	File: adc.h
 *	Date: June 5, 2022
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky
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
 *	along with this file. If fnot, see <http://www.gnu.org/licenses/>.
 *
 *	**************************************************************************/

#if !defined __PG_ADC_H
# define __PG_ADC_H 20220605L

# include <system/boards.h>
# include <lib/fmath.h>

namespace pg
{
	// Type that increases ADC resolution by oversampling.
	class AdcOversampler
	{
	public:
		static constexpr uint8_t AdcResolutionMin = 10;
		static constexpr uint8_t AdcResolutionMax = 16;
		static constexpr uint8_t AdcPrescalerDefault = 4;

	public:
		AdcOversampler(uint8_t = AdcResolutionMin);

	public:
		analog_t max() const;
		void prescaler(uint8_t);
		uint8_t prescaler() const;
		analog_t read(pin_t);
		void resolution(uint8_t);
		uint8_t resolution() const;
		uint16_t samples() const;

	private:
		inline uint8_t setOvsBits(uint8_t);
		inline uint16_t setOvsSamples(uint8_t);
		uint8_t setPrescaler(uint8_t);
		inline uint8_t setResolution(uint8_t);

	private:
		uint8_t resolution_;
		uint8_t prescaler_;
		uint8_t ovsbits_;
		uint16_t ovsamples_;
	};

	AdcOversampler::AdcOversampler(uint8_t res) : 
		resolution_(setResolution(res)), prescaler_(setPrescaler(AdcPrescalerDefault)),
		ovsbits_(setOvsBits(resolution_)), ovsamples_(setOvsSamples(ovsbits_))
	{

	}

	analog_t AdcOversampler::max() const
	{
		return (ipow2(board_traits<board_type>::adc_digits) - 1) * ipow2(ovsbits_);
	}

	void AdcOversampler::prescaler(uint8_t prescale)
	{
		(void)setPrescaler(prescale);
	}

	uint8_t AdcOversampler::prescaler() const
	{
		return prescaler_;
	}

	analog_t AdcOversampler::read(pin_t pin)
	{
		unsigned long sum = 0;
		
		for (uint16_t i = 0; i < ovsamples_; ++i)
			sum += (analog_t)analogRead(pin);

		return sum >> ovsbits_;
	}

	void AdcOversampler::resolution(uint8_t res)
	{
		resolution_ = setResolution(res);
		ovsbits_ = setOvsBits(resolution_);
		ovsamples_ = ipow2(ovsbits_ * 2);
	}

	uint8_t AdcOversampler::resolution() const
	{
		return resolution_;
	}

	uint16_t AdcOversampler::samples() const 
	{ 
		return ovsamples_; 
	}

	uint8_t AdcOversampler::setOvsBits(uint8_t res)
	{
		return res - AdcResolutionMin;
	}

	uint16_t AdcOversampler::setOvsSamples(uint8_t nbits)
	{
		return ipow2(nbits * 2);
	}

	uint8_t AdcOversampler::setPrescaler(uint8_t prescale)
	{
		uint8_t mask = B11111000;
		ADCSRA &= mask;
		ADCSRA |= prescale;

		return (prescaler_ = prescale);
	}

	uint8_t AdcOversampler::setResolution(uint8_t res)
	{ 
		return clamp(res, AdcResolutionMin, AdcResolutionMax); 
	}
} // namespace pg

# endif // !defined __PG_ADC_H
