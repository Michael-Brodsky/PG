/*
 *	This file defines objects from the C++ Standard Template Library (STL)
 *  date and time library.
 *
 *	***************************************************************************
 *
 *	File: chrono.h
 *	Date: August 17, 2021
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
 *	***************************************************************************
 *
 *	Description:
 *
 *		This file defines objects in the <chrono> header of a C++ Standard
 *		Template Library (STL) implementation - to the extent they are
 *		supported by the Arduino implementation. The objects behave according
 *		to the ISO C++11 Standard: (ISO/IEC 14882:2011).
 *
 *	**************************************************************************/

#if !defined CHRONO_H__
# define CHRONO_H__ 20210817L

# include "numeric_limits.h"	// Needed by std::chrono::duration_values.
# include "ratio.h"				// std::ratio and related types.

# if defined __HAS_NAMESPACES

namespace std
{
	namespace chrono
	{
#pragma region forward_declarations
		template<class Rep, class Period = std::ratio<1>>
		class duration;

		template<class Clock, class Duration = typename Clock::duration>
		class time_point;
#pragma endregion

		// Implementation details.
		namespace detail
		{
			// Primary template for std::chrono::details::duration_cast_impl.
			template<class ToDuration, class CF, class CR,
				bool NumIsOne = false, bool DenomIsOne = false>
				struct duration_cast_impl
			{
				template<class Rep, class Period>
				static constexpr ToDuration
					do_cast(const std::chrono::duration<Rep, Period>& d)
				{
					using ToRep = typename ToDuration::rep;
					return ToDuration(static_cast<ToRep>(static_cast<CR>(d.count())
						* static_cast<CR>(CF::num)
						/ static_cast<CR>(CF::den)));
				}
			};

			template<class ToDuration, class CF, class CR>
			struct duration_cast_impl<ToDuration, CF, CR, true, true>
			{
				template<class Rep, class Period>
				static constexpr ToDuration
					do_cast(const std::chrono::duration<Rep, Period>& d)
				{
					using ToRep = typename ToDuration::rep;
					return ToDuration(static_cast<ToRep>(d.count()));
				}
			};

			template<class ToDuration, class CF, class CR>
			struct duration_cast_impl<ToDuration, CF, CR, true, false>
			{
				template<class Rep, class Period>
				static constexpr ToDuration
					do_cast(const std::chrono::duration<Rep, Period>& d)
				{
					using ToRep = typename ToDuration::rep;
					return ToDuration(static_cast<ToRep>(
						static_cast<CR>(d.count()) / static_cast<CR>(CF::den)));
				}
			};

			template<class ToDuration, class CF, class CR>
			struct duration_cast_impl<ToDuration, CF, CR, false, true>
			{
				template<class Rep, class Period>
				static constexpr ToDuration
					do_cast(const std::chrono::duration<Rep, Period>& d)
				{
					using ToRep = typename ToDuration::rep;
					return ToDuration(static_cast<ToRep>(
						static_cast<CR>(d.count()) * static_cast<CR>(CF::num)));
				}
			};

			// Helper type for duration_cast resolution.
			template<class Rep1, class Period1, class Rep2, class Period2>
			struct duration_cast_type
			{
				using type = typename std::common_type<std::chrono::duration<Rep1, Period1>, 
					std::chrono::duration<Rep2, Period2>>::type;
			};
		} // namespace detail

#pragma region std_chrono objects
		// Converts a std::chrono::duration to a duration of different type ToDuration.
		template <class ToDuration, class Rep, class Period>
		constexpr ToDuration duration_cast(const duration<Rep, Period>& d)
		{
			using ToPeriod = typename ToDuration::period;
			using ToRep = typename ToDuration::rep;
			using CF = std::ratio_divide<Period, ToPeriod>;
			using CR = typename std::common_type<ToRep, Rep, std::intmax_t>::type;
			using DC = detail::duration_cast_impl<ToDuration, CF, CR, CF::num == 1, CF::den == 1>;

			return DC::do_cast(d);
		}

		// Converts a std::chrono::time_point from one duration to another.
		template <class ToDuration, class Clock, class Duration>
		std::chrono::time_point<Clock, ToDuration>
			time_point_cast(const std::chrono::time_point<Clock, Duration>& t)
		{
			return std::chrono::time_point<Clock, ToDuration>(std::chrono::duration_cast<ToDuration>(t.time_since_epoch()));
		}

		// The std::chrono::duration_values type defines three common durations.
		template <class Rep>
		struct duration_values
		{
			static constexpr Rep zero() { return Rep(0); }
			static constexpr Rep min() { return std::numeric_limits<Rep>::min(); }
			static constexpr Rep max() { return std::numeric_limits<Rep>::max(); }
		};

