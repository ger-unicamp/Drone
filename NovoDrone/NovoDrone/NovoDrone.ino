#include <Servo.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <PID_v1.h>
#include <SoftwareSerial.h>

/// <value> Velocidade mínima necessária para ligar os motores </value>
#define LIGA 17 
/// <value> Velocidade mínima necessária para os motores começarem a rodar </value>
#define RODA 31    
/// <value> Velocidade mínima necessária para o drone levantar voo </value>
#define VOO  80
/// <value> Quantidade de motores do drone </value>
#define QUANT_MOTOR   4 

/// <value> Limite de velocidade dos motores </value>
#define outputLIMIT 15 
/// <value> Constante proporcial do PIDPitch </value>
#define KPpitch 1
/// <value> Constante integrativa do PIDPitch </value>
#define KIpitch 0 
/// <value> Constante derivativa do PIDPitch </value>
#define KDpitch 0 

/// <value> <para> Constante de controle da porta de comunicação utilizada </para>
/// <para> 1 = bluetooth,	0 = serial </para> </value> 
#define serial 1 

/// <value> <para> Constante de controle do registro </para> 
/// <para> 0 = não registrar dados dos sensores, 1 = registrar </para> </value>
#define imprimeSensor 1 

/// <value> <para> Constante de controle do registro </para>
/// <para> 0 = não registrar dados dos motores, 1 = registrar </para> </value>
#define imprimeMotor 0 