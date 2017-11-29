#define echoPin 2

char buf[100];
int tail;
long distance = 0;

void setup() {
  tail = 0;
  Serial.begin (9600);
  pinMode(echoPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(echoPin), handlePulse, FALLING);
}

void handlePulse() {
  detachInterrupt(digitalPinToInterrupt(echoPin));
  long duration;
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034;  
  attachInterrupt(digitalPinToInterrupt(echoPin), handlePulse, FALLING);
}

void loop() {  
    if(Serial.available())  {  
    char val = Serial.read();
    if (val == ';') {
      buf[tail++] = '\r';
      buf[tail++] = '\n';
    }
      if(val == ';' || val == '\n') {
          buf[tail++] = 0;
          tail = 0;
          if (strcmp(buf, "distance\r\n")  == 0) {
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
