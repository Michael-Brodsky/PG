/*
 *	This files declares a class that asynchronously executes a sequence of 
 *	Command objects.
 *
 *	***************************************************************************
 *
 *	File: EventSequencer.h
 *	Date: May 5, 2021
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
 *		The `EventSequencer' class encapsulates the behavior of a chronological 
 *		event sequencer. It executes a collection of Command objects (see 
 *		<icommand.h>) in order, at specified intervals. Events are encapsulated 
 *		in the nested `Event' type and have three properties: a human readable 
 *		name, a duration and the Command object that executes the event. 
 *		Clients pass a collection of Events to the EventSequencer at 
 *		construction and use its member methods to control execution. An
 *		EventSequencer can be operated synchronously with the `tick()' 
 *		method, or asynchronously with the `clock()' method (see 
 *		<iclockable.h>). Each call to `tick()' or `clock()' executes the 
 *		current event, subsequent calls check the event's time elapsed and 
 *		advance to the next event in the sequence when the current event 
 *		has expired. The `wrap()' method sets whether a sequence will 
 *		wrap-around and repeat or stop after the last event.
 * 
 *		Clients can start, stop, resume and reset the current sequence with 
 *		the respective methods and obtain the current event's information, the 
 *		time elapsed and its index (position) in the sequence. An 
 *		EventSequencer can update clients via callbacks at the beginning and 
 *		end of an event.
 * 
 *	**************************************************************************/

#if !defined __PG_EVENTSEQUENCER_H
# define __PG_EVENTSEQUENCER_H 202100505L

# include "array"					// `ArrayWrapper' type, `std::distance()'
# include "interfaces/icomponent.h"	// `icomponent' interface.
# include "interfaces/iclockable.h"	// `iclockable" and `icommand' interfaces.
# include "utilities/Timer.h"		// `Timer' class.

namespace pg
{
	// Asynchronous event sequencer.
	template<class T = Timer<std::chrono::milliseconds>>
	class EventSequencer : public iclockable, public icomponent
	{
	public:
		using timer_type = T;
		using duration = typename timer_type::duration;
		
		// Encapsulates information about an event.
		struct Event
		{
			// Enumerates the valid event states.
			enum class State
			{
				Begin = 0,	// State at the start of an event.
				End			// State at the completion of an event.
			};

			const char* name_;		// Human-readable name.
			duration	duration_;	// Duration in milliseconds.
			icommand*	command_;	// Event command object.
		};

		// Enumerates the valid sequencer states.
		enum class Status
		{
			Idle = 0,	// EventSequencer is not active.
			Active,		// EventSequencer is active.
			Done		// Current sequence is completed.
		};

		using sequencer_type = EventSequencer<T>;
		using event_type = typename sequencer_type::Event;
		using state_type = typename sequencer_type::Event::State;
		using callback_type = typename callback<void, void, const Event*, state_type>::type;
		using container_type = std::ArrayWrapper<Event*>;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

	public:
		//Constructs an uninitialized sequencer.
		EventSequencer() = default;
		// Constructs the sequencer from an array of events.
		template <std::size_t N>
		explicit EventSequencer(Event* (&)[N], callback_type, bool = false);
		// Constructs the sequencer from a pointer to and size of events..
		EventSequencer(Event* [], std::size_t, callback_type, bool = false);
		// Constructs the sequencer from a range of events.
		EventSequencer(Event**, Event**, callback_type, bool = false);
		// Constructs the sequencer from a list of events.
		EventSequencer(std::initializer_list<Event*>, callback_type, bool = false);
		// Constructs the sequencer from a container of events.
		EventSequencer(const container_type&, callback_type, bool = false);

