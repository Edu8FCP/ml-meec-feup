#include <Arduino.h>

#define LED1_1 6
#define LED2 7
#define BUTTON1 2
#define BUTTON2 3

typedef struct {
  int state, new_state;

  // tes - time entering state
  // tis - time in state
  unsigned long tes, tis;
} fsm_t;

// Input variables
uint8_t S1, prevS1;
uint8_t S2, prevS2;

// Output variables
uint8_t LED_1, LED_2;

// Our finite state machines
fsm_t fsm1, fsm2;

unsigned long interval, last_cycle;
unsigned long loop_micros;
unsigned long randNumber;
unsigned long reaction;


// Set new state
void set_state(fsm_t& fsm, int new_state)
{
  if (fsm.state != new_state) {  // if the state chnanged tis is reset
    fsm.state = new_state;
    fsm.tes = millis();
    fsm.tis = 0;
  }
   randomSeed(analogRead(0));
}

void setup() {
  // put your setup code here, to run once:
  
  pinMode(LED1_1, OUTPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON2, INPUT);
  LED_2 = 1;

  // Start the serial port with 115200 baudrate
  Serial.begin(115200);

  interval = 10;
  set_state(fsm1, 0);
  set_state(fsm2, 0);   
}

void loop() {

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
      prevS2 = S2;
      S1 = !digitalRead(BUTTON1);
      S2 = !digitalRead(BUTTON2);

      // FSM processing

      // Update tis for all state machines
      unsigned long cur_time = millis();   // Just one call to millis()
      fsm1.tis = cur_time - fsm1.tes;
      fsm2.tis = cur_time - fsm2.tes; 

      if (fsm1.state == 0 && fsm1.tis > 3000){
        fsm1.new_state = 1;
      } else if(fsm1.state == 1 && fsm1.tis > randNumber) {
        fsm1.new_state = 2;
      } else if (fsm1.state == 2 && S1){
        fsm1.new_state = 0;
        reaction = fsm1.tis;
        Serial.print("Reaction: ");
        Serial.print(reaction);
      } 


      // Update the states
      set_state(fsm1, fsm1.new_state);
      set_state(fsm2, fsm2.new_state);

      // Actions set by the current state of the first state machine
      if (fsm1.state == 0){
        LED_1 = 0;
        randNumber = random(100, 3000);
      } else if (fsm1.state == 1){
        LED_1 = 0;
      } else if (fsm1.state == 2){
        LED_1 = 1;
      }

      // A more compact way
      // LED_1 = (fsm1.state == 1);
      // LED_1 = (state == 1)||(state ==2);  if LED1_1 must be set in states 1 and 2
      
      // Actions set by the current state of the second state machine
      // LED_2 = (fsm2.state == 0);

      // Set the outputs
      digitalWrite(LED1_1, LED_1);
      digitalWrite(LED2, LED_2);

      // Debug using the serial port
    }
    
}