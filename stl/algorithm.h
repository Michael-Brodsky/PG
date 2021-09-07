/*
 *	This file defines several C++ Standard Template Library (STL) algorithms 
 *	that operate on ranges of elements.
 *
 *	***************************************************************************
 *
 *	File: algorithm.h
 *	Date: August 17, 2021
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
 *	***************************************************************************
 * 
 *	Description:
 * 
 *		This file defines objects in the <algorithm> header of the C++
 *		Standard Template Library (STL) implementation - to the extent they
 *		are supported by the Arduino implementation. The objects behave 
 *		according to the ISO C++11 Standard: (ISO/IEC 14882:2011).
 *
 *	**************************************************************************/

#if !defined ALGORITHM_H__
# define ALGORITHM_H__ 20210817L

# include "functional.h"	// STL compare functions.
# include "utility.h"		// std::pair, std::make_pair, std::move
# include "iterator.h"		// `iterator_traits' and related types.

# if defined __HAS_NAMESPACES

#  if defined min 
#   undef min	// Arduino `min' macro define conflicts with std::min.
#  endif
#  if defined max 
#   undef max	// Arduino `max' macro define conflicts with std::max.
#  endif

namespace std
{

# pragma region non-modifying_sequence_operations

template<class InputIt, class Function>
Function for_each(InputIt first, InputIt last, Function fn)
{
	while (first != last) 
	{
		fn(*first);
		++first;
	}
	return fn;
}

template<class InputIt, class T>
	typename iterator_traits<InputIt>::difference_type
		count(InputIt first, InputIt last, const T& value) 
{	
	typename iterator_traits<InputIt>::difference_type n = 0;
	for(; first != last; ++first) 
		if(*first == value) 
			n++;
	return n;
}

template< class InputIt, class UnaryPredicate >
	typename iterator_traits<InputIt>::difference_type
		count_if(InputIt first, InputIt last, UnaryPredicate p)
{
	typename iterator_traits<InputIt>::difference_type ret = 0;
	for (; first != last; ++first) {
		if (p(*first)) {
			ret++;
		}
	}
	return ret;
}

template<class InputIt1, class InputIt2>
	pair<InputIt1, InputIt2>
		mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2)
{
	while (first1 != last1 && *first1 == *first2) 
	{
		++first1, ++first2;
	}
	return std::make_pair(first1, first2);
}
	
template<class InputIt1, class InputIt2, class BinaryPredicate>
	pair<InputIt1, InputIt2>
		mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate p)
{
	while (first1 != last1 && p(*first1, *first2)) 
		++first1, ++first2;
	
	return std::make_pair(first1, first2);
}

template<class InputIt1, class InputIt2>
	pair<InputIt1, InputIt2>
		mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
	while (first1 != last1 && first2 != last2 && *first1 == *first2) 
		++first1, ++first2;
	
	return std::make_pair(first1, first2);
}

template<class InputIt, class T>
InputIt find(InputIt first, InputIt last, const T& value)
{
	InputIt it = last;
	for(; first != last; ++first) 
	{
		if(*first == value) 
		{ 
			it = first;
			break;
		}
	}
	return it;
}

template<class InputIt, class UnaryPredicate>
InputIt find_if(InputIt first, InputIt last, UnaryPredicate p)
{
	for (; first != last; ++first) 
	{
		if (p(*first)) 
			return first;
	}
	return last;
}

template<class InputIt, class UnaryPredicate>
InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate q)
{
	for (; first != last; ++first) 
	{
		if (!q(*first)) 
			return first;
	}
	return last;
}

template<class InputIt, class UnaryPredicate>
bool all_of(InputIt first, InputIt last, UnaryPredicate p)
{
	return std::find_if_not(first, last, p) == last;
}

template<class InputIt, class UnaryPredicate>
bool any_of(InputIt first, InputIt last, UnaryPredicate p)
{
	return std::find_if(first, last, p) != last;
}

template<class InputIt, class UnaryPredicate>
bool none_of(InputIt first, InputIt last, UnaryPredicate p)
{
	return find_if(first, last, p) == last;
}


template<class ForwardIt1, class ForwardIt2>
ForwardIt1 search(ForwardIt1 first, ForwardIt1 last, ForwardIt2 seq_first, ForwardIt2 seq_last)
{
	ForwardIt1 result = ForwardIt1();
	for (; !std::is_dereferenceable(result); ++first)
	{
		ForwardIt1 it = first;
		for (ForwardIt2 seq_it = seq_first; ; ++it, ++seq_it)
		{
			if (seq_it == seq_last)
			{
				result = first;
				break;
			}
			if (it == last)
			{
				result = last;
				break;
			}
			if (!(*it == *seq_it)) break;
		}
	}
	return result;
}

