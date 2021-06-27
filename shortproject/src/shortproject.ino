/*
 * Project pdp-shortproject
 * Description: Short Project Professional Development Plan
 * Author:Talal Gedeon
 * Date:
 */

#include <Adafruit_DHT.h>

#include <Grove_ChainableLED.h>

#include <Grove_OLED_128x64.h>

#include <Ubidots.h>

// Defining Temp/Humd pin
#define DHTPIN A0 

// Defining DHT11
#define DHTTYPE DHT11 

// Defining Ubidots webhook name
const char *WEBHOOK_NAME = "Ubidots"; 

// Ubidots constant 
Ubidots ubidots("webhook", UBI_PARTICLE); 

// Temp/&amp;Humi object
DHT dht(DHTPIN, DHTTYPE); 

// LED object with respective pins
ChainableLED leds (RX, TX, 1); 

void updateDisplay (int temp, int humidity, double indoorHeatIndex , double outdoorHeatIndex);

//  global indoor heat index variable
double indoorHeatIndex (float temp, float humidity); 

//  global outdoor heat index variable
double outdoorHeatIndex (float temp, float humidity);

//  global outdoor temp variable
float tempOutdoor = -100; 

//  global outdoor humditiy variable
float humidityOutdoor = -1; 


void setup() {
  Serial.begin(9600);
  Serial.println("Active Wear Station"); 
  
  // initialize DHT library
  dht.begin();

  // initialize LED library
  leds.init();

  Wire.begin();

  // initialize display library
  SeeedOled.init(); 

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

// Check if any indoor data reads have faild
  if (isnan(humidity) || isnan(temp)){
    Serial.println("Failed to read from DHT sensor");
    return;
  }

//  Publishing every 15 min
  const unsigned long publishPeriod = 15 * 60 * 1000;
// Last publish variable 
  static unsigned long lastPublish = 10000 - publishPeriod;

// Check time since last publish occurred and publish collected Webhook Outdoor weather forecast data
  if (millis() - lastPublish >= publishPeriod) {
    lastPublish = millis();
    Particle.publish("GetWeatherForecast", PRIVATE);
  }

// LED light logic based on indoor vs outdoor heat indexes

// Indoor heat index lower than outdoor heat index turn blue
  if (indoorHeatIndex < outdoorHeatIndex){
  leds.setColorRGB(0,255,0,0);
  }
  
// Indoor heat index greater than outdoor heat index turn red
  if (indoorHeatIndex > outdoorHeatIndex){
    leds.setColorRGB(0,0,0,255);
  }
  
// Indoor heat index equals outdoor heat index turn green
  if (indoorHeatIndex == outdoorHeatIndex){
    leds.setColorRGB(0,0,255,0);
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

// Json Parser for data collected from Open Weather API
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

// Clearing Display before updating
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