/*
 *	This files declares a class that asynchronously executes a sequence of 
 *	Command objects.
 *
 *	***************************************************************************
 *
 *	File: Sequencer.h
 *	Date: May 5, 2021
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
 *		The `Sequencer' class encapsulates the behavior of a chronological 
 *		event sequencer. It executes a collection of Command objects (see 
 *		<ICommand.h>) in order at specified intervals. Events are encapsulated 
 *		in the nested `Event' type and have three properties: a human readable 
 *		name, a duration and the Command object that executes the event. 
 *		Clients pass a collection of `Event' objects to the `Sequencer' at 
 *		time of construction and use its member methods to control execution.
 *		`Sequencer' objects can be operated synchronously with the `tick()' 
 *		method, or asynchronously with the `clock()' method (see 
 *		<iclockable.h>). Each call to `tick()' or `clock()' executes the 
 *		current event, subsequent calls check the event's time elapsed and 
 *		advance to the next event in the sequence when the current event 
 *		has expired. The `wrap()' method sets whether a sequence will 
 *		wrap-around and repeat or stop after the last event.
 * 
 *		Clients can start, stop, resume and reset the current sequence with 
 *		the respective methods and obtain the current event's information, the 
 *		time elapsed and its index (position) in the sequence. The `Sequencer' 
 *		can update clients via callbacks at the beginning and end of an event.
 *
 *	**************************************************************************/

#if !defined SEQUENCER_H__
# define SEQUENCER_H__ 202100505L

# include "array"						// `ArrayWrapper' type, `std::distance()'
# include "../interfaces/icomponent.h"	// `icomponent' interface.
# include "../interfaces/iclockable.h"	// `iclockable" and `icommand' interfaces.
# include "../utilities/Timer.h"		// `Timer' class.

namespace pg
{
	// Type that asynchronously executes a sequence of commands objects.
	class Sequencer : public iclockable, public icomponent
	{
	public:
		using Timer = pg::Timer<std::chrono::milliseconds>;
		using duration = typename Timer::duration;
		
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
			Idle = 0,	// Sequencer is not active.
			Active,		// Sequencer is active.
			Done		// Current sequence is completed.
		};

		using Callback = void(*)(const Event&, Event::State);	// Client callback type.
		using container_type = std::ArrayWrapper<Event*>;		// Sequence container type.
		using iterator = container_type::iterator;				// Immutable sequence iterator.
		using const_iterator = container_type::const_iterator;	// Mutable sequence iterator.

	public:
		//Constructs an uninitialized sequencer.
		Sequencer() = default;
		// Constructs the sequencer from an array of events.
		template <size_t Size>
		explicit Sequencer(Event* (&)[Size], Callback, bool wrap = false);
		// Constructs the sequencer from a pointer to and size of events..
		Sequencer(Event* [], size_t, Callback, bool wrap = false);
		// Constructs the sequencer from a range of events.
		Sequencer(Event**, Event**, Callback, bool wrap = false);

	public:
		// Returns a mutable reference to the sequence of events.
		container_type& events();
		// Returns a immutable reference to the sequence of events.
		const container_type& events() const;
		// Sets the client callback.
		void	callback(Callback);
		// Starts the current sequence.
		void	start();
		// Stops the current sequence.
		void	stop();
		// Resets the current sequence to the beginning.
		void	reset();
		// Resumes the sequence at the current event.
		void	resume();
		// Advances to the next sequence event.
		void	next();
		// Advances to the previous sequence event.
		void	prev();
		// Sets the sequence wrap-around mode.
		void	wrap(bool);
		// Returns the current sequence wrap-around mode.
		bool	wrap() const;
		// Returns the Sequencer's current status.
		Status	status() const;
		// Returns an immutable reference to the current event.
		const Event& event() const;
		// Returns a mutable reference to the current event.
		Event& event();
		// Returns the current event's time elapsed in milliseconds.
		duration	elapsed() const;
		// Returns the index of the current event within the sequence.
		uint8_t	index() const;
		// Steps through and executes the current sequence chronologically.
		void	tick();

