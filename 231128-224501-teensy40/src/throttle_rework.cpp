#include "config.h"
// LEITOURGEI MONO GIA HAND!!!!
#define THROTTLE_PIN1 0 //analog pin 6(pin 20)
#define REVERSE_PIN 4

#define HAND_MIN 340
#define HAND_MAX 60
#define HAND_LIM 335     //355 - 3 

#define WINDOW_LENGTH 50
#include <Arduino.h>
#include "OCEANOS_CAN.h"  //TODO: rename and expand library
#include "VescCAN.h"

//Can0 here is used purely for listening to Can0
CAN_message_t can_message;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;
OCEANOS_CAN Can_listen(VESC_ID);
VescCAN motor1(VESC_ID);


int angle;
int start_angle;
int start_angle2;
int8_t throttle;
uint8_t status;
elapsedMillis sinceLastCurrent_Write;

bool reverse;
bool reverse_pin;
bool fl_hand_throttle = 1;
float motor_current = 0;
bool wasStopped = true;
bool startUpEnd = false;
unsigned long startUpEndTime = 0;
unsigned long lastIncrementTime = 0;
float sweepCurrent = 40;
int window[WINDOW_LENGTH];  //for denoising the input signal
uint64_t sum = 0;
int w = 0;
void setMotorCurrent(float current);
void neutral_to_start_f();
void safety();
void setup() {
  for (int i = 0; i < WINDOW_LENGTH; i++) {
    window[i] = 0;
  }
  Serial.begin(115200);  // NOTE BAUD RATE


  Can0.begin();
  Can0.setMaxMB(16);
  Can0.enableMBInterrupts();

  Can0.setBaudRate(CAN_BAUD_RATE);     //CANBUS BAUD RATE
  Can0.attachObj(&Can_listen);
  Can0.mailboxStatus();
  //Here you can do some filtering if you'd like, though it's not really neccessary here(plus teensy is really fast)
  Can_listen.attachGeneralHandler();
  delay(200);  //wait for Can_listen to catch a packet
  Can0.events();
  pinMode(REVERSE_PIN, INPUT_PULLUP);  //reverse switch

  neutral_to_start_f();
  Serial.println("Exit start");
}

void loop() {
  //Take mean of 100 inputs, so that we minimize input noise from the throttles
  //exe geia vedder 8ee pou evales to reverse sto mpp kai mas vazeis na kanoume olo malakies
  Can0.events();
  safety();

  w %= WINDOW_LENGTH;
  sum -= window[w];
  window[w] = (int)analogRead(THROTTLE_PIN1) * 360 / 1024;
  Serial.println((String)analogRead(THROTTLE_PIN1));
  sum += window[w++];
  angle = (int)sum / WINDOW_LENGTH;


  //reverse logic -> Go to reverse (and back) only on Neutral
  reverse_pin = digitalRead(REVERSE_PIN);
  if (angle > HAND_LIM) {
    reverse = !reverse_pin;
              //TODO: Include a light indicator for reverse switch state!!!!!
    //Serial.println(angle);
  }
  //toggle throttle logic
  if(angle < HAND_MAX) angle = HAND_MAX;


  /*reverse*/
  if (reverse) {
    if (wasStopped)
      throttle = map(angle, HAND_MIN, HAND_MAX, -15, -100);
    else
      throttle = map(angle, HAND_MIN, HAND_MAX, -5, -100);
  }
  /*forward*/
  if (!reverse) {
    if (wasStopped)
      throttle = map(angle, HAND_MIN, HAND_MAX, 15, 100);
    else
      throttle = map(angle, HAND_MIN, HAND_MAX, 5, 100);
  }
  /*Neutral*/
  if (angle > HAND_LIM) {
    throttle = 0;
    wasStopped = true;
  }

  //?????????
  if (wasStopped == true && (throttle != 0) && (millis() - lastIncrementTime) > 100) {
    Serial.println(sweepCurrent);
    lastIncrementTime = millis();
    setMotorCurrent(sweepCurrent);
    sweepCurrent += 1;
    if (sweepCurrent > 50) {
      startUpEnd = true;
      wasStopped = false;
      startUpEndTime = millis();
      sweepCurrent = 40;
    }

  } 
  
  else {
    motor_current = throttle * CURRENT_LIMIT / 100;
    setMotorCurrent(motor_current);
  }
}
//On device startup(or after reconnecting to motor), ensure that throttle is in position 0
void neutral_to_start_f() {
  while (true) {
    Can0.events();
    reverse_pin = digitalRead(REVERSE_PIN);
    
    setMotorCurrent(0);
    
    start_angle = (int)analogRead(THROTTLE_PIN1) * 360 / 1024;

    delay(250);

    start_angle2 = (int)analogRead(THROTTLE_PIN1) * 360 / 1024;


    int test_throttle = map(start_angle, HAND_MIN, HAND_MAX, -5, -100);
    Serial.println((String) "More info: " + "Can_listen.motor.voltage: " + Can_listen.motor.voltage + " start_angle: " + start_angle + " " + start_angle2 +"Test Throttle:  " + test_throttle);
    if (((start_angle > HAND_LIM) && (start_angle2 > HAND_LIM)) && ((!Can_listen.motor.voltage) <= VOLTAGE_LIMIT)) break;
    Serial.println("Place throttle to zero position!!!");
    delay(250);

  }
  Serial.println("Exited neutral_to_startf, normal operation initiated");
}

//turn apropriate light when toggling the switch, but change input device only when at low 
//this shit don't work
void setMotorCurrent(float current) {
  if (sinceLastCurrent_Write  < TIME_TR) return;
  else sinceLastCurrent_Write -= TIME_TR;
  motor1.setMotorCurrent(current);
  Can0.write(motor1.setCurrent);
  Serial.println((String) "setMotorCurrent = " + current);
}

//to prevent accidental motor activation on the event throttle stays powered in 
void safety(){
  if(Can_listen.motor.voltage <= VOLTAGE_LIMIT) {
    Serial.println("voltage <= "+ (String)VOLTAGE_LIMIT+ " or CAN Frame not found");
    neutral_to_start_f();
  }
}
