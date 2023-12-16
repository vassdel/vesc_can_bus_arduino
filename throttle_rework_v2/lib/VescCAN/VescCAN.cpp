#include "VescCAN.h"
VescCAN::VescCAN(int VescId){
    id = VescId;
    setCurrent.flags.extended = true;
    setCurrentRelative.flags.extended = true;
    setDutyCycle.flags.extended = true;
    setBrakeCurrent.flags.extended = true;
    setRPMs.flags.extended = true;
    setBrakeCurrentRelative.flags.extended = true;
    setMotorCurrentLimit.flags.extended = true;
    setStoreMotorCurrentLimit.flags.extended = true;
    setInputCurrentLimit.flags.extended = true;
    setStoreInputCurrentLimit.flags.extended = true;

    setCurrent.len = 4;
    setCurrentRelative.len = 4;
    setDutyCycle.len = 4;
    setBrakeCurrent.len = 4;
    setRPMs.len = 4;
    setBrakeCurrentRelative.len = 8;
    setMotorCurrentLimit.len = 8;
    setStoreMotorCurrentLimit.len = 8;
    setInputCurrentLimit.len = 8;
    setStoreInputCurrentLimit.len = 8;

    setCurrent.id = 0x100 | VescId;
    setCurrentRelative.id = 0xA00 | VescId;
    setDutyCycle.id = 0x000 | VescId;
    setBrakeCurrent.id = 0x200 | VescId;
    setRPMs.id = 0x300 | VescId;
    setBrakeCurrentRelative.id = 0xB00 | VescId;
    setMotorCurrentLimit.id = 0x1500 | VescId;
    setStoreMotorCurrentLimit.id = 0x1600 | VescId;
    setInputCurrentLimit.id = 0x1700 | VescId;
    setStoreInputCurrentLimit.id = 0x1800 | VescId;
}

void VescCAN::setMotorCurrent(float current){
	int32_t ind = 0;
	buffer_append_int32( setCurrent.buf , current*1000, &ind);
    //Serial.println((String)"setMotorCurrent: " + current + " ID: " + this->id);

}

void VescCAN::setMotorCurrentPercentage(int32_t percentage){
	int32_t ind = 0;
	buffer_append_int32( setCurrentRelative.buf, percentage*1000, &ind);
    //Can0.write(setCurrentRelative);
}
void VescCAN::setRPM(int32_t rpm){
	int32_t ind = 0;
	buffer_append_int32( setRPMs.buf , rpm, &ind);
}

void VescCAN::printMotorCurrent(){
	Serial.print("MB "); Serial.print(setCurrent.mb);
	Serial.print("  OVERRUN: "); Serial.print(setCurrent.flags.overrun);
	Serial.print("  LEN: "); Serial.print(setCurrent.len);
	Serial.print(" EXT: "); Serial.print(setCurrent.flags.extended);
	Serial.print(" TS: "); Serial.print(setCurrent.timestamp);
	Serial.print(" ID: "); Serial.print(setCurrent.id, HEX);
	Serial.print(" Buffer: ");
	for ( uint8_t i = 0; i < setCurrent.len; i++ ) {
		Serial.print(setCurrent.buf[i], HEX); Serial.print(" ");
	} Serial.println();
}