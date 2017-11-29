#define trigPin 12

void setup() {
  pinMode(trigPin, OUTPUT);
}

void loop() {
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
}
