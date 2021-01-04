/*
 * ConfigGPIO.c
 *
 *  Created on: 4 juil. 2018
 *      Author: admin_aboyer
 *      Configuration des pads de GPIO
 *
 *  Configuration des GPIO (entrée, sortie, fonctions spéciales, type de pad ...) + lecture/écriture
 */

#include "MPC5744P.h"
#include "ConfigGPIO.h"

/*****************************************************************************/
/* Set_pad_out                                                         */
/* Description: Configure l'état logique d'un pad de sortie
/*****************************************************************************/
void Set_pad_out(uint8_t padNb, uint8_t State) {
	SIUL2.GPDO[padNb].B.PDO = State;
}

/*****************************************************************************/
/* Set_PORTA                                                         */
/* Description: Configure l'état logique de sortie du port A (16 broches)
/*****************************************************************************/
void Set_PORTA(uint16_t OutWord) {
	SIUL2.PGPDO[0].R = 0x0000FFFF & OutWord;
}

/*****************************************************************************/
/* Set_pad_in                                                         */
/* Description: Lit l'état logique d'un pad d'entrée */
/*****************************************************************************/
uint8_t Read_pad_in(uint8_t padNb) {
	uint8_t pad_state;
	pad_state = SIUL2.GPDI[padNb].B.PDI;
	return(pad_state);
}

/*****************************************************************************/
/* Config_pad_out                                                         */
/* Description: Configure un pad en sortie. On indique le numéro de la broche et les options du pad.
 * On peut aussi fixer l'état logique du pad de sortie (OutState).        */
/* Voir chap 16 - SIUL2, p 497 MPC5744PRM.pdf pour config registre MSCR.
/* Voir table 4.7 p 107 ou 4.16 p 151 MPC5744PRM.pdf pour trouver le numéro du registre
 * MSCR associé à chaque pad. */
/*****************************************************************************/
void Config_pad_out(uint8_t padNb, uint8_t SlewRateAndDrive, uint8_t OpenDrain, uint8_t Analog, uint8_t PullEn, uint8_t PullSel, uint8_t OutState) {
	SIUL2.MSCR[padNb].B.OBE = 1; //buffer de sortie activé
	SIUL2.MSCR[padNb].B.IBE = 0; //buffer d'entrée désactivé
	SIUL2.MSCR[padNb].B.SSS = 0;  //default SSS value --> GPIO
	SIUL2.MSCR[padNb].B.SRC = SlewRateAndDrive;
	SIUL2.MSCR[padNb].B.ODE = OpenDrain;
	SIUL2.MSCR[padNb].B.APC = Analog;
	SIUL2.MSCR[padNb].B.PUE = PullEn;
	SIUL2.MSCR[padNb].B.PUS = PullSel;
	Set_pad_out(padNb,OutState);
}

/*****************************************************************************/
/* Config_pad_in                                                         */
/* Description: Configure un pad en entrée. On indique le numéro de la broche et les options du pad.
/* Voir chap 16 - SIUL2, p 497 MPC5744PRM.pdf pour config registre MSCR.
/* Voir table 4.7 p 107 ou 4.16 p 151 MPC5744PRM.pdf pour trouver le numéro du registre
 * MSCR associé à chaque pad. */
/*****************************************************************************/
void Config_pad_in(uint8_t padNb, uint8_t PullEn, uint8_t PullSel, uint8_t Hyst_Sel) {
	SIUL2.MSCR[padNb].B.OBE = 0; //buffer de sortie désactivé
	SIUL2.MSCR[padNb].B.IBE = 1; //buffer d'entrée activé
	SIUL2.MSCR[padNb].B.SSS = 0;  //default SSS value --> GPIO
	SIUL2.MSCR[padNb].B.HYS  = Hyst_Sel;
	SIUL2.MSCR[padNb].B.PUE = PullEn;
	SIUL2.MSCR[padNb].B.PUS = PullSel;
}


/*****************************************************************************/
/* Config_analog_pad                                                         */
/* Description: Configure un pad en entrée analogique.
 * On indique le numéro de la broche concerné.
/* Voir chap 16 - SIUL2, p 497 MPC5744PRM.pdf pour config registre MSCR.
/* Voir table 4.7 p 107 ou 4.16 p 151 MPC5744PRM.pdf pour trouver le numéro du registre
 * MSCR associé à chaque pad. */
/*****************************************************************************/
void Config_analog_pad(uint8_t padNb) {
	SIUL2.MSCR[padNb].B.SSS = 0;  //default SSS value --> GPIO et entrée analogique
	SIUL2.MSCR[padNb].B.APC = 1;
}

/*****************************************************************************/
/* Config_Alter_Function_Out                                                         */
/* Description: Configure la fonction alternative d'un pad de sortie. */
/* On indique le numéro de la broche (padNb) et le numéro de la fonction alternative (FunctionNb) */
/*****************************************************************************/
void Config_Alter_Function_Out(uint8_t padNb, uint8_t FunctionNb) {
	if ((FunctionNb >= 0) || (FunctionNb < 16)) {
		SIUL2.MSCR[padNb].B.SSS = FunctionNb;
	}
	else {
		SIUL2.MSCR[padNb].B.SSS = 0;
	}
}

/*****************************************************************************/
/* Config_Alter_Function_In                                                   */
/* Description: Configure la fonction alternative d'un pad d'entrée.         */
/* On indique le numéro du registre IMCR (IMCRNb) et le numéro de la fonction alternative (FunctionNb) */
/*****************************************************************************/
void Config_Alter_Function_In(uint8_t IMCRNb, uint8_t FunctionNb) {
	if ((FunctionNb >= 0) || (FunctionNb < 16)) {
		SIUL2.IMCR[IMCRNb].B.SSS = FunctionNb;
	}
	else {
		SIUL2.IMCR[IMCRNb].B.SSS = 0;
	}
}


/*****************************************************************************/
/* Config_ERQ_Pad                                                   */
/* Description: Configure un pad d'entrée de type ERQ (external interrupt request) pour qu'il
 * déclenche un ISR sur événement.         */
/* On indique le numéro de la pin IRQ (0 à 31) (IRQ_pin_Nb),
 * le type d'événement qui déclenche l'ISR (Rising ou Falling edge) et l'autorisation d'un filtre
 * anti-glitch (FilterEnable). */
/*****************************************************************************/
void Config_ERQ_Pad(uint8_t IRQ_pin_Nb, uint8_t RisingEvent, uint8_t FallingEvent, uint8_t FilterEnable) {
	uint32_t IRQ_pin_Sel;

	IRQ_pin_Sel = 0x00000001 << (IRQ_pin_Nb);
	SIUL2.DIRER0.R = SIUL2.DIRER0.R | IRQ_pin_Sel;
	SIUL2.DIRSR0.R = 0; //mettre un bit à '1' dans ce registre est réservé !
	if (RisingEvent) {
		SIUL2.IREER0.R = SIUL2.IREER0.R | IRQ_pin_Sel;
	}
	else {
		SIUL2.IREER0.R = SIUL2.IREER0.R & (~IRQ_pin_Sel);
	}

	if (FallingEvent) {
		SIUL2.IFEER0.R = SIUL2.IFEER0.R | IRQ_pin_Sel;
	}
	else {
		SIUL2.IFEER0.R = SIUL2.IFEER0.R & (~IRQ_pin_Sel);
	}

	if (FilterEnable) {
		SIUL2.IFER0.R = SIUL2.IFER0.R | IRQ_pin_Sel;
	}
	else {
		SIUL2.IFER0.R = SIUL2.IFER0.R & (~IRQ_pin_Sel);
	}
}



