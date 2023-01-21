#ifndef IMU_H
#define IMU_H

/***************
 * BIBLIOTECAS *
 ***************/

#include <Arduino.h>
#include <Wire.h>
#include "pico/cyw43_arch.h"

/**********
 * MACROS *
 **********/

#define MPU_ADDR 0x68 // endereço do slave
#define G 9.8067

/************************
 * VARIÁVEIS REQUERIDAS *
 ************************/

// Ax,y,z  - aceleração em 3 eixos
// Gx,y,z  - giroscópio em 3 eixos
// X_Angle, Y_Angle

/*************
 *  FUNÇÕES  *
 *************/

void ReadIMU(float X_Angle, float Y_Angle);

/* readIMU
    Inputs: X_Angle, Y_Angle
*/

#endif