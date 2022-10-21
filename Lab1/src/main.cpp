#include <Arduino.h>
//#include <EEPROM.h> // Biblio para salvar config na flash

/***************************************
*                                      *
*             DECLARAÇÕES              *
*                                      *
* *************************************/

// Define dos GPIO que vamos querer controlar
#define LED1 6
#define LED2 7
#define LED3 8
#define LED4 9
#define LED5 10
#define LED6 11
#define LED7 12
#define BUTTON1 2
#define BUTTON2 3

typedef struct
{
	int state, new_state;

	// tes - time entering state
	// tis - time in state
	unsigned long tes, tis;
} fsm_t;

/**********************************
 *                                *
 *    DECLARAÇÃO DE VARIÁVEIS     *
 *                                *
 * *******************************/

// Input variables
uint8_t S1, prevS1;
uint8_t S2, prevS2;

// Output variables
uint8_t LED_1, LED_2, LED_3, LED_4, LED_5, LED_6, LED_7;

// Our finite state machines
fsm_t fsm1, fsm2, fsm3, fsm4, fsm5, fsm6;

unsigned long interval, last_cycle, last_cycle1, interval_print;
unsigned long loop_micros;
unsigned long blink_rate, flash_time, flash_time2;

// Controlador da ampulheta
int k = 0;
int k2 = 0;

// Gama de tempos
int i = 1; // começa em 1 porque o tempo começa em 2000
int gama_intervalos[4] = {1000, 2000, 4000, 8000};
bool modo; // Só pode tomar valores de 1 ou 0
// Blink mode
int j;

// Fade control
int brilho;
int fade_amount;

// Set new state - função
void set_state(fsm_t &fsm, int new_state)
{
	if (fsm.state != new_state)
	{ // if the state chnanged tis is reset
		fsm.state = new_state;
		fsm.tes = millis();
		fsm.tis = 0;
	}
}

/***************************************
 *                                     *
 *           INICIALIZAÇÃO             *
 *                                     *
 * ************************************/

