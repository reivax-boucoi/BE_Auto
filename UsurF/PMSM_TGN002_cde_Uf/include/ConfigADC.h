/*
 * ConfigADC.h
 *
 *  Created on: 16 août 2018
 *      Author: admin_aboyer
 *
 *  Fonctions associées aux ADC (initialisation, acquisition, calibration)
 */

#ifndef CONFIGADC_H_
#define CONFIGADC_H_

typedef struct {
	uint8_t ValidResult;
	uint16_t ConvResult;
} ADC_Result_struct;

//les différents modules d'ADC
#define ADC0 0
#define ADC1 1
#define ADC2 2
#define ADC3 3

//les différents modes de l'ADC
#define RIGHT_ALIGNED 0
#define LEFT_ALIGNED 1
#define ONE_SHOT 0
#define SCAN 1
#define EXT_TRIG_DIS 0
#define EXT_TRIG_EN 1
#define TRIG_EDGE_FALL 0
#define TRIG_EDGE_RISE 1
#define CTU_TRIG_DIS 0
#define CTU_TRIG_EN 1
#define HALF_CLK_ADC 0
#define FULL_CLK_ADC 1
#define EOC_OFF 0
#define EOC_ON 1
#define ECH_OFF 0
#define ECH_ON 1
#define JEOC_OFF 0
#define JEOC_ON 1
#define JECH_OFF 0
#define JECH_ON 1
#define ECTU_OFF 0
#define ECTU_ON 1


void ADC_Enter_PWDN(uint8_t ADC_Nb);
void ADC_Exit_PWDN(uint8_t ADC_Nb);
void ADC_Start_NormalMode(uint8_t ADC_Nb);
void ADC_Stop_NormalMode(uint8_t ADC_Nb);
void ADC_init(uint8_t ADC_Nb, uint8_t Align, uint8_t ConvModeNormal, uint8_t TrigEn, uint8_t TrigEdge, uint8_t CTUTrig, uint8_t ADCClkSel);
void ADC_ConvTime_config(uint8_t ADC_Nb, uint8_t SamplDuration);
void ADC_DisableAllChannel_NormalConv(uint8_t ADC_Nb);
void ADC_SelectChannel_NormalConv(uint8_t ADC_Nb, uint8_t ChannelNb);
void ADC_ISRGlobalEnable(uint8_t ADC_Nb, uint8_t EOC, uint8_t ECH, uint8_t JEOC, uint8_t JECH, uint8_t ECTU);
void ADC_ChannelMask_ISR(uint8_t ADC_Nb, uint8_t ChannelNb, uint8_t Mask);
ADC_Result_struct ADC_ReadChannel(uint8_t ADC_Nb, uint8_t ChannelNb);
uint8_t ADC_Calibration(uint8_t ADC_Nb);

uint8_t ADC_Get_EOCTU(uint8_t ADC_Nb);
void ADC_Clear_EOCTU(uint8_t ADC_Nb);
void ADC_Clear_All_Flags(uint8_t ADC_Nb);


#endif /* CONFIGADC_H_ */
