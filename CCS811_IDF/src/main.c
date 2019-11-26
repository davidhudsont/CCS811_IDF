/**
 * @file main.c
 * @author David Hudson
 * @brief 
 * @version 0.1
 * @date 2019-11-16
 * 
 * 
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "sdkconfig.h"
#include <string.h>
#include "CCS811.h"

#define DEBUG_MAIN (0)


#define BLINK_GPIO (26)

// time in milliseconds
void delay(int time)
{
    vTaskDelay( time / portTICK_PERIOD_MS);
}

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    printf("Blink Task Start!\n");

    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Blink Off!\n");
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Blink On!\n");
        fflush(stdout);
    }
}


void ccs811_task(void *pvParameter)
{
    CCS811_STRUCT ccs811_device;

    printf("Initialize Device\n");
    esp_err_t err = CCS811_Initialize(&ccs811_device);
    if ( err == ESP_FAIL)
    {
        printf("Initialization Fail\n");
        CCS811_Print_Error();
    }

    if (err == ESP_OK)
    {
        printf("Set the Drive Mode\n");
        CCS811_Set_Drive_Mode(CONSTANT_POWER, false, false);
    }
    else
    {
        CCS811_Set_Drive_Mode(IDLE, false, false);
    }

    while (1)
    {
        if (CCS811_Data_Available())
        {
            CCS811_ReadAlgorithm_Results(&ccs811_device);
            uint16_t eCO2 = CCS811_Get_CO2(&ccs811_device);
            printf("eC02 = %d ppm\n",eCO2);
            uint16_t tVOC = CCS811_Get_TVOC(&ccs811_device);
            printf("tVOC = %d \n",tVOC);
        }
        delay(1000);
    }
}


void app_main()
{
    printf("Starting Tasks!\n");
#if !DEBUG_MAIN
    //xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&ccs811_task, "ccs811_task", configMINIMAL_STACK_SIZE*4, NULL, 5, NULL);
#else 
    CCS811_STRUCT ccs811_device;
    memset(&ccs811_device, 0, sizeof(CCS811_STRUCT));

    BSP_I2C_Setup();

    CCS811_writeReg(REG_APP_START, 0x00);
    delay(2000);
    while ( (CCS811_readReg(REG_STATUS) & FW_MODE) != FW_MODE)
    {
        uint8_t buf[1];
        printf("Put CCS811 into APP Mode\n");
        // To start the application we need to do the following
        // start - DEV_ADDR | WR - REG_APP_START - STOP
        CCS811_multiWriteReg(REG_APP_START, buf, 0); 
        //CCS811_writeReg(REG_APP_START, 0x00);
        delay(1000);

    }

    if ( (CCS811_readReg(REG_STATUS) & FW_MODE) == FW_MODE)
    {
        while ( (CCS811_readReg(REG_MEAS_MODE) & MEAS_1SECOND) != MEAS_1SECOND)
        {
            printf("Put CCS811 into Constant Power mode\n");
            CCS811_writeReg(REG_MEAS_MODE, MEAS_1SECOND);
            delay(1000);
        }
    }

    printf("MEAS MODE: 0x%x\n", (unsigned int) CCS811_readReg(REG_MEAS_MODE));

    while (1)
    {

        if ( (CCS811_readReg(REG_STATUS) & DATA_READY) == DATA_READY)
        {
            CCS811_ReadAlgorithm_Results(&ccs811_device);
            printf("eC02: %d\n", ccs811_device.eCO2);
        }
        delay(1000);
    }

#endif
    
}
