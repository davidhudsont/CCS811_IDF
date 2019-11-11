


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


#define BUTTON_PIN (25)
#define ESP_INTR_FLAG_DEFUALT 0

#define LED_PIN (5)



void button_isr_init();

void gpio_task_example(void *arg);