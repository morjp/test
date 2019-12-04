import serial, time, struct

ser = serial.Serial('COM6', 9600)
time.sleep(3) #give the connection a second to settle

ser.write((struct.pack('>B', 42)))

#print (ser.readline())
#time.sleep(1)
#print("Waiting Bytes:", ser.inWaiting())

reading = ser.readline().decode('utf-8') #ser.read(ser.inWaiting()).decode('utf-8') 
print(reading)


time.sleep(1)
ser.close()
