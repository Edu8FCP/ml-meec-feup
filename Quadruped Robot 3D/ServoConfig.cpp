#include <Arduino.h>
// #include <EEPROM.h>
#include <RP2040_PWM.h>

/**
 * @brief 
 * Este programa serve para configurar dinamicamente os Servos quando não se possui
 * um PI PICO com módulo Wi-fi!
 * Usa dois botões!
 * S1 -> aumenta duty cycle a aplicar
 * S2 -> Guarda e avança para o próximo
 */

/**********************
 * DEFINIÇÕES GPIO's  *
***********************/
#define BUTTON1 2
#define BUTTON2 3
#define PinToPWM1 4
#define PinToPWM2 5
#define PinToPWM3 6
#define PinToPWM4 7
#define PinToPWM5 8
#define PinToPWM6 9
#define PinToPWM7 10
#define PinToPWM8 11

/*************************
 * ESTRUTURAS E MÉTODOS  *
**************************/

typedef struct
{
	int state, new_state;

	// tes - time entering state
	// tis - time in state
	unsigned long tes, tis;
} fsm_t;

typedef struct
{
  int noventa_mais;
  int noventa_menos;
  int ID;
} servos;


void set_state(fsm_t &fsm, int new_state)
{
	if (fsm.state != new_state)
	{ // if the state chnanged tis is reset
		fsm.state = new_state;
		fsm.tes = millis();
		fsm.tis = 0;
	}
}

/*************************
 * DECLARAÇÕES VARIÁVEIS *
**************************/

RP2040_PWM* PWM_Instance;

fsm_t fsm1;

float frequency;
float dutyCycle;

u_int8_t S1, prevS1, S2, prevS2;
unsigned long interval, last_cycle, loop_micros;
int zero, noventa_mais, noventa_menos;
servos motores[8];
int posicoes[3]; // noventa_mais, noventa_menos, zero
int i, j;

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  Serial.begin(115200);

  /********************************************
  * ACESSO À FLASH - SERVOS PARAMS GUARDADOS  *
  *********************************************/

  // Start EEPROM emulation
  // EEPROM.begin(512);
  // EEPROM Layout
  // Address  Value
  // 0        65     ->  'A'
  // 1        67     ->  'C'
  // 2        69     ->  'E'
  // 3        blink_period low byte
  // 4        blink_period high byte 

//if (EEPROM.read(0) == 'A' && EEPROM.read(1) == 'C' && EEPROM.read(2) == 'E') {
    // if the signature is present then we can read the cofiguration from the EEPROM
    // for(int i = 0; i < EEPROM.read(3); i++){
    //   motores[i].noventa_mais = EEPRON.read(3+i*2+1) | (EEPROM.read(4+i*2+1) << 8);
    //   motores[i].noventa_menos = EEPRON.read(5+i*2+1) | (EEPROM.read(6+i*2+1) << 8); 
    //}
    //blink_period = EEPROM.read(3) | (EEPROM.read(4) << 8);
  //} else {
    // if not, set it to the default value
    //blink_period = 2000;
  //}

  motores[0].ID = PinToPWM1;
  motores[1].ID = PinToPWM2;
  motores[2].ID = PinToPWM3;
  motores[3].ID = PinToPWM4;
  motores[4].ID = PinToPWM5;
  motores[5].ID = PinToPWM6;
  motores[6].ID = PinToPWM7;
  motores[7].ID = PinToPWM8;

  // INTERVALO EM QUE SE VAI FAZER LEITURAS DO PICO
  interval = 10; 
  i = 0; // controla os serrvos
  j = 0; // controla as posicoes

  //assigns pin 25 (built in LED), with frequency of 50 Hz and a duty cycle of 0%
  PWM_Instance = new RP2040_PWM(motores[0].ID, 50, 0);

  zero =  7.5; // percentagem de duty cycle
  noventa_mais = 10;
  noventa_menos = 5;
  posicoes[0] = noventa_mais;
  posicoes[1] = noventa_menos;
  posicoes[2] = zero;

  set_state(fsm1, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  frequency = 50;
  
  unsigned long now = millis();
	if (now - last_cycle > interval)
	{
		loop_micros = micros();
		last_cycle = now;

		// Read the inputs
		prevS1 = S1;
		prevS2 = S2;
		S1 = !digitalRead(BUTTON1);
		S2 = !digitalRead(BUTTON2);

    /****************************************
    * USO DA BIBLIOTECA PARA MEXER O MOTOR  *
    *****************************************/

    PWM_Instance->setPWM(motores[i].ID, frequency,posicoes[j]);

    /*****************************************
    * CONFIGURAÇÃO POR INTERMÉDIO DOS BOTÕES *
    ******************************************/
    
    if(S1 && !prevS1 && i<8 && fsm1.state == 0){ // aumenta os valores até o pretendido
      posicoes[j] += 0.1;
      Serial.print("Duty Cycle atual: ");
      Serial.print(posicoes[j]);
    }
    if(S1 && !prevS1 && i<8 && fsm1.state == 2){ // aumenta os valores até o pretendido
      posicoes[j] -= 0.1;
      Serial.print("Duty Cycle atual: ");
      Serial.print(posicoes[j]);
    }

    /*********************************************
    * MÁQUINA DE ESTADOS PARA AUMENTAR/DIMINUIR  *
    **********************************************/

    if(S1 && !prevS1 && fsm1.state == 0){
      fsm1.new_state = 1;   // Estado de transição
    } if(S1 && fsm1.tis > 3000 && fsm1.state == 1){
      fsm1.new_state = 2;  // Entra no estado de diminuir
    } if(!S1 && fsm1.state == 1){
      fsm1.new_state = 0;  // Se largar volta ao 0
    } if(S1 && !prevS1 && fsm1.state == 2){
      fsm1.new_state = 3;  // Se carregar quando está no segundo estado
    } if(S1 && fsm1.tis>3000 && fsm1.state == 3){
      fsm1.new_state = 0;  // Volta ao 1
    } if(!S1 && fsm1.state == 3){
      fsm1.new_state = 2; // Se largar volta ao 2
    }


    /******************************
    * GUARDAR O VALOR NA MEMÓRIA  *
    *******************************/
    
    if(S2 && !prevS2 && i<8){ // quando o i chegar a 8 estão todos configurados
      if(j==0){
        j++; // passa para a próxima posicao
      }
      if(j==1){
        posicoes[2] = (posicoes[1] + posicoes[0])/2; // zero está no meio
        i++;
        j=0;
        //EEPROM.write(0, 'A');
        //EEPROM.write(1, 'C');
        //EEPROM.write(2, 'E');
        //EEPROM.write(3, i); // Quantos servos estão gravados
        //EEPROM.write(3+i*2+1, posicoes[1] & 0xFF);          // blink_period low byte
        //EEPROM.write(4+i*2+1, (posicoes[1] >> 8) & 0xFF);   // blink_period high byte 
        //EEPROM.write(5+i*2+1, posicoes[2] & 0xFF);          // blink_period low byte
        //EEPROM.write(6+i*2+1, (posicoes[2] >> 8) & 0xFF);   // blink_period high byte         
        //EEPROM.commit();
        Serial.print("Saving EEPROM! ");
        posicoes[0] = noventa_mais;
        posicoes[1] = noventa_menos;
        posicoes[2] = zero;
        Serial.print("Vamos configurar o servo nº: ");
        Serial.print(i);
      }
    }
    if(i==8){
      Serial.println("Todos os Servos estão configurados");
    }
  }

  // UPDATE STATE
  set_state(fsm1, fsm1.new_state);
}