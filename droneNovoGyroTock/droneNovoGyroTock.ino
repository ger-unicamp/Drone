#include <Servo.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <PID_v1.h>

#define LIGA      17  //Mínimo necessário para ligar os ESCS
#define RODA      31  //Mínimo necessário para os motores começarem a rodar    
#define VOO       80 //Mínimo necessário para o drone levantar voo
#define QUANT_MOTOR   4

#define outputLIMIT 15
#define KP 1
#define KI 0
#define KD 0

int pinosPwm[] = {3, 5, 6, 9, 10, 11};
MPU6050 mpu6050(Wire);
Servo motores[QUANT_MOTOR];
long timer = 0;
double input, output, setpoint, outputSomado;

// Pitch, Roll and Yaw values
double pitch = 0;
double roll = 0;
double yaw = 0;

PID PIDteste(&pitch, &output, &setpoint, KP, KI, KD, DIRECT);



void setup() {

  delay(100);

  Serial.begin(115200);

  //Inicia os Motores
  Serial.println("Attach");
  for (int i = 0; i < QUANT_MOTOR; i++)
    motores[i].attach(pinosPwm[i]); // Anexa os motores com os pinos pwm. ATENÇAO: OS PINOS ESTAO EM ORDEM CRESCENTE
  delay(10);
  rodaMotores(LIGA);
  
  Wire.begin();
  setpoint = 0;
  PIDteste.SetOutputLimits(-outputLIMIT, outputLIMIT);
  PIDteste.SetMode(AUTOMATIC);


  delay(100);
  mpu6050.begin();
  //mpu6050.calcGyroOffsets(true);



  Serial.println("Liga os ESCs");
  rodaMotores(LIGA);
  delay(2000); // Tempo entre ligar o ESC e começar a usar.


  Serial.println("Comeca a rodar os motores");
  rodaMotores(RODA);
  delay(2000); //DEixa os moteres começarem a rodar

}

void loop() {

  timer = millis();

  calculaAng();

  PIDteste.Compute();

  ImprimirSensor();

  Serial.print(" Outout = ");
  Serial.print(output);

  motores[0].write((VOO + output)*1); Serial.print(" Motor 1 (Dir) = "); Serial.print(VOO + output);
  motores[1].write((VOO - output)*1); Serial.print(" Motor 2 (Esq) = "); Serial.print(VOO - output);
  motores[2].write((VOO - output)*1); Serial.print(" Motor 3 (Esq) = "); Serial.print(VOO - output);
  motores[3].write((VOO + output)*1); Serial.print(" Motor 4 (Dir) = "); Serial.print(VOO + output);
  Serial.println(".");

  if (millis() > 25000)
    desligar();
}

void desligar() {

  int velocidade = RODA;

  Serial.println("Desligando");

  while (velocidade >= 0) {
    velocidade -= 5;
    rodaMotores(velocidade);
    delay(500);
  }

  rodaMotores(LIGA);

  while (1);
}

void rodaMotores(int velocidade) {
  for (int i = 0; i < QUANT_MOTOR; i++)
    motores[i].write(velocidade);   //Liga os ESCs
}

void ImprimirSensor() {
  Serial.print("Pitch = ");
  Serial.print(pitch);
  Serial.print(" Roll = ");
  Serial.print(roll);
  Serial.print(" Yaw = ");
  Serial.print(yaw);

  outputSomado = output+VOO;
  if(     outputSomado <  60) outputSomado =  60;
  else if(outputSomado > 180) outputSomado = 180;
}

void calculaAng() {
  mpu6050.update();

  pitch = double(mpu6050.getAngleY())-6.55-0.33;
  roll = double(mpu6050.getAngleX());
  yaw = double(mpu6050.getAngleZ());
}
