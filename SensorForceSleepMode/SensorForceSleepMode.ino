void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(12, INPUT);

}


void loop() {
  // put your main code here, to run repeatedly:
  int movSensor = digitalRead(12);
  if (movSensor == 1) {
    digitalWrite(2, 1);
    Serial.println("Sensor movement detection!");
    delay(1000);
    digitalWrite(2, 0);
    delay(60000);    
  } else {
    delay(100);
  }
}
