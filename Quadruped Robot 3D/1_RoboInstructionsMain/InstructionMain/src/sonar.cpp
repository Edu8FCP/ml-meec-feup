#include <Arduino.h>
#include "sonar.h"

/***********
*  MACROS  *
************/

#define TIME_LIMIT 38000 //38ms
#define TIME_MIN 10 // 10us

float CalcSpeed(float Temp){
  return 331+0.6*Temp;
}

float CalcDist(float Speed, float Time){
  // Ver conversão necessária para o Tempo estar em m/s
  // Ver limite máximo para o tempo (limite do sonar)
    if (Time > TIME_LIMIT || Time < TIME_MIN){
        Serial.println("Exception - Sonar couldn't read");
    }

    Time = Time*(10^-6); // converter para s
    Time = Time/2; // Ida e Volta

    float distance = Speed * Time;

    distance = distance/100; // converter para cm

    return distance;
}