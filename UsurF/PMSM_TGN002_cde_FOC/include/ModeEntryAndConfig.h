/*
 * ModeEntryAndConfig.h
 *
 *  Created on: 4 juil. 2018
 *      Author: admin_aboyer
 */

#ifndef MODEENTRYANDCONFIG_H_
#define MODEENTRYANDCONFIG_H_

#define RSTMODE 0
#define TESTMODE 1
#define SAFEMODE 2
#define DRUNMODE 3
#define RUN0MODE 4
#define RUN1MODE 5
#define RUN2MODE 6
#define RUN3MODE 7
#define HALT0MODE 8
#define STOP0MODE 10


#define RUN1_OFF 0
#define RUN1_ON 1
#define RUN2_OFF 0
#define RUN2_ON 1
#define RUN3_OFF 0
#define RUN3_ON 1
#define HALT0_OFF 0
#define HALT0_ON 1
#define TEST_OFF 0
#define TEST_ON 1



void peri_clock_gating(void);
void ModeConfig(uint8_t mode, uint8_t SysClk, uint8_t IRCActiv, uint8_t XoscActiv, uint8_t PLL0Activ, uint8_t PLL1Activ);
void ModeEnable(uint8_t Run1, uint8_t Run2, uint8_t Run3, uint8_t Halt0, uint8_t Test);
void ModeTransition(uint8_t TargetMode);

#endif /* MODEENTRYANDCONFIG_H_ */
