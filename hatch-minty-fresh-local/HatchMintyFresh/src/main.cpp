#include <Arduino.h>

// libraries 
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>
#include <PID_v1.h>
#include <Ticker.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// define GPIO
#define ZCROSS 5            // pin D1
#define PWM 4               // pin D2
#define LOWERDHTPIN 14      // pin D5
#define UPPERDHTPIN 12      // pin D6
#define HUMIDITYPOWER 13    // pin D7
#define HUMIDITYCONTROL 15  // pin D8
#define RESETPIN 9          // pin SDS

// define wifi credentials
#define WIFI_SSID "Braden, Use This Wifi"
#define WIFI_PASSWORD ""

// define firebase constants
#define API_KEY ""
#define USER_EMAIL "minty@fresh.com"
#define USER_PASSWORD ""
#define DATABASE_URL "https://hatchmintyfresh-default-rtdb.firebaseio.com/"


// Create DHT Objects
DHT highdht(UPPERDHTPIN, DHT22);
DHT lowdht(LOWERDHTPIN, DHT22);

// Create PID Objects
double TargetTemp, Input, Output = 0;
double softKp = 0.3, softKi = 30, softKd = 0.5;
double hardKp = 20, hardKi = 15, hardKd = 0;
double softMinRelThres = -2, softMaxRelThres = 1.5;
int softMinOut = 10, softMaxOut = 55;
int hardMinOut = 0, hardMaxOut = 99;
PID tempPID(&Input, &Output, &TargetTemp, softKp, softKi, softKd, DIRECT);
bool softActive = true;

// Create Humidity Controller Objects
double Humidity;
double TargetHum;
bool HumidifierOn;


// Create Timer Objects
Ticker timer;
unsigned long pidStartTime = millis();
unsigned long humDataPrevTime = millis();
unsigned long sendDataPrevTime = millis();

// Create Firebase Objects
FirebaseData fbdo;
FirebaseData stream;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
volatile bool dataChanged = false;
String eventPath = "";

// Create Time Objects
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

bool firstTimeBoot = true;

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
  return ((lowdht.readTemperature(true) + highdht.readTemperature(true))/2.0);
}


/*------------------------------------------------- getAvgHum --------
|  Function getAvgHum
|
|  Purpose:  reads the current humitidy from the two DHT22 sensors
|            and returns the average humidity in percentage
|
|  Parameters: none
|
|  Returns:  average humidity in percent
*-------------------------------------------------------------------*/
double getAvgHum() {
  return ((lowdht.readHumidity() + highdht.readHumidity())/2.0);
}


/*------------------------------------------------- turnOnHum --------
|  Function turnOnHum
|
|  Purpose:  turns on the humidifier
|
|  Parameters: none
|
|  Returns:  none
*-------------------------------------------------------------------*/
void turnOnHum() {
  if (!HumidifierOn) {
    HumidifierOn = true;
    digitalWrite(HUMIDITYPOWER, HIGH);
    digitalWrite(HUMIDITYCONTROL, HIGH);
    delay(50);
    digitalWrite(HUMIDITYCONTROL, LOW);
    delay(50);
    digitalWrite(HUMIDITYCONTROL, HIGH);
  }
}


/*------------------------------------------------- turnOffHum -------
|  Function turnOffHum
|
|  Purpose:  turns off the humidifier
|
|  Parameters: none
|
|  Returns:  none
*-------------------------------------------------------------------*/
void turnOffHum() {
  HumidifierOn = false;
  digitalWrite(HUMIDITYPOWER, LOW);
  digitalWrite(HUMIDITYCONTROL, LOW);
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
  double lInput = getAvgTemp();
  if(!isnan(lInput)) {  // check for NaN
    Input = lInput;
    // soft PID controller mode
    if (TargetTemp+softMinRelThres <= Input && TargetTemp+softMaxRelThres >= Input) {
      if (softActive == false) {
        softActive = true;
        tempPID.SetTunings(softKp, softKi, softKd);
        tempPID.SetOutputLimits(softMinOut, softMaxOut);
      }
    }

    // hard PID controller mode
    else if (softActive == true)
    {
      softActive = false;
      tempPID.SetTunings(hardKp, hardKi, hardKd);
      tempPID.SetOutputLimits(hardMinOut, hardMaxOut);
    }
    
    tempPID.Compute();
  }
}


