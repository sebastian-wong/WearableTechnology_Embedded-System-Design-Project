#import rpiarduart3
from multiprocessing import Process, Queue

class DataParser(object):
        acc = Queue()
        gyro = Queue()
        baro = Queue()
        compass = Queue()
        IR = Queue()
        ultrasound = Queue()

        def request_readings(self, acc, gyro, baro, compass, IR, ultrasound):
                #data = rpiarduart3.receive_data()
                data = [1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10]

                acc_read_n = data[0]
                acc_read = []
                for n in range(acc_read_n):
                        acc_read.append(data[n+1])
                acc.put(acc_read)

                gyro_read_m = data[acc_read_n+1]
                gyro_read = []
                for m in range(gyro_read_m):
                        gyro_read.append(data[m+acc_read_n+2])
                gyro.put(gyro_read)

                compass_read_c = data[acc_read_n+gyro_read_m+2]
                compass_read = []
                for c in range(compass_read_c):
                        compass_read.append(data[c+gyro_read_m+acc_read_n+3])
                compass.put(compass_read)

                baro_read_b = data[compass_read_c+gyro_read_m+acc_read_n+3]
                baro_read = []
                for b in range(baro_read_b):
                        baro_read.append(data[b+compass_read_c+gyro_read_m+acc_read_n+4])
                baro.put(baro_read)


                IR_read_i = data[baro_read_b+compass_read_c+gyro_read_m+acc_read_n+4]
                IR_read = []
                for i in range(IR_read_i):
                        IR_read.append(data[i+baro_read_b+compass_read_c+gyro_read_m+acc_read_n+5])
                IR.put(IR_read)

                ultrasound_read_u = data[IR_read_i+baro_read_b+compass_read_c+gyro_read_m+acc_read_n+5]
                ultrasound_read = []
                for u in range(ultrasound_read_u):
                        ultrasound_read.append(data[u+IR_read_i+baro_read_b+compass_read_c+gyro_read_m+acc_read_n+6])
                ultrasound.put(ultrasound_read)
                
                #return acc_read, gyro_read, compass_read, baro_read, IR_read, ultrasound_read
                
        def __init__(self):
                p = Process(target= self.request_readings, args= (DataParser.acc, DataParser.gyro, DataParser.baro, DataParser.compass, DataParser.IR, DataParser.ultrasound))
                p.start()
                p.join()

        def get_acc_read(self):
                return self.acc_read.get()

        def get_gyro_read(self):
                return self.gyro_read.get()

        def get_compass_read(self):
                print "in compass read"
                return self.compass.get()

        def get_baro_read(self):
                return self.baro_read.get()

        def get_IR_read(self):
                return self.IR_read.get()

        def get_ultrasound_read(self):
                return self.ultrasound_read.get()

if __name__ == '__main__':
        data_parser = DataParser()
        sample = data_parser.get_compass_read()
        print sample
