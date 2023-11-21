#include <SPI.h>
#include <mcp2515.h>

MCP2515 mcp2515(10); // chip select pin: 10

void setup()
{
  Serial.begin(115200);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); // canSpeed = CAN_500KBPS
  mcp2515.setNormalMode();

  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}

struct can_frame canMsg;
/* HELP: can_frame from can.h library (used by mcp2515 library)

struct can_frame {
    canid_t can_id;  // 32 bit CAN_ID + EFF/RTR/ERR flags 
    __u8    can_dlc; // frame payload length in byte (0 .. CAN_MAX_DLEN) 
    __u8    data[CAN_MAX_DLEN] __attribute__((aligned(8)));
    // __u8 -> 8 bit unsigned integer
};*/

void loop()
{

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) // return ERROR_FAIL/ERROR_OK
  { 
    '''0x80001B73 is input Voltage'''
    if (canMsg.can_id == 0x80001B73)
    {
      /*
      Serial.print(canMsg.can_id, HEX); // print ID
      Serial.print(" ");
      Serial.print(canMsg.can_dlc, HEX); // print DLC
      Serial.print(" ");
      /*
      for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
        Serial.print(canMsg.data[i],HEX);
        Serial.print(" ");
      }
      */

      int64_t inVolt = ((int64_t)canMsg.data[4] << 8) | ((int64_t)canMsg.data[5]);
       /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        ((int64_t)canMsg.data[4] << 8) shifts the value of canMsg.data[4] left by 8 bits to make room for next byte.
        ((int64_t)canMsg.data[5]) takes the value of canMsg.data[5].
      */
      Serial.print((int32_t)inVolt * 0.1);
      Serial.print(" ");

      /*

      * we have finished with inVolt
      * have to find tempMosfet
      * we search for tempMosfet ID: 0x80001073
     
      */
      mcp2515.readMessage(&canMsg);
      while (canMsg.can_id != 0x80001073)
      {
        /*
        for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
          Serial.print(canMsg.data[i],HEX);
          Serial.print(" ");
        }
        */
        mcp2515.readMessage(&canMsg);
      }
      int64_t tempMosfet = ((int64_t)canMsg.data[0] << 8) | ((int64_t)canMsg.data[1]);
      Serial.print((int32_t)tempMosfet * 0.1);
      Serial.println(" ");
      delay(100);
    }
  }
}

// could write this with for loop
