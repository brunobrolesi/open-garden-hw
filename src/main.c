#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "tasks/tasks.h"
#include "../lib/ultrasonic/ultrasonic.h"

void app_main(void)
{
    xTaskCreate(read_water_level, "read_water_level", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
    xTaskCreate(read_temperature, "read_temperature", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
    xTaskCreate(read_conductivity, "read_conductivity", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}