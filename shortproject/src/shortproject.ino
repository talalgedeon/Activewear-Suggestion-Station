/*
 * Project pdp-shortproject
 * Description: Short Project Professional Development Plan
 * Author:test
 * Date:
 */

#include <Adafruit_DHT.h>

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
  delay(10000);

  float h = dht.getHumidity();
  float t = dht.getTempCelcius();
  float f = dht.getTempFarenheit();

  if (isnan(h) || isnan(t) || isnan(f)){
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  Serial.print("tempC");
  Serial.print(t);

  Particle.publish("tempC",String (t));
  Particle.publish("tempF", String (f));
  Particle.publish("humid", String (h));

}