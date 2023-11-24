#include <SPI.h>
#include <mcp2515.h>

MCP2515 mcp2515(10);  // chip select pin: 10

bool is_found[7]; // is_found[i] = false,  means that we do not have captured the info about what i indicates
double data[15]; // data[i] contains the double data for the info about what i indicates


int index(canid_t x){       // mapping of HEX codes to int indexes for the arrays
  if (x == 0x6B0) return  0;
  if (x == 0x80001B6E) return  5; // Tachometer, inputVolt
  if (x == 0x8000106E) return  4; // motorMosfetTemp, motorTemp, motorInputCurrent, (pid)
  if (x == 0x8000096E) return  1; // motorRpm , motorCurrent, motorDutyCycle
  if (x == 0x80000E6E) return  2; // motorConsumedAh, motorChargedAh
  if (x == 0x80000F6E) return  3; // motorConsumedWh, motorChargedWh
  if (x == 0x80003A6E) return  6; // xixi (i nina itan edw)
  return -1;
}

struct can_frame canMsg;
// HELP: can_frame from can.h library (used by mcp2515 library)
/*
struct can_frame {
    canid_t can_id;  // 32 bit CAN_ID + EFF/RTR/ERR flags 
    __u8    can_dlc; // frame payload length in byte (0 .. CAN_MAX_DLEN) 
    __u8    data[CAN_MAX_DLEN] __attribute__((aligned(8)));
    // __u8 -> 8 bit unsigned integer
};*/

int64_t motorRpm, motorInVolt, motorMosfetTemp, motorConsumedWh, motorChargedWh, motorAmpere, motorDutyCycle;
int64_t motorTemp, motorInputCurrent, motorConsumedAh, motorChargedAh, batteryAmpHour, batteryVoltage, batteryCharge, batteryAvgTemp ;

void convert(int index_info, __u8 canMsgdata[]){  // converts canMsgdata to final double form and saves value in data[index_info]
    switch (index_info){
      case 1: 
        motorRpm = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        motorAmpere = ((int64_t)canMsgdata[4] << 8 | (int64_t)canMsgdata[5]);
        motorDutyCycle = ((int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[4] = (int32_t)motorRpm * 1.0 ;           // check
        data[9] = (int32_t)motorAmpere * 0.1;         // check
        data[11] = (int32_t)motorDutyCycle * 0.001;    // check
        break;
      case 4:
        motorMosfetTemp = ((int64_t)canMsgdata[0] << 8) | ((int64_t)canMsgdata[1]);
        motorTemp = ((int64_t)canMsgdata[2] << 8) | ((int64_t)canMsgdata[3]);
        motorInputCurrent = ((int64_t)canMsgdata[4] << 8) | ((int64_t)canMsgdata[5]);
        data[7] = (int32_t)motorMosfetTemp * 0.1;     // check
        data[8] = (int32_t)motorTemp * 0.1;           // we have no sensor connected
        data[10] = (int32_t)motorInputCurrent * 0.1;   // check
        break;
      case 3:
        motorConsumedWh = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        motorChargedWh = ((int64_t)canMsgdata[4] << 24 |(int64_t)canMsgdata[4] << 16 |(int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[6] = (int32_t)motorConsumedWh * 0.0001;  // check
        data[13] = (int32_t)motorChargedWh * 0.0001;   // check 
        break;
      case 2:
        motorConsumedAh = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        motorChargedAh = ((int64_t)canMsgdata[4] << 24 |(int64_t)canMsgdata[4] << 16 |(int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[12] = (int32_t)motorConsumedAh * 0.0001; // check
        data[14] = (int32_t)motorChargedAh * 0.0001;  // check
        break;
      case 5:
        motorInVolt = ((int64_t)canMsgdata[4] << 8) | ((int64_t)canMsgdata[5]);
        data[5] = (int32_t)motorInVolt * 0.1; // check
        break;
      /*
      case 0:
        //Serial.println("case 0");
        batteryAmpHour = ((int64_t)canMsg.data[0] << 8) | ((int64_t)canMsg.data[1]);
        batteryVoltage = ((int64_t)canMsg.data[2] << 8) | ((int64_t)canMsg.data[3]);
        batteryCharge = ((int64_t)canMsg.data[4]);
        batteryAvgTemp = ((int64_t)canMsg.data[5]);
        data[0] = (int32_t)batteryAmpHour * 0.1;
        data[1] = (int32_t)batteryVoltage * 0.1;
        data[2] = (int32_t)batteryCharge * 0.5;
        data[3] = (int32_t)batteryAvgTemp * 1.0;
        break;
      */
  }
}

// *** BMS - BATTERY INFO ***
// ah battery
// volt 
// charge
// avgtemp

// *** VESC INFO ***
// motorRPM
// inpVolt
// wh
// motorMosfetTemp
// motorTemp
// avgMotorCurrent
// avgmotorInputCurrent
// motorDutyCycleNow


void results_print(){                 // prints the recent results from array data
  //Serial.print("Results: ");
  Serial.print(" #");
  Serial.print(millis());
  for (int i=4; i<12; i++){
    Serial.print("#");
    Serial.print(data[i]);
  }
  Serial.println();
}

void check_all_data(){                // if we have all the data (7 info) for this short time it delays() and sets is_found[] = true; 
  for(int i = 1; i<7; i++){           // change 1 to 0 for BMS
    if (is_found[i]==false) return;   // if at least one info is unknown stop checking
  }
  for(int i = 0; i<7; i++){           // new time season everything is unknown
    is_found[i]=false;
    //Serial.print(i);   // for check only
  }
  //Serial.println();

  results_print();
  delay(200);
}

void setup() {
  Serial.begin(115200);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  // canSpeed = CAN_500KBPS
  mcp2515.setNormalMode();

  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");

  for(int i = 1; i<7; i++){
    is_found[i]=false;
  }
  
}



void loop() {

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)  // return ERROR_FAIL/ERROR_OK
  {
    
    int index_info = index(canMsg.can_id);

    if (index_info != -1) {               // if index is valid (the can_id is one of the HEX numbers declared in function `int index()` )
      is_found[index_info] = true;        // indicate that you found info indexed with (i)
      convert(index_info, canMsg.data);   // convert canMsg.data to final double form and save it in data[index_info]
      check_all_data();                   // checks if we have gathered all data for this short period of time
    }
  }
  
}
