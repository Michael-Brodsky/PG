/*
 *	This file declares simple interval timer and event counter types.
 *
 *	***************************************************************************
 *
 *	File: timer.h
 *	Date: July 9, 2022
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
 *	The `Timer' class encapsulates the behaviors of a simple interval timer
 *	that can be used to schedule or time events. The interval type is 
 *	specified by the template parameter, which must be of type 
 *	std::chrono::duration and which sets the timer's resolution. Clients can 
 *	set the interval, start, stop, resume or reset the timer. The class also 
 *	provides methods to check how much time has elapsed since a timer was 
 *	started or last resumed, whether the current interval has expired 
 *	and whether a timer is currently active. Once the the current interval 
 *	expires, the timer remains expired until reset or restarted. When running, 
 * 	the elapsed time continues to increase whether or not the current interval 
 *	is expired, until the timer is stopped. A timer whose interval is set to 0 
 *	never expires (expired() method always returns `false'), but otherwise 
 *	behaves identically to one with a non-zero interval.
 * 
 *  The `Counter' class encapsulates the behaviors of a simple event counter.
 *	The count type is specified by the template parameter, which must be an  
 *	arithmetic type and sets the counter's maximum count value. Clients can 
 *	preset the count, and increment or decrement the count arbitrarily. The 
 *	count() method returns the current count. The Counter type also has 
 *	start(), stop(), resume(), reset() and active() methods, like the Timer 
 *	type, to give the two types a more common interface. If not active, a 
 *	Counter's count cannot be modified except with the reset() method. Both 
 *	the start() and resume() method set the Counter to active, the stop() 
 *	method sets it to inactive. The reset() method only effects the count 
 *	and not the active status.
 *
 *****************************************************************************/

#if !defined __PG_TIMER_H 
# define __PG_TIMER_H 20220609L

