#include <Arduino.h>
#include "wificommands.h"

commands_wifi::commands_wifi()
{
  count = 0;
  state = cs_wait_for_command;
  command = ' ';
  value = 0;
  process_command = NULL;

}


void commands_wifi::init(void (*process_command_function)(char command, float value))
{
  process_command = process_command_function;
}


void commands_wifi::process_char(char b)
{
  if (state == cs_wait_for_command && isalpha(b))  { // A command is allways a letter
    state = cs_reading_value;
    command = b;

  } else if (state == cs_reading_value && b == 0x0A)  { // LF (enter key received)
    // Now we can process the buffer
    if (count != 0) {
      data[count] = 0; // Null terminate the string
      value = atof((const char*)data);
    } 

    if (process_command)                  // If "process_command" is not null
      (*process_command)(command, value); // Do something with the pair (command, value)

    command = ' '; // Clear current command
    value = 0;     // Default value for "value"
    count = 0;     // Clear Buffer 
    state = cs_wait_for_command;

  } else if (state == cs_reading_value && count < COMMANDS_BUF_IN_SIZE - 1)  { // A new digit can be read
    data[count] = b;  // Store byte in the buffer
    count++;

  }
}

