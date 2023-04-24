// This #include statement was automatically added by the Particle IDE.
// Delete this include and re-add the library in the Particle IDE
#include <Adafruit_IO_Particle.h>

#include <Wire.h>
#include <application.h>
#include "Adafruit_IO_Client.h"
 
#define AIO_KEY "your key here"          // Adafruit IO AIO Key
TCPClient client;                                           // TCP Client used by Adafruit IO library


// Create the AIO client object
Adafruit_IO_Client  AIOClient = Adafruit_IO_Client(client, AIO_KEY);
 
// Create a feed object to send and get data
Adafruit_IO_Feed    soilFeed = AIOClient.getFeed("soil");
Adafruit_IO_Feed    toggleledFeed = AIOClient.getFeed("toggleled");

String toggleLED;

// name the pins
#define SOILPIN A0
int led = D7;

void setup() 
{
    pinMode(led, OUTPUT);
    // Start the Adafruit IO Client
    AIOClient.begin();
    
    // Start a serial port connection
    Serial.begin(9600);
 
}
 
void loop()
{
    int analogValue = analogRead(SOILPIN);       // read  sensor
    int SoilValue = map((analogValue),3569,2331, 0, 100);  // convert output to a percent
    Serial.print("Analog Value: ");
    Serial.println(analogValue);
    Particle.publish("Analog",String (analogValue),60,PRIVATE);
    Particle.publish("Moisture",String (SoilValue),60,PRIVATE);
    
    soilFeed.send(SoilValue);
  
  
    // Grab the latest data from AIO    
    FeedData latest = toggleledFeed.receive();
    toggleLED=String(latest);
    
    // Determine if the data is valid
    if(latest.isValid())
    {
        // It was so print it!
        Serial.println(latest);
        
        //turn on LED or not
         if(toggleLED=="1"){
        digitalWrite(led, HIGH);
        Serial.println("LED On");
                }
                
        if(toggleLED=="0"){
        digitalWrite(led, LOW);
        Serial.println("OFF");
                }
        
             }
    else
    {
        // Whoops, something went wrong!
        Serial.println("error");
    }
    
  
    
    delay(5000);        //time in ms betwee
}