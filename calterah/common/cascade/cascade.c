#include <string.h>
#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dw_gpio.h"
#include "gpio_hal.h"
#include "spi_hal.h"
#include "cascade_config.h"
#include "cascade.h"
#include "frame.h"
#include "baseband_cas.h"

static uint32_t cas_tx_buf[BUF_SIZE_CASCADE];
static uint32_t cas_rx_buf_cmd[CMD_SIZE_CASCADE];

static uint32_t *cas_rx_buf = NULL;
static uint32_t cas_rx_len = 0;
static uint32_t cas_tx_len = 0;

/* write buffer */
void cascade_write_buf_req() /* allocation buffer */
{
        static uint32_t first_xfer = 0;
        if (1 == first_xfer) {
                while (E_OK !=cascade_transmit_done())
                        ; // waiting for IDLE of tx buf
        } else {
                first_xfer = 1;
        }

        cas_tx_len = 0;
}

void cascade_write_buf(uint32_t value)
{
        *(cas_tx_buf + cas_tx_len++) = value;
}

void cascade_write_buf_done()
{
        while (E_OK != cascade_write(cas_tx_buf, cas_tx_len))
                EMBARC_PRINTF("Error: cas_tx_spi failed >>> try again \r\n");
}

/* read buffer */
int32_t cascade_read_buf_req(TickType_t xTicksToWait) /* 1 tick 1ms*/
{
        return cascade_read(&cas_rx_buf, &cas_rx_len, xTicksToWait); /* wait for xTicksToWait */
}

uint32_t cascade_read_buf(uint32_t offset)
{
        volatile uint32_t *buf = (uint32_t *)(cas_rx_buf + offset);
        return *buf;
}

void cascade_read_buf_done()
{
        cascade_process_done();
}

/* command string tx/rx */
void cascade_write_buf_str(const char *pcCommandString)
{
        char *dest_addr = (char *)(cas_tx_buf + cas_tx_len);
        strcpy(dest_addr, pcCommandString); /* write cmd string */

        /* add 3 extra characters,  */
        uint32_t strlen_cmd = strlen(pcCommandString);
        dest_addr[strlen_cmd    ] = '\r';
        dest_addr[strlen_cmd + 1] = '\n';
        dest_addr[strlen_cmd + 2] = '\0';
        strlen_cmd = strlen_cmd + 3; /* add 3 extra characters */

        cas_tx_len += (strlen_cmd + 3) /4; /* char length to word length */
}

void cascade_read_buf_str()
{
        // move data to new buf, no occuping spi underlying buffer
        for (int32_t i = 1; i < cas_rx_len; i++)
                *(cas_rx_buf_cmd + i - 1) = cascade_read_buf(i);
}

char * cascade_spi_cmd_info(uint32_t *len)
{
        char *pcCommandString = (char *)(cas_rx_buf_cmd);
        *len = strlen(pcCommandString);

        return pcCommandString;
}
