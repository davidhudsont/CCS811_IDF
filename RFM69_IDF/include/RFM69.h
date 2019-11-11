/**
 * @file RFM69.h
 * @author David Hudson 
 * @brief Driver module for a RFM69 Device
 * @version 0.2
 * @date 2019-11-10
 * 
 * 
 */

#include "driver/spi_master.h"
#include "soc/spi_pins.h"
#include "bspSpi.h"
#include "RFM69registers.h"

#ifndef _RMF69_H_
#define _RFM69_H_

#define RFM_IRQ (4)
#define ESP_INTR_FLAG_DEFUALT 0

typedef struct {
    spi_device_handle_t *spi;

} RFM_69_STRUCT;

void initialize_rfm(spi_device_handle_t spi, uint8_t freqBand, uint8_t ID, uint8_t networkID);


uint8_t readReg(spi_device_handle_t spi, uint8_t address);
void writeReg(spi_device_handle_t spi, uint8_t address, uint8_t data);


void rfm_isr_setup();


#endif