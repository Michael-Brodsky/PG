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
 *      The `Keypad' class encapsulates the behaviors of a keypad (buttons) 
 *      attached to an analog input pin. It is a generic, reusable type that 
 *      can be parameterized to fit a particular application. It supports any 
 *      number of buttons and the standard button events, such as `press', 
 *      `release', `longpress' and `repeat', and can issue client callbacks 
 *      when an event occurs.
 * 
 *      Keypad buttons are defined by the nested `Button' type which contains  
 *      a `trigger_level_' field and inherits the `id()' method from its base 
 *      class `Unique'. Clients use id() to uniquely identify Button instances 
 *      and trigger_level_ to set the analog input levels that trigger events. 
 *      The trigger_level_ is compared to the value returned by the Arduino 
 *      analogRead() function from the attached input, and are determined by 
 *      how buttons are physically wired to the input.
 * 
 *      Keypad objects are constructed from a collection of Button objects 
 *      and attached to any valid GPIO analog input. Button collections MUST 
 *      be ordered from low to high with respect to their trigger_level_ (see 
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
 *      Valid events are enumerated by the Keypad::Event type and can be one 
 *      the    following:      Press,       Release     or      Longpress.
 *      The `Press' event occurs whenever the polled input level first becomes 
 *      less than a Button's trigger_level_. No further events are triggered 
 *      until the `Release' event, which occurs when the input level becomes 
 *      greater than or equal to the pressed Button's trigger_level_. This 
 *      behavior can be changed by the `repeat()' method which, if enabled, 
 *      will trigger the press event each time the Keypad is polled until the 
 *      Button is released.
 * 
 *      `Longpress' events can occur in one of three ways, when a button is 
 *      held down, after a button is released, or they can be disabled. The 
 *      mode is set by the value of the Keypad::Longpress type passed to the 
 *      constructor. The event is triggered when a timer, started after the 
 *      Press event, expires. A Release event stops and resets the timer,  
 *      thereby cancelling the Longpress event.
 * 
 *      A Keypad can issue a client callback whenever an event occurs. The 
 *      callback passes a reference to the Button and the triggered Event. 
 *      Callbacks are only enabled if a valid callback method is passed to 
 *      to the Keypad constructor (see `callback_type' below for signature).
 * 
 *	    Keypad objects are not copyable or assignable as this would lead to 
 *	    multiple instances attached to the same analog input, which is 
 *      redundant.
 * 
 *	**************************************************************************/

#if !defined __PG_ANALOGKEYPAD_H 
# define __PG_ANALOGKEYPAD_H 20211005L

