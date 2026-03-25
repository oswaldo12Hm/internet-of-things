int ledpin = 12;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("HOLA MUNDO, ESP32!");
}

void loop() {
  digitalWrite(ledpin, HIGH);
  delay(10000);
}