template< class ForwardIt1, class ForwardIt2, class BinaryPredicate >
ForwardIt1 search(ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first, ForwardIt2 s_last, BinaryPredicate p)
{
	for (; ; ++first)
	{
		ForwardIt1 it = first;
		for (ForwardIt2 s_it = s_first; ; ++it, ++s_it)
		{
			if (s_it == s_last)
				return first;
			if (it == last)
				return last;
			if (!p(*it, *s_it))
				break;
		}
	}
}

template< class ForwardIt, class Size, class T >
ForwardIt search_n(ForwardIt first, ForwardIt last, Size count, const T& value)
{
	if (count <= 0)
		return first;
	for (; first != last; ++first)
	{
		if (!(*first == value))
			continue;

		ForwardIt candidate = first;
		Size cur_count = 0;

		while (true)
		{
			++cur_count;
			if (cur_count >= count)
				return candidate; // success
			++first;
			if (first == last)
				return last; // exhausted the list
			if (!(*first == value))
				break; // too few in a row
		}
	}

	return last;
}

template< class ForwardIt, class Size, class T, class BinaryPredicate >
ForwardIt search_n(ForwardIt first, ForwardIt last, Size count, const T& value, BinaryPredicate p)
{
	if (count <= 0)
		return first;

	for (; first != last; ++first)
	{
		if (!p(*first, value))
			continue;

		ForwardIt candidate = first;
		Size cur_count = 0;

		while (true)
		{
			++cur_count;
			if (cur_count >= count)
				return candidate; // success
			++first;
			if (first == last)
				return last; // exhausted the list
			if (!p(*first, value))
				break; // too few in a row
		}
	}

	return last;
}

template<class ForwardIt1, class ForwardIt2>
ForwardIt1 find_end(ForwardIt1 first, ForwardIt1 last, ForwardIt2 seq_first, ForwardIt2 seq_last)
{
	ForwardIt1 result = last;
	if(seq_first != seq_last) 
	{
		while(1) 
		{
			ForwardIt1 search_result = std::search(first, last, seq_first, seq_last);
			if(search_result == last) 
				break;
			else 
			{
				result = search_result;
				first = result;
				++first;
			}
		}
	}
	return result;
}

template<class ForwardIt1, class ForwardIt2, class BinaryPredicate>
ForwardIt1 find_end(ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first, ForwardIt2 s_last, BinaryPredicate p)
{
	if (s_first == s_last)
		return last;
	ForwardIt1 result = last;
	while (true) 
	{
		ForwardIt1 new_result = std::search(first, last, s_first, s_last, p);
		if (new_result == last) 
			break;
		else 
		{
			result = new_result;
			first = result;
			++first;
		}
	}
	return result;
}

template <class ForwardIt>
ForwardIt adjacent_find(ForwardIt first, ForwardIt last)
{
	ForwardIt result = last;
	if (first != last)
	{
		ForwardIt next = first; ++next;
		while (next != last) 
		{
			if (*first == *next) 
			{
				result = first;
				break;
			}
			++first; ++next;
		}
	}
	return result;
}

template<class ForwardIt, class BinaryPredicate>
ForwardIt adjacent_find(ForwardIt first, ForwardIt last, BinaryPredicate p)
{
	if (first == last) 
		return last;
	
	ForwardIt next = first;
	++next;
	for (; next != last; ++next, ++first) 
	{
		if (p(*first, *next)) 
			return first;
	}
	return last;
}

template<class InputIt, class ForwardIt>
InputIt find_first_of(InputIt first1, InputIt last1, ForwardIt first2, ForwardIt last2)
{
	while (first1 != last1) 
	{
		for (ForwardIt it = first2; it != last2; ++it) 
		{
			if (*it == *first1)
			return first1;
		}
		++first1;
	}
	return last1;
}

template<class InputIt, class ForwardIt, class BinaryPredicate>
InputIt find_first_of(InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last,	BinaryPredicate p)
{
	for (; first != last; ++first) {
		for (ForwardIt it = s_first; it != s_last; ++it) 
		{
			if (p(*first, *it)) 
				return first;
		}
	}
	return last;
}
template<class InputIt, class UnaryPredicate>
bool is_partitioned(InputIt first, InputIt last, UnaryPredicate p)
{
	for (; first != last; ++first)
		if (!p(*first))
			break;
	for (; first != last; ++first)
		if (p(*first))
			return false;
	return true;
}

