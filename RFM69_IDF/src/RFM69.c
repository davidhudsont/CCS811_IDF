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