#include <Servo.h>
char val; // variable to receive data from the serial port
char buf[200];
int tail;

int pos = 0;    // variable to store the servo position
Servo myservo2;
Servo myservo1;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  tail = 0;
  Serial.begin(9600);
  while (!Serial);
  delay(500);  

//digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
delay(1000);
digitalWrite(8, HIGH);
delay(1000);

    Serial.print("AT+INIT\r\n");
    delay(5000);
//    Serial.print("AT+PAIR=AB53,86,3439DA,1\r\n");
//    delay(1000);
//    Serial.write("AT+BIND=AB53,86,3439DA\r\n");
//    delay(1000);
//    Serial.write("AT+LINK=AB53,86,3439DA\r\n");
//    delay(1000);
//    
//    Serial.print("AT+PAIR=ABB0,89,563402,9\r\n");
//    delay(9000);
    Serial.write("AT+BIND=ABD8,89,563402\r\n");
    delay(1000);
    Serial.write("AT+LINK=ABD8,89,563402\r\n");
    delay(1000);
//    
//    Serial.print("AT+PAIR=AB92,8E,563402,9\r\n");
//    delay(9000);
//    Serial.write("AT+BIND=AB92,8E,563402\r\n");
//    delay(1000);
//    Serial.write("AT+LINK=AB92,8E,563402\r\n");
    delay(1000);
//

//  while(Serial.available()){
//    char s = Serial.read();
//    Serial.write(s);
//  }
//  Serial.write("\r\n");
//  digitalWrite(8, LOW);
//    Serial.write("AT+PAIR=ABB0,89,563402,9\r\n");
//  delay(9000);
//    while(Serial.available()){
//    char s = Serial.read();
//    Serial.write(s);
//  }
//  Serial.write("\r\n");
//  delay(500);
//    Serial.print("AT+BIND=AB92,8E,56,3402\r\n");
//  delay(9000);
//    while(Serial.available()){
//    char s = Serial.read();
//    Serial.write(s);
//  }
//  Serial.write("\r\n");
//  delay(500);

  delay(500);
//    while(Serial.available()){
//    char s = Serial.read();
//    Serial.write(s);
//  }
//  Serial.write("\r\n");
//  delay(500);
//
//      Serial.print("AT+LINK=ABB0,89,563402\r\n");
//  delay(500);
//    while(Serial.available()){
//    char s = Serial.read();
//    Serial.write(s);
//  }
//  Serial.write("\r\n");
//  delay(500);
//  digitalWrite(9, LOW);
//  delay(500);
//  digitalWrite(8, LOW);
//  delay(500);
//  digitalWrite(9, HIGH);
//  Serial.print("AT+LINK=ABB0,89,563402\r\n");
//  delay(500);
//    while(Serial.available()){
//    char s = Serial.read();
//    Serial.write(s);
//  }
//  Serial.write("\r\n");
//  delay(500);
   

//  digitalWrite(8, LOW);
//  delay(1000);
//  digitalWrite(9, LOW);
//   delay(1000);
//    digitalWrite(9, HIGH);


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
          if (strncmp("ERROR", buf, 5 != 0)) {
          for (int i = 0; buf[i]; i++) {
            Serial.print(buf[i]);
          }
          if (strcmp(buf, "test\r\n")  == 0) {
            Serial.print("tested\r\n");
          }
        }
      } else {
         buf[tail++] = val;
      }
  } else {
    Serial.write("distance;"); 
    delay(2000);
  }
}


