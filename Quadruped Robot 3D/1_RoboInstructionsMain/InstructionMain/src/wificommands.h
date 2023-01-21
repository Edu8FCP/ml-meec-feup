#ifndef WIFI_H
#define WIFI_H

/******************
*  LIBS REQUIRED  *
*******************/

#include <Arduino.h>
#include <Wifi.h>

#define COMMANDS_BUF_IN_SIZE 64

typedef enum {  // Estrutura que funciona entre espera/a ler
  cs_wait_for_command,
  cs_reading_value
} commands_state_t;


class commands_wifi
{ 
  public:
    // Pointer to the function that processes commands
    void (*process_command)(char command, float value);

    char data[COMMANDS_BUF_IN_SIZE];
    int count;
    commands_state_t state;
    char command;
    float value;

    commands_wifi();
    void init(void (*process_command_function)(char command, float value));
    void process_char(char b);
};

#endif // PID_H