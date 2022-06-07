/*
 *	This file declares simple interval timer and event counter types.
 *
 *	***************************************************************************
 *
 *	File: timer.h
 *	Date: April 19, 2022
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
# define __PG_TIMER_H 20210409L

# include "chrono"

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// Simple interval timer.
	template<class T>
	class Timer
	{
	public:
		using duration = T;
		using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;
		using clock = std::chrono::steady_clock; // chg to clock_type.

	public:
		// Constructs an uninitialized timer.
		Timer() = default;
		// Constructs and initializes the timer with a given interval.
		Timer(duration);

	public:
		bool		active() const;
		duration	elapsed() const;
		bool		expired() const;
		void		interval(duration); // Resets the timer and assigns a new interval.
		duration	interval() const;
		void		reset();
		void		resume();
		void		start();
		void		start(duration);	// Restarts the timer with a new interval.
		void		stop();

	private:
		time_point	begin_;		// Time point when timer started.
		time_point	end_;		// Time point when timer stopped.
		duration	interval_;	// Current timer interval.
		bool		active_;	// Flag indicating whether the timer is currently running.
	};

	// Simple event counter.
	template<class T>
	class Counter
	{
	public:
		using count_type = T;

	public:
		explicit Counter(count_type = count_type());
		template<class U>
		explicit Counter(U);

	public:
		Counter& operator++();
		Counter& operator--();
		Counter operator++(int);
		Counter operator--(int);
		Counter& operator+=(count_type);
		Counter& operator-=(count_type);
		Counter& operator=(count_type);
		Counter& operator=(const Counter&);
		template<class U>
		Counter& operator+=(U);
		template<class U>
		Counter& operator-=(U);
		template<class U>
		Counter& operator=(U);
		count_type& count() const;
		count_type& count();
		void start();
		void stop();
		void resume();
		void reset();
		bool active() const;

	private:
		count_type	count_;
		bool		active_;
	};

	// Simple event counter/interval timer.
	template<class T, class U>
	class CounterTimer
	{
	public:
		using time_type = T;
		using count_type = U;
		using time_point = std::chrono::time_point<std::chrono::steady_clock, time_type>;
		using clock_type = std::chrono::steady_clock;
		struct counter_tag {};
		struct timer_tag {};

	public:
		// Constructs an uninitialized counter/timer.
		CounterTimer() = default;
		// Constructs and initializes a timer with a given interval.
		CounterTimer(time_type);
		// Constructs and initializes a counter with a given limit.
		CounterTimer(count_type);
		// Constructs and initializes a counter/timer with a given limit and interval.
		CounterTimer(count_type, time_type);

	public:
		CounterTimer&		operator++();
		CounterTimer&		operator--();
		CounterTimer		operator++(int);
		CounterTimer		operator--(int);
		CounterTimer&		operator+=(count_type);
		CounterTimer&		operator-=(count_type);
		CounterTimer&		operator=(count_type);
		CounterTimer&		operator=(const CounterTimer&);
		template<class V>
		CounterTimer&		operator+=(V);
		template<class V>
		CounterTimer&		operator-=(V);
		template<class V>
		CounterTimer&		operator=(V);
		bool				active() const;			// Checks if a counter/timer is currently active.
		bool				active(counter_tag) const;	// Checks if a counter is currently active.
		bool				active(timer_tag) const;	// Checks if a timer is currently active.
		count_type&			count() const;			// Returns an immutable reference to the current count.
		count_type&			count();				// Returns a mutable reference to the current count.	
		time_type			elapsed() const;		// Returns the currently elapsed time.
		bool				reached() const;		// Checks if a counter has reached its limit.
		bool				expired() const;		// Checks if a timer interval has expired.
		void				interval(time_type);	// Resets a timer and assigns a new interval.
		time_type			interval() const;		// Returns the timer interval. 
		void				limit(count_type);		// Resets a counter and assigns a new limit.
		count_type			limit() const;			// Returns the current count limit.
		void				reset();				// Resets a counter/timer.
		void				reset(counter_tag);				// Resets a counter/timer.
		void				reset(timer_tag);				// Resets a counter/timer.
		void				resume();				// Resumes a counter/timer.
		void				resume(counter_tag);				// Resumes a counter/timer.
		void				resume(timer_tag);				// Resumes a counter/timer.
		void				start();				// Starts a counter/timer.
		void				start(counter_tag);				// Starts a counter/timer.
		void				start(timer_tag);				// Starts a counter/timer.
		void				start(time_type);		// Restarts a timer with a new interval.
		void				start(count_type);		// Restarts a counter with a new limit.
		void				stop();					// Stops a counter/timer.
		void				stop(counter_tag);					// Stops a counter/timer.
		void				stop(timer_tag);					// Stops a counter/timer.

	private:
		time_point			begin_;			// Time point when timer started.
		time_point			end_;			// Time point when timer stopped.
		time_type			interval_;		// Current timer interval.
		count_type			count_;			// The current count.
		count_type			limit_;			// The current counter limit.
		bool				count_active_;	// Flag indicating whether the timer is currently active.
		bool				timer_active_;	// Flag indicating whether the timer is currently active.
	};


#pragma region CounterTimer

	template<class T, class U>
	CounterTimer<T, U>::CounterTimer(time_type interval) :
		count_active_(), timer_active_(), begin_(), end_(), interval_(interval), count_(), limit_()
	{

	}

	template<class T, class U>
	CounterTimer<T, U>::CounterTimer(count_type limit) :
		count_active_(), timer_active_(), begin_(), end_(), interval_(), count_(), limit_(limit)
	{

	}

	template<class T, class U>
	CounterTimer<T, U>::CounterTimer(count_type limit, time_type interval) :
		count_active_(), timer_active_(), begin_(), end_(), interval_(interval), count_(), limit_(limit)
	{

	}

	template<class T, class U>
	bool CounterTimer<T, U>::active() const
	{
		return active(counter_tag{}) || active(timer_tag{});
	}

	template<class T, class U>
	bool CounterTimer<T, U>::active(counter_tag) const
	{
		return count_active_;
	}

	template<class T, class U>
	bool CounterTimer<T, U>::active(timer_tag) const
	{
		return timer_active_;
	}

	template<class T, class U>
	typename CounterTimer<T, U>::count_type& CounterTimer<T, U>::count() const
	{
		return count_;
	}

	template<class T, class U>
	typename CounterTimer<T, U>::count_type& CounterTimer<T, U>::count()
	{
		return count_;
	}

	template<class T, class U>
	typename CounterTimer<T, U>::time_type CounterTimer<T, U>::elapsed() const
	{
		time_point now = timer_active_
			? time_point(clock_type::now())
			: end_;

		return time_type(now - begin_);
	}

	template<class T, class U>
	bool CounterTimer<T, U>::reached() const
	{
		return count_ >= limit_;
	}

	template<class T, class U>
	bool CounterTimer<T, U>::expired() const
	{
		// Returns true only if initialized and expired.
		return (interval_.count() != 0) && !(elapsed() < interval_);
	}

	template<class T, class U>
	void CounterTimer<T, U>::interval(time_type intvl)
	{
		reset();
		interval_ = intvl;
	}

	template<class T, class U>
	typename CounterTimer<T, U>::time_type CounterTimer<T, U>::interval() const
	{
		return interval_;
	}

	template<class T, class U>
	void CounterTimer<T, U>::limit(count_type lim)
	{
		reset();
		limit_ = lim;
	}

	template<class T, class U>
	typename CounterTimer<T, U>::count_type CounterTimer<T, U>::limit() const
	{
		return limit_;
	}

	template<class T, class U>
	void CounterTimer<T, U>::reset()
	{
		// Resets the elapsed time/count only, not whether counter/timer is active.
		reset(timer_tag{});
		reset(counter_tag{});
	}

	template<class T, class U>
	void CounterTimer<T, U>::reset(counter_tag)
	{
		// Resets the count only, not whether counter is active.
		count_ = count_type();
	}

	template<class T, class U>
	void CounterTimer<T, U>::reset(timer_tag)
	{
		// Resets the elapsed time only, not whether timer is active.
		begin_ = end_ = clock_type::now();
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
		count_active_ = true;
	}

	template<class T, class U>
	void CounterTimer<T, U>::resume(timer_tag)
	{
		if (!timer_active_)
			begin_ = clock_type::now() - elapsed();
		timer_active_ = true;
	}

	template<class T, class U>
	void CounterTimer<T, U>::start()
	{
		reset();
		resume();
	}

	template<class T, class U>
	void CounterTimer<T, U>::start(counter_tag)
	{
		reset(counter_tag{});
		resume(counter_tag{});
	}

	template<class T, class U>
	void CounterTimer<T, U>::start(timer_tag)
	{
		reset(timer_tag{});
		resume(timer_tag{});
	}

	template<class T, class U>
	void CounterTimer<T, U>::start(time_type intvl)
	{
		interval(intvl);
		start(timer_tag{});
	}

	template<class T, class U>
	void CounterTimer<T, U>::start(count_type lim)
	{
		limit(lim);
		start(counter_tag{});
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
		if (active(timer_tag{}))
			end_ = clock_type::now();
		timer_active_ = false;
	}

	template<class T, class U>
	void CounterTimer<T, U>::stop(counter_tag)
	{
		count_active_ = false;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator++()
	{
		if (count_active_)
			++count_;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator--()
	{
		if (count_active_)
			--count_;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U> CounterTimer<T, U>::operator++(int)
	{
		CounterTimer<T, U> tmp = *this;

		if (count_active_)
			++* this;

		return tmp;
	}

	template<class T, class U>
	CounterTimer<T, U> CounterTimer<T, U>::operator--(int)
	{
		CounterTimer<T, U> tmp = *this;

		if (count_active_)
			--* this;

		return tmp;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator+=(count_type n)
	{
		if (count_active_)
			count_ -= n;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator-=(count_type n)
	{
		if (count_active_)
			count_ += n;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator=(count_type n)
	{
		if (count_active_)
			count_ = n;

		return *this;
	}

	template<class T, class U>
	CounterTimer<T, U>& CounterTimer<T, U>::operator=(const CounterTimer<T, U>& other)
	{
		count_ = other.count_;
		return *this;
	}

	template<class T, class U>
	template<class V>
	CounterTimer<T, U>& CounterTimer<T, U>::operator+=(V n)
	{
		if (count_active_)
			count_ += static_cast<count_type>(n);

		return *this;
	}

	template<class T, class U>
	template<class V>
	CounterTimer<T, U>& CounterTimer<T, U>::operator-=(V n)
	{
		if (count_active_)
			count_ -= static_cast<count_type>(n);

		return *this;
	}

	template<class T, class U>
	template<class V>
	CounterTimer<T, U>& CounterTimer<T, U>::operator=(V n)
	{
		if (count_active_)
			count_ = static_cast<count_type>(n);

		return *this;
	}

#pragma endregion
#pragma region Timer

	template<class T>
	Timer<T>::Timer(duration interval) : active_(), begin_(), end_(), interval_(interval)
	{

	}

	template<class T>
	bool Timer<T>::active() const
	{
		return active_;
	}

	template<class T>
	typename Timer<T>::duration Timer<T>::elapsed() const 
	{
		time_point now = active_
			? time_point(clock::now())
			: end_;

		return duration(now - begin_);
	}

	template<class T>
	bool Timer<T>::expired() const
	{
		// Returns true only if initialized and expired.
		return (interval_.count() != 0) && !(elapsed() < interval_);
	}

	template<class T>
	void Timer<T>::interval(duration intvl)
	{
		reset();
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
		begin_ = end_ = clock::now();
	}

	template<class T>
	void Timer<T>::resume()
	{
		if (!active())
		{
			begin_ = clock::now() - elapsed();
			active_ = true;
		}
	}

	template<class T>
	void Timer<T>::start()
	{
		if (!active())
		{
			reset();
			resume();
		}
	}

	template<class T>
	void Timer<T>::start(duration intvl)
	{
		if (!active())
		{
			interval(intvl);
			resume();
		}
	}

	template<class T>
	void Timer<T>::stop()
	{
		if (active())
		{
			end_ = clock::now();
			active_ = false;
		}
	}

#pragma endregion
#pragma region Counter

	template<class T>
	Counter<T>::Counter(count_type count) : 
		count_(count), active_()
	{

	}

	template<class T>
	template<class U>
	Counter<T>::Counter(U count) :
		count_(static_cast<T>(count)), active_()
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
	Counter<T>& Counter<T>::operator=(count_type n)
	{
		if (active_)
			count_ = n;
		return *this;
	}

	template<class T>
	Counter<T>& Counter<T>::operator=(const Counter<T>& other)
	{
		count_ = other.count_;
		return *this;
	}


	template<class T>
	template<class U>
	Counter<T>& Counter<T>::operator+=(U n)
	{
		if (active_)
			count_ += static_cast<count_type>(n);
		return *this;
	}

	template<class T>
	template<class U>
	Counter<T>& Counter<T>::operator-=(U n)
	{
		if (active_)
			count_ -= static_cast<count_type>(n);
		return *this;
	}

	template<class T>
	template<class U>
	Counter<T>& Counter<T>::operator=(U n)
	{
		if (active_)
			count_ = static_cast<count_type>(n);
		return *this;
	}

	template<class T>
	typename Counter<T>::count_type& Counter<T>::count() const
	{
		return count_;
	}

	template<class T>
	typename Counter<T>::count_type& Counter<T>::count()
	{
		return count_;
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
	bool Counter<T>::active() const
	{
		return active_;
	}

#pragma endregion

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_TIMER_H