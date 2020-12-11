#ifndef CALTERAH_TRACK_CLI_H
#define CALTERAH_TRACK_CLI_H

enum OBJECT_OUTPUT
{
        UART_STRING = 0,        /* output the tacking data in original string form by uart interface. */
        UART_HEX,            /* output the tacking data in hexadecimal form by uart interface. */
        CAN,                 /* output the tacking data in hexadecimal form by CAN interface. */
};
/*--- DECLARAION ---------------------*/
void track_cmd_register(void);
int8_t get_track_cfg(void);
#endif
