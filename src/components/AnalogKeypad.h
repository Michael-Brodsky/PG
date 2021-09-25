/*
 *	This files declares a class that encapsulates behaviors of a keypad 
 *  attached to an analog GPIO input.
 *
 *	***************************************************************************
 *
 *	File: AnalogKeypad.h
 *	Date: July 18, 2021
 *	Version: 0.99
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
 *      `release', `longpress' and `repeat'. `Keypad' objects can be operated 
 *      asynchronously using the `clock()' method, or synchronously with the 
 *      `poll()' method. Communication with the client is handled via a 
 *      callback that must be implemented by the client. The button and event 
 *      type (press, release or longpress) are passed to the client in the 
 *      callback. 
 * 
 *      Keypad buttons are encapsulated by the nested `Button' class which  
 *      contains a `ButtonTag' field that uniquely identifies each button. 
 *      Since the buttons and their functions are implementation-specific, 
 *      the `ButtonTag' type is only forward declared and must be defined by 
 *      the client, and the definition must be visible to the `Keypad' class. 
 *      Buttons are passed as a collection to the `Keypad' class constructor. 
 * 
 *      Longpress events can occur in one of three ways, when a button is 
 *      held down, after a button is released, or they can be disabled. The 
 *      longpress modes are enumerated by the `Longpress' type and the mode 
 *      is specified by the client at time of construction, as is the long-
 *      press interval which determines how long it takes to trigger the 
 *      event in milliseconds. 
 * 
 *      The `Keypad' class also provides a mechanism to to repeatedly fire 
 *      the button `press' event. Clients enable/disable the repeat mode 
 *      using the `repeat()' method. The `Keypad' class only provides the 
 *      mechanism, the repeat logic is implemented by the client and can be 
 *      enabled or disabled by the client at any time, regardless of the  
 *      internal state of the `Keypad' class. The repeat event is triggered 
 *      by each call to `poll()' and thus fires at the same rate that the 
 *      keypad is polled.
 * 
 *	Examples:
 *
 *      // Client defines the `ButtonTag's, usually to indicate each button's function. 
 *      enum class ButtonTag { Up, Down, Left, Right };
 * 
 *      // Client defines the callback function that handles button events.
 *      void callback(const Keypad::Button&, Keypad::Event) { ... };
 * 
 *      // Client instantiates a button collection. Each button must have a unique 
 *      // analog triggering level (see `analogRead()') and, the buttons MUST be 
 *      // instantiated in increasing order of this level within the collection, from lowest to highest.
 *      Keypad::Button buttons[] = { Keypad::Button(ButtonTag::Up, 0), Keypad::Button(ButtonTag::Up, 42), ... }; 
 * 
 *      // Client instantiates the `Keypad' object specifying the analog input 
 *      // pin to attach, the callback, the longpress mode and interval and 
 *      // the button collection.
 *      Keypad keypad(0, Keypad::Callaback(&callback), Keypad::Longpress::Hold, 1000U, buttons);
 * 
 *      // Client polls the keypad in a loop.
 *      void loop() { 
 *          keypad.poll();
 *      }
 * 
 *      If a button event occurs, the keypad object calls the client callback
 *      function, passing it the button and the type of event.
 * 
 *  Notes:
 * 
 *      It is absolutely essential that button collections appear in increasing 
 *      order of each button's `trigger_level', from lowest to highest. This is 
 *      required because in each iteration through the button collection, each 
 *      button's `trigger_level' is less-than compared to the value returned by 
 *      the `analogRead()' function. If a button with a higher `trigger_level' 
 *      appears in the collection before buttons with lower values, it will 
 *      prevent those buttons from being triggered. 
 * 
 *	**************************************************************************/

#if !defined __PG_ANALOGKEYPAD_H 
# define __PG_ANALOGKEYPAD_H 20210718L

