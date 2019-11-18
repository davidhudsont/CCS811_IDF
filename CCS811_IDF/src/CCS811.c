

#include "CCS811.h"


esp_err_t CCS811_Initialize(CCS811_STRUCT * ccs811)
{
    memset(ccs811, 0 , sizeof(CCS811_STRUCT));
    BSP_I2C_Setup();

    uint8_t reset_data[4] = {RESET_CODE_1, 
                            RESET_CODE_2, 
                            RESET_CODE_3, 
                            RESET_CODE_4};

    CCS811_multiWriteReg(REG_SW_RESET, reset_data, 4);

    if (CCS811_readReg(REG_HW_ID) != CCS811_HW_ID_VALUE)
        return ESP_FAIL;

    if ( (CCS811_readReg(REG_STATUS) & APP_VALID) != APP_VALID)
        return ESP_FAIL;

    CCS811_writeReg(REG_STATUS, 0x00);

    if ( (CCS811_readReg(REG_STATUS) & FW_MODE) != FW_MODE )
        return ESP_FAIL;

    return ESP_OK;

}

void CCS811_ReadAlgorithm_Results(CCS811_STRUCT * ccs811)
{
    uint8_t data[4];
    uint32_t len = 4;
    
    CCS811_multiReadReg(REG_ALG_DATA, data, len);

    ccs811->eCO2 = ((uint16_t)data[0] << 8) | data[1];
    ccs811->tVOC = ((uint16_t)data[2] << 8) | data[3];

}


void CCS811_Set_Drive_Mode(DRIVE_MODES modes, bool intr_data_rdy, bool int_thresh)
{
    uint8_t data = 0;
    switch (modes)
    {
    case IDLE:
        data |= MEAS_IDLE;
        break;
    case CONSTANT_POWER:
        data |= MEAS_1SECOND;
    
    default:
        data |= MEAS_IDLE;
        break;
    }

    data |= intr_data_rdy ? MEAS_INT_DATARDY : 0x00;
    data |= int_thresh ? MEAS_INT_THRESH : 0x00;

    CCS811_writeReg(REG_MEAS_MODE, data);

}


void CCS811_Print_Error()
{
    uint8_t error = CCS811_readReg(REG_ERROR_ID);

    if ( (error & WRITE_REG_INVALID) == WRITE_REG_INVALID)
        printf("WRITE_REG_INVALID\n");
    if ( (error & READ_REG_INVALID) == READ_REG_INVALID)
        printf("READ_REG_INVALID\n");
    if ( (error & MEASMODE_INVALID) == MEASMODE_INVALID)
        printf("MEASMODE_INVALID\n");
    if ( (error & MAX_RESISTANCE) == MAX_RESISTANCE)
        printf("MAX_RESISTANCE\n");
    if ( (error & HEATER_FAULT) == HEATER_FAULT)
        printf("HEATER_FAULT\n");
    if ( (error & HEATER_SUPPLY) == HEATER_SUPPLY)
        printf("HEATER_SUPPLY\n");

}


uint8_t CCS811_readReg(uint8_t address)
{
    return BSP_I2C_Read_Byte(CCS811_DEV_ADDR_B, address);
}


void CCS811_writeReg(uint8_t address, uint8_t data)
{
    BSP_I2C_Write_Byte(CCS811_DEV_ADDR_B, address, data);
}


void CCS811_multiWriteReg(uint8_t address, uint8_t * buf, uint32_t len)
{
    BSP_I2C_Write_Burst(CCS811_DEV_ADDR_B, address, buf, len);
}

void CCS811_multiReadReg(uint8_t address, uint8_t * buf, uint32_t len)
{
    BSP_I2C_Read_Burst(CCS811_DEV_ADDR_B, address, buf, len);
}