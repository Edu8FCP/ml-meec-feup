#ifndef SONAR_H
#define SONAR_H

/******************
*  LIBS REQUIRED  *
*******************/

#include <Arduino.h>

/***********
*  MACROS  * 
************/

#define TIME_LIMIT 38000 //38ms
#define TIME_MIN 10 // 10us

/************************
 * VARIÁVEIS REQUERIDAS *
 ************************/

// SpeedOfSound  - velocidade do Som
// Distance - distância do objeto
// MediaMovel[5] - filtro
// SomaAtual - soma dos valores do filtro

/*********************
*  LISTA DE FUNÇÕES  *
**********************/

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

void ActAverage(float MediaMovel[5], float distance, float SomaAtual, int filtro);
/* Atualiza a média móvel de janela 5
  MediaMovel - vetor onde estão guardados
  distance - valor lido
  SomaAtual - soma dos valores que estão no filtro
  filtro - int que sinaliza se existiu um valor estranho ou não. Janela -5...0,1,2. Começa a 0
  Função - verifica se há algum valor "estranho"
  */

#endif