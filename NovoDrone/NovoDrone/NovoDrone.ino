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

