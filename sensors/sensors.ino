//#define trigPin 12
#define echoPin 11

char val; // variable to receive data from the serial port
char buf[100];
int tail;

void setup() {
  tail = 0;
  Serial.begin (9600);
//  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
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
          if (strcmp(buf, "distance\r\n")  == 0) {
//              for (int i = 0; buf[i]; i++) {
//                Serial.write(buf[i]);
//              }
              long duration, distance;
              //  digitalWrite(trigPin, LOW);  // Added this line
              //  delayMicroseconds(2); // Added this line
              //  digitalWrite(trigPin, HIGH);
              //  delayMicroseconds(10); // Added this line
              //  digitalWrite(trigPin, LOW);
                duration = pulseIn(echoPin, HIGH);
                distance = (duration/2) / 29.1;
                if (distance <= 0){
                  Serial.write("S1:-1;");
                }
                else {
                  Serial.write("S1:");
                  Serial.print(distance);
                  Serial.write(";");
                }
          }
      } else {
         buf[tail++] = val;
      }
  }  
}
