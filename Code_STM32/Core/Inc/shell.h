/**
 *     _______    _____     __     ______    _______        ____
 *    |   ____|  |     \   |  |   / _____)  |   ____|      /    \
 *    |  |__     |  |\  \  |  |  ( (____    |  |__        /  /\  \
 *    |   __|    |  | \  \ |  |   \____ \   |   __|      /  ____  \
 *    |  |____   |  |  \  \|  |   _____) )  |  |____    /  /    \  \
 *    |_______|  |__|   \_____|  (______/   |_______|  /__/      \__\
 *
 * @file    shell.h
 * @Author  Dorian Dalbin, Gael Gourdin
 * @Created	2023-10-11
 * @brief
 *
 **/

#define UART_RX_BUFFER_SIZE 1
#define UART_TX_BUFFER_SIZE 10
#define CMD_BUFFER_SIZE 64
#define MAX_ARGS 9
#define ASCII_LF 0x0A			// LF = line feed, saut de ligne
#define ASCII_CR 0x0D			// CR = carriage return, retour chariot
#define ASCII_BACK 0x08			// BACK = Backspace

void Shell_Init(void);
void Shell_Loop(void);
