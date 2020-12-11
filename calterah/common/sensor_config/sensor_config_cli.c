#include "embARC.h"
#include "embARC_toolchain.h"
#include "embARC_assert.h"
#include "embARC_debug.h"
#include "sensor_config.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include <string.h>
#include <stdlib.h>
#include "flash.h"

#include "spi_master.h"
#include "cascade.h"
#include "baseband_cas.h"
#include "sensor_config_cli.h"
#include "flash_mmap.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static bool sensor_config_cli_registered = false;

/* helpers */
static void print_help(char* buffer, size_t buff_len, const CLI_Command_Definition_t* cmd)
{
        uint32_t tot_count = 0;
        int32_t count = sprintf(buffer, "Wrong input\n\r %s", cmd->pcHelpString);
        EMBARC_ASSERT(count > 0);
        tot_count += count;
        EMBARC_ASSERT(tot_count < buff_len);
}

static BaseType_t sensor_cfg_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t ant_prog_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t ang_calib_prog_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t sensor_cfg_sel_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

/* sensor_cfg command */
static const CLI_Command_Definition_t sensor_cfg_command = {
        "sensor_cfg",
        "sensor_cfg \n\r"
        "\tWrite or read sensor config. \n\r"
        "\tUsage: sensor_cfg [[param] value] \n\r",
        sensor_cfg_command_handler,
        -1
};

static BaseType_t sensor_cfg_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
#ifdef CHIP_CASCADE
        const char *par1, *par2;
        BaseType_t l1, l2;
        par1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &l1);
        par2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &l2);
        if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                if ((par1 != NULL) && (par2 != NULL)) {
                        EMBARC_PRINTF(pcCommandString);
                        EMBARC_PRINTF(" sent to slave \n");
                        baseband_write_cascade_cmd(pcCommandString); // send "sensor_cfg" command string to slave chip through spi in cascade mode
                }
        }
#endif
        #include "sensor_config_cmd.hxx"
}

/* ant_prog command */
static const CLI_Command_Definition_t ant_prog_command = {
        "ant_prog",
        "ant_prog\n\r"
        "\tProgram or read antenna pos/comp on flash. \n\r"
        "\tUsage: ant_prog [[pos/comp] val1, val2, ...]\n\r",
        ant_prog_command_handler,
        -1
};

static int32_t dump_ant_pos_info(char *pcWriteBuffer, size_t xWriteBufferLen, sensor_config_t *cfg, int32_t *offset)
{
        int32_t status;
        uint8_t *buff = sensor_config_get_buff();
        status = flash_memory_read(cfg->ant_info_flash_addr, buff, ANTENNA_INFO_LEN);
        if (status != E_OK) {
                snprintf(pcWriteBuffer, xWriteBufferLen - 1, "Fail to read on-flash antenna pos info!\n\r");
        } else {
                uint32_t *tmp = (uint32_t *)buff;
                if (*tmp == ANTENNA_INFO_MAGICNUM) {
                        antenna_pos_t *pos = (antenna_pos_t *)(buff + 4);
                        *offset = snprintf(pcWriteBuffer, xWriteBufferLen - 1, "ant_pos = ");
                        for(int cnt = 0; cnt < MAX_ANT_ARRAY_SIZE && (*offset) < xWriteBufferLen; cnt++) {
                                *offset += snprintf(pcWriteBuffer + (*offset), xWriteBufferLen - 1 - (*offset), "(%.2f, %.2f), ", pos[cnt].x, pos[cnt].y);
                                if (cnt % 4 == 3)
                                        *offset += snprintf(pcWriteBuffer + (*offset), xWriteBufferLen - 1 - (*offset), "\n\r");
                        }
                } else {
                        snprintf(pcWriteBuffer, xWriteBufferLen-1, "Invalid on-flash antenna pos info!\n\r");
                }
        }
        return status;
}

