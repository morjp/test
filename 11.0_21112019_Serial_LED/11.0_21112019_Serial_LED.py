import serial, time
ser = serial.Serial('COM6', 9600)
time.sleep(2) #give the connection a second to settle

#send number as byte for LED to blink this amount
print("sending", b'10')
ser.write(b'10')

while True:
    ser.readline()