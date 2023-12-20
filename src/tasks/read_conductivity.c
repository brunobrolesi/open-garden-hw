#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "../../lib/tds/tds.h"

const static char *TAG = "read_conductivity";

/*---------------------------------------------------------------
        ADC General Macros
---------------------------------------------------------------*/
// ADC1 Channels
#define TDS_SENSOR_ADC_CHANNEL ADC_CHANNEL_4
#define ADC_ATTENUATION ADC_ATTEN_DB_11
#define NUM_SAMPLES 64

static void adc_calibration_init(bool *calibrated, adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);

void read_conductivity(void *pvParameters)
{
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t adc_init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_init_config, &adc_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t adc_chan_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTENUATION,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, TDS_SENSOR_ADC_CHANNEL, &adc_chan_config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_handle = NULL;
    bool calibrated = false;
    while (!calibrated)
    {
        adc_calibration_init(&calibrated, &adc1_cali_handle);
        if (!calibrated)
        {
            ESP_LOGE(TAG, "ADC1 Calibration Failed");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }

    tds_sensor_t sensor = {
        .kValue = DEFAULT_K_VALUE,
    };

    while (true)
    {
        int adc_raw[NUM_SAMPLES];
        int voltage;
        int total = 0;
        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, TDS_SENSOR_ADC_CHANNEL, &adc_raw[i]));
            total += adc_raw[i];
        }
        int average = total / NUM_SAMPLES;
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_handle, average, &voltage));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Voltage: %d mV", ADC_UNIT_1 + 1, TDS_SENSOR_ADC_CHANNEL, voltage);
        float tds;
        calc_tds_value(&sensor, voltage, 25.0, &tds);
        ESP_LOGI(TAG, "TDS: %.2f ppm", tds);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    // Tear Down
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
    if (calibrated)
    {
        adc_calibration_deinit(adc1_cali_handle);
    }
}

/*---------------------------------------------------------------
        ADC Calibration
---------------------------------------------------------------*/
static void adc_calibration_init(bool *calibrated, adc_cali_handle_t *out_handle)
{
    ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTENUATION,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    adc_cali_handle_t handle = NULL;

    esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
    if (ret == ESP_OK)
    {
        *calibrated = true;
        *out_handle = handle;
    }

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Calibration Success");
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)
    {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    }
    else
    {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }
}

static void adc_calibration_deinit(adc_cali_handle_t handle)
{
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
}
