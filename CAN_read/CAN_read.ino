#include <SPI.h>
#include <mcp2515.h>



MCP2515 mcp2515(10);


void setup() {
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}


struct can_frame canMsg;

void loop() {
  

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if (canMsg.can_id == 0x80001B73){
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
    Serial.print((int32_t)inVolt*0.1);
    Serial.print(" ");

    Serial.println();
    delay(100);      
  }
  }
  
}
