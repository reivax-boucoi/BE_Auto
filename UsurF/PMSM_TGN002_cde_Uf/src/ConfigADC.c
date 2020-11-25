/*
 * ConfigADC.c
 *
 *  Created on: 16 août 2018
 *      Author: admin_aboyer
 *
 *  Fonctions associées aux ADC (initialisation, acquisition, calibration)
 */

#include "MPC5744P.h"
#include "ConfigADC.h"

/*****************************************************************************/
/* ADC_Enter_PWDN                                                           */
/* Description:requête pour faire entrer l'ADC indiqué par ADC_Nb en         */
/* mode power down.                                                          */
/*****************************************************************************/
void ADC_Enter_PWDN(uint8_t ADC_Nb) {
	if (ADC_Nb == ADC0) {
		ADC_0.MCR.B.PWDN = 1; //requête d'entrée en mode power down
		while(ADC_0.MSR.B.ADCSTATUS != 1) {} //Attente de l'entrée en mode power down
	}
	else if (ADC_Nb == ADC1) {
		ADC_1.MCR.B.PWDN = 1; //requête d'entrée en mode power down
		while(ADC_1.MSR.B.ADCSTATUS != 1) {} //Attente de l'entrée en mode power down
	}
	else if (ADC_Nb == ADC2) {
		ADC_2.MCR.B.PWDN = 1; //requête d'entrée en mode power down
		while(ADC_2.MSR.B.ADCSTATUS != 1) {} //Attente de l'entrée en mode power down
	}
	else if (ADC_Nb == ADC3) {
		ADC_3.MCR.B.PWDN = 1; //requête d'entrée en mode power down
		while(ADC_3.MSR.B.ADCSTATUS != 1) {} //Attente de l'entrée en mode power down
	}
}

/*****************************************************************************/
/* ADC_Exit_PWDN                                                           */
/* Description:requête pour faire sortir ADC[ADC_Nb] du mode power down.            */
/*****************************************************************************/
void ADC_Exit_PWDN(uint8_t ADC_Nb) {
	if (ADC_Nb == ADC0) {
		ADC_0.MCR.B.PWDN = 0; //requête de sortie du mode power down
		while(ADC_0.MSR.B.ADCSTATUS == 1) {} //Attente de la sortie du mode power down
	}
	else if (ADC_Nb == ADC1) {
		ADC_1.MCR.B.PWDN = 0; //requête de sortie du mode power down
		while(ADC_1.MSR.B.ADCSTATUS == 1) {} //Attente de la sortie du mode power down
	}
	else if (ADC_Nb == ADC2) {
		ADC_2.MCR.B.PWDN = 0; //requête d'entrée en mode power down
		while(ADC_2.MSR.B.ADCSTATUS == 1) {} //Attente de l'entrée en mode power down
	}
	else if (ADC_Nb == ADC3) {
		ADC_3.MCR.B.PWDN = 0; //requête d'entrée en mode power down
		while(ADC_3.MSR.B.ADCSTATUS == 1) {} //Attente de l'entrée en mode power down
	}
}

/*****************************************************************************/
/* ADC_Start_NormalMode                                                           */
/* Description:requête pour lancer le mode de conversion normal (uniquement hors
 * du mode Power down.            */
/*****************************************************************************/
void ADC_Start_NormalMode(uint8_t ADC_Nb) {
		if (ADC_Nb == ADC0) {
			//if (ADC_0.MSR.B.ADCSTATUS != 1) {
				ADC_0.MCR.B.NSTART = 1;
			//}
		}
		else if (ADC_Nb == ADC1) {
			//if (ADC_1.MSR.B.ADCSTATUS != 1) {
				ADC_1.MCR.B.NSTART = 1;
			//}
		}
		else if (ADC_Nb == ADC2) {
			//if (ADC_2.MSR.B.ADCSTATUS != 1) {
				ADC_2.MCR.B.NSTART = 1;
			//}
		}
		else if (ADC_Nb == ADC3) {
			//if (ADC_3.MSR.B.ADCSTATUS != 1) {
				ADC_3.MCR.B.NSTART = 1;
			//}
		}
}

/*****************************************************************************/
/* ADC_Stop_NormalMode                                                           */
/* Description:requête pour stopper le mode de conversion normal (uniquement hors
 * du mode Power down.            */
