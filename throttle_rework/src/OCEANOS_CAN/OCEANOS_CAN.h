#ifndef OCEANOS_CAN_h
#define OCEANOS_CAN_h

#include <FlexCAN_T4.h>
#include <TimeLib.h>
#include "buffer.h"



#define MAXEQ(A,B) (A) = max((A), (B))

// --------------------------------------------------------

class Throttle_Data{
    public:
        int8_t value;   // Requested Power [-100;100]
        bool zero;      // If set, throttle has gone to zero after startup

};

// --------------------------------------------------------

class Motor_Data{
    public:
        float motor_current;        // Motor current (quadrature current)
        float input_current;        // VESC Input Current
        float duty_cycle;           // VESC Duty Cycle
        int32_t rpm;                // Motor RPMs 
        float voltage;              // VESC Input Voltage
        float temp_motor;           // Motor Temperature
        float temp_mos_max;         // Highest MOS Temperature DOSE NOT COME FROM THE CAN BUS
        float temp_mos_1;           // VESC MOS 1 Temperature
        float temp_mos_2;           // VESC MOS 2 Temperature
        float temp_mos_3;           // VESC MOS 3 Temperature
        uint8_t fault;              // VESC Fault Code
        uint8_t id;                 // VESC ID
        int16_t asked_current;      // Motors Current Resquested
        
};
// --------------------------------------------------------


//CanListener is a dummy class FlexCANT4 uses to service interrupts
class OCEANOS_CAN : public CANListener 
{
private:
    static time_t getTeensy3Time();
	int32_t vesc_id;
public:
    int *filters;
    int32_t ind = 0;
    uint8_t status;


    Throttle_Data throttle;
    Motor_Data motor;

    OCEANOS_CAN(int VESC_id);  
    void receive_Message(CAN_message_t &frame);
    void printFrame(CAN_message_t &frame, int mailbox);
    bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something 
};

#endif
