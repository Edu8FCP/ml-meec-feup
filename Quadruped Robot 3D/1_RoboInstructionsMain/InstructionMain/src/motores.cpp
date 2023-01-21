/****************
*  BIBLIOTECAS  *
*****************/

#include <Arduino.h> 
#include <EEPROM.h>
#include "motores.h"

/************
*  FUNÇÕES  *
*************/

int MemorySave(float motores[8][2]){
  EEPROM.write(0, 'A');
  EEPROM.write(1, 'C');
  EEPROM.write(2, 'E');
  for(int indice_motor=0; indice_motor<NR_MOTORES; indice_motor++){
    EEPROM.write(3+indice_motor*4, (int)motores[indice_motor][0]*100 & 0xFF);
    EEPROM.write(4+indice_motor*4, (((int)motores[indice_motor][0]*100)>> 8) & 0xFF);
    EEPROM.write(5+indice_motor*4, (int)motores[indice_motor][1]*100 & 0xFF); // valores de 8 bits
    EEPROM.write(6+indice_motor*4, (((int)motores[indice_motor][1]*100)>> 8) & 0xFF); // valores de 8 bits
  }
  EEPROM.commit();
  Serial.print("Saving EEPROM! ");
  return 0;
}

int MemoryRead(float motores[8][2]){
    if (EEPROM.read(0) == 'A' && EEPROM.read(1) == 'C' && EEPROM.read(2) == 'E') {
    // if the signature is present then we can read the cofiguration from the EEPROM
    for(int indice_motor=0; indice_motor<NR_MOTORES; indice_motor++){
      motores[indice_motor][0] = EEPROM.read(3*indice_motor*4) | (EEPROM.read(4*indice_motor*4) << 8);
      motores[indice_motor][1] = EEPROM.read(5*indice_motor*4) | (EEPROM.read(6*indice_motor*4) << 8); 
      motores[indice_motor][0] = motores[indice_motor][0]/100;
      motores[indice_motor][1] = motores[indice_motor][1]/100; // conversão para float
    }
      return 0; // código de sucesso
    }
    
    return -1; // código de erro
}

int DefaultValues(float motores[8][2]){
    for(int indice_motor=0; indice_motor<NR_MOTORES; indice_motor++){
      motores[indice_motor][0] = 12.5;
      motores[indice_motor][1] = 7.5; 
    }
    return 0;
}