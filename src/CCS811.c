/**
 * @file CCS811.c
 * @author David Hudson (you@domain.com)
 * @brief CCS811 driver module
 * @date 2020-01-11
 * 
 * 
 */

#include "CCS811.h"
#include <driver/gpio.h>
#include "freertos/queue.h"
#include "freertos/event_groups.h"
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

    ccs811->low_threshold = LOW_THRESHOLD;
    ccs811->high_threshold = HIGH_THRESHOLD;
    ccs811->hysteresis = HYSTERESIS;
    // Start the I2C Bus
    BSP_I2C_Setup();

    // If the device is running skip initialization
    if ( (CCS811_readReg(REG_STATUS) & FW_MODE) == FW_MODE )
        return ESP_OK;

    // Software Reset
    CCS811_SW_Reset();

    // Wait until the device finishes reseting
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
    ccs811->app_start = true;
    return ESP_OK;

}


/**
 * @brief Print out the data structure
 * 
 * @param ccs811 
 */
void CCS811_Print_Struct(CCS811_STRUCT * ccs811)
{
    printf("Application Start: %d\n", ccs811->app_start);
    printf("Measurement Mode: %d\n", ccs811->modes);
    printf("Data Intr: %d, Thesh Intr: %d\n", ccs811->data_ready_intr_mode, ccs811->threshold_intr_mode);
    printf("Threshold High: %d\n", ccs811->high_threshold);
    printf("Threshold Low: %d\n", ccs811->low_threshold);
    printf("Hysteresis: %d\n", ccs811->hysteresis);
    printf("Baseline: %d\n", ccs811->baseline);
    printf("eC02: %d, tVOC: %d\n", ccs811->eCO2, ccs811->tVOC);
    printf("Counter: %d\n", ccs811->counter);

}


/**
 * @brief Reset the CCS811 into boot mode
 * 
 */
void CCS811_SW_Reset()
{
    // Reset Code
    uint8_t reset_data[4] = {RESET_CODE_1, 
                            RESET_CODE_2, 
                            RESET_CODE_3, 
                            RESET_CODE_4};

    // Reset the device to a known state
    CCS811_multiWriteReg(REG_SW_RESET, reset_data, 4);

}


/**
 * @brief Check if new data is available
 * 
 * @return bool
 */
