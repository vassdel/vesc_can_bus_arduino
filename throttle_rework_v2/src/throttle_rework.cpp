#include "config.h"
// LEITOURGEI MONO GIA HAND!!!!
#define THROTTLE_PIN1 0 //analog pin 0(pin 14)
#define REVERSE_PIN 4
#define THROTTLE_ORIENTATION_REVERSED 1  //determines the orientation of the throttle
#define invert(x) 1023-x

#ifndef THROTTLE_ORIENTATION_REVERSED
#define HAND_MAP_MIN 190    //To be reviewed-Field Tested
#define HAND_MAP_MAX 970

#define HAND_MAP_MIN_RANGE 100
#define HAND_MAP_MAX_RANGE 100

#define HAND_SAFETY_MIN 160     
#define HAND_SAFETY_MAX 1200   //to protect against accidental throttle disconnection
#endif
#ifdef THROTTLE_ORIENTATION_REVERSED
#define HAND_MAP_MIN invert(970)    //To be reviewed-Field Tested
#define HAND_MAP_MAX invert(190)

#define HAND_MAP_MIN_RANGE 100
#define HAND_MAP_MAX_RANGE 100

#define HAND_SAFETY_MIN invert(1000)   
#define HAND_SAFETY_MAX invert(100)   //to protect against accidental throttle disconnection
#endif
#define WINDOW_LENGTH 50
#include <Arduino.h>
#include "OCEANOS_CAN.h"  //TODO: expand library
#include "VescCAN.h"

//Can0 here is used purely for listening to Can0
CAN_message_t can_message;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;
OCEANOS_CAN Can_listen(VESC_ID);
VescCAN motor1(VESC_ID);


int angle;
int8_t throttle;
uint8_t status;
elapsedMillis sinceLastCurrent_Write;
uint64_t timeStopped;

bool reverse;
bool reverse_pin;
bool fl_hand_throttle = 1;
float motor_current = 0;
bool wasStopped = true;
bool startUpEnd = false;
unsigned long startUpEndTime = 0;
unsigned long lastIncrementTime = 0;
float sweepCurrent = 40;
int window[WINDOW_LENGTH] = {};  //for denoising the input signal
uint64_t sum;
int w = 0;
void setMotorCurrent(float current);
void neutral_to_start_f();
void safety();
bool fl_zero = false;
void setup() {

  sum = 360 * WINDOW_LENGTH;
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
  setMotorCurrent(0);
  neutral_to_start_f();
  Serial.println("Exit start");
}

