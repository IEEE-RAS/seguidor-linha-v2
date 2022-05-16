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
#define CW 1 // Horário
#define CCW 0 // Anti-horário

void setup()
{
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

void stop()
{
  digitalWrite(A1A, LOW);
  digitalWrite(A1B, LOW);
  digitalWrite(B1A, LOW);
  digitalWrite(B1B, LOW);

}

void loop()
{
  rotateDigital(CW, A1A, A1B, true);
  rotateDigital(CW, B1A, B1B, true);
  delay(1000);
  stop();
  delay(3000);
  rotateDigital(CCW, A1A, A1B, true);
  rotateDigital(CCW, B1A, B1B, true);
  delay(1000);
  stop();
  delay(3000);
}