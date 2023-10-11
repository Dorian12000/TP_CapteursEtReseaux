/**
 *     _______    _____     __     ______    _______        ____
 *    |   ____|  |     \   |  |   / _____)  |   ____|      /    \
 *    |  |__     |  |\  \  |  |  ( (____    |  |__        /  /\  \
 *    |   __|    |  | \  \ |  |   \____ \   |   __|      /  ____  \
 *    |  |____   |  |  \  \|  |   _____) )  |  |____    /  /    \  \
 *    |_______|  |__|   \_____|  (______/   |_______|  /__/      \__\
 *
 * @file    BMP280_register.h
 * @Author  Dorian Dalbin, Gael Gourdin
 * @Created	2023-10-11
 * @brief
 *
 **/

#define BMP280_ADRESS	(((uint16_t)0x0077) << 1)

// Registers
#define BMP280_REG_CALIB00              ((uint8_t)0x88) // Calibration data calib00
#define BMP280_REG_CALIB25              ((uint8_t)0xA1) // Calibration data calib25
#define BMP280_REG_ID                   ((uint8_t)0xD0) // Chip ID
#define BMP280_REG_RESET                ((uint8_t)0xE0) // Software reset control register
#define BMP280_REG_STATUS               ((uint8_t)0xF3) // Device status register
#define BMP280_REG_CTRL_MEAS            ((uint8_t)0xF4) // Pressure and temperature measure control register
#define BMP280_REG_CONFIG               ((uint8_t)0xF5) // Configuration register
#define BMP280_REG_PRESS_MSB            ((uint8_t)0xF7) // Pressure readings MSB
#define BMP280_REG_PRESS_LSB            ((uint8_t)0xF8) // Pressure readings LSB
#define BMP280_REG_PRESS_XLSB           ((uint8_t)0xF9) // Pressure readings XLSB
#define BMP280_REG_TEMP_MSB             ((uint8_t)0xFA) // Temperature data MSB
#define BMP280_REG_TEMP_LSB             ((uint8_t)0xFB) // Temperature data LSB
#define BMP280_REG_TEMP_XLSB            ((uint8_t)0xFC) // Temperature data XLSB

// Register masks
#define BMP280_STATUS_MSK               ((uint8_t)0x09) // unused bits in 'status'
#define BMP280_OSRS_T_MSK               ((uint8_t)0xE0) // 'osrs_t' in 'control'
#define BMP280_OSRS_P_MSK               ((uint8_t)0x1C) // 'osrs_p' in 'control'
#define BMP280_MODE_MSK                 ((uint8_t)0x03) // 'mode' in 'control'
#define BMP280_STBY_MSK                 ((uint8_t)0xE0) // 't_sb' in 'config'
#define BMP280_FILTER_MSK               ((uint8_t)0x1C) // 'filter' in 'config'

// Value to call a complete power-on-reset routine
#define BMP280_SOFT_RESET_KEY           ((uint8_t)0xB6)

// Chip IDs for samples and mass production parts
#define BMP280_CHIP_ID1                 ((uint8_t)0x56)
#define BMP280_CHIP_ID2                 ((uint8_t)0x57)
#define BMP280_CHIP_ID3                 ((uint8_t)0x58)