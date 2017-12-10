
// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <Servo.h> 
#include <RF24.h>
#include <stdio.h>
#include <math.h>
#include <avr/wdt.h>

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

struct Point {
  float x = 0;
  float y = 0;
} p, pPrev;

struct Angles {
  int alpha = 0;
  int betta = 0;
} angles;

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
    wdt_enable(WDTO_4S);
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
      
      calcAngles(d1, d2, angles, p, pPrev);
      
      servoLow.write(90 - angles.alpha);
      servoHigh.write(190 - angles.betta);
      
      serialPrintLn("alpha:");
      serialPrintLn(angles.alpha);
      serialPrintLn("betta:");
      serialPrintLn(angles.betta);
      prevMillis = millis();
    }
    wdt_reset();
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
            serialPrintLn("d1_error:");
            serialPrintLn(d);
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
            serialPrintLn("d2_error:");
            serialPrintLn(d);
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


void calcAngles(float d1, float d2, Angles &angles, Point &p, Point &pPrev) {
  p.x = (d1 * d1 - d2 * d2) / 2;
  p.y = sqrt(abs(d1 * d1 - (p.x + 0.5) * (p.x + 0.5)));
  
  if (abs(p.x - pPrev.x) >= 0.05 || abs(p.y - pPrev.y) >= 0.05) {   
    float dx = p.x - pPrev.x;
    float dy = p.y - pPrev.y;

    float x = p.x + (dx * 0.20) / sqrt(dx * dx + dy * dy);
    float y;
    
    if (abs(x) > 0.5) {
      x = getSign(x) * 0.5;
      float dx1 = x - p.x;
      float dy1 = getSign(dy) * sqrt(0.04 - dx1 * dx1);
      y = p.y + dy1;
    } else {
      y = p.y + (dy * 0.20) / sqrt(dx * dx + dy * dy);
    }
    
    angles.alpha = round(atan(y) * 57.2958) + 10;
    angles.betta = round(atan(x / sqrt(1 + y * y)) * 57.2958) + 90;
    
    if (angles.alpha > 90) {
      angles.alpha = 90;
    } else if (angles.alpha < 0) {
      angles.alpha = 0;
    }
    if (angles.betta > 190) {
      angles.betta = 190;
    } else if (angles.betta < 0) {
      angles.betta = 0;
    }

    pPrev.x = p.x;
    pPrev.y = p.y;
  }
}

int getSign(float x) {
  if (x > 0.0000001) return 1;
  if (x < -0.0000001) return -1;
  return 0;
}

