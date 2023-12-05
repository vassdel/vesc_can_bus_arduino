#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>
#define VESC_ID 110
MCP2515 mcp2515(4);  // chip select pin: 10 -> 4 on pcb

double data[11];      // data[i] contains the double data for the info about what i indicates
struct can_frame canMsg;
int64_t rpm, inVolt, tempMosfet, consumedWh, chargedWh, motorAmpere, dutyCycle, tempMotor, inputCurrent, consumedAh, chargedAh;
uint64_t last_print_time;
int64_t batteryAmpHour, batteryVoltage, batteryCharge, batteryAvgTemp; 
uint64_t since_last_read;
void convert(canid_t index_info, __u8 canMsgdata[]){  // converts canMsgdata to final double form and saves value in data[index_info]
    switch (index_info){
      case (0x80000900| VESC_ID): 
        rpm = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        motorAmpere = ((int64_t)canMsgdata[4] << 8 | (int64_t)canMsgdata[5]);
        dutyCycle = ((int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[4] = (int32_t)rpm * 1.0 ;          // check
        data[9] = (int32_t)motorAmpere * 0.1;   // should be around 2-3. ours is 20-60
        data[11] = (int32_t)dutyCycle * 0.001;   // always 0
        break;
      case (0x80000E00| VESC_ID):
        consumedAh = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        chargedAh = ((int64_t)canMsgdata[4] << 24 |(int64_t)canMsgdata[4] << 16 |(int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[12] = (int32_t)consumedAh * 0.0001; // check
        data[14] = (int32_t)chargedAh * 0.0001;  // check
        break;
      case (0x80000F00| VESC_ID):
        consumedWh = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        chargedWh = ((int64_t)canMsgdata[4] << 24 |(int64_t)canMsgdata[4] << 16 |(int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[6] = (int32_t)consumedWh * 0.0001; // check
        data[13] = (int32_t)chargedWh * 0.0001; // check 
        break;
      case (0x80001000| VESC_ID):
        tempMosfet = ((int64_t)canMsgdata[0] << 8) | ((int64_t)canMsgdata[1]);
        tempMotor = ((int64_t)canMsgdata[2] << 8) | ((int64_t)canMsgdata[3]);
        inputCurrent = ((int64_t)canMsgdata[4] << 8) | ((int64_t)canMsgdata[5]);
        data[7] = (int32_t)tempMosfet * 0.1; // check
        data[8] = (int32_t)tempMotor * 0.1;  // we have no sensor connected
        data[10] = (int32_t)inputCurrent * 0.1; // ... something like overcurrent. from 0.15 to 446
        break;
      case (0x80001B00| VESC_ID):
        inVolt = ((int64_t)canMsgdata[4] << 8) | ((int64_t)canMsgdata[5]);
        data[10] = (int32_t)inVolt * 0.1;       // check 
        break;
      case(0x80000100 | VESC_ID):
        Serial.println("Throttle set voltage frame...");
        break;

      case(0x6B0):
        batteryAmpHour = ((int64_t)canMsgdata[0] << 8) | ((int64_t)canMsgdata[1]);
        batteryVoltage = ((int64_t)canMsgdata[2] << 8) | ((int64_t)canMsgdata[3]);
        batteryCharge = ((int64_t)canMsgdata[4]);
        batteryAvgTemp = ((int64_t)canMsgdata[5]);
        data[0] = (int32_t)batteryAmpHour * 0.1;
        data[1] = (int32_t)batteryVoltage * 0.1;
        data[2] = (int32_t)batteryCharge * 0.5;
        data[3] = (int32_t)batteryAvgTemp * 1.0;
        break;
      default:
        Serial.print("Invalid CAN_ID:");Serial.println(index_info, HEX);
        break;
        /*missing case 0: for BMS*/
  }
    //int64_t x = ((int64_t)canMsgdata[0] << 32 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
    int64_t x = ((int64_t)canMsgdata[4] << 8 | (int64_t)canMsgdata[5]); // for testing
    data[index_info] = x; // for testing
}

// *** BMS - BATTERY INFO ***
// ah battery
// volt 
// charge
// avgtemp

// *** VESC INFO ***
// RPM
// inpVolt
// wh
// tempMosfet
// tempMotor
// avgMotorCurrent
// avgInputCurrent
// dutyCycleNow

void results_print(){   // prints the recent results from array data
  if(millis() - since_last_read < 200) return;
  since_last_read = millis();
  Serial.print(" #");
  Serial.print(millis());
  for (int i=0; i<11; i++){
    Serial.print("#");
    Serial.print(data[i]);
  }
  Serial.println();
  for (int i = 0; i <11; i++) data[i] = -1.0;
}

void setup() {
  Serial.begin(115200);
  since_last_read = millis();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  // canSpeed = CAN_500KBPS
  Serial.println("CAN Speed: 500Kbps");
  mcp2515.setNormalMode();

  last_print_time = millis();
  for (int i = 0; i <11; i++) data[i] = -1.0;
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)  // return ERROR_FAIL/ERROR_OK
  {          
      convert(canMsg.can_id, canMsg.data);   // convert canMsg.data to final double form and save it in data[index_info]
      results_print();                   // checks if we have gathered all data for this short period of time
  }
}
