#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// define GPIO
#define LOWERDHTPIN 14  // pin D5
#define UPPERDHTPIN 12  // pin D6

// Create DHT Objects
DHT updht(UPPERDHTPIN, DHT22);
DHT lowdht(LOWERDHTPIN, DHT22);

void setup() {
  Serial.begin(115200);

  // DHT setup
  updht.begin();
  lowdht.begin();
}

void loop() {
  float h = lowdht.readHumidity();
  float f = lowdht.readTemperature(true);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(f);
  Serial.print(F("°F "));
  h = updht.readHumidity();
  f = updht.readTemperature(true);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(f);
  Serial.print(F("°F \n\n"));
  delay(2000);
}