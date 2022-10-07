#include <Arduino.h>
#include <strings.h>

#define LED7 12
#define BUTTON1 2
#define BUTTON2 3
#define ARRAY_SIZE 6

typedef struct
{
	int state, new_state;

	// tes - time entering state
	// tis - time in state
	unsigned long tes, tis;
} fsm_t;

//Leds GPIOs
uint8_t LED_ARRAY[6] = {6, 7, 8, 9, 10, 11};

// Input variables
uint8_t S1, prevS1;
uint8_t S2, prevS2;

// Output variables
uint8_t led_array_value[6] = {0};
uint8_t LED_7 = 0;

// Our finite state machines
fsm_t fsm1, fsm2;

unsigned long interval, last_cycle;
unsigned long loop_micros;
unsigned long blink_time;
unsigned long saved_time;

int K = 0;

// Set new state
void set_state(fsm_t &fsm, int new_state)
{
	if (fsm.state != new_state)
	{ // if the state chnanged tis is reset
		fsm.state = new_state;
		fsm.tes = millis();
		fsm.tis = 0;
	}
}

void setup()
{
	// put your setup code here, to run once:
	for (int i = 0; i < 6; i++)
	{
		pinMode(LED_ARRAY[i], OUTPUT);
	}
	pinMode(BUTTON1, INPUT);
	pinMode(LED7, OUTPUT);
	pinMode(BUTTON2, INPUT);

	// Start the serial port with 115200 baudrate
	Serial.begin(115200);

	blink_time = 2000;
	interval = 10;
	set_state(fsm1, 0);
	set_state(fsm2, 0);
}

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

		// Calculate next state for the first state machine - 4.

		if (fsm1.state == 0 && S1)
		{
			fsm1.new_state = 1;
		}
		else if (fsm1.state == 1 && fsm1.tis > blink_time)
		{
			fsm1.new_state = 2;
		}
		else if (fsm1.state == 2 && S2)
		{
			fsm1.new_state = 4;
			saved_time = fsm1.tis;
		}
		else if (fsm1.state == 4 && (S2 != prevS2))
		{
			fsm1.new_state = 2;
		}
		else if (fsm1.state == 2 && (int)(fsm1.tis / blink_time) > 4)
		{
			fsm1.new_state = 3;
		}
		else if (fsm1.state == 3 && S1)
		{
			fsm1.state = 0;
		}

		// Update the states
		set_state(fsm1, fsm1.new_state);

		// Actions set by the current state of the first state machine
		if (fsm1.state == 0)
		{
			bzero(led_array_value, 6);
			LED_7 = 0;
		}
		else if (fsm1.state == 1)
		{
			for (int i = 0; i < ARRAY_SIZE; i++)
			{
				led_array_value[i] = 1;
			}
		}
		else if (fsm1.state == 2)
		{
			fsm1.tis += saved_time;
			K = (int)(fsm1.tis / blink_time);
			led_array_value[K] = 0;
			saved_time = 0;
		}
		else if (fsm2.state == 4)
		{
		}
		else if (fsm2.state == 3)
		{
			LED_7 = 1;
		}

		// Actions set by the current state of the second state machine
		// LED_2 = (fsm2.state == 0);

		// Set the outputs
		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			digitalWrite(LED_ARRAY[i], led_array_value[i]);
		}
		digitalWrite(LED7, LED_7);

		// Debug using the serial port
		Serial.print("S1: ");
		Serial.print(S1);

		Serial.print(" S2: ");
		Serial.print(S2);

		Serial.print(" fsm1.state: ");
		Serial.print(fsm1.state);

		Serial.print(" loop: ");
		Serial.println(micros() - loop_micros);
	}
}