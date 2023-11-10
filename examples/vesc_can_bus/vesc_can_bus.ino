#include <mcp_can.h>
#include "vesc_can_bus_arduino.h"

CAN can;             // get torque sensor data, throttle for now

bool print_realtime_data=false;
long last_print_data;


void setup()
{    
    Serial.begin(115200); // Wired comms from USB port
    can.initialize();

}

void loop()
{
    can.spin();

    can.vesc_set_current(2); //2 amps of current

    if (print_realtime_data == true)
    {
        if (millis() - last_print_data > 200)
        {
            Serial.println(can.erpm); 
            Serial.println(can.inpVoltage);
            Serial.println(can.dutyCycleNow);
            Serial.println(can.avgInputCurrent);
            Serial.println(can.avgMotorCurrent);
            Serial.println(can.tempFET);
            Serial.println(can.tempMotor);

            last_print_data = millis();
        }
    }
}
