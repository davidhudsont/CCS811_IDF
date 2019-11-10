/**
 * @file main.c
 * @author David Hudson
 * @brief 
 * @version 0.1
 * @date 2019-11-10
 * 
 * 
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "sdkconfig.h"
#include "bspSpi.h"
#include "RFM69.h"
#include <string.h>


#define BLINK_GPIO (5)

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

void rfm_task(void *pvParameter)
{
    spi_device_handle_t spi;
    memset(&spi, 0, sizeof(spi));
    int clock_speed = 4*1000*1000; // 4MHz

    esp_err_t err = false;
    err = BSP_SPI_Init(&spi, clock_speed);
    if (err != ESP_OK)
    {
        printf("Error\n");
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    uint8_t reg = readReg(spi, REG_OPMODE);
    printf("Reg Op Mode: %x\n", (unsigned int)reg);

    while (1)
    {
        printf("READ RFM VERSION\n");
        uint32_t data = readReg(spi, REG_VERSION);
        printf("RFM69 REGISTER: %x\n", (unsigned int)data); //Should be 0x24
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        fflush(stdout);

    }
}

void app_main()
{
    //printf("Starting Tasks!\n");

    //xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    //xTaskCreate(&rfm_task, "rfm_task", configMINIMAL_STACK_SIZE*3, NULL, 5, NULL);

    spi_device_handle_t spi;
    memset(&spi, 0, sizeof(spi));    
    int clock_speed = 10*1000*1000; // Clock speed 10MHz

    esp_err_t err = false;
    err = BSP_SPI_Init(&spi, clock_speed);
    if (err != ESP_OK)
    {
        printf("Spi Configuration Error\n");
    }


    for (int i=0; i<=0xFF; i++) 
    {
        writeReg(spi, REG_DIOMAPPING1, i);
        uint8_t reg = readReg(spi, REG_DIOMAPPING1);
        printf("Reg DIO : 0x%x\n", (unsigned int)reg);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1)
    {
        printf("READ RFM VERSION\n");
        uint32_t data = readReg(spi, REG_VERSION);
        printf("RFM69 REGISTER: 0x%x\n", (unsigned int)data); //Should be 0x24
        uint8_t reg = readReg(spi, REG_DIOMAPPING1);
        printf("Reg DIO : 0x%x\n", (unsigned int)reg);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        fflush(stdout);

    }
}