bool CCS811_Data_Available(void)
{
    uint8_t value = CCS811_readReg(REG_STATUS);

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
    
    // Read the eC02 and tVOC registers
    CCS811_multiReadReg(REG_ALG_DATA, data, 4);

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
 * @todo should split up the drive mode into 3 functions
 *       1 for measurment and the other 2 for interrupt modes
 * @param modes - Different drive modes
 * @param intr_data_rdy - Set interrupt when data is ready (true)
 * @param int_thresh - Set interrupt when threshold is crossed (true)
 */
void CCS811_Set_Drive_Mode(CCS811_STRUCT * ccs811, DRIVE_MODES modes, bool intr_data_rdy, bool int_thresh)
{
    uint8_t data = 0;
    ccs811->modes = modes;
    ccs811->data_ready_intr_mode = intr_data_rdy;
    ccs811->threshold_intr_mode = int_thresh;

    // TODO should read the current configuration

    switch (modes)
    {
    case IDLE:
        data |= MEAS_IDLE;
        break;
    case CONSTANT_POWER:
        data |= MEAS_1SECOND;
        break;
    case PULSE_HEATING:
        data |= MEAS_10SECOND;
        break;
    case LOW_POWER:
        data |= MEAS_60SECOND;
        break;
    case FAST_MODE:
        data |= MEAS_4HZ;
        break;
    default:
        data |= MEAS_IDLE;
        break;
    }

    // TODO this is not the correct way to overite the data.
    data |= intr_data_rdy ? MEAS_INT_DATARDY : 0x00;
    data |= int_thresh ? MEAS_INT_THRESH : 0x00;

    CCS811_writeReg(REG_MEAS_MODE, data);

}


/**
 * @brief Interrupt handler for data ready pin
 * 
 * @param arg 
 */
static void IRAM_ATTR data_ready_isr_handler(void * arg)
{
    char msg = 'r'; // Send data ready character
    BaseType_t base;

    CCS811_STRUCT * ccs811_dev =  arg;
    ccs811_dev->counter += 1;
    
    xQueueSendFromISR(queue, (void *)&msg, &base);
    
}


/**
 * @brief If using data ready interrupt pin
 * 
 * @param ccs811 - ccs811 device struct
 */
void CCS811_ISR_Init(CCS811_STRUCT * ccs811)
{
    gpio_config_t io_conf;

    // Create a queue to communicate data is ready
    queue = xQueueCreate(3, 1);

    // Interrupt on the negative edge
    // Data ready pin is pulled down when data is ready
    io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;

    // Set CCS811_INTR_PIN_NUM to 1 in the bit mask
    io_conf.pin_bit_mask = 1ULL << CCS811_INTR_PIN_NUM;

    // Set to input mode
    io_conf.mode = GPIO_MODE_INPUT;

    // enable pull-down
    io_conf.pull_up_en = false;
    io_conf.pull_down_en = true;

    gpio_config(&io_conf);

    gpio_set_intr_type(CCS811_INTR_PIN_NUM, GPIO_PIN_INTR_NEGEDGE);

    gpio_install_isr_service(CCS811_INTR_FLAGS_DEFAULT);

    gpio_isr_handler_add(CCS811_INTR_PIN_NUM, data_ready_isr_handler, (void*) ccs811);

}


#if NTC_REG_EXISTS
    /**
     * @brief Calculate the temperature from the thermistor
     *        and store it. 
     * 
     * The NTC register is no longer documented in the CCS811 data sheet
     * so this function most likely doesn't do anything.
     * See https://github.com/sparkfun/SparkFun_CCS811_Arduino_Library/issues/18
     * 
     *  Equation: 1. B = log(R/Ro)/(1/T - 1/To)
     *            2. (1/T - 1/To) = log(R/Ro)/B
     *            3. 1/T = log(R/Ro)/B + 1/To
     *            4. T = 1/(log(R/Ro)/B + 1/To)
     *  
     * @param ccs811 - CCS811 device struct
     */
    void CCS811_Read_NTC(CCS811_STRUCT * ccs811)
    {
        uint8_t data[4];
        CCS811_multiReadReg(REG_NTC, data, 4);
        CCS811_Print_Error();

        uint16_t vRef = (uint16_t) ((uint16_t) data[0] << 8 | data[1]);
        uint16_t vNTC = (uint16_t) ((uint16_t) data[2] << 8 | data[3]);

        for (int i =0; i<4; i++)
        {
            printf("DATA[%d] = 0x%x\n", i, (unsigned int) data[i]);
        }

        printf("vRef =%d\n",vRef);
        printf("vNTC =%d\n",vNTC);

        uint32_t Rntc = vNTC * Rref / vRef;
        printf("vRntc =%d\n", Rntc);


        float ntc_temp;
        ntc_temp = logf( Rntc / RNTC_25C);
        ntc_temp /= BCONSTANT;
        ntc_temp += 1.0 / (RNTC_TEMP + 273.15f);
        ntc_temp = 1.0 / ntc_temp;
        ntc_temp -= 273.15;

        ccs811->temperature = ntc_temp;

    }


    /**
     * @brief - Probably doesn't work.
     * 
     * @param ccs8111 
     * @return float 
     */
    float CCS811_Get_Temperature(CCS811_STRUCT * ccs811)
    {
        return ccs811->temperature;
    }
#endif


/**
 * @brief Write environmental data to the CCS811
 * Credit to sparkfun library for the corrected function of 
 * writing environmental data see https://github.com/sparkfun/Qwiic_BME280_CCS811_Combo
 * @param temperature - In degrees Celsius (-25 to 50)
 * @param relativeHumidity - In percent humidity (0 to 100%)
 */
void CCS811_Write_Env(float temperature, float relativeHumidity) 
{
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
 * @brief Stores low and high threshold values
 * 
 * @param low_to_med 
 * @param med_to_high 
 */
void CCS811_Write_Threshold(CCS811_STRUCT * ccs811, uint16_t low_to_med, uint16_t med_to_high)
{
    uint8_t buf[4];
    ccs811->low_threshold = low_to_med;
    ccs811->high_threshold = med_to_high;

    buf[0] = low_to_med >> 8;
    buf[1] = low_to_med & 0x00FF;
    buf[2] = med_to_high >> 8;
    buf[3] = med_to_high & 0x00FF;

    CCS811_multiWriteReg(REG_THRESHOLDS, buf, 4);

}


/**
 * @brief Read the baseline register store the baseline
 * 
 * @param ccs811 
 */
void CCS811_Read_Baseline(CCS811_STRUCT * ccs811)
{
    uint8_t buf[2];
    CCS811_multiReadReg(REG_BASELINE, buf, 2);
    ccs811->baseline = (((uint16_t) buf[0] << 8) | buf[1]);

}


/**
 * @brief Write to the base line register the baseline value
 * 
 * @param baseline 
 */
void CCS811_Write_Baseline(CCS811_STRUCT * ccs811, uint16_t baseline)
{
    uint8_t buf[2];
    ccs811->baseline = baseline;
    buf[0] = baseline >> 8;
    buf[1] = baseline & 0x00FF;

    CCS811_multiWriteReg(REG_BASELINE, buf, 2);

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