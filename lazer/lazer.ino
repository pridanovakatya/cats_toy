#include <Servo.h>
int incomingByte = 0;
char val; // variable to receive data from the serial port
char buf[100];
int tail;

int pos = 0;    // variable to store the servo position
Servo myservo2;
Servo myservo1;

void setup() {
  tail = 0;
  Serial.begin(9600);
  while (!Serial);
  delay(500);  
  myservo1.attach(6); 
  myservo2.attach(7); 
 }

void loop() {      
  if(Serial.available())  {  
    val = Serial.read();
    if (val == ';') {
      buf[tail++] = '\r';
      buf[tail++] = '\n';
    }
      if(val == ';' || val == '\n') {
          buf[tail++] = 0;
          tail = 0;
          for (int i = 0; buf[i]; i++) {
            Serial.write(buf[i]);
          }
          if (strcmp(buf, "test\r\n")  == 0) {
            Serial.print("tested\r\n");
          }
      } else {
         buf[tail++] = val;
      }
  }  
  delay(100);

}


