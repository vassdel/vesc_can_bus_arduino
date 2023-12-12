import can 
import time 
import os #using command to define the bitrate to 50000

is_found = [False] * 7
data = [0.0] * 15
#for debugging
#name = ["battery_amp_hour", "battery_voltage", "battery_charge", 
#"battery_avg_temp","motor_rpm", "motorInVolt", 
#"motorConsumeWh", "motor_mosfet_temp","motor_temp", 
#"motor_ampere", "motor_input_current", "motor_duty_cycle",
#"battery_amp_hour", "motorChargedWh", "motorChargedAh"]

def index(x):
    if x == 0x6B0: #BMS
        return 0
    if x == 0x01B6E: #Tachometer, inputVolt
        return 5
    if x == 0x0106E: #MotorMosfetTemp, motorTemp, motorInputCurrent
        return 4
    if x == 0x096E: #motorRmp, motorcurrent, motordutycycle
        return 1
    if x == 0x0E6E: #motorcomsumeAh,motorChargedAh
        return 2
    if x == 0x0F6E: #motorconsumeWh, motorChargedWh
        return 3
    if x == 0x03A6E: # logika kinhthras;;
        return 6
    return -1

def convert(index_info, can_msg_data): #convert canMsdata to final double form and saves the value in data[index_info]
    global data
    global is_found

    if index_info == 1:
        motor_rpm = int.from_bytes(can_msg_data[0:4], byteorder='big', signed=True)
        motor_ampere = int.from_bytes(can_msg_data[4:6], byteorder='big', signed=True)
        motor_duty_cycle = int.from_bytes(can_msg_data[6:8], byteorder='big', signed=True)
        
        data[4] = motor_rpm * 1.0 
        data[9] = motor_ampere * 0.1
        data[11] = motor_duty_cycle * 0.001
        

    elif index_info == 4:
        motor_mosfet_temp = int.from_bytes(can_msg_data[0:2], byteorder='big', signed=True)
        motor_temp = int.from_bytes(can_msg_data[2:4], byteorder='big', signed=True)
        motor_input_current = int.from_bytes(can_msg_data[4:6], byteorder='big', signed=True)

        data[7] = motor_mosfet_temp * 0.1
        data[8] = motor_temp * 0.1          #we have no sensor connected
        data[10] = motor_input_current * 0.1


    elif index_info == 2:
        motorConsumeAh = int.from_bytes(can_msg_data[0:4], byteorder='big', signed=True)
        motorChargedAh = int.from_bytes(can_msg_data[4:8], byteorder='big', signed=True)

        data[12] = motorConsumeAh * 0.0001
        data[14] = motorChargedAh * 0.0001
        

        
    elif index_info == 3:
        motorConsumeWh = int.from_bytes(can_msg_data[0:4], byteorder='big', signed=True)
        motorChargedWh = int.from_bytes(can_msg_data[4:8], byteorder='big', signed=True)
        
        data[6] = motorConsumeWh * 0.0001
        data[13] = motorChargedWh * 0.0001
        

        
    elif index_info == 5:
        motorInVolt = int.from_bytes(can_msg_data[10:12], byteorder='big', signed=True)
        data[5] = motorInVolt * 0.1
        

        
    elif index_info == 0:
        battery_amp_hour = int.from_bytes(can_msg_data[0:2], byteorder='big', signed=True)
        battery_voltage = int.from_bytes(can_msg_data[2:4], byteorder='big', signed=True)
        battery_charge = int.from_bytes(can_msg_data[4:6], byteorder='big', signed=True)
        battery_avg_temp = int.from_bytes(can_msg_data[6:8], byteorder='big', signed=True)
        data[0] = battery_amp_hour * 0.1
        data[1] = battery_voltage * 0.1
        data[2] = battery_charge * 0.5
        data[3] = battery_avg_temp * 1.0   
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
def results_print():
    current_time = time.localtime()
    formatted_time = time.strftime("%H:%M:%S ", current_time)
    print(f"{formatted_time}", end="")
    for i in range(0, 15):
        print(data[i], end=" ") # for debugging add name[i], in print
    print()

def check_all_data():           #if we have all data it delays and sets is_found = true;
    global is_found
    for i in range(1,6):        #change 1 to 0 for BMS
        if not is_found[i]:     #if at least one info is unknown stop ckecking
            return
    is_found = [False] * 7
    results_print()
    time.sleep(0.2)

def main():
    os.system('sudo ip link set can0 up type can bitrate 500000') # without this the code doesnt run
    
    bus = can.interface.Bus(channel='can0', bustype='socketcan',bitrate=500000)
    
    print("------- CAN Read ----------")
    print("ID  DLC   DATA")

    global is_found
    is_found = [False] * 7
    try:
        while True:
            message = bus.recv(timeout=1.0)
            
            if message is not None:
                index_info = index(message.arbitration_id) #find message

                if index_info != -1:
                    is_found[index_info] = True             
                    convert(index_info, message.data)
                    check_all_data()
                    
    except KeyboardInterrupt:
        bus.shutdown()                   #close socket


if  __name__=="__main__":
    main()



