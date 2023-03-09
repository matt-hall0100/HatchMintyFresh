#include <Arduino.h>

// libraries 
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PID_v1.h>
#include <Ticker.h>

// define GPIO
#define ZCROSS 5        // pin D1
#define PWM 4           // pin D2
#define LOWERDHTPIN 14  // pin D5
#define UPPERDHTPIN 12  // pin D6

// Create DHT Objects
DHT updht(UPPERDHTPIN, DHT22);
DHT lowdht(LOWERDHTPIN, DHT22);

// Create PID Objects
double Setpoint, Input, Output = 0;
double Kp = 0.75, Ki = 0.1, Kd = 0;
PID tempPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// timer vars
Ticker timer;
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


/*------------------------------------------------- ZeroCross --------
|  Function ZeroCross
|
|  Purpose:  interupt function for the zero crossing givin by the AC
|            dimmer, PWM output is set low and the timer is schedule
|            to go high according to the current PID Output.
|
|  Parameters: none
|
|  Returns:  none
*-------------------------------------------------------------------*/
IRAM_ATTR void ZeroCross() {
  if (Output != 99) {
    digitalWrite(PWM, LOW);
    if (Output > 1) {
      timer1_write(41650-(41650*Output/100.0)); // 8.33 ms max
    }
  } 
  else {
    digitalWrite(PWM, HIGH);
  }
}


/*------------------------------------------------- onTime -----------
|  Function onTime
|
|  Purpose:  interupt function for the timer scheduled during the zero
|            crossing interupt to change the AC PWM dimmer to high
|
|  Parameters: none
|
|  Returns:  none
*-------------------------------------------------------------------*/
IRAM_ATTR void onTime() {
  digitalWrite(PWM, HIGH);
}

void setup() {
  Serial.begin(115200);
  // timer setup
  timer1_attachInterrupt(onTime); // Add ISR Function
	timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);

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
  tempPID.SetOutputLimits(0, 99);
  tempPID.SetSampleTime(500);
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
  if(millis() - pidStartTime >= 500) {
    updatePID();
    counter++;
  }
}