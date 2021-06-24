/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/talalagedeon/Desktop/particlePDP/shortproject/src/shortproject.ino"
/*
 * Project pdp-shortproject
 * Description: Short Project Professional Development Plan
 * Author:test
 * Date:
 */

#include <Adafruit_DHT.h>

#include <Grove_ChainableLED.h>

#include <Grove_OLED_128x64.h>

#include <Ubidots.h>

void setup();
void loop();
void setCurrentWeather(const char *event, const char *data);
double outdoorHeatIndex (float tempOutdoor, float humidityOutdoor);
#line 16 "/Users/talalagedeon/Desktop/particlePDP/shortproject/src/shortproject.ino"
#define DHTPIN A0 // Defining Temp/Humd pin

#define DHTTYPE DHT11 // Defining DHT11

const char *WEBHOOK_NAME = "Ubidots"; // Defining Ubidots webhook name

Ubidots ubidots("webhook", UBI_PARTICLE); // Ubidots constant 

DHT dht(DHTPIN, DHTTYPE); // Temp/&amp;Humi object

ChainableLED leds (RX, TX, 1); // LED object with respective pins

void updateDisplay (int temp, int humidity, double indoorHeatIndex , double outdoorHeatIndex); 
double indoorHeatIndex (float temp, float humidity); //  global indoor heat index variable
double outdoorHeatIndex (float temp, float humidity);//  global outdoor heat index variable

float tempOutdoor = -100; //  global outdoor temp variable
float humidityOutdoor = -1; //  global outdoor humditiy variable


void setup() {
  Serial.begin(9600);
  Serial.println("Active Wear Station"); 
  
  dht.begin();// initialize DHT library
  leds.init();// initialize LED library

  Wire.begin();
  SeeedOled.init(); // initialize display library

// Clearing display
  SeeedOled.clearDisplay();
  SeeedOled.setNormalDisplay();
  SeeedOled.setPageMode();


// Adding Active Wear Station at setup
  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Active");
  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("Wear");
  SeeedOled.setTextXY(4, 0);
  SeeedOled.putString("Station");

// Subscribing to GetWeatherForecast webhook
  Particle.subscribe(System.deviceID() + "/GetWeatherForecast/", setCurrentWeather, MY_DEVICES);
}

void loop() {
  delay(50000);

// Read Humidity Data
  float humidity = dht.getHumidity();

// Read Temp Data
  float temp = dht.getTempFarenheit();

//  Publishing every 15 min
  const unsigned long publishPeriod = 15 * 60 * 1000;
  static unsigned long lastPublish = 10000 - publishPeriod;

// Check time since last publish occurred and publish collected Outdoor weather forcast data
  if (millis() - lastPublish >= publishPeriod) {
    lastPublish = millis();
    Particle.publish("GetWeatherForecast", PRIVATE);
  }

// Check if any indoor data reads have faild
  if (isnan(humidity) || isnan(temp)){
    Serial.println("Failed to read from DHT sensor");
    return;
  }

// LED logic
  if (indoorHeatIndex > outdoorHeatIndex){
  leds.setColorRGB(0,255,0,0);
  }

  if (indoorHeatIndex < outdoorHeatIndex){
    leds.setColorRGB(0,0,0,255);
  }

// Updating OLED Display
  updateDisplay(temp, humidity, tempOutdoor, humidityOutdoor);

// Ubidots Variables publish into Ubidots dashboard
  ubidots.add("Indoor Temp", temp);
  ubidots.add("Indoor Humidity", humidity);
  ubidots.add("Outdoor Temp", tempOutdoor);
  ubidots.add("Outdoor Humidity", humidityOutdoor);
  ubidots.add("Indoor Heat Index",indoorHeatIndex(temp, humidity));
  ubidots.add("Outdoor Heat Index", outdoorHeatIndex(tempOutdoor, humidityOutdoor));

  bool bufferSent = false;

  bufferSent = ubidots.send(WEBHOOK_NAME, PUBLIC); 

// Particle publish into console 
  Particle.publish("TempF",String (temp));
  Particle.publish("Humid", String (humidity));
  Particle.publish("Indoor Heat Index",String(indoorHeatIndex(temp, humidity)));
  Particle.publish("Outdoor Heat Index",String(outdoorHeatIndex(tempOutdoor, humidityOutdoor)));

}

// Json Parser
void setCurrentWeather(const char *event, const char *data) {
    Log.info("subscriptionHandler %s", data);
    JSONValue outerObj = JSONValue::parseCopy(data);
    JSONObjectIterator iter(outerObj);
    while (iter.next()) {
        if (iter.name() == "temp") {
            tempOutdoor = iter.value().toDouble();
        }
        if (iter.name() == "hum") {
            humidityOutdoor = iter.value().toDouble();
        }
    }}

// Updating OLED Display
void updateDisplay (int temp, int humidity, double indoorHeatIndex , double outdoorHeatIndex)
{
  SeeedOled.clearDisplay(), 
  SeeedOled.setTextXY(1, 0);
  SeeedOled.putString("Indoor Temp: ");
  SeeedOled.putNumber(temp);
  SeeedOled.putString("F");

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Indoor Humd: ");
  SeeedOled.putNumber(humidity);
  SeeedOled.putString("%");

  SeeedOled.setTextXY(4, 0);
  SeeedOled.putString("In Index: ");
  SeeedOled.putNumber(indoorHeatIndex);
  SeeedOled.putString("F");

  SeeedOled.setTextXY(5, 0);
  SeeedOled.putString("Out Index: ");
  SeeedOled.putNumber(outdoorHeatIndex);
  SeeedOled.putString("F");

}

// Indoor heat index equation
double indoorHeatIndex (float temp, float humidity) {
    const double c1 = -42.379;
    const double c2 = 2.04901523;
    const double c3 = 10.14333127;
    const double c4 = -.22475541;
    const double c5 = -0.00683783;
    const double c6 = -0.05481717;
    const double c7 = 0.00122874;
    const double c8 = 0.00085282;
    const double c9 = -0.00000199;

    double heatIndex = c1 + (c2 * temp) +
                           (c3 * humidity) +
                           (c4 * temp*humidity) + 
                           (c5 * (temp*temp)) +
                           (c6 * (humidity * humidity)) +
                           (c7 * (temp * temp) * humidity) + 
                           (c8 * temp * (humidity * humidity)) +
                           (c9 * (temp * temp) * (humidity * humidity));

  return heatIndex;
}   

// Outdoor heat index equation
double outdoorHeatIndex (float tempOutdoor, float humidityOutdoor) {
    const double c1 = -42.379;
    const double c2 = 2.04901523;
    const double c3 = 10.14333127;
    const double c4 = -.22475541;
    const double c5 = -0.00683783;
    const double c6 = -0.05481717;
    const double c7 = 0.00122874;
    const double c8 = 0.00085282;
    const double c9 = -0.00000199;

    double outHeatIndex = c1 + (c2 * tempOutdoor) +
                           (c3 * humidityOutdoor) +
                           (c4 * tempOutdoor*humidityOutdoor) + 
                           (c5 * (tempOutdoor*tempOutdoor)) +
                           (c6 * (humidityOutdoor * humidityOutdoor)) +
                           (c7 * (tempOutdoor * tempOutdoor) * humidityOutdoor) + 
                           (c8 * tempOutdoor * (humidityOutdoor * humidityOutdoor)) +
                           (c9 * (tempOutdoor * tempOutdoor) * (humidityOutdoor * humidityOutdoor));

  return outHeatIndex;
}   