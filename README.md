# seguidor-linha-v2
Nova implementação de robo seguidor de linha, versão mais leve e esbelta.

### Sobre a versão 2
O objetivo de uma segunda implementação é expandir o processo de desenvolvimento do robô com etapas de simulação e de uso de tecnologias agora acessíveis, nesse caso, impressão 3D.\
Além do processo, esse robô pode servir como uma base para o desenvolvimento de outros robôs similares em estrutura e funcionamento, diminuindo o esforço do início de um projeto.

## Uso
Este robô possui um adaptador bluetooth para controle e comunicação.\
O nome configurado é `SEGLIN2`, mas o módulo permite configuração via comandos AT quando não conectado.\
Ao ligar o sistema, o robô espera uma conexão com um terminal bluetooth e depende dos comandos para funcionar:
```bash
# Iniciar a peseguição da linha
CM+START

# Parar a perseguição da linha
CM+STOP
```

## Hardware
- DAGU MicroMagician
- TP4056 BMS
- MT3608 step-down
- N20 micro motor
- HC-06 módulo Bluetooth\

\
\
A placa principal escolhida é a **DAGU MicroMagician**, uma placa compacta, compatível com Arduino e que já contém um módulo de controle de motor, receptor de infravermelho e acelerômetro, além de disposição
para servos.\
Para gestão de energia, um BMS (TP4056) para uma célula 18560 e um step-up MT3608.\
Dois motores com redução do tipo N20.\
O módulo utilizado é o HC-06, ou seja, a comunicação AT só é possível enquanto nenhuma conexão com outro dispositivo é estabelecida.
\
\
## Snapshot

![estrutura principal do seguidor v2](/docs/snapshot1.png)