# include <chrono>

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Simple interval timer.
	template<class T>
	class Timer
	{
		// The clock source is set to millis() (std::chrono::system_clock), 
		// since it may be more useful as micros() (std::chrono::steady_clock)
		// rolls over after 70 minutes. Might want to add another template 
		// parameter so clients can select the source.
	public:
		using duration = T;
		using time_point = std::chrono::time_point<std::chrono::system_clock, duration>;
		using clock_type = std::chrono::system_clock;

	public:
		// Constructs and initializes the timer with a given interval.
		explicit Timer(duration = duration());
		// Copy constructor.
		Timer(const Timer&);

	public:
		Timer&		operator=(const Timer&);	// Copy assignment operator.
		bool		active() const;				// Checks whether the timer is currently active.
		duration	elapsed() const;			// Returns the current elapsed time.
		bool		expired() const;			// Checks whether the timer is currently expired.
		void		interval(duration);			// Assigns a new interval.
		duration	interval() const;			// Returns the current timer interval.
		void		reset();					// Resets the elapsed time.
		void		resume();					// Resumes the timer at the currently elapsed time.
		void		start();					// Starts the timer.
		void		start(duration);			// Starts the timer with a new interval.
		void		stop();						// Stops the timer at the currently elapsed time.

	private:
		time_point	begin_;		// Time point when timer started.
		time_point	end_;		// Time point when timer stopped.
		duration	interval_;	// Current timer interval.
		bool		active_;	// Flag indicating whether the timer is currently active.
	};

	// Simple event counter.
	template<class T>
	class Counter
	{
	public:
		using count_type = T;

	public:
		// Constructs and initializes the counter with a given limit.
		explicit Counter(count_type = count_type());
		// Copy constructor.
		Counter(const Counter&);

	public:
		Counter&	operator++();				// Increments the counter.
		Counter&	operator--();				// Decrements the counter.
		Counter		operator++(int);			// Increments the counter.
		Counter		operator--(int);			// Decrements the counter.
		Counter&	operator+=(count_type);		// Increments the counter by n.
		Counter&	operator-=(count_type);		// Decrements the counter by n.
		Counter&	operator=(const Counter&);	// Copy assignment operator.
		count_type	count() const;				// Returns the current count.
		bool		active() const;				// Checks whether the counter is currently expired.
		bool		exceeded() const;			// Checks whether the counter has exceeded its limit.
		void		limit(count_type);			// Assigns a count limit.
		count_type	limit() const;				// Returns the current count limit.
		void		reset();					// Resets the count.
		void		resume();					// Resumes the counter at the current count.
		void		start();					// Starts the counter.
		void		stop();						// Stops the counter at the current count.

	private:
		count_type	count_;		// The current count.
		count_type	limit_;		// The current count limit.
		bool		active_;	// Flag indicating whether the counter is currently active.
	};

	// Simple event counter/interval timer.
	template<class T, class U>
	class CounterTimer
	{
	public:
		using time_type = T;
		using count_type = U;
		using timer_type = Timer<time_type>;
		using counter_type = Counter<count_type>;
		struct counter_tag {};
		struct timer_tag {};

	public:
		// Constructs an uninitialized counter/timer.
		CounterTimer();
		// Constructs and initializes a counter/timer with a given interval.
		explicit CounterTimer(time_type);
		// Constructs and initializes a counter/timer with a given limit.
		explicit CounterTimer(count_type);
		// Constructs and initializes a counter/timer with a given limit and interval.
		explicit CounterTimer(count_type, time_type);
		// Copy constructor.
		CounterTimer(const CounterTimer<T, U>&);

	public:
		CounterTimer&		operator++();				// Increments the counter.
		CounterTimer&		operator--();				// Decrements the counter.
		CounterTimer		operator++(int);			// Increments the counter.
		CounterTimer		operator--(int);			// Decrements the counter.
		CounterTimer&		operator+=(count_type);		// Increments the counter by n.
		CounterTimer&		operator-=(count_type);		// Decrements the counter by n.
		CounterTimer&		operator=(const CounterTimer<T, U>&);	// Copy assignment operator.
		bool				active() const;				// Checks if the counter/timer is currently active.
		bool				active(counter_tag) const;	// Checks if the counter is currently active.
		bool				active(timer_tag) const;	// Checks if the timer is currently active.
		count_type			count() const;				// Returns the current count.
		time_type			elapsed() const;			// Returns the current elapsed time.
		bool				exceeded() const;			// Checks if the counter has exceeded its limit.
		bool				expired() const;			// Checks if the timer interval has expired.
		void				interval(time_type);		// Assigns a new timer interval.
		time_type			interval() const;			// Returns the current timer interval. 
		void				limit(count_type);			// Assigns a new count limit.
		count_type			limit() const;				// Returns the current count limit.
		void				reset();					// Resets the counter/timer.
		void				reset(counter_tag);			// Resets the counter.
		void				reset(timer_tag);			// Resets the timer.
		void				resume();					// Resumes the counter/timer.
		void				resume(counter_tag);		// Resumes the counter.
		void				resume(timer_tag);			// Resumes the timer.
		void				start();					// Starts the counter/timer.
		void				start(counter_tag);			// Starts the counter.
		void				start(timer_tag);			// Starts the timer.
		void				start(time_type);			// Restarts the timer with a new interval.
		void				start(count_type);			// Restarts the counter with a new limit.
		void				stop();						// Stops the counter/timer.
		void				stop(counter_tag);			// Stops the counter.
		void				stop(timer_tag);			// Stops the timer.

	private:
		counter_type	counter_;	// Counter object.
		timer_type		timer_;		// Timer object.
	};
