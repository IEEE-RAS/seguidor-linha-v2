#include <Arduino.h>

#define A1A 6
#define A1B 5
#define B1A 11
#define B1B 10

#define CW 1
#define CCW 0

void setup()
{
  // put your setup code here, to run once:
  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);
  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);
}

void rotateDigital(bool dir, byte pA, byte pB, bool en)
{
  byte pLow = dir ? pA : pB;
  byte pHigh = dir ? pB : pA;

  digitalWrite(pLow, LOW);
  digitalWrite(pHigh, en);
}

void loop()
{
  rotateDigital(CW, A1A, A1B, true);
  delay(1000);
  rotateDigital(CW, A1A, A1B, false);
  delay(3000);
}