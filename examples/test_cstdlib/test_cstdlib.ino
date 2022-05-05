#include <pg.h>
#include <cstring>
//#include <components/Jack.h>

char s1[] = "0,1,2,3", *p = s1;
const char* d = ",";
const char* s2 = "Try not. Do, or do not. There is no try.";
const char* target = "not";
const char* result = s2;
const char* s3 = "Hey Ho Let's Go!";

void setup() 
{
	char* tok = std::strtok(s1, d);

  Serial.begin(9600);
	Serial.println();
	while (tok)
	{
		Serial.println(tok);
		tok = std::strtok(nullptr, d);
	}
	while ((result = std::strstr(result, target)) != nullptr) {
		Serial.print("Found '"); Serial.print(target);
		Serial.print("' starting at '"); Serial.println(result);

		// Increment result, otherwise we'll find target at the same location
		++result;
	}
	Serial.print("strlen(Hey Ho Let's Go!)="); Serial.println(std::strlen(s3));
}

void loop() 
{
  
}