static int32_t dump_ant_comp_info(char *pcWriteBuffer, size_t xWriteBufferLen, sensor_config_t *cfg, int32_t *offset)
{
        uint8_t *buff = sensor_config_get_buff();
        int32_t status = flash_memory_read(cfg->ant_info_flash_addr + ANTENNA_INFO_LEN, buff, ANTENNA_INFO_LEN);
        if (status != E_OK) {
                snprintf(pcWriteBuffer, xWriteBufferLen-1, "Fail to read on-flash antenna comp info!\n\r");
        } else {
                uint32_t *tmp = (uint32_t *)buff;
                if (*tmp == ANTENNA_INFO_MAGICNUM) {
                        float *comp = (float *)(buff + 4);
                        *offset += snprintf(pcWriteBuffer + (*offset), xWriteBufferLen - 1 - (*offset), "\n\rant_comps = ");
                        for(int cnt = 0; cnt < MAX_ANT_ARRAY_SIZE && (*offset) < xWriteBufferLen; cnt++)
                                *offset += snprintf(pcWriteBuffer + (*offset), xWriteBufferLen - 1 - (*offset), "%.2f, ", comp[cnt]);
                        *offset += snprintf(pcWriteBuffer + (*offset), xWriteBufferLen - 1 - (*offset), "\n\r");
                } else {
                        snprintf(pcWriteBuffer, xWriteBufferLen-1, "Invalid on-flash antenna comp info!\n\r");
                }
        }
        return status;
}

static BaseType_t ant_prog_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2;
        BaseType_t len1, len2;
        int32_t status;
        int32_t offset = 0;
        sensor_config_clear_buff();
        uint8_t *buff = sensor_config_get_buff();
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

        baesband_frame_interleave_cnt_clr(); // clear recfg count in frame interleaving
        baseband_t *bb = baseband_frame_interleave_recfg(); // reconfigure the frame pattern for frame-interleaving
        sensor_config_t *cfg = (sensor_config_t*)(bb->cfg);

        if (param1 != NULL) {
                if (strncmp(param1, "pos", len1) == 0) {
                        if (param2 == NULL) {
                                status = dump_ant_pos_info(pcWriteBuffer, xWriteBufferLen, cfg, &offset);
                        } else {
                                uint32_t cnt = 0;
                                uint32_t *tmp = (uint32_t *)buff;
                                *tmp = ANTENNA_INFO_MAGICNUM;
                                antenna_pos_t *pos = (antenna_pos_t *)(buff + 4);
                                while(param2 != NULL && cnt < MAX_ANT_ARRAY_SIZE * 2) {
                                        if (cnt % 2 == 0)
                                                pos[cnt/2].x = (float) strtof(param2, NULL);
                                        else
                                                pos[cnt/2].y = (float) strtof(param2, NULL);
                                        cnt++;
                                        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                                }
                                status = flash_memory_write(cfg->ant_info_flash_addr, buff, ANTENNA_INFO_LEN);
                                if (status != E_OK)
                                        snprintf(pcWriteBuffer, xWriteBufferLen-1, "Fail to write ant-pos on flash!\n\r");
                                else
                                        dump_ant_pos_info(pcWriteBuffer, xWriteBufferLen, cfg, &offset);
                        }
                } else if (strncmp(param1, "comp", len1) == 0) {
                        if (param2 == NULL) {
                                status = dump_ant_comp_info(pcWriteBuffer, xWriteBufferLen, cfg, &offset);
                        } else {
                                uint32_t cnt = 0;
                                uint32_t *tmp = (uint32_t *)buff;
                                *tmp = ANTENNA_INFO_MAGICNUM;
                                float *comp = (float *)(buff + 4);
                                while(param2 != NULL && cnt < MAX_ANT_ARRAY_SIZE) {
                                        comp[cnt++] = (float) strtof(param2, NULL);
                                        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                                }
                                status = flash_memory_write(cfg->ant_info_flash_addr + ANTENNA_INFO_LEN, buff, ANTENNA_INFO_LEN);
                                if (status != E_OK)
                                        snprintf(pcWriteBuffer, xWriteBufferLen-1, "Fail to write ant-pos on flash!\n\r");
                                else
                                        dump_ant_comp_info(pcWriteBuffer, xWriteBufferLen, cfg, &offset);
                        }
                } else
                        print_help(pcWriteBuffer, xWriteBufferLen, &ant_prog_command);
                return pdFALSE;
        } else {
                status = dump_ant_pos_info(pcWriteBuffer, xWriteBufferLen, cfg, &offset);
                if (status != E_OK)
                        return pdFALSE;
                status = dump_ant_comp_info(pcWriteBuffer, xWriteBufferLen, cfg, &offset);
                if (status != E_OK)
                        return pdFALSE;
                return pdFALSE;
        }
}

/* ang_prog command */
static const CLI_Command_Definition_t ang_calib_prog_command = {
        "ang_calib_prog",
        "ang_calib_prog\n\r"
        "\tProgram or read angle calib on flash. \n\r"
        "\tUsage: ang_calib_prog [[packet1/packet2] val1, val2, ...]\n\r",
        ang_calib_prog_command_handler,
        -1
};

