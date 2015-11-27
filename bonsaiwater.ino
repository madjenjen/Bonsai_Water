int pump = A0;
int light = 13;
int sensor = A1;
char specials[] = "$&+,/:;=?@ <>#%{}|~[]`"; //For URL encoding

#include <ArduinoJson.h>; //For wifi
#include "config.h";

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
  int sensorValue = analogRead(sensor);

  if (sensorValue >=600){
    Serial.println(sensorValue);
    digitalWrite(pump, HIGH);
    digitalWrite(light, HIGH);
    Serial.print("Set pump: ");
    Serial.println(digitalRead(pump));
    sendWaterMessage("Bonsai H2GO!");
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

void sendWaterMessage(char* message)
{
  Serial.println("Connecting...");
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial2.println(cmd);
  if(Serial2.find("Error")) return;
  cmd = "GET /api/v1/slack?key=";
  cmd += API_KEY;
  cmd += "&message=";
  char encoded_message[120];
  *urlencode(encoded_message ,message);
  cmd += encoded_message;
  cmd += "&channel=";
  cmd += CHANNEL;
  cmd += " HTTP/1.0\r\n\r\n";
  Serial.println(cmd);
  Serial2.print("AT+CIPSEND=");
  Serial2.println(cmd.length());
  if(Serial2.find(">")) {
    Serial.print(">");
  } else {
    Serial2.println("AT+CIPCLOSE");
    Serial.println("connection timeout");
    delay(1000);
    return;
  }
  Serial2.println(cmd);

  unsigned int i = 0; //timeout counter
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
