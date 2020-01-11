/**
 * @file CCS811.h
 * @author David Hudson
 * @brief CCS811 Driver Header file
 * @date 2020-01-11
 * 
 * 
 */

#include "bspI2C.h"
#include "CCS811Registers.h"
#include <string.h>

#ifndef _CCS811_H_
#define _CCS811_H_


QueueHandle_t queue;

#define CCS811_nWAKE_PIN_NUM (10)

#define CCS811_INTR_PIN_NUM (4) // Interrupt pin connected to CCS811 Intr pin
#define CCS811_INTR_FLAGS_DEFAULT (0) // Interrupt Flags

#define CCS811_INTR_PIN_NUM2 (5) // Interrupt pin connectec to CCS811 Threshold intr pin

/**
 * @brief CCS811 Measurement modes enumeration
 * 
 */
typedef enum drive_modes {
    IDLE            = 0,  // Sleep Mode         
    CONSTANT_POWER  = 1,  // Measurement every 1 second
    PULSE_HEATING   = 2,  // 10 second measurement
    LOW_POWER       = 3,  // 60 second measurement
    FAST_MODE       = 4   // 1/4 second measuremnt 
} DRIVE_MODES;




/**
 * @brief CCS811 device structure
 * 
 */
typedef struct ccs811_struct {
    uint16_t tVOC;              // Current tVOC reading
    uint16_t eCO2;              // Current eC02 reading
    DRIVE_MODES modes;          // Current measurement mode
    bool app_start;             // Application has been started
    bool data_ready_intr_mode;  // Data ready interrupt mode is set
    bool threshold_intr_mode;   // Threshold interrupt mdoe is set
    uint16_t baseline;          // Baseline value
    uint16_t low_threshold;     // Low threshold value
    uint16_t high_threshold;    // High threshold value
    uint16_t hysteresis;        // Hysteresis value
#if NTC_REG_EXISTS
    float temperature;
#endif
    uint32_t counter;

} CCS811_STRUCT;


esp_err_t CCS811_Initialize(CCS811_STRUCT * ccs811);

void CCS811_Print_Struct(CCS811_STRUCT * ccs811);

void CCS811_SW_Reset();

void CCS811_Set_Drive_Mode(CCS811_STRUCT * ccs811, DRIVE_MODES modes, bool intr_data_rdy, bool int_thresh);

bool CCS811_Data_Available(void);

void CCS811_ReadAlgorithm_Results(CCS811_STRUCT * ccs811);
uint16_t CCS811_Get_CO2(CCS811_STRUCT * ccs811);
uint16_t CCS811_Get_TVOC(CCS811_STRUCT * ccs811);

void CCS811_ISR_Init(CCS811_STRUCT * ccs811);

void CCS811_Read_NTC(CCS811_STRUCT * ccs811);
float CCS811_Get_Temperature(CCS811_STRUCT * ccs811);

void CCS811_Write_Env(float temperature, float humidity);
void CCS811_Write_Threshold(CCS811_STRUCT * ccs811, uint16_t low_to_med, uint16_t med_to_high);

void CCS811_Read_Baseline(CCS811_STRUCT * ccs811);
void CCS811_Write_Baseline(CCS811_STRUCT * ccs811, uint16_t baseline);

void CCS811_Print_Error();

// CCS811 register functions
uint8_t CCS811_readReg(uint8_t address);
void CCS811_writeReg(uint8_t address, uint8_t data);

void CCS811_multiWriteReg(uint8_t address, uint8_t * buf, uint32_t len);
void CCS811_multiReadReg(uint8_t address, uint8_t * buf, uint32_t len);




#endif