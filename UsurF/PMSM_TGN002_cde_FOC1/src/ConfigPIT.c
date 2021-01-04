/*
 * ConfigPIT.c
 *
 *  Created on: 5 juil. 2018
 *      Author: admin_aboyer
 *      Configuration des Periodic Interrupt Timer (chap 44 MPC5744PRM.pdf) */
/*****************************************************************************/
/* EnablePITModule                                                         */
/* Description: Active le module PIT (PIT_0 est utilisé comme Timer général)	*/

#include "MPC5744P.h"
#include "ConfigPIT.h"
#include "ClockConfig.h"  //contient la définition des fréquences d'horloge.


void EnablePITModule(void) {
	PIT_0.MCR.B.MDIS = 0; /* Enable PIT module. NOTE: PIT module must be       */
		                        /* enabled BEFORE writing to its registers.         */
		                        /* Other cores will write to PIT registers so the    */
		                        /* PIT is enabled here before starting other cores.  */
}

/*****************************************************************************/
/* DisablePITModule                                                         */
/* Description: Active le module PIT (PIT_0)								*/
void DisablePITModule(void) {
	PIT_0.MCR.B.MDIS = 1; /* Disable PIT module.                           */
}

/*****************************************************************************/
/* FreezeDBGPITModule                                                         */
/* Description: Freeze PIT module in debug mode								*/
void FreezeDBGPITModule(void) {
	PIT_0.MCR.B.FRZ = 1;
}

/*****************************************************************************/
/* UnFreezeDBGPITModule                                                         */
/* Description: UnFreeze PIT module	in debug mode							*/
void UnFreezeDBGPITModule(void) {
	PIT_0.MCR.B.FRZ = 0;
}

/*****************************************************************************/
/* PIT_ch_Init                                                         */
/* Description: Initialise le canal CanalCh du module Timer							*/
/*	fixe la période de débordement et active l'interruption liée au débordement */
void PIT_ch_Init(uint32_t CanalCh, uint32_t TimeOut) {
	PIT_0.TIMER[CanalCh].LDVAL.R = TimeOut; /* Load number of PIT clocks to count */
	PIT_0.TIMER[CanalCh].TCTRL.B.TIE = 1; /* Enable interrupt */
	PIT_0.TIMER[CanalCh].TCTRL.B.CHN = 0; //le timer n'est pas chainé avec les timers précédents
}


/*****************************************************************************/
/* startPIT_ch                                                        */
/* Description: lance le canal CanalCh du module Timer							*/
void startPIT_ch(uint32_t CanalCh) {
	PIT_0.TIMER[CanalCh].TCTRL.B.TEN = 1; //Enable channel CanalCh of PIT
}

/*****************************************************************************/
/* startPIT_ch                                                        */
/* Description: suspend le canal CanalCh du module Timer							*/
void stopPIT_ch(uint32_t CanalCh) {
	PIT_0.TIMER[CanalCh].TCTRL.B.TEN = 0; //Disable channel CanalCh of PIT
}

/*****************************************************************************/
/* DelayTimerGeneral_us                                                       */
/* Description: cette fonction génère une attente = Duree_us en µs.							*/
/* Le Timer général s'appuie sur le canal 0 du PIT.                                           */
void DelayTimerGeneral_us(uint32_t Duree_us) {
	uint32_t TimeOutTimer0;

	TimeOutTimer0 = Duree_us*F_BRIDGE_MHz;
	PIT_ch_Init(TIMER_GENERAL,TimeOutTimer0);
	PIT_0.TIMER[TIMER_GENERAL].TCTRL.B.TIE = 0; /* disable interrupt */
	//lancement du timer 0
	startPIT_ch(TIMER_GENERAL);
	//PIT_0.MCR.B.FRZ = 0; //Unfreeze timers

	//Attente débordement
	while (PIT_0.TIMER[TIMER_GENERAL].TFLG.B.TIF == 0) {}

	//RAZ flag et stop timer
	PIT_0.TIMER[TIMER_GENERAL].TFLG.B.TIF = 1;
	stopPIT_ch(TIMER_GENERAL);
	//PIT_0.MCR.B.FRZ = 1;
}

/*****************************************************************************/
/* DelayTimerGeneral_ms                                                       */
/* Description: cette fonction génère une attente = Duree_ms en ms.							*/
/* Le Timer général  s'appuie sur le canal 0 du PIT.                                           */
void DelayTimerGeneral_ms(uint32_t Duree_ms) {
	uint32_t TimeOutTimer0;

	TimeOutTimer0 = Duree_ms*F_BRIDGE_KHz;
	PIT_ch_Init(TIMER_GENERAL,TimeOutTimer0);
	PIT_0.TIMER[TIMER_GENERAL].TCTRL.B.TIE = 0; /* disable interrupt */
	//lancement du timer 0
	startPIT_ch(TIMER_GENERAL);
	//PIT_0.MCR.B.FRZ = 0; //Unfreeze timers

	//Attente débordement
	while (PIT_0.TIMER[TIMER_GENERAL].TFLG.B.TIF == 0) {}

	//RAZ flag et stop timer
	PIT_0.TIMER[TIMER_GENERAL].TFLG.B.TIF = 1;
	stopPIT_ch(TIMER_GENERAL);
	//PIT_0.MCR.B.FRZ = 1;
}