/*****************************************************************************/
void ADC_Stop_NormalMode(uint8_t ADC_Nb) {
		if (ADC_Nb == ADC0) {
			//if (ADC_0.MSR.B.ADCSTATUS != 1) {
				ADC_0.MCR.B.NSTART = 0;
			//}
		}
		else if (ADC_Nb == ADC1) {
			//if (ADC_1.MSR.B.ADCSTATUS != 1) {
				ADC_1.MCR.B.NSTART = 0;
			//}
		}
		else if (ADC_Nb == ADC2) {
			//if (ADC_2.MSR.B.ADCSTATUS != 1) {
				ADC_2.MCR.B.NSTART = 0;
			//}
		}
		else if (ADC_Nb == ADC3) {
			//if (ADC_3.MSR.B.ADCSTATUS != 1) {
				ADC_3.MCR.B.NSTART = 0;
			//}
		}
}


/*****************************************************************************/
/* ADC_init                                                                 */
/* Description: configuration de l'ADC[ADC_Nb] (doit être fait en mode power down). */
/* Cette fonction ne gère pas le mode d'injection. 							*/
/*	Align indique si le résultat est aligné à droite ou à gauche, ConvModeNormal
 * décrit le mode de conversion (one shot ou continuous scan), TrigEn active ou
 * non un trigger ext pour le lancement de la conversion, TrigEdge la nature du front
 * de déclenchement, CTU_Trig autorise ou non la conversion déclenchée par le CTU,
 * ADCClkSel indique si la fréq clock du SAR de l'ADC est égal au bus clock ou à la moitié.
 */
/*****************************************************************************/
void ADC_init(uint8_t ADC_Nb, uint8_t Align, uint8_t ConvModeNormal, uint8_t TrigEn, uint8_t TrigEdge, uint8_t CTUTrig, uint8_t ADCClkSel) {
	//configuration de la conversion :
	if (ADC_Nb == ADC0) {
		ADC_0.MCR.B.OWREN = 0; //pas d'overwrite du résultat précédent
		ADC_0.MCR.B.WLSIDE = Align;
		ADC_0.MCR.B.MODE = ConvModeNormal;
		ADC_0.MCR.B.TRGEN = TrigEn;
		ADC_0.MCR.B.EDGE = TrigEdge;
		ADC_0.MCR.B.CTUEN = CTUTrig;
		ADC_0.MCR.B.ADCLKSEL = ADCClkSel;
	}
	else if (ADC_Nb == ADC1) {
		ADC_1.MCR.B.OWREN = 0; //pas d'overwrite du résultat précédent
		ADC_1.MCR.B.WLSIDE = Align;
		ADC_1.MCR.B.MODE = ConvModeNormal;
		ADC_1.MCR.B.TRGEN = TrigEn;
		ADC_1.MCR.B.EDGE = TrigEdge;
		ADC_1.MCR.B.CTUEN = CTUTrig;
		ADC_1.MCR.B.ADCLKSEL = ADCClkSel;
	}
	else if (ADC_Nb == ADC2) {
		ADC_2.MCR.B.OWREN = 0; //pas d'overwrite du résultat précédent
		ADC_2.MCR.B.WLSIDE = Align;
		ADC_2.MCR.B.MODE = ConvModeNormal;
		ADC_2.MCR.B.TRGEN = TrigEn;
		ADC_2.MCR.B.EDGE = TrigEdge;
		ADC_2.MCR.B.CTUEN = CTUTrig;
		ADC_2.MCR.B.ADCLKSEL = ADCClkSel;
	}
	else if (ADC_Nb == ADC3) {
		ADC_3.MCR.B.OWREN = 0; //pas d'overwrite du résultat précédent
		ADC_3.MCR.B.WLSIDE = Align;
		ADC_3.MCR.B.MODE = ConvModeNormal;
		ADC_3.MCR.B.TRGEN = TrigEn;
		ADC_3.MCR.B.EDGE = TrigEdge;
		ADC_3.MCR.B.CTUEN = CTUTrig;
		ADC_3.MCR.B.ADCLKSEL = ADCClkSel;
	}
}

/*****************************************************************************/
/* ADC_ConvTime_config                                                                 */
/* Description: configuration de la durée d'échantillonnage (au minimum 8 cycles d'horloge)
 */