template<class ForwardIt, class UnaryPredicate>
ForwardIt partition_point(ForwardIt first, ForwardIt last, UnaryPredicate p) 
{
	auto n = std::distance(first, last);
	while (n > 0)
	{
		ForwardIt it = first;
		auto step = n / 2;
		std::advance(it, step);
		if (p(*it)) { first = ++it; n -= step + 1; }
		else n = step;
	}
	return first;
}

#pragma endregion
#pragma region modifying_sequence_operations

template<class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt dest) 
{
	while(first != last) 
		*dest++ = *first++;
	return dest;
}

template< class InputIt, class OutputIt, class UnaryPredicate >
OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first,	UnaryPredicate pred)
{
	while (first != last) 
	{
		if (pred(*first))
			*d_first++ = *first;
		first++;
	}

	return d_first;
}

template<class BidirIt1, class BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 dest)
{
	while (last!=first) 
		*(--dest) = *(--last);
	return dest;
}

template<class InputIt, class Size, class OutputIt>
OutputIt copy_n(InputIt first, Size count, OutputIt result)
{
	if (count > 0) {
		*result++ = *first;
		for (Size i = 1; i < count; ++i) {
			*result++ = *++first;
		}
	}
	return result;
}

template<class InputIt, class OutputIt>
OutputIt move(InputIt first, InputIt last, OutputIt dest)
{
	while (first != last) 
	{
		*dest = std::move(*first);
		++dest; 
		++first;
	}
	return dest;
}

template<class BidirIt1, class BidirIt2>
BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 dest)
{
	while (last != first) *(--dest) = std::move(*(--last));
	return dest;
}

template<class ForwardIt, class T>
void fill(ForwardIt first, ForwardIt last, const T& value) 
{
	while(first != last) 
		*first++ = value;
}

template<class OutputIt, class Size, class T>
OutputIt fill_n(OutputIt first, Size count, const T& value)
{
	for (Size i = 0; i < count; i++) 
		*first++ = value;
	
	return first;
}

template< class ForwardIt, class Generator >
void generate(ForwardIt first, ForwardIt last, Generator g)
{
	while (first != last) 
		*first++ = g();
}

template< class OutputIt, class Size, class Generator >
OutputIt generate_n(OutputIt first, Size count, Generator g)
{
	for (Size i = 0; i < count; i++) 
		*first++ = g();

	return first;
}

template <class ForwardIt, class T>
ForwardIt remove(ForwardIt first, ForwardIt last, const T& val)
{
	first = std::find(first, last, val);
	if (first != last)
	{
		for (ForwardIt i = first; ++i != last;)
		{
			if (!(*i == val))
				*first++ = std::move(*i);
		}
	}
	return first;
}

template<class ForwardIt, class UnaryPredicate>
ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPredicate p)
{
	first = std::find_if(first, last, p);
	if (first != last)
	{
		for (ForwardIt i = first; ++i != last; )
		{
			if (!p(*i))
				*first++ = std::move(*i);
		}
	}
	return first;
}

template <class InputIt, class OutputIt, class T>
OutputIt remove_copy(InputIt first, InputIt last, OutputIt result, const T& val)
{
	while (first != last) {
		if (!(*first == val)) 
		{
			*result = *first;
			++result;
		}
		++first;
	}
	return result;
}

template<class InputIt, class OutputIt, class UnaryPredicate>
OutputIt remove_copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate p)
{
	for (; first != last; ++first) 
	{
		if (!p(*first)) 
			*d_first++ = *first;
	}
	return d_first;
}
template <class ForwardIt, class T>
void replace(ForwardIt first, ForwardIt last, const T& old_value, const T& new_value)
{
	while (first != last) 
	{
		if (*first == old_value) 
			*first = new_value;
		++first;
	}
}

template<class ForwardIt, class UnaryPredicate, class T>
void replace_if(ForwardIt first, ForwardIt last, UnaryPredicate p, const T& new_value)
{
	for (; first != last; ++first) 
	{
		if (p(*first)) 
			*first = new_value;
	}
}

template <class InputIt, class OutputIt, class T>
OutputIt replace_copy(InputIt first, InputIt last, OutputIt result, const T& old_value, const T& new_value)
{
	while (first != last) 
	{
		*result = (*first == old_value) ? new_value : *first;
		++first; 
		++result;
	}
	return result;
}

template<class InputIt, class OutputIt,	class UnaryPredicate, class T>
OutputIt replace_copy_if(InputIt first, InputIt last, OutputIt d_first,	UnaryPredicate p, const T& new_value)
{
	for (; first != last; ++first) 
		*d_first++ = p(*first) ? new_value : *first;

	return d_first;
}

