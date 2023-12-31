/**
 *     _______    _____     __     ______    _______        ____
 *    |   ____|  |     \   |  |   / _____)  |   ____|      /    \
 *    |  |__     |  |\  \  |  |  ( (____    |  |__        /  /\  \
 *    |   __|    |  | \  \ |  |   \____ \   |   __|      /  ____  \
 *    |  |____   |  |  \  \|  |   _____) )  |  |____    /  /    \  \
 *    |_______|  |__|   \_____|  (______/   |_______|  /__/      \__\
 *
 * @file    shell.c
 * @Author  Dorian Dalbin, Gael Gourdin
 * @Created	2023-10-11
 * @brief
 *
 **/

#include "main.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BMP280/drv_BMP280.h"
#include "motor.h"
#include "shell.h"

uint8_t prompt[]="user@Nucleo-STM32F446>>";
uint8_t cmdNotFound[]="Command not found\r\n";
uint8_t brian[]="Brian is in the kitchen\r\n";
uint8_t newline[]="\r\n";
uint8_t backspace[]="\b \b";
uint8_t uartRxReceived;
uint8_t uartRxBufferRasp[UART_RX_BUFFER_SIZE];
uint8_t uartRxBufferPC[UART_RX_BUFFER_SIZE];
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];

char	 	cmdBuffer[CMD_BUFFER_SIZE];
int 		idx_cmd;
char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;

void Shell_Init(void){
	memset(argv, NULL, MAX_ARGS*sizeof(char*));
	memset(cmdBuffer, NULL, CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer, NULL, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartRxBufferRasp, NULL, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartRxBufferPC, NULL, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer, NULL, UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, uartRxBufferPC, UART_RX_BUFFER_SIZE);
	//HAL_UART_Receive_IT(&huart1, uartRxBufferRasp, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart1, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
}

void Shell_Loop(void){
	if(uartRxReceived){
		switch(uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1, newline, sizeof(newline), HAL_MAX_DELAY);
			cmdBuffer[idx_cmd] = '\0';
			argc = 0;
			token = strtok(cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: // Suppression du dernier caractère
			cmdBuffer[idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;

		default: // Nouveau caractère
			cmdBuffer[idx_cmd++] = uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		uartRxReceived = 0;
	}

	if(newCmdReady){
		if(strcmp(argv[0],"WhereisBrian?")==0){
			HAL_UART_Transmit(&huart2, brian, sizeof(brian), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"GET_T")==0){
			BMP280_S32_t temp = bmp280GetCompensateTemp();
			sprintf(uartTxBuffer, "T = %ld_C\n\r", temp);
			HAL_UART_Transmit(&huart2, uartTxBuffer, sizeof(uartTxBuffer), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1, uartTxBuffer, sizeof(uartTxBuffer), HAL_MAX_DELAY);
			//motorSetPositionDenpendingTemperature();
		}
		else if(strcmp(argv[0],"GET_P")==0){
			BMP280_S32_t press = bmp280GetCompensatePress();
			sprintf(uartTxBuffer, "P = %d Pa\n\r", press);
			HAL_UART_Transmit(&huart2, uartTxBuffer, sizeof(uartTxBuffer), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1, uartTxBuffer, sizeof(uartTxBuffer), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"GO_TO")==0){
			//HAL_UART_Transmit(&huart1, press, sizeof(press), HAL_MAX_DELAY);
			uint8_t positionAngle = 0;
			positionAngle = atoi(&argv[1]);
			motorSetPosition(positionAngle, 1);
			sprintf(uartTxBuffer, "Go to %d°\n\r", positionAngle);
			HAL_UART_Transmit(&huart2, uartTxBuffer, sizeof(uartTxBuffer), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"GET_K")==0){
			//HAL_UART_Transmit(&huart1, press, sizeof(press), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"GET_A")==0){
			//HAL_UART_Transmit(&huart1, press, sizeof(press), HAL_MAX_DELAY);
		}
		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart1, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	if(huart->Instance == USART1) {
		uartRxBuffer[0] = uartRxBufferRasp[0];
		//HAL_UART_Receive_IT(&huart1, uartRxBufferRasp, UART_RX_BUFFER_SIZE);
	}
	else if(huart->Instance == USART2) {
		uartRxBuffer[0] = uartRxBufferPC[0];
		HAL_UART_Receive_IT(&huart2, uartRxBufferPC, UART_RX_BUFFER_SIZE);
	}
	uartRxReceived = 1;
}