/*****************************************************************************/
void ADC_ConvTime_config(uint8_t ADC_Nb, uint8_t SamplDuration) {
	if (ADC_Nb == ADC0) {
		if (SamplDuration < 8) {
			ADC_0.CTR0.B.INPSAMP = 8;
		}
		else {
			ADC_0.CTR0.B.INPSAMP = SamplDuration;
		}
	}
	else if (ADC_Nb == ADC1) {
		if (SamplDuration < 8) {
			ADC_1.CTR0.B.INPSAMP = 8;
		}
		else {
			ADC_1.CTR0.B.INPSAMP = SamplDuration;
		}
	}
	else if (ADC_Nb == ADC2) {
		if (SamplDuration < 8) {
			ADC_2.CTR0.B.INPSAMP = 8;
		}
		else {
			ADC_2.CTR0.B.INPSAMP = SamplDuration;
		}
	}
	else if (ADC_Nb == ADC3) {
		if (SamplDuration < 8) {
			ADC_3.CTR0.B.INPSAMP = 8;
		}
		else {
			ADC_3.CTR0.B.INPSAMP = SamplDuration;
		}
	}
}

/*****************************************************************************/
/* ADC_DisableAllChannel_NormalConv                                             */
/* Description: desactive tous les canaux de l'ADC [ADC_Nb]pour une conversion normal (doit être fait
 * quand aucune conversion n'est lancée).
 */
void ADC_DisableAllChannel_NormalConv(uint8_t ADC_Nb) {
	if (ADC_Nb == ADC0) {
		ADC_0.NCMR0.R = 0x00000000;
	}
	else if (ADC_Nb == ADC1) {
		ADC_1.NCMR0.R = 0x00000000;
	}
	else if (ADC_Nb == ADC2) {
		ADC_2.NCMR0.R = 0x00000000;
	}
	else if (ADC_Nb == ADC3) {
		ADC_3.NCMR0.R = 0x00000000;
	}
}

/*****************************************************************************/
/* ADC_SelectChannel_NormalConv                                             */
/* Description: autorise une conversion normal pour le canal ChannelNb du module */
/* ADC_Nb (doit être faitquand aucune conversion n'est lancée).              */
/*****************************************************************************/
void ADC_SelectChannel_NormalConv(uint8_t ADC_Nb, uint8_t ChannelNb) {

	if (ADC_Nb == ADC0) {
		switch(ChannelNb) {
		case 0 : ADC_0.NCMR0.B.CH0 = 1; break;
		case 1 : ADC_0.NCMR0.B.CH1 = 1; break;
		case 2 : ADC_0.NCMR0.B.CH2 = 1; break;
		case 3 : ADC_0.NCMR0.B.CH3 = 1; break;
		case 4 : ADC_0.NCMR0.B.CH4 = 1; break;
		case 5 : ADC_0.NCMR0.B.CH5 = 1; break;
		case 6 : ADC_0.NCMR0.B.CH6 = 1; break;
		case 7 : ADC_0.NCMR0.B.CH7 = 1; break;
		case 8 : ADC_0.NCMR0.B.CH8 = 1; break;
		case 9 : ADC_0.NCMR0.B.CH9 = 1; break;
		case 10 : ADC_0.NCMR0.B.CH10 = 1; break;
		case 11 : ADC_0.NCMR0.B.CH11 = 1; break;
		case 12 : ADC_0.NCMR0.B.CH12 = 1; break;
		case 13 : ADC_0.NCMR0.B.CH13 = 1; break;
		case 14 : ADC_0.NCMR0.B.CH14 = 1; break;
		case 15 : ADC_0.NCMR0.B.CH15 = 1; break;
		}
	}
	else if (ADC_Nb == ADC1) {
		switch(ChannelNb) {
		case 0 : ADC_1.NCMR0.B.CH0 = 1; break;
		case 1 : ADC_1.NCMR0.B.CH1 = 1; break;
		case 2 : ADC_1.NCMR0.B.CH2 = 1; break;
		case 3 : ADC_1.NCMR0.B.CH3 = 1; break;
		case 4 : ADC_1.NCMR0.B.CH4 = 1; break;
		case 5 : ADC_1.NCMR0.B.CH5 = 1; break;
		case 6 : ADC_1.NCMR0.B.CH6 = 1; break;
		case 7 : ADC_1.NCMR0.B.CH7 = 1; break;
		case 8 : ADC_1.NCMR0.B.CH8 = 1; break;
		case 9 : ADC_1.NCMR0.B.CH9 = 1; break;
		case 10 : ADC_1.NCMR0.B.CH10 = 1; break;
		case 11 : ADC_1.NCMR0.B.CH11 = 1; break;
		case 12 : ADC_1.NCMR0.B.CH12 = 1; break;
		case 13 : ADC_1.NCMR0.B.CH13 = 1; break;
		case 14 : ADC_1.NCMR0.B.CH14 = 1; break;
		case 15 : ADC_1.NCMR0.B.CH15 = 1; break;
		}
	}
	else if (ADC_Nb == ADC2) {
		switch(ChannelNb) {
		case 0 : ADC_2.NCMR0.B.CH0 = 1; break;
		case 1 : ADC_2.NCMR0.B.CH1 = 1; break;
		case 2 : ADC_2.NCMR0.B.CH2 = 1; break;
		case 3 : ADC_2.NCMR0.B.CH3 = 1; break;
		case 4 : ADC_2.NCMR0.B.CH4 = 1; break;
		case 5 : ADC_2.NCMR0.B.CH5 = 1; break;
		case 6 : ADC_2.NCMR0.B.CH6 = 1; break;
		case 7 : ADC_2.NCMR0.B.CH7 = 1; break;
		case 8 : ADC_2.NCMR0.B.CH8 = 1; break;
		case 9 : ADC_2.NCMR0.B.CH9 = 1; break;
		case 10 : ADC_2.NCMR0.B.CH10 = 1; break;
		case 11 : ADC_2.NCMR0.B.CH11 = 1; break;
		case 12 : ADC_2.NCMR0.B.CH12 = 1; break;
		case 13 : ADC_2.NCMR0.B.CH13 = 1; break;
		case 14 : ADC_2.NCMR0.B.CH14 = 1; break;
		case 15 : ADC_2.NCMR0.B.CH15 = 1; break;
		}
	}
	else if (ADC_Nb == ADC3) {
		switch(ChannelNb) {
		case 0 : ADC_3.NCMR0.B.CH0 = 1; break;
		case 1 : ADC_3.NCMR0.B.CH1 = 1; break;
		case 2 : ADC_3.NCMR0.B.CH2 = 1; break;
		case 3 : ADC_3.NCMR0.B.CH3 = 1; break;
		case 4 : ADC_3.NCMR0.B.CH4 = 1; break;
		case 5 : ADC_3.NCMR0.B.CH5 = 1; break;
		case 6 : ADC_3.NCMR0.B.CH6 = 1; break;
		case 7 : ADC_3.NCMR0.B.CH7 = 1; break;
		case 8 : ADC_3.NCMR0.B.CH8 = 1; break;
		case 9 : ADC_3.NCMR0.B.CH9 = 1; break;
		case 10 : ADC_3.NCMR0.B.CH10 = 1; break;
		case 11 : ADC_3.NCMR0.B.CH11 = 1; break;
		case 12 : ADC_3.NCMR0.B.CH12 = 1; break;
		case 13 : ADC_3.NCMR0.B.CH13 = 1; break;
		case 14 : ADC_3.NCMR0.B.CH14 = 1; break;
		case 15 : ADC_3.NCMR0.B.CH15 = 1; break;
		}
	}
}

