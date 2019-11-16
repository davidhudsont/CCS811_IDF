/**
 * @file RFM69.h
 * @author David Hudson 
 * @brief Driver module for a RFM69 Device
 * @version 0.3
 * @date 2019-11-15
 * 
 * 
 */

#include "driver/spi_master.h"
#include "soc/spi_pins.h"
#include "bspSpi.h"
#include "RFM69registers.h"

#ifndef _RMF69_H_
#define _RFM69_H_

#define FXOSC (32*1000*1000) // 32 MHz
#define RF_FSTEP (61.03515625) // == FXOSC / 2^19 = 32MHz / 2^19 (p13 in datasheet)

#define RFM_IRQ (4) // IRQ Pin number
#define ESP_INTR_FLAG_DEFUALT (0) // INTERRUPT FLAG

typedef enum rfm_carrier_freq {
    RFM_913_MHZ = 0,
    RFM_914_MHZ = 1,
    RFM_915_MHZ = 3,
    RFM_916_MHZ = 4,
    RFM_917_MHZ = 5,
    RFM_918_MHZ = 6

} RFM_CARRIER_FREQ;

typedef enum rfm_bitrate {
    RFM_4_8_KBPS = 0
} RFM_BITRATE;


typedef enum rfm_modes {
    SLEEP   = 0,
    STANDBY = 1,
    SYNTH   = 2,
    TX      = 3,
    RX      = 4
} RFM_MODES;


typedef struct {
    spi_device_handle_t *spi;

} RFM_69_STRUCT;

void initialize_rfm(spi_device_handle_t spi, uint8_t freqBand, uint8_t ID, uint8_t networkID);

void RFM_Set_Mode(spi_device_handle_t spi, RFM_MODES mode, bool sequencerOff, bool listenOn);

void RFM_Set_Carrier_Frequency(spi_device_handle_t spi, RFM_CARRIER_FREQ freq);
uint32_t RFM_Get__Carrier_Frequency(spi_device_handle_t spi);

void RFM_Set_Bitrate(spi_device_handle_t spi, RFM_BITRATE bitrate);
uint32_t RFM_Get_Bitrate(spi_device_handle_t);

uint8_t readReg(spi_device_handle_t spi, uint8_t address);
void writeReg(spi_device_handle_t spi, uint8_t address, uint8_t data);

void rfm_isr_setup();


#endif