template<class ForwardIt1, class ForwardIt2>
void iter_swap(ForwardIt1 a, ForwardIt2 b)
{
	std::swap(*a, *b);
}

template<class ForwardIt1, class ForwardIt2>
ForwardIt2 swap_ranges(ForwardIt1 first, ForwardIt1 last, ForwardIt2 dest)
{
	while(first != last) 
		std::iter_swap(first++, dest++);
	return dest;
}

template <class BidirIt>
void reverse(BidirIt first, BidirIt last)
{
	while ((first != last) && (first != --last)) 
	{
		std::iter_swap (first, last);
		++first;
	}
}

template <class BidirIt, class OutputIt>
OutputIt reverse_copy(BidirIt first, BidirIt last, OutputIt dest)
{
	while (first != last) 
	{
		--last;
		*dest = *last;
		++dest;
	}
	return dest;
}

template <class ForwardIt>
void rotate(ForwardIt first, ForwardIt middle, ForwardIt last)
{
	ForwardIt next = middle;
	while (first != next)
	{
		std::swap(*first++,*next++);
		if (next == last) 
			next = middle;
		else if (first == middle) 
			middle = next;
	}
}

template <class ForwardIt, class OutputIt>
OutputIt rotate_copy(ForwardIt first, ForwardIt middle, ForwardIt last, OutputIt dest)
{
	dest = std::copy(middle,last,dest);
	return std::copy(first,middle,dest);
}

template <class ForwardIt>
ForwardIt unique(ForwardIt first, ForwardIt last)
{
	if (first == last)
		return last;

	ForwardIt result = first;
	while (++first != last) 
	{
		if (!(*result == *first) && ++result != first) 
			*result = std::move(*first);
	}
	return ++result;
}

template<class ForwardIt, class BinaryPredicate>
ForwardIt unique(ForwardIt first, ForwardIt last, BinaryPredicate p)
{
	if (first == last)
		return last;

	ForwardIt result = first;
	while (++first != last) 
	{
		if (!p(*result, *first) && ++result != first) 
			*result = std::move(*first);
	}
	return ++result;
}

template <class InputIt, class OutputIt>
OutputIt unique_copy(InputIt first, InputIt last, OutputIt dest)
{
	if (first == last) return dest;

	*dest = *first;
	while (++first != last) {
		typename iterator_traits<InputIt>::value_type val = *first;
		if (!(*dest == val))
			*(++dest) = val;
	}
	return ++dest;
}

template <class InputIt, class OutputIt, class BinaryPredicate>
OutputIt unique_copy(InputIt first, InputIt last, OutputIt dest, BinaryPredicate p)
{
	if (first == last) return dest;

	*dest = *first;
	while (++first != last) 
	{
		typename iterator_traits<InputIt>::value_type val = *first;
		if (!p(*dest, val))
			*(++dest) = val;
	}
	return ++dest;
}

template<class ForwardIt, class UnaryPredicate>
ForwardIt partition(ForwardIt first, ForwardIt last, UnaryPredicate p)
{
	first = std::find_if_not(first, last, p);
	if (first == last) return first;

	for (ForwardIt i = std::next(first); i != last; ++i) {
		if (p(*i)) {
			std::iter_swap(i, first);
			++first;
		}
	}
	return first;
}

/* TODO */
template<class BidirIt, class UnaryPredicate>
BidirIt stable_partition(BidirIt first, BidirIt last, UnaryPredicate p);

template<class InputIt, class OutputIt1, class OutputIt2, class UnaryPredicate>
	pair<OutputIt1, OutputIt2>
		partition_copy(InputIt first, InputIt last,
			OutputIt1 d_first_true, OutputIt2 d_first_false, UnaryPredicate p)
{
	while (first != last) 
	{
		if (p(*first)) 
		{
			*d_first_true = *first;
			++d_first_true;
		}
		else 
		{
			*d_first_false = *first;
			++d_first_false;
		}
		++first;
	}
	return std::pair<OutputIt1, OutputIt2>(d_first_true, d_first_false);
}

template<class InputIt, class OutputIt, class UnaryOperation>
OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first,	UnaryOperation unary_op)
{
	while (first1 != last1)
		*d_first++ = unary_op(*first1++);

	return d_first;
}

template<class InputIt1, class InputIt2, class OutputIt, class BinaryOperation>
OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first, BinaryOperation binary_op)
{
	while (first1 != last1)
		*d_first++ = binary_op(*first1++, *first2++);
	
	return d_first;
}