#pragma region Timer

	template<class T>
	Timer<T>::Timer(duration interval) : active_(), begin_(), end_(), interval_(interval)
	{

	}

	template<class T>
	Timer<T>::Timer(const Timer<T>& other) :
		active_(other.active_), begin_(other.begin_), end_(other.end_), interval_(other.interval_)
	{

	}

	template<class T>
	Timer<T>& Timer<T>::operator=(const Timer<T>& other)
	{
		active_ = other.active_;
		begin_ = other.begin_; 
		end_ = other.end_; 
		interval_ = other.interval_;

		return *this;
	}

	template<class T>
	bool Timer<T>::active() const
	{
		return active_;
	}

	template<class T>
	typename Timer<T>::duration Timer<T>::elapsed() const 
	{
		if (active_)
			end_ = time_point(clock_type::now());

		return duration(end_ - begin_);
	}

	template<class T>
	bool Timer<T>::expired() const
	{
		// Returns true only if initialized and expired.
		return !(interval_.count() == 0 || elapsed() < interval_);
	}

	template<class T>
	void Timer<T>::interval(duration intvl)
	{
		interval_ = intvl;
	}

	template<class T>
	typename Timer<T>::duration Timer<T>::interval() const
	{
		return interval_;
	}

	template<class T>
	void Timer<T>::reset()
	{
		// Resets the elapsed time only, not whether timer is active.
		begin_ = end_ = clock_type::now();
	}

	template<class T>
	void Timer<T>::resume()
	{
		if (!active())
		{
			begin_ = clock_type::now() - elapsed();
			active_ = true;
		}
	}

	template<class T>
	void Timer<T>::start()
	{
		reset();
		resume();
	}

	template<class T>
	void Timer<T>::start(duration intvl)
	{
		interval(intvl);
		start();
	}

	template<class T>
	void Timer<T>::stop()
	{
		if (active())
		{
			end_ = clock_type::now();
			active_ = false;
		}
	}

#pragma endregion
#pragma region Counter

	template<class T>
	Counter<T>::Counter(count_type limit) : 
		count_(), limit_(limit), active_()
	{
		
	}

	template<class T>
	Counter<T>::Counter(const Counter<T>& other) :
		count_(other.count_), limit_(other.limit_), active_(other.active_)
	{

	}

	template<class T>
	Counter<T>& Counter<T>::operator++()
	{
		if(active_)
			++count_;

		return *this;
	}

	template<class T>
	Counter<T>& Counter<T>::operator--()
	{
		if (active_) 
			--count_;

		return *this;
	}

	template<class T>
	Counter<T> Counter<T>::operator++(int)
	{
		Counter<T> tmp = *this;

		if (active_)
			++*this;

		return tmp;
	}

	template<class T>
	Counter<T> Counter<T>::operator--(int)
	{
		Counter<T> tmp = *this;

		if (active_)
			--*this;

		return tmp;
	}

	template<class T>
	Counter<T>& Counter<T>::operator+=(count_type n)
	{
		if (active_) 
			count_ += n;

		return *this;
	}

	template<class T>
	Counter<T>& Counter<T>::operator-=(count_type n)
	{
		if (active_)
			count_ -= n;

		return *this;
	}

	template<class T>
	Counter<T>& Counter<T>::operator=(const Counter<T>& other)
	{
		count_ = other.count_;
		limit_ = other.limit_;
		active_ = other.active_;

		return *this;
	}

	template<class T>
	bool Counter<T>::active() const
	{
		return active_;
	}

	template<class T>
	typename Counter<T>::count_type Counter<T>::count() const
	{
		return count_;
	}

	template<class T>
	bool Counter<T>::exceeded() const
	{
		return !(limit_ == count_type() || limit_ < count_);
	}

	template<class T>
	void Counter<T>::limit(count_type value)
	{
		limit_ = value;
	}

	template<class T>
	typename Counter<T>::count_type Counter<T>::limit() const
	{
		return limit_;
	}

	template<class T>
	void Counter<T>::resume()
	{
		active_ = true; 
	}

	template<class T>
	void Counter<T>::reset()
	{
		count_ = count_type();
	}

	template<class T>
	void Counter<T>::start()
	{
		reset();
		resume();
	}

	template<class T>
	void Counter<T>::stop()
	{
		active_ = false;
	}

