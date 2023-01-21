#ifndef SONAR_H
#define SONAR_H

/******************
*  LIBS REQUIRED  *
*******************/

#include <Arduino.h>

float CalcSpeed(float Temp);
/* Calcula a velocidade do som
 Temp - temperatura do ar
 Retorna - a velocidade em m/s
*/

float CalcDist(float Speed, float Time);
/* Calcula a distância
  Speed - a velocidade do ar
  Time - o tempo de voo recebido pelo Sonar
  Retorna - a distância em cm
*/

#endif