#ATmega 328P Serial Protocol Host
#Version 0.1
#Compatible with client version 0.1
#Created 02/12/2019 by Moritz Prenzlow <m.prenzlow@uni-koeln.de>
#
#This host application can send packets over the serial bus to be received from the ATmega 328P and waits for a response.
#Packet structure: START BYTE (DEC 42), COMMAND BYTE, VALUE BYTE, END BYTE (DEC 255).
#
#sendPacket(int command, int value) 
#   Arguments:     int value, int command; in range 0<=x<=254 
#   Returns:       string response


import serial, time, struct
import warnings
import serial.tools.list_ports

#searches for arduino in connected serial devices, opens serial connection if arduino is found
arduino_ports = [
    p.device
    for p in serial.tools.list_ports.comports()
    if 'Arduino' in p.description
]
if not arduino_ports:
    raise IOError("No Arduino found")
if len(arduino_ports) > 1:
    warnings.warn('Multiple Arduinos found - using the first')

ser = serial.Serial(arduino_ports[0], 9600, timeout=2)
time.sleep(3)


#sends 4-byte packet over serial bus
def sendPacket(command, value):
    #check if arguments are going to fit in one byte, returns False if not; does not check for validity of command
    if command in range(0,255) and value in range(0,255): #both cannot contain 255 (end byte)
        #write packet to serial bus in bytes format
        ser.write((struct.pack('>B', 42))) #write start byte (DEC 42)
        ser.write((struct.pack('>B', command)))
        ser.write((struct.pack('>B', value)))
        ser.write((struct.pack('>B', 255))) #write end byte (DEC 255)

        #waits for response and returns it; possible responses are: 
        #
        #ACK (acknowledged: command is known and will be executed)
        #
        #NACK (not acknowledged: packet is invalid or command is not known
        #
        #IF NO RESPONSE IS RECEIVED AFTER 20 SECONDS, BECAUSE THE CLIENT IS CURRENTLY EXECUTING OTHER CODE,
        #BSY (busy: client is busy) IS RETURNED, THE FUCTION WILL BLOCK CODE FOR THOSE 20 SECONDS
        #THE CLIENT WILL THEN NOT EXECUTE THE COMMAND
        #THUS THE CIENT CANNOT PERFORM ACTIONS BLOCKING CODE LONGER HAN 20 SECONDS (or whatever the timeout parameter is set to) WITHOUT PROPER HANDLING BY THE HOST
        response = ser.readline().decode('utf-8') #wait for response, decode from byte to ASCII
        if response == "": #no response, return BSY
            return "BSY"
        else: #response received, return response
            return response
        ser.reset_input_buffer() #clean the input buffer
        #ser.reset_output_buffer()
    else:
        return False


######## Examples ########

#A sample function that blinks a LED on pin 13 can be called with the command 0, values 5 to 20 (amount of blinks).
#A sample function that lights a LED on pin 12 can be called with the command 1, values 1 to 254 (seconds)

#blinks LED 10 times, returns ACK
print("Command: 0   Value: 10")
returns = sendPacket(0, 10)
print(returns)
time.sleep(5)

#returns NACK, because minimum value for command 0 is 5
print("Command: 0   Value: 4")
returns = sendPacket(0, 4)
print(returns)
time.sleep(5)

#lights LED 10 seconds
print("Command: 1   Value: 10")
returns = sendPacket(1, 10)
print(returns)
time.sleep(0.2) #not enough sleep to let client complete action

#not enough time, no response in timeout=2 seconds => returns BSY and packet is discarded on client side
print("Command: 0   Value: 7")
returns = sendPacket(0, 7)
print(returns)
time.sleep(5)

#returns False, because arguments are bigger than 1 byte
print("Command: 1   Value: 256")
returns = sendPacket(1, 256)
print(returns) 

#TODO: -Change timeout to real value
#      -Set wait time after opening serial port to appropriate value to leave room for homing
#       so no BSY is returned already on the first packet 