/*Notes

4/23/23- Didnt quite finish adding the LED toggle
*/
//Delete this 4 include statements and manually reassociate the libraries the way you did before

// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

// This #include statement was automatically added by the Particle IDE.
#include <SparkFun_Qwiic_Humidity_AHT20.h>

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_IO_Particle.h>

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_VEML7700.h>




/*Add these in;;;
// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_IO_Particle.h>
// This #include statement was automatically added by the Particle IDE.
#include <SparkFun_Qwiic_Humidity_AHT20.h>

*/

#include <Wire.h>
#include <application.h>
#include "Adafruit_IO_Client.h"

//light setup
Adafruit_VEML7700 veml;
 
//AIO presetup
#define AIO_KEY "aio_lQtD234aq6CzfrGhD2erl0VKWvnN"          // Adafruit IO AIO Key
TCPClient client;                                           // TCP Client used by Adafruit IO library


 //Initiate AHT
 AHT20 humiditySensor;

// Create the AIO client object
Adafruit_IO_Client  AIOClient = Adafruit_IO_Client(client, AIO_KEY);
 
// Create a feed object to send and get data
Adafruit_IO_Feed    tempFeed = AIOClient.getFeed("temperature");
Adafruit_IO_Feed    humFeed = AIOClient.getFeed("humidity");
Adafruit_IO_Feed    toggleFeed = AIOClient.getFeed("toggle");
Adafruit_IO_Feed    lightFeed = AIOClient.getFeed("light");
Adafruit_IO_Feed    colorFeed = AIOClient.getFeed("color");
Adafruit_IO_Feed    soilFeed = AIOClient.getFeed("soil");
Adafruit_IO_Feed    servoFeed = AIOClient.getFeed("servo");
Adafruit_IO_Feed    toggleledFeed = AIOClient.getFeed("toggleled");


 int randNumber;
 int fan = D3;
 int i=0;               //loop for LEDs
 String toggle;
 String servoval;
 uint32_t color;
 int CURRENT_COLOR;
 
 //Neopixel setup
 // IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D5
#define PIXEL_COUNT 12
#define PIXEL_TYPE WS2812       //2811 is fine for ours, but if you use a different brand you need to look this up in the library file
#define BRIGHTNESS 30           // 0 - 255 (note, these are super bright, so lower is fine for the bench)
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

uint32_t Wheel(byte WheelPos);

//breathe variables
int numLoops1 = 5;
int MinBrightness = 20;       //value 0-255
int MaxBrightness = 100;      //value 0-255
int fadeInWait = 30;          //lighting up speed, steps.
int fadeOutWait = 50;         //dimming speed, steps.

#define SOILPIN A0


//setup servo
Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created

int pos = 0;    // variable to store the servo position

 
 
void setup() 
{
    //set fan pin to output
    pinMode(fan, OUTPUT);
    // Start the Adafruit IO Client
    AIOClient.begin();
    
    // Start a serial port connection
    Serial.begin(9600);
    
    Wire.begin(); //Join I2C bus

  //Check if the AHT20 will acknowledge
  if (humiditySensor.begin() == false)
  {
    Serial.println("AHT20 not detected. Please check wiring. Freezing.");
    while (1);
  }
  
  //VEML7700 Setup
  
  Serial.println("Adafruit VEML7700 Test");

  if (!veml.begin())
  {
    Serial.println("Sensor not found");
    while (1)
      ;
  }
  Serial.println("Sensor found");

  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_800MS);

  Serial.print(F("Gain: "));
  switch (veml.getGain())
  {
  case VEML7700_GAIN_1:
    Serial.println("1");
    break;
  case VEML7700_GAIN_2:
    Serial.println("2");
    break;
  case VEML7700_GAIN_1_4:
    Serial.println("1/4");
    break;
  case VEML7700_GAIN_1_8:
    Serial.println("1/8");
    break;
  }

  Serial.print(F("Integration Time (ms): "));
  switch (veml.getIntegrationTime())
  {
  case VEML7700_IT_25MS:
    Serial.println("25");
    break;
  case VEML7700_IT_50MS:
    Serial.println("50");
    break;
  case VEML7700_IT_100MS:
    Serial.println("100");
    break;
  case VEML7700_IT_200MS:
    Serial.println("200");
    break;
  case VEML7700_IT_400MS:
    Serial.println("400");
    break;
  case VEML7700_IT_800MS:
    Serial.println("800");
    break;
  }

  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  veml.setLowThreshold(10000);
  veml.setHighThreshold(20000);
  veml.interruptEnable(true);
  
  //NEO Setup
  strip.setBrightness(BRIGHTNESS);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    
    
    //servo
     myservo.attach(D6);   // attach the servo on the D0 pin to the servo object
     
  
}
 
