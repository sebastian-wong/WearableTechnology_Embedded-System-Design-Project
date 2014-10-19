import rpiUartCommunication
from multiprocessing import Process, Queue
import numpy as np



class DataParser(object):
        step = Queue()
        compass = Queue()
        gyro = Queue()
        baro = Queue()
        ultrasound = Queue()
        IR = Queue()

        def location_input_type(input_type):
                voiceInput = False
                if input_type == 1:
                        voiceInput = True
                return voiceInput             
                        

        def request_readings(self, step, compass, gyro, baro, ultrasound, IR):
                data = rpiUartCommunication.get_sensor_data()

                #Steps for this second up to 5 steps: 1 byte
                #Compass bearings for those steps: 10 bytes
                #Gyro readings: 3 bytes
                #Barometer reading: 1 byte
                #Ultrasound readings: 5 bytes
                #IR readings: 5 bytes
                #Total right now: 25 bytes
                #ACK_A=[30,31,32,33,34,35,36,37]
                #Every two bytes: motor and their respective duration - left,right, forward,backward
                #data = [1, 30,0,0,0,0, 0,0,0, 0, 0,0,0,0,0,0, 0,0]
                step.put(data[0])

                compass_no = 10
                #compass_read = []
                compass_read = np.zeros((10,), dtype=np.uint8)
                for i in range(compass_no):
                        compass_read[i] = data[i+1]
                compass.put(compass_read)
                
                gyro_no = 3
                gyro_read = []
                for g in range(gyro_no):
                       gyro_read.append(data[g+compass_no+1])
                gyro.put(gyro_read)

                baro_no = 1
                baro_read = data[compass_no+gyro_no+1]
                baro.put(baro_read)

                ultrasound_no = 5
                ultrasound_read = []
                for u in range(ultrasound_no):
                        ultrasound_read.append(data[u+compass_no+gyro_no+baro_no+1])
                ultrasound.put(ultrasound_read)


                IR_no = 5
                IR_read = []
                for i in range(IR_no):
                        IR_read.append(data[i+compass_no+gyro_no+baro_no+ultrasound_no+1])
                IR.put(IR_read)

                
                
                #return acc_read, gyro_read, compass_read, baro_read, IR_read, ultrasound_read
                
        def __init__(self):
                p = Process(target= self.request_readings, args= (DataParser.step, DataParser.compass, DataParser.gyro, DataParser.baro, DataParser.ultrasound, DataParser.IR))
                p.start()
                p.join()

        def get_step_read(self):
                return self.step.get()

        def get_gyro_read(self):
                return self.gyro.get()

        def get_compass_read(self):
                print "in compass read"
                return self.compass.get()

        def get_baro_read(self):
                return self.baro.get()

        def get_IR_read(self):
                return self.IR.get()

        def get_ultrasound_read(self):
                return self.ultrasound.get()

if __name__ == '__main__':
        data_parser = DataParser()
        step = data_parser.get_step_read()
        compass = data_parser.get_compass_read()
        print step
        print compass
