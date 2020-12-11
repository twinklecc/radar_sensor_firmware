#ifndef SENSOR_CONFIG_CLI_H
#define SENSOR_CONFIG_CLI_H


#define ANGLE_CALIB_INFO_LEN       163
#define FLASH_PAGE_SIZE            256


void sensor_config_cli_commands(void);
void ang_calib_data_init(float *data_buf);

#endif