/*------------------------------------------------- updateHum --------
|  Function updateHum
|
|  Purpose:  updates the Humidity reading and computes required output
|
|  Parameters: none
|
|  Returns:  none
*-------------------------------------------------------------------*/
void updateHum() {
  humDataPrevTime = millis();
  double lInput = getAvgHum();
  if(!isnan(lInput)) {  // check for NaN
    Humidity = lInput;
    if (TargetHum > Humidity) {
      if (!HumidifierOn) {
        turnOnHum();
      }
    }
    else if (HumidifierOn)
    {
      turnOffHum();
    }
    
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


/*------------------------------------------------- initWiFi ---------
|  Function initWiFi
|
|  Purpose:  initial setup to connect to the wifi
|
|  Parameters: none
|
|  Returns:  none
*-------------------------------------------------------------------*/
IRAM_ATTR void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int count = 0;
  if(!firstTimeBoot) {
    digitalWrite(RESETPIN, LOW);
  }
  while (WiFi.status() != WL_CONNECTED && count < 15) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(100);
    digitalWrite(BUILTIN_LED, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(100);
    digitalWrite(BUILTIN_LED, LOW);
    delay(200);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(250);
    digitalWrite(BUILTIN_LED, LOW);
    delay(250);
    count++;
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  firstTimeBoot = false;
}


/*------------------------------------------------- getTime ----------
|  Function getTime
|
|  Purpose:  gets current epoch time from the web
|
|  Parameters: none
|
|  Returns:  none
*-------------------------------------------------------------------*/
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}


void streamCallback(FirebaseStream data)
{
  dataChanged = true;
}


void streamTimeoutCallback(bool timeout)
{
  // do nothing
}


void setup() {
  // reset pin setup
  digitalWrite(RESETPIN, HIGH);
  pinMode(RESETPIN, OUTPUT);
  
  // timer setup
  timer1_attachInterrupt(onTime); // Add ISR Function
	timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);

  // dimmer setup
  pinMode(PWM, OUTPUT);
  pinMode(ZCROSS, INPUT);
  attachInterrupt(digitalPinToInterrupt(ZCROSS), ZeroCross, RISING);
  
  // humitiy setup
  pinMode(HUMIDITYPOWER, OUTPUT);
  pinMode(HUMIDITYCONTROL, OUTPUT);
  turnOffHum();
  TargetHum = 45;
  
  // Wifi setup
  pinMode(BUILTIN_LED, OUTPUT);
  initWiFi();
  timeClient.begin();

  // Firebase setup
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  !Firebase.signUp(&config, &auth, "", "");
  Firebase.begin(&config, &auth);
  Firebase.RTDB.beginStream(&stream, "/Configurable");
  Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);

  // DHT setup
  highdht.begin();
  lowdht.begin();

  // PID setup
  TargetTemp = 99.5;
  Input = getAvgTemp();
  tempPID.SetOutputLimits(softMinOut, softMaxOut);
  tempPID.SetSampleTime(2000);
  tempPID.SetMode(AUTOMATIC);
}


void loop() {
  // Update PID Timer
  if(millis() - pidStartTime >= 2000) {
    updatePID();
  }
  
  // Update Humidity Timer
  if(millis() - humDataPrevTime >= 4000) {
    updateHum();
  }
  
  // Add sample to Firebase Timer
  if(millis() - sendDataPrevTime >= 10000) {
    if (Firebase.ready()) {
      sendDataPrevTime = millis();

      String curTime = String(getTime());

      json.set("0", Input);
      json.set("1", Humidity);
      json.set("2", Output);
      json.set("3", HumidifierOn);
      json.set("4", TargetTemp);
      json.set("5", TargetHum);
      Firebase.RTDB.setJSON(&fbdo, "IncubatorAtmosphere/" + curTime, &json);
    }
  }

  // Firebase Data Changed
  if(dataChanged && Firebase.ready()) {
    dataChanged = false;
    Firebase.RTDB.getFloat(&fbdo, "Configurable/TargetTemp/");
    TargetTemp = fbdo.to<float>();
    Firebase.RTDB.getFloat(&fbdo, "Configurable/TargetHum/");
    TargetHum = fbdo.to<float>();
  }
}