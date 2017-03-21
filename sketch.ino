#include <SoftwareSerial.h>
#include "MeOrion.h"

#define    STX          0x02
#define    ETX          0x03

SoftwareSerial mySerial(2, 8); // Port 4
MeDCMotor motor3(M1);
MeDCMotor motor4(M2);
MeUltrasonicSensor ultraSensor(PORT_3); /* Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield. */

int btbufN = 0;
char btbuf[10];
long lastInfo = 0;
long distance = 0;
long prevdistance = 0;

void setup() {

  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  mySerial.begin(9600);
}

void loop() {

  sendBlueToothData();
  
  if (mySerial.available()) {
    char c = mySerial.read();
    
    if (c == 2) {
      btbuf[btbufN] = 0;
      process();
      btbufN = 0;
    } else if (c < 5) {    
    } else {
      btbuf[btbufN++] = c;
      if (btbufN > 8) {
        btbufN = 0;
      }
    }
  }
  
}

void sendBlueToothData()  {
   long dist = ultraSensor.distanceCm();
   if (dist != 400) {
    distance = dist;
   }
   /*
   if ( abs(distance - prevdistance) < 10) {
     if (distance < 15) {
      digitalWrite(A3, HIGH);
      motor3.run(250);
      motor4.run(250);
      delay(1500);
      motor3.stop();
      motor4.stop();
     } else {
      digitalWrite(A3, LOW);
     }
   }
   prevdistance = distance;
*/
 
 static long previousMillis = 0;                             
 long currentMillis = millis();
 if(currentMillis - previousMillis > 1000) {   // send data back to smartphone
   previousMillis = currentMillis; 

   
// Data frame transmitted back from Arduino to Android device:
// < 0X02   Buttons state   0X01   DataField#1   0x04   DataField#2   0x05   DataField#3    0x03 >  
// < 0X02       "01011"      0X01     "120.00"    0x04     "-4500"     0x05  "Motor enabled" 0x03 >    // example
   

   mySerial.print((char)STX);
   mySerial.print("0000");
   mySerial.print( digitalRead(A3) ? "1" : "0" );
   mySerial.print( digitalRead(A2) ? "1" : "0" );
   mySerial.print((char)0x1);
   mySerial.print( distance );
   mySerial.print("cm");
   mySerial.print((char)0x4);
   
   mySerial.print("34");          
   mySerial.print((char)0x5);
   
   mySerial.print("Hello");
   mySerial.print((char)ETX);
 }  
}

void process() {

  if (btbufN == 6) {
    int x = round ( (100*((int)btbuf[0]-48) + 10*((int)btbuf[1]-48) + 1*((int)btbuf[2]-48) - 200) * 1);
    int y = round ( (100*((int)btbuf[3]-48) + 10*((int)btbuf[4]-48) + 1*((int)btbuf[5]-48) - 200) * 1);

    int m1 = round(y*2.55 + x * 2.55);
    int m2 = round(y*2.55 - x * 2.55);
    if (m1 > 255) {
      m1 = 255;
    } else if (m1 < -254) {
      m1 = -254;
    }
    if (m2 > 255) {
      m2 = 255;
    } else if (m2 < -254) {
      m2 = -254;
    }
    double dist = ultraSensor.distanceCm();

//Serial.print(m1); Serial.print(" "); Serial.print(m2); Serial.print(" ");
//Serial.println( distance );
    if (distance < 15 && m1>-50 && m2>-50) {
  //    Serial.println("STOP");
      m1 = 0; 
      m2 = 0;
    }

    if (abs(m1) < 20) {
        motor3.stop();
    } else {
        motor3.run(-1*m1);
    }
    if (abs(m2) < 20) {
      motor4.stop();
    } else {
        motor4.run(m2);
    }
    
    
  } else if (btbufN == 1) {
    if (btbuf[0] == 'A') {
      digitalWrite(A2, HIGH);
    } else if (btbuf[0] == 'B') {
      digitalWrite(A2, LOW);
    }
    if (btbuf[0] == 'C') {
      digitalWrite(A3, HIGH);
    } else if (btbuf[0] == 'D') {
      digitalWrite(A3, LOW);
    }

  
  }
}
