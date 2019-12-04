void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
      byte iByte = Serial.read();

      // Receives incoming byte and blinks LED number of times, dose not matter if we write it in hex or as dec
      // When entering in serial monitor, one must use ASCII representation of byte e.g. M for 77 or 0x4D
      Serial.println(iByte);
      for (int i = 0; i < iByte; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
      }
   }
}
