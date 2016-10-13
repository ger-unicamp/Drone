/*
  Configurar ESCs:
    O codigo abaixo envia um pulso pwm proporcional a entrada analogica de um potenciometro, controlando os quatro escs simultameamente
    O objetivo principal  calibrar os escs de forma que todos fiquem iguais
    Ultima atualizaçao: 06/09/2016 - Bruno Eduardo S. de Oliveira
*/

#include <Servo.h> 

#define QUANT_MOTOR 4                  // Definiço de quantos motores serao testados
#define PUSH_BOTTON 7                  // Pino analogico ligado ao potenciometro
 
Servo motores[QUANT_MOTOR];            // Cria um vetor de objetos servo, com tamanho QUANT_MOTOR 
int pinosPwm[] = {3, 5, 6, 9, 10, 11}; // Vetor com os pinos que possuem funçao pwm

int val = 5;
 
void setup() 
{
  for(int i=0; i<QUANT_MOTOR; i++)
    motores[i].attach(pinosPwm[i]);    // Anexa os motores com os pinos pwm. ATENÇAO: OS PINOS ESTAO EM ORDEM CRESCENTE
  Serial.begin(9600);                  //Inicia o terminal serial
  pinMode(PUSH_BOTTON, INPUT);
} 
 
void loop() 
{
  if(digitalRead(PUSH_BOTTON))
  {
    for(int i=0; i<QUANT_MOTOR; i++)
      motores[i].write(val);           // Escreve em cada motor o valor convertido
    Serial.println(val); 
    delay(1000);
    val = (val+1)%180;
  }
} 
