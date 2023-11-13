#include <SPI.h>
#include <mcp2515.h>

MCP2515 mcp2515(10); //στον περσινό κώδικα ηταν στο 4 ΠΡΟΣΟΧΗ!

bool rtcstate = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}

struct can_frame canMsg;

uint32_t timer = millis();

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(" #");
  Serial.print(millis());
  Serial.print("#");
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    while (canMsg.can_id != 0x80001B73){
      mcp2515.readMessage(&canMsg);
    }
    int64_t inVolt = ((int64_t)canMsg.data[4] << 8) | ((int64_t)canMsg.data[5]);
    Serial.print((int32_t)inVolt*0.1);
    Serial.print("#");
    while (canMsg.can_id != 0x80001073){
      mcp2515.readMessage(&canMsg);
    }
    int64_t tempMosfet = ((int64_t)canMsg.data[0] << 8) | ((int64_t)canMsg.data[1]);
    Serial.print((int32_t)tempMosfet*0.1);
    Serial.print("#");
    while (canMsg.can_id != 0x6B0){
      mcp2515.readMessage(&canMsg);
    }
    int64_t ampHour = ((int64_t)canMsg.data[0] << 8) | ((int64_t)canMsg.data[1]);
    Serial.print((int32_t)ampHour*0.1);
    Serial.print("#");
    int64_t volt = ((int64_t)canMsg.data[2] << 8) | ((int64_t)canMsg.data[3]);
    Serial.print((int32_t)volt*0.1);
    Serial.print("#");
    int charge = canMsg.data[4];
    Serial.print(0.5*charge);
    Serial.print("#");
    int avgtemp = canMsg.data[5];
    Serial.print(avgtemp);
  }
  Serial.println();
}