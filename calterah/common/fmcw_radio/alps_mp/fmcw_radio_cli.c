/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

/* sensor includes. */
#include "sensor_config.h"
#include "baseband.h"
#include "radio_ctrl.h"

#define READ_BACK_LEN 64

static char readBack[READ_BACK_LEN];
static int readBackLen;

static bool fmcw_radio_cli_registered = false;

/* radio register command */
static BaseType_t radioRegCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio register dump command*/
static BaseType_t radioRegDumpCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio power on command*/
static BaseType_t radioPowerOnCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio power off command*/
static BaseType_t radioPowerOffCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio temperature command */
static BaseType_t radioTempCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio saturation detection command */
static BaseType_t radioSaturationCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio rx gain command */
static BaseType_t radioRxGainCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio radio tx power command */
static BaseType_t radioTxPowerCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio tx phase command */
static BaseType_t radioTxPhaseCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio fmcw command */
static BaseType_t radioFmcwCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio pll command */
static BaseType_t radioPllCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio vctrl output */
static BaseType_t radioVctrlCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio IF signal output */
static BaseType_t radioIfoutCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio TX */
static BaseType_t radioTxCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio single tone mode */
static BaseType_t radioSingleToneCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio Highpass Filer */
static BaseType_t radioHpfCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio HP Auto */
static BaseType_t radioHpaCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio TX Auto */
static BaseType_t radioAutoTXCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio AUXADC1 Voltage */
static BaseType_t radioAUXADC1VoltageCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio AUXADC2 Voltage */
static BaseType_t radioAUXADC2VoltageCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* radio rf compensation code */
static BaseType_t radioRFCompCodeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* helpers */
static void print_help(char* buffer, size_t buff_len, const CLI_Command_Definition_t* cmd)
{
        uint32_t tot_count = 0;
        int32_t count = sprintf(buffer, "Wrong input\n\r %s", cmd->pcHelpString);
        EMBARC_ASSERT(count > 0);
        tot_count += count;
        EMBARC_ASSERT(tot_count < buff_len);
}

/* radio register command ****************************************************/

/*
 * command   : radio_reg
 * parameter : <address(hex)> - read value of radio register address
 *             <address(hex)> <data(hex)> - write value to radio register address
 *
 * example   : radio_reg 12 - read address 0x12 value of current bank
 *             radio_reg 10 55 - write 0x55 to address 0x10 of current bank
 *
 * note      : this command doesn't have parameter of bank, if you need to change
 *             bank, please use 'radio_reg 00 xx' to change to xx bank.
 *
 */

static const CLI_Command_Definition_t xRadioReg = {
        "radio_reg",      /* The command string to type. */
        "\r\nradio_reg \r\n"
        "\t <address(hex)> - read value of radio register address\r\n"
        "\t <address(hex)> <data(hex)> - write value to radio register address\r\n",
        radioRegCommand, /* The function to run.        */
        -1               /* Up to 2 commands.           */
};

static BaseType_t radioRegCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2;
        BaseType_t xParameter1StringLength, xParameter2StringLength;
        unsigned int regAddr, regValue;
        volatile char reg_read_value;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL){
        /* two parameters: write register */
                regAddr  = (unsigned int) strtol(pcParameter1, NULL, 0);
                regValue = (unsigned int) strtol(pcParameter2, NULL, 0);
                fmcw_radio_reg_write(NULL, regAddr, regValue);

                readBackLen = sprintf(readBack, "\r\nset radio register: address - %3d (0x%02X)",
                                      regAddr, regAddr);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, "  value - %3d (0x%02X)\r\n",
                                      regValue, regValue);
                strncat(pcWriteBuffer, readBack, readBackLen);
        } else if(pcParameter1 != NULL && pcParameter2 == NULL) {
        /* one parameters: read register */
                regAddr  = (unsigned int) strtol(pcParameter1, NULL, 0);
                reg_read_value = fmcw_radio_reg_read(NULL, regAddr);

                readBackLen = sprintf(readBack, "\r\nread radio register: address - %3d (0x%02X)",
                                      regAddr, regAddr);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, "  value - %3d (0x%02X)\r\n",
                                      reg_read_value, reg_read_value);
                strncat(pcWriteBuffer, readBack, readBackLen);
        } else {
        /* no parameter: error */
                readBackLen = sprintf(readBack, "\r\none or two parameter is needed! \r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_reg done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* dump all registers of radio ***********************************************/

/*
 * command   : radio_regdump
 * parameter : <NA> - dump all registers of radio.
 *
 */

static const CLI_Command_Definition_t xRadioRegDump = {
        "radio_regdump"     , /* The command string to type. */
        "\r\nradio_regdump:\r\n"
        "\t <NA> - dump all registers of radio\r\n",
        radioRegDumpCommand, /* The function to run.        */
        0                    /* No parameters are expected. */
};

static BaseType_t radioRegDumpCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        fmcw_radio_reg_dump(NULL);

        /* display info */
        sprintf(pcWriteBuffer, "\r\n radio register dumping done \r\n");

        /* return */
        return pdFALSE;
}


/* radio power on ************************************************************/

/*
 * command   : radio_poweron
 * parameter : <idx> - all radio module power on
 *
 */

static const CLI_Command_Definition_t xRadioPowerOn = {
        "radio_poweron"     , /* The command string to type. */
        "\r\nradio_poweron\r\n"
        "\t <idx> - all radio module power on\r\n",
        radioPowerOnCommand, /* The function to run.        */
        -1                   /* Up to 1 commands.           */
};

static BaseType_t radioPowerOnCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        BaseType_t len1 = 0;
        uint32_t indx = 0;

        const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        indx = (uint32_t)strtol(param1, NULL, 0);

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        baseband_t* bb_ptr = baseband_get_bb(indx);

        /* FIXME: one need to pass true pointer instead of NULL */
        /* Fixed */
        fmcw_radio_power_on(&bb_ptr->radio);

        /* display info */
        sprintf(pcWriteBuffer, "\r\n radio power on! \r\n");

        /* return */
        return pdFALSE;
}

/* radio power off ************************************************************/

/*
 * command   : radio_poweroff
 * parameter : <idx> - radio module PMU current off
 *
 */

static const CLI_Command_Definition_t xRadioPowerOff = {
        "radio_poweroff"     , /* The command string to type. */
        "\r\nradio_poweroff\r\n"
        "\t <idx> - radio module PMU current off\r\n",
        radioPowerOffCommand, /* The function to run.        */
        -1                   /* Up to 1 commands.           */
};

static BaseType_t radioPowerOffCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        BaseType_t len1 = 0;
        uint32_t indx = 0;

        const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        indx = (uint32_t)strtol(param1, NULL, 0);

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        baseband_t* bb_ptr = baseband_get_bb(indx);

        fmcw_radio_power_off(&bb_ptr->radio);

        /* display info */
        sprintf(pcWriteBuffer, "\r\n radio power off! \r\n");

        /* return */
        return pdFALSE;
}

/* radio temperature *********************************************************/

/*
 * command   : radio_temp
 * parameter : <NA> - get temperature of radio
 *
 */

