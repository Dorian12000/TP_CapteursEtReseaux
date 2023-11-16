/**
 *     _______    _____     __     ______    _______        ____
 *    |   ____|  |     \   |  |   / _____)  |   ____|      /    \
 *    |  |__     |  |\  \  |  |  ( (____    |  |__        /  /\  \
 *    |   __|    |  | \  \ |  |   \____ \   |   __|      /  ____  \
 *    |  |____   |  |  \  \|  |   _____) )  |  |____    /  /    \  \
 *    |_______|  |__|   \_____|  (______/   |_______|  /__/      \__\
 *
 * @file    drv_BMP280.c
 * @Author  Dorian Dalbin, Gael Gourdin
 * @Created	2023-10-11
 * @brief
 *
 **/

#include "main.h"
#include "i2c.h"
#include "log/logger.h"

#include "BMP280/BMP280_register.h"
#include "BMP280/drv_BMP280.h"

enum {
	SLEEP_MODE = 0,
	FORCED_MODE1,
	FORCED_MODE2,
	NORMAL_MODE,
};

enum {
	PRESSURE_OVERSAMPLING_SKIPPED = 0,
	PRESSURE_OVERSAMPLING_1,
	PRESSURE_OVERSAMPLING_2,
	PRESSURE_OVERSAMPLING_4,
	PRESSURE_OVERSAMPLING__8,
	PRESSURE_OVERSAMPLING_16,
};

enum {
	TEMPERATURE_OVERSAMPLING_SKIPPED = 0,
	TEMPERATURE_OVERSAMPLING_1,
	TEMPERATURE_OVERSAMPLING_2,
	TEMPERATURE_OVERSAMPLING_4,
	TEMPERATURE_OVERSAMPLING_8,
	TEMPERATURE_OVERSAMPLING_16,
};


BMP280_S32_t tFine;
float tFine_f;

/**
 * @brief Get the BMP280 sensor ID.
 *
 * This function retrieves the ID of the BMP280 sensor by performing
 * an I2C communication to read the sensor's ID register. The ID is
 * stored in the provided memory location pointed to by the 'id' parameter.
 *
 * @param id Pointer to the memory location where the BMP280 sensor ID will be stored.
 * @return 0 if successful, 1 if an error occurs during I2C communication.
 *
 * @note This function assumes that the I2C hardware (hi2c1) is already initialized
 *       and the BMP280 sensor is connected and properly configured.
 */
uint8_t bmp280GetId(uint8_t *id) {
    uint8_t reg = BMP280_REG_ID; /**< Register address for BMP280 sensor ID */

    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 1; /**< Return error code if I2C transmit fails */
    }

    if (HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, id, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 1; /**< Return error code if I2C receive fails */
    }

    return 0; /**< Return 0 if the operation is successful */
}

/**
 * @brief Configure BMP280 sensor settings.
 *
 * This function configures the BMP280 sensor by setting the control measurement
 * register with specific oversampling rates for pressure and temperature, as well
 * as the operating mode.
 *
 * @return 0 if successful, 1 if an error occurs during I2C communication.
 *
 * @note This function assumes that the I2C hardware (hi2c1) is already initialized
 *       and the BMP280 sensor is connected and properly configured.
 */
uint8_t bmp280Config(void) {
    uint8_t reg[2] = {
        BMP280_REG_CTRL_MEAS,
        ((PRESSURE_OVERSAMPLING_2 << 5) | (TEMPERATURE_OVERSAMPLING_16 << 2) | NORMAL_MODE)
    }; /**< Control measurement register configuration */

    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, reg, 2, HAL_MAX_DELAY) != HAL_OK) {
        return 1; /**< Return error code if I2C transmit fails */
    }

    return 0; /**< Return 0 if the operation is successful */
}

/**
 * @brief Retrieve calibration data from BMP280 sensor.
 *
 * This function reads calibration data from BMP280 sensor registers
 * and stores it in the provided BMP280 structure for future use in
 * temperature and pressure calculations.
 *
 * @param bmp Pointer to the BMP280 structure where calibration data will be stored.
 * @return 0 if successful, 1 if an error occurs during I2C communication.
 *
 * @note This function assumes that the I2C hardware (hi2c1) is already initialized
 *       and the BMP280 sensor is connected and properly configured.
 */