template<class InputIt1, class InputIt2, class OutputIt>
OutputIt merge(InputIt1 first1, InputIt1 last1,	InputIt2 first2, InputIt2 last2, OutputIt d_first)
{
	for (; first1 != last1; ++d_first) 
	{
		if (first2 == last2) 
			return std::copy(first1, last1, d_first);
		if (*first2 < *first1) 
		{
			*d_first = *first2;
			++first2;
		}
		else {

			*d_first = *first1;
			++first1;
		}
	}
	return std::copy(first2, last2, d_first);
}

template<class InputIt1, class InputIt2, class OutputIt, class Compare>
OutputIt merge(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt d_first, Compare comp)
{
	for (; first1 != last1; ++d_first) {

		if (first2 == last2) 
			return std::copy(first1, last1, d_first);

		if (comp(*first2, *first1)) 
		{
			*d_first = *first2;
			++first2;
		}
		else 
		{
			*d_first = *first1;
			++first1;
		}
	}
	return std::copy(first2, last2, d_first);
}

template<class BidirIt>
bool next_permutation(BidirIt first, BidirIt last)
{
	if (first == last) return false;
	BidirIt i = last;
	if (first == --i) return false;

	while (true) 
	{
		BidirIt i1, i2;

		i1 = i;
		if (*--i < *i1) 
		{
			i2 = last;
			while (!(*i < *--i2));
			std::iter_swap(i, i2);
			std::reverse(i1, last);
			return true;
		}
		if (i == first) 
		{
			std::reverse(first, last);
			return false;
		}
	}
}

template<class BidirIt>
bool prev_permutation(BidirIt first, BidirIt last)
{
	if (first == last) return false;
	BidirIt i = last;
	if (first == --i) return false;

	while (true) 
	{
		BidirIt i1, i2;

		i1 = i;
		if (*i1 < *--i) 
		{
			i2 = last;
			while (!(*--i2 < *i));
			std::iter_swap(i, i2);
			std::reverse(i1, last);
			return true;
		}
		if (i == first) 
		{
			std::reverse(first, last);
			return false;
		}
	}
}

#pragma endregion
#pragma region sorting_operations

template <class ForwardIt, class Compare>
ForwardIt is_sorted_until (ForwardIt first, ForwardIt last, Compare comp)
{
	ForwardIt next = last;
	if (first != last) 
	{
		next = first;
		while (++next != last) 
		{
			if (comp(*next, *first))
				break;
			first = next;
		}
	}
	return next;
}

template<class ForwardIt>
bool is_sorted(ForwardIt first, ForwardIt last)
{
	return std::is_sorted_until(first, last) == last;
}

template<class ForwardIt, class Compare>
bool is_sorted(ForwardIt first, ForwardIt last, Compare comp)
{
	return std::is_sorted_until(first, last, comp) == last;
}

template<class ForwardIt>
ForwardIt is_sorted_until(ForwardIt first, ForwardIt last)
{
	return std::is_sorted_until(
		first, last, less<typename iterator_traits<ForwardIt>::value_type>());
}

struct heap_sort_tag {};
struct insertion_sort_tag{};
struct quick_sort_tag {};

	namespace detail
	{
		template <class RandomIt>
		RandomIt partition(RandomIt first, RandomIt last)
		{
			RandomIt i = first, j = first;

			while (j < last)
			{
				if (*j < *last)
				{
					std::iter_swap(i, j);
					++i;
				}
				++j;
			}
			std::iter_swap(i, last);

			return i;
		}

		template <class RandomIt>
		void max_heap(RandomIt first, RandomIt last, RandomIt root)
		{
			RandomIt largest = root;
			RandomIt left = first + 2 * std::distance(first, root) + 1;
			RandomIt right = left + 1;

			if (left < last && *left >* largest)
				largest = left;
			if (right < last && *right >* largest)
				largest = right;
			if (largest != root)
			{
				std::iter_swap(root, largest);
				detail::max_heap(first, last, largest);
			}
		}
	} // namespace detail

template <class RandomIt>
void make_heap(RandomIt first, RandomIt last)
{
	RandomIt it = first + std::distance(first, last) / 2 - 1;

	while (it >= first)
		detail::max_heap(first, last, it--);
}

template <class RandomIt>
bool is_heap(RandomIt first, RandomIt last, RandomIt root)
{
	bool result = false;

	if (root > first + (std::distance(first, last) - 2) / 2)
		result = true;
	else
	{
		RandomIt left = first + 2 * std::distance(first, root) + 1;
		RandomIt right = left + 1;

		if (*root >= *left && *root >= *right &&
			std::is_heap(first, last, left) && std::is_heap(first, last, right))
			result = true;
	}

	return result;
}

