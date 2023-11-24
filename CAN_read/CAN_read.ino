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
    int64_t inVolt = ((int64_t)canMsg.data[4] << 8) | ((int64_t)canMsg.data[5]);
    Serial.print((int32_t)inVolt*0.1);
    Serial.print(" ");
    while (canMsg.can_id != 0x80001073){
    /*TODO: add a counter
    add mcp2515.readMessage(&canMsg); before while
    */
    mcp2515.readMessage(&canMsg);
    } 
    int64_t tempMosfet = ((int64_t)canMsg.data[0] << 8) | ((int64_t)canMsg.data[1]);
    Serial.print((int32_t)tempMosfet*0.1);
    Serial.print(" ");
    Serial.println();
    delay(100);      
  }
  } 
}