		// The std::chrono::treat_as_floating_point trait helps determine if a 
		// duration can be converted to another duration with a different tick period.
		template <class Rep>
		struct treat_as_floating_point : std::is_floating_point<Rep> {};

		// Class template std::chrono::time_point represents a point in time.
		template<class Clock, class Duration>
		class time_point
		{
		public:
			using clock = Clock;
			using duration = Duration;
			using rep = typename duration::rep;
			using period = typename duration::period;

		public:
			constexpr time_point() : d_(duration::zero()) {}
			constexpr explicit time_point(const duration& dur) : d_(dur) {}
			template<class Duration2>
			time_point(const time_point<Clock, Duration2>& t) : d_(t.time_since_epoch()) {}

		public:
			constexpr duration time_since_epoch() const { return d_; }
			time_point& operator+=(const duration& dur) { d_ += dur; return *this; }
			time_point& operator-=(const duration& dur) { d_ -= dur; return *this; }
			static constexpr time_point	min() { return time_point(duration::min()); }
			static constexpr time_point max() { return time_point(duration::max()); }

		private:
			duration d_;
		};

		// Class template std::chrono::duration represents a time interval.
		template<class Rep, class Period>
		class duration
		{
		public:
			using rep = Rep;
			using period = typename Period::type;
			using self_type = duration<rep, period>;

		public:
			constexpr duration() = default;
			duration(const duration&) = default;
			template<class Rep2>
			constexpr explicit duration(const Rep2& r) : r_(r) {}
			template<class Rep2, class Period2>
			constexpr duration(const duration<Rep2, Period2>& d) : r_(std::chrono::duration_cast<duration>(d).count()) {}
			duration& operator=(const duration& other) = default;

		public:
			constexpr rep count() const { return r_; }
			static constexpr duration zero() { return duration(std::chrono::duration_values<rep>::zero()); }
			static constexpr duration min() { return duration(std::chrono::duration_values<rep>::min()); }
			static constexpr duration max() { return duration(std::chrono::duration_values<rep>::max()); }
			constexpr duration operator+() const { return duration(*this); }
			constexpr duration operator-() const { return duration(-r_); }
			duration& operator++() { ++r_; return *this; }
			duration operator++(int) { return duration(r_++); }
			duration& operator--() { --r_; return *this; }
			duration operator--(int) { return duration(r_--); }
			duration& operator+=(const duration& d) { r_ += d.count(); return *this; }
			duration& operator-=(const duration& d) { r_ -= d.count(); return *this; }
			duration& operator*=(const rep& rhs) { r_ *= rhs; return *this; }
			duration& operator/=(const rep& rhs) { r_ /= rhs; return *this; }
			template<class Rep2 = rep>
			typename enable_if<!treat_as_floating_point<Rep2>::value, duration&>::type
				operator%=(const rep& rhs) { r_ %= rhs; return *this; }
			template<class Rep2 = rep>
			typename enable_if<!treat_as_floating_point<Rep2>::value, duration&>::type
				operator%=(const duration& d) { r_ %= d.count(); return *this; }

		private:
			Rep r_;
			Period p_;
		};

#pragma endregion
#pragma region duration_helper_types

		using nanoseconds = std::chrono::duration<std::time_t, std::nano>;
		using microseconds = std::chrono::duration<std::time_t, std::micro>;
		using milliseconds = std::chrono::duration<std::time_t, std::milli>;
		using seconds = std::chrono::duration<std::time_t>;
		using minutes = std::chrono::duration<std::time_t, std::ratio<60>>;
		using hours = std::chrono::duration<std::time_t, std::ratio<3600>>;

#pragma endregion
#pragma region clock_types

		// Class std::chrono::steady_clock represents a monotonic clock.
		struct steady_clock
		{
			using duration = std::chrono::microseconds;
			using rep = duration::rep;
			using period = duration::period;
			using time_point = std::chrono::time_point<steady_clock, duration>;
			static constexpr bool is_steady = true;
			static time_point now() noexcept { return time_point(duration(micros())); }
		};

#pragma endregion
	} // namespace chrono

#pragma region non-member objects

	template <class Rep1, class Period1, class Rep2, class Period2>
	struct common_type<std::chrono::duration<Rep1, Period1>,
		std::chrono::duration<Rep2, Period2>>
	{
		using ToPeriod = std::ratio<std::gcd(Period1::num, Period2::num), 
			std::lcm(Period1::den, Period2::den)>;
		using type = std::chrono::duration<typename std::common_type<Rep1, Rep2>::type,	ToPeriod>;
	};

