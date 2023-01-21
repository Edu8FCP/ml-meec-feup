#include <Arduino.h>
#include <Wire.h>
#include "pico/cyw43_arch.h"
#include "imu.h"

/***********
 * FUNÇÕES *
 ***********/

void ReadIMU(float X_angle, float Y_angle) {

    // LEITURA
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR,14,true);

    // EXTRAÇÃO DOS DADOS
  int16_t ax=Wire.read()<<8|Wire.read();
  int16_t ay=Wire.read()<<8|Wire.read();
  int16_t az=Wire.read()<<8|Wire.read();
  int16_t temp=Wire.read()<<8|Wire.read();
  int16_t gx=Wire.read()<<8|Wire.read();
  int16_t gy=Wire.read()<<8|Wire.read();
  int16_t gz=Wire.read()<<8|Wire.read();

    // CÁLCULO DOS DADOS
  float AcXF = ax / 16394.0 * G;
  float AcYF = ay / 16394.0 * G;
  float AcZF = az / 16394.0 * G;
  float tempF = temp / 340.0 + 36.53;
  float GyXF = gx / 131.0 * 0.0174533;
  float GyYF = gy / 131.0 * 0.0174533;
  float GyZF = gz / 131.0 * 0.0174533;

    // RESULTADOS FINAIS QUE SÃO RETORNADOS
  X_angle = atan(AcZF/AcXF)*180/3.14;
  Y_angle = atan(AcZF/AcYF)*180/3.14;

  if (X_angle < 0) X_angle += 180;
  if (Y_angle < 0) Y_angle += 180;

  X_angle -= 90;
  Y_angle -= 90;
}