/*
  Configurar ESCs:
  O codigo abaixo envia um pulso pwm proporcional a entrada analogica de um potenciometro, controlando os quatro escs simultameamente
  O objetivo principal  calibrar os escs de forma que todos fiquem iguais
  Ultima atualizaçao: 25/10/2016 - Bruno Eduardo S. de Oliveira
*/

#include <Servo.h>
#include <NewPing.h>

#define QUANT_MOTOR   4                   // Definição de quantos motores serao testados
#define LIGA         17                   // Parametros de largura de pulso (medidos empiricamente)
#define RODA         32
#define PASSO         5
#define LIMITE      110

#define PINOECHO 11
#define PINOTRIG 12

#define ALTURA 5                          // Altura (em CM) para estabilizar

#define TEMPOATUALIZA 100
#define TEMPODEVOO 10000

Servo motores[QUANT_MOTOR];               // Cria um vetor de objetos servo, com tamanho QUANT_MOTOR
int pinosPwm[] = {3, 5, 6, 9, 10, 11};    // Vetor com os pinos que possuem funçao pwm

NewPing sensor(PINOTRIG, PINOECHO, 200);  // Sensor da newping

int val;

void setup() {
  void ligar();
  val = RODA;
  decolar();
  estabilizar();
  pousar();
}

void loop() {
  desligar();
}

void ligar() {
  delay(10000);
  for (int i = 0; i < QUANT_MOTOR; i++)
    motores[i].attach(pinosPwm[i]);     // Anexa os motores com os pinos pwm. ATENÇAO: OS PINOS ESTAO EM ORDEM CRESCENTE

  for (int i = 0; i < QUANT_MOTOR; i++)
    motores[i].write(LIGA);             //Liga os ESCs

  delay(2000);

  for (int i = 0; i < QUANT_MOTOR; i++)
    motores[i].write(RODA);             //Começa a rodar os motores
}

void decolar() {
  while (val < LIMITE) {
    for (int i = 0; i < QUANT_MOTOR; i++) {
      motores[i].write(val);
    }
    delay(1000);
    val += PASSO;        //Incrementa a rotação dos motores pelo valor de PASSO
  }

  val -= PASSO;
}

void estabilizar() {
  //Todo
  int count = 0;
  while(count < (TEMPODEVOO/TEMPOATUALIZA)){
    int alturaLida = sensor.ping_cm();
    if(alturaLida < ALTURA)
      val++;
    else if(alturaLida > ALTURA)
      val--;
    delay(TEMPOATUALIZA);
    count++;
  }
      
  
}

void pousar() {
  while (val > 0) {
    for (int i = 0; i < QUANT_MOTOR; i++) {
      motores[i].write(val);
    }
    delay(1000);
    val -= 1;        //Delisga os motores devagar
    if (val < 80)
      val = 0;
  }
}

void desligar() {
  while (1) {
    for (int i = 0; i < QUANT_MOTOR; i++) {
      motores[i].write(0);    //Encerra os ESCs e motores
    }
  }
}
