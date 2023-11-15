// MEAM 5100 Group 1
// Author: Yining Guo
// Motor and Encoder feedback control
// Compatible with 9V battery

#include <WiFi.h>
#include <WiFiUdp.h>

#include "Webpage.h"
#include "html510.h"

#define Motor_channel1 0 
#define Motor_channel2 1 

#define Motor_channel3 2
#define Motor_channel4 3

#define Servo_channel 4 //servo

#define Motor_freq 1000  //Motor frequency
#define Motor_resolution_bits 8 //Motor resolution in bits
#define Motor_resolution ((1<<Motor_resolution_bits)-1)

HTML510Server h(80);
const int DiodePin = 1;  // GPIO pin for the photodiode1
const int DiodePin2 = 4;  // GPIO pin for the photodiode2

const int MotorPin1 = 6;  // GPIO pin for the Motor1 Pin1
const int MotorPin2 = 7; // GPIO pin for the Motor1 Pin2

const int MotorPin3 = 18;  // GPIO pin for the Motor2 Pin1
const int MotorPin4 = 19; // GPIO pin for the Motor2 Pin2

int u1, u2, desired_value;

const int ServoPin = 10;  //GPIO pin for the servo motor

volatile uint32_t DownTime1;
volatile uint32_t Counter1 = 0;
volatile uint32_t DownTime2;
volatile uint32_t Counter2 = 0;

uint32_t interruptsPerSecond1;
uint32_t interruptsPerSecond2;

const double KP = 1.4;

WiFiServer server(80);

const char* ssid = "TP-Link_E0C8";  //TP-Link_E0C8
const char* password = "52665134";   //52665134
char numberArray[20];
int recorder;

void IRAM_ATTR handleInterrupt1() {
  DownTime1 = millis(); // millis returns ms since the program started.
  Counter1++;
}

void IRAM_ATTR handleInterrupt2() {
  DownTime2 = millis(); // millis returns ms since the program started.
  Counter2++;
}

void setup() {
  Serial.begin(115200);
  
  ledcSetup(Motor_channel1, Motor_freq, Motor_resolution_bits); // setup Motor1 channel1
  ledcSetup(Motor_channel2, Motor_freq, Motor_resolution_bits); // setup Motor1 channel2

  ledcSetup(Motor_channel3, Motor_freq, Motor_resolution_bits); // setup Motor2 channel1
  ledcSetup(Motor_channel4, Motor_freq, Motor_resolution_bits); // setup Motor2 channel2

  ledcSetup(Servo_channel, Motor_freq, Motor_resolution_bits); // setup Servo
  
  ledcAttachPin(MotorPin1, Motor_channel1); //motor 1
  ledcAttachPin(MotorPin2, Motor_channel2); //motor 1

  ledcAttachPin(MotorPin3, Motor_channel3); //motor 2
  ledcAttachPin(MotorPin4, Motor_channel4); //motor 2

  ledcAttachPin(ServoPin, Servo_channel); // servo
  
  pinMode(DiodePin, INPUT_PULLUP);  // Enable internal pull-up resistor for the switch
  attachInterrupt(digitalPinToInterrupt(DiodePin), handleInterrupt1, FALLING);

  pinMode(DiodePin2, INPUT_PULLUP);  // Enable internal pull-up resistor for the switch
  attachInterrupt(digitalPinToInterrupt(DiodePin2), handleInterrupt2, FALLING);

  IPAddress myIP(192,168,1,130);
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  WiFi.config(myIP,IPAddress(192,168,1,1),IPAddress(255,255,255,0));
  
  while(WiFi.status()!= WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("connected to %s on",ssid); 
  Serial.println(myIP);

  h.begin();
  h.attachHandler("/",handleRoot);
  h.attachHandler("/setDirection?val=", handleSlider1);
  h.attachHandler("/setDutyCycle?val=", handleSlider2);
  h.attachHandler("/setServo?val=", handleSlider3);
}

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  uint32_t duty = Motor_resolution * min(value, valueMax) / valueMax;
  ledcWrite(channel, duty); // write duty to Motor pin
}

int controlFunction(int desired, int sensor){
  int u = desired;
  u = u + KP * (desired-sensor);
  if (u > 100) u = 100; /* motor limits */
  if (u < 0) u = 0;
  return u;
}

void handleRoot(){
  h.sendhtml(body);
}

void handleSlider1() {
  int sliderValue = h.getVal();
  String s = "Move";
  if (sliderValue == 1) {
    s = s+ " Backward";
    ledcAnalogWrite(Motor_channel2,0);
    ledcAnalogWrite(Motor_channel4,0);
    Serial.println(sliderValue);
  }
  else if(sliderValue == 3) {
    s = s+ " Forward";
    ledcAnalogWrite(Motor_channel1,0);
    ledcAnalogWrite(Motor_channel3,0);
    Serial.println(sliderValue);
  }
  else if(sliderValue == 2) {
    s = s+ " Stop";
    ledcAnalogWrite(Motor_channel1,0);
    ledcAnalogWrite(Motor_channel2,0);
    ledcAnalogWrite(Motor_channel3,0);
    ledcAnalogWrite(Motor_channel4,0);
    Serial.println(sliderValue);
  }
  recorder = sliderValue;
  h.sendplain(s);
} // mode slider

