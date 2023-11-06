#include "main.h"
#include "can.h"
#include "motor.h"
#include "log/logger.h"
#include "BMP280/drv_BMP280.h"

CAN_TxHeaderTypeDef TxHeader;
uint8_t 			TxData[8];
uint32_t			TxMailbox;

void motorInit(void) {
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.StdId = 0x62;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.DLC = 3;

	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, NULL, &TxMailbox) != HAL_OK)
	{
		Error_Handler ();
	}
}

void motorSetPosition(uint8_t positionAngle, uint8_t positionSign) {
	TxHeader.StdId = 0x61;
	TxData[0] = positionAngle;
	TxData[1] = positionSign;
	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		printf("motorSetPosition error");
		Error_Handler ();
	}
	else 
		printf("go to %d°", positionAngle);
}

void motorSetPositionDenpendingTemperature(void) {
	uint8_t positionTemperatureRate;
	bmp280Struct_t bmp;
	bmp280GetTemperature(&bmp);
	positionTemperatureRate = bmp.temperature%180;
	motorSetPosition(positionTemperatureRate, 1);
}