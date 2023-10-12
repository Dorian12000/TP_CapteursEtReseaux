/**
 *     _______    _____     __     ______    _______        ____
 *    |   ____|  |     \   |  |   / _____)  |   ____|      /    \
 *    |  |__     |  |\  \  |  |  ( (____    |  |__        /  /\  \
 *    |   __|    |  | \  \ |  |   \____ \   |   __|      /  ____  \
 *    |  |____   |  |  \  \|  |   _____) )  |  |____    /  /    \  \
 *    |_______|  |__|   \_____|  (______/   |_______|  /__/      \__\
 *
 * @file    drv_BMP280.h
 * @Author  Dorian Dalbin, Gael Gourdin
 * @Created	2023-10-11
 * @brief
 *
 **/

typedef long signed int BMP280_S32_t;

typedef struct bmp280Struct_s {
	uint16_t calibration[12];
	uint32_t temperature;
	uint32_t pressure;
}bmp280Struct_t;

uint8_t bmp280GetId(uint8_t *id);
uint8_t bmp280Config(void);
uint8_t bmp280GetCalib(bmp280Struct_t *param);
uint8_t bmp280GetTemperature(bmp280Struct_t *bmp);
uint8_t bmp280GetPressure(bmp280Struct_t *bmp);
BMP280_S32_t bmp280CompensateTInt32(bmp280Struct_t adc);
BMP280_S32_t bmp280CompensatePInt32(bmp280Struct_t adc);
float bmp280GetCompensateTemp(void);
float bmp280GetCompensatePress(void);