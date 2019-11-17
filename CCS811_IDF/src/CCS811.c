

#include "CCS811.h"



uint8_t CCS811_readReg(uint8_t address)
{
    return BSP_I2C_Read_Byte(CCS811_DEV_ADDR, address);
}


void CCS811_writeReg(uint8_t address, uint8_t data)
{
    BSP_I2C_Write_Byte(CCS811_DEV_ADDR, address, data);
}