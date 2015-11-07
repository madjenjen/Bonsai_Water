int pump = A0;
int light = 13;
int sensor = A1;

void setup() {
  // initialize serial communication at 9600 bits per second:
  pinMode(pump, OUTPUT);
  pinMode(light, OUTPUT);
  Serial.begin(9600);
}


void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(sensor); 
     
  if (sensorValue >=600){
    Serial.println(sensorValue);
    digitalWrite(pump, HIGH);
    digitalWrite(light, HIGH);
    Serial.print("Set pump: ");
    Serial.println(digitalRead(pump));
  }
  else{
    Serial.println(sensorValue);
    digitalWrite(pump, LOW);
    digitalWrite(light, LOW);
  }
  delay(1000);
}

