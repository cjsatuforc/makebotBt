#include <SoftwareSerial.h>
#include "MeOrion.h"


SoftwareSerial mySerial(A0, A7); // RX, TX

MeDCMotor motor3(M1);
MeDCMotor motor4(M2);

int btbufN = 0;
char btbuf[10];

void setup() {

  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  mySerial.begin(9600);
}

void loop() { // run over and over
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
  
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
 
}

void process() {
  Serial.print("B: ");
  Serial.print(btbufN);
  Serial.print(" ");
  Serial.println(btbuf);
  if (btbufN == 6) {
    int x = round ( (100*((int)btbuf[0]-48) + 10*((int)btbuf[1]-48) + 1*((int)btbuf[2]-48) - 200) * 2.55);
    int y = round ( (100*((int)btbuf[3]-48) + 10*((int)btbuf[4]-48) + 1*((int)btbuf[5]-48) - 200) * 2.55);

  motor3.run(x*-1);
  motor4.run(y*1);
if (x == 0) {
    motor3.stop();
}
if (y==0) {
  motor4.stop();
}
    
    Serial.print("x="); Serial.print(x); Serial.print(" y="); Serial.println(y);
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
