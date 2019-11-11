

#include "button_intr.h"

static volatile int switch_led = 0;

static void IRAM_ATTR button_isr_handler(void* arg)
{
    /* Blink off (output low) */
    gpio_set_level(LED_PIN, switch_led);

    switch_led = switch_led ^ 1;

}



void button_isr_init()
{
    gpio_config_t io_conf;

    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_ANYEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = 1ULL<<BUTTON_PIN; // Bit mask so we need to set bit 25 to 1
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_ANYEDGE);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFUALT);

    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);


    // LED setup
    gpio_pad_select_gpio(LED_PIN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

}

void gpio_task_example(void *arg)
{

}