uint8_t bmp280GetCalib(bmp280Struct_t *bmp) {
    uint8_t reg;
    uint8_t buf[2];

    for (int i = BMP280_REG_CALIB00; i <= BMP280_REG_CALIB25; i += 2) {
        reg = i;

        if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 1, HAL_MAX_DELAY) != HAL_OK) {
            return 1; /**< Return error code if I2C transmit fails */
        }

        if (HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
            return 1; /**< Return error code if I2C receive fails */
        }

        bmp->calibration[(i - BMP280_REG_CALIB00) / 2] = (buf[0] | (buf[1] << 8));
    }

    return 0; /**< Return 0 if the operation is successful */
}

/**
 * @brief Retrieve temperature data from BMP280 sensor.
 *
 * This function reads temperature data from BMP280 sensor registers
 * and stores it in the provided BMP280 structure for future use.
 *
 * @param bmp Pointer to the BMP280 structure where temperature data will be stored.
 * @return 0 if successful, 1 if an error occurs during I2C communication.
 *
 * @note This function assumes that the I2C hardware (hi2c1) is already initialized
 *       and the BMP280 sensor is connected and properly configured.
 */
uint8_t bmp280GetTemperature(bmp280Struct_t *bmp) {
    uint8_t reg[3] = {BMP280_REG_TEMP_MSB, BMP280_REG_TEMP_LSB, BMP280_REG_TEMP_XLSB};
    uint8_t buf[3];

    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, reg, 3, HAL_MAX_DELAY) != HAL_OK) {
        return 1; /**< Return error code if I2C transmit fails */
    }

    if (HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, buf, 3, HAL_MAX_DELAY) != HAL_OK) {
        return 1; /**< Return error code if I2C receive fails */
    }

    bmp->temperature = ((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));

    return 0; /**< Return 0 if the operation is successful */
}

/**
 * @brief Retrieve pressure data from BMP280 sensor.
 *
 * This function reads pressure data from BMP280 sensor registers
 * and stores it in the provided BMP280 structure for future use.
 *
 * @param bmp Pointer to the BMP280 structure where pressure data will be stored.
 * @return 0 if successful, 1 if an error occurs during I2C communication.
 *
 * @note This function assumes that the I2C hardware (hi2c1) is already initialized
 *       and the BMP280 sensor is connected and properly configured.
 */
uint8_t bmp280GetPressure(bmp280Struct_t *bmp) {
    uint8_t reg[3] = {BMP280_REG_PRESS_MSB, BMP280_REG_PRESS_LSB, BMP280_REG_PRESS_XLSB};
    uint8_t buf[3];

    if (HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, reg, 3, HAL_MAX_DELAY) != HAL_OK) {
        return 1; /**< Return error code if I2C transmit fails */
    }

    if (HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, buf, 3, HAL_MAX_DELAY) != HAL_OK) {
        return 1; /**< Return error code if I2C receive fails */
    }

    bmp->pressure = ((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));

    return 0; /**< Return 0 if the operation is successful */
}

/**
 * @brief Compensate temperature in signed 32-bit format using BMP280 calibration data.
 *
 * This function compensates the raw temperature data obtained from the BMP280 sensor
 * by applying calibration parameters. The compensated temperature is returned in
 * signed 32-bit format.
 *
 * @param adc BMP280 structure containing the raw temperature data and calibration parameters.
 * @return Compensated temperature in signed 32-bit format.
 *
 * @note This function assumes that the BMP280 sensor has been properly configured,
 *       and calibration data is available in the BMP280 structure.
 */
BMP280_S32_t bmp280CompensateTInt32(bmp280Struct_t adc) {
    BMP280_S32_t var1, var2, T;
    uint16_t dig_T1 = adc.calibration[0];
    uint16_t dig_T2 = adc.calibration[1];
    uint16_t dig_T3 = adc.calibration[2];

    var1 = ((((adc.temperature >> 3) - ((int32_t)dig_T1 << 1))) * (int32_t)dig_T2) >> 11;
    var2 = (((((adc.temperature >> 4) - (int32_t)dig_T1) * ((adc.temperature >> 4) - (int32_t)dig_T1)) >> 12)
            * (int32_t)dig_T3) >> 14;

    tFine = var1 + var2;
    return (tFine * 5 + 128) >> 8;
}