	private:
		// Begins the current event.
		void	begin();
		// Advances to the next event in the current sequence.
		void	advance();
		// Ends the current event.
		void	end();
		// Rewinds the sequence to the first event.
		void	rewind();
		// Calls the `tick()' method.
		void	clock() override;
		// Executes the current callback.
		void	callback(const_iterator, Event::State);

	private:
		container_type	events_;		// The current events collection.
		const_iterator	current_;		// Points to the current event in the collection.
		Callback		callback_;		// Client callback.
		bool			wrap_;			// Flag indicating whether the sequence wraps-around continuously.
		bool			done_;			// Flag indicating whether the current sequence is completed.
		bool			exec_;			// Flag indicating whether to execute the current event on resume.
		Timer			event_timer_;	// Sequence event timer.
	};

	template <size_t Size>
	Sequencer::Sequencer(Event* (&events)[Size], Callback callback, bool wrap) :
		events_(events), current_(events), callback_(callback),
		wrap_(wrap), done_(), exec_(), event_timer_()
	{

	}

	Sequencer::Sequencer(Event* events[], size_t size, Callback callback, bool wrap) :
		events_(events, size), current_(events), callback_(callback),
		wrap_(wrap), done_(), exec_(), event_timer_()
	{

	}

	Sequencer::Sequencer(Event** first, Event** last, Callback callback, bool wrap) :
		events_(first, last), current_(first), callback_(callback),
		wrap_(wrap), done_(), exec_(), event_timer_()
	{

	}

	Sequencer::container_type& Sequencer::events()
	{
		return events_;
	}

	const Sequencer::container_type& Sequencer::events() const
	{
		return events_;
	}

	void Sequencer::callback(Callback cb)
	{
		callback_ = cb;
	}


	void Sequencer::start()
	{
		if (status() != Status::Active)
		{
			rewind();
			begin();
			event_timer_.start();
		}
	}

	void Sequencer::stop()
	{
		event_timer_.stop();
	}

	void Sequencer::reset()
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

	void Sequencer::resume()
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

	void Sequencer::next()
	{
		if (++current_ == std::end(events_))
			current_ = std::begin(events_);
		exec_ = true;
		event_timer_.interval((*current_)->duration_);
		event_timer_.reset();
	}

	void Sequencer::prev()
	{
		if (current_ == std::begin(events_))
			current_ = std::end(events_);
		--current_;
		exec_ = true;
		event_timer_.interval((*current_)->duration_);
		event_timer_.reset();
	}

	void Sequencer::wrap(bool value)
	{
		wrap_ = value;
	}

	bool Sequencer::wrap() const
	{
		return wrap_;
	}

	Sequencer::Status Sequencer::status() const
	{
		return event_timer_.active()
			? Status::Active
			: (done_)
			? Status::Done
			: Status::Idle;
	}

	const Sequencer::Event& Sequencer::event() const
	{
		return **current_;
	}

	Sequencer::Event& Sequencer::event()
	{
		return const_cast<Event&>(**current_);
	}

	Sequencer::duration Sequencer::elapsed() const
	{
		return event_timer_.elapsed();
	}

	uint8_t	Sequencer::index() const
	{
		return static_cast<uint8_t>(std::distance(std::begin(events_), current_) + 1);
	}

	void Sequencer::tick()
	{
		if (event_timer_.expired())
		{
			end();
			advance();
			if (status() == Status::Active)
				begin();
		}
	}

	void Sequencer::begin()
	{
		event_timer_.interval((*current_)->duration_);
		if ((*current_)->command_)
			(*current_)->command_->execute();
		callback(current_, Event::State::Begin);
	}

	void Sequencer::advance()
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
				//callback(current_, Event::State::End); // So clients can check if done.
			}
		}
	}

	void Sequencer::end()
	{
		callback(current_, Event::State::End);
	}

	void Sequencer::rewind()
	{
		current_ = std::begin(events_);
		done_ = false;
	}

	void Sequencer::clock()
	{
		tick();
	}

	void Sequencer::callback(const_iterator event, Event::State state)
	{
		if (callback_)
			(*callback_)(**event, state);
	}
}

#endif // !defined SEQUENCER_H__