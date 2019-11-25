

#ifndef _CCS811_REGISTERS_H_
#define _CCS811_REGISTERS_H_

#define CCS811_DEV_ADDR_A   (0x5A)
#define CCS811_DEV_ADDR_B   (0x5B)

#define REG_STATUS              (0x00) // 1 bytes (R) Status register
#define REG_MEAS_MODE           (0x01) // 1 bytes (R/W) Measurement mode and conditions register
#define REG_ALG_DATA            (0x02) // 8 bytes (R) Algorithm result
#define REG_RAW_DATA            (0x03) // 2 bytes (R) Raw ADC values
#define REG_ENV_DATA            (0x05) // 4 bytes (W) temperatue and humidty 
#define REG_NTC                 (0x06) // 4 bytes (W) This register might not exist
#define REG_THRESHOLDS          (0x10) // 4 bytes (R/W) eC02 thresholds gen interrupt 
#define REG_BASELINE            (0x11) // 2 bytes (R/W) current baseline value 
#define REG_HW_ID               (0x20) // 1 bytes (R) Hardware ID value (0x81)
#define REG_HW_VERSION          (0x21) // 1 bytes (R) Hardware Version value (0x1X)
#define REG_FW_BOOT_VERSION     (0x23) // 2 bytes (R) Firmware Boot Version
#define REG_FW_APP_VERSION      (0x23) // 2 bytes (R) Firmware Application Version
#define REG_INTERNAL_STATE      (0xA0) // 1 bytes (R) Internal Status register
#define REG_ERROR_ID            (0xE0) // 1 bytes (R) Error ID
#define REG_APP_START           (0xF4) // APP_START write 0 to it
#define REG_SW_RESET            (0xFF) // 4 bytes (W) Software Reset

#define CCS811_HW_ID_VALUE  (0x81)

#define FW_MODE         (0x80)
#define APP_ERASE       (0x40)
#define APP_VERIFY      (0x20)
#define APP_VALID       (0x10)
#define DATA_READY      (0x08)
#define ERROR           (0x01)

#define MEAS_IDLE       (0x00)
#define MEAS_1SECOND    (0x10)
#define MEAS_10SECOND   (0x20)
#define MEAS_60SECOND   (0x30)
#define MEAS_4HZ        (0x40)

#define MEAS_INT_DATARDY (0x08)
#define MEAS_INT_THRESH  (0x04)


#define RESET_CODE_1    (0x11)
#define RESET_CODE_2    (0xE5)
#define RESET_CODE_3    (0x72)
#define RESET_CODE_4    (0x8A)

// Error Codes
#define WRITE_REG_INVALID   (0x01)
#define READ_REG_INVALID    (0x02)
#define MEASMODE_INVALID    (0x04)
#define MAX_RESISTANCE      (0x08)
#define HEATER_FAULT        (0x10)
#define HEATER_SUPPLY       (0x20)


#define Rref (10000) // 10kOhm Resistor
#define RNTC_25C (10000) // Thermistor resistance at 25C
#define BCONSTANT (3977) // BConstant for a 10k Thermistor
#define RNTC_TEMP (25) // Room Temperature for Thermistor 25C

#endif