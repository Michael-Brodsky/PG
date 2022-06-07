/*
 *	This file defines data types and performance traits of servo motors.
 *
 *	***************************************************************************
 *
 *	File: servos.h
 *	Date: October 26, 2021
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
 *	Description:
 * 
 *	This file defines types to control servo motors using natural units, i.e 
 *	degrees of rotation and angular velocity (deg/sec) instead of pulse 
 *	widths. It also defines performance traits of several common servo motors: 
 * 
 *		MinControlAngle, 
 *		MaxControlAngle, 
 *		MinPulseWidth, 
 *		MaxPulseWidth, 
 *		LowRotationSpeed, 
 *		HighRotationSpeed (see primary servo_traits template below for 
 *	                       descriptions).
 * 
 *	Performance traits were obtained from public available literature, which 
 *	may not be accurate. Users are encouraged to verify the data through 
 *	independent sources.
 * 
 *	**************************************************************************/

#if !defined __PG_SERVOS_H
# define __PG_SERVOS_H 20211026L

# include <chrono>
# include <lib/fmath.h>

# undef degrees

using namespace std::chrono;

namespace pg
{
	namespace servos
	{
		using degrees = float;

		// Servo typedefs template.
		template<class AngleT = degrees, class StepT = microseconds, class DurationT = milliseconds>
		struct servo_types
		{
			using angle_type = AngleT;			// Type that holds a rotation angle in degrees.
			using step_type = StepT;			// Type that holds a rotation postion as a pulse width duration.
			using duration_type = DurationT;	// Type that holds time intervals.
			// Fractional type that holds rotation angles per unit time, i.e. angular velocity.
			struct speed_type
			{
				using TimeType = typename std::remove_reference<duration_type>::type;
				using AngleType = typename std::remove_reference<angle_type>::type;

				constexpr speed_type() = default;
				constexpr speed_type(const duration_type& t, const angle_type& a) : interval(t), angle(a) {}
				// C++11 move ctor workaround (std::forward & std::move not constexpr in C++11).
				constexpr speed_type(TimeType&& t, AngleType&& a) :
					interval(static_cast<duration_type&&>(t)), angle(static_cast<angle_type&&>(a)) {}
				speed_type& operator=(const speed_type& other) = default;
				bool operator==(const speed_type& other) const { return other.interval == interval && other.angle == angle; }
				bool operator!=(const speed_type& other) const { return !(other == *this); }
				template<class T>
				speed_type operator+(const T& v) { return speed_type(interval, angle + v); }
				template<class T>
				speed_type operator-(const T& v) { return speed_type(interval, angle - v); }
				template<class T>
				speed_type operator*(const T& v) { return speed_type(interval, angle * v); }
				template<class T>
				speed_type operator/(const T& v) { return speed_type(interval, angle / v); }
				template<class T>
				speed_type& operator+=(const T& v) { angle += v; return *this; }
				template<class T>
				speed_type& operator-=(const T& v) { angle -= v; return *this; }
				template<class T>
				speed_type& operator*=(const T& v) { angle *= v; return *this; }
				template<class T>
				speed_type& operator/=(const T& v) { angle /= v; return *this; }

				duration_type	interval;	// time interval.
				angle_type		angle;		// rotation angle.
			};
		};

		// Primary servo_traits template.
		template<class T>
		struct servo_traits
		{
			using type = servo_traits<T>;

			// Absolute minimum rotation angle in degrees.
			static constexpr typename T::angle_type	MinControlAngle = T::MinControlAngle;
			// Absolute maximum rotation angle in degrees.
			static constexpr typename T::angle_type	MaxControlAngle = T::MaxControlAngle;
			// Absolute minimum control pulse width.
			static constexpr typename T::step_type	MinPulseWidth = T::MinPulseWidth;
			// Absolute maximum control pulse width.
			static constexpr typename T::step_type	MaxPulseWidth = T::MaxPulseWidth;
			// Maximum low voltage rotation speed in time/degrees.
			static constexpr typename T::speed_type	LowRotationSpeed = T::LowRotationSpeed;
			// Maximum high voltage rotation speed in time/degrees.
			static constexpr typename T::speed_type	HighRotationSpeed = T::HighRotationSpeed;
		};
 
		// Converts a rotation angle to a pulse width for a given servo.
		template<class T>
		constexpr typename T::step_type steps(typename T::angle_type angle)
		{
			return typename T::step_type(norm(angle, T::MinControlAngle, T::MaxControlAngle, 
				T::MinPulseWidth.count(), T::MaxPulseWidth.count()));
		}

