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

void setup();
void loop();
void setCurrentWeather(const char *event, const char *data);
void updateDisplay (int temp, int humidity, int tempOutdoor , int humidityOutdoor);
#line 14 "/Users/talalagedeon/Desktop/particlePDP/shortproject/src/shortproject.ino"
#define DHTPIN A0

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

ChainableLED leds (RX, TX, 1);

void updateDisplay(int temp, int humidity);

float tempOutdoor = -100;
float humidityOutdoor = -1; 


void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 test");
  
  dht.begin();
  leds.init();

  Wire.begin();
  SeeedOled.init();

  SeeedOled.clearDisplay();
  SeeedOled.setNormalDisplay();
  SeeedOled.setPageMode();

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Particle");
  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("Weather");
  SeeedOled.setTextXY(4, 0);
  SeeedOled.putString("Station");

  Particle.subscribe(System.deviceID() + "/GetWeatherForecast/", setCurrentWeather, MY_DEVICES);
}

void loop() {
  delay(50000);

  float humidity = dht.getHumidity();
  float temp = dht.getTempFarenheit();
  const unsigned long publishPeriod = 15 * 60 * 1000;
  static unsigned long lastPublish = 10000 - publishPeriod;

  if (millis() - lastPublish >= publishPeriod) {
    lastPublish = millis();
    Particle.publish("GetWeatherForecast", PRIVATE);
  }

  if (isnan(humidity) || isnan(temp)){
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  if (temp > tempOutdoor){
  leds.setColorRGB(0,255,0,0);
  }

  if (temp < tempOutdoor){
    leds.setColorRGB(0,0,0,255);
  }

  updateDisplay(temp, humidity, tempOutdoor, humidityOutdoor);

  Particle.publish("tempF",String (temp));
  Particle.publish("humid", String (humidity));


}

void setCurrentWeather(const char *event, const char *data) {
    Log.info("subscriptionHandler %s", data);
    JSONValue outerObj = JSONValue::parseCopy(data);
    JSONObjectIterator iter(outerObj);
    while (iter.next()) {
        if (iter.name() == "temp") {
            tempOutdoor = iter.value().toDouble();
        }
        if (iter.name() == "humidity") {
            humidityOutdoor = iter.value().toDouble();
        }
    }}


void updateDisplay (int temp, int humidity, int tempOutdoor , int humidityOutdoor)
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
  SeeedOled.putString("Otdoor Temp: ");
  SeeedOled.putNumber(tempOutdoor);
  SeeedOled.putString("F");

  SeeedOled.setTextXY(5, 0);
  SeeedOled.putString("Otdoor Humd: ");
  SeeedOled.putNumber(humidityOutdoor);
  SeeedOled.putString("%");

}