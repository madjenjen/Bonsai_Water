int pump = 9 ;
int light = 13;

void setup() {
  pinMode(pump, OUTPUT);
  pinMode(light, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  //Try the pump at full power.
  analogWrite(pump, 255);
  digitalWrite(light, HIGH);
  delay(10000);
  analogWrite(pump, 0);
  digitalWrite(light, LOW);
  delay(10000);

  //Try the pump at lower power.
  analogWrite(pump, 100);
  digitalWrite(light, HIGH);
  delay(10000);
  analogWrite(pump, 0);
  digitalWrite(light, LOW);
  delay(10000);
}
