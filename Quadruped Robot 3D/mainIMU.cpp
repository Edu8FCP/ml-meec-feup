/********************************************
*  MAPEAMENTO COM IMU VELOCIDADE + ROTAÇÃO  *
*********************************************/

#include <Arduino.h>
#include <Wire.h>
#include "pico/cyw43_arch.h"

// I2C - two wire
// SDA - Serial Data Line, SCL - Serial Clock Line, common ground
// master-slave. master clocks the bus, addresses slaves and writes/read data
// slaves - respond only when interrogated, through their unique address
// START - SDA vai de HIGH para LOW, enquanto SCL HIGH
// STOP - SDA vai de Low para HIGH quando SCL HIGH

// 1 IMU

/**********
 * MACROS *
 **********/

// Define pinos para I2C
#define SDA_PIN 20
#define SCL_PIN 21
#define MPU_ADDR 0x68 // endereço do slave
#define G 9.8067
#define CYCLE_INTERVAL 10

/*************
 * VARIÁVEIS *
 *************/

float X_angle, Y_angle;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t temp;

/***********
 * FUNÇÕES *
 ***********/

void readIMU() {
  Wire.beginTransmission(MPU_IMU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_IMU,14,true);
  int16_t ax=Wire.read()<<8|Wire.read();
  int16_t ay=Wire.read()<<8|Wire.read();
  int16_t az=Wire.read()<<8|Wire.read();
  int16_t temp=Wire.read()<<8|Wire.read();
  int16_t gx=Wire.read()<<8|Wire.read();
  int16_t gy=Wire.read()<<8|Wire.read();
  int16_t gz=Wire.read()<<8|Wire.read();

  float AcXF = ax / 16394.0 * G;
  float AcYF = ay / 16394.0 * G;
  float AcZF = az / 16394.0 * G;
  float tempF = temp / 340.0 + 36.53;
  float GyXF = gx / 131.0 * 0.0174533;
  float GyYF = gy / 131.0 * 0.0174533;
  float GyZF = gz / 131.0 * 0.0174533;

  X_angle = atan(AcZF/AcXF)*180/3.14;
  Y_angle = atan(AcZF/AcYF)*180/3.14;

  if (X_angle < 0) X_angle += 180;
  if (Y_angle < 0) Y_angle += 180;

  X_angle -= 90;
  Y_angle -= 90;
}


/*****************
 * INICIALIZAÇÃO *
 *****************/

void setup()
{
  // Start the serial port with 115200 baudrate
  Serial.begin(115200);

  delay(5000);

  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);

  Wire.begin();
  Wire.beginTransmission(MPU_IMU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  delay(1000);

}

/*********************
 * CICLO DE CONTROLO *
 *********************/

void loop()
{
  unsigned long now = millis();
  if (now - last_cycle > CYCLE_INTERVAL) {
    last_cycle = now;

    // Reads data from IMU
    readIMU();

    // temp = Wire.read() << 8 | Wire.read();

    Serial.print("ax: ");
    Serial.print(ax);
    Serial.print(" ay: ");
    Serial.print(ay);
    Serial.print(" az: ");
    Serial.print(az);
    Serial.print(" gx: ");
    Serial.print(gx);
    Serial.print(" gy: ");
    Serial.print(gy);
    Serial.print(" gz: ");
    Serial.print(gz);
    Serial.println();
  }
}