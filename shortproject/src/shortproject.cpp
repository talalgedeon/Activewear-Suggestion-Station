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

void setup();
void loop();
#line 10 "/Users/talalagedeon/Desktop/particlePDP/shortproject/src/shortproject.ino"
#define DHTPIN A0

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 test");
  
  dht.begin();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  delay(2000);

  float h = dht.getHumidity();
  float t = dht.getTempCelcius();
  float f = dht.getTempFarenheit();

  if (isnan(h) || isnan(t) || isnan(f)){
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  Serial.print('Humid: ', h);
  Serial.print(h);

}