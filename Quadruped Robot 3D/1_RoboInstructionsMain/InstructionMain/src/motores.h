#ifndef MOTORES_H
#define MOTORES_H

/******************
*  LIBS REQUIRED  *
*******************/

#include <Arduino.h>

/***********
*  MACROS  * 
************/

#define NR_MOTORES 8

/************************
 * VARIÁVEIS REQUERIDAS *
 ************************/

// motores[8][2] - estrutura com os valores de movimento
// posicao[8][2] - posicao dos motores

/*********************
*  LISTA DE FUNÇÕES  *
**********************/

int MemorySave(float motores[8][2]);
/* Escreve os valores dos motores em memória
    motores[8][2] - estrutura que guarda os valores dos motores +90, -90
    retorno:   
        0 - funcionou
*/

int MemoryRead(float motores[8][2]);
/* Lê os valores dos motores da memória
    motores[8][2] - estrutura que guarda os valores dos motores +90, -90
    retorno:   
        0 - funcionou
        -1 - deu erro
*/

int DefaultValues(float motores[8][2]);
/*
    Caso a leitura da memória falhe, podemos optar por ir buscar os valores default
    retorno:
        0 - funcionou
*/


#endif