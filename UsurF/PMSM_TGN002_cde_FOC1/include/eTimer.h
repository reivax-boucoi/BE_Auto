/*
 * eTimer.h
 *
 *  Created on: 21 juil. 2019
 *      Author: admin_aboyer
 *
 *      configuration des eTimer0, 1, 2 (uniquement pour une mesure de type Input Compare
 */

#ifndef ETIMER_H_
#define ETIMER_H_

#include "typedefs.h"

void Config_eTimer0_InputCapture(uint8_t channel);
void Config_eTimer1_InputCapture(uint8_t channel);
void Config_eTimer2_InputCapture(uint8_t channel);
void EnableInterrupt_eTimer0_InputCapture(uint8_t channel,uint8_t NumCapture);
void EnableInterrupt_eTimer1_InputCapture(uint8_t channel,uint8_t NumCapture);
void EnableInterrupt_eTimer1_InputCapture(uint8_t channel,uint8_t NumCapture);
void Start_eTimer0_InputCapture_Count(uint8_t channel);
void Start_eTimer1_InputCapture_Count(uint8_t channel);
void Start_eTimer2_InputCapture_Count(uint8_t channel);
uint16_t Get_eTimer0_InputCaptureReg1(uint8_t channel);
uint16_t Get_eTimer1_InputCaptureReg1(uint8_t channel);
uint16_t Get_eTimer2_InputCaptureReg1(uint8_t channel);
uint16_t Get_eTimer0_InputCaptureReg2(uint8_t channel);
uint16_t Get_eTimer1_InputCaptureReg2(uint8_t channel);
uint16_t Get_eTimer2_InputCaptureReg2(uint8_t channel);

#endif /* ETIMER_H_ */
