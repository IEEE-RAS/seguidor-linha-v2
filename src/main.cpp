#include <Arduino.h>

/**
 * @brief Pinos de controle dos motores, os pinos foram escolhidos por
 * terem capacidade de realizar PWM, para controle de velocidade.
 * São nomeados de acordo com as entradas do módulo de ponte H utilizado
 *
 */
#define A1A 6
#define A1B 5
#define B1A 11
#define B1B 10

/**
 * @brief Definição auxiliar para sentido de rotação
 *
 */
#define CW 1  // Horário
#define CCW 0 // Anti-horário

struct motor
{
  byte TA, TB;
};

struct motor MotE = {A1A, A1B}, MotD = {B1A, B1B};

void setup()
{
  pinMode(MotE.TA, OUTPUT);
  pinMode(MotE.TB, OUTPUT);
  pinMode(MotD.TA, OUTPUT);
  pinMode(MotD.TB, OUTPUT);
}

void rotateDigital(bool dir, byte pA, byte pB, bool en = true)
{
  byte pLow = dir ? pA : pB;
  byte pHigh = dir ? pB : pA;

  digitalWrite(pLow, LOW);
  digitalWrite(pHigh, en);
}

void rotateDigital(bool dir, struct motor mt, bool en = true)
{
  rotateDigital(dir, mt.TA, mt.TA, en);
}

void rotateAnalog(bool dir, byte pA, byte pB, byte pwm, bool en = true)
{
  byte pLow = dir ? pA : pB;
  byte pHigh = dir ? pB : pA;
  byte state = en ? pwm : LOW;

  digitalWrite(pLow, LOW);
  analogWrite(pHigh, state);
}

void rotateAnalog(bool dir, struct motor mt, byte pwm, bool en = true)
{
  rotateAnalog(dir, mt.TA, mt.TB, pwm, en);
}

void stop()
{
  rotateDigital(CW, MotE, false);
  rotateDigital(CW, MotD, false);
}

void forward()
{
  rotateDigital(CW, MotE);
  rotateDigital(CW, MotD);
}

void left()
{
  rotateDigital(CW, MotD);
  rotateAnalog(CW, MotE, 127);
}

void right()
{
  rotateDigital(CW, MotE);
  rotateDigital(CW, MotD, 127);
}

void loop()
{
  forward();
  delay(1000);
  stop();
  delay(3000);
  left();
  delay(1000);
  stop();
  delay(3000);
  right();
  delay(1000);
  stop();
  delay(3000);
}