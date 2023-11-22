#include <SPI.h>
#include <mcp2515.h>

MCP2515 mcp2515(10);  // chip select pin: 10

bool is_found[7]; // is_found[i] = false,  means that we do not have captured the info about what i indicates
double data[11]; // data[i] contains the double data for the info about what i indicates


int index(canid_t x){       // mapping of HEX codes to int indexes for the arrays
  if (x == 0x6B0) return  0;
  if (x == 0x80001B73) return  5; // Tachometer, inputVolt
  if (x == 0x80001073) return  4; // tempMosfet, tempMotor, inputCurrent, (pid)
  if (x == 0x80000973) return  1; // rpm , motorCurrent, dutyCycle
  if (x == 0x80000E73) return  2; // consumedAh, chargedAh
  if (x == 0x80000F73) return  3; // consumedWh, chargedWh
  if (x == 0x80003A73) return  6; // xixi (i nina itan edw)
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

int64_t rpm, inVolt, tempMosfet, consumedWh, chargedWh, motorAmpere, dutyCycle, tempMotor, inputCurrent, consumedAh, chargedAh;

void convert(int index_info, __u8 canMsgdata[]){  // converts canMsgdata to final double form and saves value in data[index_info]
    switch (index_info){
      case 1: 
        rpm = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        motorAmpere = ((int64_t)canMsgdata[4] << 8 | (int64_t)canMsgdata[5]);
        dutyCycle = ((int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[0] = (int32_t)rpm * 1.0 ;          // check
        data[1] = (int32_t)motorAmpere * 0.1;   // should be around 2-3. ours is 20-60
        data[2] = (int32_t)dutyCycle * 0.001;   // always 0
        break;
      case 4:
        tempMosfet = ((int64_t)canMsgdata[0] << 8) | ((int64_t)canMsgdata[1]);
        tempMotor = ((int64_t)canMsgdata[2] << 8) | ((int64_t)canMsgdata[3]);
        inputCurrent = ((int64_t)canMsgdata[4] << 8) | ((int64_t)canMsgdata[5]);
        data[3] = (int32_t)tempMosfet * 0.1; // check
        data[4] = (int32_t)tempMotor * 0.1;  // we have no sensor connected
        data[5] = (int32_t)inputCurrent * 0.1; // ... something like overcurrent. from 0.15 to 446
        break;
      case 3:
        consumedWh = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        chargedWh = ((int64_t)canMsgdata[4] << 24 |(int64_t)canMsgdata[4] << 16 |(int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[6] = (int32_t)consumedWh * 0.0001; // check
        data[7] = (int32_t)chargedWh * 0.0001; // check 
        break;
      case 2:
        consumedAh = ((int64_t)canMsgdata[0] << 24 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
        chargedAh = ((int64_t)canMsgdata[4] << 24 |(int64_t)canMsgdata[4] << 16 |(int64_t)canMsgdata[6] << 8 | (int64_t)canMsgdata[7]);
        data[8] = (int32_t)consumedAh * 0.0001; // check
        data[9] = (int32_t)chargedAh * 0.0001;  // check
        break;
      case 5:
        inVolt = ((int64_t)canMsgdata[4] << 8) | ((int64_t)canMsgdata[5]);
        data[10] = (int32_t)inVolt * 0.1; // 
        break;
  }
    //int64_t x = ((int64_t)canMsgdata[0] << 32 |(int64_t)canMsgdata[1] << 16 |(int64_t)canMsgdata[2] << 8 | (int64_t)canMsgdata[3]);
    int64_t x = ((int64_t)canMsgdata[4] << 8 | (int64_t)canMsgdata[5]);
    data[index_info] = x; //(int32_t)tempMosfet * 0.1;
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
  Serial.print("Results: ");
  for (int i=0; i<11; i++){
    Serial.print(data[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void check_all_data(){ // if we have all the data (7 info) for this short time it delays() and sets is_found[] = true; 
  for(int i = 1; i<7; i++){ // change 1 to 0 for BMS
    if (is_found[i]==false) return;  // if at least one info is unknown stop checking
  }
  for(int i = 1; i<7; i++){ // new time season everything is unknown
    is_found[i]=false;
    Serial.print(i);   
  }
  Serial.println();

  results_print();
  delay(200);
}

void setup() {
  Serial.begin(115200);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);  // canSpeed = CAN_500KBPS
  mcp2515.setNormalMode();

  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");

  for(int i = 0; i<7; i++){
    is_found[i]=false;
    
  }
  
}



void loop() {

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)  // return ERROR_FAIL/ERROR_OK
  {
    /*if (canMsg.can_id == 0x6B0) {  // Battery
      Serial.println(canMsg.can_id, HEX);
      //is_found[0]=true;
    }
    //0x80001B73 is input Voltage
    else if (canMsg.can_id == 0x80001B73) {
      Serial.print(canMsg.can_id, HEX);
      Serial.print(": ");
      int64_t inVolt = ((int64_t)canMsg.data[4] << 8) | ((int64_t)canMsg.data[5]);
      Serial.print((int32_t)inVolt * 0.1);
      Serial.println(" ");
    } 
    else if (canMsg.can_id == 0x80001073) {
      Serial.print(canMsg.can_id, HEX);
      Serial.print(": ");
      int64_t tempMosfet = ((int64_t)canMsg.data[0] << 8) | ((int64_t)canMsg.data[1]);
      Serial.print((int32_t)tempMosfet * 0.1);
      Serial.println(" ");
    }
    else if (canMsg.can_id == 0x80000973) {  // Battery
      Serial.println(canMsg.can_id, HEX);
      //is_found[0]=true;
    }
    else if (canMsg.can_id == 0x80000E73) {  // Battery
      Serial.println(canMsg.can_id, HEX);
      //is_found[0]=true;
    }
    else if (canMsg.can_id == 0x80000F73) {  // Battery
      Serial.println(canMsg.can_id, HEX);
      //is_found[0]=true;
    }
    else if (canMsg.can_id == 0x80003A73) {  // Battery
      Serial.println(canMsg.can_id, HEX);
      //is_found[0]=true;
      Serial.println("\n***************************************");
    }
    else {
      Serial.println("\n***************************************Another code captured");
      Serial.println(canMsg.can_id, HEX);
    }*/

    int index_info = index(canMsg.can_id);
    /*Serial.print("Index  is: "); 
    Serial.print(index_info);
    Serial.print("   ");
    for (int i=0; i<8; i++) { Serial.print(canMsg.data[i]); Serial.print(" ");}
    Serial.println(); */
    if (index_info != -1) {     // if index is valid (the can_id is one of the HEX numbers declared in function `int index()` )
      is_found[index_info] = true; // indicate that you found info indexed with (i)
      convert(index_info, canMsg.data);   // convert canMsg.data to final double form and save it in data[index_info]
      check_all_data();                   // checks if we have gathered all data for this short period of time
    }
  }
  
  
  //else {Serial.println("error");}
}



// could write this with for loop