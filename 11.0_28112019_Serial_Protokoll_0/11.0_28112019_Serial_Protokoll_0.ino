int inData[4]; // data buffer
int index = 0; // data buffer index
bool isReceiving = false; // true if currently receiving packet
bool isBusy = false; // true if packet has been received and is being processed
bool isOk = false; // true if packet is valid 

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  receivePacket();

  if (!isReceiving && isBusy && isOk) { // packet stored in inData is fully received and valid, so command can be executed

    if (inData[1] == 0) {
      blinkLED(inData[2]);
    } 
    //TODO: execute command with ifs
    //set isbusy to false
    serialFlush(); // flush serial buffer to discard everything that has been send during execution of command
    isBusy = false; // packet is discarded and next incoming one is processed
  }
 
}

void receivePacket() {
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
        // reset flags, packet will be discarded in following if-loop
        isReceiving = false;
        isBusy = true;
      }
    }

    if (isReceiving) { // saves current inByte in data buffer
      inData[index] = inByte;
    }

  }

  // validates packet in inData and sends response
  //TODO: if not ok just set isbusy to false
  //TODO: check if value and command are valid
  if (isBusy && !isReceiving) {
    if (inData[0] == 42 && inData[3] == 255) { // packet contains start and end byte in the right place
      
      //check command and value range with two dimensionql array

      Serial.print("ACK"); // sends acknowledged response
      //DEBUG:
      /*Serial.print(inData[0]); Serial.print(" ");
      Serial.print(inData[1]); Serial.print(" ");
      Serial.print(inData[2]); Serial.print(" ");
      Serial.println(inData[3]); */
      //DEBUG
      isOk = true; // packet is valid and command can now be executed
    } 
    else { 
      Serial.print("NACK"); // sends not acknowledged response
      //DEBUG:
      /*Serial.print(inData[0]);
      Serial.print(inData[1]);
      Serial.print(inData[2]);
      Serial.println(inData[3]); */
      //DEBUG
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

// blinks builtin LED
void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
}