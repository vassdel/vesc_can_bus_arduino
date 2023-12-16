# TSB SM01's CAN protocol

Copyright (C) 2021  Técnico Solar Boat

This repository and its contents  is free software: you can redistribute 
it and/or modify it under the terms of the GNU General Public License 
as published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version.

The content of this repository is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

You can contact Técnico Solar Boat by email at: tecnico.solarboat@gmail.com
or via our facebook page at https://fb.com/tecnico.solarboat


⚠️ **SM01's CAN Bus needs to run at 500 kbps given that the Fuel Cell is limited to this speed.** ⚠️

⚠️ **CAN TSB now uses 4 bits for Source and 5 bits for Data.** ⚠️

⚠️ **CAN TSB now uses tonton81's FlexCAN_T4 library check the example at the end of this README** ⚠️


#### Índice

- [Message Structure](#Mensagens)
- [ID Structure](#ID)
- [Code](#Código)

<a name="Mensagens"/>

## Message Structure
![JC](https://upload.wikimedia.org/wikipedia/commons/thumb/5/5e/CAN-Bus-frame_in_base_format_without_stuffbits.svg/709px-CAN-Bus-frame_in_base_format_without_stuffbits.svg.png)

On the code, only ID, DL (message length) and DB (message content) need to be defined.

<a name="ID"/>

### ID Structure

| Priority bits |  Data ID | Source ID |
|:-------------:|:--------:|:---------:|
|    2 bits     |  5 bits  |  4 bits   |


#### Priority bits

- Defines the message priority
- 00 maximum priority
- 11 lowest priority

#### Data ID

- Defines the message content
- With respect to each device

#### Source ID

- Defines which node sent the message
- Together with the Data ID defines which message was sent

| Source ID (4 bits) |           Node          |
|:------------------:|:-----------------------:|
|       0x0          |  [Throttle](#0x0)       |
|       0x1          |  [BMS](#0x1)            |
|       0x2          |  [Fuel Cell](#0x2)      |
|       -          |  [Motor](#0x3)          |
|	    0x5		     |	[Screen](#0x5)         |
|		0x6		     |  [Current Sensor](#0x6) |
|       0x7          |  [Fuse Board](#0x7)     |
|       0x8          |  [Power Box](#0x8)        |
|       0x9          |  [H2 Board](#0x9)       |

<br/>

 ⚠️ Scale is scale argument you need to pass to the  decoding function see: **[buffer.c](https://gitlab.com/tecnicosb/tsb-es/2021/can_tsb/-/blob/master/src/buffer.cpp)** If there is no scale the int function must be used. Otherwise, float functions must be used. ⚠️

<a name="0x0"/>

## Throttle:
### value (CAN ID: 0x10)
**LEN = 2**
|  Offset |   Type   |        Name       |       Scale        |         Description           |
|:-------:|:--------:|:-----------------:|:------------------:|:-----------------------------:|
|    0    |  int8_t  |        Value      |                    | Requested power \[-100;100\]  |
|    1    |          |       Status      |          -         |          Status bits          |

#### Status bits: 
| Position |      Message     | Description |
|:--------:|:----------------:|:-----------:|
|     7    |         -        |             |
|     6    |         -        |             |
|     5    |         -        |             |
|     4    |         -        |             |
|     3    |         -        |             |
|     2    |         -        |             |
|     1    |         -        |             |
|     0    |       Zero       | If set, throttle has gone to zero after startup |

<a name="0x1"/>

## BMS:

We did not had time to properly implement this code.

<a name="0x2"/>

## Fuel Cell:

Documents are under NDA.

<a name="0x3"/>

## Motor Controller PCB:

#### These messages are now broadcasted by the VESCs directly to the boat's CAN Bus, **they use EXTENDED CAN IDs**. To do a slightly modified version of VESC firmware is used so that the telemetry has the data we want.

### motor_current (CAN EXT ID: 0x901)
**LEN = 8**

|  Offset |    Type    |        Name       |          Scale           |        Description         |
|:-------:|:----------:|:-----------------:|:------------------------:|:--------------------------:|
|    0    |  int32_t   |  Motor_current    |			     1e2		        | Motor Current 	           |
|    4    |  int32_t   |Motor_input_current|           1e2            | VESC Motor Input Current   |


### motor_duty_rpm_voltage (CAN EXT ID: 0xE01)
**LEN = 8**

|  Offset |    Type    |        Name       |          Scale           |        Description         |
|:-------:|:----------:|:-----------------:|:------------------------:|:--------------------------:|
|    0    |  int16_t   |  Duty_cycle   	   |			      1e2           | VESC duty cycle            |
|    2    |  int32_t   |  RPM   		       |            1e0           | Motor RPMs     		         |
|    6    |  int16_t   |  Motor_voltage 	 |            1e1           | VESC input voltage         |


### motor_temperatures (CAN EXT ID: 0xF01)
**LEN = 8**

|  Offset |    Type    |        Name       |          Scale           |        Description         |
|:-------:|:----------:|:-----------------:|:------------------------:|:--------------------------:|
|    0    |  int16_t   | Temp_motor		     |		      	1e1           | Motor Temperature          |
|    2    |  int16_t   | Temp_mos_1    	   |            1e1           | VESC Mos 1 Temp.     	     |
|    4    |  int16_t   | Temp_mos_2        |            1e1           | VESC Mos 2 Temp.           |
|    6    |  int16_t   | Temp_mos_3	       |            1e1           | VESC Mos 3 Temp.           |


### motor_fault_id_asked_current (CAN EXT ID: 0x1001)
**LEN = 4**

|  Offset |    Type    |        Name       |          Scale           |        Description         |
|:-------:|:----------:|:-----------------:|:------------------------:|:--------------------------:|
|    0    |  uint8_t   | Fault 			       |					  -             | VESC Fault code            |
|    1    |  uint8_t   | ID          	     |            -             | VESC id      	             |
|    2    |  int16_t   | Asked_current     |            -             | Throttle asked current     |


<a name="0x5"/>

## Screen:
### new_sd_log_file (CAN ID: 0x605)
**LEN = 0**
Content does not matter, just send message with empty buffer.

<a name="0x6"/>

## CAB 500-C/SP5 Current Sensor (LEM):

For more information regarding this sensor messages check [this documents](https://gitlab.com/tecnico.solar.boat/2021/SM01/-/tree/main/Datasheets/LEM%20CAB500%20Current%20Sensor). **Be aware that we are not using the default CAN ID's shown on LEM documents.**

### CAB500_Ip frame (CAN ID: 0x606)
**LEN = 8**

<img src="Auxiliary%20Files/CAB500_IP_Frame.png"  width="60%">

<img src="Auxiliary%20Files/CAB500_Errors.png"  width="60%">


### UDS_CLIENT (CAN ID: 0x406)
**LEN = 8**

### UDS_SERVER (CAN ID: 0x506)
**LEN = 8**

<a name="0x7"/>

## Fuse Board:
### status (CAN ID: 0x607)
**LEN = 2**

|  Offset |   Type    |        Name       |     Scale     |           Description            |
|:-------:|:---------:|:-----------------:|:-------------:|:--------------------------------:|
|    0    |  int8_t  |     current_24    |      1e1      | current in 24V system in A       |
|    1    |  int8_t  |     current_48    |      1e1      | current in 48V system in A       | 

---

<a name="0x8"/>

## Power Box:

### PB_GPS (CAN ID: 0x618)
**LEN = 8**

| Offset |   Type  |    Name   |     Scale       |                  Description                  |
|:------:|:-------:|:---------:|:---------------:|:---------------------------------------------:|
|    0   | int32_t |  Latitude | 2<sup>+24</sup> |  Boat Latitude in decimal degrees format (DD) |
|    4   | int32_t | Longitude | 2<sup>+23</sup> | Boat Longitude in decimal degrees format (DD) |

### PB_Vel_Angle_Status (CAN ID: 0x628)
**LEN = 5**

| Offset |   Type  |  Name  | Scale |                   Description                   |
|:------:|:-------:|:------:|:-----:|:-----------------------------------------------:|
|    0   | int16_t |  Speed |  1e2  |                  Speed in knots                 |
|    2   | uint16_t|  Angle |   -   | Direction with north (0-360) - Counterclockwise |
|    4   |         | Status |   -   |                 Status bits                     |

#### Status messages:
| Position |      Message     | Description                  |
|:--------:|:----------------:|:----------------------------:|
|     7    |         -        |                              |
|     6    |         -        |                              |
|     5    |         -        |                              |
|     4    | KillSwitch_Sense | 1 means the killswitch is on |
|     3    |  FuelCell_Sense  | 1 means there is voltage     |
|     2    |  FuelCell_Relay  | 1 means Relay is closed      |
|     1    |   Motor_Relay    | 1 means Relay is closed      |
|     0    | PreCharge_Relay  | 1 means Relay is closed      |



<a name="0x9"/>

## H2 Board:
### HB_Temp_Humid (CAN ID: 0x619)
**LEN = 8**

| Offset |  Type   |       Name       | Scale |         Description        |
|:------:|:-------:|:----------------:|:-----:|:--------------------------:|
|    0   | uint8_t |    Temp_H2_In    |   -   |    Temperature H2 Inlet    |
|    1   | uint8_t |    Temp_H2_Out   |   -   |    Temperature H2 Outlet   |
|    2   | uint8_t |    Humid_H2_In   |   -   |      Humidity H2 Inlet     |
|    3   | uint8_t |   Humid_H2_Out   |   -   |     Humidity H2 Outlet     |
|    4   | uint8_t |    Temp_Air_In   |   -   |    Temperature Air Inlet   |
|    5   | uint8_t |   Humid_Air_In   |   -   |      Humidity Air Inlet    |
|    6   | uint8_t |  Temp_Cooling_In |   -   |  Temperature Cooling Inlet |
|    7   | uint8_t | Temp_Cooling_Out |   -   | Temperature Cooling Outlet |

### HB_Pressure_Status (CAN ID: 0x629)
**LEN = 8**

| Offset |   Type   |       Name       | Scale |        Description       |
|:------:|:--------:|:----------------:|:-----:|:------------------------:|
|    0   |  int8_t  |  Pressure_H2_In  |  1e1  | Pressure H2 Inlet        |
|    1   |  int8_t  | Pressure_H2O_Out |  1e1  | Pressure H2O Outlet      |
|    2   | uint16_t |    RPM_Blower    |   -   | RPMs of Air blower       |
|    4   |  int16_t |     Flow_Air     |  1e1  | Air flow Inlet           |
|    6   |  uint8_t |  Temp_Blower     |   -   | Temperature Air Blower   |
|    7   |          |      Status      |   -   | Status bits              |

#### Status messages:
| Position |        Message       | Description |
|:--------:|:--------------------:|-------------|
|     7    |           -          |             |
|     6    |           -          |             |
|     5    |           -          |             |
|     4    |           -          |             |
|     3    |           -          |             |
|     2    |  Status_H2O_Solen    | 1 means valve is open |
|     1    |  Status_H2_Solen     | 1 means valve is open |
|     0    |  Status_Cooling_Pump | 1 means pump is ON    |

#### Messages location on vessel
<img src="Auxiliary Files/SM01_Overview.png"  width="100%">


<a name="Código"/>


#### Requirements

- **[VSCode](https://code.visualstudio.com/)**
- **[Platformio](https://platformio.org/)**
- **[tonton81's FlexCAN_T4](https://github.com/tonton81/FlexCAN_T4)**


### Example

```c++
#include <Arduino.h>
#include "CAN_TSB_H2.h"
#if defined(__AVR_AT90CAN32__) || defined(__AVR_AT90CAN64__) || defined(__AVR_AT90CAN128__)
  #include "avr_can.h"
#endif

TSB_CAN canMessageHandler;
CAN_message_t to_send; // One for each CAN ID that needs to be sent 

#if defined(__IMXRT1062__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
  // Declare your CAN interface, CAN0 available for Teensy 3.2/5 CAN 0 and 1 for Teensy 3.6 
  // CAN 1, 2 and 3 avalible for Teensy 4.X
  FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> Can0; // <- This name can be whatever you like but this is the name used to send commands to the interface
#endif

void initCAN_Messages(){ // Inicialise the message fixed parameters, do it for all messages
  to_send.flags.extended = 0;
  to_send.id = 0x04;
  to_send.len = 4;
  // if you whant to be sure that messages are sent in the order they are generated set this to 1
  // otherwise you don't need to set it.
  // to_send.seq = 1; 
}

void sendCAN_Messages(){ // Call this function preodically to send the messages use IntervalTimer
  // Lets show how it works for sending the time
  unsigned long time_now = now(); // Get current time
  int32_t ind = 0; 
  buffer_append_uint32(to_send.buf, time_now, &ind); // Populate the message data 
  Can0.write(to_send); // Send message (call this for every message to be sent)
}

void setup(void){
	Serial.begin(9600);
	initCAN_Messages(); // Call the function to inicialise all you CAN Messages
	// Setup the CAN
	#if defined(__IMXRT1062__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
		Can0.begin(); // Inicialise CAN BUS
		Can0.setBaudRate(500000); // Define CAN BaudRate

		// FIFO allows ordered receptions, Mailboxes can receive in any order as long as a slot is empty. 
		// You can have FIFO and Mailboxes combined, for example, assign a Mailbox to receive a filtered ID in 
		// it's own callback, so a separate callback will fire when your critical frame is received. 
		// With FIFO alone you have only 1 callback essentially. 
		// In either case Teensy is fast enough to receive all critical frames without filters 
		// unless you are in non-interrupt mode or your loop code is causing delays...

		// Either use Mailboxes or FIFO if you are not using filters
		Can0.setMaxMB(16); // Teensy 3.X have only 16 Mailboxes per CAN Channel Teensy 4.X has 64
		Can0.enableMBInterrupts();

		// Can0.enableFIFO();
		// Can0.enableFIFOInterrupt();

	#elif defined(__AVR_AT90CAN32__) || defined(__AVR_AT90CAN64__) || defined(__AVR_AT90CAN128__)
		if( Can0.init(CAN_BPS_500K)){
			Serial.println("CAN Init OK!");
		}
		else{
			Serial.println("CAN Init Failed!");
		}
	#endif
	Can0.attachObj(&canMessageHandler);
	
	#if defined(__AVR_AT90CAN32__) || defined(__AVR_AT90CAN64__) || defined(__AVR_AT90CAN128__)  
		Can0.setNumTXBoxes(1);    // Use all MOb (only 6x on the ATmegaxxM1 series) for receiving.

		//standard  
		Can0.setRXFilter(1, 0, 0, false);       //catch all mailbox
		Can0.setRXFilter(2, 0, 0, false);       //catch all mailbox
		Can0.setRXFilter(3, 0, 0, false);       //catch all mailbox
		Can0.setRXFilter(4, 0, 0, false);       //catch all mailbox
		Can0.setRXFilter(5, 0, 0, false);       //catch all mailbox
	#endif
		canMessageHandler.attachGeneralHandler();

	#if defined(__IMXRT1062__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
		Can0.mailboxStatus(); // This prints the mailboxes status, is also works when using FIFO
	#endif
  
}

void loop(void)
{
  // If you want to print something available on the CAN Bus check the CAN_TSB.h to see the structs used.
  // As an example lets print the battery current
  Serial.println(canMessageHandler.bat.voltage);

  // To send mesages:
  sendCAN_Messages();
  delay(1000);
}
```

---
