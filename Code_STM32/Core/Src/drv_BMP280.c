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
	uint8_t reg[2];
	uint8_t buf[2];
	for(int i = BMP280_REG_CALIB00; i <= BMP280_REG_CALIB25; i+=2) { 
		reg[0] = i+1;
		reg[1] = i;
		if(HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 2, HAL_MAX_DELAY) != HAL_OK) {
			return 1;
		}
		if(HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
			return 1;
		}
		bmp->calibration[(i-BMP280_REG_CALIB00)/2] = (buf[0] + (buf[1] << 8));
	}
	return 0;
}

uint8_t bmp280GetTemperature(bmp280Struct_t *bmp) {
	uint8_t reg[2] = {BMP280_REG_TEMP_MSB, BMP280_REG_TEMP_LSB};
	uint8_t buf[2];
	if(HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADRESS, &reg, 2, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	if(HAL_I2C_Master_Receive(&hi2c1, BMP280_ADRESS, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
		return 1;
	}
	bmp->temperature = (buf[0] + (buf[1] << 8));
	return 0;
}
