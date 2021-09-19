#pragma once

#if defined ARDUINO 
# include "Arduino.h"
# define clock_api micros
#endif

#include "ctime"

namespace std
{
	namespace chrono
	{
		static std::time_t steady_clock_api() { return clock_api(); }
	}
}
