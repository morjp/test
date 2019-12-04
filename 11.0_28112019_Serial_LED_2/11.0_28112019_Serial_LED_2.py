import serial, time, structs
ser = serial.Serial('COM6', 9600)
time.sleep(2) #give the connection a second to settle

#send int 72 as byte to turn on LED
ser.write((struct.pack('>B', 72)))
time.sleep(2)
#send int 76 as byte to turn off LED
ser.write((struct.pack('>B', 76)))