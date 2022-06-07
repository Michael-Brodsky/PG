/*
 *	This file defines an asynchronous servo controller class.
 *
 *	***************************************************************************
 *
 *	File: SweepServo.h
 *	Date: November 4, 2021
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
 *	**************************************************************************/


#if !defined __PG_SWEEPSERVO_H 
# define __PG_SWEEPSERVO_H 20211104L

# include <Servo.h>					// Arduino servo api.
# include <system/types.h>			// Pg typedefs.
# include <lib/callback.h>			// Callback signature templates.
# include <lib/servos.h>			// Servo types and traits.
# include <interfaces/iclockable.h>	// `iclockable' interface.
# include <interfaces/icomponent.h>	// `icomponent' interface.
# include <utilities/Timer.h>		// Interval timer.

using namespace std::chrono;

# if defined __PG_HAS_NAMESPACES 

namespace pg
{
	namespace servos
	{
		// Asynchronous servo controller.
		template<class T>
		class SweepServo : public iclockable, public icomponent
		{
		public:
			enum class State
			{
				Idle = 0,
				Active
			};
			using servo_type = typename servo_traits<T>::type;
			using angle_type = typename servo_type::angle_type;
			using step_type = typename servo_type::step_type;
			using speed_type = typename servo_type::speed_type;
			using duration_type = typename servo_type::duration_type;
			using timer_type = Timer<duration_type>;
			using apistep_type = int;
			using callback_type = typename callback<void, void, State>::type;

			static constexpr angle_type MinControlAngle() { return servo_type::MinControlAngle; }
			static constexpr angle_type MaxControlAngle() { return servo_type::MaxControlAngle; }
			static constexpr step_type	MinPulseWidth() { return servo_type::MinPulseWidth; }
			static constexpr step_type	MaxPulseWidth() { return servo_type::MaxPulseWidth; }
			static constexpr speed_type	LowRotationSpeed() { return servo_type::LowRotationSpeed; }
			static constexpr speed_type	HighRotationSpeed() { return servo_type::HighRotationSpeed; }

			const speed_type InitSpeed = speed_type(milliseconds(50), degrees(18.0));

		public:
			// Constructs an uninitialized SweepServo.
			SweepServo();

		public:
			// Attaches the servo to a pwm output pin.
			pin_t attach(pin_t);
			// Returns the currently attached output pin or InvalidPin if not attached.
			pin_t attach() const;
			// Initializes the servo to a specified position.
			void initialize(const angle_type & = MinControlAngle());
			// Commands the servo to rotate to a specified angle at an optional speed.
			void sweep(const angle_type&, const speed_type& = speed_type());
			// Returns the servo's current rotation angle.
			const angle_type& sweep() const;
			// Sets the servo's rotation speed.
			void speed(const speed_type&);
			// Returns the servo's current rotation speed.
			const speed_type& speed() const;
			// Returns the servo's current state.
			State state() const;
			// Sets the client callback.
			void callback(callback_type);

		private:
			void changeState(State);
			void auto_rotate();
			void man_rotate();
			void clock() override;

		private:
			mutable Servo	servo_;		// Arduino Servo api.
			pin_t			pin_;		// The currently attached pwm output pin.
			State			state_;		// The current servo state.
			bool			init_;		// Flag indicating initialization state.
			timer_type		timer_;		// Step interval timer.
			speed_type		speed_;		// The current rotation speed.
			speed_type		cmd_speed_;	// The commanded rotation speed.
			angle_type		angle_;		// The current rotation angle.
			angle_type		cmd_angle_;	// The commanded rotation angle.
			callback_type	callback_;	// Client callback.
		};

		template<class T>
		SweepServo<T>::SweepServo() :
			servo_(), pin_(InvalidPin), state_(), init_(), timer_(),
			speed_(LowRotationSpeed()), cmd_speed_(speed_),
			angle_(MinControlAngle()), cmd_angle_(angle_), callback_()
		{
			timer_.start();
		}

		template<class T>
		pin_t SweepServo<T>::attach(pin_t pin)
		{
			servo_.attach(pin, MinPulseWidth().count(), MaxPulseWidth().count());

			return (pin_ = servo_.attached() ? pin : InvalidPin);
		}

		template<class T>
		pin_t SweepServo<T>::attach() const
		{
			return pin_;
		}

		template<class T>
		void SweepServo<T>::initialize(const angle_type& angle)
		{
			uint32_t wait = static_cast<uint32_t>((MaxControlAngle() - MinControlAngle()) / 
				LowRotationSpeed().angle * LowRotationSpeed().interval.count());

			init_ = true;
			servo_.writeMicroseconds(static_cast<apistep_type>(MaxPulseWidth().count()));
			angle_ = MaxControlAngle();
			delay(wait);
			sweep(MinControlAngle(), InitSpeed);
			man_rotate();
			sweep(angle, InitSpeed);
			man_rotate();
			angle_ = angle;
			init_ = false;
			if (callback_)
				(*callback_)(state_);
		};

		template<class T>
		void SweepServo<T>::sweep(const angle_type& angle, const speed_type& spd)
		{
			cmd_speed_ = spd.angle != degrees(0) ? spd : speed_;
			cmd_angle_ = angle;
		}

		template<class T>
		const typename SweepServo<T>::angle_type& SweepServo<T>::sweep() const
		{
			return angle_;
		}

		template<class T>
		void SweepServo<T>::speed(const speed_type& spd)
		{
			speed_ = cmd_speed_ = spd;
		}

		template<class T>
		const typename SweepServo<T>::speed_type& SweepServo<T>::speed() const
		{
			return state_ == State::Active ? cmd_speed_ : speed_;
		}

		template<class T>
		typename SweepServo<T>::State SweepServo<T>::state() const
		{
			return state_;
		}

		template<class T>
		void SweepServo<T>::callback(callback_type cb)
		{
			callback_ = cb;
		}

		template<class T>
		void SweepServo<T>::changeState(State state)
		{
			if (state_ != state)
			{
				if (!init_ && callback_)
					(*callback_)(state);
				state_ = state;
			}
		}

		template<class T>
		void SweepServo<T>::auto_rotate()
		{
			step_type pos = step_type(servo_.readMicroseconds());
			const step_type cmd_pos = servos::steps<servo_type>(cmd_angle_);
			const step_type delta_p = cmd_pos > pos ? cmd_pos - pos : pos - cmd_pos;

			if (delta_p.count() != 0)
			{
				const angle_type delta_a = std::min(cmd_speed_.angle,
					cmd_speed_.angle * timer_.elapsed().count() / cmd_speed_.interval.count());
				const step_type delta_s = std::min(delta_p, servos::steps<servo_type>(delta_a) - MinPulseWidth());

				pos += pos < cmd_pos ? delta_s : -delta_s;
				servo_.writeMicroseconds(static_cast<apistep_type>(pos.count()));
				changeState(State::Active);
				angle_ = servos::angle<servo_type>(step_type(servo_.readMicroseconds()));
			}
			else
				changeState(State::Idle);

		}

		template<class T>
		void SweepServo<T>::man_rotate()
		{
			do
			{
				delay(cmd_speed_.interval.count());
				clock();
			} while (state_ == State::Active);
		}

		template<class T>
		void SweepServo<T>::clock()
		{
			auto_rotate();
			timer_.reset();
		}

	} // nsamespace servos
} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_SWEEPSERVO_H 
