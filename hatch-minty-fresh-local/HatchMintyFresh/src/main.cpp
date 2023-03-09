#include <Arduino.h>

// libraries 
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PID_v1.h>

// define GPIO
#define ZCROSS 5        // pin D1
#define PWM 4           // pin D2
#define LOWERDHTPIN 14  // pin D5
#define UPPERDHTPIN 12  // pin D6

// Create DHT Objects
DHT updht(UPPERDHTPIN, DHT22);
DHT lowdht(LOWERDHTPIN, DHT22);

// Create PID Objects
double Setpoint, Input, Output;
double Kp = 2, Ki = 5, Kd = 1;
PID tempPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// timer vars
unsigned long dimmerStartTime = millis();
unsigned long pidStartTime = millis();
int counter = 0;

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
  pidStartTime = millis();
  Input = getAvgTemp();
  if(Input == Input) {  // check for NaN
    tempPID.Compute();
  }
}

IRAM_ATTR void ZeroCross() {
  dimmerStartTime = millis();
  digitalWrite(PWM, LOW);
}

void setup() {
  Serial.begin(115200);

  // dimmer setup
  pinMode(PWM, OUTPUT);
  pinMode(ZCROSS, INPUT);
  attachInterrupt(digitalPinToInterrupt(ZCROSS), ZeroCross, RISING);
  
  // DHT setup
  updht.begin();
  lowdht.begin();

  // PID setup
  Setpoint = 99.5;
  Input = getAvgTemp();
  tempPID.SetOutputLimits(0, 100);
  tempPID.SetSampleTime(1000);
  tempPID.SetMode(AUTOMATIC);
}


void loop() {
  if (counter == 10){
    Serial.print(F("\n\nPID Output set: "));
    Serial.print(Output);
    Serial.print(F("\nTemperature: "));
    Serial.print(Input);
    counter = 0;
  }
  if(millis() - pidStartTime >= 1000) {
    updatePID();
    counter++;
  }
  if(millis() - dimmerStartTime >= (((100-Output)/100.0)*8.33)) {
    digitalWrite(PWM, HIGH);
  }
}