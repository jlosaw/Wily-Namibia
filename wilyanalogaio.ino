// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_IO_Particle.h>

#include <Wire.h>
#include <application.h>
#include "Adafruit_IO_Client.h"
 
#define AIO_KEY "yourkeyhere"          // Adafruit IO AIO Key
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
int tled = D6;

#define enA 2
#define in1 3
#define in2 4

void setup() 
{
    pinMode(led, OUTPUT);
    pinMode(tled, OUTPUT);
    
    pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
 
  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
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
        digitalWrite(tled, HIGH);
        analogWrite(enA, 200); // Send PWM signal to L298N Enable pin

        Serial.println("LED On");
        delay (5000);
        analogWrite(enA, 00);
        
                }
                
        if(toggleLED=="0"){
        digitalWrite(tled, LOW);
        analogWrite(enA, 00);
        Serial.println("OFF");
                }
        
             }
    else
    {
        // Whoops, something went wrong!
        Serial.println("error");
    }
    
    // Logic area. This is where we use an if statement turn on the LED on the Argon if moisture gets too low
     if (SoilValue<50){     //can change this to any desired threshold from 1 to 99
 
     digitalWrite(led, HIGH);       //turn LED on
     }
     
     else{
     digitalWrite(led, LOW);        //turn LED off if moisture is good
     }
  
    
    delay(5000);
}