/*****************************************************************************/
/* ADC_ISRGlobalEnable                                            */
/* Description: autorise les différents types interruptions de l'ADC (hormis celles liées
 *    au watchdog analogique). La fonction ADC1_ChannelMask_ISR permet en plus un
 *    masquage individuel des canaux.
 *    EOC = End Of Conversion, ECH = end of conversion of a chain, JEOC = end of an injected conversion,
 *    JECH = end of injection chain, ECTU = end of conversion in CTU conversion mode.
 */
/*****************************************************************************/
void ADC_ISRGlobalEnable(uint8_t ADC_Nb, uint8_t EOC, uint8_t ECH, uint8_t JEOC, uint8_t JECH, uint8_t ECTU){
	if (ADC_Nb == ADC0) {
		ADC_0.IMR.B.MSKEOC = EOC;
		ADC_0.IMR.B.MSKECH = ECH;
		ADC_0.IMR.B.MSKJEOC = JEOC;
		ADC_0.IMR.B.MSKJECH = JECH;
		ADC_0.IMR.B.MSKEOCTU = ECTU;
		//Par sécurité, on masque les interruptions individuelles de chaque entrée (démasquable
		//individuellement à l'aide de ADC_ChannelMask_ISR.
		ADC_0.CIMR0.R = 0x00000000;
	}
	else if (ADC_Nb == ADC1) {
		ADC_1.IMR.B.MSKEOC = EOC;
		ADC_1.IMR.B.MSKECH = ECH;
		ADC_1.IMR.B.MSKJEOC = JEOC;
		ADC_1.IMR.B.MSKJECH = JECH;
		ADC_1.IMR.B.MSKEOCTU = ECTU;
		//Par sécurité, on masque les interruptions individuelles de chaque entrée (démasquable
		//individuellement à l'aide de ADC_ChannelMask_ISR.
		ADC_1.CIMR0.R = 0x00000000;
	}
	else if (ADC_Nb == ADC2) {
		ADC_2.IMR.B.MSKEOC = EOC;
		ADC_2.IMR.B.MSKECH = ECH;
		ADC_2.IMR.B.MSKJEOC = JEOC;
		ADC_2.IMR.B.MSKJECH = JECH;
		ADC_2.IMR.B.MSKEOCTU = ECTU;
		//Par sécurité, on masque les interruptions individuelles de chaque entrée (démasquable
		//individuellement à l'aide de ADC_ChannelMask_ISR.
		ADC_2.CIMR0.R = 0x00000000;
	}
	else if (ADC_Nb == ADC3) {
		ADC_3.IMR.B.MSKEOC = EOC;
		ADC_3.IMR.B.MSKECH = ECH;
		ADC_3.IMR.B.MSKJEOC = JEOC;
		ADC_3.IMR.B.MSKJECH = JECH;
		ADC_3.IMR.B.MSKEOCTU = ECTU;
		//Par sécurité, on masque les interruptions individuelles de chaque entrée (démasquable
		//individuellement à l'aide de ADC3_ChannelMask_ISR.
		ADC_3.CIMR0.R = 0x00000000;
	}
}