		// Converts a pulse width to a rotation angle for a given servo.
		template<class T>
		constexpr typename T::angle_type angle(typename T::step_type step)
		{
			return typename T::angle_type(norm(step.count(), T::MinPulseWidth.count(), T::MaxPulseWidth.count(),
				T::MinControlAngle, T::MaxControlAngle));
		}

#pragma region Specialized Servo Traits Templates

		struct hiwonder_ld20mg {};
		struct hiwonder_ld27mg {};
		struct hiwonder_ld220mg {};
		struct hiwonder_ld260mg {};
		struct hiwonder_ldx218 {};
		struct hiwonder_lfd01m {};
		struct hitech_d85mg {};
		struct hitech_d85mg_rep {};
		struct hitech_d89mw {};
		struct hitech_d89mw_rep {};
		struct hitech_hs40 {};
		struct hitech_hs45hb {};
		struct hitech_hs53 {};
		struct hitech_hs55 {};
		struct hitech_hs65hb {};
		struct hitech_hs65mg {};
		struct hitech_hs70mg {};
		struct hitech_hs82mg {};
		struct hitech_hs85mg {};
		struct hitech_hs81 {};
		struct hitech_hs85bb {};
		struct hitech_hs311 {};
		struct hitech_hs318 {};
		struct hitech_hs322hd {};
		struct hitech_hs422 {};
		struct hitech_hs425bb {};
		struct hitech_hs430bh {};
		struct hitech_hs485hb {};
		struct hitech_hs488hb {};
		struct hitech_hs625mg {};
		struct hitech_hs645mg {};
		struct hitech_hs646wp {};
		struct hitech_hs5055mg {};
		struct hitech_hs5055mg_rep {};
		struct hitech_hs5065mg {};
		struct hitech_hs5065mg_rep {};
		struct hitech_hs5070mh {};
		struct hitech_hs5070mh_rep {};
		struct hitech_hs5086wp {};
		struct hitech_hs5086wp_rep {};
		struct hitech_hs5087mh {};
		struct hitech_hs5087mh_rep {};
		struct hitech_hs5496mh {};
		struct hitech_hs5496mh_rep {};
		struct hitech_hs5565mh {};
		struct hitech_hs5565mh_rep {};
		struct hitech_hs5585mh {};
		struct hitech_hs5585mh_rep {};
		struct hitech_hs7950th {};
		struct hitech_hs7950th_rep {};
		struct multicomp_pro {};
		struct towerpro_mg995r {};
		struct towerpro_mg996 {};
		struct towerpro_sg90 {};
		struct towerpro_sg92r {};
		struct towerpro_sg5010 {};

		template<>
		struct servo_traits<hiwonder_ld20mg> : public servo_types<>
		{
			using type = servo_traits<hiwonder_ld20mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(180.0);
			static constexpr step_type	MinPulseWidth = microseconds(544);
			static constexpr step_type	MaxPulseWidth = microseconds(2580);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(250), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(200), degrees(60.0));
		};

		template<>
		struct servo_traits<hiwonder_ld220mg> : public servo_traits<hiwonder_ld20mg> 
		{
			using type = servo_traits<hiwonder_ld220mg>;
		};