static const CLI_Command_Definition_t xRadioTemp = {
        "radio_temp"     , /* The command string to type. */
        "\r\nradio_temp\r\n"
        "\t <NA> - get temperature of radio\r\n",
        radioTempCommand, /* The function to run.        */
        0                 /* No parameters are expected. */
};

static BaseType_t radioTempCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        float temperature = 0;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        temperature = fmcw_radio_get_temperature(NULL);

        /* display info */
        sprintf(pcWriteBuffer, "\r\n radio temperature: %.2f \r\n", temperature);

        /* return */
        return pdFALSE;
}


/* radio saturation detection ***********************************************/

/*
 * command   : radio_saturation
 * parameter : <NA> - get saturation detector status
 *
 */

static const CLI_Command_Definition_t xRadioSaturation = {
        "radio_saturation"            , /* The command string to type. */
        "\r\nradio_saturation\r\n"
        "\t <NA> - get saturation detector status\r\n",
        radioSaturationCommand, /* The function to run.        */
        0                       /* No parameters are expected. */
};

static BaseType_t radioSaturationCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){


        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* FIXME: add saturation function */

        /* display info */
        sprintf(pcWriteBuffer, "\r\n radio saturation status \r\n");

        /* return */
        return pdFALSE;
}


/* radio rx gain *************************************************************/

/*
 * command   : radio_rxgain
 * parameter : <NA> - get tia, vga1 and vga2 gain of all channel
 *             <tia/vga1/vga2> <gain_index> - set gain_index to tia, vga1 or gva2 to all channel
 *             <tia/vga1/vga2> <ch_index> <gain_index> - set gain_index to tia, vga1 or gva2 to channel index
 *
 * note      : tia gain index  - 1(133ohms) / 2(250ohms) / 3(500ohms) / 4(1kohms) / 5(2kohms)
 *             vga1 gain index - 1(6.0dB) / 2(10.5dB) / 3(15.0dB) / 4(19.5dB) / 5(24.0dB) / 6(28.5dB)
 *             vga2 gain index - 1(9.0dB) / 2(13.5dB) / 3(18.0dB) / 4(21.5dB) / 5(27.0dB) / 6(31.5dB)
 *             channel index   - 0 / 1 / 2 / 3
 *
 */

static const CLI_Command_Definition_t xRadioRxGain = {
        "radio_rxgain",      /* The command string to type. */
        "\r\nradio_rxgain \r\n"
        "\t <NA> - get tia, vga1 and vga2 gain of all channel\r\n"
        "\t <tia/vga1/vga2> <gain_index> - set gain_index to tia, vga1 or gva2 to all channel\r\n"
        "\t <tia/vga1/vga2> <ch_index> <gain_index> - set gain_index to tia, vga1 or gva2 to channel index\r\n"
        "\t note:\r\n"
        "\t tia gain index  - 1(133ohms) / 2(250ohms) / 3(500ohms) / 4(1kohms) / 5(2kohms)\r\n"
        "\t vga1 gain index - 1(6.0dB) / 2(10.5dB) / 3(15.0dB) / 4(19.5dB) / 5(24.0dB) / 6(28.5dB)\r\n"
        "\t vga2 gain index - 1(9.0dB) / 2(13.5dB) / 3(18.0dB) / 4(21.5dB) / 5(27.0dB) / 6(31.5dB)\r\n"
        "\t channel index   - 0 / 1 / 2 / 3\r\n",
        radioRxGainCommand, /* The function to run.        */
        -1                  /* Up to 3 commands.           */
};

