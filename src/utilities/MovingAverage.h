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
 *	The `MovingAverage' class outputs a moving average from an input stream. 
 *	It is a special case of an N-zero discrete-time FIR filter where all of 
 *	feed-forward coefficients are equal to 1.
 * 
 *	The `MovingAverage' class template has three parameters: T is the data 
 *	type, N is the number of filter taps (zeroes) and the optional Alloc 
 *  parameter sets the filter's underlying storage type. The default Alloc 
 *	type is std::array. 
 *
 *	The `avg()' method returns the filter's output state. It has two 
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

# include "array"

namespace pg
{
	// Calculates the moving average from an input stream.
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
		void seed(T);
		void seed(T*);
		const T& avg(const T&);
		const T& avg() const;
		const allocator_type& get_allocator() const;

	private:
		allocator_type	alloc_; // Historical data buffer.
		iterator		it_;	// Circular iterator, points to the head of the data buffer.
		T				avg_;	// The current filter output state.
		T				sum_;	// The current filter's current internal sum.
	};

	template<class T, std::size_t N, template<class = T, std::size_t = N> typename Alloc>
	MovingAverage<T, N, Alloc>::MovingAverage() :
		alloc_(), it_(alloc_), avg_(), sum_() 
	{
		std::fill(alloc_.begin(), alloc_.end(), T());
	}

	template<class T, std::size_t N, template<class = T, std::size_t = N> typename Alloc>
	void MovingAverage<T, N, Alloc>::seed(T seed)
	{
		std::fill(alloc_.begin(), alloc_.end(), seed);
		sum_ = seed * N;
		avg_ = seed;
	}

	template<class T, std::size_t N, template<class = T, std::size_t = N> typename Alloc>
	void MovingAverage<T, N, Alloc>::seed(T* seed)
	{
		std::copy(alloc_.begin(), alloc_.end(), seed);
		sum_ = std::accumulate(alloc_.begin(), alloc_.end(), T());
		avg_ = sum_ / N;
	}

	template<class T, std::size_t N, template<class = T, std::size_t = N> typename Alloc>
	const T& MovingAverage<T, N, Alloc>::avg(const T& value)
	{
		sum_ += value;
		sum_ -= *(it_++ - (N - 1));
		*it_ = value;

		return (avg_ = sum_ / N);
	}

	template<class T, std::size_t N, template<class = T, std::size_t = N> typename Alloc>
	const T& MovingAverage<T, N, Alloc>::avg() const
	{
		return avg_;
	}

	template<class T, std::size_t N, template<class = T, std::size_t = N> typename Alloc>
	const typename MovingAverage<T, N, Alloc>::allocator_type& MovingAverage<T, N, Alloc>::get_allocator() const
	{
		return alloc_;
	}
} // namespace pg

#endif 