/*
 * ModeEntryAndConfig.c
 *
 *  Created on: 4 juil. 2018
 *      Author: admin_aboyer
 *  Gestion des modes de fonctionnement du microcontrolleur et des périph, changement de mode
 */

#include "MPC5744P.h"
#include "ModeEntryAndConfig.h"
#include "ClockConfig.h"


/*****************************************************************************/
/* peri_clock_gating                                                         */
/* Description: Configures enabling clocks to peri modules or gating them off*/
/*              Default PCTL[RUN_CFG]=0, so by default RUN_PC[0] is selected.*/
/*              RUN_PC[0] is configured here to gate off all clocks.         */
/*****************************************************************************/
void peri_clock_gating(void) {
	//config des 8 registres de config de comportement des périph (on ne se servira
	//que de la config 1). Comportement au sens clock gating en fonction du mode de fct.
	//Voir chap 59, p 2348 MPC5744PRM.pdf.


	MC_ME.RUN_PC[0].R = 0x00000000;  /* gate off clock for all RUN modes */
	MC_ME.RUN_PC[1].R = 0x000000FE;  /* config. peri clock for all RUN modes */
	MC_ME.RUN_PC[2].R = 0x00000000;  /* gate off clock for all RUN modes */
	MC_ME.RUN_PC[3].R = 0x00000000;  /* gate off clock for all RUN modes */
	MC_ME.RUN_PC[4].R = 0x00000000;  /* gate off clock for all RUN modes */
	MC_ME.RUN_PC[5].R = 0x00000000;  /* gate off clock for all RUN modes */
	MC_ME.RUN_PC[6].R = 0x00000000;  /* gate off clock for all RUN modes */
	MC_ME.RUN_PC[7].R = 0x00000000;  /* gate off clock for all RUN modes */

	//choix config du comportement des périph. Voir chap 59, p 2348 MPC5744PRM.pdf.
	//pour la liste des registres PCTL, voir la liste des registres du module Mode Entry (p. 2291).
	MC_ME.PCTL99.B.RUN_CFG = 0b001; //PCTL99 is DSPI0 Peripheral Control Registers for Panther, choix config 1.
	MC_ME.PCTL30.B.RUN_CFG = 0b001; //PCTL30 is PIT0 Peripheral Control Registers for Panther, choix config 1.
	MC_ME.PCTL91.B.RUN_CFG = 0b001; //PCTL91 is LINFLEX1 Peripheral Control Registers for Panther, choix config 1.
	MC_ME.PCTL255.B.RUN_CFG = 0b001; //PCTL255 is FlexPWM0 Peripheral Control Registers for Panther, choix config 1.
	MC_ME.PCTL209.B.RUN_CFG = 0b001; //PCTL209 is DSPI2 Peripheral Control Registers for Panther
									//choix config 1.
	MC_ME.PCTL237.B.RUN_CFG = 0b001; //PCTL237 is ADC0 Peripheral Control Registers for Panther, choix config 1.
	MC_ME.PCTL126.B.RUN_CFG = 0b001; //PCTL126 is ADC1 Peripheral Control Registers for Panther, choix config 1.
	MC_ME.PCTL251.B.RUN_CFG = 0b001; //PCTL251 is CTU0 Peripheral Control Registers for Panther
							//choix config 1.
	MC_ME.PCTL245.B.RUN_CFG= 0b001; ///PCTL245 is eTimer2 Peripheral Control Registers for Panther
							//choix config 1
}

/*****************************************************************************/
/* ModeConfig                                                         */
/* Description: configuration des différents modes (clock source, allumage des différentes
 * sources de clocks         */