namespace detail
{
	//
	// QuickSort is a recursive algorithm and limited by stack size!!!
	//
	template <class RandomIt>
	void sort_impl(RandomIt first, RandomIt last, quick_sort_tag)
	{
		if (first < last)
		{
			RandomIt it = detail::partition(first, last);

			detail::sort_impl(first, it - 1, quick_sort_tag());
			detail::sort_impl(it + 1, last, quick_sort_tag());
		}
	}

	template <class RandomIt>
	void sort_impl(RandomIt first, RandomIt last, heap_sort_tag)
	{
		std::make_heap(first, last);
		while (--last >= first)
		{
			std::iter_swap(first, last);
			detail::max_heap(first, last, first);
		}
	}

	template <class RandomIt>
	void sort_impl(RandomIt first, RandomIt last, insertion_sort_tag)
	{
		RandomIt i = first;
		while (++i < last)
		{
			typename iterator_traits<RandomIt>::value_type tmp = *i;
			RandomIt j = i - 1;
			while (j >= first && *j > tmp)
			{
				*(j + 1) = *j;
				j--;
			}
			*(j + 1) = tmp;
		}
	}

	template <class RandomIt, class Compare>
	void sort_impl(RandomIt first, RandomIt last, Compare comp, insertion_sort_tag)
	{
		RandomIt i = first;

		while (++i < last)
		{
			typename iterator_traits<RandomIt>::value_type tmp = *i;
			RandomIt j = i - 1;

			while (j >= first && comp(*j, tmp))
			{
				*(j + 1) = *j;
				j--;
			}
			*(j + 1) = tmp;
		}
	}
} // namespace detail

// Sort the range [first, last) using less than operator.
template <class RandomIt>
void sort(RandomIt first, RandomIt last)
{
	// Default sort algorithm is Insertion Sort.
	detail::sort_impl(first, last, insertion_sort_tag());
}

// Sort the range [first, last) using comp.
template <class RandomIt, class Compare>
void sort(RandomIt first, RandomIt last, Compare comp)
{
	// This overload only implemented with Insertion Sort.
	detail::sort_impl(first, last, comp, insertion_sort_tag());
}

#pragma endregion
#pragma region binary_search_operations

template <class ForwardIt, class T>
ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& val)
{
	ForwardIt it;
	typename iterator_traits<ForwardIt>::difference_type count, step;
	count = std::distance(first, last);
	while (count > 0)
	{
		it = first; 
		step = count / 2; 
		std::advance(it, step);
		if (*it < val) 
		{
			first = ++it;
			count -= step + 1;
		}
		else 
			count = step;
	}
	return first;
}

template<class ForwardIt, class T, class Compare>
ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp)
{
	ForwardIt it;
	typename iterator_traits<ForwardIt>::difference_type count, step;
	count = std::distance(first, last);

	while (count > 0) 
	{
		it = first;
		step = count / 2;
		std::advance(it, step);
		if (comp(*it, value)) 
		{
			first = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}
	return first;
}

template <class ForwardIt, class T>
ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& val)
{
	ForwardIt it;
	typename iterator_traits<ForwardIt>::difference_type count, step;
	count = std::distance(first, last);
	while (count > 0)
	{
		it = first; 
		step = count / 2; 
		std::advance(it, step);
		if (!(val < *it))
		{ 
			first = ++it; 
			count -= step + 1;  
		}
		else count = step;
	}
	return first;
}

