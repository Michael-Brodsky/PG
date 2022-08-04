#if !defined __PG_HARDWARE_H
# define __PG_HARDWARE_H 20220803L

# include <cstdint>
# include <utilities/Timer.h>

namespace pg
{
	using std::chrono::milliseconds;
	using ct_type = CounterTimer<milliseconds, uint32_t>;
	using counter_tag = ct_type::counter_tag;
	using timer_tag = ct_type::timer_tag;

	// Aggregates information about a gpio pin.
	struct GpioPin
	{
		enum Type : uint8_t
		{
			Digital = 0, 							
			Analog = 1, 
			Pwm = 2 
		};

		enum Mode : uint8_t
		{
			Input = 0, 
			Output = 1, 
			Pullup = 2, 
			PwmOut = 3, 
			Reserved = 4 
		};

		Type	type_; 
		Mode	mode_; 
		bool	int_; 

		bool isInput() const { return mode_ == Mode::Input || mode_ == Mode::Pullup; }
		bool isOutput() const { return mode_ == Mode::Output || mode_ == Mode::PwmOut; }
		bool isAnalog() const { return type_ == Type::Analog; }
		bool isDigital() const { return !isAnalog(); }
		bool hasInterrupt() const { return int_; }
		bool isAvailable() const { return mode_ < Mode::Reserved; }
	};

	// Aggregates information about an event counter/timer. 
	struct TimerCounter
	{
		enum Mode : uint8_t
		{
			Counter = 0, 
			Timer = 1 
		};

		enum Action : uint8_t
		{
			Stop = 0, 
			Start = 1, 
			Resume = 2, 
			Reset = 3 
		};

		enum Timing : uint8_t
		{
			Continuous = 0, 
			OneShot = 1, 
		};

		pin_t			pin_; 
		Mode			mode_; 
		PinStatus		trigger_; 
		Timing			timing_; 
		bool			enabled_; 
		bool			instant_; 
		uint32_t		value_; 
		ct_type			object_; 

		bool isAttached() const { return pin_ != InvalidPin; }
	};

	using gpio_type = GpioPin::Type;
	using gpio_mode = GpioPin::Mode;
	using timer_action = TimerCounter::Action;
	using timer_mode = TimerCounter::Mode;
	using timing_mode = TimerCounter::Timing;

} // namespace pg
#endif // !defined __PG_HARDWARE_H
