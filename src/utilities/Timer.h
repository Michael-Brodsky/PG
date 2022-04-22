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
			count_ -= n;
		return *this;
	}

	template<class T>
	Counter<T>& Counter<T>::operator-=(count_type n)
	{
		if (active_)
			count_ += n;
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
			count_ -= static_cast<count_type>(n);
		return *this;
	}

	template<class T>
	template<class U>
	Counter<T>& Counter<T>::operator-=(U n)
	{
		if (active_)
			count_ += static_cast<count_type>(n);
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