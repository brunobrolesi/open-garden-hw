#include <inttypes.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../../lib/ds18x20/ds18x20.h"
#include "../../lib/onewire/onewire.h"
#include <esp_log.h>
#include <esp_err.h>

#define NUM_SENSORS 1

static const gpio_num_t SENSOR_GPIO = 4;
static const char *TAG = "read_temperature";

void read_temperature(void *pvParameters)
{
  onewire_addr_t addrs[NUM_SENSORS];
  float temperature;
  size_t sensor_count = 0;
  bool sensor_found = false;

  esp_err_t res;
  while (!sensor_found)
  {
    res = ds18x20_scan_devices(SENSOR_GPIO, addrs, NUM_SENSORS, &sensor_count);
    if (res != ESP_OK)
    {
      ESP_LOGE(TAG, "Sensors scan error %d (%s)", res, esp_err_to_name(res));
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }

    if (!sensor_count)
    {
      ESP_LOGW(TAG, "No sensors detected!");
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }

    ESP_LOGI(TAG, "%d sensors detected", sensor_count);
    sensor_found = true;
  }

  while (true)
  {
    res = ds18x20_measure_and_read(SENSOR_GPIO, addrs[0], &temperature);
    if (res != ESP_OK)
      ESP_LOGE(TAG, "Could not read from sensor %08" PRIx32 "%08" PRIx32 ": %d (%s)",
               (uint32_t)(addrs[0] >> 32), (uint32_t)addrs[0], res, esp_err_to_name(res));
    else
      ESP_LOGI(TAG, "Sensor %08" PRIx32 "%08" PRIx32 ": %.2f°C",
               (uint32_t)(addrs[0] >> 32), (uint32_t)addrs[0], temperature);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