uint8_t strtohex(const char *str)
{
        uint8_t ang_str1,ang_str2,ang_str3;

        ang_str1 = *str;
        if(ang_str1 > 0x39)
                ang_str1 -= 0x37;
        else
                ang_str1 -= 0x30;

        ang_str2 = *(str+1);
        if(ang_str2 > 0x39)
                ang_str2 -= 0x37;
        else
                ang_str2 -= 0x30;

        ang_str3 = (ang_str1 << 4) + ang_str2;

        return ang_str3;
}

static uint8_t Accumulate_Checksum(uint8_t *data, uint16_t len)
{
        uint16_t i;
        uint8_t checksum = 0;

        for (i = 0 ; i < len ; i++)
                checksum += data[i];

        return checksum;
}

static BaseType_t ang_calib_prog_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1, *param2;
        BaseType_t len1, len2;
        int32_t status;
        uint8_t checksum = 0;
        uint32_t cnt = 0;
        uint8_t *ptr_ang_calib_save;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

        if (param1 != NULL) {
                if (strncmp(param1, "packet1", len1) == 0) {
                        if (param2 == NULL) {
                                //status = dump_ant_comp_info(pcWriteBuffer, xWriteBufferLen, cfg, &offset);
                        } else {
                                ptr_ang_calib_save = (uint8_t*)pvPortMalloc(ANGLE_CALIB_INFO_LEN);
                                cnt = 0;
                                while(param2 != NULL && cnt < ANGLE_CALIB_INFO_LEN) {
                                        ptr_ang_calib_save[cnt++] = strtohex(param2);
                                        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                                }

                                //write packet1 data to flash
                                checksum = 0;
                                checksum = Accumulate_Checksum(ptr_ang_calib_save, ANGLE_CALIB_INFO_LEN-1);
                                if(checksum == ptr_ang_calib_save[ANGLE_CALIB_INFO_LEN-1]){
                                        status = flash_memory_erase(FLASH_ANGLE_CALIBRATION_INFO_BASE, FLASH_PAGE_SIZE*2);
                                        if (status != E_OK)
                                                EMBARC_PRINTF("FLASH_ANGLE_CALIBRATION_INFO_BASE ERASE FAIL!\r\n");
                                        else{
                                                status = flash_memory_write(FLASH_ANGLE_CALIBRATION_INFO_BASE, ptr_ang_calib_save, ANGLE_CALIB_INFO_LEN);
                                                if (status != E_OK)
                                                        snprintf(pcWriteBuffer, xWriteBufferLen-1, "Fail to write ang-calib on flash!\n\r");
                                                else
                                                        EMBARC_PRINTF("write ang calib data packet1 to flash ok!\r\n");
                                        }
                                } else{
                                        EMBARC_PRINTF("uart rx ang packet1 alib data checksum error : %x\n", checksum);
                                }
                                vPortFree(ptr_ang_calib_save);
                        }
                }
                else if (strncmp(param1, "packet2", len1) == 0) {
                        if (param2 == NULL) {
                                //status = dump_ant_comp_info(pcWriteBuffer, xWriteBufferLen, cfg, &offset);
                        } else {
                                ptr_ang_calib_save = (uint8_t*)pvPortMalloc(ANGLE_CALIB_INFO_LEN);
                                cnt = 0;
                                while(param2 != NULL && cnt < ANGLE_CALIB_INFO_LEN) {
                                        ptr_ang_calib_save[cnt++] = strtohex(param2);
                                        param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2+cnt, &len2);
                                }

                                uint8_t checksum = 0;
                                checksum = Accumulate_Checksum(ptr_ang_calib_save, ANGLE_CALIB_INFO_LEN-1);
                                if(checksum == ptr_ang_calib_save[ANGLE_CALIB_INFO_LEN-1]){
                                        status = flash_memory_write(FLASH_ANGLE_CALIBRATION_INFO_BASE+FLASH_PAGE_SIZE, ptr_ang_calib_save, ANGLE_CALIB_INFO_LEN);
                                        if (status != E_OK)
                                                snprintf(pcWriteBuffer, xWriteBufferLen-1, "Fail to write ang-calib packet2 on flash!\n\r");
                                        else
                                                EMBARC_PRINTF("write ang calib data packet2 to flash ok!\r\n");
                                } else{
                                        EMBARC_PRINTF("uart rx ang calib data packet2 checksum error : %x\n\r", checksum);
                                }
                                vPortFree(ptr_ang_calib_save);
                        }
                } else
                        print_help(pcWriteBuffer, xWriteBufferLen, &ant_prog_command);

                return pdFALSE;
        }else {
                return pdFALSE;
        }
}

