#include <stdio.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../lib/ultrasonic/ultrasonic.h"
#include <esp_err.h>
#include <esp_log.h>

static const char *TAG = "read_water_level";

#define MAX_DISTANCE_CM 500 // 5m max
#define TRIGGER_GPIO 17
#define ECHO_GPIO 16

void read_water_level(void *pvParameters)
{
  ultrasonic_sensor_t sensor = {
      .trigger_pin = TRIGGER_GPIO,
      .echo_pin = ECHO_GPIO};

  ultrasonic_init(&sensor);

  while (true)
  {
    float level;
    esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &level);
    if (res == ESP_OK)
    {
      ESP_LOGI(TAG, "Level: %.2f cm", level);
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    switch (res)
    {
    case ESP_ERR_ULTRASONIC_PING:
      ESP_LOGE(TAG, "cannot ping (device is in invalid state)");
      break;
    case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
      ESP_LOGE(TAG, "ping timeout (no device found)");
      break;
    case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
      ESP_LOGE(TAG, "echo timeout (i.e. distance too big)");
      break;
    default:
      ESP_LOGE(TAG, "ultrasonic measure fails with: %d (%s)", res, esp_err_to_name(res));
    }

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}