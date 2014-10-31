import serial
import time

port = serial.Serial("/dev/ttyAMA0", baudrate=9600, timeout = 0.0)

connectionStatus = 0
timeout = 0

HANDSHAKE = 1
ACK = 2
NAK = 3
READ = 4
ACK_READ = 5
ACK_S_CHECKSUM = 8
NACK_S_CHECKSUM = 46
NUM = 9
ACK_NUM = 40
NACK_NUM = 41;
ACK_NUM_CHECKSUM = 42;
NACK_NUM_CHECKSUM = 43;
VOICE = 44;
ACK_VOICE = 45;
receivedSensorData = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
numpadData = []
numpadDatacopy = []
ACK_S = [10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28, 29, 30, 31, 32, 33, 34]
ACK_S_SUCCESS = 100
ACK_S_FAILURE = 101

sensor_data_size = 25
#Steps for this second up to 5 steps: 1 byte
#Compass bearings for those steps: 10 bytes #2 bytes = 1 compass reading
#Gyro readings: 3 bytes
#Barometer reading: 1 byte
#Ultrasound readings: 5 bytes
#IR readings: 5 bytes
#Total right now: 25 bytes

#send data to arduino, with expected readings
#timeout if reading not equal to expected reading
def send_data(send, expected):
	global timeout
        charReceived='-1'
        timeout = 0
        while charReceived != expected and timeout == 0:
		print(chr(send+48)+" sent to Arduino")	#debugging purposes.
                port.write(chr(send))
                start = time.time()
                end = start
                while end-start < 1 and charReceived != expected:
                    charReceived = port.read(1)
                    end = time.time()
                if end-start > 1:
                    timeout = 1

#3 way handshake with Arduino
def establish_connection():
	global connectionStatus
	global timeout
	send_data(HANDSHAKE, chr(ACK))

	if timeout == 1:
        	connectionStatus = 0
        	timeout = 0

    	else:
        	print('ACK received.')	
        	port.write(chr(ACK))
	        print('Sent ACK.')	
	        connectionStatus = 1
	        print('---Connected---')

#to get type of input for location
#if user requested voice input, return 1
#if user requested keypad input, return 2
def location_input():
	loc = 0
#	while loc != 1:
	request = port.read(1)
	if request:
       		if ord(request) == VOICE:
                        port.write(chr(ACK_VOICE))
               	        print("voice activated")
 	    	        loc = 1
			return 1
                elif ord(request) == NUM:
       	                print("reading numpad")
			loc = 1
			return 2
	else:
		print("no input")
		return 0

#to calculate checksum from the received list of keypad inputs
def verifyNumChecksum(length):
	index = 0
	checksum = 0
	remainder = 0
	while index < length:
		remainder = numpadData[index]%17
		checksum = checksum + remainder
		index = index + 1
	return checksum

#to get keypad input
#to be called if user uses keypad for input
def get_numpad_input():
	global loc
	global numpadData
	input_num = 0
	characterReceived = 0
	num_received = 0
	numpadData = []

	port.write(chr(ACK_NUM))
	while not num_received :
		input_num_temp = port.read(1)
		print("reading num input")
		if input_num_temp:
			input_num = ord(input_num_temp)
			num_received = 1
			print("numbers of input from keypad is: ")
			print(input_num)
	print("input is:")
	while characterReceived < input_num:
		numpad_input = port.read(1)
		print("reading input:")
		if numpad_input:
			numpadData.append(ord(numpad_input)-48)
			print(numpadData[characterReceived])
			characterReceived = characterReceived + 1
	while characterReceived == input_num:
		checksum = port.read(1)
		if checksum:
			print("Checksum: ")
			print(ord(checksum))
			if verifyNumChecksum(input_num) == ord(checksum):
				port.write(chr(ACK_NUM_CHECKSUM))
				loc = 1
				print(loc)
				print("ACK_NUM_CHECKSUM sent")
				characterReceived = characterReceived + 1
			else:
				port.write(chr(NACK_NUM_CHECKSUM))
				print("NACK_NUM_CHECKSUM")
				characterReceived = characterReceived + 1
	numpadDatacopy = numpadData
	return numpadDatacopy

#to request sensor data from Arduino
#return array of received sensor data
def get_sensor_data():
	global connectionStatus
	global timeout
	global ACK_S
	global receivedSensorData
	global sensor_data_size
	sensorValue=0
	index = 0
	checksum = -1
	send_data(READ, chr(ACK_READ))
    
	if timeout == 1:
		connectionStatus = 0
		timeout = 0
		print("time out")

	else:
		print("ACK_READ received")
		connectionStatus = 1
		#Once read connection and status has been established
		start = time.time()
		end = start
	        while index < sensor_data_size+1 and end-start < 1:
	#	while index < sensor_data_size+1:
			sensorValue = port.read(1)
			if sensorValue:
		#		print("Sensor value received: " + sensorValue)
				if index == sensor_data_size:
				#	port.write(chr(ACK_S_CHECKSUM))
					checksum = ord(sensorValue)
					print (checksum)
					if sensor_checksum() == checksum:
						port.write(chr(ACK_S_CHECKSUM))
						print(receivedSensorData)
						print("ACK_S_CHECKSUM sent")
					else:
						port.write(chr(NACK_S_CHECKSUM))
						print("original: ")
						print(receivedSensorData)
						receivedSensorData = [-1]*sensor_data_size
						print(receivedSensorData)
						print("NACK_S_CHECKSUM sent")
					index = index + 1
				else:
				#	print(ord(sensorValue))
					receivedSensorData[index]=ord(sensorValue)
					port.write(chr(ACK_S[index]))
					index = index + 1
				start = time.time()
				end = start
			else:
				end = time.time()
		print(receivedSensorData)
		print("Checksum is: ")
		print(checksum)
	return receivedSensorData

#to calculate checksum for received sensor data
def sensor_checksum():
	index = 0
	remainder = 0
	checksum = 0
	while index < sensor_data_size:
		remainder = receivedSensorData[index]%17
		checksum = checksum + remainder
		index = index + 1
	print("calculated checksum is:")
	print(checksum)
	return checksum


#test code
#handshake
#location = 0
while connectionStatus != 1:
	establish_connection()
#	while location == 0:
#		location = location_input()
#		if location == 1:
#			print("voice activated(outside)")
#		elif location == 2:
#			print("numpad input")
#			data = list(get_numpad_input())
#			print(data)
#	read_sensor = list(get_sensor_data())
#	print (read_sensor)
#	time.sleep(1)
