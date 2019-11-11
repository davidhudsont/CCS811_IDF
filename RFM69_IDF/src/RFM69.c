/**
 * @file RFM69.c
 * @author David Hudson 
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * 
 */

#include "RFM69.h"
#include "driver/gpio.h"



/**
 * @brief read a register of a RFM69 module
 * 
 * @param spi - spi device handle
 * @param address - address of the register
 * @return uint8_t - value stored in the register
 */
uint8_t readReg(spi_device_handle_t spi, uint8_t address)
{
    uint8_t reg = SPI_readReg(spi , address & 0x7F);
    return reg;
}


/**
 * @brief write to a register of a RFM69 module
 * 
 * @param spi - spi device handle
 * @param address - address of the register
 * @param data - data to be written to the register
 */
void writeReg(spi_device_handle_t spi, uint8_t address, uint8_t data)
{
    SPI_writeReg(spi, address | 0x80, data);
}


static void IRAM_ATTR rfm_isr_handler(void* arg)
{

}


void rfm_isr_setup()
{
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;

    io_conf.pin_bit_mask = 1ULL << RFM_IRQ;

    io_conf.mode = GPIO_MODE_INPUT;

    io_conf.pull_up_en = 1;

    gpio_config(&io_conf);

    gpio_set_intr_type(RFM_IRQ, GPIO_PIN_INTR_POSEDGE);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFUALT);

    gpio_isr_handler_add(RFM_IRQ, rfm_isr_handler, NULL);
}