/**
 * @brief Compensate pressure in signed 32-bit format using BMP280 calibration data.
 *
 * This function compensates the raw pressure data obtained from the BMP280 sensor
 * by applying calibration parameters. The compensated pressure is returned in
 * signed 32-bit format.
 *
 * @param adc BMP280 structure containing the raw pressure data and calibration parameters.
 * @return Compensated pressure in signed 32-bit format.
 *
 * @note This function assumes that the BMP280 sensor has been properly configured,
 *       and calibration data is available in the BMP280 structure.
 */
BMP280_S32_t bmp280CompensatePInt32(bmp280Struct_t adc) {
    uint32_t v1, v2, p;
    uint32_t dig_P1 = adc.calibration[3];
    uint32_t dig_P2 = adc.calibration[4];
    uint32_t dig_P3 = adc.calibration[5];
    uint32_t dig_P4 = adc.calibration[6];
    uint32_t dig_P5 = adc.calibration[7];
    uint32_t dig_P6 = adc.calibration[8];
    uint32_t dig_P7 = adc.calibration[9];
    uint32_t dig_P8 = adc.calibration[10];
    uint32_t dig_P9 = adc.calibration[11];

    v1 = (((int32_t)tFine) >> 1) - (int32_t)64000;
    v2 = (((v1 >> 2) * (v1 >> 2)) >> 11) * (dig_P6);
    v2 = v2 + ((v1 * (dig_P5)) << 1);
    v2 = (v2 >> 2) + ((dig_P4) << 16);
    v1 = (((dig_P3 * (((v1 >> 2) * (v1 >> 2)) >> 13)) >> 3) + (((dig_P2) * v1) >> 1)) >> 18;
    v1 = (((32768 + v1)) * (dig_P1)) >> 15;

    if (v1 == 0) {
        // avoid exception caused by division by zero
        return 0;
    }

    p = (((uint32_t)(((int32_t)1048576) - adc.pressure) - (uint32_t)(v2 >> 12))) * 3125U;

    if (p < 0x80000000U) {
        p = (p << 1) / ((uint32_t)v1);
    } else {
        p = (p / (uint32_t)v1) << 1;
    }

    v1 = ((dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    v2 = (((int32_t)(p >> 2)) * (dig_P8)) >> 13;
    p = (uint32_t)((int32_t)p + ((v1 + v2 + dig_P7) >> 4));

    return p * 1000U;
}

/**
 * @brief Get compensated temperature from BMP280 sensor.
 *
 * This function retrieves the compensated temperature from the BMP280 sensor
 * by obtaining calibration data, reading raw temperature data, and applying
 * compensation calculations. The temperature is returned in floating-point format.
 *
 * @return Compensated temperature in degrees Celsius.
 *
 * @note This function assumes that the BMP280 sensor has been properly configured
 *       and is connected. Calibration data and temperature measurements are obtained
 *       using the corresponding BMP280 functions.
 */
float bmp280GetCompensateTemp(void) {
    bmp280Struct_t dev; /**< BMP280 structure to store calibration and temperature data */
    bmp280GetCalib(&dev); /**< Obtain calibration data from BMP280 sensor */
    bmp280GetTemperature(&dev); /**< Obtain raw temperature data from BMP280 sensor */
    return bmp280CompensateTInt32(dev) / 100.0f; /**< Return compensated temperature in degrees Celsius */
}

/**
 * @brief Get compensated pressure from BMP280 sensor.
 *
 * This function retrieves the compensated pressure from the BMP280 sensor
 * by obtaining calibration data, reading raw pressure data, and applying
 * compensation calculations. The pressure is returned in floating-point format.
 *
 * @return Compensated pressure in Pascals.
 *
 * @note This function assumes that the BMP280 sensor has been properly configured
 *       and is connected. Calibration data and pressure measurements are obtained
 *       using the corresponding BMP280 functions.
 */
float bmp280GetCompensatePress(void) {
    bmp280Struct_t dev; /**< BMP280 structure to store calibration and pressure data */
    bmp280GetCalib(&dev); /**< Obtain calibration data from BMP280 sensor */
    bmp280GetPressure(&dev); /**< Obtain raw pressure data from BMP280 sensor */
    return bmp280CompensatePInt32(dev) / 256.0f; /**< Return compensated pressure in Pascals */
}
