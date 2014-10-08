import serial
import time

port = serial.Serial("/dev/ttyAMA0", baudrate=9600, timeout = 0.0)

HAND	 = 1
ACK	 = 2
NAK	 = 3
READ	 = 4
ACK_READ = 5
WRITE	 = 6
ACK_WRITE= 7

# ACK_S0 = 10
# ACK_S1 = 11
# ACK_S2 = 12
# ACK_S3 = 13
# ACK_S4 = 14
# ACK_S5 = 15
# ACK_S6 = 16
# ACK_S7 = 17
# ACK_S8 = 18
# ACK_S9 = 19

ACK_S = [10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
ACK_A = [20, 21, 22, 23, 24, 25, 26, 27, 28, 29]

# ACK_A0 = 20
# ACK_A1 = 21
# ACK_A2 = 22
# ACK_A3 = 23
# ACK_A4 = 24
# ACK_A5 = 25
# ACK_A6 = 26
# ACK_A7 = 27
# ACK_A8 = 28
# ACK_A9 = 29

connectionStatus = -1
dataCorrupted 	 = -1
divisor		 = 17 
timeout 	 = 0


#sensorData     = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
sensorData = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

actuatorData   = [95, 96, 97, 98, 99, 100, 100, 101, 102, 103]


#transfer data to Arduino
def transfer(send, expected):
	global timeout
	dataReceived = '-1'
	timeout = 0
	while dataReceived != expected and timeout == 0:
		port.write(chr(send))
        	print(chr(send+48)+" Sent to Arduino")
		start = time.time()
		end = start
		while end-start < 1 and dataReceived != expected:
			dataReceived = port.read(1)
			end = time.time()
		
		if end-start > 1:
			timeout = 1
			
#Establish Initial connection 
def start_connection() :
	global connectionStatus
	global timeout
	transfer(HAND, chr(ACK))
	
	if timeout == 1:
		connectionStatus = 0
		timeout = 0
	else :
		print('ACK received')
		port.write(chr(ACK))
		print('ACK sent')
		connectionStatus = 1
		print('---Connected---')

def receive_data():
	request_sensor_data()
	global sensorData
	index = 0;
	checksum = 0;
	
   	if connectionStatus == 1:
		while index != (10+1):
			sensorValue = port.read(1)
		        if sensorValue and index < 10  :
        			sensorData[index] = ord(sensorValue)
	                port.write(chr(ACK_S[index]))
        	        index = index + 1
			elif sensorValue and index == 10:
 #               checksum = ord(sensorValue)
				index = index + 1
					
        print(sensorData)
#        print("checksum is : ")
#        print(checksum)

#Request for sensor data
def request_sensor_data(): 
	global connectionStatus
	global dataCorrupted
	global timeout
	transfer(READ, chr(ACK_READ))

	if timeout == 1:
		connectionStatus = 0
		timeout = 0
	else :
		print("ACK_READ received") 
		connectionStatus = 1


# def sendSensorAck(index):
	# port.write(chr(ACK_S[index]))		


def send_data():
	request_write()
	index = 0	
	port.write(chr(actuatorData[index]))
	index = index + 1

	while index != (10+1):
		actuator_ack = port.read(1)
		if actuator_ack :
			if ord(actuator_ack) == ACK_A[9]:
				checkSum = compute_actuator_checksum()
				port.write(chr(checkSum))
#				print(checkSum)

			elif actuator_ack == chr(ACK_A[index-1]):
				port.write(chr(actuatorData[index]))
			index = index + 1



#Send actuator data values
def request_write():
	global connectionStatus
	global timeout
	transfer(WRITE, chr(ACK_WRITE))
	
	if timeout == 1:
		connectionStatus = 0
		timeout = 0
	else :
		print("ACK_WRITE received")
		connectionStatus = 1


#Computing checksum before sending actuator data
def compute_actuator_checksum():
	global divisor
	CheckSum = 0
	remainder = 0
	for index in range (0, 10):
		remainder = actuatorData[index] % divisor
		CheckSum = CheckSum + remainder
		
	return CheckSum

while connectionStatus != 1:
    start_connection()
time.sleep(2)
receive_data()
time.sleep(2)
send_data()
