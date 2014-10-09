import rpiarduart3

data = rpiarduart3.receive_data()

acc_read_n = data[0]
acc_read = []
for n in range(acc_read_n):
	acc_read.append(data[n+1])

gyro_read_m = data[acc_read_n+1]
gyro_read = []
for m in range(gyro_read_m):
        gyro_read.append(data[m+acc_read_n+2])

compass_read_c = data[acc_read_n+gyro_read_m+2]
compass_read = []
for c in range(compass_read_c):
        compass_read.append(data[c+gyro_read_m+acc_read_n+3])

baro_read_b = data[compass_read_c+gyro_read_m+acc_read_n+3]
baro_read = []
for b in range(baro_read_b):
        baro_read.append(data[b+compass_read_c+gyro_read_m+acc_read_n+4])


IR_read_i = data[baro_read_b+compass_read_c+gyro_read_m+acc_read_n+4]
IR_read = []
for i in range(IR_read_i):
        IR_read.append(data[i+baro_read_b+compass_read+gyro_read_m+acc_read_n+5])

ultrasound_read_u = data[IR_read_i+baro_read_b+compass_read+gyro_read_m+acc_read_n+5]
ultrasound_read = []
for u in range(ultrasound_read_u):
        ultrasound_read.append(data[u+IR_read_i+baro_read_b+compass_read+gyro_read_m+acc_read_n+6])

def get_acc_read():
        return acc_read

def get_gyro_read():
        return gyro_read

def get_compass_read():
        return compass_read

def get_baro_read():
        return baro_read

def get_IR_read():
        return IR_read

def get_ultrasound_read():
        return ultrasound_read
