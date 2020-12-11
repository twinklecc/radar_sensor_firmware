#ifndef SENSOR_CONFIG_H
#define SENSOR_CONFIG_H

#ifdef UNIT_TEST
#include "calterah_unit_test.h"
#else
#include "embARC_toolchain.h"
#endif

#include "calterah_math.h"
#include "calterah_limits.h"

typedef struct sensor_config {
        #include "sensor_config_def.hxx"
        uint32_t nvarray; /* put nvarray here to avoid tx_group config conflict to nvarray */
        void* bb;
} sensor_config_t;

void sensor_config_check(sensor_config_t *cfg);
int32_t sensor_config_attach(sensor_config_t *cfg, void *bb, uint8_t frame_type);
sensor_config_t *sensor_config_get_config(uint32_t idx);
uint8_t *sensor_config_get_buff();
void sensor_config_clear_buff();
sensor_config_t *sensor_config_get_cur_cfg();
uint8_t sensor_config_get_cur_cfg_idx();
void sensor_config_set_cur_cfg_idx(uint32_t idx);
void sensor_config_tx_group_check(sensor_config_t *cfg);

#define ANTENNA_INFO_LEN 512
#define ANTENNA_INFO_MAGICNUM 0xdeadbeefU

#endif