static BaseType_t radioRxGainCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2, *pcParameter3;
        BaseType_t xParameter1StringLength, xParameter2StringLength, xParameter3StringLength;
        unsigned int channel, gain;
        char rx_param[4];

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);
        pcParameter3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &xParameter3StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL && pcParameter3 != NULL){
        /* three parameters: write rx gain for indicated channel */

                strncpy ( rx_param, pcParameter1, xParameter1StringLength);
                channel = (unsigned int)strtol(pcParameter2, NULL, 10);
                gain = (unsigned int)strtol(pcParameter3, NULL, 10);

                if(strncmp(rx_param, "tia", 3) == 0) {
                        switch (gain){
                        case 1:
                                fmcw_radio_set_tia_gain(NULL, channel, 0xf);
                                readBackLen = sprintf(readBack, "\r\n set channel %d tia 133ohms\r\n",
                                                      channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 2:
                                fmcw_radio_set_tia_gain(NULL, channel, 0x1);
                                readBackLen = sprintf(readBack, "\r\n set channel %d tia 250ohms\r\n",
                                                      channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 3:
                                fmcw_radio_set_tia_gain(NULL, channel, 0x2);
                                readBackLen = sprintf(readBack, "\r\n set channel %d tia 500ohms\r\n",
                                                      channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 4:
                                fmcw_radio_set_tia_gain(NULL, channel, 0x4);
                                readBackLen = sprintf(readBack, "\r\n set channel %d tia 1000ohms\r\n",
                                                      channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 5:
                                fmcw_radio_set_tia_gain(NULL, channel, 0x8);
                                readBackLen = sprintf(readBack, "\r\n set channel %d tia 2000ohms\r\n",
                                                      channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        default:
                                break;
                        }
                } else if(strncmp(rx_param, "vga1", 4) == 0) {
                        fmcw_radio_set_vga1_gain(NULL, channel, gain);
                        readBackLen = sprintf(readBack, "\r\n set channel %d vga1 %d\r\n",
                                              channel, gain);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(rx_param, "vga2", 4) == 0) {
                        fmcw_radio_set_vga2_gain(NULL, channel, gain);
                        readBackLen = sprintf(readBack, "\r\n set channel %d vga2 %d\r\n",
                                              channel, gain);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n 3 parameters error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else if(pcParameter1 != NULL && pcParameter2 != NULL && pcParameter3 == NULL){
        /* two parameters: write rx gain for all channel */

                strncpy ( rx_param, pcParameter1, xParameter1StringLength);
                gain = (unsigned int)strtol(pcParameter2, NULL, 10);

                if(strncmp(rx_param, "tia", 3) == 0) {
                        switch (gain){
                        case 1:
                                fmcw_radio_set_tia_gain(NULL, -1, 0xf);
                                readBackLen = sprintf(readBack, "\r\n set tia 133ohms to all channel\r\n");
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 2:
                                fmcw_radio_set_tia_gain(NULL, -1, 0x1);
                                readBackLen = sprintf(readBack, "\r\n set tia 250ohms to all channel\r\n");
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 3:
                                fmcw_radio_set_tia_gain(NULL, -1, 0x2);
                                readBackLen = sprintf(readBack, "\r\n set tia 500ohms to all channel\r\n");
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 4:
                                fmcw_radio_set_tia_gain(NULL, -1, 0x3);
                                readBackLen = sprintf(readBack, "\r\n set tia 1000ohms to all channel\r\n");
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 5:
                                fmcw_radio_set_tia_gain(NULL, -1, 0x4);
                                readBackLen = sprintf(readBack, "\r\n set tia 2000ohms to all channel\r\n");
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        default:
                                readBackLen = sprintf(readBack, "\r\n gain index error \r\n");
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        }
                } else if(strncmp(rx_param, "vga1", 4) == 0) {
                        fmcw_radio_set_vga1_gain(NULL, -1, gain);
                        readBackLen = sprintf(readBack, "\r\n set vga1 %d to all channel\r\n",
                                              gain);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(rx_param, "vga2", 4) == 0) {
                        fmcw_radio_set_vga2_gain(NULL, -1, gain);
                        readBackLen = sprintf(readBack, "\r\n set vga2 %d to all channel\r\n",
                                              gain);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n 2 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else if(pcParameter1 == NULL && pcParameter2 == NULL && pcParameter3 == NULL) {
        /* no parameter: read register */

                /* print tia */
                readBackLen = sprintf(readBack, "\r\n get rx gain tia  ");
                strncat(pcWriteBuffer, readBack, readBackLen);
                for (channel = 0; channel < MAX_NUM_RX; channel++){
                        gain = fmcw_radio_get_tia_gain(NULL, channel);
                        switch (gain){
                        case 0xf:
                                readBackLen = sprintf(readBack, "CH%d-133ohms  ", channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 0x1:
                                readBackLen = sprintf(readBack, "CH%d-250ohms  ", channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 0x2:
                                readBackLen = sprintf(readBack, "CH%d-500ohms  ", channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 0x3:
                                readBackLen = sprintf(readBack, "CH%d-1000ohms ", channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        default:
                                readBackLen = sprintf(readBack, "CH%d-%d ", channel, gain);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        }
                }
                readBackLen = sprintf(readBack, "\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
                /* print vga1 */
                readBackLen = sprintf(readBack, "\r\n get rx gain vga1 ");
                strncat(pcWriteBuffer, readBack, readBackLen);
                for (channel = 0; channel < MAX_NUM_RX; channel++){
                        gain = fmcw_radio_get_vga1_gain(NULL, channel);
                        readBackLen = sprintf(readBack, "CH%d-%-8d ", channel, gain);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
                readBackLen = sprintf(readBack, "\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
                /* print vga2 */
                readBackLen = sprintf(readBack, "\r\n get rx gain vga2 ");
                strncat(pcWriteBuffer, readBack, readBackLen);
                for (channel = 0; channel < MAX_NUM_RX; channel++){
                        gain = fmcw_radio_get_vga2_gain(NULL, channel);
                        readBackLen = sprintf(readBack, "CH%d-%-8d ", channel, gain);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
                readBackLen = sprintf(readBack, "\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
        } else {
        /* error */
                readBackLen = sprintf(readBack, "\r\n parameter error\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_rxgain done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* radio tx power ************************************************************/

/*
 * command   : radio_txpower
 * parameter : <NA> - get tx power of all channel
 *             <power_index> - set power index to all channel
 *             <ch_index> <power_index> - set power index to channel index
 *
 * note      : power index   - 1(default) / 2(max)
 *             channel index - 0 / 1 / 2 / 3
 *
 */

static const CLI_Command_Definition_t xRadioTxPower = {
        "radio_txpower",      /* The command string to type. */
        "\r\nradio_txpower \r\n"
        "\t <NA> - get tx power of all channel\r\n"
        "\t <power_index> - set power index to all channel\r\n"
        "\t <ch_index> <power_index> - set power index to channel index\r\n"
        "\t note:\r\n"
        "\t power index   - 1(default) / 2(max)\r\n"
        "\t channel index - 0 / 1 / 2 / 3\r\n",
        radioTxPowerCommand, /* The function to run.        */
        -1                   /* Up to 2 commands.           */
};

static BaseType_t radioTxPowerCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2;
        BaseType_t xParameter1StringLength, xParameter2StringLength;
        unsigned int channel, power;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL){
        /* two parameters: set tx power to indicated channel */

                channel = (unsigned int)strtol(pcParameter1, NULL, 10);
                power = (unsigned int)strtol(pcParameter2, NULL, 10);
                switch (power){
                case 1:
                        fmcw_radio_set_tx_power(NULL, channel, 0xaa);
                        readBackLen = sprintf(readBack, "\r\n set channel %d default power \r\n",
                                              channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        break;
                case 2:
                        fmcw_radio_set_tx_power(NULL, channel, 0xff);
                        readBackLen = sprintf(readBack, "\r\n set channel %d max power \r\n",
                                              channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        break;
                default:
                        readBackLen = sprintf(readBack, "\r\n power index error \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        break;
                }
        } else if(pcParameter1 != NULL && pcParameter2 == NULL) {
        /* one parameters: set tx power to all channel */

                power = (unsigned int)strtol(pcParameter1, NULL, 10);
                switch (power){
                case 1:
                        fmcw_radio_set_tx_power(NULL, -1, 0xaa);
                        readBackLen = sprintf(readBack, "\r\n set default power to all channel \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        break;
                case 2:
                        fmcw_radio_set_tx_power(NULL, -1, 0xff);
                        readBackLen = sprintf(readBack, "\r\n set max power to all channel \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        break;
                default:
                        readBackLen = sprintf(readBack, "\r\n power index error \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        break;
                }
        } else {
        /* no parameter: error */
                readBackLen = sprintf(readBack, "\r\n get tx power ");
                strncat(pcWriteBuffer, readBack, readBackLen);
                for (channel = 0; channel < MAX_NUM_TX; channel++){
                        power = fmcw_radio_get_tx_power(NULL, channel);
                        switch (power){
                        case 0xaa:
                                readBackLen = sprintf(readBack, "CH%d-default ", channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        case 0xff:
                                readBackLen = sprintf(readBack, "CH%d-max ", channel);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        default:
                                readBackLen = sprintf(readBack, "CH%d-%2x ", channel, power);
                                strncat(pcWriteBuffer, readBack, readBackLen);
                                break;
                        }
                }
                readBackLen = sprintf(readBack, "\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_txpower done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* radio tx phase ************************************************************/

/*
 * command   : radio_txphase
 * parameter : <NA> - get phase of all channel
 *             <phase_index> - set phase index to all channel
 *             <ch_index> <phase_index> - set phase index to channel index
 *
 * note      : phase index   - 0 / 45 / 90 / 135 / 180 / 225 / 270 / 315
 *             channel index - 0 / 1 / 2 / 3
 *
 */

static const CLI_Command_Definition_t xRadioTxPhase = {
        "radio_txphase",      /* The command string to type. */
        "\r\nradio_txphase \r\n"
        "\t <NA> - get phase of all channel\r\n"
        "\t <phase_index> - set phase index to all channel\r\n"
        "\t <ch_index> <phase_index> - set phase index to channel index\r\n"
        "\t note:\r\n"
        "\t phase index   - 0 / 22 / 45 / 67 / 90 / 112 / 135 / 157 / 180 / 202 / 225 / 247 / 270 / 292 / 315 / 337 \r\n",
        radioTxPhaseCommand, /* The function to run.        */
        -1               /* Up to  commands.           */
};

static BaseType_t radioTxPhaseCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2;
        BaseType_t xParameter1StringLength, xParameter2StringLength;
        unsigned int channel, phase, reg_val;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);

        if (pcParameter1 != NULL && pcParameter2 != NULL) {
        /* two parameters: set tx phase to indicated channel */

                channel = (unsigned int)strtol(pcParameter1, NULL, 10);
                phase = (unsigned int)strtol(pcParameter2, NULL, 10);
                reg_val = phase_val_2_reg_val(phase);
                if (reg_val) {
                        fmcw_radio_set_tx_phase(NULL, channel, reg_val);
                        readBackLen = sprintf(readBack, "\r\n set channel %d phase %d\r\n",channel, phase);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n phase index error \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
        } else if(pcParameter1 != NULL && pcParameter2 == NULL) {
        /* one parameters: set tx phase to all channel */

                phase = (unsigned int)strtol(pcParameter1, NULL, 10);
                reg_val = phase_val_2_reg_val(phase);
                if (reg_val) {
                        fmcw_radio_set_tx_phase(NULL, -1, reg_val);
                        readBackLen = sprintf(readBack, "\r\n set phase %d to all channel\r\n", phase);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n phase index error \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
        } else {
        /* no parameter: get tx phase */
                /* FIXME: fmcw_radio_get_tx_phase(fmcw_radio_t *radio,
                             int32_t channel_index, char phase_index) */
                readBackLen = sprintf(readBack, "\r\n get tx phase code ");
                strncat(pcWriteBuffer, readBack, readBackLen);
                for (channel = 0; channel < MAX_NUM_TX; channel++){
                        reg_val = fmcw_radio_get_tx_phase(NULL, channel);
                        phase = reg_val_2_phase_val(reg_val);
                        if (phase <= 360) {
                                readBackLen = sprintf(readBack, "CH%d:%ddeg(0x%04x)  ", channel, phase, reg_val);
                        } else {
                                readBackLen = sprintf(readBack, "CH%d:phase configure error! ", channel);
                        }
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
                readBackLen = sprintf(readBack, "\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_txphase done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* radio fmcw ****************************************************************/

/*
 * command   : radio_fmcw
 * parameter : <NA> - get all fmcw parameters
 *             <mode> <value> - set fmcw mode
 *             <startf> <value> - set fmcw start frequency (MHz)
 *             <bandwidthf> <value> - set fmcw bandwidth (MHz)
 *             <chirpup> <value> - set ramp up time (us)
 *             <chirpdown> <value> - set ramp down time (us)
 *             <chirpidle> <value> - set idle time (us)
 *             <start> - fmcw start
 *             <stop> - fmcw stop
 *
 * note      : mode value - predefine / fsk / hopping
 *
 */

static const CLI_Command_Definition_t xRadioFmcw = {
        "radio_fmcw",      /* The command string to type. */
        "\r\nradio_fmcw \r\n"
        "\t <NA> - get all fmcw parameters\r\n"
        "\t <mode> <value>\r\n"
        "\t <startf> <value(MHz)>\r\n"
        "\t <bandwidthf> <value(MHz)>\r\n"
        "\t <chirpup> <value(us)>\r\n"
        "\t <chirpdown> <value(us)>\r\n"
        "\t <chirpidle> <value(us)>\r\n"
        "\t <start> - fmcw start\r\n"
        "\t <stop> - fmcw stop\r\n",
        radioFmcwCommand, /* The function to run.        */
        -1               /* Up to  commands.           */
};

static BaseType_t radioFmcwCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2;
        BaseType_t xParameter1StringLength, xParameter2StringLength;
        unsigned int fmcw_value;
        char fmcw_param[10];
        char mode_param[10];
        int para_match = 0;
        sensor_config_t *cfg = sensor_config_get_config(0);
        baseband_t *bb = baseband_get_bb(0);
        fmcw_radio_t* radio = &bb->radio;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL){
        /* two parameters: set fmcw parameters */

                strncpy ( fmcw_param, pcParameter1, xParameter1StringLength);

                if(strncmp(fmcw_param, "mode", sizeof("mode") - 1) == 0) {
                        strncpy ( mode_param, pcParameter2, xParameter2StringLength);
                        para_match = 1;
                }

                if(strncmp(fmcw_param, "startf", sizeof("startf") - 1) == 0) {
                        fmcw_value = (unsigned int)strtol(pcParameter2, NULL, 10);
                        cfg->fmcw_startfreq = fmcw_value * 1e-3;
                        readBackLen = sprintf(readBack, "\r\n set fmcw startf %.2f GHz\r\n",
                                              fmcw_value * 1e-3);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        para_match = 1;
                }

                if(strncmp(fmcw_param, "bandwidthf", sizeof("bandwidthf") - 1) == 0) {
                        fmcw_value = (unsigned int)strtol(pcParameter2, NULL, 10);
                        cfg->fmcw_bandwidth = fmcw_value;
                        readBackLen = sprintf(readBack, "\r\n set fmcw bandwidthf %d\r\n",
                                              fmcw_value);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        para_match = 1;
                }

                if(strncmp(fmcw_param, "chirpup", sizeof("chirpup") - 1) == 0) {
                        fmcw_value = (unsigned int)strtol(pcParameter2, NULL, 10);
                        cfg->fmcw_chirp_rampup = fmcw_value;
                        readBackLen = sprintf(readBack, "\r\n set fmcw chirpup %d\r\n",
                                              fmcw_value);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        para_match = 1;
                }

                if(strncmp(fmcw_param, "chirpdown", sizeof("chirpdown") - 1) == 0) {
                        fmcw_value = (unsigned int)strtol(pcParameter2, NULL, 10);
                        cfg->fmcw_chirp_down = fmcw_value;
                        readBackLen = sprintf(readBack, "\r\n set fmcw chirpdown %d\r\n",
                                              fmcw_value);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        para_match = 1;
                }

                if(strncmp(fmcw_param, "chirp_period", sizeof("chirp_period") - 1) == 0) {
                        fmcw_value = (unsigned int)strtol(pcParameter2, NULL, 10);
                        cfg->fmcw_chirp_period = fmcw_value;
                        readBackLen = sprintf(readBack, "\r\n set fmcw chirpidle %d\r\n",
                                              fmcw_value);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        para_match = 1;
                }

                if (para_match == 0){
                        readBackLen = sprintf(readBack, "\r\n fmcw 2 parameters error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else if(pcParameter1 != NULL && pcParameter2 == NULL) {
        /* one parameters: fmcw run / stop */

                strncpy ( fmcw_param, pcParameter1, xParameter1StringLength);
                if(strncmp(fmcw_param, "start", sizeof("start") - 1) == 0) {
                        fmcw_radio_start_fmcw(&bb->radio);
                        readBackLen = sprintf(readBack, "\r\n set fmcw start\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(fmcw_param, "stop", sizeof("stop") - 1) == 0) {
                        fmcw_radio_stop_fmcw(&bb->radio);
                        readBackLen = sprintf(readBack, "\r\n set fmcw stop\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n fmcw 1 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else {
        /* no parameter: get fmcw parameters */
                readBackLen = sprintf(readBack, "\r\n get fmcw parameters \r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " fmcw mode         - predefine\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " start frequency   - %.2f GHz\r\n", cfg->fmcw_startfreq);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " bandwidth         - %.2f MHz\r\n", cfg->fmcw_bandwidth);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " chirp up time     - %.2f us\r\n", cfg->fmcw_chirp_rampup);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " chirp down time   - %.2f us\r\n", cfg->fmcw_chirp_down);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " chirp period time - %.2f us\r\n", cfg->fmcw_chirp_period);
                strncat(pcWriteBuffer, readBack, readBackLen);

                readBackLen = sprintf(readBack, "\r\n get fmcw registers\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " start             - 0x%x\r\n", radio->start_freq);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " stop              - 0x%x\r\n", radio->stop_freq);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " step_up           - 0x%x\r\n", radio->step_up);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " step_down         - 0x%x\r\n", radio->step_down);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " cnt_wait          - 0x%x\r\n", radio->cnt_wait);
                strncat(pcWriteBuffer, readBack, readBackLen);

                readBackLen = sprintf(readBack, "\r\n get fmcw cycles\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " up_cycle          - %d\r\n", radio->up_cycle);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " down_cycle        - %d\r\n", radio->down_cycle);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " wait_cycle        - %d\r\n", radio->wait_cycle);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " total_cycle       - %d\r\n", radio->total_cycle);
                strncat(pcWriteBuffer, readBack, readBackLen);
                readBackLen = sprintf(readBack, " samp_skip_cycle   - %d\r\n", (1 + baseband_read_reg(&bb->bb_hw, BB_REG_SYS_SIZE_RNG_SKP)));
                strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_fmcw done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* radio pll *****************************************************************/

/*
 * command   : radio_pll
 * parameter : <NA> - get reference pll and fmcw pll lock status
 *             <fmcwlock> - fmcw pll lock
 *
 */

static const CLI_Command_Definition_t xRadioPll = {
        "radio_pll",      /* The command string to type. */
        "\r\nradio_pll \r\n"
        "\t <NA> - get reference pll and fmcw pll lock status\r\n"
        "\t <fmcwlock> - fmcw pll lock\r\n",
        radioPllCommand, /* The function to run.        */
        -1               /* Up to 1 commands.           */
};

static BaseType_t radioPllCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1;
        BaseType_t xParameter1StringLength;
        char pll_param[10];

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);

        if(pcParameter1 != NULL){
        /* one parameters: pll lock */

                strncpy ( pll_param, pcParameter1, xParameter1StringLength);

                if(strncmp(pll_param, "fmcwlock", sizeof("fmcwlock") - 1) == 0) {
                        if (fmcw_radio_is_pll_locked(NULL)){
                                readBackLen = sprintf(readBack, "\r\n fmcw pll is locked \r\n");
                                strncat(pcWriteBuffer, readBack, readBackLen);
                        } else {
                                readBackLen = sprintf(readBack, "\r\n fmcw pll is unlocked \r\n");
                                strncat(pcWriteBuffer, readBack, readBackLen);
                        }
                } else {
                        readBackLen = sprintf(readBack, "\r\n pll parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
        } else {
        /* no parameter: check lock status */
                if (fmcw_radio_is_refpll_locked(NULL)) {
                        readBackLen = sprintf(readBack, "\r\n reference pll is locked \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n reference pll is unlocked \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
                if (fmcw_radio_is_pll_locked(NULL)) {
                        readBackLen = sprintf(readBack, " fmcw pll is locked \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, " fmcw pll is unlocked \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_pll done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* radio vctrl output ********************************************************/

/*
 * command   : radio_vctrl
 * parameter : <on/off> enable / disable vctrl output signal
 *
 */

static const CLI_Command_Definition_t xRadioVctrl = {
        "radio_vctrl",      /* The command string to type. */
        "\r\nradio_vctrl \r\n"
        "\t <on/off> enable / disable vctrl output signal\r\n",
        radioVctrlCommand, /* The function to run.        */
        1                  /* 1 commands.                 */
};

static BaseType_t radioVctrlCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1;
        BaseType_t xParameter1StringLength;
        char vctrl_param[3];

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);

        if(pcParameter1 != NULL){
        /* one parameters: vctrl on / off */

                strncpy ( vctrl_param, pcParameter1, xParameter1StringLength);

                if(strncmp(vctrl_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_vctrl_on(NULL);
                        readBackLen = sprintf(readBack, "\r\n vctrl enable\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(vctrl_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_vctrl_off(NULL);
                        readBackLen = sprintf(readBack, "\r\n vctrl enable\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n vctrl parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }
        } else {
                readBackLen = sprintf(readBack, "\r\n vctrl parameter error\r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_vctrl done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* radio IF signal output ****************************************************/

/*
 * command   : radio_ifout
 * parameter : <on/off> - enable or disable to all channel
 *             <on/off> <ch_index> - enable or disable index to channel index
 *
 * note      : channel index - 0 / 1 / 2 / 3
 *
 */

static const CLI_Command_Definition_t xRadioIfout = {
        "radio_ifout",      /* The command string to type. */
        "\r\nradio_ifout \r\n"
        "\t <on/off> - enable or disable to all channel\r\n"
        "\t <on/off> <ch_index> - enable or disable index to channel index\r\n",
        radioIfoutCommand, /* The function to run.        */
        -1                 /* Up to 2 commands.           */
};

static BaseType_t radioIfoutCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2;
        BaseType_t xParameter1StringLength, xParameter2StringLength;
        char ifout_param[10];
        unsigned int channel;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL){
        /* two parameters: IF output for single channel */

                strncpy ( ifout_param, pcParameter1, xParameter1StringLength);
                channel  = (unsigned int) strtol(pcParameter2, NULL, 10);
                if(strncmp(ifout_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_if_output_on(NULL, channel);
                        readBackLen = sprintf(readBack, "\r\n CH-%d IF output enbale\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(ifout_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_if_output_off(NULL, channel);
                        readBackLen = sprintf(readBack, "\r\n CH-%d IF output disable\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_ifout 2 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else if(pcParameter1 != NULL && pcParameter2 == NULL) {
        /* one parameters: IF output for all channel */

                strncpy ( ifout_param, pcParameter1, xParameter1StringLength);
                if(strncmp(ifout_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_if_output_on(NULL, -1);
                        readBackLen = sprintf(readBack, "\r\n IF output enbale for all channel\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(ifout_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_if_output_off(NULL, -1);
                        readBackLen = sprintf(readBack, "\r\n IF output disable for all channel\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_ifout 1 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else {
                readBackLen = sprintf(readBack, "\r\n ifout parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_ifout done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* radio tx switch ***********************************************************/

/*
 * command   : radio_tx
 * parameter : <on/off> - enable or disable to all tx channel
 *             <on/off> <ch_index> - enable or disable index to tx channel index
 *
 * note      : channel index - 0 / 1 / 2 / 3
 *
 */

static const CLI_Command_Definition_t xRadioTx = {
        "radio_tx",      /* The command string to type. */
        "\r\nradio_tx \r\n"
        "\t <on/off> - enable or disable to all tx channel\r\n"
        "\t <on/off> <ch_index> - enable or disable index to tx channel index\r\n",
        radioTxCommand, /* The function to run.        */
        -1                 /* Up to 2 commands.           */
};

static BaseType_t radioTxCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2;
        BaseType_t xParameter1StringLength, xParameter2StringLength;
        char tx_param[10];
        unsigned int channel;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL){
        /* two parameters: IF output for single channel */

                strncpy ( tx_param, pcParameter1, xParameter1StringLength);
                channel  = (unsigned int) strtol(pcParameter2, NULL, 10);
                if(strncmp(tx_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_tx_ch_on(NULL, channel, true);
                        readBackLen = sprintf(readBack, "\r\n TX-%d enbale\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(tx_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_tx_ch_on(NULL, channel, false);
                        readBackLen = sprintf(readBack, "\r\n TX-%d disable\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_tx 2 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else if(pcParameter1 != NULL && pcParameter2 == NULL) {
        /* one parameters: IF output for all channel */

                strncpy ( tx_param, pcParameter1, xParameter1StringLength);
                if(strncmp(tx_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_tx_ch_on(NULL, -1, true);
                        readBackLen = sprintf(readBack, "\r\n All tx channel enable\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(tx_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_tx_ch_on(NULL, -1, false);
                        readBackLen = sprintf(readBack, "\r\n All tx channel disable\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_tx 1 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else {
                readBackLen = sprintf(readBack, "\r\n tx parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_tx done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}

/* radio single tone *****************************************************************/

/*
 * command   : radio_single_tone
 * parameter : <start frequency> - in GHz
 *
 */

static const CLI_Command_Definition_t xRadioSingleTone = {
        "radio_single_tone",      /* The command string to type. */
        "\tradio_single_tone \r\n"
        "\tUsage: <frequency GHz> <on/off>\r\n",
        radioSingleToneCommand,   /* The function to run.        */
        -1                       /* 1 command parameter.        */
};

static BaseType_t radioSingleToneCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2;
        BaseType_t len1, len2;
        double freq;
        int32_t status;
        bool enable = true;
        baseband_t *bb = baseband_get_bb(0);

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

        if(pcParameter1 != NULL){
        /* one parameters: generate single tone */
                freq  = (double) strtod(pcParameter1, NULL);

                if (strncmp(pcParameter2, "off", sizeof("off") - 1) == 0)
                        enable = false;
                else
                        enable = true;

                status = fmcw_radio_single_tone(&bb->radio, freq, enable);
                if (status != E_OK) {
                        readBackLen = sprintf(readBack, " radio_single_tone pll unlocked! \r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else {
        /* no parameter */
                readBackLen = sprintf(readBack, " radio_single_tone parameter error \r\n");
                strncat(pcWriteBuffer, readBack, readBackLen);
                print_help(pcWriteBuffer, xWriteBufferLen, &xRadioSingleTone);
        }

        /* display info */
        if (enable == true)
                readBackLen = sprintf(readBack, "\r\n radio_single_tone on\r\n");
        else
                readBackLen = sprintf(readBack, "\r\n radio_single_tone off\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}

/* radio highpass filter *******************************************************/

/*
 * command   : radio_hpf
 * parameter : <NA> - get highpass filter of all channel
 *             <filter_id> <filter_index> - set filter_index to each channel's filter_id
 *             <ch_index> <filter_id> <filter_index> - set filter_index to ch_index's filter_id
 *
 * note      : HPF1_200KHz   : 0x1           HPF2_1MHz     : 0x0
 *             HPF1_150KHz   : 0x2           HPF2_750KHz   : 0x1
 *             HPF1_120KHz   : 0x3           HPF2_600KHz   : 0x2
 *             HPF1_100KHz   : 0x4           HPF2_500KHz   : 0x3
 *             HPF1_85KHz    : 0x5           HPF2_425KHz   : 0x4
 *             HPF1_75KHz    : 0x6           HPF2_375KHz   : 0x5
 *             HPF1_12KHz    : 0x7           HPF2_333KHz   : 0x6
 *
 */

static const CLI_Command_Definition_t xRadioHpf = {
        "radio_hpf",      /* The command string to type. */
        "\r\nradio_hpf \r\n"
        "\t <NA> - get highpass filter of all channel\r\n"
        "\t <filter_id> <filter_index> - set filter_index to each channel's filter_id\r\n"
        "\t <ch_index> <filter_id> <filter_index> - set filter_index to ch_index's filter_id\r\n",
        radioHpfCommand, /* The function to run.        */
        -1                   /* Up to 3 commands.           */
};

static BaseType_t radioHpfCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2, *pcParameter3;
        BaseType_t xParameter1StringLength, xParameter2StringLength, xParameter3StringLength;
        int ch_index, filter_id, filter_index;
        sensor_config_t *cfg = sensor_config_get_config(0);

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);
        pcParameter3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &xParameter3StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL && pcParameter3 != NULL){
        /* three parameters: set highpass filter */

                ch_index = (int)strtol(pcParameter1, NULL, 10);
                filter_id = (int)strtol(pcParameter2, NULL, 10);
                filter_index = (int)strtol(pcParameter3, NULL, 10);

                if (filter_id==1){
                        fmcw_radio_set_hpf1(NULL, ch_index, filter_index);
                        cfg->rf_hpf1 = filter_index;
                        if (ch_index == -1)
                                readBackLen = sprintf(readBack, "\r\n set %d to all channels' HPF1\r\n",
                                                      filter_index);
                        else
                                readBackLen = sprintf(readBack, "\r\n set %d to channel %d's HPF1\r\n",
                                                      filter_index, ch_index);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if (filter_id==2){
                        fmcw_radio_set_hpf2(NULL, ch_index, filter_index);
                        cfg->rf_hpf2 = filter_index;
                        if (ch_index == -1)
                                readBackLen = sprintf(readBack, "\r\n set %d to all channels' HPF2\r\n",
                                                      filter_index);
                        else
                                readBackLen = sprintf(readBack, "\r\n set %d to channel %d's HPF2\r\n",
                                                      filter_index, ch_index);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &xRadioHpf);
                }

        } else if(pcParameter1 != NULL && pcParameter2 != NULL) {
        /* two parameters: set highpass filter */

                filter_id = (int)strtol(pcParameter1, NULL, 10);
                filter_index = (int)strtol(pcParameter2, NULL, 10);

                if (filter_id==1){
                        fmcw_radio_set_hpf1(NULL, -1, filter_index);
                        cfg->rf_hpf1 = filter_index;
                        readBackLen = sprintf(readBack, "\r\n set %d to all channels' HPF1\r\n",
                                              filter_index);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if (filter_id==2){
                        fmcw_radio_set_hpf2(NULL, -1, filter_index);
                        cfg->rf_hpf2 = filter_index;
                        readBackLen = sprintf(readBack, "\r\n set %d to all channels' HPF2\r\n",
                                              filter_index);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        print_help(pcWriteBuffer, xWriteBufferLen, &xRadioHpf);
                }

        } else if(pcParameter1 == NULL && pcParameter2 == NULL && pcParameter2 == NULL) {
        /* no parameters: set highpass filter */

                int filter_index, channel;
                for (channel = 0; channel < MAX_NUM_TX; channel++){
                        filter_index = fmcw_radio_get_hpf1(NULL, channel);
                        readBackLen = sprintf(readBack, "  CH%d  HPF1 %d   ", channel, filter_index);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                        filter_index = fmcw_radio_get_hpf2(NULL, channel);
                        readBackLen = sprintf(readBack, "HPF2 %d\r\n", filter_index);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else {
        /* one parameter: error */
                print_help(pcWriteBuffer, xWriteBufferLen, &xRadioHpf);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_hpf done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}


/* radio hp12 auto switch ***********************************************************/

/*
 * command   : radio_hpa
 * parameter : <on/off> - enable or disable to all hp auto channel
 *             <on/off> <ch_index> - enable or disable index to hp auto channel index
 *
 * note      : channel index - 0 / 1 / 2 / 3
 *
 */

static const CLI_Command_Definition_t xRadioHpa = {
        "radio_hpa",      /* The command string to type. */
        "\r\nradio_hpa \r\n"
        "\t <on/off> - enable or disable to all hp auto channel\r\n"
        "\t <on/off> <ch_index> - enable or disable index to hp auto channel index\r\n",
        radioHpaCommand, /* The function to run.        */
        -1                 /* Up to 2 commands.           */
};

static BaseType_t radioHpaCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2;
        BaseType_t xParameter1StringLength, xParameter2StringLength;
        char hpa_param[10];
        unsigned int channel;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL){
        /* two parameters: for single channel */

                strncpy ( hpa_param, pcParameter1, xParameter1StringLength);
                channel  = (unsigned int) strtol(pcParameter2, NULL, 10);
                if(strncmp(hpa_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_hp_auto_ch_on(NULL, channel);
                        readBackLen = sprintf(readBack, "\r\n HP Auto-%d enbale\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(hpa_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_hp_auto_ch_off(NULL, channel);
                        readBackLen = sprintf(readBack, "\r\n HP Auto-%d disable\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_hpa 2 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else if(pcParameter1 != NULL && pcParameter2 == NULL) {
        /* one parameters: for all channel */

                strncpy ( hpa_param, pcParameter1, xParameter1StringLength);
                if(strncmp(hpa_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_hp_auto_ch_on(NULL, -1);
                        readBackLen = sprintf(readBack, "\r\n All hp auto channel enable\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(hpa_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_hp_auto_ch_off(NULL, -1);
                        readBackLen = sprintf(readBack, "\r\n All hp auto channel disable\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_hpa 1 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else {
                readBackLen = sprintf(readBack, "\r\n hpa parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_hpa done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}

/* radio tx auto switch ***********************************************************/

/*
 * command   : radio_autotx
 * parameter : <on/off> <ch_index> <mode_sel> - enable or disable index to tx auto channel index
 *
 * note      : channel index - 0 / 1 / 2 / 3 /-1(all channel)
 *             auto tx mode selection
 *             0000 = idle start, down and idle state
 *             0001 = idle start and down state
 *             0010 = idle start and idle state
 *             0011 = idle start state
 *             0100 = down and idle state
 *             0101 = down state
 *             0110 = idle state
 *             0111 = off
 *             1111 = no mode selected, keep old mode
 *             enable: 0x1--on, 0x0--off
 */

static const CLI_Command_Definition_t xRadioAutoTX = {
        "radio_autotx",      /* The command string to type. */
        "\r\nradio_autotx \r\n"
        "\t <on/off> <ch_index> <mode_sel> - enable or disable index to tx auto channel index\r\n"
        "\t <ch_index> - 0 / 1 / 2 / 3 / -1(all channel)\r\n"
        "\t <mode_sel> - 0(idle start, dn, idle)/1(idle start, dn)/2(idle start, idle)/3(idle start)"
        "/4(dn, idle)/5(dn)/6(idle)/7(off)/15(keep old mode)\r\n",
        radioAutoTXCommand, /* The function to run.        */
        -1                 /* Up to 3 commands.           */
};

static BaseType_t radioAutoTXCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        const char *pcParameter1, *pcParameter2, *pcParameter3;
        BaseType_t xParameter1StringLength, xParameter2StringLength, xParameter3StringLength;
        char autotx_param[10];
        unsigned int channel, mode_sel;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);
        pcParameter2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &xParameter2StringLength);
        pcParameter3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &xParameter3StringLength);

        if(pcParameter1 != NULL && pcParameter2 != NULL && pcParameter3 != NULL){
        /* three parameters: get tx auto mode selection*/

                strncpy ( autotx_param, pcParameter1, xParameter1StringLength);
                channel   = (unsigned int) strtol(pcParameter2, NULL, 10);
                mode_sel  = (unsigned int) strtol(pcParameter3, NULL, 10);
                if(strncmp(autotx_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_tx_auto_ch_on(NULL, channel, mode_sel);
                        readBackLen = sprintf(readBack, "\r\n TX%d Auto enbale\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(autotx_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_tx_auto_ch_off(NULL, channel);
                        readBackLen = sprintf(readBack, "\r\n TX%d Auto disable\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_autotx 3 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else if(pcParameter1 != NULL && pcParameter2 != NULL && pcParameter3 == NULL) {
        /* two parameters: no tx auto mode selection*/

                strncpy ( autotx_param, pcParameter1, xParameter1StringLength);
                channel   = (unsigned int) strtol(pcParameter2, NULL, 10);
                if(strncmp(autotx_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_tx_auto_ch_on(NULL, channel, 0xf);
                        readBackLen = sprintf(readBack, "\r\n TX%d Auto enbale\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(autotx_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_tx_auto_ch_off(NULL, channel);
                        readBackLen = sprintf(readBack, "\r\n TX%d Auto disbale\r\n", channel);
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_autotx 2 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else if(pcParameter1 != NULL && pcParameter2 == NULL && pcParameter3 == NULL) {
        /* one parameters: for all channel */

                strncpy ( autotx_param, pcParameter1, xParameter1StringLength);
                if(strncmp(autotx_param, "on", sizeof("on") - 1) == 0) {
                        fmcw_radio_tx_auto_ch_on(NULL, -1, 0xf);
                        readBackLen = sprintf(readBack, "\r\n All tx auto channel enable\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else if(strncmp(autotx_param, "off", sizeof("off") - 1) == 0) {
                        fmcw_radio_tx_auto_ch_off(NULL, -1);
                        readBackLen = sprintf(readBack, "\r\n All tx auto channel disable\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                } else {
                        readBackLen = sprintf(readBack, "\r\n radio_autotx 1 parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
                }

        } else {
                readBackLen = sprintf(readBack, "\r\n autotx parameter error\r\n");
                        strncat(pcWriteBuffer, readBack, readBackLen);
        }

        /* display info */
        readBackLen = sprintf(readBack, "\r\nradio_autotx done\r\n");
        strncat(pcWriteBuffer, readBack, readBackLen);

        /* return */
        /* There is only a single line of output produced in all cases.  pdFALSE is
         returned because there is no more output to be generated. */
        return pdFALSE;
}

/* radio auxadc1 voltage *********************************************************/

/*
 * command   : radio_auxadc1_voltage
 * parameter : <inputmux selection> - get selected auxadc1 voltage of radio
 *
 * note      : AUXADC1_MainBG_VPTAT:     0
 *             AUXADC1_TestMUXN:         1
 *             AUXADC1_TestMUXP:         2
 *             AUXADC1_TPANA1:           3
 */

static const CLI_Command_Definition_t xRadioAUXADC1Voltage = {
        "radio_auxadc1_voltage",     /* The command string to type. */
        "\r\nradio_auxadc1_voltage\r\n"
        "\t <muxin_sel> - get selected auxadc1 voltage of radio\r\n"
        "\t selection  - 0(MainBG_VPTAT) / 1(TestMUXN) / 2(TestMUXP) / 3(TPANA1)\r\n",
        radioAUXADC1VoltageCommand,   /* The function to run.        */
        -1                            /* 1 command parameter.        */
};

static BaseType_t radioAUXADC1VoltageCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){
        const char *pcParameter1;
        BaseType_t len1;
        float muxin_sel;
        float voltage = 0;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        if(pcParameter1 != NULL){
        /* one parameters: inputmux selection */
                muxin_sel  = (float) strtof(pcParameter1, NULL);
                voltage = fmcw_radio_auxadc1_voltage(NULL,muxin_sel);
                /* display info */
                sprintf(pcWriteBuffer, "\r\n radio auxadc1 voltage: %.3f \r\n", voltage);
        }
        else{
        /* no parameter */
                /* display info */
                sprintf(pcWriteBuffer, "\r\n please select inputmux and try again \r\n");
                print_help(pcWriteBuffer, xWriteBufferLen, &xRadioAUXADC1Voltage);
        }

        /* return */
        return pdFALSE;
}

/* radio auxadc2 voltage *********************************************************/

/*
 * command   : radio_auxadc2_voltage
 * parameter : <inputmux selection> - get selected auxadc2 voltage of radio
 *
 * note      : AUXADC2_TS_VPTAT:     0
 *             AUXADC2_TS_VBG:       1
 *             AUXADC2_TestMUXN:     2
 *             AUXADC2_TPANA2:       3
 */

static const CLI_Command_Definition_t xRadioAUXADC2Voltage = {
        "radio_auxadc2_voltage",     /* The command string to type. */
        "\r\nradio_auxadc2_voltage\r\n"
        "\t <muxin_sel> - get selected auxadc2 voltage of radio\r\n"
        "\t selection  - 0(TS_VPTAT) / 1(TS_VBG) / 2(TestMUXN) / 3(TPANA2)\r\n",
        radioAUXADC2VoltageCommand,   /* The function to run.        */
        -1                            /* 1 command parameter.        */
};

static BaseType_t radioAUXADC2VoltageCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){
        const char *pcParameter1;
        BaseType_t len1;
        float muxin_sel;
        float voltage = 0;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        /* obtain the parameter. */
        pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        if(pcParameter1 != NULL){
        /* one parameters: inputmux selection */
                muxin_sel  = (float) strtof(pcParameter1, NULL);
                voltage = fmcw_radio_auxadc2_voltage(NULL,muxin_sel);
                /* display info */
                sprintf(pcWriteBuffer, "\r\n radio auxadc1 voltage: %.3f \r\n", voltage);
        }
        else{
        /* no parameter */
                /* display info */
                sprintf(pcWriteBuffer, "\r\n please select inputmux and try again \r\n");
                print_help(pcWriteBuffer, xWriteBufferLen, &xRadioAUXADC1Voltage);
        }

        /* return */
        return pdFALSE;
}

/* radio rf compensation code *********************************************************/

/*
 * command   : radio_rf_comp_code
 * parameter : <NA> - provide rf compensation code vs junction temperature higher than 55C
 *
 */

static const CLI_Command_Definition_t xRadioRFCompCode = {
        "radio_rf_comp_code",     /* The command string to type. */
        "\r\nradio_rf_comp_code\r\n"
        "\t <NA> - provide rf compensation code vs junction temperature higher than 55C\r\n",
        radioRFCompCodeCommand,  /* The function to run.        */
        0                        /* No parameters are expected. */
};

static BaseType_t radioRFCompCodeCommand(char *pcWriteBuffer,
        size_t xWriteBufferLen, const char *pcCommandString){

        float temperature = 0;

        /* clean write buffer */
        memset( pcWriteBuffer, 0x00, xWriteBufferLen );

        temperature = fmcw_radio_rf_comp_code(NULL);

        /* display info */
        if (temperature < 55)
                sprintf(pcWriteBuffer, "\r\n radio temperature: %.2f, no need to apply code\r\n", temperature);
        else
                sprintf(pcWriteBuffer, "\r\n radio temperature: %.2f, compensation code applied\r\n", temperature);

        /* return */
        return pdFALSE;
}


/* command *******************************************************************/

void fmcw_radio_cli_commands( void ){
        if (fmcw_radio_cli_registered)
                return;
        FreeRTOS_CLIRegisterCommand(&xRadioReg);
        FreeRTOS_CLIRegisterCommand(&xRadioRegDump);
        FreeRTOS_CLIRegisterCommand(&xRadioPowerOn);
        FreeRTOS_CLIRegisterCommand(&xRadioPowerOff);
        FreeRTOS_CLIRegisterCommand(&xRadioTemp);
        FreeRTOS_CLIRegisterCommand(&xRadioSaturation);
        FreeRTOS_CLIRegisterCommand(&xRadioRxGain);
        FreeRTOS_CLIRegisterCommand(&xRadioTxPower);
        FreeRTOS_CLIRegisterCommand(&xRadioTxPhase);
        FreeRTOS_CLIRegisterCommand(&xRadioFmcw);
        FreeRTOS_CLIRegisterCommand(&xRadioPll);
        FreeRTOS_CLIRegisterCommand(&xRadioVctrl);
        FreeRTOS_CLIRegisterCommand(&xRadioIfout);
        FreeRTOS_CLIRegisterCommand(&xRadioTx);
        FreeRTOS_CLIRegisterCommand(&xRadioSingleTone);
        FreeRTOS_CLIRegisterCommand(&xRadioHpf);
        FreeRTOS_CLIRegisterCommand(&xRadioHpa);
        FreeRTOS_CLIRegisterCommand(&xRadioAutoTX);
        FreeRTOS_CLIRegisterCommand(&xRadioAUXADC1Voltage);
        FreeRTOS_CLIRegisterCommand(&xRadioAUXADC2Voltage);
        FreeRTOS_CLIRegisterCommand(&xRadioRFCompCode);
        fmcw_radio_cli_registered = true;
}
