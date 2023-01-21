/************************************
*  LEITURA DE DISTÂNCIAS COM SONAR  *
*************************************/
// -> Usa um sonar, que usa dois pinos ECHO e TRIG
// -> VCC de 5V!!!
// -> Trig (Trigger) pin is used to trigger ultrasonic sound pulses. 
// By setting this pin to HIGH for 10µs, the sensor initiates an ultrasonic burst.
// -> Echo pin goes high when the ultrasonic burst is transmitted and remains high until the 
// sensor receives an echo, after which it goes low. 
// By measuring the time the Echo pin stays high, the distance can be calculated.
// -> https://lastminuteengineers.com/arduino-sr04-ultrasonic-sensor-tutorial/#:~:text=An%20HC%2DSR04%20ultrasonic%20distance,listens%20for%20the%20transmitted%20pulses.

#include "Arduino.h"

/***********************
*  DEFINIÇÃO DE PINOS  *
************************/

const unsigned int TRIG_PIN=15;
const unsigned int ECHO_PIN=14;
const unsigned int BAUD_RATE=9600;

/**************
*  VARIÁVEIS  *
***************/

float SpeedOfSound, Distance, Temp;
long Duration;

/***********************
*  CÁLCULO VELOCIDADE  *
************************/
// Ver como se lê a temperatura

float CalcSpeed(float Temp){
  return 331+0.6*Temp;
}

/**********************
*  CÁLCULO DISTÂNCIA  * - em cm
***********************/

float CalcDist(float Speed, float Time){
  // Ver conversão necessária para o Tempo estar em m/s
  // Ver limite máximo para o tempo (limite do sonar)
  Time = Time*(10^-6); // converter para s
  Time = Time/2; // Ida e Volta
  Serial.println(Time);
  float distance = Speed * Time;
  Serial.println(Speed);
  distance = distance/100; // converter para cm
  Serial.println(distance);
  return distance;
}

/******************
*  INICIALIZAÇÃO  *
*******************/

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(BAUD_RATE);
}

/**********************
*  CICLO DE LEITURAS  *
***********************/

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // function that reads a pulse (if HIGH waits for LOW and vice-versa)
  // works well for pulses between 10us e 3min
  Duration = pulseIn(ECHO_PIN, HIGH);
  
  if(Duration==0){
   Serial.println("Warning: no pulse from sensor");
   } 
  else{
      Temp = 10;

      SpeedOfSound = CalcSpeed(Temp);
      Serial.print("distance to nearest object:");
      Distance = CalcDist(SpeedOfSound, Duration);
      Serial.println(Distance);
      Serial.println(" cm ");
      Serial.println(Duration);
  }
  delay(1000);
}