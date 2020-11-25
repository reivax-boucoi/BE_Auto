/*
 * eTimer.c
 *
 *  Created on: 21 juil. 2019
 *      Author: admin_aboyer
 *
 *      configuration des eTimer0, 1, 2 (uniquement pour une mesure de type Input Compare
 */


#include "eTimer.h"
#include "MPC5744P.h"

/*****************************************************************************/
/* Config_eTimer0_InputCapture                                               */
/* Description: configure le canal channel de eTimer0 pour une utilisation  */
/* de type Input Capture. Par défaut, capture register 1 enregistre l'instant*/
/* du front montant, capture register 2 celui du front descendant. Vitesse de */
/* comptage max = fréquence MOTC. Mesure en continue.                        */
/*****************************************************************************/
void Config_eTimer0_InputCapture(uint8_t channel) {

	ETIMER_0.CH[channel].CTRL1.B.PRISRC= 0b11000;//maximum resolution
	ETIMER_0.CH[channel].CCCTRL.B.CPT1MODE= 0x2;//capture counter by
			//rising edge of secondary input (measuresignal)
	ETIMER_0.CH[channel].CCCTRL.B.CPT2MODE= 0x1;//capture counter by
			//falling edge of secondary input (measuresignal)
	ETIMER_0.CH[channel].CCCTRL.B.CFWM= 0x2;//capture flag set as soon
			//as more than 3 values will be in FIFO
	ETIMER_0.CH[channel].CTRL1.B.LENGTH= 0x0;//continue counting to rollover
	ETIMER_0.CH[channel].CTRL1.B.ONCE= 0x0;//count repeatedly
	ETIMER_0.CH[channel].CTRL1.B.SECSRC= channel;//counter "channel" input pin
			//is use for trigger thecapturing –measuring signal is connect to this pin
	ETIMER_0.CH[channel].CTRL1.B.CNTMODE= 0x1;//count rising edge of primary source
}

/*****************************************************************************/
/* Config_eTimer1_InputCapture                                               */
/* Description: configure le canal channel de eTimer1 pour une utilisation  */
/* de type Input Capture. Par défaut, capture register 1 enregistre l'instant*/
/* du front montant, capture register 2 celui du front descendant. Vitesse de */
/* comptage max = fréquence MOTC. Mesure en continue.                        */
/*****************************************************************************/
void Config_eTimer1_InputCapture(uint8_t channel) {

	ETIMER_1.CH[channel].CTRL1.B.PRISRC= 0b11000;//maximum resolution
	ETIMER_1.CH[channel].CCCTRL.B.CPT1MODE= 0x2;//capture counter by
			//rising edge of secondary input (measuresignal)
	ETIMER_1.CH[channel].CCCTRL.B.CPT2MODE= 0x1;//capture counter by
			//falling edge of secondary input (measuresignal)
	ETIMER_1.CH[channel].CCCTRL.B.CFWM= 0x2;//capture flag set as soon
			//as more than 3 values will be in FIFO
	ETIMER_1.CH[channel].CTRL1.B.LENGTH= 0x0;//continue counting to rollover
	ETIMER_1.CH[channel].CTRL1.B.ONCE= 0x0;//count repeatedly
	ETIMER_1.CH[channel].CTRL1.B.SECSRC= channel;//counter "channel" input pin
			//is use for trigger thecapturing –measuring signal is connect to this pin
	ETIMER_1.CH[channel].CTRL1.B.CNTMODE= 0x1;//count rising edge of primary source
}

/*****************************************************************************/
/* Config_eTimer2_InputCapture                                               */
/* Description: configure le canal channel de eTimer2 pour une utilisation  */
/* de type Input Capture. Par défaut, capture register 1 enregistre l'instant*/
/* du front montant, capture register 2 celui du front descendant. Vitesse de */
/* comptage max = fréquence MOTC. Mesure en continue.                        */
/*****************************************************************************/
void Config_eTimer2_InputCapture(uint8_t channel) {

	ETIMER_2.CH[channel].CTRL1.B.PRISRC= 0b11000;//maximum resolution
	ETIMER_2.CH[channel].CCCTRL.B.CPT1MODE= 0x2;//capture counter by
			//rising edge of secondary input (measuresignal)
	ETIMER_2.CH[channel].CCCTRL.B.CPT2MODE= 0x1;//capture counter by
			//falling edge of secondary input (measuresignal)
	ETIMER_2.CH[channel].CCCTRL.B.CFWM= 0x2;//capture flag set as soon
			//as more than 3 values will be in FIFO
	ETIMER_2.CH[channel].CTRL1.B.LENGTH= 0x0;//continue counting to rollover
	ETIMER_2.CH[channel].CTRL1.B.ONCE= 0x0;//count repeatedly
	ETIMER_2.CH[channel].CTRL1.B.SECSRC= channel;//counter "channel" input pin
			//is use for trigger thecapturing –measuring signal is connect to this pin
	ETIMER_2.CH[channel].CTRL1.B.CNTMODE= 0x1;//count rising edge of primary source
}

/*****************************************************************************/
/* EnableInterrupt_eTimer0_InputCapture                                      */
/* Description: active une interruption lorsqu'un événement apparait sur le  */
/* canal channel de eTimer0, lié au registre de capture NumCapture (=1 ou 2).*/
/*****************************************************************************/
void EnableInterrupt_eTimer0_InputCapture(uint8_t channel,uint8_t NumCapture) {
	if (NumCapture = 1) {
		ETIMER_0.CH[channel].INTDMA.B.ICF1IE = 1;
		ETIMER_0.CH[channel].INTDMA.B.ICF2IE = 0;
	}
	else if (NumCapture = 2) {
		ETIMER_0.CH[channel].INTDMA.B.ICF1IE = 0;
		ETIMER_0.CH[channel].INTDMA.B.ICF2IE = 1;
	}
}

