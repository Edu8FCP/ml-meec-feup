/*******************************
*  INSERIR EM MEMÓRIA MOTORES  *
********************************/
// Materials
// 2 LED - um sinaliza que foi guardado o outro que estamos a alterar
// 2 BUTTON - um para guardar outro para aumentar/diminuir
// S1 - TROCAR MOTOR, S2 - TROCAR LADO
// 1 Motor
// terminal

#include <Arduino.h>
#include <EEPROM.h>

/************************************
*  DEFINIÇÃO DE CONSTANTES E PINOS  *
*************************************/

#define NR_MOTORES 1

#define MOTOR 8

#define LED_1 6
#define LED_2 7

#define B_S1 2
#define B_S2 3

/*********************
* MÁQUINA DE ESTADOS *
**********************/

typedef struct {
  int state, new_state;

  // tes - time entering state
  // tis - time in state
  unsigned long tes, tis;
} fsm_t;

// Input variables
uint8_t S1, prevS1;
uint8_t S2, prevS2;
// Botão S1 para inserir

// Output variables
uint8_t LED1, LED2;
float Motor1;
// LED1 para sinalizar a inserção

// Our finite state machines
fsm_t fsm1, fsm2;

// Estrutura dos motores
// +90, -90
// Tem de ser convertido para inteiros para guardar na memória, depois ao ler, volta para float
float motores[8][2] = {{75, 125}, {75, 125}, {75, 125}, {75, 125}, {75, 125}, {75, 125}, {75, 125}, {75, 125}};
uint8_t indice_motor;
uint8_t indice_lado;

unsigned long interval, last_cycle;
unsigned long loop_micros;

/************
*  FUNÇÕES  *
*************/

// Set new state
void set_state(fsm_t& fsm, int new_state)
{
  if (fsm.state != new_state) {  // if the state chnanged tis is reset
    fsm.state = new_state; // Atualiza estado
    fsm.tes = millis(); // tes - tempo de entrada
    fsm.tis = 0; // tis - tempo no estado
  }
}

// Ler da memória e inicializar motores
void motores_init(float motores[8][2]){
    if (EEPROM.read(0) == 'A' && EEPROM.read(1) == 'C' && EEPROM.read(2) == 'E') {
    // if the signature is present then we can read the cofiguration from the EEPROM
    for(indice_motor=0; indice_motor<NR_MOTORES; indice_motor++){
      motores[indice_motor][0] = EEPROM.read(3*indice_motor*4) | (EEPROM.read(4*indice_motor*4) << 8);
      motores[indice_motor][1] = EEPROM.read(5*indice_motor*4) | (EEPROM.read(6*indice_motor*4) << 8); 
      motores[indice_motor][0] = motores[indice_motor][0]/100;
      motores[indice_motor][1] = motores[indice_motor][1]/100; // conversão para float
    }
    indice_motor = 0;
  } else {
    // if not, set it to the default value
    for(indice_motor=0; indice_motor<NR_MOTORES; indice_motor++){
      motores[indice_motor][0] = 12.5;
      motores[indice_motor][1] = 7.5; 
    }
    indice_motor = 0;
  }

}

// Ações de inicialização
// LER DA MEMÓRIA

/**********************
*    INICIALIZAÇÃO    *
***********************/

void setup() 
{
  // Inicializar pinos
  pinMode(LED_1, OUTPUT);
  pinMode(B_S1, INPUT_PULLUP);
  pinMode(LED_2, OUTPUT);
  pinMode(B_S2, INPUT_PULLUP);
  pinMode(Motor1, OUTPUT);


  // Start the serial port with 115200 baudrate
  Serial.begin(115200);

  // Start EEPROM emulation
  EEPROM.begin(512);
  // EEPROM Layout
  // Address  Value
  // 0        65     ->  'A'
  // 1        67     ->  'C'
  // 2        69     ->  'E'
  // 3        blink_period low byte
  // 4        blink_period high byte 

  // indice dos motores
  indice_motor = 0;
  indice_lado = 0;

  /**************************
  *    INICIA OS MOTORES    *
  ***************************/

  motores_init(motores);

  // intervalo de leituras
  interval = 10;
  set_state(fsm1, 0);
  set_state(fsm2, 0);    
}