/*****************************************************************************/
/* ADC_ISRGlobalEnable                                            */
/* Description: masquage individuel des interruptions associées à l'entrée ChannelNb d'ADC[ADC_Nb].
 * Si Mask = 0 --> interruption masquée. Si Mask = 1, interruption autorisée.
 */
/*****************************************************************************/
void ADC_ChannelMask_ISR(uint8_t ADC_Nb, uint8_t ChannelNb, uint8_t Mask) {

	if (ADC_Nb == ADC0) {
		switch(ChannelNb) {
		case 0 : ADC_0.CIMR0.B.CIM0 = Mask; break;
		case 1 : ADC_0.CIMR0.B.CIM1 = Mask; break;
		case 2 : ADC_0.CIMR0.B.CIM2 = Mask; break;
		case 3 : ADC_0.CIMR0.B.CIM3 = Mask; break;
		case 4 : ADC_0.CIMR0.B.CIM4 = Mask; break;
		case 5 : ADC_0.CIMR0.B.CIM5 = Mask; break;
		case 6 : ADC_0.CIMR0.B.CIM6 = Mask; break;
		case 7 : ADC_0.CIMR0.B.CIM7 = Mask; break;
		case 8 : ADC_0.CIMR0.B.CIM8 = Mask; break;
		case 9 : ADC_0.CIMR0.B.CIM9 = Mask; break;
		case 10 : ADC_0.CIMR0.B.CIM10 = Mask; break;
		case 11 : ADC_0.CIMR0.B.CIM11 = Mask; break;
		case 12 : ADC_0.CIMR0.B.CIM12 = Mask; break;
		case 13 : ADC_0.CIMR0.B.CIM13 = Mask; break;
		case 14 : ADC_0.CIMR0.B.CIM14 = Mask; break;
		case 15 : ADC_0.CIMR0.B.CIM15 = Mask; break;
		}
	}
	else if (ADC_Nb == ADC1) {
		switch(ChannelNb) {
		case 0 : ADC_1.CIMR0.B.CIM0 = Mask; break;
		case 1 : ADC_1.CIMR0.B.CIM1 = Mask; break;
		case 2 : ADC_1.CIMR0.B.CIM2 = Mask; break;
		case 3 : ADC_1.CIMR0.B.CIM3 = Mask; break;
		case 4 : ADC_1.CIMR0.B.CIM4 = Mask; break;
		case 5 : ADC_1.CIMR0.B.CIM5 = Mask; break;
		case 6 : ADC_1.CIMR0.B.CIM6 = Mask; break;
		case 7 : ADC_1.CIMR0.B.CIM7 = Mask; break;
		case 8 : ADC_1.CIMR0.B.CIM8 = Mask; break;
		case 9 : ADC_1.CIMR0.B.CIM9 = Mask; break;
		case 10 : ADC_1.CIMR0.B.CIM10 = Mask; break;
		case 11 : ADC_1.CIMR0.B.CIM11 = Mask; break;
		case 12 : ADC_1.CIMR0.B.CIM12 = Mask; break;
		case 13 : ADC_1.CIMR0.B.CIM13 = Mask; break;
		case 14 : ADC_1.CIMR0.B.CIM14 = Mask; break;
		case 15 : ADC_1.CIMR0.B.CIM15 = Mask; break;
		}
	}
	else if (ADC_Nb == ADC2) {
		switch(ChannelNb) {
		case 0 : ADC_2.CIMR0.B.CIM0 = Mask; break;
		case 1 : ADC_2.CIMR0.B.CIM1 = Mask; break;
		case 2 : ADC_2.CIMR0.B.CIM2 = Mask; break;
		case 3 : ADC_2.CIMR0.B.CIM3 = Mask; break;
		case 4 : ADC_2.CIMR0.B.CIM4 = Mask; break;
		case 5 : ADC_2.CIMR0.B.CIM5 = Mask; break;
		case 6 : ADC_2.CIMR0.B.CIM6 = Mask; break;
		case 7 : ADC_2.CIMR0.B.CIM7 = Mask; break;
		case 8 : ADC_2.CIMR0.B.CIM8 = Mask; break;
		case 9 : ADC_2.CIMR0.B.CIM9 = Mask; break;
		case 10 : ADC_2.CIMR0.B.CIM10 = Mask; break;
		case 11 : ADC_2.CIMR0.B.CIM11 = Mask; break;
		case 12 : ADC_2.CIMR0.B.CIM12 = Mask; break;
		case 13 : ADC_2.CIMR0.B.CIM13 = Mask; break;
		case 14 : ADC_2.CIMR0.B.CIM14 = Mask; break;
		case 15 : ADC_2.CIMR0.B.CIM15 = Mask; break;
		}
	}
	else if (ADC_Nb == ADC3) {
		switch(ChannelNb) {
		case 0 : ADC_3.CIMR0.B.CIM0 = Mask; break;
		case 1 : ADC_3.CIMR0.B.CIM1 = Mask; break;
		case 2 : ADC_3.CIMR0.B.CIM2 = Mask; break;
		case 3 : ADC_3.CIMR0.B.CIM3 = Mask; break;
		case 4 : ADC_3.CIMR0.B.CIM4 = Mask; break;
		case 5 : ADC_3.CIMR0.B.CIM5 = Mask; break;
		case 6 : ADC_3.CIMR0.B.CIM6 = Mask; break;
		case 7 : ADC_3.CIMR0.B.CIM7 = Mask; break;
		case 8 : ADC_3.CIMR0.B.CIM8 = Mask; break;
		case 9 : ADC_3.CIMR0.B.CIM9 = Mask; break;
		case 10 : ADC_3.CIMR0.B.CIM10 = Mask; break;
		case 11 : ADC_3.CIMR0.B.CIM11 = Mask; break;
		case 12 : ADC_3.CIMR0.B.CIM12 = Mask; break;
		case 13 : ADC_3.CIMR0.B.CIM13 = Mask; break;
		case 14 : ADC_3.CIMR0.B.CIM14 = Mask; break;
		case 15 : ADC_3.CIMR0.B.CIM15 = Mask; break;
		}
	}
}

