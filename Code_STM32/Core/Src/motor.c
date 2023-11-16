#include "main.h"
#include "can.h"
#include "motor.h"
#include "log/logger.h"
#include "BMP280/drv_BMP280.h"

CAN_TxHeaderTypeDef TxHeader;
uint8_t 			TxData[8];
uint32_t			TxMailbox;

/**
 * @brief Initialize the motor communication.
 *
 * This function initializes the communication parameters for the motor control.
 * It configures the CAN header with specific values and adds a transmit message
 * to the CAN hardware for the motor control.
 *
 * @note This function assumes the CAN hardware (hcan1) is already initialized.
 *
 * @return None
 *
 * @see Error_Handler() is called in case of an error during CAN message transmission.
 */
void motorInit(void) {
    TxHeader.IDE = CAN_ID_STD;            /**< Standard CAN identifier */
    TxHeader.StdId = 0x62;                /**< Standard CAN message identifier (0x62) */
    TxHeader.RTR = CAN_RTR_DATA;          /**< Remote Transmission Request: Data frame */
    TxHeader.TransmitGlobalTime = DISABLE;/**< Disable the transmission of the global time */
    TxHeader.DLC = 3;                     /**< Data Length Code: 3 bytes */

    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, NULL, &TxMailbox) != HAL_OK) {
        Error_Handler();  /**< Handle error if message transmission fails */
    }
}

/**
 * @brief Set the target position for the motor.
 *
 * This function sets the target position for the motor by configuring
 * the CAN header with a specific message identifier and packing the
 * position angle and sign into the data array. The message is then
 * transmitted using the CAN hardware.
 *
 * @param positionAngle The angle value for the target position (in degrees).
 * @param positionSign  The sign of the target position: 0 for positive, 1 for negative.
 *
 * @note This function assumes the CAN hardware (hcan1) is already initialized.
 *       If the transmission fails, an error message is printed, and the
 *       Error_Handler() function is called.
 *
 * @return None
 */
void motorSetPosition(uint8_t positionAngle, uint8_t positionSign) {
    TxHeader.StdId = 0x61;          /**< Standard CAN message identifier (0x61) */
    TxData[0] = positionAngle;      /**< Position angle data byte */
    TxData[1] = positionSign;       /**< Position sign data byte */

    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
        printf("motorSetPosition error");  /**< Print error message */
        Error_Handler();  /**< Handle error if message transmission fails */
    } else {
        printf("go to %d°", positionAngle);  /**< Print success message with the target position */
    }
}


/**
 * @brief Set the motor position based on the current temperature.
 *
 * This function calculates a target position for the motor based on the
 * current temperature obtained from a BMP280 sensor. The position is
 * determined by taking the modulo of the temperature with 180 and setting
 * the sign to 1. The resulting position is then passed to the motorSetPosition
 * function for further processing and transmission.
 *
 * @note This function assumes the CAN hardware (hcan1) is already initialized,
 *       and a BMP280 sensor is available for temperature readings.
 *
 * @return None
 *
 * @see motorSetPosition(), bmp280GetTemperature()
 */
void motorSetPositionDenpendingTemperature(void) {
    uint8_t positionTemperatureRate; /**< Calculated position based on temperature */
    bmp280Struct_t bmp;               /**< BMP280 sensor structure for temperature readings */

    bmp280GetTemperature(&bmp); /**< Obtain current temperature from BMP280 sensor */
    positionTemperatureRate = bmp.temperature % 180; /**< Calculate position based on temperature */
    
    motorSetPosition(positionTemperatureRate, 1); /**< Set motor position using calculated values */
}
