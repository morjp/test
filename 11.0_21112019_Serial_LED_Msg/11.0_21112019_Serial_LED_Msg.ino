byte inData[10];
bool isReceiving = false;
int index = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  receiveMsg();
  /*if (!isReceiving) {
    Serial.println("Current message: ");
    for (int i; i < 10; i++) {
      Serial.println(inData[i], HEX);
      delay(10);
    } 
  } */ 
}

void receiveMsg() {
  while (true) {//Serial.available()) {
  //Serial.println("test");
    int iByte = Serial.read();

    if (!isReceiving && iByte == 42) {
      //TODO: clear data buffer, exchange handshake
      isReceiving = true;
      Serial.print("start of msg:");
      Serial.println(iByte);
      delay(100);
      index = 0;
    } else if (isReceiving && iByte == 0x45) {
      Serial.print("end of msg:");
      Serial.println(iByte, HEX);
      delay(100);
      index += 1;
      inData[index] = iByte; // Put end byte in data array
      isReceiving = false;
    } else if (isReceiving) {
      Serial.print("adding to msg:");
      Serial.println(iByte, HEX);
      delay(100);
      index += 1;
    }

    if (isReceiving) {
      inData[index] = iByte;
    }
  }
}
