import serial

ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=1)
ser.reset_input_buffer()

while True:
	if ser.in_waiting > 0:
		
		ard_data = ser.readline().decode('utf-8').rstrip()
		
		print(ard_data)
