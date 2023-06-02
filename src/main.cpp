
#include <CAN.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
/*
 * esp32 & sn65hvd230 cluster controller by gizmo87898 on github
 */
#define lo8(x) ((int)(x)&0xff)
#define hi8(x) ((int)(x)>>8)

//CAN loop coutners
unsigned long sinceLast100msLoop = 0;
unsigned long sinceLast200msLoop = 0;
unsigned long sinceLast1000msLoop = 0;
unsigned long sinceLast5sLoop = 0;

//testing shit
int randomId = 0x456;
int count = 0; // for ignition, abs, etc
int testcount = 0;


//cluster variables
int rpm = 1000; // 0-7500 rpm
float speed = 0; // m/s
int coolant_temp = 120; // C
int oil_temp = 120;
int drive_mode = 0;  // 1 = traction, 2 = comfort+, 4 = sport, 5 = sport+, 6 = dsc off, 7 = eco pro
int fuel = 50; // 0-100
int gear = 0;
// 0 = N
// 1 = 1
// ...
// 6 = 6
// 7 = P
// 8 = R
// 9 = D
char lights[16] = {0,0,0,0,0,0,0,0,0,0,0}; 
// Lights Bitfield:
// 0 ABS
// 1 LeftDir
// 2 RightDir
// 3 TPMS
// 4 Brake
// 5 TC
// 6 TC_Off
// 7 FogLights
// 8 AutoStartStop
// 9 ParkingLights
// 10 Headlights
// 11 HighBeam
// 12 CEL
// 13 Auto Highbeam
//time and date
int hour = 12;
int minute = 34;
int seconds = 56;
int day = 23;
int month = 1;
int year = 2027;

void setup() {
  // Begin serial at 115200bps
  Serial.begin(115200);
  // Set CAN RX/TX pins to 2 and 15
  CAN.setPins(25,26);
  while (!Serial);
  // start the CAN bus at 125 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  CAN.loopback();
  Serial.println("Initialized in Loopback");
}



void send123() { // Test Message
  CAN.beginPacket(0x123);
  CAN.write(0);
  CAN.write(0); 
  CAN.write(0);
  CAN.write(0);
  CAN.write(random(255));
  CAN.write(random(255));
  CAN.write(0);
  CAN.write(0);
  CAN.endPacket();

}


void sendRandom() {
  CAN.beginPacket(randomId);
  CAN.write(random(255));
  CAN.write(random(255));
  CAN.write(random(255));
  CAN.write(random(255));
  CAN.write(random(255));
  CAN.write(random(255));
  CAN.write(random(255));
  CAN.write(random(255));
  CAN.endPacket();
}


void loop() {
  unsigned long currentLoop = millis();
  String data;

  // read data from client
  if (Serial.available()) {
    speed = String(Serial.readStringUntil(';')).toFloat();
    rpm = String(Serial.readStringUntil(';')).toInt();
    oil_temp = String(Serial.readStringUntil(';')).toInt();
    coolant_temp = String(Serial.readStringUntil(';')).toInt();
    fuel = String(Serial.readStringUntil(';')).toInt();
    //lights = String(Serial.readStringUntil(';')).toInt(); fix this :D
  }

  //decode bitfield into variables

  int packetSize = CAN.parsePacket();
  if (packetSize) {
    // if a packet is present
    while (CAN.available()) {

      int buf = CAN.read();
      if(String(buf, HEX).length() == 1) {
        buf = buf | 0xF0;
      }
      data.concat(String(buf, HEX) + " ");
    }
    Serial.print(data);
    Serial.print(" | ");
    Serial.print(packetSize);
    Serial.print(" | ");
    Serial.println(CAN.packetId(), HEX);

  }

  
  if (currentLoop - sinceLast100msLoop > 100) {
    sinceLast100msLoop = currentLoop;
    send123(); // Call send fucntions here

  }

  
  if (currentLoop - sinceLast200msLoop > 200) {
    sinceLast200msLoop = currentLoop;
  }

  
  if (currentLoop - sinceLast1000msLoop > 1000) {
    sinceLast1000msLoop = currentLoop;
    sendRandom(); 

  }

  
  if (currentLoop - sinceLast5sLoop > 5000) {
    sinceLast5sLoop = currentLoop;
    randomId += 1;
  }

  
}
