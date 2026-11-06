
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SWITCH_PIN GPIO_NUM_18
#define LED_PIN GPIO_NUM_23

void app_main(void)
{
    // Настройваме превключвателя като вход
    gpio_reset_pin(SWITCH_PIN);
    gpio_set_direction(SWITCH_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SWITCH_PIN, GPIO_PULLUP_ONLY);

    // Настройваме LED като изход
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    while (1)
    {
        // Прочитаме превключвателя
        int state = gpio_get_level(SWITCH_PIN);

        if (state == 0)
        {
            gpio_set_level(LED_PIN, 1);
            printf("LED ON\n");
        }
        else
        {
            gpio_set_level(LED_PIN, 0);
            printf("LED OFF\n");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
