#include <Arduino.h>
#include <microM.h>
#include <SoftwareSerial.h>
// #define __DEBUG__

/**
 * @brief Pinos de controle dos motores, os pinos foram escolhidos por
 * terem capacidade de realizar PWM, para controle de velocidade.
 * São nomeados de acordo com as entradas do módulo de ponte H utilizado
 *
 */
#define A1A 8
#define A1B 7
#define B1A 5
#define B1B 6

/**
 * @brief Pinos dos sensores de linha, começando com o sensor mais à esquerda
 * (SEE) passando pelo sensor ao centro (SC) até o sensor mais à direita (SDD)
 *
 */
#define SEE 9
#define SES 10
#define SC 11
#define SD 12
#define SDD 13

/**
 * @brief Definição auxiliar para sentido de rotação
 *
 */
#define CW 1  // Horário
#define CCW 0 // Anti-horário

#define MAX_PWM 100
#define RED_FATOR 0.25

/**
 * @brief Constantes de comunicação Bluetooth
 * 
 */
#define BTRX 2
#define BTTX 3
#define BTKEY A5
#define BTBAUD 38400

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

SoftwareSerial btSerial(BTRX, BTTX);
String cmd = "";


/**
 * @brief Para os motores levando todos os pinos de motores para LOW
 *
 */
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
    // rotateAnalog(MotE.dir, MotE, i);
    // rotateAnalog(MotD.dir, MotD, i);
    microM.Motors(i, i, 0, 0);
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

  // rotateAnalog(MotE.dir, MotE, 255);
  // rotateAnalog(MotD.dir, MotD, 255);
  microM.Motors(MAX_PWM, MAX_PWM, 0, 0);

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

  // rotateAnalog(MotE.dir, MotE, MAX_PWM);
  // rotateAnalog(MotD.dir, MotD, MAX_PWM * RED_FATOR);
  microM.Motors(MAX_PWM, MAX_PWM * RED_FATOR, 0, 0);

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
  // rotateAnalog(MotE.dir, MotE, MAX_PWM * RED_FATOR);
  // rotateAnalog(MotD.dir, MotD, MAX_PWM);
  microM.Motors(MAX_PWM * RED_FATOR, MAX_PWM, 0, 0);

#ifdef __DEBUG__
  Serial.println("E");
#endif
}


void setup()
{
  pinMode(MotE.TA, OUTPUT);
  pinMode(MotE.TB, OUTPUT);
  pinMode(MotD.TA, OUTPUT);
  pinMode(MotD.TB, OUTPUT);

  pinMode(BTKEY, OUTPUT);
  btSerial.begin(BTBAUD);

#ifdef __DEBUG__
  Serial.begin(9600);
#endif
  forward();
}

/**
 * @brief Matrizes de estados de sensores, atuais e antigos
 *
 */
bool state[5] = {0, 0, 0, 0, 0}, oldState[5];

/**
 * @brief Matriz de pinos de sensores IR
 *
 */
int dsarr[5] = {SEE, SES, SC, SD, SDD};

/**
 * @brief Flags de desvio e estados individuais.
 *
 */
int stateOut = 0, stateOutOld = 0, detourCount = 0;
bool takeDetour = 0;

/**
 * @brief Atualiza os estados da matriz state e salva os estados antigos na matriz
 * oldState a partir dos valores lidos na matriz de pinos de sensores ds
 *
 * @param ds matriz com os pinos dos sensors IR
 * @param nds numero de sensores na matriz ds
 * @param oldState matriz de estados de sensor salvos da iteração anterior
 * @param state matriz que deverá conter os últimos estados de sensor lidos
 * @param ns numero de estados de sensor
 */
void updateStates(int ds[], int nds, bool oldState[], bool state[], int ns)
{

  for (int i = 0; i < ns; i++)
  {
    oldState[i] = state[i];
  }

  for (int d = 0; d < nds; d++)
  {
    state[d] = !digitalRead(ds[d]);
  }
}

/**
 * @brief Concatena os bits dos estados de sensores
 * em um unico byte representando o estado de maquina
 *
 * @param stateOut estado de saída
 * @param state matriz com os estados a serem concatenados
 */
void concatStates(int &stateOut, bool state[])
{
  stateOut = stateOut | (state[1] << 2);
  stateOut = stateOut | (state[2] << 1);
  stateOut = stateOut | state[3];
}

void loop()
{

  stateOut = 0;

  updateStates(dsarr, 5, oldState, state, 5);

  /**
   * @brief Concatena os bits dos estados de sensores
   * em um unico byte representando o estado de maquina
   *
   */
  concatStates(stateOut, state);


  /**
   * @brief Executa a avaliação principal dos estados da máquina de estados
   *
   */
  if (stateOut != stateOutOld)
  {
    stateOutOld = stateOut;
    switch (stateOut)
    {
    case 2:
      forward(false);
      break;
    case 1:
    case 3:
        turnRight();
      break;
    case 6:
    case 4:
        turnLeft();
      break;
    case 7:
      stop();
    default:
      break;
    }
  }
  delay(5);
}