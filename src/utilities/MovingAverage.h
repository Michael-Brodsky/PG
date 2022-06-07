/*
 *	This file defines a moving average filter class.
 *
 *	***************************************************************************
 *
 *	File: MovingAverage.h
 *	Date: October 7, 2021
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
 *	The `MovingAverage' class outputs a moving average from an input stream. 
 *	It is a special case of an N-zero discrete-time FIR filter where all of 
 *	feed-forward coefficients are equal to 1.
 * 
 *	The `MovingAverage' class template has three parameters: T is the data 
 *	type, N is the number of filter taps (zeroes) and the optional Alloc 
 *  parameter sets the filter's underlying storage type. The default Alloc 
 *	type is std::array. 
 *
 *	The `out()' method returns the filter's output state. It has two 
 *	overloads, one that inserts a new value into the stream and returns the 
 *	the resulting filter output state, and another that returns only the 
 *	current output state. The filter can be seeded with a single or a 
 *	collection of values with the `seed()' method overloads. The 
 *	`get_allocator()' method returns an immutable reference to the filter's 
 *	underlying allocator so that clients can access its properties for 
 *	calculations.
 * 
 *****************************************************************************/

#if !defined __PG_MOVINGAVERAGE_H
# define __PG_MOVINGAVERAGE_H 20211006L

# include <array>	// Default data buffer allocator type.

namespace pg
{
	// Calculates a moving average from an input stream.
	template<class T, std::size_t N, template<class = T, std::size_t = N> typename Alloc = std::array>
	class MovingAverage
	{
	public:
		using value_type = T;
		using allocator_type = Alloc<T, N>;
		using iterator = typename std::experimental::circular_iterator<allocator_type>;

	public:
		// Constructs an empty filter.
		MovingAverage();

	public:
		// Seeds the filter with a value.
		void seed(value_type);
		// Seeds the filter with values from an array beginning at a pointer.
		void seed(value_type*);
		// Updates the filter state with a value and returns the resulting output state. 
		const value_type& out(const value_type&);
		// Returns the current filter output state.
		const value_type& out() const;
		// Returns a reference to the underlying data buffer.
		const allocator_type& get_allocator() const;

	private:
		allocator_type	alloc_; // Historical data buffer.
		iterator		it_;	// Circular iterator, points to the head of the data buffer.
		value_type		avg_;	// The filter's current output state.
		value_type		sum_;	// The filter's current internal sum.
	};

	template<class value_type, std::size_t N, template<class = value_type, std::size_t = N> typename Alloc>
	MovingAverage<value_type, N, Alloc>::MovingAverage() :
		alloc_(), it_(alloc_), avg_(), sum_() 
	{
		std::fill(alloc_.begin(), alloc_.end(), value_type());
	}

	template<class value_type, std::size_t N, template<class = value_type, std::size_t = N> typename Alloc>
	void MovingAverage<value_type, N, Alloc>::seed(value_type seed)
	{
		std::fill(alloc_.begin(), alloc_.end(), seed);
		sum_ = seed * N;
		avg_ = seed;
	}

	template<class value_type, std::size_t N, template<class = value_type, std::size_t = N> typename Alloc>
	void MovingAverage<value_type, N, Alloc>::seed(value_type* seed)
	{
		std::copy(alloc_.begin(), alloc_.end(), seed);
		sum_ = std::accumulate(alloc_.begin(), alloc_.end(), value_type());
		avg_ = sum_ / N;
	}

	template<class value_type, std::size_t N, template<class = value_type, std::size_t = N> typename Alloc>
	const typename MovingAverage<value_type, N, Alloc>::value_type& 
		MovingAverage<value_type, N, Alloc>::out(const value_type& value)
	{
		sum_ += value;
		sum_ -= *(it_++ - (N - 1));
		*it_ = value;

		return (avg_ = sum_ / N);
	}

	template<class value_type, std::size_t N, template<class = value_type, std::size_t = N> typename Alloc>
	const typename MovingAverage<value_type, N, Alloc>::value_type& 
		MovingAverage<value_type, N, Alloc>::out() const
	{
		return avg_;
	}

	template<class value_type, std::size_t N, template<class = value_type, std::size_t = N> typename Alloc>
	const typename MovingAverage<value_type, N, Alloc>::allocator_type&
		MovingAverage<value_type, N, Alloc>::get_allocator() const
	{
		return alloc_;
	}

} // namespace pg

#endif 