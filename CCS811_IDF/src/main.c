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

QueueHandle_t queue;

#define BLINK_GPIO (5)

/**
 * @brief Delay a task for a specified amount of time
 * 
 * @param time - time in milliseconds
 */
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
            //CCS811_Read_NTC(&ccs811_device);
            uint16_t eCO2 = CCS811_Get_CO2(&ccs811_device);
            printf("eC02 = %d ppm\n",eCO2);
            uint16_t tVOC = CCS811_Get_TVOC(&ccs811_device);
            printf("tVOC = %d\n",tVOC);
        }
        delay(1000);
    }
}


void ccs811_task_intr(void *pvParameter)
{
    CCS811_STRUCT ccs811_device;
    uint16_t eCO2 = 201;
    uint16_t tVOC = 909;
    char msg;

    printf("Initialize Device\n");
    esp_err_t err = CCS811_Initialize(&ccs811_device);
    if ( err == ESP_FAIL)
    {
        printf("Initialization Fail\n");
        CCS811_Print_Error();
    }
    CCS811_ISR_Init(&ccs811_device);

    if (err == ESP_OK)
    {
        printf("Set the Drive Mode\n");
        CCS811_Set_Drive_Mode(CONSTANT_POWER, true, false);
    }
    else
    {
        CCS811_Set_Drive_Mode(IDLE, false, false);
    }

    CCS811_ReadAlgorithm_Results(&ccs811_device);
    delay(1000);


    while (1)
    {
        xQueueReceive(queue, &msg, 10);
        if (msg == 'r')
        {
            CCS811_ReadAlgorithm_Results(&ccs811_device);
            eCO2 = CCS811_Get_CO2(&ccs811_device);
            printf("eC02 = %d ppm\n",eCO2);
            tVOC = CCS811_Get_TVOC(&ccs811_device);
            printf("tVOC = %d\n",tVOC);
            printf("Counter = %d\n", ccs811_device.counter);  
            msg = ' ';
        }
        //delay(1000);
    }
}


void app_main()
{
    printf("Starting Tasks!\n");
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    //xTaskCreate(&ccs811_task, "ccs811_task", configMINIMAL_STACK_SIZE*4, NULL, 5, NULL);
    xTaskCreate(&ccs811_task_intr, "ccs811_task_intr", configMINIMAL_STACK_SIZE*6, NULL, 6, NULL);
    
}
