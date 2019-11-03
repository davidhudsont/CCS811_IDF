

#include "bspSpi.h"
#include <string.h>

esp_err_t BSP_SPI_Init(void * callback, int clock_speed) 
{

    spi_host_device_t host = VSPI_HOST;
    spi_bus_config_t bus_config = {
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_SCK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=64
    };
    int dma_chan = 2;

    spi_device_handle_t spi;
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = clock_speed,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size=1,
        .post_cb=callback
    };

    esp_err_t init_fail;
    init_fail =  spi_bus_initialize(host, &bus_config, dma_chan);
    ESP_ERROR_CHECK(init_fail);

    esp_err_t device_add_fail;
    device_add_fail = spi_bus_add_device(host, &devcfg, &spi);
    ESP_ERROR_CHECK(device_add_fail);

    return init_fail || device_add_fail;
}

void SPI_SendBuffer(spi_device_handle_t spi, uint8_t * buf, uint32_t buffer_length)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    // buffer_length is in bytes the transaction length needs to be in bits
    transaction.length = buffer_length*8;

    transaction.tx_buffer = buf;

}