		template<>
		struct servo_traits<hiwonder_ld27mg> : public servo_types<>
		{
			using type = servo_traits<hiwonder_ld27mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(270.0);
			static constexpr step_type	MinPulseWidth = microseconds(500);
			static constexpr step_type	MaxPulseWidth = microseconds(2500);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(250), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(200), degrees(60.0));
		};

		template<>
		struct servo_traits<hiwonder_ld260mg> : public servo_types<>
		{
			using type = servo_traits<hiwonder_ld260mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(180.0);
			static constexpr step_type	MinPulseWidth = microseconds(500);
			static constexpr step_type	MaxPulseWidth = microseconds(2500);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(130), degrees(60.0));
		};

		template<>
		struct servo_traits<hiwonder_ldx218> : public servo_types<>
		{
			using type = servo_traits<hiwonder_ldx218>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(180.0);
			static constexpr step_type	MinPulseWidth = microseconds(500);
			static constexpr step_type	MaxPulseWidth = microseconds(2500);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(210), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(160), degrees(60.0));
		};

		template<>
		struct servo_traits<hiwonder_lfd01m> : public servo_types<>
		{
			using type = servo_traits<hiwonder_lfd01m>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(180.0);
			static constexpr step_type	MinPulseWidth = microseconds(500);
			static constexpr step_type	MaxPulseWidth = microseconds(2500);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(100), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs422> : public servo_types<>
		{
			using type = servo_traits<hitech_hs422>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(195.0);
			static constexpr step_type	MinPulseWidth = microseconds(500);
			static constexpr step_type	MaxPulseWidth = microseconds(2500);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(210), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(160), degrees(60.0));
		};

		template<>
		struct servo_traits<towerpro_sg90> : public servo_types<>
		{
			using type = servo_traits<towerpro_sg90>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(180.0);
			static constexpr step_type	MinPulseWidth = microseconds(500);
			static constexpr step_type	MaxPulseWidth = microseconds(2400);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
		};

		template<>
		struct servo_traits<multicomp_pro> : public servo_types<>
		{
			using type = servo_traits<multicomp_pro>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(180.0);
			static constexpr step_type	MinPulseWidth = microseconds(500);
			static constexpr step_type	MaxPulseWidth = microseconds(2500);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(100), degrees(60.0));
		};

		template<>
		struct servo_traits<towerpro_mg996> : public servo_types<>
		{
			using type = servo_traits<towerpro_mg996>;

			static constexpr angle_type	MinControlAngle = degrees(-90.0);
			static constexpr angle_type	MaxControlAngle = degrees(90.0);
			static constexpr step_type	MinPulseWidth = microseconds(1000);
			static constexpr step_type	MaxPulseWidth = microseconds(2000);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs488hb> : public servo_types<>
		{
			using type = servo_traits<hitech_hs488hb>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(190.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2425);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(220), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(180), degrees(60.0));
		};

		template<>
		struct servo_traits<towerpro_mg995r> : public servo_types<>
		{
			using type = servo_traits<towerpro_mg995r>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(120.0);
			static constexpr step_type	MinPulseWidth = microseconds(1500);
			static constexpr step_type	MaxPulseWidth = microseconds(2500);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(200), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(160), degrees(60.0));
		};

		template<>
		struct servo_traits<towerpro_sg92r> : public servo_types<>
		{
			using type = servo_traits<towerpro_sg92r>;

			static constexpr angle_type	MinControlAngle = degrees(-90.0);
			static constexpr angle_type	MaxControlAngle = degrees(90.0);
			static constexpr step_type	MinPulseWidth = microseconds(1000);
			static constexpr step_type	MaxPulseWidth = microseconds(2000);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(100), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(100), degrees(60.0));
		};

		template<>
			struct servo_traits<towerpro_sg5010> : public servo_types<>
		{
			using type = servo_traits<towerpro_sg5010>;

			static constexpr angle_type	MinControlAngle = degrees(-90.0);
			static constexpr angle_type	MaxControlAngle = degrees(90.0);
			static constexpr step_type	MinPulseWidth = microseconds(1000);
			static constexpr step_type	MaxPulseWidth = microseconds(2000);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(200), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(160), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs40> : public servo_types<>
		{
			using type = servo_traits<hitech_hs40>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(195.0);
			static constexpr step_type	MinPulseWidth = microseconds(615);
			static constexpr step_type	MaxPulseWidth = microseconds(2495);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(100), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs53> : public servo_types<>
		{
			using type = servo_traits<hitech_hs53>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(180.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2270);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs55> : public servo_types<>
		{
			using type = servo_traits<hitech_hs55>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(203.0);
			static constexpr step_type	MinPulseWidth = microseconds(615);
			static constexpr step_type	MaxPulseWidth = microseconds(2390);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(180), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs65mg> : public servo_types<>
		{
			using type = servo_traits<hitech_hs65mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(189.0);
			static constexpr step_type	MinPulseWidth = microseconds(610);
			static constexpr step_type	MaxPulseWidth = microseconds(2360);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs45hb> : public servo_types<>
		{
			using type = servo_traits<hitech_hs45hb>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(191.0);
			static constexpr step_type	MinPulseWidth = microseconds(790);
			static constexpr step_type	MaxPulseWidth = microseconds(2405);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs65hb> : public servo_types<>
		{
			using type = servo_traits<hitech_hs65hb>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(189.0);
			static constexpr step_type	MinPulseWidth = microseconds(610);
			static constexpr step_type	MaxPulseWidth = microseconds(2360);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5055mg> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5055mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(126.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(200), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5055mg_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5055mg_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(178.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(200), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs81> : public servo_types<>
		{
			using type = servo_traits<hitech_hs81>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(165.0);
			static constexpr step_type	MinPulseWidth = microseconds(640);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(90), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs85bb> : public servo_types<>
		{
			using type = servo_traits<hitech_hs85bb>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(182.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2300);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(160), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs70mg> : public servo_types<>
		{
			using type = servo_traits<hitech_hs70mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(194.0);
			static constexpr step_type	MinPulseWidth = microseconds(650);
			static constexpr step_type	MaxPulseWidth = microseconds(2370);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs82mg> : public servo_types<>
		{
			using type = servo_traits<hitech_hs82mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(165.0);
			static constexpr step_type	MinPulseWidth = microseconds(600);
			static constexpr step_type	MaxPulseWidth = microseconds(2200);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(100), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs85mg> : public servo_types<>
		{
			using type = servo_traits<hitech_hs85mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(182.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2300);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(160), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5065mg> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5065mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(128.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5065mg_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5065mg_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(181.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5070mh> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5070mh>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(125.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5070mh_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5070mh_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(174.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(120), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5086wp> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5086wp>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(155.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(180), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(150), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5086wp_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5086wp_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(181.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(180), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(150), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5087mh> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5087mh>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(133.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(130), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5087mh_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5087mh_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(177.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(130), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_d89mw> : public servo_types<>
		{
			using type = servo_traits<hitech_d89mw>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(145.0);
			static constexpr step_type	MinPulseWidth = microseconds(850);
			static constexpr step_type	MaxPulseWidth = microseconds(2350);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_d89mw_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_d89mw_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(185.0);
			static constexpr step_type	MinPulseWidth = microseconds(850);
			static constexpr step_type	MaxPulseWidth = microseconds(2350);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_d85mg> : public servo_types<>
		{
			using type = servo_traits<hitech_d85mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(145.0);
			static constexpr step_type	MinPulseWidth = microseconds(850);
			static constexpr step_type	MaxPulseWidth = microseconds(2350);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(130), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_d85mg_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_d85mg_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(185.0);
			static constexpr step_type	MinPulseWidth = microseconds(850);
			static constexpr step_type	MaxPulseWidth = microseconds(2350);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs311> : public servo_types<>
		{
			using type = servo_traits<hitech_hs311>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(202.0);
			static constexpr step_type	MinPulseWidth = microseconds(575);
			static constexpr step_type	MaxPulseWidth = microseconds(2460);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(190), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(150), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs318> : public servo_types<>
		{
			using type = servo_traits<hitech_hs318>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(210.0);
			static constexpr step_type	MinPulseWidth = microseconds(437);
			static constexpr step_type	MaxPulseWidth = microseconds(2637);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(190), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(150), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs7950th> : public servo_types<>
		{
			using type = servo_traits<hitech_hs7950th>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(120.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(180), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(130), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs7950th_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs7950th_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(198.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(180), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(130), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs425bb> : public servo_types<>
		{
			using type = servo_traits<hitech_hs425bb>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(188.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2520);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(210), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(160), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs430bh> : public servo_types<>
		{
			using type = servo_traits<hitech_hs430bh>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(189.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2520);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(160), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs625mg> : public servo_types<>
		{
			using type = servo_traits<hitech_hs625mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(197.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2520);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(180), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(150), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs645mg> : public servo_types<>
		{
			using type = servo_traits<hitech_hs645mg>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(197.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2520);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(240), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(200), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs646wp> : public servo_types<>
		{
			using type = servo_traits<hitech_hs646wp>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(193.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2520);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(200), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs322hd> : public servo_types<>
		{
			using type = servo_traits<hitech_hs322hd>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(201.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2450);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(190), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(150), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs485hb> : public servo_types<>
		{
			using type = servo_traits<hitech_hs485hb>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(190.0);
			static constexpr step_type	MinPulseWidth = microseconds(553);
			static constexpr step_type	MaxPulseWidth = microseconds(2425);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(220), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(180), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5496mh> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5496mh>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(117.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(150), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5496mh_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5496mh_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(200.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(150), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5565mh> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5565mh>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(119.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(90), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5565mh_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5585mh_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(200.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(110), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(90), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5585mh> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5585mh>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(118.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
		};

		template<>
		struct servo_traits<hitech_hs5585mh_rep> : public servo_types<>
		{
			using type = servo_traits<hitech_hs5565mh_rep>;

			static constexpr angle_type	MinControlAngle = degrees(0.0);
			static constexpr angle_type	MaxControlAngle = degrees(200.0);
			static constexpr step_type	MinPulseWidth = microseconds(750);
			static constexpr step_type	MaxPulseWidth = microseconds(2250);
			static constexpr speed_type	LowRotationSpeed = speed_type(milliseconds(170), degrees(60.0));
			static constexpr speed_type	HighRotationSpeed = speed_type(milliseconds(140), degrees(60.0));
		};

#pragma endregion

	} // namespace servos

} // namespace pg

#endif // !defined __PG_SERVOS_H