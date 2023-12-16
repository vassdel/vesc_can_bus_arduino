#include "../OCEANOS_CAN/OCEANOS_CAN.h"
class VescCAN {  
    private:
        int32_t id;
        CAN_message_t setCurrentRelative;
        CAN_message_t setDutyCycle;
        CAN_message_t setBrakeCurrent;
        CAN_message_t setBrakeCurrentRelative;
        CAN_message_t setMotorCurrentLimit;
        CAN_message_t setStoreMotorCurrentLimit;
        CAN_message_t setInputCurrentLimit;
        CAN_message_t setStoreInputCurrentLimit;


    public:
        CAN_message_t setCurrent;
		CAN_message_t setRPMs;

        VescCAN(int VescId);
        void setMotorCurrent(float current);
        void setMotorCurrentPercentage(int32_t percentage);
		void printMotorCurrent();
		void setRPM(int32_t rpm);
		void printRPM();
};