
// SimpleRx - the slave or the receiver

#define echoPin 3
#define trigPin 2

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);

char dataReceived[10]; // this must match dataToSend in the TX
char ack[9];
bool newData = false;

double distance = 0;

//===========

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
    Serial.begin(9600);

    Serial.println("SimpleRx Starting");
    radio.begin();
    delay(100);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.setDataRate(RF24_2MBPS);
    radio.setRetries(15, 15);
    radio.openReadingPipe(1, 0xF0F0F0F0AA);
    radio.startListening();
}

//=============

void loop() {
    showData();
}

void showData() {
     if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;
    }
    if (newData == true) {
        if (strcmp(dataReceived, "sync;") == 0) {
           digitalWrite(trigPin, HIGH);
           delayMicroseconds(10);
           digitalWrite(trigPin, LOW);
           unsigned long duration = pulseIn(echoPin, HIGH);
           distance = duration * 0.034; 
           serialPrintLn("sync");
        }
        newData = false;
    }
}

void serialPrintLn(String s) {
   if (Serial) {
    Serial.println(s);
  }
}

void serialPrintLn(float s) {
   if (Serial) {
    Serial.println(s, 6);
  }
}