void ang_calib_data_init(float *data_buf)
{
        int32_t status;
        uint8_t checksum_packet1 = 0;
        uint8_t checksum_packet2 = 0;
        uint8_t *ptr_ang_calib_read = (uint8_t*)pvPortMalloc(FLASH_PAGE_SIZE*2);
        uint16_t i;
        int16_t temp;

        status = flash_memory_read(FLASH_ANGLE_CALIBRATION_INFO_BASE, ptr_ang_calib_read, FLASH_PAGE_SIZE*2);
        if (status != E_OK) {
                EMBARC_PRINTF("Fail to read on-flash ang calib data. Fallback to default setting!\n\r");
                return;
        }

        checksum_packet1 = Accumulate_Checksum(ptr_ang_calib_read, ANGLE_CALIB_INFO_LEN-1);
        checksum_packet2 = Accumulate_Checksum(&ptr_ang_calib_read[FLASH_PAGE_SIZE], ANGLE_CALIB_INFO_LEN-1);
        if((checksum_packet1 == ptr_ang_calib_read[ANGLE_CALIB_INFO_LEN-1]) && (checksum_packet2 == ptr_ang_calib_read[FLASH_PAGE_SIZE+ANGLE_CALIB_INFO_LEN-1])){
                EMBARC_PRINTF("read flash ang calib data checksum ok!\n\r");
                //update packet1 uint8 ang calib data to float
                for(i = 0; i < (ANGLE_CALIB_INFO_LEN-1); i+=2) {
                        temp = (int16_t)((((int16_t)ptr_ang_calib_read[i]) << 8) + ptr_ang_calib_read[i+1]);
                        data_buf[i/2] = ((float)temp)/100;
                }

                //update packet2 uint8 ang calib data to float
                for(i = FLASH_PAGE_SIZE; i < (FLASH_PAGE_SIZE+ANGLE_CALIB_INFO_LEN-1); i+=2) {
                        temp = (int16_t)((((int16_t)ptr_ang_calib_read[i]) << 8) + ptr_ang_calib_read[i+1]);
                        data_buf[(i+ANGLE_CALIB_INFO_LEN-FLASH_PAGE_SIZE)/2] = ((float)temp)/100;
                }
                EMBARC_PRINTF("update packet2 uint8 ang calib data to float ok!\n\r");
        } else
                EMBARC_PRINTF("read flash ang calib data checksum fail!\n\r");

        vPortFree(ptr_ang_calib_read);
}

/* ant_prog command */
static const CLI_Command_Definition_t sensor_cfg_sel_command = {
        "sensor_cfg_sel",
        "sensor_cfg_sel\n\r"
        "\tShow/Select current config index.\n\r"
        "\tUsage: sensor_cfg_sel [idx]\n\r",
        sensor_cfg_sel_handler,
        -1
};

void sensor_cfg_sel_message(char *pcWriteBuffer, size_t xWriteBufferLen)
{
        uint32_t offset = snprintf(pcWriteBuffer, xWriteBufferLen - 1, "\n\rAvailable Configs: ");
        for(int c = 0; c < NUM_FRAME_TYPE; c++) {
                if (c == sensor_config_get_cur_cfg_idx())
                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - 1 - offset, "%d*  ", c);
                else
                        offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - 1 - offset, "%d  ", c);       
        }
        snprintf(pcWriteBuffer + offset, xWriteBufferLen - 1 - offset, "\n\r"); 
}

static BaseType_t sensor_cfg_sel_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
        const char *param1;
        BaseType_t len1;
        param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
        if (param1 == NULL) {
                sensor_cfg_sel_message(pcWriteBuffer, xWriteBufferLen);
        } else {
                int idx = strtol(param1, NULL, 0);
                sensor_config_set_cur_cfg_idx(idx);
                sensor_cfg_sel_message(pcWriteBuffer, xWriteBufferLen);
        }

        return pdFALSE;
}



void sensor_config_cli_commands(void)
{
        if (sensor_config_cli_registered)
                return;
        FreeRTOS_CLIRegisterCommand(&sensor_cfg_command);
        FreeRTOS_CLIRegisterCommand(&ant_prog_command);
        FreeRTOS_CLIRegisterCommand(&ang_calib_prog_command);
        FreeRTOS_CLIRegisterCommand(&sensor_cfg_sel_command);        
        sensor_config_cli_registered = true;
}
