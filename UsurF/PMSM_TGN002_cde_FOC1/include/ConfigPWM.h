/*
 * ConfigPWM.h
 *
 *  Created on: 16 août 2018
 *      Author: admin_aboyer
 *
 *  Configuration du module FlexPWM0 et activation des sorties.
 */

#ifndef CONFIGPWM_H_
#define CONFIGPWM_H_

#define SUB0_OFF 0
#define SUB0_ON 1
#define SUB1_OFF 0
#define SUB1_ON 1
#define SUB2_OFF 0
#define SUB2_ON 1
#define SUB3_OFF 0
#define SUB3_ON 1

//les trois clocks sources pour un sous-module PWM
#define CLKSOURCE_PERIPH 0
#define CLKSOURCE_EXTCLK 1
#define CLKSOURCE_AUXCLK 2

//paramètres de fonctionnement des sorties PWM
#define COMPL_OUTPUT 0 //mode complémentaire
#define INDEP_OUTPUT 1 //mode indépendant
#define POLAR_NORMAL 0 //polarité normal --> PWM23 utilisé (IPOL)
#define POLAR_INV 1 //polarité inversé --> PWM45 utilisé (IPOL)
#define ZERO_FS 0 //sortie forcée à 0 en Fault state
#define ONE_FS 1 //sortie forcée à 1 en Fault state
#define TRISTATE_FS 2 //sortie forcée en tristate en Fault state
#define PWM_INACTIV_DBG 0 //PWM inactive en mode debug
#define PWM_ACTIV_DBG 1 //PWM active en mode debug
//les 4 signaux sources pour l'initialisation du compteur
#define INITSEL_LOCALSYNC 0 //local sync
#define INITSEL_MSTRLD 1 //master reload
#define INITSEL_MSTSYNC 2  //master sync
#define INITSEL_EXT 3 //Ext sync
//les 2 signaux source du reload
#define RELOAD_LOCAL 0 //local reload
#define RELOAD_MSTRLD 1 //master reload
#define HALF_RLD 0 //reload à la moitié d'un cycle PWM
#define FULL_RLD 1 //reload à la fin d'un cycle PWM

#define LOAD_SUB0_OFF 0
#define LOAD_SUB0_ON 1
#define LOAD_SUB1_OFF 0
#define LOAD_SUB1_ON 1
#define LOAD_SUB2_OFF 0
#define LOAD_SUB2_ON 1
#define LOAD_SUB3_OFF 0
#define LOAD_SUB3_ON 1

//sélection d'une des 3 sorties d'un sous-module (sorties A, B ou X)
#define OUTPUTA 0
#define OUTPUTB 1
#define OUTPUTX 2
//activation ou non d'une des 3 sorties d'un sous-module (sorties A, B ou X)
#define OUTA_OFF 0
#define OUTA_ON 1
#define OUTB_OFF 0
#define OUTB_ON 1
#define OUTX_OFF 0
#define OUTX_ON 1

//alignement de la PWM
#define CENTER_ALIGN 0
#define EDGE_ALIGN 1
#define PHASE_SHIFT_ALIGN 2

//valeur initiale de la sortie PWM
#define PWM_INITVAL_0 0
#define PWM_INITVAL_1 1

//autorisation trigger de sortie
#define OUT_TRIG_OFF 0
#define OUT_TRIG_ON 1


void InitPWM0_Sub_ClkConfig(uint8_t SubCh, uint8_t ClkSource, uint8_t Div);
void Init_IO_PWM0(uint8_t Sub0, uint8_t Sub1, uint8_t Sub2);
void InitPWM0_Sub_OpMode(uint8_t SubCh, uint8_t Indep, uint8_t Polarity, uint8_t RunDbg, uint8_t InitSel, uint8_t ReloadSel, uint8_t ReloadFreq, uint8_t HalfFullRld);
void LoadPWM0_Params(uint8_t Sub0, uint8_t Sub1, uint8_t Sub2, uint8_t Sub3);
void Run_PWM0_SubModule(uint8_t RunSub0, uint8_t RunSub1, uint8_t RunSub2, uint8_t RunSub3);
void Config_PWM_Output(uint8_t SubCh, uint8_t OutputNb, uint8_t Polarity, uint8_t FaultState);
void Config_PWM0_SignalParams(uint8_t SubCh, uint8_t OutputNb, uint16_t Period, uint16_t Duty,uint16_t DeadTime, uint8_t Align, uint16_t InitPhase, uint8_t InitVal);
void Config_PWM0_OutA_SignalParams(uint8_t SubCh, uint16_t Period, uint16_t Duty,uint16_t DeadTime, uint8_t Align, uint16_t InitPhase, uint8_t InitVal);
void Config_PWM0_OutB_SignalParams(uint8_t SubCh, uint16_t Period, uint16_t Duty,uint16_t DeadTime, uint8_t Align, uint16_t InitPhase, uint8_t InitVal);
void Disable_Fault_PWM0_All(void);
void Enable_Fault_PWM0_All(void);
void Activate_PWM0_Output(uint8_t SubCh, uint8_t Activ_PWMA, uint8_t Activ_PWMB, uint8_t Activ_PWMX);
void Enable_OutputTrigger(uint8_t SubCh, uint8_t EnVal0, uint8_t EnVal1, uint8_t EnVal2, uint8_t EnVal3, uint8_t EnVal4, uint8_t EnVal5);
void Disable_OutputTrigger(uint8_t SubCh);
void Start_PWM0_Trigger(void);
void Stop_PWM0_Trigger(void);


#endif /* CONFIGPWM_H_ */
