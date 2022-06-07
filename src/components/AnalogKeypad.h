/*
 *	This files defines a class that encapsulates behaviors of a keypad 
 *  attached to an analog GPIO input.
 *
 *	***************************************************************************
 *
 *	File: AnalogKeypad.h
 *	Date: October 5, 2021
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
 *	along with this file. If not, see <http://www.gnu.org/licenses/>.
 *
 *	**************************************************************************
 *
 *	Description:
 *
 *      The `AnalogKeypad' class encapsulates the behaviors of a keypad 
 *      attached to an analog input pin. It is a generic, reusable type that 
 *      can be parameterized to fit a particular application. It supports any 
 *      number of buttons and the standard button events, such as `press', 
 *      `release', `longpress' and `repeat', and can issue client callbacks 
 *      when an event occurs. AnalogKeypad compares the value returned by the 
 *	    Arduino analogRead() function from the attached input pin. The template 
 *      parameter `T' determines the data type, which should the same as that 
 *      returned by analogRead().
 * 
 *      AnalogKeypad buttons are defined by the nested `Button' type which has   
 *      a `trigger_level_' field and inherits the `id()' method from its base 
 *      class `Unique'. Clients use id() to uniquely identify Button instances 
 *      and trigger_level_ to set the analog input levels that trigger events. 
 *      The trigger_level_ is compared to the value returned by analogRead() 
 *      from the attached input, and are determined by how buttons are 
 *      physically wired to the input.
 * 
 *      AnalogKeypads are constructed from a collection of Button objects and 
 *      attached to any valid GPIO analog input. Button collections MUST be  
 *      ordered from low to high with respect to their trigger_level_ (see 
 *      below). Clients must also specify the callback method, and optionally
 *      the longpress mode and interval, in the constructor. 
 * 
 *      Keypads are polled using the `poll()' method, or asynchronously with 
 *      the `clock()' method. Each call to poll() or clock() reads the current 
 *      value from the attached input and checks it against each Button's 
 *      trigger_level_ to see if an event has occured. The input level is 
 *      less-than compared against each Button's trigger_level_ in the 
 *      collection, from first to last. If a Button with a higher 
 *      trigger_level_ appears in the collection before buttons with lower 
 *      levels it will prevent those buttons from ever being triggered. This is 
 *      why Button collections must be ordered from lowest to highest levels.
 * 
 *      Valid events are enumerated by the AnalogKeypad::Event type and can be 
 *      one of the    following:      Press,      Release     or     Longpress.
 *      The `Press' event occurs whenever the polled input level first becomes 
 *      less than a Button's trigger_level_. No further events are triggered 
 *      until the `Release' event, which occurs when the input level becomes 
 *      greater than or equal to the pressed Button's trigger_level_. This 
 *      behavior can be changed by the `repeat()' method which, if enabled, 
 *      will trigger the press event each time the AnalogKeypad is polled until
 *      the Button is released.
 * 
 *      `Longpress' events can occur in one of three ways, when a button is 
 *      held down, after a button is released, or they can be disabled. The 
 *      mode is set by the value of the AnalogKeypad::Longpress type passed to 
 *      the constructor. The event is triggered when a timer, started after 
 *      the Press event, expires. A Release event stops and resets the timer,  
 *      thereby cancelling the Longpress event.
 * 
 *      An AnalogKeypad can issue a client callback whenever an event occurs. 
 *      The callback passes a reference to the Button and the triggered Event. 
 *      Callbacks are only enabled if a valid callback method is passed to 
 *      to the AnalogKeypad constructor (see `callback_type' below for signature).
 * 
 *	    AnalogKeypad objects are not copyable or assignable as this would lead 
 *	    to multiple instances attached to the same analog input, which is 
 *      redundant.
 * 
 *	**************************************************************************/

#if !defined __PG_ANALOGKEYPAD_H 
# define __PG_ANALOGKEYPAD_H 20211005L

