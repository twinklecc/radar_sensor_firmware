#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "FreeRTOS.h"


void command_interpreter_task(void *parameters);
void freertos_cli_init();

#endif
