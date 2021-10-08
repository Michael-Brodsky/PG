#if !defined __PG_PWMOUTPUT_H
# define __PG_PWMOUTPUT_H 20211004L

# include "cassert"
# include "limits"
# include "system/boards.h"
# include "lib/fmath.h"

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	template<class FracType, class UIntType>
	struct duty_cycle
	{
		static constexpr FracType frac_min = FracType(0);
		static constexpr FracType frac_max = FracType(1.0);
		static constexpr UIntType frac_to_uint(FracType frac)
		{
			return (UIntType)(clamp(frac, frac_min, frac_max) * FracType(std::numeric_limits<UIntType>::max()));
		}
		static constexpr FracType uint_to_frac(UIntType uint)
		{
			return (FracType)(FracType(uint) / FracType(std::numeric_limits<UIntType>::max()));
		}
	};

	class Pwm
	{
	public:
		using duty_cycle_t = frequency_t; // Fractional duty cycle type.
		using dc_type = uint8_t; 

	public:
		Pwm();
		Pwm(pin_t, duty_cycle_t = 0, bool = false);
		// No copy constructor.
		Pwm(const Pwm&) = delete;
		// No copy assignment operator.
		Pwm& operator=(const Pwm&) = delete;

	public:
		void attach(pin_t);
		pin_t attach() const;
		frequency_t frequency() const;
		void duty_cycle(duty_cycle_t);
		duty_cycle_t duty_cycle() const;
		void enabled(bool);
		bool enabled() const;

	private:
		frequency_t set_frequency(frequency_t);
		void set_output();

	private:
		pin_t pin_;
		frequency_t frequency_;
		duty_cycle_t duty_cycle_;
		bool enabled_;
	};

	Pwm::Pwm() :
		pin_(InvalidPin), duty_cycle_(), frequency_(), enabled_()
	{

	}

	Pwm::Pwm(pin_t pin, duty_cycle_t duty_cycle, bool enabled) :
		pin_(board_traits<board_type>::pwm_frequency(pin) != 0 ? pin : InvalidPin),
		duty_cycle_(duty_cycle), frequency_(set_frequency(0)), enabled_(enabled)
	{
		set_output();
	}


	void Pwm::attach(pin_t pin)
	{
		pin_ = pin;
		frequency_ = set_frequency(0);
		set_output();
	}

	pin_t Pwm::attach() const
	{
		return pin_;
	}

	frequency_t Pwm::frequency() const
	{
		return frequency_;
	}

	void Pwm::duty_cycle(duty_cycle_t value)
	{
		const duty_cycle_t min = pg::duty_cycle<duty_cycle_t, dc_type>::frac_min, 
			max = pg::duty_cycle<duty_cycle_t, dc_type>::frac_max;
		duty_cycle_ = clamp(value, min, max);
		set_output();
	}

	Pwm::duty_cycle_t Pwm::duty_cycle() const
	{
		return duty_cycle_;
	}

	void Pwm::enabled(bool value)
	{
		enabled_ = value;
		set_output();
	}

	bool Pwm::enabled() const
	{
		return enabled_;
	}

	frequency_t Pwm::set_frequency(frequency_t ideal)
	{
		assert(ideal < board_traits<board_type>::clock_frequency);
		return pin_ != InvalidPin ? board_traits<board_type>::pwm_frequency(pin_) : 0;
	}

	void Pwm::set_output()
	{
		//if (pin_ != InvalidPin)
		//	analogWrite(pin_, (enabled_ && frequency_ != 0) ? pg::duty_cycle<duty_cycle_t, dc_type>::frac_to_uint(duty_cycle_) : 0);
		if (pin_ != InvalidPin)
			analogWrite(pin_, (enabled_) ? pg::duty_cycle<duty_cycle_t, dc_type>::frac_to_uint(duty_cycle_) : 0);

	}

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_PWMOUTPUT_H