# include "array"                   // `ArrayWrapper' type.
# include "system/types.h"          // `pin_t', `analog_t' types.
# include "interfaces/iclockable.h"	// `iclockable' interface.
# include "interfaces/icomponent.h" // `icomponent' interface.
# include "utilities/Timer.h"	    // `Timer' type.
# include "utilities/Unique.h"		// `Unique' base class.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
    // Encapsulates behaviors of a keypad attached to a GPIO analog input.
    class Keypad : public iclockable, public icomponent
    {
    public:

        // Enumerates valid keypad events.
        enum class Event
        {
            Press,      // Key press.
            Longpress,  // Key long-press.
            Release     // Key release.
        };

        // Keypad button type.
        struct Button : public Unique 
        {
            const analog_t  trigger_level_;	// The analog input triggering level. 
                                            // Button collections MUST be instantiated in increasing order of 
                                            // `trigger_level_', from lowest to highest.

            Button(analog_t trigger_level) :
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

        // Longpress event timer resolution type.
        using duration = std::chrono::milliseconds;
        // Client callback signature.
        using callback_type = callback<void, void, const Button*, Event>::type;
        using container_type = std::ArrayWrapper<Button*>; 
        using const_iterator = container_type::const_iterator;  

    public:
        // Constructs a Keypad from an array of Buttons.
        template <std::size_t Size>
        Keypad(Button* (&)[Size], pin_t, callback_type, LongPress = LongPress::None, duration = duration());
        // Constructs a Keypad from a Button pointer and size.
        Keypad(Button* [], std::size_t, pin_t, callback_type, LongPress = LongPress::None, duration = duration());
        // Constructs a Keypad from a range of Buttons.
        Keypad(Button**, Button**, pin_t, callback_type, LongPress = LongPress::None, duration = duration());
        // Constructs a Keypad from a list of Buttons.
        Keypad(std::initializer_list<Button*>, pin_t, callback_type, LongPress = LongPress::None, duration = duration());
        // Constructs a Keypad from a container of Buttons.
        Keypad(const container_type&, pin_t, callback_type, LongPress = LongPress::None, duration = duration());
        // No copy constructor.
        Keypad(const Keypad&) = delete;
        // No copy assignment operator.
        Keypad& operator=(const Keypad&) = delete;

    public:
        // Polls the keypad for button events.
        void            poll();
        // Sets the button repeat state.
        void	        repeat(bool);
        // Returns the current button repeat state.
        bool	        repeat();

    private:
        // Reads the attached pin's input level and returns the currently pressed button, if any.
        Button**        readInput();
        // Button press event handler.
        void            pressEvent(const_iterator);
        // Button release event handler.
        void            releaseEvent(const_iterator);
        // Calls the `poll()' method.
        void            clock() override;
        // Executes the current callback.
        void            doCallback(const_iterator, Event);

    private:
        pin_t           pin_;           // The attached analog input pin.
        callback_type   callback_;      // Client callback.
        container_type  buttons_;       // The current button collection.
        const_iterator  current_;       // Points to the currently triggered button. 
        Timer<duration> lp_timer_;	    // Longpress event timer.
        duration        lp_interval_;   // Longpress event interval.
        LongPress       lp_mode_;       // Longpress event triggering mode.
        bool            repeat_;	    // Flag indicating whether the Press event repeats.
    };

    template <std::size_t Size>
    Keypad::Keypad(Button* (&buttons)[Size], pin_t pin, callback_type callback, LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(buttons), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    Keypad::Keypad(Button* buttons[], std::size_t size, pin_t pin, callback_type callback, LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(buttons, size), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    Keypad::Keypad(Button** first, Button** last, pin_t pin, callback_type callback, LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(first, last), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    Keypad::Keypad(std::initializer_list<Button*> il, pin_t pin, callback_type callback, LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(const_cast<Button**>(il.begin()), il.size()), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    Keypad::Keypad(const container_type& buttons, pin_t pin, callback_type callback, LongPress lp_mode, duration lp_interval) :
        pin_(pin), callback_(callback), buttons_(buttons), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    void Keypad::poll()
    {
        auto button = readInput();

        if (button == std::end(buttons_))
        {
            if (current_ != std::end(buttons_))
                releaseEvent(current_);
        }
        else if (current_ == std::end(buttons_))
            pressEvent(button);
        //else if (lp_mode_ == LongPress::Hold && lp_timer_.expired()) // depends on timer remains expired() when !active() <Timer.h>
        else if (lp_mode_ == LongPress::Hold && lp_timer_.active() && lp_timer_.expired())
        {
            doCallback(button, Event::Longpress);
            lp_timer_.stop();
        }
        else
            (repeat_) ? doCallback(current_, Event::Press) : ((void)0);
        current_ = button;
    }

    Keypad::Button** Keypad::readInput()
    {
        analog_t input_level = analogRead(pin_);
        auto button = std::begin(buttons_);

        for (; button < std::end(buttons_); button++)
        {
            if (input_level < (*button)->trigger_level_)
                break;
        }

        return button;
    }

    void Keypad::repeat(bool value)
    {
        repeat_ = value;
    }

    bool Keypad::repeat()
    {
        return repeat_;
    }

    void Keypad::pressEvent(const_iterator button)
    {
        doCallback(button, Event::Press);
        if (lp_mode_ != LongPress::None)
            lp_timer_.start();
    }

    void Keypad::releaseEvent(const_iterator button)
    {
        //if (lp_mode_ == LongPress::Release && lp_timer_.expired()) // depends on timer remains expired() when !active() <Timer.h>
        if (lp_mode_ == LongPress::Release && lp_timer_.active() && lp_timer_.expired())
            doCallback(button, Event::Longpress);
        else
            doCallback(button, Event::Release);
        lp_timer_.stop();
        repeat_ = false;
    }

    void Keypad::clock()
    {
        poll();
    }

    void Keypad::doCallback(const_iterator button, Event e)
    {
        if (callback_)
            (*callback_)(*button, e);
    }

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif //!defined __PG_ANALOGKEYPAD_H 