# include "array"                   // `ArrayWrapper' type.
# include "lib/types.h"             // `pin_t', `analog_t' types.
# include "interfaces/iclockable.h"	// `iclockable' interface.
# include "interfaces/icomponent.h" // `icomponent' interface.
# include "utilities/Timer.h"	    // `Timer' type.

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
    enum class ButtonTag;       // Forward decl, button tags are user-defined.
    // Type that encapsulates behaviors of a keypad attached to an analog GPIO input.
    class Keypad : public iclockable, public icomponent
    {
    public:
        using duration = std::chrono::milliseconds;

        // Enumerates valid keypad events.
        enum class Event
        {
            Press,      // Key press.
            Longpress,  // Key long-press.
            Release     // Key release.
        };

        // Keypad button type.
        struct Button
        {
            const ButtonTag tag_;			// Uniquely identifying tag.
            const analog_t  trigger_level_;	// The analog input triggering level. 
                                            // Button collections MUST be instantiated in increasing order of 
                                            // the `trigger_level_' field, from lowest to highest.

            Button(ButtonTag tag, analog_t trigger_level) :
                tag_(tag), trigger_level_(trigger_level)
            {}
            bool operator==(const Button& other) const
            {
                return trigger_level_ == other.trigger_level_;
            }
            bool operator<(const Button& other) const
            {
                return trigger_level_ < other.trigger_level_;
            }
        };

        // Enumerates the valid longpress event modes.
        enum class LongPress
        {
            Hold,	    // Event is triggered while button is held down.
            Release,    // Event is triggered after button is released.
            None        // Event is never triggered.
        };

        using Callback = void(*)(const Button&, Event);         // Client callback type.
        using container_type = std::ArrayWrapper<const Button>; // Button collection container type.
        using const_iterator = container_type::const_iterator;  // Button container immutable iterator.

    public:
        template <size_t Size>
        Keypad(pin_t, Callback, LongPress, duration, const Button(&)[Size]);
        Keypad(pin_t, Callback, LongPress, duration, const Button[], size_t);
        Keypad(pin_t, Callback, LongPress, duration, const Button*, const Button*);

    public:
        // Polls the keypad for button events.
        void        poll();
        // Sets the button repeat state.
        void	    repeat(bool);

    private:
        // Reads the attached pin's input level and returns the currently pressed button, if any.
        const_iterator  readInput();
        // Button press event handler.
        void            pressEvent(const_iterator);
        // Button release event handler.
        void            releaseEvent(const_iterator);
        // Calls the `poll()' method.
        void            clock() override;
        // Executes the current callback.
        void            callback(const_iterator, Event);

    private:
        pin_t           pin_;           // The attached analog GPIO input pin.
        Callback        callback_;      // Client callback.
        container_type  buttons_;       // The current button collection.
        const_iterator  current_;       // Points to the currently triggered button. 
        Timer<duration> lp_timer_;	    // Keypad longpress event timer.
        duration        lp_interval_;   // Keypad longpress event interval.
        LongPress       lp_mode_;       // Keypad longpress mode.
        bool            repeat_;	    // Flag indicating whether the button press callback is executed on each call to `poll()'.
    };

    template <size_t Size>
    Keypad::Keypad(pin_t pin, Callback callback, LongPress lp_mode, duration lp_interval, const Button(&buttons)[Size]) :
        pin_(pin), callback_(callback), buttons_(buttons), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    Keypad::Keypad(pin_t pin, Callback callback, LongPress lp_mode, duration lp_interval, const Button buttons[], size_t size) :
        pin_(pin), callback_(callback), buttons_(buttons, size), current_(std::end(buttons_)),
        lp_timer_(lp_interval), lp_interval_(lp_interval), lp_mode_(lp_mode), repeat_()
    {

    }

    Keypad::Keypad(pin_t pin, Callback callback, LongPress lp_mode, duration lp_interval, const Button* first, const Button* last) :
        pin_(pin), callback_(callback), buttons_(first, last), current_(last),
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
        else if (lp_mode_ == LongPress::Hold && lp_timer_.expired())
        {
            callback(button, Event::Longpress);
            lp_timer_.stop();
        }
        else
            (repeat_) ? callback(current_, Event::Press) : ((void)0);
        current_ = button;
    }

    Keypad::const_iterator Keypad::readInput()
    {
        analog_t input_level = analogRead(pin_);
        auto button = std::begin(buttons_);

        for (; button < std::end(buttons_); button++)
        {
            if (input_level < (*button).trigger_level_)
                break;
        }

        return button;
    }

    void Keypad::repeat(bool value)
    {
        repeat_ = value;
    }

    void Keypad::pressEvent(const_iterator button)
    {
        callback(button, Event::Press);
        if (lp_mode_ != LongPress::None)
            lp_timer_.start();
    }

    void Keypad::releaseEvent(const_iterator button)
    {
        if (lp_mode_ == LongPress::Release && lp_timer_.expired())
            callback(button, Event::Longpress);
        else
            callback(button, Event::Release);
        lp_timer_.stop();
        repeat_ = false;
    }

    void Keypad::clock()
    {
        poll();
    }

    void Keypad::callback(const_iterator button, Event e)
    {
        if (callback_)
            (*callback_)(*button, e);
    }
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif //!defined __PG_ANALOGKEYPAD_H 
