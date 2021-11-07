#include <pg.h>
#include <cmath>
#include <components/SweepServo.h>
#include <utilities/TaskScheduler.h>

/* 
 *  This sketch demonstrates asynchronous control of a servo motor using 
 *	the SweepServo and TaskScheduler classes. SweepServo is a template 
 *	that takes one parameter: the servo motor's performance traits. This 
 *	allows the servo to be controlled using natural units (degrees and 
 *	degrees/second) instead of pulse widths, which have to be converted 
 *	into rotation angles and angular velocities by the user and usually 
 *	wind up melting their brain. Servo traits for about 100 motors are 
 *	defined in <lib/servos.h>. If the one you're using isn't listed, then 
 *	add it (follow the layout of the primary servo_traits template).
 */

using namespace pg;
using namespace pg::servos;
using namespace std::chrono;

// Short descriptive type aliases.

// Servo type is Hiwonder LD20MG (change if using different motor). 
// See <lib/servos.h> for supported motors or add your own.
using RotaryActuator = SweepServo<hiwonder_ld20mg>; 
using servo_speed = RotaryActuator::speed_type;
using servo_angle = RotaryActuator::angle_type;
using servo_state = RotaryActuator::State;
using Scheduler = TaskScheduler<>;
using ScheduledTask = Scheduler::Task;
using task_state = ScheduledTask::State;

// Servo callback function.
void servoCallback(servo_state);

const pin_t ServoOut = 3; // The servo pwm output pin, change if yours differs.
const servo_angle min_angle = RotaryActuator::MinControlAngle();
const servo_angle max_angle = RotaryActuator::MaxControlAngle();
const long angle_min = std::lrint(min_angle);	// min_angle rounded to whole degrees.  
const long angle_max = std::lrint(max_angle);	// max_angle rounded to whole degrees.
const milliseconds clk_rate = milliseconds(RotaryActuator::LowRotationSpeed().interval.count() / 4); // Servo clock rate.
const seconds demo_time = seconds(10);	// Time alloted to run the demo.

RotaryActuator ra;	// The servo being controlled.
ClockCommand clk_cmd(&ra);	// Command that executes the servo's clock() method. 
ScheduledTask task{ clk_rate, &clk_cmd, task_state::Active }; // Task that schedules servo clocking.
Scheduler sched({ &task });	// The task scheduler.
Timer<milliseconds> tmr(demo_time);	// Demo timer.

void setup() 
{
	ra.attach(ServoOut);				// Attach the servo to the output pin.
	ra.speed(ra.LowRotationSpeed());	// Set the rotaion speed (using the low voltage speed).
	ra.callback(servoCallback);			// Set the callback (gets called after every rotation).
	ra.initialize();					// Initialize the servo.
	delay(500);							// Wait a bit after initializing before starting the demo.
	sched.start();						// Start the task scheduler ...
	tmr.start();						// ... and the demo timer.
	ra.sweep(ra.MaxControlAngle());		// Command the servo to rotate to its max rotation angle.
}

void loop() 
{
	sched.tick();
}

void servoCallback(servo_state state)
{
	if (tmr.expired())
		task.state(task_state::Idle);	// Quit the demo if alloted time has expired.
	// When each rotation completes, toggle the rotation angle between the min and max angles.
	else if (state == servo_state::Idle)
	{
		const long sweep = std::lround(ra.sweep()); // Using rounded values to cmp floats.

		ra.sweep(sweep == angle_min ? max_angle : min_angle);
		delay(500); // Wait a bit between each rotation.
	}
}
