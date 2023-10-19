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

BMP280_S32_t tFine;
float tFine_f;

uint8_t bmp280GetId(uint8_t *id) {
	uint8_t reg = BMP280_REG_ID;
	if(HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 1, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	if(HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, id, 1, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	return 0;
}

uint8_t bmp280Config(void) {
	uint8_t reg[2] = {BMP280_REG_CTRL_MEAS, 0b01010111};
	if(HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 2, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	return 0;
}

uint8_t bmp280GetCalib(bmp280Struct_t *bmp) {
	uint8_t reg;
	uint8_t buf[2];
	for(int i = BMP280_REG_CALIB00; i <= BMP280_REG_CALIB25; i+=2) { 
		reg = i;
		if(HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 1, HAL_MAX_DELAY) != HAL_OK) {
			return 1;
		}
		if(HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
			return 1;
		}
		bmp->calibration[(i-BMP280_REG_CALIB00)/2] = (buf[0] | (buf[1] << 8));
	}
	return 0;
}

uint8_t bmp280GetTemperature(bmp280Struct_t *bmp) {
	uint8_t reg[3] = {BMP280_REG_TEMP_MSB, BMP280_REG_TEMP_LSB, BMP280_REG_TEMP_XLSB};
	uint8_t buf[3];
	if(HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 3, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	if(HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, buf, 3, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	bmp->temperature = ((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));
	return 0;
}

uint8_t bmp280GetPressure(bmp280Struct_t *bmp) {
	uint8_t reg[3] = {BMP280_REG_PRESS_MSB, BMP280_REG_PRESS_LSB, BMP280_REG_PRESS_XLSB};
	uint8_t buf[3];
	if(HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 3, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	if(HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, buf, 3, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	bmp->pressure = ((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));
	return 0;
}


BMP280_S32_t bmp280CompensateTInt32(bmp280Struct_t adc) {
	BMP280_S32_t var1, var2, T;
	uint16_t dig_T1 = adc.calibration[0];
	uint16_t dig_T2 = adc.calibration[1];
	uint16_t dig_T3 = adc.calibration[2];
	
	var1 = ((((adc.temperature >> 3) - ((int32_t) dig_T1 << 1)))
			* (int32_t) dig_T2) >> 11;
	var2 = (((((adc.temperature >> 4) - (int32_t) dig_T1)
			* ((adc.temperature >> 4) - (int32_t) dig_T1)) >> 12)
			* (int32_t) dig_T3) >> 14;

	tFine = var1 + var2;
	return (tFine * 5 + 128) >> 8;
}

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
	v2 = (((v1 >> 2) * (v1 >> 2)) >> 11 ) * (dig_P6);
	v2 = v2 + ((v1 * (dig_P5)) << 1);
	v2 = (v2 >> 2) + ((dig_P4) << 16);
	v1 = (((dig_P3 * (((v1 >> 2) * (v1 >> 2)) >> 13 )) >> 3) + \
			(((dig_P2) * v1) >> 1)) >> 18;
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

float bmp280GetCompensateTemp(void) {
	bmp280Struct_t dev;
	bmp280GetCalib(&dev);
	bmp280GetTemperature(&dev);
	return bmp280CompensateTInt32(dev)/100;
}

float bmp280GetCompensatePress(void) {
	bmp280Struct_t dev;
	bmp280GetCalib(&dev);
	bmp280GetPressure(&dev);
	return bmp280CompensatePInt32(dev)/256;
}