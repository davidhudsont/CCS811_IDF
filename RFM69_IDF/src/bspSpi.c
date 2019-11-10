/**
 * @file bspSpi.c
 * @author David Hudson
 * @brief Board support for controlling SPI interfaces on the ESP32Thing
 * @version 0.2
 * @date 2019-11-10
 * 
 * 
 */
#include "bspSpi.h"
#include <string.h>

spi_bus_config_t bus_config = {
    .miso_io_num=PIN_NUM_MISO,
    .mosi_io_num=PIN_NUM_MOSI,
    .sclk_io_num=PIN_NUM_SCK,
    .quadwp_io_num=-1,
    .quadhd_io_num=-1,
    .max_transfer_sz=12
};

spi_host_device_t host = VSPI_HOST;

const int dma_chan = 0;

spi_device_interface_config_t devcfg = {
    .mode = 0,
    .spics_io_num = PIN_NUM_CS,
    .queue_size=1
};

/**
 * @brief Initializes a SPI device on the VSPI Channel
 * 
 * @param spi - Device handle
 * @param clock_speed - spi data rate
 * @return esp_err_t - Returns an error if the intialization fails
 */
esp_err_t BSP_SPI_Init(spi_device_handle_t *spi, int clock_speed) 
{

    devcfg.clock_speed_hz = clock_speed; // set spi clock speed to passed in parameter
    devcfg.command_bits = 8; // Set the spi command to a 8bits
    esp_err_t init_fail = false;
    init_fail =  spi_bus_initialize(host, &bus_config, dma_chan); // Initialize spi bus per configurations
    ESP_ERROR_CHECK(init_fail);

    esp_err_t device_add_fail = false;
    device_add_fail = spi_bus_add_device(host, &devcfg, spi); // Add a device to the spi bus
    ESP_ERROR_CHECK(device_add_fail);

    return init_fail | device_add_fail;
}

/**
 * @brief - write 8bits to register with address as a command
 * 
 * @param spi - spi device handle
 * @param address - register address to write to
 * @param data - data to be written
 */
void SPI_writeReg(spi_device_handle_t spi, const uint8_t address, const uint8_t data)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.length = 8;
    transaction.flags = SPI_TRANS_USE_TXDATA;
    transaction.tx_data[0] = data;
    transaction.cmd = address;
    esp_err_t ret;
    ret = spi_device_polling_transmit(spi, &transaction);
    assert(ret == ESP_OK);
}


void SPI_SendByte8(spi_device_handle_t spi, const uint8_t data)
{
    spi_transaction_t transaction; 
    memset(&transaction, 0, sizeof(transaction));

    transaction.length=8;                     //Command is 8 bits
    transaction.tx_data[0] = data;               //The data is the cmd itself
    transaction.flags = SPI_TRANS_USE_TXDATA;
    printf("Command : %x\n", (unsigned int)transaction.tx_data[0]);
    esp_err_t ret;
    ret=spi_device_polling_transmit(spi, &transaction);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
}

/**
 * @brief reads a 8bit register from a spi device with
 *        address as a command
 * 
 * @param spi - spi device handle
 * @param address - address of the register
 * @return uint8_t - value stored in the register at the address
 */
uint8_t SPI_readReg(spi_device_handle_t spi, const uint8_t address)
{
    spi_transaction_t transaction; 
    memset(&transaction, 0, sizeof(transaction));

    transaction.cmd = address;
    transaction.length = 8;
    transaction.flags = SPI_TRANS_USE_RXDATA;

    esp_err_t ret;
    ret=spi_device_polling_transmit(spi, &transaction);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.

    return transaction.rx_data[0];
}




uint8_t SPI_GetByte8(spi_device_handle_t spi)
{
    spi_transaction_t transaction; 
    memset(&transaction, 0, sizeof(transaction));

    transaction.length = 8;
    transaction.flags = SPI_TRANS_USE_RXDATA;

    esp_err_t ret;
    ret=spi_device_polling_transmit(spi, &transaction);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.

    return transaction.rx_data[0];
}


void SPI_SendBuffer(spi_device_handle_t spi, uint8_t * buf, uint32_t buffer_length)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    // buffer_length is in bytes the transaction length needs to be in bits
    transaction.length = buffer_length*8;
    transaction.tx_buffer = buf;

    esp_err_t ret;
    ret = spi_device_polling_transmit(spi, &transaction);
    assert(ret == ESP_OK);

}

void SPI_GetBuffer(spi_device_handle_t spi, uint8_t * buf, uint32_t buffer_length)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    // buffer_length is in bytes the transaction length needs to be in bits
    transaction.length = buffer_length*8;

    esp_err_t ret;
    ret = spi_device_polling_transmit(spi, &transaction);
    assert(ret == ESP_OK);

    for (int i=0; i< buffer_length; i++) {
        buf[i] = transaction.rx_data[i];
    }

}