	public:
		// Sets the events collection from an array.
		template <std::size_t N>
		void			events(Event* (&)[N]);
		// Sets the events collection from a pointer and size.
		void			events(Event* [], std::size_t);
		// Sets the events collection from a range.
		void			events(Event**, Event**);
		// Sets the events collection from a list.
		void			events(std::initializer_list<Event*>);
		// Sets the events collection from a container.
		void			events(container_type&); 
		// Returns the current events collection.
		const container_type& events() const;
		// Sets the client callback.
		void			callback(callback_type);
		// Starts the current sequence.
		void			start();
		// Stops the current sequence.
		void			stop();
		// Resets the current sequence to the beginning.
		void			reset();
		// Resumes the sequence at the current event.
		void			resume();
		// Advances to the next sequence event.
		void			next();
		// Advances to the previous sequence event.
		void			prev();
		// Sets the sequence wrap-around mode.
		void			wrap(bool);
		// Returns the current sequence wrap-around mode.
		bool			wrap() const;
		// Returns the EventSequencer's current status.
		Status			status() const;
		// Returns a reference to the current event.
		const Event*	event() const;
		// Sets the current event.
		void			event(Event*);
		// Returns the current event's time elapsed in milliseconds.
		duration		elapsed() const;
		// Returns the index of the current event within the sequence.
		uint8_t			index() const;
		// Steps through and executes the current sequence chronologically.
		void			tick();

	private:
		// Begins the current event.
		void			begin();
		// Advances to the next event in the current sequence.
		void			advance();
		// Ends the current event.
		void			end();
		// Rewinds the sequence to the first event.
		void			rewind();
		// Calls the `tick()' method.
		void			clock() override;
		// Executes the current callback.
		void			callback(const_iterator, state_type);

	private:
		container_type	events_;		// The current events collection.
		const_iterator	current_;		// Points to the current event in the collection.
		callback_type	callback_;		// Client callback.
		bool			wrap_;			// Flag indicating whether the sequence wraps-around continuously.
		bool			done_;			// Flag indicating whether the current sequence is completed.
		bool			exec_;			// Flag indicating whether to execute the current event on resume.
		timer_type		event_timer_;	// Sequence event timer.
	};

	template<class T>
	template <std::size_t N>
	EventSequencer<T>::EventSequencer(Event* (&events)[N], callback_type callback, bool wrap) :
		events_(events), current_(events_.begin()), callback_(callback),
		wrap_(wrap), done_(), exec_(), event_timer_()
	{

	}

	template<class T>
	EventSequencer<T>::EventSequencer(Event* events[], std::size_t size, callback_type callback, bool wrap) :
		events_(events, size), current_(events_.begin()), callback_(callback),
		wrap_(wrap), done_(), exec_(), event_timer_()
	{

	}

	template<class T>
	EventSequencer<T>::EventSequencer(Event** first, Event** last, callback_type callback, bool wrap) :
		events_(first, last), current_(events_.begin()), callback_(callback),
		wrap_(wrap), done_(), exec_(), event_timer_()
	{

	}

	template<class T>
	EventSequencer<T>::EventSequencer(std::initializer_list<Event*> il, callback_type callback, bool wrap) :
		events_(const_cast<Event**>(il.begin()), il.size()), current_(events_.begin()), callback_(callback),
		wrap_(wrap), done_(), exec_(), event_timer_()
	{

	}

	template<class T>
	EventSequencer<T>::EventSequencer(const container_type& events, callback_type callback, bool wrap) :
		events_(events), current_(events_.begin()), callback_(callback),
		wrap_(wrap), done_(), exec_(), event_timer_()
	{

	}

	template<class T>
	template <std::size_t N>
	void EventSequencer<T>::events(Event* (&events)[N])
	{
		events_ = events;
		current_ = std::begin(events_);
	}

	template<class T>
	void EventSequencer<T>::events(Event* events[], std::size_t n)
	{
		events_ = container_type(events, n);
		current_ = std::begin(events_);
	}

	template<class T>
	void EventSequencer<T>::events(Event** first, Event** last)
	{
		events_ = container_type(first, last);
		current_ = std::begin(events_);
	}

	template<class T>
	void EventSequencer<T>::events(std::initializer_list<Event*> il)
	{
		events_ = (const_cast<Event**>(il.begin()), il.size());
		current_ = std::begin(events_);
	}