	template <class Clock, class Duration1, class Duration2>
	struct common_type<std::chrono::time_point<Clock, Duration1>,
		std::chrono::time_point<Clock, Duration2>>
	{
		using type = std::chrono::time_point<Clock, typename std::common_type<Duration1, Duration2>::type>;
	};

	template<class Rep1, class Period1, class Rep2, class Period2>
	typename std::common_type<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>>::type
		constexpr operator+(const std::chrono::duration<Rep1, Period1>& lhs,
			const std::chrono::duration<Rep2, Period2>& rhs)
	{
		using DC = typename std::chrono::detail::duration_cast_type<Rep1, Period1, Rep2, Period2>::type;

		return DC(DC(lhs).count() + DC(rhs).count());
	}

	template< class Rep1, class Period1, class Rep2, class Period2 >
	typename std::common_type<std::chrono::duration<Rep1, Period1>, std::chrono::duration<Rep2, Period2>>::type
		constexpr operator-(const std::chrono::duration<Rep1, Period1>& lhs,
			const std::chrono::duration<Rep2, Period2>& rhs)
	{
		using DC = typename std::chrono::detail::duration_cast_type<Rep1, Period1, Rep2, Period2>::type;

		return DC(DC(lhs).count() - DC(rhs).count());
	}

	template< class Rep1, class Period, class Rep2 >
	chrono::duration<typename std::common_type<Rep1, Rep2>::type, Period>
		constexpr operator*(const std::chrono::duration<Rep1, Period>& d,
			const Rep2& s)
	{
		using DC = typename std::chrono::duration<typename std::common_type<Rep1, Rep2>::type, Period>;

		return DC(DC(d).count() * s);
	}

	template< class Rep1, class Rep2, class Period >
	chrono::duration<typename std::common_type<Rep1, Rep2>::type, Period>
		constexpr operator*(const Rep1& s,
			const std::chrono::duration<Rep2, Period>& d)
	{
		using DC = typename chrono::duration<typename std::common_type<Rep1, Rep2>::type, Period>;

		return DC(DC(d).count() * s);
	}

	template< class Rep1, class Period, class Rep2 >
	chrono::duration<typename std::common_type<Rep1, Rep2>::type, Period>
		constexpr operator/(const std::chrono::duration<Rep1, Period>& d, const Rep2& s)
	{
		using DC = typename std::chrono::duration<typename std::common_type<Rep1, Rep2>::type, Period>;

		return DC(DC(d).count() / s);
	}

	template< class Rep1, class Period1, class Rep2, class Period2 >
	typename std::common_type<Rep1, Rep2>::type
		constexpr operator/(const std::chrono::duration<Rep1, Period1>& lhs,
			const chrono::duration<Rep2, Period2>& rhs)
	{
		using DC = typename std::chrono::detail::duration_cast_type<Rep1, Period1, Rep2, Period2>::type;

		return DC(lhs).count() / DC(rhs).count();
	}

	template< class Rep1, class Period, class Rep2 >
	chrono::duration<typename std::common_type<Rep1, Rep2>::type, Period>
		constexpr operator%(const chrono::duration<Rep1, Period>& d, const Rep2& s)
	{
		using DC = typename std::chrono::duration<typename std::common_type<Rep1, Rep2>::type, Period>;

		return DC(DC(d).count() % s);
	}

	template< class Rep1, class Period1, class Rep2, class Period2 >
	typename std::common_type<std::chrono::duration<Rep1, Period1>, chrono::duration<Rep2, Period2>>::type
		constexpr operator%(const std::chrono::duration<Rep1, Period1>& lhs,
			const std::chrono::duration<Rep2, Period2>& rhs)
	{
		using DC = typename std::chrono::detail::duration_cast_type<Rep1, Period1, Rep2, Period2>::type;

		return DC(DC(lhs).count() % DC(rhs).count());
	}

	template <class Rep1, class Period1, class Rep2, class Period2>
	constexpr bool operator==(const std::chrono::duration<Rep1, Period1>& lhs,
		const std::chrono::duration<Rep2, Period2>& rhs)
	{
		typedef typename std::common_type<std::chrono::duration<Rep1, 
			Period1>, std::chrono::duration<Rep2, Period2>>::type CT;

		return CT(lhs).count() == CT(rhs).count();
	}

	template <class Rep1, class Period1, class Rep2, class Period2>
	constexpr bool operator!=(const std::chrono::duration<Rep1, Period1>& lhs,
		const std::chrono::duration<Rep2, Period2>& rhs)
	{
		return !(lhs == rhs);
	}

	template <class Rep1, class Period1, class Rep2, class Period2>
	constexpr bool operator<(const std::chrono::duration<Rep1, Period1>& lhs,
		const std::chrono::duration<Rep2, Period2>& rhs)
	{
		using CT = typename std::common_type<std::chrono::duration<Rep1, Period1>, 
			std::chrono::duration<Rep2, Period2>>::type;

		return CT(lhs).count() < CT(rhs).count();
	}