void setup()
{
	// put your setup code here, to run once:
	// inicializar os pinos como I/O
	pinMode(LED1, OUTPUT);
	pinMode(BUTTON1, INPUT);
	pinMode(LED2, OUTPUT);
	pinMode(BUTTON2, INPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	pinMode(LED5, OUTPUT);
	pinMode(LED6, OUTPUT);
	pinMode(LED7, OUTPUT);

	// variável para contar o tempo que passou por LED
	flash_time = 0;
	flash_time2 = 0;
	// Start the serial port with 115200 baudrate
	Serial.begin(115200);

	interval = 10; // só vamos fazer leituras de 10 em 10 ms para evitar os efeitos de bounce e assim dos botões
	blink_rate = gama_intervalos[i];
	modo = 0;
	//interval_print = 500;
	set_state(fsm1, 0);
	set_state(fsm2, 0);
	set_state(fsm3, 0);
	set_state(fsm4, 0);
    set_state(fsm5, 0);
	set_state(fsm6, 0);

	k = 6; // CONTAR CRONÓMETROS
	k2 = 6; // FINAL ALTERNATIVO
	j = 0; // FADE
	brilho = 255; // INTENSIDADE DE BRILHO
	fade_amount = (256/(blink_rate/10)) + 1; // VARIABILIDADE DO BRILHO
	//fade_amount=(blink_rate/10);

}

/*********************************
 *                               *
 *             CICLO             *
 *                               *
 * ******************************/

void loop()
{
	// To measure the time between loop() calls
	//unsigned long last_loop_micros = loop_micros;

	// Do this only every "interval" miliseconds
	// It helps to clear the switches bounce effect
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

		// FSM processing

		// Update tis for all state machines
		unsigned long cur_time = millis(); // Just one call to millis()
		fsm1.tis = cur_time - fsm1.tes;
		fsm2.tis = cur_time - fsm2.tes;
		fsm3.tis = cur_time - fsm3.tes;
		fsm4.tis = cur_time - fsm4.tes;
        fsm5.tis = cur_time - fsm5.tes;
		fsm6.tis = cur_time - fsm6.tes;

		// Ex2 -> "ganhar tempo" - FSM1

		/***************************************
       *                                      *
       *            GANHAR TEMPO              *
       *                                      *
       * *************************************/
	  	// Passei para dentro desta condição para ele não evoluir quando se encontra em config
	  	if(fsm4.state == 0){
			if (fsm1.state == 0 && S2 && !prevS2 && k < 6)
			{
				fsm1.new_state = 1;
			}
			else if (fsm1.state == 1 && S2 && !prevS2)
			{
				fsm1.new_state = 2; // volta ao inicio
				k++; // aumenta k
				Serial.print("Ganhou 2s"); // 
				flash_time = 0; // reinicia tempos
				fsm2.tes = millis();
				fsm2.tis = 0;
				brilho = 255;  // tambem reiniciamos o brilho
			}
			else if (fsm1.state == 1 && fsm1.tis > 500)
			{
				fsm1.new_state = 0;
			}
			else if (fsm1.state == 2 && 1)
			{
				fsm1.new_state = 0;
			}
		}

		/***************************************
       *                                      *
       *       PROGRAMA DE CONTROLO           *
       *                                      *
       * *************************************/

		// Ex1 -> Controlo dos LEDS - FSM2
		// alínea a)
		// Passei para dentro desta condição para ele não evoluir quando se encontra em config
		if (fsm4.state == 0)
		{
			if (fsm2.state == 0 && S2 && !prevS2)
			{ // START
				fsm2.new_state = 1;
			}
			else if (fsm2.state == 1 && S2 && !prevS2)
			{ // vai para o estado de pausa
				fsm2.new_state = 2;
				flash_time += fsm2.tis; //salva quanto tempo tinha passado quando foi para a pausa
										// fsm2.state == 2 || fsm2.state == 3 na condição abaixo -> Não pode fazer reset se tiver em pausa
			}
			else if ((fsm2.state == 1) && S1 && !prevS1)
			{ // se quisermos fazer reset
				k = 6;
				fsm2.new_state = 0;
			}
			else if ((fsm2.state == 2 || fsm2.state == 3) && S2 && !prevS2)
			{ // voltar ao funcionamento
				fsm2.new_state = 1;
			}
			else if (fsm2.state == 2 && !S2 && fsm2.tis > 500)
			{ //piscar quando em pausa
				fsm2.new_state = 3;
			}
			else if (fsm2.state == 3 && fsm2.tis > 500 && !S2)
			{ // piscar quando em pausa
				fsm2.new_state = 2;
			}
			else if (fsm2.state == 1 && (flash_time + fsm2.tis) >= blink_rate)
			{ // cronómetro a funcionar
				k--; // menos 1 led
				flash_time = 0; // flash time a 0
				fsm2.tes = millis(); // vamos reiniciar também a contagem de tempo no ciclo
				fsm2.tis = 0;
				brilho = 255; // vamos também reiniciar o brilho para o caso de estar no modo j==2
			}
			else if (k == 0 && fsm2.state == 1)
			{ // acabou a contagem
				fsm2.new_state = 4;
				flash_time2 = 0; // modo de terminação alternativo
				Serial.print("Tempo terminou...\n");
			}
			else if (fsm2.state == 4 && S1 && !prevS1)
			{
				k = 6;
				fsm2.new_state = 0;
				brilho = 255;
			}
		}

		// else if(fsm2.state == 3 && S2) {
		// fsm2.new_state = 1; - passei para dentro da mesma condição. O código fica mais compacto mas acho que fica mais difícil de fazer debug, perguntar o que é melhor.

		// Ex1 alínea c) -> Entrada no menu de configuração -FSM3
		// Para entrar no modo de configuração vamos precisar de adicionar e não fsm3.state == 2 a todos os anteriores e nas saídas
		// TEREMOS DE UTILIZAR ALGO PARA DIFERENCIAR ENTRE RESET E MENU DE CONFIG?

		/****************************************
       *                                      *
       *    ENTRADA NO MENU DE CONFIG         *
       *                                      *
       * *************************************/

		if (fsm3.state == 0 && S1 && !prevS1)
		{ // START
			fsm3.new_state = 1;
		}
		else if (fsm3.state == 1 && fsm3.tis > 3000)
		{ // vai para o menu
			fsm3.new_state = 2;
			Serial.print("\n\n Entrei no modo de configuração...\n");
		}
		else if (fsm3.state == 1 && !S1 && prevS1)
		{ // não foi long click
			fsm3.new_state = 0;
		}
		else if (fsm3.state == 2 && S1 && !prevS1)
		{ // tenta sair do menu
			fsm3.new_state = 3;
		}
		else if (fsm3.state == 3 && fsm3.tis > 3000)
		{ // SAI do menu
			fsm3.new_state = 0;
			//fsm4.new_state = 0; // Desliga automaticamente o menu de configuração
		}
		else if (fsm3.state == 3 && !S1)
		{ // não foi long click para sair
			fsm3.new_state = 2;
		}

		// Menu de configuração - FSM4

		/****************************************
       *                                        *
       *          MENU DE CONFIGURAÇÃO          *
       *                                        *
       * ***************************************/

		if (fsm4.state == 0 && fsm3.state == 2)
		{ // entra no menu de configuração
			fsm4.new_state = 1;
			Serial.print("Configurar interval...\n");
		}
		else if (fsm4.state == 1 && S1 && !prevS1)
		{ // lógica de troca entre possíveis configurações
			fsm4.new_state = 2;
			Serial.print("Configurar blink mode...\n");
		}
		else if (fsm4.state == 2 && S1 && !prevS1)
		{ //
			fsm4.new_state = 3;
			Serial.print("Configurar end...\n");
		}
		else if (fsm4.state == 3 && S1 && !prevS1)
		{ //
			Serial.print("Configurar interval...\n");
			fsm4.new_state = 1;
		}
		else if ((fsm4.state == 1 || fsm4.state == 2 || fsm4.state == 3) && fsm3.state == 0)
		{
			fsm4.new_state = 0;
			brilho = 255; // quando sai do modo de config metemos o brilho a toda a intensidade
		}

		if (fsm4.state == 1 && S2 && !prevS2)
		{ // Lógica de configuração
			if (i < 3)
			{
				i++;
				fsm4.tes = millis(); // é preciso estas duas linhas para reiniciar
				fsm4.tis = 0; // reiniciamos o tis de cada vez que troca
				brilho = 255; // reiniciar brilho
			}
			else{
				i = 0;
				fsm4.tes = millis(); // é preciso estas duas linhas para reiniciar
				fsm4.tis = 0; // reiniciamos o tis de cada vez que troca
				brilho = 255; // reiniciar brilho
			} 

			blink_rate = gama_intervalos[i]; // atualiza o tempo da ampulheta
			if (j==2){
				fade_amount = (256/(blink_rate/10)) + 1; //tambem atualizamos o fade quando mudamos o blink rate
				brilho = 255;
			}
		}
		else if (fsm4.state == 2 && S2 && !prevS2)
		{
			if (j < 2)
			{
				fsm4.tes = millis(); // é preciso estas duas linhas para reiniciar
				fsm4.tis = 0; // reiniciamos o tis de cada vez que troca
				j++;
			}
			else
				j = 0;
			if(j==2){
				brilho = 255; // reiniciar brilho
				fade_amount = (256/(blink_rate/10)) + 1; // atualizamos o fade
			}
			//if(j == 2){  // ilustrar fade com o LED7
			//	brilho -= fade_amount; // reduz luminosidade
			//	if (brilho < 0)
			//	{
			//		brilho = 0; //não vai a valores negativos
			//	}
			//	analogWrite(LED7, brilho); // ilustrar
			//}
		}
		else if (fsm4.state == 3 && S2 && !prevS2)
		{
			modo = !modo;
		}

        /****************************************
         *                                      *
         *   LÓGICA SINGULAR DE PISCAR DO LED   *
         *                                      *
         * *************************************/

        if(fsm5.state == 0 && (fsm4.state == 1 || fsm4.state == 2 || fsm4.state == 3)){
            fsm5.new_state = 1;
        } else if((fsm5.state == 1 || fsm5.state == 2) && fsm4.state == 0){
            fsm5.new_state = 0;
        } else if(fsm5.state == 1 && fsm5.tis > 200){
            fsm5.new_state = 2;
        } else if(fsm5.state == 2 && fsm5.tis >200){
            fsm5.new_state = 1;
        }

        if(fsm6.state == 0 && (fsm2.state == 1 && j == 1 && fsm4.state == 0)){
            fsm6.new_state = 1;
        } else if((fsm6.state == 1 || fsm6.state == 2) && ((fsm2.tis + flash_time > blink_rate) || fsm4.state != 0)){
            fsm6.new_state = 0;
        } else if(fsm6.state == 1 && fsm6.tis > 200){
            fsm6.new_state = 2;
        } else if(fsm6.state == 2 && fsm6.tis >200){
            fsm6.new_state = 1;
        }

		/****************************************
       *                                      *
       *        UPDATE THE STATES             *
       *                                      *
       * *************************************/

		// Update the states
		set_state(fsm1, fsm1.new_state);
		set_state(fsm2, fsm2.new_state);
		set_state(fsm3, fsm3.new_state);
		set_state(fsm4, fsm4.new_state);
        set_state(fsm5, fsm5.new_state);
		set_state(fsm6, fsm6.new_state);

		// Actions set by the current state of the first state machine

		/*  if (fsm1.state == 0){
        LED_1 = 0;
      } else if (fsm1.state == 1){
        LED_1 = 1;
      } */

	    /**************************************
       *                                      *
       *       ATIVAÇÃO DAS SAÍDAS            *
       *                                      *
       * *************************************/

	   // -------->>>>>>> PODIA METER TODAS AS ATIVAÇÕES RELATIVAS AO LED_7 DENTRO DUM IF QUE DISSESSE RESPEITO AO MENU DE CONFIG

		// A more compact way
		//if(fsm4.state == 0){

        /*****************************************
         *                                       *
         *     SAÍDAS EM FUNCIONAMENTO BASE      *
         *                                       *
         * **************************************/

		// os leds estarão ligados se estiverem nos modos de contar ou de pausa e o k ainda estiver "válido"
		LED_1 = ((fsm2.state == 1 || fsm2.state == 2) && k >= 1 && (fsm4.state == 0));
		LED_2 = ((fsm2.state == 1 || fsm2.state == 2) && k >= 2 && (fsm4.state == 0));
		LED_3 = ((fsm2.state == 1 || fsm2.state == 2) && k >= 3 && (fsm4.state == 0));
		LED_4 = ((fsm2.state == 1 || fsm2.state == 2) && k >= 4 && (fsm4.state == 0));
		LED_5 = ((fsm2.state == 1 || fsm2.state == 2) && k >= 5 && (fsm4.state == 0));
		LED_6 = ((fsm2.state == 1 || fsm2.state == 2) && k >= 6 && (fsm4.state == 0));
		LED_7 = ((fsm2.state == 4) && (modo == 0)); // para ligar tanto em caso de finalizar como em caso de representação da config
		//}

        /*****************************************
         *                                       *
         *      MODO DE TERMINAÇÃO A PISCAR      *
         *                                       *
         * **************************************/

		if (fsm2.state == 4 && modo == 1) // vamos meter o modo a funcionar também quando está no menu de config
		{
			if (fsm2.tis - flash_time2 > 100 && k2 > 0)
			{
				flash_time2 = fsm2.tis; // aumentamos flash_time2 de 100 em 100 ms
				k2--;
			}
			else if (k2 == 0)
			{
				k2 = 6;
			}
			LED_1 = (k2 == 1);
			LED_2 = (k2 == 2);
			LED_3 = (k2 == 3);
			LED_4 = (k2 == 4);
			LED_5 = (k2 == 5);
			LED_6 = (k2 == 6);
		}

		// LED_1 = (fsm1.state == 1);
		// LED_1 = (state == 1)||(state ==2);  if LED1 must be set in states 1 and 2

		// Actions set by the current state of the second state machine
		// LED_2 = (fsm2.state == 0);

		// Set the outputs
		// Modo de contagem 1

		//TODO: Rever fade pq prof quer

        /*****************************************
         *                                       *
         *       MODO DE CONTAGEM 3 -> FADE      *
         *                                       *
         * **************************************/

		// se modo fade e estivermos a contar ou no menu de config (para ilustrar)
		if (j == 2 && (fsm2.state == 1))
		{
			// Serial.println(brilho);
			brilho -= fade_amount; // reduz luminosidade
			if (brilho < 0)
			{
				brilho = 0; //não vai a valores negativos
			}
		}

		// SÓ ENTRA AQUI SE ESTIVER NO MENU DE CONFIG
		if(fsm3.state==2){ // enquanto estiver no menu de conf estas configurações podem ser ativadas
			// Código para o LED7 representar o fade enquanto se está no menu de config
			if((j==2) && fsm4.state == 2){
								// Serial.println(brilho);
				brilho -= fade_amount; // reduz luminosidade
				if (brilho < 0)
				{
					brilho = 0; //não vai a valores negativos
				}
				analogWrite(LED7, brilho);
			}
			else if(fsm4.state == 1 && fsm4.tis < blink_rate){ // led_7 a ilustrar o tempo de config
				LED_7 = 1; // vamos impôr isto quando estivermos a ilustrar, depois segue o funcionamento normal que naturalmente será 0
				analogWrite(LED7, LED_7*255);
			}
			else if(fsm4.state == 3 && modo == 0){
				LED_7 = 1;
				analogWrite(LED7, LED_7*255);
			}			
			else if ((fsm4.state == 3 && modo == 1)) // vamos meter o modo a funcionar também quando está no menu de config
			{
				LED_7 = 0;
				analogWrite(LED7, LED_7*255);
				if (fsm4.tis - flash_time2 > 100 && k2 > 0)
				{
					flash_time2 = fsm4.tis; // aumentamos flash_time2 de 100 em 100 ms
					k2--;
				}
				else if (k2 == 0)
				{
					k2 = 6;
				}
				LED_1 = (k2 == 1);
				LED_2 = (k2 == 2);
				LED_3 = (k2 == 3);
				LED_4 = (k2 == 4);
				LED_5 = (k2 == 5);
				LED_6 = (k2 == 6);
			}
			else if(j==1 && fsm4.state == 2 && fsm4.tis < blink_rate){ // este ciclo basicamente altera a condição dos LED_i e sobrepoem se à anterior qd j == 1
				LED_7 = 1;
				if((fsm4.tis)>blink_rate/2){
					LED_7 = (fsm5.state == 1);
				}
				analogWrite(LED7, LED_7*255);
			}
			else{
				LED_7 = 0 ;
				analogWrite(LED7, LED_7*255);
			}
			} else if(fsm3.state == 3){
			brilho = 255; // repoe brilho quando sai do menu
			LED_7 = 0;
			analogWrite(LED7,LED_7*255);
		} 
		
		/*****************************************
         *                                       *
         *  PASSAGEM DOS VALORES PARA AS SAÍDAS  *
         *              MODOS 1, 2 E 3           *
         *                                       *
         * **************************************/

		// a menos que esteja a contar e no menu fade, ele faz digital writes normais
		if(j==1 && fsm2.state == 1){ // este ciclo basicamente altera a condição dos LED_i e sobrepoem se à anterior qd j == 1
			if((fsm2.tis + flash_time)>blink_rate/2){
				if(k==1) LED_1 = (fsm6.state == 1 && k == 1 && (fsm4.state == 0));
				if(k==2) LED_2 = (fsm6.state == 1 && k == 2 && (fsm4.state == 0));
				if(k==3) LED_3 = (fsm6.state == 1 && k == 3 && (fsm4.state == 0));
				if(k==4) LED_4 = (fsm6.state == 1 && k == 4 && (fsm4.state == 0));
				if(k==5) LED_5 = (fsm6.state == 1 && k == 5 && (fsm4.state == 0));
				if(k==6) LED_6 = (fsm6.state == 1 && k == 6 && (fsm4.state == 0));
			}
		}		

		// a menos que esteja a contar e no menu fade, ele faz digital writes normais
		if ((j == 2) && (k == 1) && (fsm2.state == 1))
		{
			analogWrite(LED1, brilho);
		}
		else
		{	
			//Serial.println("Estou aqui");
			analogWrite(LED1, LED_1*255);
		}

		if ((j == 2) && (k == 2) && (fsm2.state == 1))
		{
			analogWrite(LED2, brilho);
		}
		else
		{
			analogWrite(LED2, LED_2*255);
		}

		if ((j == 2) && (k == 3) && (fsm2.state == 1))
		{
			analogWrite(LED3, brilho);
		}
		else
		{
			analogWrite(LED3, LED_3*255);
		}

		if ((j == 2) && (k == 4) && (fsm2.state == 1))
		{
			analogWrite(LED4, brilho);
		}
		else
		{
			analogWrite(LED4, LED_4*255);
		}

		if ((j == 2) && (k == 5) && (fsm2.state == 1))
		{
			analogWrite(LED5, brilho);
		}
		else
		{
			//Serial.print("Estou aqui");
			//Serial.print(LED_5);
			analogWrite(LED5, LED_5*255);
		}

		if ((j == 2) && (k == 6) && (fsm2.state == 1))
		{
			// Serial.print("Fade: ");
			// Serial.println(brilho);
			analogWrite(LED6, brilho);
		}
		else
		{
			analogWrite(LED6, LED_6*255);
		}


		 // --->>> MODO EM FUNCIONAMENTO BASE
		if(fsm4.state == 0){
			analogWrite(LED7, LED_7*255);
		}
		
        /********************************************************
         *                                                      *
         *    SAÍDAS NO CASO DE ESTAR NO MENU DE CONFIGURAÇÃO   *
         *                                                      *
         * *****************************************************/

		// SÓ ENTRA AQUI SE ESTIVER NO MENU DE CONFIG
        if(fsm4.state != 0){
            LED_1 = ((fsm4.state == 1) && (fsm5.state == 1)); 
            LED_2 = ((fsm4.state == 2) && (fsm5.state == 1)); 
            LED_3 = ((fsm4.state == 3) && (fsm5.state == 1)); 
           	analogWrite(LED1, LED_1*255);
            analogWrite(LED2, LED_2*255);
            analogWrite(LED3, LED_3*255);
        }

		/**********************************************************
		 *                                                        *
		 *       LED 7 EM REPRESENTAÇÃO DAS OPÇÕES DE CONFIG      *
		 *                                                        *
		 * *******************************************************/

		// Debug using the serial port
		// if (now - last_cycle1 > interval_print) {
		//   last_cycle1 = now;

		/****************************************
       *                                      *
       *       PRINTS NO TERMINAL             *
       *                                      *
       * *************************************/

		Serial.print(" LED_7: ");
		Serial.print(LED_7);

		Serial.print(" fsm4.state: ");
		Serial.print(fsm4.state);

		Serial.print(" fsm3.state: ");
		Serial.print(fsm3.state);

		Serial.print(" blink_rate: ");
		Serial.print(blink_rate);

		Serial.print(" modo: ");
		Serial.print(modo);

		Serial.print(" fsm2.state: ");
		Serial.print(fsm2.state);

		Serial.print(" LED_4: ");
		Serial.print(LED_4);

		Serial.print(" k: ");
		Serial.print(k);

		Serial.print(" j: ");
		Serial.print(j);

		Serial.print(" loop: ");
		Serial.println(micros() - loop_micros);
	}
}