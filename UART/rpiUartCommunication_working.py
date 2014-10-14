import serial
import time

port = serial.Serial("/dev/ttyAMA0", baudrate=9600, timeout = 0.0)

connectionStatus = 0
timeout = 0

SYN = 1
ACK = 2
NAK = 3
READ = 4
ACK_READ = 5
WRITE = 6
ACK_WRITE = 7
ACK_S_CHECKSUM = 8
receivedSensorData = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
actuatorData = [50,100,150,200,1,2,3,4]
ACK_S = [10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28]
ACK_S_SUCCESS = 100
ACK_S_FAILURE = 101
ACK_A = [30,31,32,33,34,35,36,37]
ACK_A_SUCCESS = 240
ACK_A_FAILURE = 241
#Steps for this second up to 5 steps: 1 byte
#Compass bearings for those steps: 5 bytes
#Gyro readings: 3 bytes
#Barometer reading: 1 byte
#Ultrasound readings: 6 bytes
#IR readings: 2 bytes
#Total right now: 18 bytes
#ACK_A=[30,31,32,33,34,35,36,37]
#Every two bytes: motor and their respective duration - left,right, forward,backward

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

def establish_connection():
	global connectionStatus
	global timeout
	send_data(SYN, chr(ACK))

	if timeout == 1:
        	connectionStatus = 0
        	timeout = 0

    	else:
        	print('Received ACK.')	
        	port.write(chr(ACK))
	        print('Sent ACK.')	
	        connectionStatus = 1
	        print('---Connection Established---')

def get_sensor_data():
	global connectionStatus
	global timeout
	global ACK_S
	global receivedSensorData
	sensorValue=0
	index = 0
	checksum = -1
	send_data(READ, chr(ACK_READ))
    
	if timeout == 1:
		connectionStatus = 0
		timeout = 0
	else:
		print("ACK_READ received")
		connectionStatus = 1
#Once read connection and status has been established
        	while index < 19:
			sensorValue = port.read(1)
			if sensorValue:
				if index == 18:
					port.write(chr(ACK_S_CHECKSUM))
					checksum = ord(sensorValue)
					index = index + 1
				else:
					receivedSensorData[index]=ord(sensorValue)
					port.write(chr(ACK_S[index]))
					index = index + 1
#	    		if index == 19:
#				#sensorValue = port.read(1)
#				port.write(ACK_S_CHECKSUM)
#				checksum = ord(sensorValue)
#				index = index + 1
		print(receivedSensorData)
		print("Checksum is: ")
		print(checksum)
		if sensor_checksum() == checksum:
			return 0
		elif sensor_checksum() != checksum:
			return 1	#Up to caller to decide whether to try again.

def send_actuator_data():
	global connectionStatus
	global timeout
	index = 0
	send_data(WRITE, chr(ACK_WRITE))
	if timeout == 1:
		connectionStatus = 0
		timeout = 0
	else:
		print("ACK_WRITE received")
		connectionStatus = 1
#Once write connection and status has been established
		port.write(chr(actuatorData[index]))
        	while index < 8:
#			port.write(chr(actuatorData[index]))
			actuator_ack = port.read(1)
			if actuator_ack:
				print("Actuator_ack expected is: " + chr(ACK_A[index]))
				print("Actuator_ack is: " + actuator_ack)
    				if actuator_ack == chr(ACK_A[index]) and index != 7:
	    				index = index + 1
					port.write(chr(actuatorData[index]))
				elif actuator_ack == chr(ACK_A[index]) and index == 7:		
					index = index+1
					checkSum = actuator_checksum()
					port.write(chr(checkSum)) #send checksum
		while index == 8: #ACK_A7 Acknowledged
			print("Index 7!")
#			port.write(chr(checkSum)) #send checksum
			actuator_ack = port.read(1)
                        if actuator_ack:
				print("Checksum status is: ")
				print(actuator_ack)
				if ord(actuator_ack) == ACK_A_SUCCESS:
					print(actuator_ack + "is success")
					index = index + 1
				#	return 0
				elif ord(actuator_ack) == ACK_A_FAILURE:
					print(actuator_ack + "is failure")
					index = index + 1
				#	return 1	#Up to the caller to repeat the send.
				else:
					print("wo bu yaoooooo")
					index = index +1
def sensor_checksum():
	index = 0
	sumValues = 0
	while index < 18:
		sumValues = sumValues + receivedSensorData[index]
		index = index + 1
	sumValues = sumValues % 17
	return sumValues

def actuator_checksum():
	index = 0
	checksum = 0
	remainder = 0
	while index < 8:
		remainder = actuatorData[index] % 17
		checksum = checksum + remainder
		index = index + 1
	return checksum

while connectionStatus != 1:
	establish_connection()
read_sensor = get_sensor_data()
#get_sensor_data()
time.sleep(2)
write_act = send_actuator_data()
#send_actuator_data()
