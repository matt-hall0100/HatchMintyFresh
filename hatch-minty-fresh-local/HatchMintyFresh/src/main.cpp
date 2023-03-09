#include <Arduino.h>

// libraries 
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PID_v1.h>

// define GPIO
#define LOWERDHTPIN 14  // pin D5
#define UPPERDHTPIN 12  // pin D6

// Create DHT Objects
DHT updht(UPPERDHTPIN, DHT22);
DHT lowdht(LOWERDHTPIN, DHT22);

// Create PID Objects
double Setpoint, Input, Output;
double Kp = 2, Ki = 5, Kd = 1;
PID tempPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

/*------------------------------------------------- getAvgTemp -------
|  Function getAvgTemp
|
|  Purpose:  reads the current temperature from the two DHT22 sensors
|            and returns the average temperature in degrees fahrenheit
|
|  Parameters: none
|
|  Returns:  average temperature in degrees fahrenheit
*-------------------------------------------------------------------*/
double getAvgTemp() {
  return ((lowdht.readTemperature(true) + updht.readTemperature(true))/2.0);
}

/*------------------------------------------------- updatePID --------
|  Function updatePID
|
|  Purpose:  updates the PID input then compute/update the PID output
|
|  Parameters: none
|
|  Returns:  none
*-------------------------------------------------------------------*/
void updatePID() {
  Input = getAvgTemp();
  tempPID.Compute();
  Serial.print(F("\nPID Output Updated to: "));
  Serial.print(Output);
}


void setup() {
  Serial.begin(115200);

  // DHT setup
  updht.begin();
  lowdht.begin();

  // PID setup
  Setpoint = 99.5;
  Input = getAvgTemp();
  tempPID.SetOutputLimits(0, 100);
  tempPID.SetMode(AUTOMATIC);
}

void loop() {
  Serial.print(F("\nTemperature: "));
  Serial.print(getAvgTemp());
  updatePID();
  delay(2000);
}