#include <Arduino.h>
#include "sonar.h"

/************
*  FUNÇÕES  * 
*************/

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

void ActAverage(float MediaMovel[5], float distance, float SomaAtual, int filtro){
  if((distance > (SomaAtual/5)*(3/2) || distance < (SomaAtual/5)*(1/2)) && 0<=filtro<2){
    Serial.println("Elemento ""estranho"" detetado");
    filtro++;
  } else filtro--;

  if(filtro == 2){ // duas amostras "estranhas" consecutivas
    Serial.println("Dois elementos ""estranhos"" consecutivos");
    filtro = -5;
  }

  if(0<=filtro<2){  // caso normal, onde descarta 1 amostra "estranha"
    for(int i=4; i>0; i++){
      MediaMovel[i] = MediaMovel[i-1]; 
      SomaAtual += MediaMovel[i];
    }
    MediaMovel[0] = distance; // adiciona o novo elemento
    SomaAtual += MediaMovel[0];

    filtro = 0;
  }

  if(filtro<0){  // caso em que ocorreram 2 amostras "estranhas" seguidas e então preenche "de novo" o filtro
    for(int i=4; i>0; i++){
      MediaMovel[i] = MediaMovel[i-1]; 
      SomaAtual += MediaMovel[i];
    }
    MediaMovel[0] = distance; // adiciona o novo elemento
    SomaAtual += MediaMovel[0];

    filtro++;
  }
}