/*****************************************************************************/
/* ADC_ReadChannel                                            */
/* Description: la fonction renvoie la valeur lue par l'entrée ChannelNb de l'ADC[ADC_Nb].
 * Si le résultat n'est pas valide, la fonction renvoie 0.
 */
/*****************************************************************************/
//uint16_t ADC_ReadChannel(uint8_t ADC_Nb, uint8_t ChannelNb) {
//	uint16_t result;
//
//	if (ADC_Nb == ADC0) {
//		if (ADC_0.CDR[ChannelNb].B.VALID) {
//			result = ADC_0.CDR[ChannelNb].B.CDATA; //résultat valide uniquement si config en aligné à droite.
//		}
//		else {
//			result = 0;
//		}
//	}
//	else if (ADC_Nb == ADC1) {
//		if (ADC_1.CDR[ChannelNb].B.VALID) {
//			result = ADC_1.CDR[ChannelNb].B.CDATA; //résultat valide uniquement si config en aligné à droite.
//		}
//		else {
//			result = 0;
//		}
//	}
//	else if (ADC_Nb == ADC2) {
//		if (ADC_2.CDR[ChannelNb].B.VALID) {
//			result = ADC_2.CDR[ChannelNb].B.CDATA; //résultat valide uniquement si config en aligné à droite.
//		}
//		else {
//			result = 0;
//		}
//	}
//	else if (ADC_Nb == ADC3) {
//		if (ADC_3.CDR[ChannelNb].B.VALID) {
//			result = ADC_3.CDR[ChannelNb].B.CDATA; //résultat valide uniquement si config en aligné à droite.
//		}
//		else {
//			result = 0;
//		}
//	}
//
//	return result;
//}
ADC_Result_struct ADC_ReadChannel(uint8_t ADC_Nb, uint8_t ChannelNb) {
	ADC_Result_struct result;

	if (ADC_Nb == ADC0) {
		if (ADC_0.CDR[ChannelNb].B.VALID) {
			result.ConvResult = ADC_0.CDR[ChannelNb].B.CDATA; //résultat valide uniquement si config en aligné à droite.
			result.ValidResult = 1;
		}
		else {
			result.ConvResult = 0;
			result.ValidResult = 0;
		}
	}
	else if (ADC_Nb == ADC1) {
		if (ADC_1.CDR[ChannelNb].B.VALID) {
			result.ConvResult = ADC_1.CDR[ChannelNb].B.CDATA; //résultat valide uniquement si config en aligné à droite.
			result.ValidResult = 1;
		}
		else {
			result.ConvResult = 0;
			result.ValidResult = 0;
		}
	}
	else if (ADC_Nb == ADC2) {
		if (ADC_2.CDR[ChannelNb].B.VALID) {
			result.ConvResult = ADC_2.CDR[ChannelNb].B.CDATA; //résultat valide uniquement si config en aligné à droite.
			result.ValidResult = 1;
		}
		else {
			result.ConvResult = 0;
			result.ValidResult = 0;
		}
	}
	else if (ADC_Nb == ADC3) {
		if (ADC_3.CDR[ChannelNb].B.VALID) {
			result.ConvResult = ADC_3.CDR[ChannelNb].B.CDATA; //résultat valide uniquement si config en aligné à droite.
			result.ValidResult = 1;
		}
		else {
			result.ConvResult = 0;
			result.ValidResult = 0;
		}
	}

	return result;
}

