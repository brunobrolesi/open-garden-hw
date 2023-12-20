/**
 * @file tds.h
 * @defgroup tds tds
 * @{
 *
 * ESP-IDF driver for tds meters
 *
 *
 * Copyright (c) 2023 brunobrolesi
 *
 */
#ifndef __tds_H__
#define __tds_H__

#include <esp_err.h>

#define TDS_FACTOR 0.5
#define DEFAULT_K_VALUE 1.0
#define STANDARD_EC_SOLUTION 770 // ppm

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Device descriptor
 */
typedef struct
{
    float kValue; /*!< K value */   
} tds_sensor_t;

/**
 * @brief Calculate the tds value from voltage and temperature
 *
 * @param dev Pointer to the device descriptor
 * @param voltage Voltage in mV
 * @param temperature Temperature in celsius
 * @param[out] tds Total dissolved solids
 * @return `ESP_OK` on success
 */
esp_err_t calc_tds_value(const tds_sensor_t *dev, int voltage, float temperature, float *tds);


/**
 * @brief Calculate the k value from voltage and temperature, the default k value is 1.0
 *
 * @param dev Pointer to the device descriptor
 * @param temperature Temperature in celsius
 * @param voltage Voltage in mV
 * @return `ESP_OK` on success
 */
esp_err_t calibrate_k_value(tds_sensor_t *dev, float temperature, float voltage);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* __tds_H__ */
