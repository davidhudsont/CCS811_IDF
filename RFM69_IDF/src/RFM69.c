/**
 * @file RFM69.c
 * @author David Hudson 
 * @brief 
 * @version 0.2
 * @date 2019-11-15
 * 
 * 
 */

#include "RFM69.h"
#include "driver/gpio.h"



/**
 * @brief read a register of a RFM69 module
 * 
 * @param spi - spi device handle
 * @param address - address of the register
 * @return uint8_t - value stored in the register
 */
uint8_t readReg(spi_device_handle_t spi, uint8_t address)
{
    uint8_t reg = SPI_readReg(spi , address & 0x7F);
    return reg;
}


/**
 * @brief write to a register of a RFM69 module
 * 
 * @param spi - spi device handle
 * @param address - address of the register
 * @param data - data to be written to the register
 */
void writeReg(spi_device_handle_t spi, uint8_t address, uint8_t data)
{
    SPI_writeReg(spi, address | 0x80, data);
}


/**
 * @brief Sets the operation mode of the RFM69 module
 * 
 * @param spi - SPI device handle
 * @param mode - RFM_MODES enum
 * @param sequencerOff - Turn the sequence off (true)
 * @param listenOn - Turn the listener on (true)
 */
void RFM_Set_Mode(spi_device_handle_t spi, RFM_MODES mode, bool sequencerOff, bool listenOn)
{
    uint8_t RegMode = 0;
    RegMode |= sequencerOff ? RF_OPMODE_SEQUENCER_OFF : RF_OPMODE_SEQUENCER_ON;
    RegMode |= listenOn ? RF_OPMODE_LISTEN_ON : RF_OPMODE_LISTEN_OFF;

    switch (mode)
    {
    case SLEEP:
        RegMode |= RF_OPMODE_SLEEP;
        break;
    case STANDBY:
        RegMode |= RF_OPMODE_STANDBY;
        break;
    
    default:
        break;
    }
}

/**
 * @brief 
 * 
 * @param spi - SPI device handle
 * @param freq - FREQUENCY ENUM choose from list of frequncies (usa)
 */
void RFM_Set_Carrier_Frequency(spi_device_handle_t spi, RFM_CARRIER_FREQ freq)
{

    switch (freq)
    {
    case RFM_913_MHZ:
        /* code */
        break;
    case RFM_914_MHZ:
        /* code */
        break;
    case RFM_915_MHZ:
        writeReg(spi, REG_FRFMSB, RF_FRFMSB_915);
        writeReg(spi, REG_FRFMID, RF_FRFMID_915);
        writeReg(spi, REG_FRFLSB, RF_FRFLSB_915);
        break;
    case RFM_916_MHZ:
        /* code */
        break;
    case RFM_917_MHZ:
        /* code */
        break;
    case RFM_918_MHZ:
        /* code */
        break;
    
    default:
        writeReg(spi, REG_FRFMSB, RF_FRFMSB_915);
        writeReg(spi, REG_FRFMID, RF_FRFMID_915);
        writeReg(spi, REG_FRFLSB, RF_FRFLSB_915);
        break;
    }
}

uint32_t RFM_Get__Carrier_Frequency(spi_device_handle_t spi)
{
    uint32_t freq = 0;
    freq |= readReg(spi, REG_FRFMSB) << 16;
    freq |= readReg(spi, REG_FRFMID) << 8;
    freq |= readReg(spi, REG_FRFLSB);
    freq = (uint32_t)( (float)freq * RF_FSTEP );
    return freq;
}

/**
 * @brief 
 * 
 * @param spi - SPI device handle
 * @param bitrate - RFM_BITRATE enum choose between a list of bitrates
 */
void RFM_Set_Bitrate(spi_device_handle_t spi, RFM_BITRATE bitrate)
{
    switch (bitrate)
    {
    case RFM_4_8_KBPS:
        writeReg(spi, REG_BITRATEMSB, RF_BITRATEMSB_4800);
        writeReg(spi, REG_BITRATELSB, RF_BITRATELSB_4800);
        break;
    
    default:
        writeReg(spi, REG_BITRATEMSB, RF_BITRATEMSB_4800);
        writeReg(spi, REG_BITRATELSB, RF_BITRATELSB_4800);
        break;
    }

}


/**
 * @brief 
 * 
 * @param spi - SPI device handle
 * @return uint32_t - Bitrate in bit/s 
 */
uint32_t RFM_Get_Bitrate(spi_device_handle_t spi)
{
    uint16_t bitrate = 0x00;

    bitrate |= readReg(spi, REG_BITRATEMSB) << 8; 
    bitrate |= readReg(spi, REG_BITRATELSB); 

    uint32_t bps = FXOSC / bitrate;
    return bps;
}


static void IRAM_ATTR rfm_isr_handler(void* arg)
{

}


void rfm_isr_setup()
{
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;

    io_conf.pin_bit_mask = 1ULL << RFM_IRQ;

    io_conf.mode = GPIO_MODE_INPUT;

    io_conf.pull_up_en = 1;

    gpio_config(&io_conf);

    gpio_set_intr_type(RFM_IRQ, GPIO_PIN_INTR_POSEDGE);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFUALT);

    gpio_isr_handler_add(RFM_IRQ, rfm_isr_handler, NULL);
}