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
#line 14 "/Users/talalagedeon/Desktop/particlePDP/shortproject/src/shortproject.ino"
#define DHTPIN A0

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

ChainableLED leds (RX, TX, 1);


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

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  delay(50000);

  float h = dht.getHumidity();
  float t = dht.getTempCelcius();
  float f = dht.getTempFarenheit();

  if (isnan(h) || isnan(t) || isnan(f)){
    Serial.println("Failed to read from DHT sensor");
    return;
  }


  if (f > 75.0){
leds.setColorRGB(0,255,0,0);
  }

  if (f < 75.0){
    leds.setColorRGB(0,0,0,255);
  }

  Serial.print("tempC");
  Serial.print(t);

  Particle.publish("tempC",String (t));
  Particle.publish("tempF", String (f));
  Particle.publish("humid", String (h));

}