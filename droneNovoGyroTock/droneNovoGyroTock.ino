#include <Servo.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <PID_v1.h>
#include <SoftwareSerial.h>

#define LIGA      17  //Mínimo necessário para ligar os ESCS
#define RODA      31  //Mínimo necessário para os motores começarem a rodar    
#define VOO       80 //Mínimo necessário para o drone levantar voo
#define QUANT_MOTOR   4

#define outputLIMIT 15
#define KP 1
#define KI 0
#define KD 0
#define serial 1 //1 = bluetooth, 0 = serial
#define imprimeSensor 1
#define imprimeMotor 0

class Voltimetro
{
  public:
    Voltimetro(int porta)
    {
      this->porta = byte(porta);
    }

    int leitura()
    {
      tensao = float(analogRead(porta)) /67.2727 ;

      return tensao;
    }
    
  protected:
    byte porta;
    float tensao;
};

int pinosPwm[] = {3, 5, 6, 9, 10, 11};
MPU6050 mpu6050(Wire);
Servo motores[QUANT_MOTOR];
long timer = 0;
double input, output, setpoint, outputSomado;

// Pitch, Roll and Yaw values
double pitch = 0;
double roll = 0;
double yaw = 0;

SoftwareSerial bluetooth(12,13); //Serial para o bluetooth - (RX,TX)
PID PIDteste(&pitch, &output, &setpoint, KP, KI, KD, DIRECT);
Voltimetro voltimetro(0);

void print(char texto[100])
{
  if(serial == 1)
  {
    bluetooth.print(texto);
  }
  else if (serial == 0)
  {
    Serial.print(texto);
  }
}

void println(char texto[100])
{
  if(serial == 1)
  {
    bluetooth.println(texto);
  }
  else if (serial == 0)
  {
    Serial.println(texto);
  }
}


void print(float num)
{
  if(serial == 1)
  {
    bluetooth.print(num);
  }
  else if (serial == 0)
  {
    Serial.print(num);
  }
}

void println(float num)
{
  if(serial == 1)
  {
    bluetooth.println(num);
  }
  else if (serial == 0)
  {
    Serial.println(num);
  }
}

void println()
{
   if(serial == 1)
  {
    bluetooth.println();
  }
  else if (serial == 0)
  {
    Serial.println();
  }
}

void pouso()
{
  desligar();
}

void setup() {

  delay(100);

  if(serial == 1)
  {
    bluetooth.begin(9600);
  }
  else if(serial == 0)
  {
    Serial.begin(115200);  
  }
  

  //Inicia os Motores
  println("Attach");
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



  println("Liga os ESCs");
  rodaMotores(LIGA);
  delay(2000); // Tempo entre ligar o ESC e começar a usar.


  println("Comeca a rodar os motores");
  rodaMotores(RODA);
  delay(2000); //DEixa os moteres começarem a rodar

}

void loop() 
{
  if(voltimetro.leitura() < 10)
  {
    pouso(); 
  }
  timer = millis();

  calculaAng();

  PIDteste.Compute();

  motores[0].write((VOO + output)*1); 
  motores[1].write((VOO - output)*1); 
  motores[2].write((VOO - output)*1); 
  motores[3].write((VOO + output)*1);

  if (millis() > 25000)
    desligar();

  enviaLog();
}

void enviaLog()
{
  print("Output= ");
  print(output);
  print(" ");
  print("Tensao= ");
  print(voltimetro.leitura());
  
  if(imprimeMotor == 1)
  {
    print("Motor1(Dir)= "); 
    print(VOO + output);
    print(" ");
    print("Motor2(Esq)= "); 
    print(VOO - output);
    print(" ");
    print("Motor3(Esq)= "); 
    print(VOO - output);
    print(" ");
    print("Motor4(Dir)= "); 
    print(VOO + output);
    print(" ");  
  }
  if(imprimeSensor == 1)
  {
    print("Pitch= ");
    print(pitch);
    print(" ");
    print("Roll= ");
    print(roll);
    print(" ");
    print("Yaw= ");
    print(yaw);
    print(" ");
  }
  println();
}

void desligar() {

  int velocidade = RODA;

  println("Desligando");

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
  print("Pitch = ");
  print(pitch);
  print(" Roll = ");
  print(roll);
  print(" Yaw = ");
  print(yaw);

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
