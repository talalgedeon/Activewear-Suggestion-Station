/*
 * Project pdp-shortproject
 * Description: Short Project Professional Development Plan
 * Author:test
 * Date:
 */

#include <Adafruit_DHT.h>

#include <Grove_ChainableLED.h>

#include <Grove_OLED_128x64.h>

#define DHTPIN A0

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

ChainableLED leds (RX, TX, 1);

void updateDisplay(int temp, int humidity);


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

  Particle.subscribe("hook-response/CurrentWeather", myHandler, MY_DEVICES);
}

void myHandler(const char *event, const char *data) {
  // Handle the integration response
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  delay(50000);

  String data = String(10);

  float humidity = dht.getHumidity();
  float temp = dht.getTempCelcius();
  float f = dht.getTempFarenheit();

  if (isnan(humidity) || isnan(temp) || isnan(f)){
    Serial.println("Failed to read from DHT sensor");
    return;
  }
  if (f > 75.0){
leds.setColorRGB(0,255,0,0);
  }

  if (f < 75.0){
    leds.setColorRGB(0,0,0,255);
  }

  // Serial.print("tempC");
  // Serial.print(temp);

  updateDisplay(temp, humidity);

  Particle.publish("tempC",String (temp));
  Particle.publish("tempF", String (f));
  Particle.publish("humid", String (humidity));

  // Trigger the integration
  Particle.publish("CurrentWeather", data, PRIVATE);
}

void updateDisplay (int temp, int humidity)
{
  SeeedOled.clearDisplay(), 
  SeeedOled.setTextXY(1, 0);
  SeeedOled.putString("Indoor Temp: ");
  SeeedOled.putNumber(temp);
  SeeedOled.putString("C");

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Indoor Humd: ");
  SeeedOled.putNumber(humidity);
  SeeedOled.putString("%");
}