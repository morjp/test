/*
ATmega 328P Serial Protocol Client 
Version 0.1
Compatible with host version 0.1
Created 02/12/2019 by Moritz Prenzlow <m.prenzlow@uni-koeln.de>

This client receives a serial packet from the host using the function receivePacket().
Packet structure: START BYTE (DEC 42), COMMAND BYTE, VALUE BYTE, END BYTE (DEC 255).

It checks for validity of the packet and if command and value are known.
The packet is always stored in inData[], but a flag is set if the packet is valid and the data can be used.

A sample function that blinks a LED on pin 13 can be called with the command 0, values 5 to 20 (amount of blinks).
A sample function that lights a LED on pin 12 can be called with the command 1, values 1 to 254 (seconds)
*/

int inData[4]; // data buffer
int index = 0; // data buffer index
// packet flags
bool isReceiving = false; // true if currently receiving packet
bool isBusy = false; // true if packet has been received and is being processed
bool isOk = false; // true if packet is valid

// arrays for packet validation
const int numberOfCommands = 2;
int validCommandRange[2] = {0,1}; // range of valid commands (all commands in validCommandRange[0] <= x <= validCommandRange[1] will be accepted
int validValueRanges[numberOfCommands][2] = { {5,20},
                                              {1, 10} }; // valid value range for each command (row 0 contains value range for the command in validCommandRange[0], row 1 for command in validCommandRange[1]...)

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT); // for sample functions
}

void loop() {
  receivePacket();

  if (!isReceiving && isBusy && isOk) { // packet stored in inData is fully received and valid, so command stored in it can now be executed

    /////// execution of sample functions ///////
    
    // example: when the command byte is 0, blinkLED() is executed with the value byte as a parameter
    if (inData[1] == 0) {
      blinkLED(inData[2]);
    } 

    if (inData[1] == 1) {
      lightLED(inData[2]);
    }

    ///////////////////////////////////////////

    serialFlush(); // flush serial buffer to discard everything that has been send during execution of command
    isBusy = false; // packet is discarded and next incoming one is processed
  }
 
}

// receives packet sent by host from serial buffer, checks for validity and if command and value are known, then sends ACK/NACK response
void receivePacket() {
  // reads packet from serial buffer, stores it in inData
  if (Serial.available() > 0) {

    int inByte = Serial.read(); // read the first available ("oldest") byte from the serial buffer

    if (!isBusy && !isReceiving && inByte == 42) { // look for start byte (DEC 42), but only when currently not receiving and not busy
      //TODO: clear data buffer I don´t think we need to do that
      // reset flags
      isReceiving = true;
      isOk = false;
      index = 0; // reset data buffer index
    } 
    else if (!isBusy && isReceiving && inByte == 255) { // look for end byte (DEC 255), but only when currently receiving
      index += 1;
      inData[index] = inByte; // put end byte in data array
      isReceiving = false;
      isBusy = true; // packet has been received and is now stored in inData
    }
    else if (!isBusy && isReceiving) { // if inByte is neither start or end byte, it has to be one of the data bytes
      index += 1;
      if (index == 3) { // true, if no end byte has been received as fourth byte and loop continues to receive
        inData[index] = inByte; //store non-end byte
        // reset flags, packet will be discarded in following validation-if-loop
        isReceiving = false;
        isBusy = true;
      }
    }

    if (isReceiving) { // saves current inByte in data buffer
      inData[index] = inByte;
    }

  }

  // validates packet in inData and sends response
  if (isBusy && !isReceiving) {
    if (inData[0] == 42 && inData[3] == 255 && // packet contains start and end byte in the right place
        inData[1] >= validCommandRange[0] && inData[1] <= validCommandRange[1] && // packet contains valid command byte (is in validCommandRange)
        inData[2] >= validValueRanges[inData[1]][0] && inData[2] <= validValueRanges[inData[1]][1]) // packet contains valid value byte for the corresponding command
    { 
      Serial.print("ACK"); // sends acknowledged response
      isOk = true; // packet is valid and command can now be executed
    } 
    else { 
      Serial.print("NACK"); // sends not acknowledged response
      isOk = false; // packet is invalid
      isBusy = false; // packet is discarded and next incoming one is processed
    } 
  }
}

// flushes serial input buffer
void serialFlush() {
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

/////// Sample Functions: ///////

// blinks builtin LED
void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
}

// lights builtin LED
void lightLED(int time) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(time * 1000);
  digitalWrite(LED_BUILTIN, LOW);
}