int pump = A0;
int light = 13;
int sensor = A1;

//For wifi
#include <ArduinoJson.h>
#include "config.h"

void setup() {
  // initialize serial communication at 9600 bits per second:
  pinMode(pump, OUTPUT);
  pinMode(light, OUTPUT);
  Serial.begin(9600);

  //For wifi
  Serial2.begin(115200);
  Serial2.setTimeout(5000);
  Serial.println("Init");
  delay(1000);
  if(Serial2.find("ready")) {
    Serial.println("WiFi - Module is ready");
  }else{
    Serial.println("Module dosn't respond.");
    //while(1);
  }
  delay(1000);
  // try to connect to wifi
  boolean connected=false;
  for(int i=0;i<5;i++){
    if(connectWiFi()){
      connected = true;
      Serial.println("Connected to WiFi...");
      break;
    }
  }
  if (!connected){
    Serial.println("Coudn't connect to WiFi.");
    while(1);
  }
  delay(5000);
  Serial2.println("AT+CIPMUX=0"); // set to single connection mode
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
    sendWaterMessage("Watering plant");
  }
  else{
    Serial.println(sensorValue);
    digitalWrite(pump, LOW);
    digitalWrite(light, LOW);
  }
  delay(1000);
}

//For wifi
boolean connectWiFi()
{
  Serial2.println("AT+CWMODE=1");
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  Serial2.println(cmd);
  delay(2000);
  if(Serial2.find("OK")){
    Serial.println("OK, Connected to WiFi.");
    return true;
  }else{
    Serial.println("Can not connect to the WiFi.");
    return false;
  }
  if (!Serial2.available()){
    delay(5000);
    software_Reset();
    }
}

void sendWaterMessage(String message)
{
  Serial.println("Connecting...");
  String data="payload={\"text\":\""+message+"\"}";
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "192.168.0.15";
  cmd += "\",3000";
  Serial.println(cmd);
  Serial2.println(cmd);
  String req = "POST / HTTP/1.0/r/n/r/n";
  req += "Content-Type: application/json/r/n";
  req += "Host: 192.168.0.15/r/n";
  req += "Content-Length: ";
  req += data.length();
  req += "/r/n/r/n";
  req += data;
  Serial.println(req);
  Serial2.print("AT+CIPSEND=");
  Serial2.println(req.length());
  if(Serial2.find(">")) {
    Serial.print(">");
  } else {
    Serial2.println("AT+CIPCLOSE");
    Serial.println("connection timeout");
    delay(1000);
    return;
  }
  Serial2.println(req);

  unsigned int i = 0; //timeout counter
  //int n = 1; // char counter
  //char json[100]="{";
  while (i<6000) {
    if(Serial2.available()) {
      char c = Serial2.read();
      Serial.print(c);
      //json[n]=c;
      //n++;
      i=0;
    }
    i++;
  }
}

void software_Reset() {// Restarts program from beginning but does not reset the peripherals and registers
  asm volatile ("  jmp 0");  
}  