void loop()
{

//Get moisture

int soil_moisture = analogRead(SOILPIN);
soilFeed.send(String(soil_moisture));


// Set Neo to solid color
/*
    for(i=0; i<strip.numPixels(); i++) 
    strip.setPixelColor(i,0x00FF0);   //strip.setPixelColor(i, 0xFF0000);
    strip.show();
    */
 
// Temp Sensor
    if (humiditySensor.available() == true)
  {
    //Get the new temperature and humidity value
    float temperature = humiditySensor.getTemperature();
    float humidity = humiditySensor.getHumidity();

    //Print the results
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" C\t");
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.print("% RH");

    Serial.println();
    
    tempFeed.send(String(temperature,1));
    humFeed.send(String(humidity,1));
  }
  
  //Light Sensor
  Serial.print("Lux: ");
  Serial.println(veml.readLux());
  float light=(veml.readLux());
  lightFeed.send(String(light,1));
  
  
  /* Random Sensor Sim-- Comment out if have real data
  
     randNumber = random(15, 25);
    // Send the number 20 to AIO then wait for 2 seconds
    testFeed.send(String(randNumber));
    delay(2000);
 */
 
 
    // Grab the latest data from AIO    
    FeedData latest = toggleFeed.receive();
    toggle=String(latest);
    
    // Determine if the data is valid
    if(latest.isValid())
    {
        // It was so print it!
        Serial.println(latest);
        
        //turn on LED or not
         if(toggle=="1"){
        digitalWrite(fan, HIGH);
        Serial.println("LED On");
                }
                
        if(toggle=="0"){
        digitalWrite(fan, LOW);
        Serial.println("OFF");
                }
        
         /*else
            {
            digitalWrite(led, LOW);
            Serial.println("error");
            }
            */
    }
    else
    {
        // Whoops, something went wrong!
        Serial.println("error");
    }
    
    //Servo tst
    
     // Grab the latest data from AIO    
    FeedData servolatest = servoFeed.receive();
    servoval=String(servolatest);
    
    // Determine if the data is valid
    if(servolatest.isValid())
    {
        // It was so print it!
        Serial.println(latest);
        
        //turn on LED or not
         if(servoval=="1"){
        myservo.write(90); 
        Serial.println("LED On");
                }
                
        if(servoval=="0"){
        myservo.write(00); 
        Serial.println("OFF");
                }
        
         /*else
            {
            digitalWrite(led, LOW);
            Serial.println("error");
            }
            */
    }
    else
    {
        // Whoops, something went wrong!
        Serial.println("error");
    }
    
    
    
  
  //color change WIP
  
  
  //this compiles
    FeedData colorlatest = colorFeed.receive();
    Serial.print("color: ");
    Serial.println(colorlatest);
    
    String hexstring = String (colorlatest);

    // Get rid of '#' and convert it to integer
    int number = (int) strtol( &hexstring[1], NULL, 16);

    // Split them up into r, g, b values
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;

    Serial.print("hexval: ");
    Serial.println(number);
    Serial.print("red: ");
    Serial.println(r);
    Serial.print("Green: ");
    Serial.println(g);
    Serial.print("Blue: ");
    Serial.println(b);
    
    for(i=0; i<strip.numPixels(); i++) 
    strip.setPixelColor(i,r,g,b);   //strip.setPixelColor(i, 0xFF0000);
    strip.show();
    
    /*
    strip.setPixelColor(0, 0,0,255);
    strip.setPixelColor(1, r,g,b);
    strip.show();
    */

 
 
  //new try
  //int a = colorFeed.receive();
  
  //int varName;
//FeedData colorlatest.intValue(&varName);

  //FeedData latest = toggleFeed.receive();
   // toggle=String(latest);
    
  //FeedData colorlatest = colorFeed.toInt();
  //FeedData colorlatest = colorFeed.receive.toInt();
  
  /*
  long colornum=colorlatest.tolnt();
  for(i=0; i<strip.numPixels(); i++) 
 strip.setPixelColor(1, color);
 strip.show();
 */
 
 //this compiles
// FeedData colorlatest = colorFeed.receive();
// Serial.print("color: ");
 //Serial.println(colorlatest);
 
  
  //color=String(colorlatest);
   //Serial.println(color);
   
   
  // for(i=0; i<strip.numPixels(); i++) 
 //strip.setPixelColor(1, colorlatest);
 //strip.show();
 
  //color=String(colorlatest);
  //int colornum=colorval.toInt();
  //Serial.println(colornum);
  
 /* 
  Adafruit_IO_Feed    toggleFeed = AIOClient.getFeed("toggle");
Adafruit_IO_Feed    lightFeed = AIOClient.getFeed("light");
Adafruit_IO_Feed    colorFeed = AIOClient.getFeed("color");


FeedData latest = toggleFeed.receive();
    toggle=String(latest);
    
    */
    
  //int colornum=colorval.toInt();
  //Serial.print("colornum: ");
  //Serial.println(colornum);
  /*  
  
  //color_feed = IO.get_feed(COLOR_FEED)
       // color_data = IO.receive_data(color_feed["key"])
        CURRENT_COLOR = int(colorval["value"][1:], 16)
        Serial.println(CURRENT_COLOR);
        
  /*
  for(i=0; i<strip.numPixels(); i++) 
 strip.setPixelColor(i, color);
 strip.show();
 */
 
    
    delay(10000);
}

//subs


void rgbBreathe(uint32_t c, uint8_t x, uint8_t y) {
  for (int j = 0; j < x; j++) {
    for (uint8_t b = MinBrightness; b < MaxBrightness; b++) {
      strip.setBrightness(b * 255 / 255);
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
      }
      strip.show();
      delay(fadeInWait);
    }
    strip.setBrightness(MaxBrightness * 255 / 255);
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(y);
    }
    for (uint8_t b = MaxBrightness; b > MinBrightness; b--) {
      strip.setBrightness(b * 255 / 255);
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
      }
      strip.show();
      delay(fadeOutWait);
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


/*full rainbow

 strip.setPixelColor(0, 0,0,255);
    strip.setPixelColor(1, 85,0,255);
    strip.setPixelColor(2, 127,0,180);
    strip.setPixelColor(3, 170,0,90);
    strip.setPixelColor(4, 255,0,0);
    strip.setPixelColor(5, 255,43,0);
    strip.setPixelColor(6, 213,85,0);
    strip.setPixelColor(7, 170,128,0);
    strip.setPixelColor(8, 128,255,0);
    strip.setPixelColor(9, 85,255,0);
    strip.setPixelColor(10, 30,255,0);
    strip.setPixelColor(11,0,255,0);
    
    */
    

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
