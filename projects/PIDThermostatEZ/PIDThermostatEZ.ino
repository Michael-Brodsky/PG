// This sketch shows how to create a simple thermostat that reads the temperature 
// from a thermistor, runs it through the PID algorithm and generates a pulse-width 
// modulated (PWM) output signal that can control machinery such as a heater. The 
// working part of the program is just a handful of lines, the rest being type 
// definitions and value initializers. It shows how a fairly complicated program 
// can be constructed easily with Pg Library objects.

// The hardware requirements are that a thermistor configured as part of a voltage 
// divider with another resistor R be used as the temperature sensor. The center tap 
// of the divider is connected to one of the analog input pins and that voltage is 
// used to compute the temperature. The constants defined below are for a standard 
// 10K thermistor, with R ~ 10K and the center tap fed through a common-collector 
// current amplifier with a measured base-emitter voltage drop of 0.6 volts. The 
// maths used to compute the temperature are standard, of-the-shelf algorithms.

#include <pg.h>
#include <lib/thermometer.h>			// Thermometer math functions.
#include <components/AnalogInput.h>		// Defines a class that reads from analog inputs.
#include <utilities/PWMOutput.h>		// Defines a class that generates PWM outputs.
#include <utilities/PIDController.h>	// Defines a class that computes the PID algorithm.

using namespace pg;
using namespace std::chrono;

using value_type = float;	// Float type for analog input values (temperature, voltage, etc.)
using adc_type = analog_t;	// Int type for A/D converter output values.
using TemperatureSensor = AnalogInput<adc_type>;	// Type alias for the temperature sensor.
using ThermostatOutput = PWMOutput<value_type>;		// Type alias for the thermostat output.
using PidController = PIDController<value_type>;	// Type alias for the PID controller.

// Function to convert an analog voltage from a 
// thermistor into a temperature in degrees Kelvin.
value_type readTemp(adc_type);

const value_type a = 1.125e-3, b = 2.347e-4, c = 8.566e-8;	// Steinhart-Hart thermistor eqn. coeffs for 10K.
const value_type R = 10030.0, Vss = 4.97, Vbe = 0.6;		// Thermistor voltage divider circuit params.
value_type Kp = 0.33, Ki = 0.005, Kd = 0.0, Av = 0.05;		// PID coefficients, p, i, d and overall gain.
value_type Sp = 80.0;										// Default thermostat set point in degrees F.

TemperatureSensor sensor(A7);		// Connect the temp sensor to an analog input pin.
ThermostatOutput output(46);		// Connect the thermostat out to a pwm output pin.
PidController pid(Sp,Kp,Ki,Kd,Av);	// Initialize the pid controller. 

void setup() 
{
	pid.start(steady_clock::now());	// PID needs to know what time it is.
	output.enabled(true);			// Enable the thermostat pwm output.
}

void loop() 
{
	// Get the current temp converted to Fahrenheit.
	value_type Tsense = temperature<units::fahrenheit>(readTemp(sensor()));
	// Run it through the pid, getting back a control value.
	value_type Cv = pid.loop(Tsense);
	// Compute the output duty cycle from the control value.
	value_type Dc = clamp(Cv, 0.0f, 1.0f);
	// Send it to the thermostat output.
	output.duty_cycle(Dc);
	// Delay is EVIL! Dont' use it.
	delay(1000);
}

value_type readTemp(adc_type aout)
{
	return tsense(aout, AnalogMax<board_type>(), R, Vss, Vbe, a, b, c);
}