/*****************************************************************************/
void ModeConfig(uint8_t mode, uint8_t SysClk, uint8_t IRCActiv, uint8_t XoscActiv, uint8_t PLL0Activ, uint8_t PLL1Activ) {
	if (mode == RSTMODE) {
		//tous les bits de MC_ME_RESET_MC sont en mode lecture
	}
	if (mode == TESTMODE) {
		//on active d'abord les différentes horloges avant de sélectionner l'horloge système
		MC_ME.TEST_MC.B.IRCON = IRCActiv;
		MC_ME.TEST_MC.B.XOSCON = XoscActiv;
		MC_ME.TEST_MC.B.PLL0ON = PLL0Activ;
		MC_ME.TEST_MC.B.PLL1ON = PLL1Activ;
		//sélection horloge système
		MC_ME.TEST_MC.B.SYSCLK = SysClk;
	}

	if (mode == SAFEMODE) {
		//on active d'abord les différentes horloges avant de sélectionner l'horloge système
		MC_ME.SAFE_MC.B.IRCON = IRCActiv;
		MC_ME.SAFE_MC.B.XOSCON = XoscActiv;
		MC_ME.SAFE_MC.B.PLL0ON = PLL0Activ;
		MC_ME.SAFE_MC.B.PLL1ON = PLL1Activ;
		//sélection horloge système
		MC_ME.SAFE_MC.B.SYSCLK = SysClk;
	}

	if (mode == DRUNMODE) {
		//on active d'abord les différentes horloges avant de sélectionner l'horloge système
		MC_ME.DRUN_MC.B.IRCON = IRCActiv;
		MC_ME.DRUN_MC.B.XOSCON = XoscActiv;
		MC_ME.DRUN_MC.B.PLL0ON = PLL0Activ;
		MC_ME.DRUN_MC.B.PLL1ON = PLL1Activ;
		//sélection horloge système
		MC_ME.DRUN_MC.B.SYSCLK = SysClk;
		}

	if (mode == RUN1MODE) {
		//on active d'abord les différentes horloges avant de sélectionner l'horloge système
		MC_ME.RUN1_MC.B.IRCON = IRCActiv;
		MC_ME.RUN1_MC.B.XOSCON = XoscActiv;
		MC_ME.RUN1_MC.B.PLL0ON = PLL0Activ;
		MC_ME.RUN1_MC.B.PLL1ON = PLL1Activ;
		//sélection horloge système
		MC_ME.RUN1_MC.B.SYSCLK = SysClk;
	}
}

/*****************************************************************************/
/* ModeEnable                                                        */
/* Description: la fonction permet d'activer les différents modes voulus         */
/*	Seuls les modes Run1, Run2, Run3, Halt0 et Test peuvent être désactivés  */
/*****************************************************************************/
void ModeEnable(uint8_t Run1, uint8_t Run2, uint8_t Run3, uint8_t Halt0, uint8_t Test) {
	MC_ME.ME.B.RUN1 = Run1;
	MC_ME.ME.B.RUN2 = Run2;
	MC_ME.ME.B.RUN3 = Run3;
	MC_ME.ME.B.HALT0 = Halt0;
	MC_ME.ME.B.TEST = Test;
}

/*****************************************************************************/
/* ModeTransition                                                       */
/* Description: la fonction permet de faire la transition vers un nouveau mode indiqué par TargetMode
 * Attention à vérifier au préalable que cette transition est possible
 */
/*****************************************************************************/
void ModeTransition(uint8_t TargetMode) {
	uint32_t MyTarget;

	MyTarget = TargetMode << 28;
	MC_ME.MCTL.R = MyTarget | 0x00005AF0; //key
	MC_ME.MCTL.R = MyTarget | 0x0000A50F; //inverted key


	while(MC_ME.GS.B.S_MTRANS == 1);      /* Wait for mode transition complete */

	while(MC_ME.GS.B.S_CURRENT_MODE != TargetMode) {} /* Verify the current mode */
		                                  /* Note: This verification ensures a SAFE mode */
		                                  /*       tranistion did not occur. SW could instead */
		                                  /*       enable the safe mode tranision interupt */
}