	template<class T>
	void EventSequencer<T>::events(container_type& events)
	{
		events_ = events;
		current_ = std::begin(events_);
	}

	template<class T>
	const typename EventSequencer<T>::container_type& EventSequencer<T>::events() const
	{
		return events_;
	}

	template<class T>
	void EventSequencer<T>::callback(callback_type cb)
	{
		callback_ = cb;
	}

	template<class T>
	void EventSequencer<T>::start()
	{
		if (status() != Status::Active)
		{
			rewind();
			begin();
			event_timer_.start();
		}
	}

	template<class T>
	void EventSequencer<T>::stop()
	{
		event_timer_.stop();
	}

	template<class T>
	void EventSequencer<T>::reset()
	{
		rewind();
		if (status() == Status::Active)
		{
			event_timer_.reset();
			begin();
		}
		else
			event_timer_.interval(duration(0)); // "Flag" to indicate sequencer is reset.

	}

	template<class T>
	void EventSequencer<T>::resume()
	{
		if (status() == Status::Idle)
		{
			if (event_timer_.interval().count() == 0) // Check if sequencer is reset.
				start();
			else
			{
				if (exec_)
				{
					begin();
					exec_ = false;
				}
				event_timer_.resume();
			}
		}
	}

	template<class T>
	void EventSequencer<T>::next()
	{
		if (++current_ == std::end(events_))
			current_ = std::begin(events_);
		exec_ = true;
		event_timer_.interval((*current_)->duration_);
		event_timer_.reset();
	}

	template<class T>
	void EventSequencer<T>::prev()
	{
		if (current_ == std::begin(events_))
			current_ = std::end(events_);
		--current_;
		exec_ = true;
		event_timer_.interval((*current_)->duration_);
		event_timer_.reset();
	}

	template<class T>
	void EventSequencer<T>::wrap(bool value)
	{
		wrap_ = value;
	}

	template<class T>
	bool EventSequencer<T>::wrap() const
	{
		return wrap_;
	}

	template<class T>
	typename EventSequencer<T>::Status EventSequencer<T>::status() const
	{
		return event_timer_.active()
			? Status::Active
			: (done_)
			? Status::Done
			: Status::Idle;
	}

	template<class T>
	const typename EventSequencer<T>::Event* EventSequencer<T>::event() const
	{
		return *current_;
	}

	template<class T>
	void EventSequencer<T>::event(Event* event)
	{
		*current_ = event;
	}

	template<class T>
	typename EventSequencer<T>::duration EventSequencer<T>::elapsed() const
	{
		return event_timer_.elapsed();
	}

	template<class T>
	uint8_t	EventSequencer<T>::index() const
	{
		return static_cast<uint8_t>(std::distance(std::begin(events_), current_) + 1);
	}

	template<class T>
	void EventSequencer<T>::tick()
	{
		if (event_timer_.expired())
		{
			end();
			advance();
			if (status() == Status::Active)
				begin();
		}
	}

	template<class T>
	void EventSequencer<T>::begin()
	{
		Event* it = *current_;

		event_timer_.interval(it->duration_);
		if (it->command_)
			it->command_->execute();
		callback(current_, EventSequencer<T>::Event::State::Begin);
	}

	template<class T>
	void EventSequencer<T>::advance()
	{
		if (++current_ == std::end(events_))
		{
			if (wrap_)
				current_ = std::begin(events_);
			else
			{
				stop();
				--current_;
				done_ = true;
			}
		}
	}

	template<class T>
	void EventSequencer<T>::end()
	{
		callback(current_, Event::State::End);
	}

	template<class T>
	void EventSequencer<T>::rewind()
	{
		current_ = std::begin(events_);
		done_ = false;
	}

	template<class T>
	void EventSequencer<T>::clock()
	{
		tick();
	}

	template<class T>
	void EventSequencer<T>::callback(const_iterator event, state_type state)
	{
		if (callback_)
			(*callback_)(*event, state);
	}

} // namespace pg

#endif // !defined __PG_EVENTSEQUENCER_H