/*************************
* CICLO DE PROCESSAMENTO *
**************************/

void loop() 
{
    uint8_t b;
    if (Serial.available()) {  // Only do this if there is serial data to be read
      b = Serial.read();       
      if (b == '-') motores[indice_motor][indice_lado]-=0.1;  // Press '-' to decrease the frequency
      if (b == '+') motores[indice_motor][indice_lado]+=0.1;  // Press '+' to increase the frequency
      if (b == '*') {  // Save blink_period to EEPROM
        LED1 = 1;
        EEPROM.write(0, 'A');
        EEPROM.write(1, 'C');
        EEPROM.write(2, 'E');
        for(indice_motor=0; indice_motor<NR_MOTORES; indice_motor++){
          EEPROM.write(3+indice_motor*4, (int)motores[indice_motor][0]*100 & 0xFF);
          EEPROM.write(4+indice_motor*4, (((int)motores[indice_motor][0]*100)>> 8) & 0xFF);
          EEPROM.write(5+indice_motor*4, (int)motores[indice_motor][1]*100 & 0xFF); // valores de 8 bits
          EEPROM.write(6+indice_motor*4, (((int)motores[indice_motor][1]*100)>> 8) & 0xFF); // valores de 8 bits
        }
        EEPROM.commit();
        Serial.print("Saving EEPROM! ");
      }
    }  
    // To measure the time between loop() calls
    //unsigned long last_loop_micros = loop_micros; 
    
    // Do this only every "interval" miliseconds 
    // It helps to clear the switches bounce effect
    unsigned long now = millis();
    if (now - last_cycle > interval) {
      loop_micros = micros();
      last_cycle = now;
      
      // Read the inputs
      prevS1 = S1;
      S1 = !digitalRead(B_S1);
      prevS2 = S2;
      S2 = !digitalRead(B_S2);

      // FSM processing

      // Update tis for all state machines
      unsigned long cur_time = millis();   // Just one call to millis()
      fsm1.tis = cur_time - fsm1.tes;
      fsm2.tis = cur_time - fsm2.tes; 

      // Calculate next state for the first state machine
      if (fsm1.state == 0 && S1 && !prevS1){
        fsm1.new_state = 1;
      } else if (fsm1.state == 1 && S1 && !prevS1){
        fsm1.new_state = 2;
        if(indice_motor == NR_MOTORES){
          indice_motor=0;
        } else indice_motor++;
      } else if(fsm1.state == 1 && fsm1.tis > 500){
        fsm1.new_state = 0;
      } else if(fsm1.state == 2){
        fsm1.new_state = 0;
      }

      // Calculate next state for the second state machine
      if (fsm2.state == 0 && S2 && !prevS2){
        fsm2.new_state = 1;
        indice_lado = 1;
      } else if (fsm2.state == 1 && S2 && !prevS2){
        fsm2.new_state = 0;
        indice_lado = 0;
      }

      // Update the states
      set_state(fsm1, fsm1.new_state);
      set_state(fsm2, fsm2.new_state);

      // OUTPUTS
      LED2 = indice_lado;
      Motor1 = motores[indice_motor][indice_lado];

      // Set the outputs - passa para os dispositivos físicos
      digitalWrite(LED_1, LED1);
      digitalWrite(LED_2, LED2);
      analogWrite(MOTOR, Motor1);

      // Debug using the serial port
      Serial.print(" Índice ");
      Serial.println(indice_motor);

      Serial.print(" Motor_Mais ");
      Serial.println(motores[indice_motor][0]);

      Serial.print(" Motor_Menos: ");
      Serial.println(motores[indice_motor][1]);

      Serial.print(" loop: ");
      Serial.println(micros() - loop_micros);
    }
    
}