void handleSlider2() {
  int sliderValue = h.getVal();
  Serial.println(sliderValue);
  int value_map = map(sliderValue,0 , 100, 0, 255);
  desired_value = map(sliderValue,0 , 100, 0, 70);
  if (recorder == 1){
    ledcAnalogWrite(Motor_channel1,value_map);  // update Motor duty cycle
    ledcAnalogWrite(Motor_channel3,value_map);  // update Motor duty cycle
  } else if (recorder == 3){
    ledcAnalogWrite(Motor_channel2,value_map);
    ledcAnalogWrite(Motor_channel4,value_map);  // update Motor duty cycle
  }
  h.sendplain(itoa(sliderValue,numberArray,10)); //print slider value to website
} // speed slider

void handleSlider3() {
  int sliderValue = h.getVal();
  Serial.println(sliderValue);
  int value_map = map(sliderValue,0 , 100, 0, 255);
  ledcAnalogWrite(Servo_channel,value_map);
  h.sendplain(itoa(sliderValue,numberArray,10)); //print slider value to website
} //servo motor slider

void loop() {
  h.serve();
  delay(10);

  static uint32_t oldDownTime1;
  static uint32_t oldCounter1;

  static uint32_t oldDownTime2;
  static uint32_t oldCounter2;
  
  if (DownTime1 - oldDownTime1 >= 1000) {
    interruptsPerSecond1 = Counter1 - oldCounter1;
    Serial.print("Photodiode 1: ");
    Serial.println(interruptsPerSecond1);

    oldCounter1 = Counter1;
    oldDownTime1 = DownTime1;
    u1 = controlFunction(desired_value, interruptsPerSecond1); // determine control from desired, sensor
    u2 = controlFunction(desired_value, interruptsPerSecond2); // determine control from desired, sensor

    if (recorder ==1 && interruptsPerSecond1 <80 && interruptsPerSecond2 <80) {
      ledcAnalogWrite(Motor_channel1,map(u1,0 , 80, 0, 255));  // update Motor duty cycle
      ledcAnalogWrite(Motor_channel3,map(u2,0 , 80, 0, 255));  // update Motor duty cycle
      Serial.print("Control 1: ");
      Serial.print(u1);
      Serial.println(u2);
      Serial.print("desired value: ");
      Serial.println(desired_value);
    } else if (recorder == 3 && interruptsPerSecond1 <80 && interruptsPerSecond2 <80){
      ledcAnalogWrite(Motor_channel2,map(u1,20 , 80, 0, 255));  // update Motor duty cycle
      ledcAnalogWrite(Motor_channel4,map(u2,20 , 80, 0, 255));  // update Motor duty cycle
      Serial.print("Control 2 :");
      Serial.print(u1);
      Serial.println(u2);
      Serial.print("desired value: ");
      Serial.println(desired_value);
    }
  }

  // Check interrupts for the second photodiode
  if (DownTime2 - oldDownTime2 >= 1000) {
    interruptsPerSecond2 = Counter2 - oldCounter2;
    Serial.print("Photodiode 2: ");
    Serial.println(interruptsPerSecond2);

    oldCounter2 = Counter2;
    oldDownTime2 = DownTime2;

    u1 = controlFunction(desired_value, interruptsPerSecond1); // determine control from desired, sensor
    u2 = controlFunction(desired_value, interruptsPerSecond2); // determine control from desired, sensor

    if (recorder ==1 && interruptsPerSecond1 <80 && interruptsPerSecond2 <80) {
      ledcAnalogWrite(Motor_channel1,map(u1,0 , 80, 0, 255));  // update Motor duty cycle
      ledcAnalogWrite(Motor_channel3,map(u2,0 , 80, 0, 255));  // update Motor duty cycle
      Serial.print("Control 1: ");
      Serial.print(u1);
      Serial.println(u2);
      Serial.print("desired value: ");
      Serial.println(desired_value);
    } else if (recorder == 3 && interruptsPerSecond1 <80 && interruptsPerSecond2 <80){
      ledcAnalogWrite(Motor_channel2,map(u1,0 , 80, 0, 255));  // update Motor duty cycle
      ledcAnalogWrite(Motor_channel4,map(u2,0 , 80, 0, 255));  // update Motor duty cycle
      Serial.print("Control 2: ");
      Serial.print(u1);
      Serial.println(u2);
      Serial.print("desired value: ");
      Serial.println(desired_value);
    }
  }
}
