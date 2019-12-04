int iByte;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
if (Serial.available() > 0) {
  iByte = Serial.read();
  if (iByte == 42) {
    Serial.println("OK");
  }
}
}
