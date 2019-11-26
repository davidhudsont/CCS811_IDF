

#include "CCS811.h"
#include <math.h>

/**
 * @brief Initialize the CCS811 Device
 * 
 * @param ccs811 - CCS811_STRUCT to store device information
 * @return esp_err_t - ESP Error
 */
esp_err_t CCS811_Initialize(CCS811_STRUCT * ccs811)
{
    // Clear CCS811_STRUCT
    memset(ccs811, 0 , sizeof(CCS811_STRUCT));
    
    // Start the I2C Bus
    BSP_I2C_Setup();

    // If the device is running skip initialization
    printf("Check if device is initialized\n");
    if ( (CCS811_readReg(REG_STATUS) & FW_MODE) == FW_MODE )
        return ESP_OK;


    // Software Reset
    CCS811_SW_Reset();

    volatile uint8_t temp = 0;
    for (uint32_t i=0; i<5600000; i++) 
    {
        temp++;
    }

    // Check to see if the device is connected
    if (CCS811_readReg(REG_HW_ID) != CCS811_HW_ID_VALUE)
        return ESP_FAIL;

    // Check if the application is valid
    if ( (CCS811_readReg(REG_STATUS) & APP_VALID) != APP_VALID)
        return ESP_FAIL;

    // Start the device
    uint8_t buf[1];
    CCS811_multiWriteReg(REG_APP_START, buf, 0); 


    // Check if the device has started
    if ((CCS811_readReg(REG_STATUS) & FW_MODE) != FW_MODE  )
        return ESP_FAIL;

    // Success!
    return ESP_OK;

}


void CCS811_SW_Reset()
{
    // Reset Code
    uint8_t reset_data[4] = {RESET_CODE_1, 
                            RESET_CODE_2, 
                            RESET_CODE_3, 
                            RESET_CODE_4};

    // Reset the device to a known state
    printf("Reset the device\n");
    CCS811_multiWriteReg(REG_SW_RESET, reset_data, 4);

}



bool CCS811_Data_Available(void)
{
    uint8_t value;
    value = CCS811_readReg(REG_STATUS);

    if ( (value & DATA_READY) ==  DATA_READY)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * @brief Read the CCS811 algorith results into
 *        CCS811 struct
 * 
 * @param ccs811 - CCS811_STRUCT to store device information 
 */
void CCS811_ReadAlgorithm_Results(CCS811_STRUCT * ccs811)
{
    uint8_t data[4];
    uint32_t len = 4;
    
    // Read the eC02 and tVOC registers
    CCS811_multiReadReg(REG_ALG_DATA, data, len);

    // Store into CCS811 struct
    ccs811->eCO2 = ((uint16_t)data[0] << 8) | data[1];
    ccs811->tVOC = ((uint16_t)data[2] << 8) | data[3];

}


/**
 * @brief 
 * 
 * @param ccs811 - CCS811 struct to store information
 * @return uint16_t - The current eCO2 measurement 
 */
uint16_t CCS811_Get_CO2(CCS811_STRUCT * ccs811)
{
    return ccs811->eCO2;
}


/**
 * @brief 
 * 
 * @param ccs811 - CCS811 struct to store information
 * @return uint16_t - The current tVOC measurement
 */
uint16_t CCS811_Get_TVOC(CCS811_STRUCT * ccs811)
{
    return ccs811->tVOC;
}


/**
 * @brief Set the drive mode of the CCS811 device
 * 
 * @param modes - Different drive modes
 * @param intr_data_rdy - Set interrupt when data is ready (true)
 * @param int_thresh - Set interrupt when threshold is crossed (true)
 */
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

    printf("Meas mode : 0x%x\n", (unsigned int)data);
    CCS811_writeReg(REG_MEAS_MODE, data);

}


/**
 * @brief Calculate the temperature from the thermistor
 *        and store it.
 * 
 *  Equation: B = log(R/Ro)/(1/T - 1/To)
 *            (1/T - 1/To) = log(R/Ro)/B
 *            1/T = log(R/Ro)/B + 1/To
 *            T = 1/(log(R/Ro)/B + 1/To)
 * 
 * @param ccs811 
 */
void CCS811_Read_NTC(CCS811_STRUCT * ccs811)
{
    uint8_t data[4];
    CCS811_multiReadReg(REG_NTC, data, 4);

    uint16_t vRef = (uint16_t) (data[0] << 8 | data[1]);
    uint16_t vNTC = (uint16_t) (data[2] << 8 | data[3]);

    uint32_t Rntc = vNTC * Rref / vRef;

    float ntc_temp;
    ntc_temp = logf( Rntc / RNTC_25C);
    ntc_temp /= BCONSTANT;
    ntc_temp += 1.0 / (RNTC_TEMP + 273.15f);
    ntc_temp = 1.0 / ntc_temp;
    ntc_temp -= 273.15;

    ccs811->temperature = ntc_temp;

}

void CCS811_Write_Env(float temperature, float relativeHumidity) {
    	//Check for invalid temperatures
	if((temperature < -25)||(temperature > 50)) return;
	
	//Check for invalid humidity
	if((relativeHumidity < 0)||(relativeHumidity > 100)) return;
	
	uint32_t rH = relativeHumidity * 1000; //42.348 becomes 42348
	uint32_t temp = temperature * 1000; //23.2 becomes 23200

	uint8_t envData[4];

	//Correct rounding. See issue 8: https://github.com/sparkfun/Qwiic_BME280_CCS811_Combo/issues/8
	envData[0] = (rH + 250) / 500;
	envData[1] = 0; //CCS811 only supports increments of 0.5 so bits 7-0 will always be zero

	temp += 25000; //Add the 25C offset
	
	//Correct rounding
	envData[2] = (temp + 250) / 500;
	envData[3] = 0;

    CCS811_multiWriteReg(REG_ENV_DATA, envData, 4);
}


/**
 * @brief Print any errors that occured.
 * 
 */
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

/**
 * @brief Read a CCS811 register
 * 
 * @param address - register address
 * @return uint8_t - data stored in the register
 */
uint8_t CCS811_readReg(uint8_t address)
{
    return BSP_I2C_Read_Byte(CCS811_DEV_ADDR_B, address);
}


/**
 * @brief Write to a CCS811 register
 * 
 * @param address - register address
 * @param data - data to write
 */
void CCS811_writeReg(uint8_t address, uint8_t data)
{
    BSP_I2C_Write_Byte(CCS811_DEV_ADDR_B, address, data);
}


/**
 * @brief Read multiple registers
 * 
 * @param address - address to start at
 * @param buf - buffer to store elements into
 * @param len - number of elements
 */
void CCS811_multiReadReg(uint8_t address, uint8_t * buf, uint32_t len)
{
    BSP_I2C_Read_Burst(CCS811_DEV_ADDR_B, address, buf, len);
}


/**
 * @brief Write to multiple registers
 * 
 * @param address - address to start at
 * @param buf - elements to write
 * @param len - number of elements
 */
void CCS811_multiWriteReg(uint8_t address, uint8_t * buf, uint32_t len)
{
    BSP_I2C_Write_Burst(CCS811_DEV_ADDR_B, address, buf, len);
}