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

const unsigned long PUBLISH_PERIOD_MS1 = 60000;
const unsigned long FIRST_PUBLISH_MS1 = 5000;
const char *PUBLISH_EVENT_NAME = "activeSuggestion";

unsigned long lastPublish1 = FIRST_PUBLISH_MS1 - PUBLISH_PERIOD_MS1;

// Ubidots constant 
Ubidots ubidots("webhook", UBI_PARTICLE); 

// Temp/&amp;Humi object
DHT dht(DHTPIN, DHTTYPE); 

// LED object with respective pins
ChainableLED leds (RX, TX, 1);

void updateDisplay (double inside, double outside);

//  global outdoor temp variable
float tempOutdoor = -100; 

//  global outdoor humditiy variable
float humidityOutdoor = -1; 

// Read Humidity Data
float humidity = dht.getHumidity();

// Read Temp Data
float temp = dht.getTempFarenheit();


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
  SeeedOled.putString("Suggestion");
  SeeedOled.setTextXY(5, 0);
  SeeedOled.putString("Station");

// Subscribing to GetWeatherForecast webhook
  Particle.subscribe(System.deviceID() + "/GetWeatherForecast/", setCurrentWeather, MY_DEVICES);
}

void loop() {
  delay(50000);

double inside = indoorHeatIndex(temp, humidity);

double outside = outdoorHeatIndex (tempOutdoor, humidityOutdoor);

// Check if any indoor data reads have failed
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

	if (millis() - lastPublish1 >= PUBLISH_PERIOD_MS1) {
		lastPublish1 = millis();
		publishData();
  }
  
// Indoor heat index lower than outdoor heat index turn blue
  if (inside < outside){
  leds.setColorRGB(0,0,0,255);
  }
  
// Indoor heat index greater than outdoor heat index turn red
  if (inside > outside){
    leds.setColorRGB(0,255,0,0);
  }

// Updating OLED Display
 updateDisplay (inside, outside);

// Ubidots Variables publish into Ubidots dashboard
  ubidots.add("Indoor Temp", temp);
  ubidots.add("Indoor Humidity", humidity);
  ubidots.add("Outdoor Temp", tempOutdoor);
  ubidots.add("Outdoor Humidity", humidityOutdoor);
  ubidots.add("Indoor Heat Index",indoorHeatIndex(temp, humidity));
  ubidots.add("Outdoor Heat Index", outdoorHeatIndex(tempOutdoor, humidityOutdoor));

  bool bufferSent = false;
  bufferSent = ubidots.send(WEBHOOK_NAME, PUBLIC); 

// Particle publish temperatures, humidities and heat indexes into console 
  Particle.publish("Indoor Temp",String (temp));
  Particle.publish("Indoor Humidity", String (humidity));
  Particle.publish("Outdoor Temp",String (tempOutdoor));
  Particle.publish("Outdoor Humidity", String (humidity));
  Particle.publish("Indoor Heat Index",String(indoorHeatIndex(temp, humidity)));
  Particle.publish("Outdoor Heat Index",String(outdoorHeatIndex(tempOutdoor, humidityOutdoor)));

  Serial.println(long(indoorHeatIndex(temp, humidity)));
  Serial.println(long(outdoorHeatIndex(tempOutdoor, humidityOutdoor)));
  Serial.println(outside);
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
void updateDisplay (double inside, double outside)
{

// Clearing Display before updating
  SeeedOled.clearDisplay(), 

  SeeedOled.setTextXY(1, 0);
  SeeedOled.putString("Heat Indexes");

  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("In Index:");
  SeeedOled.putNumber(inside);
  SeeedOled.putString("F");

  SeeedOled.setTextXY(5, 0);
  SeeedOled.putString("Out Index:");
  SeeedOled.putNumber(outside);
  SeeedOled.putString("F");

}

// Indoor heat index equation
double indoorHeatIndex (double temp, double humidity) {
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
double outdoorHeatIndex (double tempOutdoor, double humidityOutdoor) {
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

void publishData() {
	// This just publishes some somewhat random data for testing
  double insideHeatIndex = indoorHeatIndex(temp, humidity);;
  double outsideHeatIndex = outdoorHeatIndex(tempOutdoor, humidityOutdoor);

	char buf[256];
	snprintf(buf, sizeof(buf), "{\"insideHeatIndex\":%f,\"outsideHeatIndex\":%f}", insideHeatIndex, outsideHeatIndex);
	Serial.printlnf("publishing %s", buf);
	Particle.publish(PUBLISH_EVENT_NAME, buf, PRIVATE);
}
