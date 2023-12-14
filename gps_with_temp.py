import board
import serial
import adafruit_gps
import time
import csv
from datetime import datetime
from datetime import date
import subprocess
import math
from math import sin, cos, sqrt, atan2, radians
import RPi.GPIO as GPIO
import os 

try:
    old_fn = 'data.csv'
    new_fn = 'data_'+str(date.today())+'_'+str(datetime.now().strftime("%H-%M-%S"))+'.csv'
    #os.rename('/Users/vassdel/Documents/Raspberry/Oceanos/'+old_fn,'/Users/vassdel/Documents/Raspberry/Oceanos/data_history/'+new_fn) #pc
    os.rename('/home/oceanos/'+old_fn,'/home/oceanos/history/'+new_fn)
except:
    pass

USE_UART = True
BUTTON_PIN = 18
GPIO.setmode(GPIO.BCM)
GPIO.setup(BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
# Create a serial connection for the GPS connection.
uart = serial.Serial(port = '/dev/tty1', baudrate=9600, timeout=1)
ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=1)
ser.reset_input_buffer()

# Create a GPS module instance.
gps = adafruit_gps.GPS(uart, debug=False)  # Use UART/pyserial

# Set update rate to 1000 milliseconds (1Hz)
gps.send_command(b"PMTK220,250")

# Ask for specific data to be sent.
#                          A B C D E F G H                   I
gps.send_command(b'PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0')

#   A - send GLL sentences
#   B - send RMC sentences
#   C - send VTG sentences
#   D - send GGA sentences
#   E - send GSA sentences
#   F - send GSV sentences
#   G - send GRS sentences
#   H - send GST sentences
#   I - send ZDA sentences

# The number is how often to send the sentence compared to the update frequency.
# If the update frequency is 500ms and the number is 5, it will send that message
# every 2.5 seconds.
last_print = time.monotonic()
command = "/usr/bin/vcgencmd measure_temp"

lat = 0
longt = 0
speed = 0.00
speed_km_h = 0
rtc = "start"
millis = 0
rpm = 0
input_voltage = 0
motor_watt_hours = 0
motor_tempMosfet = 0
motor_tempMotor = 0
motor_current = 0
battery_current = 0
motor_dutyCycle = 0
motor_error = 0
rasp_temp = "0"
previous_latitude = None
previous_longitude = None
miles = 0
miles1 = 0
km = 0
distance_miles_naut = 0
distance_km = 0
R = 6373.0
battery_ampere = 0
battery_voltage = 0
charge = 0
battery_temperature = 0
miles_lap1 = 0
miles_lap = 0
autonomy = 0
autonomy1 = 0


fieldnames = ["time","latitude","longitude","speed","miles","miles_lap", "rtc","millis","rpm","input_voltage","motor_watt_hours","motor_tempMosfet","motor_tempMotor","motor_current","battery_current","motor_dutyCycle","motor_error","rasp_temp", "battery_ampere", "battery_voltage", "charge", "battery_temperature", "autonomy"]
with open('data.csv', 'w') as csv_file:
    csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
    csv_writer.writeheader()


while True:
    with open('data.csv', 'a') as csv_file: #open test1 in append mode, keep appending to the csv
        csv_writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        current_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        data = {"time": current_time, "latitude": lat, "longitude": longt, "speed": speed, "miles": miles, "miles_lap": miles_lap, "rtc": rtc, "millis": millis, "rpm": rpm, "input_voltage": input_voltage, "motor_watt_hours": motor_watt_hours, "motor_tempMosfet": motor_tempMosfet, "motor_tempMotor": motor_tempMotor, "motor_current": motor_current, "battery_current": battery_current, "motor_dutyCycle": motor_dutyCycle, "motor_error": motor_error, "rasp_temp": rasp_temp, "battery_ampere": battery_ampere, "battery_voltage": battery_voltage, "charge": charge, "battery_temperature": battery_temperature, "autonomy": autonomy}
        csv_writer.writerow(data)
        csv_file.close()
    while True:
        gps.update()
        current = time.monotonic() # antistoixo me ta millis
        button_state = GPIO.input(BUTTON_PIN)
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        if ser.in_waiting > 0:
            
            try:
                ard_data = ser.readline().decode('utf-8').rstrip()
                #print(ard_data)
                ard_data_list = ard_data.split("#")
                #print(ard_data_list)
                rtc = str(ard_data_list[0])
                millis = str(ard_data_list[1])
                battery_ampere = str(ard_data_list[2])
                battery_voltage = str(ard_data_list[3])
                charge = str(ard_data_list[4])
                battery_temperature = str(ard_data_list[5])
                rpm = str(ard_data_list[6])
                input_voltage = str(ard_data_list[7])
                motor_watt_hours = str(ard_data_list[8])
                motor_tempMosfet = str(ard_data_list[9])
                motor_tempMotor = str(ard_data_list[10])
                motor_current = str(ard_data_list[11])
                battery_current = str(ard_data_list[12])
                motor_dutyCycle = str(ard_data_list[13])
                motor_error = str(ard_data_list[14])
                if battery_ampere != " " and battery_voltage != " ":
                    if float(battery_ampere) != 0 and float(battery_voltage) !=0:  
                        autonomy1 = (float(charge)*100)/(float(battery_ampere) * float(battery_voltage))
                        autonomy = round(autonomy1)
                else:
                    autonomy = " "
            except:
                pass
            
            
            
        if result.returncode == 0 and current - last_print>= 0.25:
            output = result.stdout.strip()
            output1 = (output.replace('temp=', ''))
            output2 = (output1.replace("'C", ''))
            output3 = int(float(output2))
            print("Temperature:", output3)
            rasp_temp = output3
            #rasp_temp = 0
            
            
        #elif result.returncode != 0 and current - last_print>= 0:
            #error = result.stderr.strip()
            #print("Error:", error)
            #rasp_temp = " "
            #pass
            
        if button_state != GPIO.HIGH and current - last_print >=0.25:
            miles_lap1 = 0
            print("button pressed")
            

        if current - last_print >= 0.25:
            last_print = current
            # A valid sentence was received - do something
            if gps.has_fix:
                lat = gps.latitude #dd latitude
                longt = gps.longitude
                speed1 = gps.speed_knots
                speed = round(speed1, 1)
                #print(lat)
                #print(lat_new)
                print("speed: ", speed)
                #print(lat_new)

                speed_km_h = (gps.speed_knots * 1.852)
                print("Speed in km/h: ", speed_km_h)
                if previous_latitude is not None and previous_longitude is not None:
                    lat1 = radians(previous_latitude) #lat meta apo metatroph
                    lat2 = radians(lat)
                    lon1 = radians(previous_longitude)
                    lon2 = radians(longt)
                   
                    dlon = lon2 - lon1
                    dlat = lat2 - lat1
                    a = sin(dlat / 2)**2 + cos(lat1) * cos(lat2) * sin(dlon / 2)**2
                    c = 2 * atan2(sqrt(a), sqrt(1 - a))
                    distance_km = R * c
                    km += distance_km
                    distance_miles_naut = R * c * 0.539968
                    
                    miles1 += distance_miles_naut
                    miles_lap1 += distance_miles_naut
                    miles = round(miles1, 1)
                    miles_lap = round(miles_lap1, 1)
                previous_latitude = lat
                previous_longitude = longt
               
                print("distance in km: ", km)
                print("distance miles: ", miles)
                break
            else:
                lat = " "
                longt = " "
                speed = -1
                print("Waiting for a fix...")
                break
