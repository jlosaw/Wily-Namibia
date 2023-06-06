// name the pins
#define SOILPIN A0
int led = D7;

// This routine runs only once upon reset
void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(9600);           // This will output the data to computer through USB if you care to look at it there
}

// This routine loops forever
void loop()
{
 int analogValue = analogRead(SOILPIN);       // read  sensor
 
 // convert output to a percent 
 // 1. first purple number: sensor reading at 0 moisture
 // 2. second purple number:sensor reading at 100% moisture
 
 Serial.print("Analog Value: ");
 int SoilValue = map((analogValue),3569,2331, 0, 100);  
 
 Serial.println(analogValue);           // send to serial port if you want
 
 //Send data to the cloud to view it in Particle platform
 Particle.publish("Analog",String (analogValue),60,PRIVATE);
 Particle.publish("Moisture",String (SoilValue),60,PRIVATE);
 
 // Logic area. This is where we use an if statement turn on the LED on the Argon if moisture gets too low
 if (SoilValue<50){     //can change this to any desired threshold from 1 to 99
 
     digitalWrite(led, HIGH);       //turn LED on
 }
 
 else{
 digitalWrite(led, LOW);        //turn LED off if moisture is good
 }
 
 delay(2000);        // two second refresh time. going too fast will overwhelm Particle cloud
}
