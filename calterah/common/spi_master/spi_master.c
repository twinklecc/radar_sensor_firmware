#include <string.h>
#include "FreeRTOS.h"
#include "embARC_assert.h"
#include "embARC_error.h"
#include "spi_hal.h"
#include "spi_master.h"

static spi_xfer_desc_t spim_xfer_desc = 
{
    .clock_mode = SPI_CLK_MODE_0,
    .dfs = 32,
    .cfs = 0,
    .spi_frf = SPI_FRF_STANDARD,
    .rx_thres = 0,
    .tx_thres = 0,
};

void spi_master_init(eSPIM_ID eSPIM_ID_Num, uint32_t SPIM_Baud_Rate)
{
    int32_t result = E_OK;

    if (eSPIM_ID_Num == eSPIM_ID_0){
        io_mux_spi_m0_func_sel(0);
    } else if (eSPIM_ID_Num == eSPIM_ID_1){
        io_mux_spi_m1_func_sel(0);
    }else{
        EMBARC_PRINTF("Invalid SPI Master ID. \r\n");
    }

    result = spi_open(eSPIM_ID_Num, SPIM_Baud_Rate);
    if (E_OK != result){
        EMBARC_PRINTF("Error: spi master init failed %d\r\n",result);
    }

    result = spi_transfer_config(eSPIM_ID_Num, &spim_xfer_desc);
    if (E_OK != result){
        EMBARC_PRINTF("Error: spi master xfer config failed %d.\r\n",result);
    }

}

