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
 * @brief Pinos dos sensores de linha, começando com o sensor mais à esquerda
 * (SEE) passando pelo sensor ao centro (SC) até o sensor mais à direita (SDD)
 *
 */
#define SEE A3
#define SE A4
#define SC A5
#define SD A6
#define SDD A7

/**
 * @brief Definição auxiliar para sentido de rotação
 *
 */
#define CW 1  // Horário
#define CCW 0 // Anti-horário

struct motor
{
  byte TA, TB;
  bool dir;
};

struct motor MotE = {A1A, A1B, CW}, MotD = {B1A, B1B, CCW};

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
  digitalWrite(MotE.TA, LOW);
  digitalWrite(MotE.TB, LOW);
  digitalWrite(MotD.TA, LOW);
  digitalWrite(MotD.TB, LOW);
}

void accelerate(int from, int to, int rate)
{
  for (int i = from; i < to; i += rate)
  {
    rotateAnalog(MotE.dir, MotE, i);
    rotateAnalog(MotD.dir, MotD, i);
    delay(2);
  }
}

void forward(bool accel = true)
{
  if (accel)
  {
    accelerate(30, 255, 2);
  }

  rotateAnalog(MotE.dir, MotE, 255);
  rotateAnalog(MotD.dir, MotD, 255);
}

void turnRight()
{
  rotateAnalog(MotE.dir, MotE, 255);
  rotateAnalog(MotD.dir, MotD, 100);
}

void turnLeft()
{
  rotateAnalog(MotE.dir, MotE, 100);
  rotateAnalog(MotD.dir, MotD, 255);
}

void curvaAoCentro(int SENSOR, void (*funcao)())
{
  if (!digitalRead(SENSOR)) // detecção do sensor de direção
  {
    while (digitalRead(SC)) // loop garante a curva até o centro
    {
      funcao();
      delay(50);
    }
  }
}

void setup()
{
  pinMode(MotE.TA, OUTPUT);
  pinMode(MotE.TB, OUTPUT);
  pinMode(MotD.TA, OUTPUT);
  pinMode(MotD.TB, OUTPUT);
  forward();
}

/**
 * @brief Caso o sensor da esquerda (SE) dispara, este está encima da linha,
 * e deve garantir a curva à direita até que o sensor do centro seja ativado,
 * e vice versa. Do contrário o veiculo segue em frente.
 *
 */
void loop()
{
  curvaAoCentro(SE, &turnRight);
  curvaAoCentro(SD, &turnLeft);
  forward(false);
}