void loop() {
  //Take mean of 100 inputs, so that we minimize input noise from the throttles
  //exe geia vedder 8ee pou evales to reverse sto mpp kai mas vazeis na kanoume olo malakies
  Can0.events();

  // w %= WINDOW_LENGTH;
  // sum -= window[w];
  // window[w] = (int)analogRead(THROTTLE_PIN1);
  // //Serial.println((String)analogRead(THROTTLE_PIN1));
  // sum += window[w++];
  // angle = (int)sum / WINDOW_LENGTH;
  angle = analogRead(THROTTLE_PIN1);

  if(THROTTLE_ORIENTATION_REVERSED) angle = invert(angle);
  safety();

  #ifdef DEBUG
  Serial.println(angle);
  #endif
  //reverse logic -> Go to reverse (and back) only on Neutral
  reverse_pin = digitalRead(REVERSE_PIN);
  if (angle < (HAND_MAP_MIN + HAND_MAP_MIN_RANGE)) {
    reverse = !reverse_pin;
              //TODO: Include a light indicator for reverse switch state!!!!!
    //Serial.println(angle);
  }
  //toggle throttle logic
  if(angle < HAND_MAP_MIN) angle = HAND_MAP_MIN;
  if(angle > HAND_MAP_MAX) angle = HAND_MAP_MAX;



  /*reverse*/
  if (reverse) {
    if (wasStopped)
      throttle = map(angle, HAND_MAP_MIN, HAND_MAP_MAX, -15, -100);
    else
      throttle = map(angle, HAND_MAP_MIN, HAND_MAP_MAX, -5, -100);
  }
  /*forward*/
  if (!reverse) {
    if (wasStopped)
      throttle = map(angle, HAND_MAP_MIN, HAND_MAP_MAX, 15, 100);
    else
      throttle = map(angle, HAND_MAP_MIN, HAND_MAP_MAX, 5, 100);
  }
  /*Neutral*/
  if (angle < (HAND_MAP_MIN + HAND_MAP_MIN_RANGE)) {
    throttle = 0;
    wasStopped = true;
    timeStopped = millis();
  }
  if((millis() - timeStopped) > 1000) wasStopped = false;
  #ifdef DEBUG
  Serial.println("Throttle: " + (String)throttle);
  #endif
  //?????????
  #ifdef CURRENT_SWEEP_ENABLE
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
  #endif
  motor_current = throttle * (CURRENT_LIMIT / 100);
  if(throttle == 0) motor_current = 0;
  setMotorCurrent(motor_current);
}
//On device startup(or after reconnecting to motor), ensure that throttle is in position 0
void neutral_to_start_f() {
  int start_angle_1;
  int start_angle_2;
  while (true) {
    Can0.events();
    reverse_pin = digitalRead(REVERSE_PIN);
    
    setMotorCurrent(0);
    
    start_angle_1 = (int)analogRead(THROTTLE_PIN1);
    if(THROTTLE_ORIENTATION_REVERSED) start_angle_1 = invert(start_angle_1);

    delay(250);

    start_angle_2 = (int)analogRead(THROTTLE_PIN1);
    if(THROTTLE_ORIENTATION_REVERSED) start_angle_2 = invert(start_angle_2);

    if((start_angle_1 < HAND_SAFETY_MIN)| (start_angle_1 > HAND_SAFETY_MAX) | (start_angle_2 < HAND_SAFETY_MIN)| (start_angle_2 > HAND_SAFETY_MAX)) {
      Serial.println("Throttle Disconnected !");
      continue;
      }
    int test_throttle = map(start_angle_1, HAND_MAP_MIN, HAND_MAP_MAX, -5, -100);
    #ifdef DEBUG
    Serial.println((String) "More info: " + "Can_listen.motor.voltage: " + Can_listen.motor.voltage + " start_angle: " + start_angle_1 + " " + start_angle_2 +" Test Throttle:  " + test_throttle);
    if ((start_angle_1 < (HAND_MAP_MIN + HAND_MAP_MIN_RANGE)) && (start_angle_2 < (HAND_MAP_MIN + HAND_MAP_MIN_RANGE))) break;
    #endif
    #ifndef DEBUG
    Serial.println((String) "More info: " + "Can_listen.motor.voltage: " + Can_listen.motor.voltage + " start_angle: " + start_angle_1 + " " + start_angle_2 +" Test Throttle:  " + test_throttle);
    if ((start_angle_1 < (HAND_MAP_MIN + HAND_MAP_MIN_RANGE)) && (start_angle_2 < (HAND_MAP_MIN + HAND_MAP_MIN_RANGE)) && (Can_listen.motor.voltage > VOLTAGE_LIMIT)) break;
    Serial.println("Place throttle to zero position!!!");
    delay(250);
    #endif
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
  motor1.printMotorCurrent();
  //Serial.println((String)"setMotorCurrent = " + current);
}

//to prevent accidental motor activation on the event throttle stays powered on 
void safety(){
  #ifdef DEBUG
  if((angle < HAND_SAFETY_MIN) |(angle > HAND_SAFETY_MAX)) {
    Serial.println("voltage <= "+ (String)VOLTAGE_LIMIT+ ", CAN Frame not found, or throttle disconnected");
    neutral_to_start_f();
  }
  return;
  #endif
  if((Can_listen.motor.voltage <= VOLTAGE_LIMIT )| (angle < HAND_SAFETY_MIN) |(angle > HAND_SAFETY_MAX)) {
    Serial.println("voltage <= "+ (String)VOLTAGE_LIMIT+ ", CAN Frame not found, or throttle disconnected");
    neutral_to_start_f();
  }
}
