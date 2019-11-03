

#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "driver\spi_common.h"
#include "driver\spi_master.h"
#include "driver\spi_slave.h"


#define PIN_NUM_MISO (19)
#define PIN_NUM_MOSI (23)
#define PIN_NUM_SCK  (18)
#define PIN_NUM_CS   (2)


esp_err_t BSP_SPI_Init(void * callback, int clock_speed);
void SPI_SendBuffer(spi_device_handle_t spi, uint8_t * buf, uint32_t buffer_length);
void SPI_GetBuffer(spi_device_handle_t spi, uint8_t * buf, uint32_t buffer_length);
void SPI_SetDataRate();



#endif