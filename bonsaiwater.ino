int pump = 9 ;
int light = 13;
int sensor = 2;
char specials[] = "$&+,/:;=?@ <>#%{}|~[]`"; //For URL encoding

int wateringPoint = 300;

#include <ArduinoJson.h>; //For wifi
#include "config.h";
#include <Time.h>
#include <TimeAlarms.h>
#include <Wire.h>  

void setup() {
  Wire.begin();
  pinMode(pump, OUTPUT);
  pinMode(light, OUTPUT);
  pinMode(sensor, OUTPUT);
  Serial.begin(9600);
  setTime(6,27,0,10,06,16); 
  Alarm.timerRepeat(3600, sendStatusMessage); //send message every hour

  digitalWrite(sensor, LOW);
  digitalWrite(sensor, HIGH);
  writeI2CRegister8bit(0x20, 3);
  
  //For wifi
  Serial2.begin(115200);
  Serial2.setTimeout(5000);
  delay(1000);
  // try to connect to wifi
  Serial2.println("AT+RST");
  Serial.println("Resetting module");
  if(Serial2.find("OK")){
    Serial.println("OK");
  }
  Serial2.flush();
  if(Serial2.find("ready"))
  {
    Serial.println("Module is ready");
  }
  else
  {
    Serial.println("Module not responding.");
    while(1);
  }
  delay(1000);
  
  boolean connected=false;
  for(int i=0;i<5;i++){
    if(connectWiFi()){
      connected = true;
      Serial2.println("AT+CWJAP?");
      if(Serial2.find("OK")){
        Serial.println("Ok, connected to WiFi");
        break;
      }
      else{
        Serial.println("Cannot connect to WiFi");
        while(1);
      }
    }
  }
  if (!connected){
    while(1);
  }
  delay(5000);
  Serial2.println("AT+CIPMUX=0"); // set to single connection mode
}

void loop() {
  int sensorValue = readI2CRegister16bit(0x20, 0);
  Alarm.delay(1000);
  Serial.println(sensorValue);
  if (sensorValue <= wateringPoint){
    analogWrite(pump, 100);
    digitalWrite(light, HIGH);
    sendWaterMessage("Bonsai H2GO!");
  }
  else{
    analogWrite(pump, 0);
    digitalWrite(light, LOW);
  }
  delay(10000);
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
  Serial2.println(cmd);
  Serial.println(cmd);
  delay(2000);
  
  if(Serial2.find("WIFI")){
    return true;
  }
  else{
    Serial.println("Cannot connect to WiFi");
    return false;
  }
  if (!Serial2.available()){
    Serial.println("Serial 2 not available");
    delay(5000);
    software_Reset();
    }
    Serial.println("Finished");
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

void sendWaterMessage(char* message)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",";
  cmd += PORT;
  Serial2.println(cmd);
  if(Serial2.find("Error")) return;
  cmd = "GET /api/slack?apiKey=";
  cmd += API_KEY;
  cmd += "&text=";
  char encoded_message[120];
  *urlencode(encoded_message ,message);
  cmd += encoded_message;
  char encoded_channel[120];
  *urlencode(encoded_channel ,CHANNEL);
  cmd += "&channel=";
  cmd += encoded_channel;
  cmd += " HTTP/1.0\r\n\r\n";
  Serial2.print("AT+CIPSEND=");
  Serial2.println(cmd.length());
  if(Serial2.find(">")) {} 
  else {
    Serial2.println("AT+CIPCLOSE");
    delay(1000);
    return;
  }
  Serial2.println(cmd);
  Serial.println(cmd);
}

void sendStatusMessage(){
  int sensorValue = readI2CRegister16bit(0x20, 0);
   //Convert int to char for messsage
  char sensorValueChar[4]; //The sensor reading will be 3 characters long, so array has to be 4 long
  String sensorValueStr;
  sensorValueStr = String(sensorValue);
  sensorValueStr.toCharArray(sensorValueChar,4); //Add chars to the end of the array
  
  char wateringPointValueChar[4]; //The watering point will be 3 characters long, so array has to be 4 long
  String wateringPointValueStr;
  wateringPointValueStr = String(wateringPoint);
  wateringPointValueStr.toCharArray(wateringPointValueChar,4); //Add chars to the end of the array
  
  char statusMessage[120];
  strcpy(statusMessage, "Your bonsai's soil is currently at ");
  strcat(statusMessage, sensorValueChar);
  strcat(statusMessage, ". I will water it once it goes below ");
  strcat(statusMessage, wateringPointValueChar);
  strcat(statusMessage, ".");
  sendWaterMessage(statusMessage);
}

static char hex_digit(char b) //For URL encoding
{  
  return "01234567890ABCDEF"[b & 0x0F];
}

char *urlencode(char *dst,char *src) //For URL encoding
{  
  char b;
  char *d = dst;
  while (b = *src++)
  {  
    if (strchr(specials,b))
    {  *d++ = '%';
       *d++ = hex_digit(b >> 4);
       b = hex_digit(b);
    }
    *d++ = b;
   }
   *d = '\0';
   return dst;
}

void software_Reset() {// Restarts program from beginning but does not reset the peripherals and registers
  asm volatile ("  jmp 0");  
}  
