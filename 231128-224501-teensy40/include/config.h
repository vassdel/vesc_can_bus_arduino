#ifndef _CONFIG_H_
#define _CONFIG_H_
#endif

//VESC CANBus config
#define VESC_ID 110
#define CAN_BAUD_RATE 500000    //in bps
#define F_TRANSMIT 10 //Hz                  //limit output rate of current/RPM CAN Frames
#define TIME_TR 1000/F_TRANSMIT  //ms

//safety limit for throttle
//if motor is disconnected, or CAN is lost, revert to neutral_start_f
#define VOLTAGE_LIMIT 10
//Maximum current to request from VESC(not safety critical, but still important)
#define CURRENT_LIMIT 400