

#include "bspI2C.h"
#include "CCS811Registers.h"
#include <string.h>

#ifndef _CCS811_H_
#define _CCS811_H_


typedef enum drive_modes {
    IDLE            = 0,  // Sleep Mode         
    CONSTANT_POWER  = 1,  // Measurement every 1 second
    PULSE_HEATING   = 2,  // 10 second measurement
    LOW_POWER       = 3,  // 60 second measurement
    FAST_MODE       = 4   // 1/4 second measuremnt 
} DRIVE_MODES;


typedef struct ccs811_struct {
    float refResistance;
    float resistance;
    uint16_t tVOC;
    uint16_t eCO2;
    uint16_t vrefCounts;
    uint16_t ntcCounts;
    float temperature;

} CCS811_STRUCT;

esp_err_t CCS811_Initialize(CCS811_STRUCT * ccs811);

void CCS811_Set_Drive_Mode(DRIVE_MODES modes, bool intr_data_rdy, bool int_thresh);

void CCS811_ReadAlgorithm_Results(CCS811_STRUCT * ccs811);

void CCS811_Print_Error();

uint8_t CCS811_readReg(uint8_t address);
void CCS811_writeReg(uint8_t address, uint8_t data);

void CCS811_multiWriteReg(uint8_t address, uint8_t * buf, uint32_t len);
void CCS811_multiReadReg(uint8_t address, uint8_t * buf, uint32_t len);




#endif