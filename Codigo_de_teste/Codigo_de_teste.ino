
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
#define TEMPODEVOO 60000

Servo motores[QUANT_MOTOR];               // Cria um vetor de objetos servo, com tamanho QUANT_MOTOR
int pinosPwm[] = {
  3, 5, 6, 9, 10, 11};    // Vetor com os pinos que possuem funçao pwm

NewPing sensor(PINOTRIG, PINOECHO, 200);  // Sensor da newping

int val;

void ligar();
void decolar();
void estabilizar();
void pousar();
void desligar();

void setup()
  {
  pinMode(13, OUTPUT);
  ligar();
  val = RODA;
  decolar();
  //Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  estabilizar();
  pousar();
  }

void loop()
  {
  desligar();
  }

void ligar()
  {
  delay(10000);
  for (int i = 0; i < QUANT_MOTOR; i++)
    motores[i].attach(pinosPwm[i]);     // Anexa os motores com os pinos pwm. ATENÇAO: OS PINOS ESTAO EM ORDEM CRESCENTE

  for (int i = 0; i < QUANT_MOTOR; i++)
    motores[i].write(LIGA);             //Liga os ESCs

  delay(2000);

  for (int i = 0; i < QUANT_MOTOR; i++)
    motores[i].write(RODA);             //Começa a rodar os motores
  }

void decolar()
  {
  while (val < LIMITE) 
    {
    for (int i = 0; i < QUANT_MOTOR; i++) 
      {
      motores[i].write(val);
      }
    delay(1000);
    val += PASSO;     //Incrementa a rotação dos motores pelo valor de PASSO
    }
  digitalWrite(13, HIGH);
  val -= PASSO;
  }

void estabilizar()
  {
  //Todo
  int count = 0;
  while(count < (TEMPODEVOO/TEMPOATUALIZA))
    {
    int alturaLida = sensor.convert_cm(sensor.ping_median(5));
    //Serial.print("Ping: "); Serial.print(alturaLida); Serial.println("cm");
    if(alturaLida < ALTURA)
      {
      val++;
      digitalWrite(13, HIGH);  //Led acesso subindo
      }
    else if(alturaLida > ALTURA)
      {
      val--;
      digitalWrite(13, LOW); //Led apago descendo
      }
    for (int i = 0; i < QUANT_MOTOR; i++) 
      {
      motores[i].write(val);
      }
    delay(TEMPOATUALIZA);
    count++;
    }
  }

void pousar()
  {
  while (val > 0) 
    {
    for (int i = 0; i < QUANT_MOTOR; i++)
      {
      motores[i].write(val);
      }
    delay(1000);
    //val -= 1;       //Delisga os motores devagar
    val = 0;          //Delisga os motores de uma vez
    if (val < 80)
      val = 0;
    }
  }

void desligar()
  {
  while (1) 
    {
    for (int i = 0; i < QUANT_MOTOR; i++) 
      {
      motores[i].write(0);    //Encerra os ESCs e motores
      }
    }
  }
