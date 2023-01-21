// 2023 E&M Instruction's Main
// Pico W

/******************************
*  Enviar Ordens para o PICO  *
*******************************/
// Materials
// PI PICO - Vcc - pino 36; Ground - 3,8,13,18,23,28,33,38
// terminal
// Wifi-connection
// IMU
// Sonar
// Servos SG90 (x8)

/****************
*  BIBLIOTECAS  *
*****************/

#include <Arduino.h>
#include <WiFi.h>

#include "pico/cyw43_arch.h"
#include <Wire.h>

#include "wificommands.h"

#include "sonar.h"

#include "imu.h"

#include "motores.h"

/***********
*  MACROS  *
************/

#define SSID "NetworkName"
#define PASS "NetworkPass"
#define CYCLE_INTERVAL 10

/***********************
*  PINO'S CONNECTIONS  *
************************/

#define SDA_PIN 20 // IMU
#define SCL_PIN 21
#define ECHO_PIN 7 // SONAR
#define TRIG_PIN 8
#define LED1 1 // LEDS
#define LED2 2
// BUTTONS
// MOTORES

/*********************
* MÁQUINA DE ESTADOS *
**********************/

typedef struct {
  int state, new_state;

  // tes - time entering state
  // tis - time in state
  unsigned long tes, tis;
} fsm_t;

/**************
*  VARIÁVEIS  *
***************/

// Sonar
float Distance;
float DistanceAverage[5];
int filtro, SomaTotal = 0;
float SpeedOfSound, Temp, Time;

// Controlo do Ciclo
unsigned long interval;
unsigned long currentMicros, previousMicros;
int loop_count;

// Wireless Mode
commands_wifi wifi_commands;

//IMU
float X_Angle, Y_Angle;

// Controlo de mostradores/movimento
bool FlagIMU = false;
bool FlagSonar = false;
bool FlagMotores = false;
bool FlagEsquerda = false;
bool FlagDireita = false;

// Motores
float motores[8][2];
float posicao[8][2];

/*******************************
*  FUNÇÕES GENÉRICAS DE APOIO  *
********************************/

void PrintController(bool FlagIMU, bool FlagSonar, bool FlagMovimento){
  if(FlagIMU || FlagSonar || FlagMotores){
    Serial.print("Loop Count: ");
    Serial.print(loop_count);
  }
  
  if(FlagIMU){
    Serial.print("X_Angle: ");
    Serial.print(X_Angle);
    Serial.print(" Y_Angle: ");
    Serial.println(Y_Angle);
  }

  if(FlagSonar){
    Serial.print("Distance (cm): ");
    Serial.print(Distance);
  }
}

/*********************************
*  Network Connection w/ MACROS  *
**********************************/

void initWiFi() {
  //WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("Wifi local IP: ");
  Serial.println(WiFi.localIP());
}

/******************************
*  Lista de Comandos do robo  *
*******************************/

void process_command(char command, float value)
{
  if (command == 'A' || command == 'a') {  // The 'L' command sets the LED_intensity to the value that follows
    // STOP MOTION
    FlagMotores = false;

  } else if (command == 'B' || command == 'b') { 
    // MOVE IT! :) Shake that ass, SpiderPig!
    FlagMotores = true;

  } else if (command == 'C' || command == 'c') { 
    // TURN RIGHT!
    FlagDireita = true;
    FlagEsquerda = false;
    FlagMotores = false;

  } else if (command == 'D' || command == 'd') { 
    // TURN LEFT!
    FlagEsquerda = true;
    FlagDireita = false;
    FlagMotores = false;

  } else if (command == 'E' || command == 'e') { 
    // STOP TURN, otherwise you'll get dizzy
    FlagDireita = false;
    FlagEsquerda = false;

  } else if (command == 'F' || command == 'f') { 
    // SHOW DISTANCE
    FlagSonar = true;

  } else if (command == 'G' || command == 'g') { 
    // STOP TO SHOW, I'M ENOUGH OF YOU...
    FlagSonar = false;

  } else if (command == 'H' || command == 'h') { 
    // SHOW ME WHERE YOU WENT...
    FlagIMU = true;

  } else if (command == 'I' || command == 'i') { 
    // OK GOOD WORK, YOU ACHIEVED YOUR 10 THOUSAND STEPS, TODAY!
    FlagIMU = false;

  } // Put here more commands... if you want, of course :)
}

float ReadSonar(float Speed, float Temp, float Time){
  digitalWrite(TRIG_PIN, LOW); // TODO - tirar estes delays
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // function that reads a pulse (if HIGH waits for LOW and vice-versa)
  // works well for pulses between 10us e 3min
  int Duration = pulseIn(ECHO_PIN, HIGH);

  if(Duration==0){
    Serial.println("Warning: no pulse from sensor");
  } 

  float Speed = CalcSpeed(Temp);
  float Distance = CalcDist(Speed, Time);

  return Distance;
}

/**********
*  SETUP  *
***********/

void setup() 
{
  interval = 40 * 1000;

  // Inicializações de plataformas
  Serial.begin(115200);
  initWiFi();

  wifi_commands.init(process_command);

  // Inicialização de variáveis
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Temp = 20;

}

/*********************
*  LOOP DE CONTROLO  *
**********************/

void loop() 
{
  /************************************
  *  LEITURA PARA PROCESSAR COMANDOS  *
  *************************************/
  // MUDAR PARA WI-FI
  uint8_t b;
  if (Serial.available()) {  // Only do this if there is serial data to be read
      
    b = Serial.read();    
    wifi_commands.process_char(b);

  }  
  
  // Contagem de controlo do ciclo
  currentMicros = micros();

  // THE Control Loop
  if (currentMicros - previousMicros >= CYCLE_INTERVAL) {
    previousMicros = currentMicros;

    // Loop Count 
    loop_count++;

    // State Machines Process
    Distance = ReadSonar(SpeedOfSound, Temp, Time);
    ReadIMU(X_Angle, Y_Angle);

    // Tratamento dos Dados

    // Imprimir no Terminal
    PrintController(FlagIMU, FlagSonar, FlagMotores);
  }
}
