/*
 * ClockConfig.c
 *
 *  Created on: 4 juil. 2018
 *      Author: admin_aboyer
 *
 *  Configuration des différentes horloges (sources, activation des bus d'horloge)
 */

#include "ClockConfig.h"
#include "ConfigGPIO.h"
#include "MPC5744P.h"

/*****************************************************************************/
/* ConfigIRC                                                                 */
/* Description: l'oscillateur interne IRC est toujours activé                */
/*      on définit si cette horloge sort ou pas sur la broche CLKOUT         */
/*****************************************************************************/
void ConfigIRC(uint8_t outIRC, uint8_t divCLKOUT) {
	if (outIRC == 1)  {
		clock_out_FIRC(divCLKOUT);           /* Pad PB6 = CLOCKOUT = FIRC / 10 */
	}
}

/*****************************************************************************/
/* ConfigXOSC                                                                 */
/* Description:        l'oscillateur n'est pas utilisé en mode Bypass        */
/*****************************************************************************/
void ConfigXOSC(void) {
	XOSC.CTL.B.OSCBYP = 0;
	XOSC.CTL.B.EOCV = 1;
}

/*****************************************************************************/
/* ConfigPLL0                                                                 */
/* Description: configuration de la PLL0.                                     */
/*    La fonction sélectionne la clock source, les facteurs de divisions/prescaler
 *    Par contre, elle n'active pas la PLL. Cela est fait dans la config du
 *    mode de fonctionnement. Les configurations des PLL doivent être faites
 *    uniquement lorsqu'elles sont en mode low power --> inactives !
 */
/*****************************************************************************/
void ConfigPLL0(uint8_t Source, uint8_t PREDIV, uint8_t MFD, uint8_t RFDPHI, uint8_t RFDPHI1) {
	if ((Source == 0) || (Source == 1)) {
		MC_CGM.AC3_SC.B.SELCTL = Source; //0 = IRC, 1 = XOSC
	}
	else {
		MC_CGM.AC3_SC.B.SELCTL = 0; //valeur par défaut
	}

	/* Configure PLL parameters */
	/* VCO range must be within 600-1250 MHz */
	/* PHI must be within 4.76-200 MHz */
	/* PHI1 must be within 20-156 MHz */
	/* PREDIV must be within 1-7 */
	/* MFD must be within 8-127 */
	/* RFDPHI must be within 1-63 */
	/* RFDPHI1 must be within 4-15 */
	  PLLDIG.PLL0DV.B.RFDPHI1 = RFDPHI1;
	  PLLDIG.PLL0DV.B.RFDPHI = RFDPHI;
	  PLLDIG.PLL0DV.B.PREDIV  = PREDIV;
	  PLLDIG.PLL0DV.B.MFD = MFD;



}

/*****************************************************************************/
/* ConfigPLL1                                                                 */
/* Description: configuration de la PLL1 (sans diviseur fractionnaire).
 *    La fonction sélectionne la clock source, les facteurs de division
 *    Par contre, elle n'active pas la PLL. Cela est fait dans la config du
 *    mode de fonctionnement. Les configurations des PLL doivent être faites
 *    uniquement lorsqu'elles sont en mode low power --> inactives !
 */
/*****************************************************************************/
void ConfigPLL1(uint8_t Source, uint8_t MFD, uint8_t RFDPHI) {
	if ((Source == 1) || (Source == 3)) {
		MC_CGM.AC3_SC.B.SELCTL = Source; //1 = XOSC, 3 = PLL0_PHI1
	}
	else {
		MC_CGM.AC3_SC.B.SELCTL = 1; //valeur par défaut
	}

	PLLDIG.PLL1FD.B.FDEN = 0; //divieur fractionnaire desactivé.

	/* Configure PLL parameters */
	/* clock input must be in the range 38 - 78 MHz */
	/* VCO range must be within 600-1250 MHz */
	/* PHI must be within 4.76-200 MHz */
	/* MFD must be within 16-34 */
	/* RFDPHI must be within 1-63 */
	  PLLDIG.PLL1DV.B.RFDPHI = RFDPHI;
	  PLLDIG.PLL1DV.B.MFD = MFD;
}


/*****************************************************************************/
/* clock_out_FIRC                                                                 */
/* Description: on indique si on fait sortir la sortie de l'osc IRC sur la broche CLKOUT
 *     cle signal sorti sur CLKOUT est une version divisée de la clock auxiliaire 6
 *     Voir chap 27.4.2 (p 829 MPC5744PRM.pdf)
 */
/*****************************************************************************/
void clock_out_FIRC(uint8_t div)
{
 MC_CGM.AC6_SC.B.SELCTL = 0;            /* Select FIRC as clock source for aux. clock 6*/
 if ((div > 0) && (div < 128)) {
	 MC_CGM.AC6_DC0.B.DE    = 1;            /* Enable AC6 divider 0 */
	 MC_CGM.AC6_DC0.B.DIV   = div;            /* Divide by 10 */
 }
 else
 {
	 MC_CGM.AC6_DC0.B.DE    = 0; /* disable AC6 divider 0 */
 }

 /* Configure Pin for Clock out 0 on PB6 */
 SIUL2.MSCR[PB6].R = 0x02000001;        /* PB6 = 22 */
}

/*****************************************************************************/
/* DivideSystClock                                                                 */
/* Description: division de la clock système PBRIDGEx_CLK  */
/*****************************************************************************/
void DivideSystClock(uint8_t div) {

	//division de la clock system --> AIPS
	if ((div > 0) && (div < 64)) {
		MC_CGM.SC_DC0.B.DE    = 1;            /* Enable system clock divider*/
		MC_CGM.SC_DC0.B.DIV   = div;
	 }
	 else
	 {
		 MC_CGM.SC_DC0.B.DE    = 0; /* disable AC6 divider 0 */
	 }
}

/*****************************************************************************/
/* ConfigADCClock                                                                 */
/* Description: Configuration de la clock ADC_clk                             */
/* MUX = AUX clock selector 0 - divider 2, division (div) possible de 1 à 32. */
/* 4 sources possibles : IRC, XOSC, PLL0, PLL1  */
/*****************************************************************************/
void ConfigADCClock(uint8_t source, uint8_t div) {

	if ((div > 0) && (div < 32)) {
		MC_CGM.AC0_SC.B.SELCTL = source;    // Select PLL0 for auxiliary clock 0
		MC_CGM.AC0_DC2.B.DE    = 1;            /* Enable system clock divider*/
		MC_CGM.AC0_DC2.B.DIV   = div;            /* Divide by div+1 */
	 }
	 else
	 {
		 MC_CGM.AC0_DC2.B.DE    = 0; /* disable divider*/
	 }
}

/*****************************************************************************/
/* ConfigMotCClock                                                                 */
/* Description: Configuration de la clock MOTC (FlexPWM, eTimer,SGEN,CTU)    */
/* MUX = AUX clock selector 0 - divider 0, division (div) possible de 1 à 16. */
/* 4 sources possibles : IRC, XOSC, PLL0, PLL1  */
/*****************************************************************************/
void ConfigMotCClock(uint8_t source, uint8_t div) {

	if ((div > 0) && (div < 16)) {
		MC_CGM.AC0_SC.B.SELCTL = source;    // Select PLL0 for auxiliary clock 0
		MC_CGM.AC0_DC0.B.DE    = 1;            /* Enable system clock divider*/
		MC_CGM.AC0_DC0.B.DIV   = div;            /* Divide by div+1 */
	 }
	 else
	 {
		 MC_CGM.AC0_DC0.B.DE    = 0; /* disable divider*/
	 }
}
