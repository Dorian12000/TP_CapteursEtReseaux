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

typedef struct bmp280Struct_s {
	uint16_t calibration[12];
	uint16_t temperature;
	uint16_t pressure;
}bmp280Struct_t;

uint8_t bmp280GetId(uint8_t *id);
uint8_t bmp280Config(void);
uint8_t bmp280GetCalib(bmp280Struct_t *param);
uint8_t bmp280GetTemperature(bmp280Struct_t *bmp);