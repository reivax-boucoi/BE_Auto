/*
 * UARTConfig.h
 *
 *  Created on: 22 août 2018
 *      Author: admin_aboyer
 *
 *  Initialisation de l'UART, transmission et réception.
 *  Dans ce BE, l'UART est utilisé pour les besoins de Freemaster.
 */

#ifndef UARTCONFIG_H_
#define UARTCONFIG_H_

#define LINFlex0 0
#define LINFlex1 1
#define LINInitMode 1
#define LINNormalMode 0

#define UART_BAUD_RATE 115200 //baud rate de l'UART

void Config_LINFlex1_UART(void);
void InitPortLinFlex1(void);
void InitUARTLinFlex1(void);
void SetBaudRateLinFlex(uint8_t Module_nb, uint32_t LIN_clk, uint32_t BaudRate);
void EnterExitInitModeLinFlex(uint8_t Module_nb, uint32_t En_InitMode);
void TransmitData(uint8_t Module_nb, const char *pBuf, uint32_t MessageSize);
void ReceiveData(uint8_t Module_nb, char *pBuf);
void HexToASCII_Word(unsigned int input,uint8_t* buffer);

#endif /* UARTCONFIG_H_ */