template<class ForwardIt, class T, class Compare>
ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp)
{
	ForwardIt it;
	typename iterator_traits<ForwardIt>::difference_type count, step;
	count = std::distance(first, last);
	while (count > 0) 
	{
		it = first;
		step = count / 2;
		std::advance(it, step);
		if (!comp(value, *it)) 
		{
			first = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}
	return first;
}

template <class ForwardIt, class T>
bool binary_search(ForwardIt first, ForwardIt last, const T& val)
{
	first = std::lower_bound(first, last, val);
	return (first != last && !(val < *first));
}

template<class ForwardIt, class T, class Compare>
bool binary_search(ForwardIt first, ForwardIt last, const T& value, Compare comp)
{
	first = std::lower_bound(first, last, value, comp);
	return (!(first == last) && !(comp(value, *first)));
}

template <class ForwardIt, class T>
	std::pair<ForwardIt, ForwardIt>
		equal_range(ForwardIt first, ForwardIt last, const T& val)
{
	ForwardIt it = std::lower_bound(first, last, val);
	return std::make_pair(it, std::upper_bound(it, last, val));
}

template<class ForwardIt, class T, class Compare>
	pair<ForwardIt, ForwardIt>
		equal_range(ForwardIt first, ForwardIt last, const T& value, Compare comp)
{
	return std::make_pair(std::lower_bound(first, last, value, comp),
		std::upper_bound(first, last, value, comp));
}

#pragma endregion
#pragma region min/max_operations

template <class T> 
constexpr const T& min(const T& a, const T& b)
{
	return (b < a) ? b : a;
}

template<class T, class Compare>
constexpr const T& min(const T& a, const T& b, Compare comp)
{
	return (comp(b, a)) ? b : a;
}

template <class T> 
constexpr const T& max (const T& a, const T& b)
{
	return (a < b) ? b : a;
}

template<class T, class Compare>
constexpr const T& max(const T& a, const T& b, Compare comp)
{
	return (comp(a, b)) ? b : a;
}

template <class T> 
	std::pair <const T&, const T&> 
		minmax(const T& a, const T& b) 
{
	return (b < a) ? std::make_pair(b, a) : std::make_pair(a, b);
}
	
template<class T, class Compare>
	std::pair<const T&, const T&> 
		minmax(const T& a, const T& b, Compare comp)
{
	return comp(b, a) ? std::make_pair<const T&, const T&>(b, a)
		: std::make_pair<const T&, const T&>(a, b);
}

template <class ForwardIt>
ForwardIt min_element(ForwardIt first, ForwardIt last)
{
	if (first == last) return last;
	ForwardIt smallest = first;

	while (++first != last)
		if (*first < *smallest) 
			smallest = first;
	return smallest;
}

template<class ForwardIt, class Compare>
ForwardIt min_element(ForwardIt first, ForwardIt last, Compare comp)
{
	if (first == last) return last;

	ForwardIt smallest = first;
	++first;
	for (; first != last; ++first) {
		if (comp(*first, *smallest)) {
			smallest = first;
		}
	}
	return smallest;
}

template <class ForwardIt>
ForwardIt max_element(ForwardIt first, ForwardIt last)
{
	if (first == last) return last;
	ForwardIt largest = first;

	while (++first != last)
		if (*largest < *first)
			largest = first;
	return largest;
}

template<class ForwardIt, class Compare>
ForwardIt max_element(ForwardIt first, ForwardIt last, Compare comp)
{
	if (first == last) return last;

	ForwardIt largest = first;
	++first;
	for (; first != last; ++first) {
		if (comp(*largest, *first)) {
			largest = first;
		}
	}
	return largest;
}
#pragma endregion
#pragma region set_operations

template <class InputIt1, class InputIt2>
bool includes(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
	bool result = true;
	for (; first2 != last2; ++first1)
	{
		if (first1 == last1 || *first2 < *first1)
		{
			result = false;
			break;
		}
		if (!(*first1 < *first2))
			++first2;
	}
	return result;
}

template<class InputIt1, class InputIt2, class Compare>
bool includes(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Compare comp)
{
	for (; first2 != last2; ++first1)
	{
		if (first1 == last1 || comp(*first2, *first1))
			return false;
		if (!comp(*first1, *first2))
			++first2;
	}
	return true;
}

template <class InputIt1, class InputIt2, class OutputIt>
OutputIt set_difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt dest)
{
	while (first1 != last1 && first2 != last2)
	{
		if (*first1 < *first2) 
		{ 
			*dest = *first1; 
			++dest; 
			++first1; 
		}
		else if (*first2 < *first1) 
			++first2;
		else 
		{ 
			++first1; 
			++first2; 
		}
	}
	return copy(first1, last1, dest);
}

template<class InputIt1, class InputIt2, class OutputIt, class Compare>
OutputIt set_difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt d_first, Compare comp)
{
	while (first1 != last1) 
	{
		if (first2 == last2) 
			return std::copy(first1, last1, d_first);

		if (comp(*first1, *first2)) 
			*d_first++ = *first1++;

		else 
		{
			if (!comp(*first2, *first1)) 
				++first1;
			++first2;
		}
	}
	return d_first;
}
template <class InputIt1, class InputIt2, class OutputIt>
OutputIt set_intersection(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt dest)
{
	while (first1!=last1 && first2!=last2)
	{
		if (*first1<*first2) 
			++first1;
		else if (*first2<*first1) 
			++first2;
		else 
		{
			*dest = *first1;
			++dest; 
			++first1; 
			++first2;
		}
	}
	return dest;
}

