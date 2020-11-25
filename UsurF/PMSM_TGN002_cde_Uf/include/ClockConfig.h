/*
 * ClockConfig.h
 *
 *  Created on: 4 juil. 2018
 *      Author: admin_aboyer
 *
 *      Configuration des différentes horloges (sources, activation des bus d'horloge)
 */

#ifndef CLOCKCONFIG_H_
#define CLOCKCONFIG_H_

//system clock source
#define IRCSYSCLK 0
#define XOSCSYSCLK 1
#define PLL0SYSCLK 2 //primary PLL
#define PLL1SYSCLK 4 //secondary PLL

//auxiliary clock 0 source
#define IRCAUX0CLK 0
#define XOSCAUX0CLK 1
#define PLL0AUX0CLK 2 //primary PLL
#define PLL1AUX0CLK 4 //secondary PLL

//PLL0 and PLL1 clock sources
#define PLL0_SRC_IRC 0
#define PLL0_SRC_XOSC 1
#define PLL1_SRC_XOSC 1
#define PLL1_SRC_PLL0 3

#define IRC_OFF 0
#define IRC_ON 1
#define XOSC_OFF 0
#define XOSC_ON 1
#define PLL0_OFF 0
#define PLL0_ON 1
#define PLL1_OFF 0
#define PLL1_ON 1

//définition des fréquences utilisées dans le MCU
#define F_SYS 200000000 //fréquence de l'horloge système (exprimée en Hz)
#define F_SYS_MHz 200 //fréquence de l'horloge système (exprimée en MHz)
#define F_SYS_KHz 200000 //fréquence de l'horloge système (exprimée en kHz)
#define F_BRIDGE 50000000 //fréquence de l'horloge PBRIDGE (exprimée en Hz)
#define F_BRIDGE_MHz 50 //fréquence de l'horloge PBRIDGE(exprimée en MHz)
#define F_BRIDGE_KHz 50000 //fréquence de l'horloge PBRIDGE (exprimée en kHz)
#define F_MOTC 40000000 //fréquence de l'horloge MOTC (exprimée en Hz)
#define F_MOTC_MHz 40 //fréquence de l'horloge MOTC (exprimée en MHz)
#define F_MOTC_KHz 40000 //fréquence de l'horloge MOTC (exprimée en kHz)
#define LIN_CLK 100000000 //horloge LIN = HALFSYS_clk = 1/2*SYS_clk = 200M/2 = 100 MHz
#define FLEXPWM_CLK 10000000 //horloge de la flexPWM (FMOTC/ prescaler (= 4) = 10 MHz)
#define FLEXPWM_CLK_MHz 10 //horloge de la flexPWM (FMOTC/ prescaler (= 4) = 10 MHz)
#define F_CTU_MHz 10 //fréquence de comptage du CTU (pour le réglage du délai entre le trigger
	//et l'instant d'acqusition par l'ADC) = F_MOTC/prescaler = FMOTC/4


#include "typedefs.h"

void ConfigIRC(uint8_t outIRC, uint8_t divCLKOUT);
void ConfigXOSC(void);
void ConfigPLL0(uint8_t Source, uint8_t PREDIV, uint8_t MFD, uint8_t RFDPHI, uint8_t RFDPHI1);
void ConfigPLL1(uint8_t Source, uint8_t MFD, uint8_t RFDPHI);
void clock_out_FIRC(uint8_t div);
void DivideSystClock(uint8_t div);
void ConfigADCClock(uint8_t source, uint8_t div);
void ConfigMotCClock(uint8_t source, uint8_t div);

#endif /* CLOCKCONFIG_H_ */
