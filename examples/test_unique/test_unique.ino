#include <pg.h>
#include <utilities/Unique.h>

struct U : public Unique {} u, v;

void setup() 
{
  Serial.begin(9600);
  Serial.print("u="); Serial.println(u.id());
  Serial.print("v="); Serial.println(v.id());
}

void loop() 
{

}
