

#include "bspI2C.h"

#ifndef _CCS811_H_
#define _CCS811_H_

#define CCS811_DEV_ADDR (0x81)

#define REG_HW_ID (0x20)



uint8_t CCS811_readReg(uint8_t address);
void CCS811_writeReg(uint8_t address, uint8_t data);



#endif