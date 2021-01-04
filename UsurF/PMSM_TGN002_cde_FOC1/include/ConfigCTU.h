/*
 * ConfigCTU.h
 *
 *  Created on: 16 août 2018
 *      Author: admin_aboyer
 *
 *  Configuration du CTU0 (synchronisation FlexPWM --> ADC)
 */

#ifndef CONFIGCTU_H_
#define CONFIGCTU_H_

#define CTUOUT_ON 0
#define CTUOUT_OFF 1
#define MODE_ADC_STREAM 0
#define MODE_ADC_PARALLEL 1
#define TRIG_DIS 0
#define TRIG_EN 1

void Init_CTU0_TriggerGenerator_Trigger(uint8_t TriggerInput, uint8_t RiseTrigger, uint8_t FallTrigger,
		uint8_t TriggerOutput,uint8_t ClkPres, uint16_t InitCptr, uint16_t MaxCptr, uint16_t CompareVal);
void Init_CTU0_TriggerGenerator_Trigger2(uint8_t TriggerInput, uint8_t RiseTrigger, uint8_t FallTrigger,
		uint8_t OutTrig0, uint8_t OutTrig1, uint8_t ClkPres, uint16_t InitCptr, uint16_t MaxCptr,
		uint16_t CompareVal0, uint16_t CompareVal1);
void Init_CTU0_TriggerGenerator_Sequential(uint8_t TriggerInput0, uint8_t TriggerInput1, uint8_t TriggerInput2,
		uint8_t TriggerOutput0, uint8_t TriggerOutput1, uint8_t TriggerOutput2,uint8_t RiseTrigger, uint8_t FallTrigger,
		uint8_t ClkPres, uint16_t InitCptr, uint16_t MaxCptr, uint16_t CompareVal);
void CTU0_Stream_or_Parallel_ADC_List(uint8_t Mode);
void Init_CTU0_TriggerScheduler(uint8_t TrigIn, uint8_t En_Out, uint8_t ADC_En, uint8_t ExtTrigOut_En);
void Init_CTU0_TriggerScheduler_ADC_SingleConv(uint8_t Trig_Index, uint8_t ADCPort, uint8_t ADC_ch, uint8_t List_Cmd_Adr, uint8_t FIFO_dest);
void Init_CTU0_TriggerScheduler_ADC_DualConv(uint8_t ADC_chA, uint8_t ADC_chB, uint8_t FIFO_dest);
void Init_CTU0_GeneralControl(uint8_t OutputEnable);
void CTU0_ConfigDMA_ADC(uint8_t Fifo_Nb, uint8_t Thres);
void CTU0_General_Reload_Enable(void);
void CTU0_General_Reload_Disable(void);

#endif /* CONFIGCTU_H_ */
