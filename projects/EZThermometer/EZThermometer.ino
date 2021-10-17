// This sketch shows how to create a digital thermometer that reads the temperature 
// from a thermistor and displays it on an LCD using just a handful of lines of code. 
// 
// The Pg `LCDDisplay' type is used to assign a display Field to a display Screen 
// and refresh the entire display device with one line of code. Fields define the 
// column, row, label, print formatting and visibility of some value of interest that 
// the program prints to the lcd device. Screens group collections of Fields together 
// and define a screen label. The program can easily switch between screens, changing 
// all displayed values at once with one line of code.
// 
// The hardware requirements are that a thermistor configured as part of a voltage 
// divider with another resistor R be used as the temperature sensor. The center tap 
// of the divider is connected to an analog input pin, and that voltage is used to 
// compute the temperature. The constants defined below are for a standard 10K 
// thermistor, with R ~ 10K and the center tap fed through a common-collector 
// current amplifier with a measured base-emitter voltage drop of 0.6 volts. If the  
// current amplifier is omitted, set Vbe = 0. The maths used to compute temperatures 
// are standard, of-the-shelf algorithms.

#include <pg.h>						// Pg library.
#include <LiquidCrystal.h>			// Arduino LCD api.
#include <lib/thermo.h>				// Thermometer math functions.
#include <components/LCDDisplay.h>	// Defines a class that manages a display device.
#include <components/AnalogInput.h>	// Defines a class that reads from analog inputs.

using namespace pg;

using value_type = float;	// Float type for analog input values (temperature, voltage, etc.)
using adc_type = analog_t;	// Int type for A/D converter output values.
using TemperatureSensor = AnalogInput<adc_type>;	// Type alias for the temperature sensor.
using Display = LCDDisplay<16, 2>;	// Type alias for a 16x2 character display.

// Function to convert an analog voltage from the 
// thermistor into a temperature in degrees Kelvin.
value_type readTemp(adc_type);


//////////////////////////////////////////////////////////////
// 
// Important Safety Tip!
//
// The following values define circuit hardware parameters 
// and MUST be accurate. Incorrect values will give 
// erroneous temperature readings. Use only MEASURED values 
// from the actual circuit and thermistor coefficients from 
// the manufacturer.
//
////////////////////////////////////////////////////////////////

const value_type a = 1.125e-3, b = 2.347e-4, c = 8.566e-8;	// Steinhart-Hart thermistor eqn coeffs.
const value_type R = 10030.0, Vss = 4.97, Vbe = 0.6;		// Thermistor voltage divider circuit params.

TemperatureSensor sensor(A7);	// Connect the temp sensor to an analog input pin.

// Temperature display field {col,row,label,format,visible}
Display::Field temp_field{ 0,1,"Temp:","%6.1f",true };
// Temperature display screen {&fields, ...}, label
Display::Screen temp_screen( {&temp_field},"EZTHERMOMETER" );
// Connect the lcd hardware to appropriate pins, 
// and assign it to the display manager.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Display display(&lcd);

void setup() 
{
	lcd.begin(Display::cols(), Display::rows());
	display.screen(&temp_screen);	// Set the active display screen.
	display.clear();
}

void loop() 
{
	value_type temp = readTemp(sensor());

	display.refresh(temperature<units::fahrenheit>(temp));
	// Delay is EVIL!! Do not use.
	delay(1000);
}

value_type readTemp(adc_type aout)
{
	return tsense(aout, AnalogMax<board_type>(), R, Vss, Vbe, a, b, c);
}
