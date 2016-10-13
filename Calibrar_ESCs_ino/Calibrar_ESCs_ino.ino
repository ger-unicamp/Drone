/*
  Configurar ESCs:
    O codigo abaixo envia um pulso pwm proporcional a entrada analogica de um potenciometro, controlando os quatro escs simultameamente
    O objetivo principal  calibrar os escs de forma que todos fiquem iguais
    Ultima atualizaçao: 06/09/2016 - Bruno Eduardo S. de Oliveira
*/

#include <Servo.h> 

#define QUANT_MOTOR 4                  // Definiço de quantos motores serao testados
#define PUSH_BOTT 7                       // Pino analogico ligado ao potenciometro
 
Servo motores[QUANT_MOTOR];            // Cria um vetor de objetos servo, com tamanho QUANT_MOTOR 
int pinosPwm[] = {3, 5, 6, 9, 10, 11}; // Vetor com os pinos que possuem funçao pwm
 
void setup() 
{
  for(int i=0; i<QUANT_MOTOR; i++)
    motores[i].attach(pinosPwm[i]);    // Anexa os motores com os pinos pwm. ATENÇAO: OS PINOS ESTAO EM ORDEM CRESCENTE
  //Serial.begin(9600);                  //Inicia o terminal serial
  pinMode(7, INPUT);
} 
 
void loop() 
{ 
  int val = digitalRead(PUSH_BOTT);        // Le o valor do potenciometro (de 0 a 1023)
  if(val == LOW)
    val = 179;
  else
    val = 16;
  //Serial.println(val);                 // Imprime no terminal o resultado da conversao
  for(int i=0; i<QUANT_MOTOR; i++){
    motores[i].write(val);             // Escreve em cada motor o valor convertido
    //Serial.println(i);                 // Imprime no terminal o resultado da conversao
  }
  
  
} 
