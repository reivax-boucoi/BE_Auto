/*
 * ConfigPIT.h
 *
 *  Created on: 5 juil. 2018
 *      Author: admin_aboyer
 *      Configuration des Periodic Interrupt Timer
 */

#ifndef CONFIGPIT_H_
#define CONFIGPIT_H_

#define TIMER_GENERAL 0 //le canal 0 du module PIT_0 est utilisé comme timer général
		//pour attendre des durées données. Ne pas l'utilisez pour cadencer des interruptions.
#define TIMER1 1
#define TIMER2 2
#define TIMER3 3

void EnablePITModule(void);
void DisablePITModule(void);
void FreezeDBGPITModule(void);
void UnFreezeDBGPITModule(void);
void PIT_ch_Init(uint32_t CanalCh, uint32_t TimeOut);
void startPIT_ch(uint32_t CanalCh);
void stopPIT_ch(uint32_t CanalCh);
void DelayTimerGeneral_us(uint32_t Duree_us);
void DelayTimerGeneral_ms(uint32_t Duree_us);

#endif /* CONFIGPIT_H_ */