# include <array>                   // `ArrayWrapper' type.
# include <system/types.h>          // `pin_t', `analog_t' types.
# include <interfaces/iclockable.h>	// `iclockable' interface.
# include <interfaces/icomponent.h> // `icomponent' interface.
# include <utilities/Timer.h>	    // `Timer' type.
# include <utilities/Unique.h>		// `Unique' base class.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
    // Encapsulates behaviors of a keypad attached to a GPIO analog input.
    template<class T = analog_t, class TimerType = Timer<std::chrono::milliseconds>>
    class AnalogKeypad : public iclockable, public icomponent
    {
    public:
        using value_type = T;
        using timer_type = TimerType;
        using duration = typename timer_type::duration;

        // Enumerates valid keypad events.
        enum class Event
        {
            Press,      // Key press.
            Longpress,  // Key long-press.
            Release     // Key release.
        };

        // AnalogKeypad button type.
        struct Button : public Unique 
        {
            const value_type  trigger_level_;	// The analog input triggering level. 
                                                // Button collections MUST be instantiated in increasing order of 
                                                // `trigger_level_', from lowest to highest.

            Button(value_type trigger_level) :
                trigger_level_(trigger_level)
            {}

            friend bool operator==(const Button& lhs, const Button& rhs)
            {
                return lhs.trigger_level_ == rhs.trigger_level_;
            }
            friend bool operator<(const Button& lhs, const Button& rhs)
            {
                return lhs.trigger_level_ < rhs.trigger_level_;
            }
        };

        // Enumerates the valid Longpress event triggering modes.
        enum class LongPress
        {
            Hold = 0,   // Event is triggered while button is held down.
            Release,    // Event is triggered after button is released.
            None        // Event is never triggered.
        };

        // Multiplies a value for faster scrolling when a keypad is used to adjust 
        // application settings over wide ranges. Has the effect of increasing the 
        // scroll rate when, for instance, a button is held down over a period of  
        // time. Allows clients to scroll slowly at first, for fine adjustments, 
        // and increase the scroll rate on each clock() for increasingly coarser 
        // adjustments that cover a wider range. Used in conjunction with the 
        // Keypad repeat setting.
        class Multiplier : public iclockable
        {
        public:
            enum class Direction { Up = 0, Down };
            using factor_type = unsigned;

        public:
            Multiplier(factor_type mulmax, uint8_t mulfact = 11) :
                mul_(1), mulmax_(mulmax), mulfact_(mulfact) {}

        public:
            // Resets the current multiplication factor to 1.
            void reset() { mul_ = 1; }
            // Returns u multiplied by the current multiplication factor and signed according to dir.
            template<class U>
            typename std::make_signed<U>::type value(U u, Direction dir)
            {
                typename std::make_signed<U>::type val = u * mul_;

                return dir == Direction::Up ? val : -val;
            }

        private:
            // Bumps the current multiplication factor by a factor of mulfact_ upto mulmax_.
            void clock() override { mul_ *= mul_ < mulmax_ ? mulfact_ : 1; }

        private:
            factor_type	mul_;		// The current multiplication factor.
            factor_type	mulmax_;	// The maximum multiplication factor.
            uint8_t     mulfact_;   // Amount to multiply factor by on each clock cycle. 
        };

        using callback_type = typename callback<void, void, const Button*, Event>::type;
        using container_type = typename std::ArrayWrapper<Button*>; 
        using iterator = typename container_type::iterator;  

    public:
        // Constructs an uninitialized AnalogKeypad.
        AnalogKeypad();
        // Constructs a AnalogKeypad without Buttons.
        explicit AnalogKeypad(pin_t, callback_type = nullptr);
        // Constructs a AnalogKeypad from a range of Buttons.
        AnalogKeypad(pin_t, callback_type, Button** = nullptr, Button** = nullptr, LongPress = LongPress::None, duration = duration());
        // Constructs a AnalogKeypad from an array of Buttons.
        template <std::size_t Size>
        AnalogKeypad(pin_t, callback_type, Button* (&)[Size], LongPress = LongPress::None, duration = duration());
        // Constructs a AnalogKeypad from a Button pointer and size.
        AnalogKeypad(pin_t, callback_type, Button* [], std::size_t, LongPress = LongPress::None, duration = duration());
        // Constructs a AnalogKeypad from a list of Buttons.
        AnalogKeypad(pin_t, callback_type, std::initializer_list<Button*>, LongPress = LongPress::None, duration = duration());
        // Constructs a AnalogKeypad from a container of Buttons.
        AnalogKeypad(pin_t, callback_type, const container_type&, LongPress = LongPress::None, duration = duration());
        // Move constructor.
        AnalogKeypad(AnalogKeypad&&) = default;
        // No copy constructor.
        AnalogKeypad(const AnalogKeypad&) = delete;
        // No copy assignment operator.
        AnalogKeypad& operator=(const AnalogKeypad&) = delete;

    public:
        // Attaches a keypad to an analog input pin.
        void attach(pin_t);
        // Returns the currently attached input pin.
        const pin_t attach() const;
        // Sets the Buttons collection from an array.
        template<std::size_t N>
        void buttons(Button* (&)[N]);
        // Sets the Buttons collection from a pointer and size.
        void buttons(Button* [], std::size_t);
        // Sets the Buttons collection from a range.
        void buttons(Button**, Button**);
        // Sets the Buttons collection from a list.
        void buttons(std::initializer_list<Button*>);
        // Sets the Buttons collection from a container.
        void buttons(container_type&);
        // Returns a cointainer of the current Buttons collection.
        const container_type& buttons() const;
        // Sets the longpress mode.
        void lp_mode(LongPress);
        // Sets the longpress interval.
        void lp_interval(duration);
        // Sets the client callback.
        void callback(callback_type);
        // Polls the keypad for button events.
        void poll();
        // Sets the button repeat state.
        void repeat(bool);
        // Returns the current button repeat state.
        bool repeat();
        // Polls the keypad and returns the currently pressed button, if any.
        Button* operator()();
        // Returns the last pressed Button, if any.
        Button* value() const;

    private:
        // Reads the attached pin's input level and returns the currently pressed button, if any.
        iterator readInput();
        // Button press event handler.
        void pressEvent(iterator);
        // Button release event handler.
        void releaseEvent(iterator);
        // Calls the `poll()' method.
        void clock() override;
        // Executes the current callback.
        void doCallback(iterator, Event);

    private:
        pin_t           pin_;           // The attached analog input pin.
        callback_type   callback_;      // Client callback.
        container_type  buttons_;       // The current button collection.
        iterator        current_;       // Points to the currently triggered button. 
        timer_type      lp_timer_;	    // Longpress event timer.
        duration        lp_interval_;   // Longpress event interval.
        LongPress       lp_mode_;       // Longpress event triggering mode.
        bool            repeat_;	    // Flag indicating whether the Press event repeats.
    };

    template<class T, class TimerType>
    AnalogKeypad<T, TimerType>::AnalogKeypad() :
        pin_(InvalidPin), callback_(), buttons_(), current_(), lp_timer_(),
        lp_interval_(), lp_mode_(), repeat_()
    {

    }

    template<class T, class TimerType>
    AnalogKeypad<T, TimerType>::AnalogKeypad(pin_t pin, callback_type callback) : 
        pin_(pin), callback_(callback), buttons_(), current_(), lp_timer_(),
        lp_interval_(), lp_mode_(), repeat_()
    {

    }

    template<class T, class TimerType>
    template <std::size_t Size>
    AnalogKeypad<T, TimerType>::AnalogKeypad(pin_t pin, callback_type callback, Button* (&buttons)[Size],  
        LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(buttons), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    template<class T, class TimerType>
    AnalogKeypad<T, TimerType>::AnalogKeypad(pin_t pin, callback_type callback, Button* buttons[], std::size_t size, 
        LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(buttons, size), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    template<class T, class TimerType>
    AnalogKeypad<T, TimerType>::AnalogKeypad(pin_t pin, callback_type callback, Button** first, Button** last, 
        LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(first, last), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    template<class T, class TimerType>
    AnalogKeypad<T, TimerType>::AnalogKeypad(pin_t pin, callback_type callback, std::initializer_list<Button*> il,  
        LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(const_cast<Button**>(il.begin()), il.size()), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    template<class T, class TimerType>
    AnalogKeypad<T, TimerType>::AnalogKeypad(pin_t pin, callback_type callback, const container_type& buttons, 
        LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(buttons), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::attach(pin_t pin)
    {
        pin_ = pin;
    }

    template<class T, class TimerType>
    const pin_t AnalogKeypad<T, TimerType>::attach() const
    {
        return pin_;
    }

    template<class T, class TimerType>
    template<std::size_t N>
    void AnalogKeypad<T, TimerType>::buttons(Button* (&buttons)[N])
    {
        buttons_ = container_type(buttons);
        current_ = std::end(buttons_);
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::buttons(Button* buttons[], std::size_t n)
    {
        buttons_ = container_type(buttons, n);
        current_ = std::end(buttons_);
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::buttons(Button** first, Button** last)
    {
        buttons_ = container_type(first, last);
        current_ = std::end(buttons_);
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::buttons(std::initializer_list<Button*> il)
    {
        buttons_ = container_type(const_cast<Button**>(il.begin()), il.size());
        current_ = std::end(buttons_);
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::buttons(container_type& container)
    {
        buttons_ = container;
        current_ = std::end(buttons_);
    }

    template<class T, class TimerType>
    const typename AnalogKeypad<T, TimerType>::container_type& 
        AnalogKeypad<T, TimerType>::buttons() const
    {
        return buttons_;
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::lp_mode(LongPress mode)
    {
        lp_mode_ = mode;
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::lp_interval(duration interval)
    {
        lp_timer_.interval(interval);
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::callback(callback_type callback)
    {
        callback_(callback);
    }

    template<class T, class TimerType>
    typename AnalogKeypad<T, TimerType>::Button* AnalogKeypad<T, TimerType>::operator()() 
    {
        return (current_ = readInput()) == std::end(buttons_) ? nullptr : *current_;
    }

    template<class T, class TimerType>
    typename AnalogKeypad<T, TimerType>::Button* AnalogKeypad<T, TimerType>::value() const
    {
        return current_ == std::end(buttons_) ? nullptr : *current_;
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::poll()
    {
        auto button = readInput();

        if (button == std::end(buttons_))
        {
            if (current_ != std::end(buttons_))
                releaseEvent(current_);
        }
        else if (current_ == std::end(buttons_))
            pressEvent(button);
        else if (lp_mode_ == LongPress::Hold && lp_timer_.active() && lp_timer_.expired())
        {
            doCallback(button, Event::Longpress);
            lp_timer_.stop();
        }
        else
            (repeat_) ? doCallback(current_, Event::Press) : ((void)0);
        current_ = button;
    }

    template<class T, class TimerType>
    typename AnalogKeypad<T, TimerType>::iterator AnalogKeypad<T, TimerType>::readInput()
    {
        value_type input_level = analogRead(pin_);
        auto button = std::begin(buttons_);

        for (; button < std::end(buttons_); button++)
        {
            if (input_level < (*button)->trigger_level_)
                break;
        }

        return button;
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::repeat(bool value)
    {
        repeat_ = value;
    }

    template<class T, class TimerType>
    bool AnalogKeypad<T, TimerType>::repeat()
    {
        return repeat_;
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::pressEvent(iterator button)
    {
        doCallback(button, Event::Press);
        if (lp_mode_ != LongPress::None)
            lp_timer_.start();
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::releaseEvent(iterator button)
    {
        if (lp_mode_ == LongPress::Release && lp_timer_.active() && lp_timer_.expired())
            doCallback(button, Event::Longpress);
        else
            doCallback(button, Event::Release);
        lp_timer_.stop();
        repeat_ = false;
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::clock()
    {
        poll();
    }

    template<class T, class TimerType>
    void AnalogKeypad<T, TimerType>::doCallback(iterator button, Event e)
    {
        if (callback_)
            (*callback_)(*button, e);
    }

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif //!defined __PG_ANALOGKEYPAD_H 
