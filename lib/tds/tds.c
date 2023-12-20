/**
 * @file tds.c
 *
 * ESP-IDF driver for tds meters
 *
 *
 * Copyright (c) 2023 brunobrolesi
 *
 */
#include "tds.h"
#include <esp_err.h>
#include <math.h>

#define CHECK_ARG(VAL)                  \
    do                                  \
    {                                   \
        if (!(VAL))                     \
            return ESP_ERR_INVALID_ARG; \
    } while (0)
#define CHECK(x)                \
    do                          \
    {                           \
        esp_err_t __;           \
        if ((__ = x) != ESP_OK) \
            return __;          \
    } while (0)

esp_err_t calc_tds_value(const tds_sensor_t *dev, int voltage, float temperature, float *tds)
{
    CHECK_ARG(dev && voltage && temperature && tds);
    
    float conv_voltage = voltage / (float)1000.0;
    float ecValue = (133.42 * pow(conv_voltage, 3) - 255.86 * pow(conv_voltage, 2) + 857.39 * conv_voltage) * dev->kValue;                                                                  
    float ecValueCompensated = ecValue / (1.0 + 0.02 * (temperature - 25.0));                                                                                                   
    float tdsValue = ecValueCompensated * TDS_FACTOR;
    *tds = tdsValue;

    return ESP_OK;
}

esp_err_t calibrate_k_value(tds_sensor_t *dev, float temperature, float voltage)
{
    CHECK_ARG(dev && temperature && voltage);

    float conv_voltage = voltage / (float)1000.0;
    float rawECsolution = STANDARD_EC_SOLUTION / (float)(TDS_FACTOR);
    rawECsolution = rawECsolution * (1.0 + 0.02 * (temperature - 25.0));

    float KValueTemp = rawECsolution / (133.42 * pow(conv_voltage, 3) - 255.86 * pow(conv_voltage, 2) + 857.39 * conv_voltage);
    if ((rawECsolution > 0) && (rawECsolution < 2000) && (KValueTemp > 0.25) && (KValueTemp < 4.0))
    {
        dev->kValue = KValueTemp;
        return ESP_OK;
    }
    return ESP_FAIL;
}
