
// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <Servo.h> 
#include <RF24.h>
#include <stdio.h>
#include <math.h>

#define SERVO_LOW 5
#define SERVO_HIGH 6

#define CE_PIN   9
#define CSN_PIN 10

#define D1_PIPE 0xF0F0F0F0BB
#define D2_PIPE 0xF0F0F0F0CC
#define SYNC_PIPE 0xF0F0F0F0AA

#define DEBUG 0

float d1 = 0;
float d2 = 0;

int alpha = 0;
int betta = 0;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

Servo servoLow;
Servo servoHigh;

char syncCmd[10] = "sync;\0";
char dCmd[10] = "distance;\0";

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 700; // send once per second


char ack[13];

void setup() {
    pinMode(DEBUG, INPUT);
    servoLow.attach(SERVO_LOW);
    servoHigh.attach(SERVO_HIGH);
    Serial.begin(9600);

    serialPrintLn("SimpleTx Starting");

    radio.begin();
    delay(100);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.setDataRate(RF24_2MBPS);
    radio.setRetries(15, 15);
}

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
      sync();     
      delay(10); 
      getD1(d1);
      delay(10);
      getD2(d2);
      delay(10);
      calcAngles(d1, d2, alpha, betta);
      servoLow.write(90 - alpha);
      servoHigh.write(190 - betta);
      serialPrintLn("alpha:");
      serialPrintLn(alpha);
      serialPrintLn("betta:");
      serialPrintLn(betta);
      prevMillis = millis();
    }
}

void sync() {
    radio.openWritingPipe(SYNC_PIPE);
    if (radio.write( &syncCmd, sizeof(syncCmd) )) {
      serialPrintLn("Synced");
      }
     else {
      serialPrintLn("SYNC FAILED");
    }
}

void getD1(float &d1) {
  radio.openWritingPipe(D1_PIPE);
    if (radio.write( dCmd, sizeof(dCmd) )) {
      if (radio.isAckPayloadAvailable()) {
        while (!radio.available()); 
        radio.read(&ack, sizeof(ack));
        if (ack[0] == 'd') {
          float d = stof(ack);
          if (d >= 400) {
            serialPrintLn("d1: error");
          } else {
            d /= 100.0;
            d1 = d;
          }
          serialPrintLn("d1:");
          serialPrintLn(d1);
        }
      }
    } else {
      Serial.println("D1 FAILED");
    }
}

void getD2(float &d2) {
    radio.openWritingPipe(D2_PIPE);
    if (radio.write( dCmd, sizeof(dCmd) )) {
      if (radio.isAckPayloadAvailable()) {
        while (!radio.available()); 
        radio.read(&ack, sizeof(ack));
        if (ack[0] == 'd') {
          float d = stof(ack);
          if (d >= 400) {
            serialPrintLn("d2: error");
          } else {
            d /= 100.0;
            d2 = d;
          }
          serialPrintLn("d2:");
          serialPrintLn(d2);
        }
      }
    } else {
      serialPrintLn("D2 FAILED");
    }
}

float stof(const char* s){
  if (s[0] == 'd') {
    s += 2;
  }
  float rez = 0, fact = 1;
  if (*s == '-'){
    s++;
    fact = -1;
  };
  for (int point_seen = 0; *s; s++){
    if (*s == '.'){
      point_seen = 1; 
      continue;
    };
    int d = *s - '0';
    if (d >= 0 && d <= 9){
      if (point_seen) fact /= 10.0f;
      rez = rez * 10.0f + (float)d;
    };
  };
  return rez * fact;
};

bool isDebug() {
  return digitalRead(DEBUG) == HIGH;
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


void calcAngles(float d1, float d2, int &alpha, int &betta) {
  float x = (d1 * d1 - d2 * d2) / 2;
  float y = sqrt(abs(d1 * d1 - (x + 0.5) * (x + 0.5)));
  alpha = round(atan(y) * 57.2958);
  if (alpha > 90) {
    alpha = 90;
  } else if (alpha < 0) {
    alpha = 0;
  }
  betta = round(atan(x / sqrt(1 + y * y)) * 57.2958) + 90;
  if (betta > 180) {
    betta = 180;
  } else if (betta < 0) {
    betta = 0;
  }
}

