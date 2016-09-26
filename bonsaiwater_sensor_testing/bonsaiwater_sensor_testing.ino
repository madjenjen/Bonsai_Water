int light = 13;
int sensor = 2;

#include <Wire.h>  

void setup() {
  Wire.begin();
  pinMode(light, OUTPUT);
  pinMode(sensor, OUTPUT);
  Serial.begin(9600);
  
  digitalWrite(sensor, LOW);
  digitalWrite(sensor, HIGH);
  writeI2CRegister8bit(0x20, 3);
}

void loop() {
  int sensorValue = readI2CRegister16bit(0x20, 0);
  Serial.println(sensorValue);
  if (sensorValue <= 300){
    digitalWrite(light, HIGH);
  }
  else{
    digitalWrite(light, LOW);
  }
  delay(10000);
}

void writeI2CRegister8bit(int addr, int value) {
  Wire.beginTransmission(addr);
  Wire.write(value);
  Wire.endTransmission();
}

unsigned int readI2CRegister16bit(int addr, int reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();
  delay(1100);
  Wire.requestFrom(addr, 2);
  unsigned int t = Wire.read() << 8;
  t = t | Wire.read();
  return t;
} 
