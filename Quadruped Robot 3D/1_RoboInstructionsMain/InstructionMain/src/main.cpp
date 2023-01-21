// 2023 E&M Instruction's Main
// Pico W

/******************************
*  Enviar Ordens para o PICO  *
*******************************/
// Materials
// terminal

/****************
*  BIBLIOTECAS  *
*****************/

#include <Arduino.h>
#include <WiFi.h>

#include "pico/cyw43_arch.h"
#include <Wire.h>

#include "wificommands.h"

#include "sonar.h"

/***********
*  MACROS  *
************/

#define SSID "NetworkName"
#define PASS "NetworkPass"

/***********************
*  PINO'S CONNECTIONS  *
************************/

#define LED1 1
#define LED2 2

/**************
*  VARIÁVEIS  *
***************/

float distance, prev_distance;

int LED_state;
unsigned long interval;
unsigned long currentMicros, previousMicros;
int loop_count;
commands_wifi wifi_commands;

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
    //if (LED_intensity > 100) LED_intensity = 100;  // Avoid "impossible" values for LED_intensity

  } else if (command == 'B' || command == 'b') { 
    // MOVE IT! :) Shake that ass, SpiderPig!

  } else if (command == 'C' || command == 'c') { 
    // TURN RIGHT!

  } else if (command == 'D' || command == 'd') { 
    // TURN LEFT!

  } else if (command == 'E' || command == 'e') { 
    // SHOW DISTANCE

  } else if (command == 'F' || command == 'f') { 
    // STOP TO SHOW, I'M ENOUGH OF YOU...

  } else if (command == 'G' || command == 'g') { 
    // SHOW ME WHERE YOU WENT...

  } else if (command == 'H' || command == 'h') { 
    // OK GOOD WORK, YOU ACHIEVED YOUR 10 THOUSAND STEPS, TODAY!

  } // Put here more commands... if you want, of course :)
}

/**********
*  SETUP  *
***********/

void setup() 
{
  interval = 40 * 1000;

  wifi_commands.init(process_command);
  Serial.begin(115200);

  initWiFi();

}

#define CYW43_WL_GPIO_LED_PIN 0

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
  
  // Controlo do ciclo
  currentMicros = micros();

  // THE Control Loop
  if (currentMicros - previousMicros >= interval) {
    previousMicros = currentMicros;

    // Toggle builtin LED    
    loop_count++;
    if (loop_count > 5) {
      LED_state = !LED_state;
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, LED_state);
      loop_count = 0;
    }
  }
}