#pragma endregion
#pragma region CounterTimer

	template<class T, class U>
	CounterTimer<T, U>::CounterTimer() : 
		counter_(), timer_()
	{

	}

	template<class T, class U>
	CounterTimer<T, U>::CounterTimer(time_type interval) :
		counter_(), timer_(interval)
	{

	}

	template<class T, class U>
	CounterTimer<T, U>::CounterTimer(count_type limit) :
		counter_(limit), timer_()
	{

	}

	template<class T, class U>
	CounterTimer<T, U>::CounterTimer(count_type limit, time_type interval) :
		counter_(limit), timer_(interval)
	{

	}

	template<class T, class U>
	CounterTimer<T, U>::CounterTimer(const CounterTimer<T, U>& other) :
		counter_(other.counter_), timer_(other.timer_)
	{

	}


	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator++()
	{
		++counter_;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator--()
	{
		--counter_;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U> CounterTimer<T, U>::operator++(int)
	{
		counter_++;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U> CounterTimer<T, U>::operator--(int)
	{
		counter_--;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator+=(count_type n)
	{
		counter_ += n;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator-=(count_type n)
	{
		counter_ -= n;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator=(const CounterTimer<T, U>& other)
	{
		counter_ = other.counter_;
		timer_ = other.timer_;

		return *this;
	}

	template<class T, class U>
	bool CounterTimer<T, U>::active() const
	{
		return active(counter_tag{}) || active(timer_tag{});
	}

	template<class T, class U>
	bool CounterTimer<T, U>::active(counter_tag) const
	{
		return counter_.active();
	}

	template<class T, class U>
	bool CounterTimer<T, U>::active(timer_tag) const
	{
		return timer_.active();
	}

	template<class T, class U>
	typename CounterTimer<T, U>::count_type CounterTimer<T, U>::count() const
	{
		return counter_.count();
	}

	template<class T, class U>
	typename CounterTimer<T, U>::time_type CounterTimer<T, U>::elapsed() const
	{
		return timer_.elapsed();
	}

	template<class T, class U>
	bool CounterTimer<T, U>::exceeded() const
	{
		return counter_.exceeded();
	}

	template<class T, class U>
	bool CounterTimer<T, U>::expired() const
	{
		return timer_.expired();
	}

	template<class T, class U>
	void CounterTimer<T, U>::interval(time_type intvl)
	{
		timer_.interval(intvl);
	}

	template<class T, class U>
	typename CounterTimer<T, U>::time_type CounterTimer<T, U>::interval() const
	{
		return timer_.interval();
	}

	template<class T, class U>
	void CounterTimer<T, U>::limit(count_type value)
	{
		counter_.limit(value);
	}

	template<class T, class U>
	typename CounterTimer<T, U>::count_type CounterTimer<T, U>::limit() const
	{
		return counter_.limit();
	}

	template<class T, class U>
	void CounterTimer<T, U>::reset()
	{
		reset(counter_tag{});
		reset(timer_tag{});
	}

	template<class T, class U>
	void CounterTimer<T, U>::reset(counter_tag)
	{
		counter_.reset();
	}

	template<class T, class U>
	void CounterTimer<T, U>::reset(timer_tag)
	{
		timer_.reset();
	}

	template<class T, class U>
	void CounterTimer<T, U>::resume()
	{
		resume(timer_tag{});
		resume(counter_tag{});
	}

	template<class T, class U>
	void CounterTimer<T, U>::resume(counter_tag)
	{
		counter_.resume();
	}

	template<class T, class U>
	void CounterTimer<T, U>::resume(timer_tag)
	{
		timer_.resume();
	}

	template<class T, class U>
	void CounterTimer<T, U>::start()
	{
		start(counter_tag{});
		start(timer_tag{});
	}

	template<class T, class U>
	void CounterTimer<T, U>::start(counter_tag)
	{
		counter_.start();
	}

	template<class T, class U>
	void CounterTimer<T, U>::start(timer_tag)
	{
		timer_.start();
	}

	template<class T, class U>
	void CounterTimer<T, U>::start(time_type intvl)
	{
		timer_.start(intvl);
	}

	template<class T, class U>
	void CounterTimer<T, U>::start(count_type limit)
	{
		counter_.start(limit);
	}

	template<class T, class U>
	void CounterTimer<T, U>::stop()
	{
		stop(timer_tag{});
		stop(counter_tag{});
	}

	template<class T, class U>
	void CounterTimer<T, U>::stop(timer_tag)
	{
		timer_.stop();
	}

	template<class T, class U>
	void CounterTimer<T, U>::stop(counter_tag)
	{
		counter_.stop();
	}

#pragma endregion

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_TIMER_H