	template <class Rep1, class Period1, class Rep2, class Period2>
	constexpr bool operator<=(const std::chrono::duration<Rep1, Period1>& lhs,
		const std::chrono::duration<Rep2, Period2>& rhs)
	{
		return !(rhs < lhs);
	}

	template <class Rep1, class Period1, class Rep2, class Period2>
	constexpr bool operator>(const std::chrono::duration<Rep1, Period1>& lhs,
		const std::chrono::duration<Rep2, Period2>& rhs)
	{
		return rhs < lhs;
	}

	template <class Rep1, class Period1, class Rep2, class Period2>
	constexpr bool operator>=(const std::chrono::duration<Rep1, Period1>& lhs,
		const std::chrono::duration<Rep2, Period2>& rhs)
	{
		return !(lhs < rhs);
	}

	template< class C, class D1, class R2, class P2 >
	std::chrono::time_point<C, typename std::common_type<D1, std::chrono::duration<R2, P2>>::type>
		operator+(const std::chrono::time_point<C, D1>& pt,
			const std::chrono::duration<R2, P2>& d)
	{
		using CT = std::chrono::time_point<C, typename std::common_type<D1, 
			std::chrono::duration<R2, P2>>::type>;

		return CT(pt.time_since_epoch() + d);
	}

	template< class R1, class P1, class C, class D2 >
	std::chrono::time_point<C, typename std::common_type<std::chrono::duration<R1, P1>, D2>::type>
		operator+(const std::chrono::duration<R1, P1>& d,
			const std::chrono::time_point<C, D2>& pt)
	{
		using CT = std::chrono::time_point<C, typename std::common_type<std::chrono::duration<R1, P1>, D2>::type>;

		return CT(pt.time_since_epoch() + d);
	}

	template< class C, class D1, class R2, class P2 >
	std::chrono::time_point<C, typename std::common_type<D1, std::chrono::duration<R2, P2>>::type>
		operator-(const std::chrono::time_point<C, D1>& pt,
			const std::chrono::duration<R2, P2>& d)
	{
		using CT = std::chrono::time_point<C, typename std::common_type<D1, std::chrono::duration<R2, P2>>::type>;

		return CT(pt.time_since_epoch() - d);
	}

	template< class C, class D1, class D2 >
	typename std::common_type<D1, D2>::type
		operator-(const std::chrono::time_point<C, D1>& pt_lhs,
			const std::chrono::time_point<C, D2>& pt_rhs)
	{
		using CT = typename std::common_type<D1, D2>::type;

		return CT(pt_lhs.time_since_epoch()) - CT(pt_rhs.time_since_epoch());
	}

	template< class Clock, class Dur1, class Dur2 >
	bool operator==(const std::chrono::time_point<Clock, Dur1>& lhs,
		const std::chrono::time_point<Clock, Dur2>& rhs)
	{
		using CT = typename std::common_type<Dur1, Dur2>::type;

		return CT(lhs.time_since_epoch()) == CT(rhs.time_since_epoch());
	}

	template< class Clock, class Dur1, class Dur2 >
	bool operator!=(const std::chrono::time_point<Clock, Dur1>& lhs,
		const std::chrono::time_point<Clock, Dur2>& rhs)
	{
		return !(lhs == rhs);
	}

	template< class Clock, class Dur1, class Dur2 >
	bool operator<(const std::chrono::time_point<Clock, Dur1>& lhs,
		const std::chrono::time_point<Clock, Dur2>& rhs)
	{
		using CT = typename std::common_type<Dur1, Dur2>::type;

		return CT(lhs.time_since_epoch()) < CT(rhs.time_since_epoch());
	}

	template< class Clock, class Dur1, class Dur2 >
	bool operator<=(const std::chrono::time_point<Clock, Dur1>& lhs,
		const std::chrono::time_point<Clock, Dur2>& rhs)
	{
		return !(rhs < lhs);
	}

	template< class Clock, class Dur1, class Dur2 >
	bool operator>(const std::chrono::time_point<Clock, Dur1>& lhs,
		const std::chrono::time_point<Clock, Dur2>& rhs)
	{
		return rhs < lhs;
	}

	template< class Clock, class Dur1, class Dur2 >
	bool operator>=(const std::chrono::time_point<Clock, Dur1>& lhs,
		const std::chrono::time_point<Clock, Dur2>& rhs)
	{
		return !(lhs < rhs);
	}

#pragma endregion
} // namespace std

# else // !defined __HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __HAS_NAMESPACES 
#endif // !defined CHRONO_H__
