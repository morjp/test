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


#sends 4-byte packet over serial bus; format: start byte (DEC 42), command byte, value byte, end byte (DEC 255)
def sendPacket(command, value):
    #check if arguments are going to fit in one byte, returns False if not; does not check for validity of command
    if command in range(0,254) and value in range(0,254): #both cannot contain 255 (end byte)
        #write packet to serial bus in bytes format
        ser.write((struct.pack('>B', 42))) #write start byte (DEC 42)
        ser.write((struct.pack('>B', command)))
        ser.write((struct.pack('>B', value)))
        ser.write((struct.pack('>B', 255))) #write end byte (DEC 255)

        #waits for response and returns it; possible responses are: 
        #ACK (acknowledged: command is known and will be executed)
        #NACK (not acknowledged: packet is invalid or command is not known)
        #IF NO RESPONSE IS RECEIVED AFTER 20 SECONDS, BECAUSE THE CLIENT IS CURRENTLY EXECUTING OTHER CODE,
        #BSY (busy: client is busy) IS RETURNED
        #THE CLIENT WILL THEN NOT EXECUTE THE COMMAND
        #THUS THE CIENT CANNOT PERFORM ACTIONS BLOCKING CODE LONGER HAN 20 SECONDS (or whatever the timout parameter is set to)
        response = ser.readline().decode('utf-8') #wait for response, decode from byte to ASCII
        if response == "": #no response, return BSY
            return "BSY"
        else: #response received, return response
            return response
        ser.reset_input_buffer() #clean the input buffer
        #ser.reset_output_buffer()
    else:
        return False



returns = sendPacket(0, 5)
print(returns)
time.sleep(0.1)
returns = sendPacket(0, 3)
print(returns)
time.sleep(1)
returns = sendPacket(0, 3)
print(returns)
time.sleep(1)


#TODO: Change timeout to real value