#include "OCEANOS_CAN.h"
OCEANOS_CAN::OCEANOS_CAN(int VESC_id){
	vesc_id = VESC_id;
}

void OCEANOS_CAN::receive_Message(CAN_message_t &frame)
{
	uint32_t current_id = this->vesc_id;

		if ((0x900 | current_id) == (frame.id)){
			ind = 0;		
			this->motor.rpm = buffer_get_int32(frame.buf, &ind)/5;
			this->motor.motor_current = buffer_get_float16(frame.buf, 1e2, &ind);
			this->motor.duty_cycle = buffer_get_float16(frame.buf, 1e2, &ind);
		}else if((0x1000 | current_id) == (frame.id)){
			ind = 0;
			this->motor.fault = buffer_get_uint8(frame.buf, &ind);
			this->motor.id = buffer_get_uint8(frame.buf, &ind);
		}
		else if ((0x1B00 | current_id) == (frame.id)){
			ind = 4;
			this->motor.voltage = buffer_get_float16(frame.buf, 1e1, &ind);
    }
}

void OCEANOS_CAN::printFrame(CAN_message_t &msg, int mailbox)
{
	Serial.print("MB "); Serial.print(msg.mb);
	Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
	Serial.print("  LEN: "); Serial.print(msg.len);
	Serial.print(" EXT: "); Serial.print(msg.flags.extended);
	Serial.print(" TS: "); Serial.print(msg.timestamp);
	Serial.print(" ID: "); Serial.print(msg.id, HEX);
	Serial.print(" Buffer: ");
	for ( uint8_t i = 0; i < msg.len; i++ ) {
		Serial.print(msg.buf[i], HEX); Serial.print(" ");
	} Serial.println();
}

bool OCEANOS_CAN::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
		// printFrame(frame, mailbox);
		receive_Message(frame);
		return true;
}