/*****************************************************************************/
/* ADC_Calibration                                           */
/* Description: la fonction effectue une test de calibration de l'ADC[ADC_Nb].      */
/* 		Les paramètres par défaut sont utilisés.                            */
/*****************************************************************************/
uint8_t ADC_Calibration(uint8_t ADC_Nb) {
	uint8_t ADC_Calibration_OK = 1;    /* Calibration has not passed yet */

	if (ADC_Nb == ADC0) {
		  ADC_0.MCR.B.PWDN = 1;     /* Power down for starting calibration process */
		  ADC_0.MCR.B.ADCLKSEL = HALF_CLK_ADC; /* ADC clock = bus clock/2 (40 MHz) */
		        /* Note: Calibration requires it to be set at max 40MHz for best results */
		  ADC_0.CALBISTREG.B.TEST_EN = 1;       /* Enable calibration test */
		  ADC_0.MCR.B.PWDN = 0;     /* Power back up for calibration test to start */
		  while(ADC_0.CALBISTREG.B.C_T_BUSY);  /* Wait for calibration to finish */
		  if(ADC_0.MSR.B.CALIBRTD) {            /* If calibration ran successfully */
		    ADC_Calibration_OK = 1;        /* Calibration was successful */
		  }
		  else  {
		    ADC_Calibration_OK = 0;        /* Calibration was not successful */
		  }
	}
	else if (ADC_Nb == ADC1) {
		  ADC_1.MCR.B.PWDN = 1;     /* Power down for starting calibration process */
		  ADC_1.MCR.B.ADCLKSEL = HALF_CLK_ADC; /* ADC clock = bus clock/2 (40 MHz) */
		        /* Note: Calibration requires it to be set at max 40MHz for best results */
		  ADC_1.CALBISTREG.B.TEST_EN = 1;       /* Enable calibration test */
		  ADC_1.MCR.B.PWDN = 0;     /* Power back up for calibration test to start */
		  while(ADC_1.CALBISTREG.B.C_T_BUSY);  /* Wait for calibration to finish */
		  if(ADC_1.MSR.B.CALIBRTD) {            /* If calibration ran successfully */
			  ADC_Calibration_OK = 1;        /* Calibration was successful */
		  }
		  else  {
			  ADC_Calibration_OK = 0;        /* Calibration was not successful */
		  }
	}
	else if (ADC_Nb == ADC2) {
		  ADC_2.MCR.B.PWDN = 1;     /* Power down for starting calibration process */
		  ADC_2.MCR.B.ADCLKSEL = HALF_CLK_ADC; /* ADC clock = bus clock/2 (40 MHz) */
		        /* Note: Calibration requires it to be set at max 40MHz for best results */
		  ADC_2.CALBISTREG.B.TEST_EN = 1;       /* Enable calibration test */
		  ADC_2.MCR.B.PWDN = 0;     /* Power back up for calibration test to start */
		  while(ADC_2.CALBISTREG.B.C_T_BUSY);  /* Wait for calibration to finish */
		  if(ADC_2.MSR.B.CALIBRTD) {            /* If calibration ran successfully */
			  ADC_Calibration_OK = 1;        /* Calibration was successful */
		  }
		  else  {
			  ADC_Calibration_OK = 0;        /* Calibration was not successful */
		  }
	}
	else if (ADC_Nb == ADC3) {
		  ADC_3.MCR.B.PWDN = 1;     /* Power down for starting calibration process */
		  ADC_3.MCR.B.ADCLKSEL = HALF_CLK_ADC; /* ADC clock = bus clock/2 (40 MHz) */
		        /* Note: Calibration requires it to be set at max 40MHz for best results */
		  ADC_3.CALBISTREG.B.TEST_EN = 1;       /* Enable calibration test */
		  ADC_3.MCR.B.PWDN = 0;     /* Power back up for calibration test to start */
		  while(ADC_3.CALBISTREG.B.C_T_BUSY);  /* Wait for calibration to finish */
		  if(ADC_3.MSR.B.CALIBRTD) {            /* If calibration ran successfully */
			  ADC_Calibration_OK = 1;       /* Calibration was successful */
		  }
		  else  {
			  ADC_Calibration_OK = 0;        /* Calibration was not successful */
		  }
	}

  return(ADC_Calibration_OK);

}

