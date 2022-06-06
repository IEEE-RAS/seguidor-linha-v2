#include <Arduino.h>

// #define __DEBUG__

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
#define SD A2
#define SDD A7

/**
 * @brief Definição auxiliar para sentido de rotação
 *
 */
#define CW 1  // Horário
#define CCW 0 // Anti-horário

/**
 * @brief Estrutura de motor: Contém os pinos do motor e o sentido
 * de rotação para direção à frente
 * 
 */
struct motor
{
  byte TA, TB;
  bool dir;
};

struct motor MotE = {A1A, A1B, CW}, MotD = {B1A, B1B, CCW};

/**
 * @brief Provoca a rotação de motor de forma digital binária (velocidade máxima)
 *
 * @param dir direção da rotação
 * @param pA pino A do motor
 * @param pB pino B do motor
 * @param en habilita a saída para o motor
 */
void rotateDigital(bool dir, byte pA, byte pB, bool en = true)
{
  byte pLow = dir ? pA : pB;
  byte pHigh = dir ? pB : pA;

  digitalWrite(pLow, LOW);
  digitalWrite(pHigh, en);
}

/**
 * @brief Provoca a rotação de motor de forma digital binária (velocidade máxima)
 *
 * @param dir direção de rotação
 * @param mt estrutura contendo informações do motor
 * @param en habilita a saída para o motor
 */
void rotateDigital(bool dir, struct motor mt, bool en = true)
{
  rotateDigital(dir, mt.TA, mt.TA, en);
}

/**
 * @brief Provoca a rotação de um motor de forma analógica (PWM)
 *
 * @param dir direção de rotação
 * @param pA pino A do motor
 * @param pB pino B do motor
 * @param pwm valor de PWM a ser enviado para o pino ativo
 * @param en habilita a saída para o motor
 */
void rotateAnalog(bool dir, byte pA, byte pB, byte pwm, bool en = true)
{
  byte pLow = dir ? pA : pB;
  byte pHigh = dir ? pB : pA;
  byte state = en ? pwm : LOW;

  digitalWrite(pLow, LOW);
  analogWrite(pHigh, state);
}

/**
 * @brief Provoca a rotação de um motor de forma analógica (PWM)
 *
 * @param dir direção de rotação
 * @param mt estrutura contendo informações do motor
 * @param pwm valor de PWM a ser enviado para o pino ativo
 * @param en habilita a saída para o motor
 */
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

/**
 * @brief Função de aceleração dos motores
 *
 * @param from valor inicial do PWM
 * @param to valor final do PWM
 * @param rate taxa de adição do valor de PWM
 */
void accelerate(int from, int to, int rate)
{
  for (int i = from; i < to; i += rate)
  {
    rotateAnalog(MotE.dir, MotE, i);
    rotateAnalog(MotD.dir, MotD, i);
    delay(2);
  }
}

/**
 * @brief Seguir em frente: ativa os motores com a velocidade máxima para frente
 *
 * @param accel ativa ou desativa pequena aceleração para reduzir corrente
 */
void forward(bool accel = true)
{
  if (accel)
  {
    accelerate(30, 255, 2);
  }

  rotateAnalog(MotE.dir, MotE, 255);
  rotateAnalog(MotD.dir, MotD, 255);

#ifdef __DEBUG__
  Serial.println("em frente");
#endif
}

/**
 * @brief Vira a direita: Reduz velocidade do motor direito para provocar curva à direita
 *
 */
void turnRight()
{

  rotateAnalog(MotE.dir, MotE, 255);
  rotateAnalog(MotD.dir, MotD, 100);

#ifdef __DEBUG__
  Serial.println("D");
#endif
}

/**
 * @brief Vira à esquerda: Reduz velocidade do motor ESQUERDO para provocar curva à esquerda
 *
 */
void turnLeft()
{
  rotateAnalog(MotE.dir, MotE, 100);
  rotateAnalog(MotD.dir, MotD, 255);

#ifdef __DEBUG__
  Serial.println("E");
#endif
}

/**
 * @brief Avalia o estado do sensor (SENSOR) e mantém a função de curva até que o sensor do centro
 * seja detectado
 *
 * @param SENSOR sensor de lado a ser avaliado
 * @param funcao função de curva a ser executada
 */
void curvaAoCentro(int SENSOR, void (*funcao)())
{
  if (!digitalRead(SENSOR)) // detecção do sensor de direção
  {
    while (digitalRead(SC)) // loop garante a curva até o centro
    {
#ifdef __DEBUG__
      Serial.print("curva: ");
#endif
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
  Serial.begin(9600);
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
#ifdef __DEBUG__
  Serial.print("Sensores: ");
  Serial.print(digitalRead(SE));
  Serial.print(digitalRead(SC));
  Serial.println(digitalRead(SD));
#endif
  curvaAoCentro(SE, &turnRight);
  curvaAoCentro(SD, &turnLeft);
  delay(50);
  forward(false);
}