/*****************************************************************************/
/* EnableInterrupt_eTimer1_InputCapture                                      */
/* Description: active une interruption lorsqu'un événement apparait sur le  */
/* canal channel de eTimer1, lié au registre de capture NumCapture (=1 ou 2).*/
/*****************************************************************************/
void EnableInterrupt_eTimer1_InputCapture(uint8_t channel,uint8_t NumCapture) {
	if (NumCapture = 1) {
		ETIMER_1.CH[channel].INTDMA.B.ICF1IE = 1;
		ETIMER_1.CH[channel].INTDMA.B.ICF2IE = 0;
	}
	else if (NumCapture = 2) {
		ETIMER_1.CH[channel].INTDMA.B.ICF1IE = 0;
		ETIMER_1.CH[channel].INTDMA.B.ICF2IE = 1;
	}
}

/*****************************************************************************/
/* EnableInterrupt_eTimer2_InputCapture                                      */
/* Description: active une interruption lorsqu'un événement apparait sur le  */
/* canal channel de eTimer2, lié au registre de capture NumCapture (=1 ou 2).*/
/*****************************************************************************/
void EnableInterrupt_eTimer2_InputCapture(uint8_t channel,uint8_t NumCapture) {
	if (NumCapture = 1) {
		ETIMER_2.CH[channel].INTDMA.B.ICF1IE = 1;
		ETIMER_2.CH[channel].INTDMA.B.ICF2IE = 0;
	}
	else if (NumCapture = 2) {
		ETIMER_2.CH[channel].INTDMA.B.ICF1IE = 0;
		ETIMER_2.CH[channel].INTDMA.B.ICF2IE = 1;
	}
}

/*****************************************************************************/
/* Start_eTimer0_InputCapture_Count                                          */
/* Description: lance l'opération InputCapture sur le canal channel de eTimer0.*/
/*****************************************************************************/
void Start_eTimer0_InputCapture_Count(uint8_t channel) {
	ETIMER_0.CH[channel].CCCTRL.B.ARM= 0x1;  //enable/start capturing
}

/*****************************************************************************/
/* Start_eTimer1_InputCapture_Count                                          */
/* Description: lance l'opération InputCapture sur le canal channel de eTimer1.*/
/*****************************************************************************/
void Start_eTimer1_InputCapture_Count(uint8_t channel) {
	ETIMER_1.CH[channel].CCCTRL.B.ARM= 0x1;  //enable/start capturing
}

/*****************************************************************************/
/* Start_eTimer2_InputCapture_Count                                          */
/* Description: lance l'opération InputCapture sur le canal channel de eTimer2.*/
/*****************************************************************************/
void Start_eTimer2_InputCapture_Count(uint8_t channel) {
	ETIMER_2.CH[channel].CCCTRL.B.ARM= 0x1;  //enable/start capturing
}

/*****************************************************************************/
/* Get_eTimer0_InputCaptureReg1                                              */
/* Description: récupère la valeur dans le registre de InputCapture n°1 du   */
/* canal channel de eTimer0.                                                 */
/*****************************************************************************/
uint16_t Get_eTimer0_InputCaptureReg1(uint8_t channel) {
	return(ETIMER_0.CH[channel].CAPT1.R);
}

/*****************************************************************************/
/* Get_eTimer1_InputCaptureReg1                                              */
/* Description: récupère la valeur dans le registre de InputCapture n°1 du   */
/* canal channel de eTimer1.                                                 */
/*****************************************************************************/
uint16_t Get_eTimer1_InputCaptureReg1(uint8_t channel) {
	return(ETIMER_1.CH[channel].CAPT1.R);
}

/*****************************************************************************/
/* Get_eTimer2_InputCaptureReg1                                              */
/* Description: récupère la valeur dans le registre de InputCapture n°1 du   */
/* canal channel de eTimer2.                                                 */
/*****************************************************************************/
uint16_t Get_eTimer2_InputCaptureReg1(uint8_t channel) {
	return(ETIMER_2.CH[channel].CAPT1.R);
}

/*****************************************************************************/
/* Get_eTimer0_InputCaptureReg2                                              */
/* Description: récupère la valeur dans le registre de InputCapture n°2 du   */
/* canal channel de eTimer0.                                                 */
/*****************************************************************************/
uint16_t Get_eTimer0_InputCaptureReg2(uint8_t channel) {
	return(ETIMER_0.CH[channel].CAPT2.R);
}

/*****************************************************************************/
/* Get_eTimer1_InputCaptureReg2                                              */
/* Description: récupère la valeur dans le registre de InputCapture n°2 du   */
/* canal channel de eTimer1.                                                 */
/*****************************************************************************/
uint16_t Get_eTimer1_InputCaptureReg2(uint8_t channel) {
	return(ETIMER_1.CH[channel].CAPT2.R);
}

/*****************************************************************************/
/* Get_eTimer2_InputCaptureReg2                                              */
/* Description: récupère la valeur dans le registre de InputCapture n°2 du   */
/* canal channel de eTimer2.                                                 */
/*****************************************************************************/
uint16_t Get_eTimer2_InputCaptureReg2(uint8_t channel) {
	return(ETIMER_2.CH[channel].CAPT2.R);
}
