#include "embARC_assert.h"
#include <string.h>
#include <portmacro.h>
#include <FreeRTOS_CLI.h>
#include <projdefs.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "track_cli.h"

static bool track_cli_registered = false;

static enum OBJECT_OUTPUT new_format;

static BaseType_t track_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1 = NULL, *param2 = NULL;
        BaseType_t len1 = 0, len2 = 0;

        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

        if (param1 != NULL) {
                if (strncmp(param1, "new_format", len1) == 0) {
                        if (param2 != NULL)
                                new_format = (int8_t) strtol(param2, NULL, 0);
                        if (new_format == CAN) {
                                #if (USE_CAN_FD == 1)
                                        snprintf(pcWriteBuffer, xWriteBufferLen, "CAN FD\n\r");
                                #else
                                        snprintf(pcWriteBuffer, xWriteBufferLen, "CAN\n\r");
                                #endif
                        } else {
                                snprintf(pcWriteBuffer, xWriteBufferLen, "new_format = %d\n\r", new_format);
                        }
                        return pdFALSE;
                }
        } else {
                static uint32_t count = 0;
                switch(count) {
                case 0 :
                        snprintf(pcWriteBuffer, xWriteBufferLen, "new_format = %d\r\n", new_format);
                        count++;
                        return pdTRUE;
                default :
                        count = 0;
                        return pdFALSE;
                }
        }

        return pdFALSE;
}

/* track command */
static const CLI_Command_Definition_t track_cmd = {
        "track",
        "track \n\r"
        "\tWrite or read track cfg information. \n\r"
        "\tUsage: track [[param] value] \n\r",
        track_cmd_handler,
        -1
};

void track_cmd_register(void)
{
        if (track_cli_registered)
                return;

        FreeRTOS_CLIRegisterCommand(&track_cmd);
        track_cli_registered = true;

        /* Setting the default output mode of tracking data*/
        new_format = 0;
        return;
}

int8_t get_track_cfg(void)
{
        return new_format;
}
