//#include "application.h"

// name the pins
#define SOILPIN A0

// This routine runs only once upon reset
void setup()
{
  Serial.begin(9600);           // This will output the data to computer through USB
}

// This routine loops forever
void loop()
{
 int analogValue = analogRead(SOILPIN);       // read  sensor
 Serial.print("Analog Value: ");
 Serial.println(analogValue);
 Particle.publish("Moisture",String (analogValue),60,PRIVATE);
// Particle.variable("analogvalue", String(analogValue), INT);
 
 delay(5000);
}