template<class InputIt1, class InputIt2, class OutputIt, class Compare>
OutputIt set_intersection(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,	OutputIt d_first, Compare comp)
{
	while (first1 != last1 && first2 != last2) 
	{
		if (comp(*first1, *first2)) 
			++first1;
		else 
		{
			if (!comp(*first2, *first1)) 
				*d_first++ = *first1++;
			++first2;
		}
	}
	return d_first;
}

template <class InputIt1, class InputI2, class OutputIt>
OutputIt set_union(InputIt1 first1, InputIt1 last1, InputI2 first2, InputI2 last2, OutputIt dest)
{
	while(true)
	{
		if (first1 == last1) return std::copy(first2, last2, dest);
		if (first2 == last2) return std::copy(first1, last1, dest);
		if (*first1 < *first2) 
		{ 
			*dest = *first1; 
			++first1; 
		}
		else if (*first2 < *first1) 
		{ 
			*dest = *first2; 
			++first2; 
		}
		else 
		{ 
			*dest = *first1; 
			++first1; 
			++first2; 
		}
		++dest;
	}
}

template<class InputIt1, class InputIt2, class OutputIt, class Compare>
OutputIt set_union(InputIt1 first1, InputIt1 last1,	InputIt2 first2, InputIt2 last2, OutputIt d_first, Compare comp)
{
	for (; first1 != last1; ++d_first) 
	{
		if (first2 == last2)
			return std::copy(first1, last1, d_first);
		if (comp(*first2, *first1)) 
			*d_first = *first2++;
		else 
		{
			*d_first = *first1;
			if (!comp(*first1, *first2))
				++first2;
			++first1;
		}
	}
	return copy(first2, last2, d_first);
}

template <class InputIt1, class InputIt2, class OutputIt>
OutputIt set_symmetric_difference(
	InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt dest)
{
	while (true)
	{
		if (first1 == last1) return std::copy(first2, last2, dest);
		if (first2 == last2) return std::copy(first1, last1, dest);
		if (*first1 < *first2) 
		{ 
			*dest = *first1; 
			++dest; 
			++first1; 
		}
		else if (*first2 < *first1) 
		{ 
			*dest = *first2; 
			++dest; 
			++first2; 
		}
		else 
		{
			++first1; 
			++first2; 
		}
	}
}

template<class InputIt1, class InputIt2, class OutputIt, class Compare>
OutputIt set_symmetric_difference(
	InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2,	OutputIt d_first, Compare comp)
{
	while (first1 != last1) {

		if (first2 == last2) return std::copy(first1, last1, d_first);

		if (comp(*first1, *first2)) 
			*d_first++ = *first1++;
		else 
		{
			if (comp(*first2, *first1)) 
				*d_first++ = *first2;
			else 
				++first1;
			++first2;
		}
	}
	return std::copy(first2, last2, d_first);
}

#pragma endregion
#pragma region compare_operations

template<class InputIt1, class InputIt2>
bool lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
	bool result = false;
	for(; (first1 != last1) && (first2 != last2); ++first1, (void)++first2)
	{
		if(*first1 < *first2) 
		{
			result = true;
			break;
		}
		if(*first2 < *first1) 
			break;
	}
	result = first1 == last1 ? first2 != last2 : result;
	return result;
}

template<class InputIt1, class InputIt2, class Compare>
bool lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Compare comp)
{
	for (; (first1 != last1) && (first2 != last2); ++first1, (void) ++first2) 
	{
		if (comp(*first1, *first2)) return true;
		if (comp(*first2, *first1)) return false;
	}
	return (first1 == last1) && (first2 != last2);
}

template<class InputIt1, class InputIt2>
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2)
{
	for (; first1 != last1; ++first1, ++first2)
	{
		if (!(*first1 == *first2))
			break;
	}
	return first1 == last1;
}

template<class InputIt1, class InputIt2, class BinaryPredicate>
bool equal(InputIt1 first1, InputIt1 last1,	InputIt2 first2, BinaryPredicate p)
{
	for (; first1 != last1; ++first1, ++first2) 
	{
		if (!p(*first1, *first2)) 
			return false;
	}
	return true;
}

#pragma endregion
#pragma region non-standard_overloads

// Non-standard overload for containers having iterators that are pointers.
// Use at your own risk!
//template<class InputIt, class OutputIt>
//OutputIt* copy(InputIt* first, InputIt* last, OutputIt* dest)
//{
//	while (first != last)
//		*(*dest++) = *(*first++);
//	return dest;
//}

#pragma endregion

} // namespace std

# else // !defined __HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __HAS_NAMESPACES
#endif // !defined ALGORITHM_H__