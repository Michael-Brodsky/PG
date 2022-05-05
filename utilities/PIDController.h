/*
 *	This file defines a proportional–integral–derivative (PID) controller 
 *	class.
 *
 *	***************************************************************************
 *
 *	File: PIDController.h
 *	Date: September 26, 2021
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
 *	The `PIDController' class encapsulates the behaviors of a control loop 
 *	mechanism used in industrial automation applications. An overview of PID 
 *	controllers can be viewed here: https://en.wikipedia.org/wiki/PID_controller
 *	The PIDController generates a control or output value from a process 
 *	variable according to a PID algorithm. The process variable is supplied by 
 *	the client, from a sensor or other device and the generated output value 
 *	is used by the client to control the machinery, such as a motor, heater, 
 *	valve or similar device. The algorithm is tuned by three parameters: 
 *	the proportional, integral and derivative coefficients, which must all be
 *	non-negative values and a fourth parameter which sets the overall system 
 *	gain. Proper operation of a PID controller requires careful tuning and 
 *	experimentation to find ideal parameter values which can be affected by 
 *	numerous variables.
 * 
 *	The `PIDController' class is parameterized by its member methods and can
 *	be operated synchronously with the loop() method or asynchronously 
 *	with the clock() method. Loop timing is critical as it determines the  
 *	algorithm derivative/integration time and resulting output value. Ideally, 
 *	clients should keep the loop time constant. Prior to operation, clients 
 *	must call the start() method, passing in the current system time. 
 * 
 *****************************************************************************/

#if !defined __PG_PIDCONTROLLER_H 
# define __PG_PIDCONTROLLER_H 20210926L 

# include "chrono"
# include "utility"
# include "interfaces/iclockable.h"

# if defined __PG_HAS_NAMESPACES

namespace pg
{
	// A proportional–integral–derivative (PID) controller.
	template<class T = float>
	class PIDController : public iclockable 
	{
	public:
		using value_type = T;
		using duration = std::chrono::duration<value_type, std::ratio<1>>;
		using clock_type = std::chrono::steady_clock;
		using time_point = std::chrono::time_point<clock_type, duration>;
		using input_type = typename callback<value_type>::type;
		using output_type = typename callback<void, void, value_type>::type;

	public:
		PIDController() : 
			measured_value_(), set_point_(), previous_error_(), integral_(), 
			Kp_(), Ki_(), Kd_(), gain_(), output_value_(), dt_(), tp_(), 
			in_(), out_() 
		{}
		PIDController(value_type set_point, value_type Kp, value_type Ki, value_type Kd, 
			value_type gain, input_type in = nullptr, output_type out = nullptr) : 
			measured_value_(), set_point_(set_point), previous_error_(), integral_(),
			Kp_(Kp), Ki_(Ki), Kd_(Kd), gain_(gain), output_value_(), dt_(), tp_(),
			in_(in), out_(out)
		{}

	public:
		void set_point(value_type value) { integral_ = 0; previous_error_ = 0; set_point_ = value; }
		const value_type set_point() const { return set_point_; }
		const value_type measured_value() const { return measured_value_; }
		const value_type output() const { return output_value_; }
		void proportional(value_type value) { Kp_ = value; }
		const value_type proportional() const { return Kp_; }
		void integral(value_type value) { Ki_ = value; }
		const value_type integral() const { return Ki_; }
		void derivative(value_type value) { Kd_ = value; }
		const value_type derivative() const { return Kd_; }
		void gain(value_type value) { gain_ = value; }
		const value_type gain() const { return gain_; } 
		void start(time_point value) { tp_ = value; }
		const duration dt() const { return dt_; }
		void input(input_type in) { in_ = in; }
		void output(output_type out) { out_ = out; }

		value_type loop(value_type measured_value)
		{
			// CV = A(Kp*e + Ki*int(0,t) e(t)dt + Kd*de/dt), 
			// where e = SP - PV, SP = process set point, PV = process variable.

			time_point now = time_point(clock_type::now());
			dt_ = duration(now - tp_);
			value_type error = set_point_ - measured_value;
			value_type proportional = error;
			value_type derivative = (error - previous_error_) / dt_.count();
			integral_ += (error * dt_.count());
			value_type control_value = Kp_ * proportional + Ki_ * integral_ + Kd_ * derivative;
			measured_value_ = measured_value;
			previous_error_ = error;
			tp_ = now;

			return (output_value_ = control_value * gain_);
		}

	private:
		void clock() override 
		{ 
			(*out_)(loop((*in_)()));
		}

	private:
		value_type measured_value_; // Current measured process value (PV).
		value_type set_point_;		// Current process set point (SP).
		value_type previous_error_;	// Current calculated process error (e).
		value_type integral_;		// Current value of integration (int(0,t) e(t)dt).
		value_type Kp_;				// Current proportional coefficient.
		value_type Ki_;				// Current integral coefficient.
		value_type Kd_;				// Current derivative coefficient.
		value_type gain_;			// Current output gain (A).
		value_type output_value_;	// Current output value (CV).
		duration dt_;				// Current integration/derivative time (dt).
		time_point tp_;				// Last loop time.
		input_type in_;				// Process value read method.
		output_type out_;			// Control value write method.
	};

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif // !defined __PG_PIDCONTROLLER_H 