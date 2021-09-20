/*
 *	This file declares a simple interval timer type.
 *
 *	***************************************************************************
 *
 *	File: timer.h
 *	Date: April 9, 2021
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
		using clock = std::chrono::steady_clock;

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
		// returns true only if initialized and expired.
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
		// Changes the interval only, not whether timer is active.
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
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_TIMER_H