/*****************************************************************************/
/* ADC_Get_EOCTU                                          */
/* Description: la fonction renvoie le statut du bit EOCTU (end of conv launched by CTU */
/* lié à la conversion de l'ADC ADC_Nb.
/*****************************************************************************/
uint8_t ADC_Get_EOCTU(uint8_t ADC_Nb) {
	uint8_t status_EOCTU;

	if (ADC_Nb == ADC0) {
		status_EOCTU = ADC_0.ISR.B.EOCTU;
	}
	else if (ADC_Nb == ADC1) {
		status_EOCTU = ADC_1.ISR.B.EOCTU;
	}
	else if (ADC_Nb == ADC2) {
		status_EOCTU = ADC_2.ISR.B.EOCTU;
	}
	else if (ADC_Nb == ADC3) {
		status_EOCTU = ADC_3.ISR.B.EOCTU;
	}
	return(status_EOCTU);
}

/*****************************************************************************/
/* ADC_Clear_EOCTU                                          */
/* Description: la fonction met à 0 le statut du bit EOCTU (end of conv launched by CTU */
/* lié à la conversion de l'ADC ADC_Nb.
/*****************************************************************************/
void ADC_Clear_EOCTU(uint8_t ADC_Nb) {
	if (ADC_Nb == ADC0) {
		ADC_0.ISR.B.EOCTU = 1;
	}
	else if (ADC_Nb == ADC1) {
		ADC_1.ISR.B.EOCTU = 1;
	}
	else if (ADC_Nb == ADC2) {
		ADC_2.ISR.B.EOCTU = 1;
	}
	else if (ADC_Nb == ADC3) {
		ADC_3.ISR.B.EOCTU = 1;
	}
}

/*****************************************************************************/
/* ADC_Clear_All_Flags                                          */
/* Description: la fonction met à 0 tous les flags associés à  ADC ADC_Nb.
/*****************************************************************************/
void ADC_Clear_All_Flags(uint8_t ADC_Nb) {
	if (ADC_Nb == ADC0) {
		ADC_0.ISR.R = 0x0000001F; //raz des flags de l'ADC0
	}
	else if (ADC_Nb == ADC1) {
		ADC_1.ISR.R = 0x0000001F; //raz des flags de l'ADC1
	}
	else if (ADC_Nb == ADC2) {
		ADC_2.ISR.R = 0x0000001F; //raz des flags de l'ADC2
	}
	else if (ADC_Nb == ADC3) {
		ADC_3.ISR.R = 0x0000001F; //raz des flags de l'ADC3
	}
}
