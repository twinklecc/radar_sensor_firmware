#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

typedef enum {
    eSPIM_ID_0 = 0,
    eSPIM_ID_1 = 1,
} eSPIM_ID;

#define SPIM_BAUD_RATE_1M (1000000)

void spi_master_init(eSPIM_ID eSPIM_ID_Num, uint32_t SPIM_Baud_Rate);

#endif

