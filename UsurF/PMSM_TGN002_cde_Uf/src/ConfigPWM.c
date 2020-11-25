/*
 * ConfigPWM.c
 *
 *  Created on: 16 août 2018
 *      Author: admin_aboyer
 *
 *  Configuration du module FlexPWM0 et activation des sorties.
 */

#include "MPC5744P.h"
#include "ConfigPWM.h"
#include "ConfigGPIO.h"


/*****************************************************************************/
/* InitPWM0_SubClkConfig                                                            */
/* Description: Configure les paramètres d'horloge du sous-module SubCh de FlexPWM0 */
/* Les paramètres configurés sont la source de la clock et la valeur du prescaler
 * (la valeur du prescaler = 2^Div, où Div = 0..7)
 */
/*****************************************************************************/
void InitPWM0_Sub_ClkConfig(uint8_t SubCh, uint8_t ClkSource, uint8_t Div) {
	FlexPWM_0.SUB[SubCh].CTRL2.B.CLK_SEL = ClkSource; //clock source selection (0 --> peripheral clock MOTC
					//1 --> EXT_CLK, 2 --> Submodule 0's clock (AUX_CLK)
	FlexPWM_0.SUB[SubCh].CTRL1.B.PRSC = Div;

}

/*****************************************************************************/
/* Init_IO_PWM0
/* Description: Configure les pads de sortie associées au module PWM0
/*****************************************************************************/
void Init_IO_PWM0(uint8_t Sub0, uint8_t Sub1, uint8_t Sub2) {
	if (Sub0 == SUB0_ON) {
		// A[11] pin as FlexPWM A[0] output
		Config_pad_out(PA11,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,0);
		Config_Alter_Function_Out(PA11,2);
		// A[10] pin as FlexPWM B[0] output
		Config_pad_out(PA10,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,0);
		Config_Alter_Function_Out(PA10,2);
	}

	if (Sub1 == SUB1_ON) {
		// F[0] pin as FlexPWM A[1] output
		Config_pad_out(PF0,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,0);
		Config_Alter_Function_Out(PF0,1);
		// D[14] pin as FlexPWM B[1] output
		Config_pad_out(PD14,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,0);
		Config_Alter_Function_Out(PD14,1);
	}

	if (Sub2 == SUB2_ON) {
		// A[12] pin as FlexPWM A[2] output
		Config_pad_out(PA12,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,0);
		Config_Alter_Function_Out(PA12,2);
		// A[13] pin as FlexPWM B[2] output
		Config_pad_out(PA13,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,0);
		Config_Alter_Function_Out(PA13,2);
	}
}


/*****************************************************************************/
/* Config_PWM_Output_Polarity                                                            */
/* Description: Configure la polarité et l'état en cas d'erreur de la sortie (A, B, ou X d'un sous-module
 * La variable OutputNb indique s'il s'agit de la sortie A (0), B (1) ou X (2).
 * Si lA variable Polarity = 1, la polarité de la sortie est inversée.
 * La variable FaultState definit si la sortie sera à 0, 1 ou tristate en cas d'erreur.
 */
/*****************************************************************************/
void Config_PWM_Output(uint8_t SubCh, uint8_t OutputNb, uint8_t Polarity, uint8_t FaultState) {
	if (OutputNb == OUTPUTA) {
		FlexPWM_0.SUB[SubCh].OCTRL.B.POLA = Polarity;
		FlexPWM_0.SUB[SubCh].OCTRL.B.PWMAFS = FaultState;
	}
	else if (OutputNb == OUTPUTB) {
		FlexPWM_0.SUB[SubCh].OCTRL.B.POLB = Polarity;
		FlexPWM_0.SUB[SubCh].OCTRL.B.PWMBFS = FaultState;
	}
	else if (OutputNb == OUTPUTX) {
		FlexPWM_0.SUB[SubCh].OCTRL.B.POLX = Polarity;
		FlexPWM_0.SUB[SubCh].OCTRL.B.PWMXFS = FaultState;
	}
}

/*****************************************************************************/
/* InitPWM0_Sub_OpMode                                                            */
/* Description: Configure le mode de fonctionnement du sous-module SubCh de FlexPWM0 */
/* Indep = 1 --> voies A et B sont en mode indépendants, sinon complémentaires.
 * Polarity indique si PWM23 ou PWM45 est utilisé comme source du signal de sortie.
 * N'intervient qu'en mode complémentaire
 * Si RunDbg = 1, le signal PWM continue à être émis en mode debug.
 * InitSel indique le signal d'initialisation du compteur.
 * ReloadSel indique si le signal de reload des paramètres du sous-module est local ou vien du master reload.
 * ReloadFreq indique la fréquence du reload en nb de cycle de PWM (0+1 à 15+1)
 * HalfFullRld indique si le reload est fait à la moitié (0) ou à la fin (1) d'un cycle PWM
 */
/*****************************************************************************/
void InitPWM0_Sub_OpMode(uint8_t SubCh, uint8_t Indep, uint8_t Polarity, uint8_t RunDbg,
		uint8_t InitSel, uint8_t ReloadSel, uint8_t ReloadFreq, uint8_t HalfFullRld) {
	uint8_t mask;

	switch(SubCh) {
		case 0 : mask = 0b0001; break;
		case 1 : mask = 0b0010; break;
		case 2 : mask = 0b0100; break;
		case 3 : mask = 0b1000; break;
	}

	FlexPWM_0.SUB[SubCh].CTRL2.B.INDEP = Indep;  //mode indépendant ou complémentaire
	//gestion de la polarité (valable uniquement en mode complémentaire)
	if (Polarity == 0) {
		FlexPWM_0.MCTRL.B.IPOL = FlexPWM_0.MCTRL.B.IPOL | (0b0000 & mask);
	}
	else if (Polarity == 1) {
		FlexPWM_0.MCTRL.B.IPOL = FlexPWM_0.MCTRL.B.IPOL | (0b1111 & mask);
	}

	FlexPWM_0.SUB[SubCh].CTRL2.B.DBGEN = RunDbg; //PWM active ou non en mode debug
	FlexPWM_0.SUB[SubCh].CTRL2.B.INIT_SEL = InitSel; //init source selection (0 --> Local Sync (PWMX)
	//1 --> Master reload from Sub0, 2 --> Master Sync from Sub0, 3 --> EXT_SYNC
	FlexPWM_0.SUB[SubCh].CTRL2.B.RELOAD_SEL = ReloadSel; //reload source selection (0 --> local,
	//1 --> Master Reload (from submodule 0)

	if (ReloadFreq < 16) {
		FlexPWM_0.SUB[SubCh].CTRL1.B.LDFQ = ReloadFreq;
	}
	else {
		FlexPWM_0.SUB[SubCh].CTRL1.B.LDFQ = 0;
	}

	if (HalfFullRld == 0) { //reload at the half of PWM cycle
		FlexPWM_0.SUB[SubCh].CTRL1.B.FULL = 0;
		FlexPWM_0.SUB[SubCh].CTRL1.B.HALF = 1;
	}
	else if (HalfFullRld == 0) { //reload at the end of PWM cycle
		FlexPWM_0.SUB[SubCh].CTRL1.B.FULL = 1;
		FlexPWM_0.SUB[SubCh].CTRL1.B.HALF = 0;
	}
}


/*****************************************************************************/
/* LoadPWM0_Params                                                            */
/* Description: charge les paramètres dans les sous-modules sélectionnées.
 * Si SubX = 1, alors le chargement des paramètres se fera dans le sous-module X
 */
/*****************************************************************************/
void LoadPWM0_Params(uint8_t Sub0, uint8_t Sub1, uint8_t Sub2, uint8_t Sub3) {
	uint8_t LoadSubModule;
	if ((Sub0 != 0) && (Sub0 != 1)) {
		Sub0 = 0;
	}
	if ((Sub1 != 0) && (Sub1 != 1)) {
		Sub1 = 0;
	}
	if ((Sub2 != 0) && (Sub2 != 1)) {
		Sub2 = 0;
	}
	if ((Sub3 != 0) && (Sub3 != 1)) {
		Sub3 = 0;
	}
	LoadSubModule = Sub0 | (Sub1 << 1) | (Sub2 << 2) | (Sub3 << 3);
	FlexPWM_0.MCTRL.B.LDOK = LoadSubModule; /* Load the PRSC bits of CTRL1 and the INIT, and VALx registers
		    								   into a set of buffers */
}

/*****************************************************************************/
/* Run_PWM_SubModule                                                         */
/* Description: active l'horloge des différents sous-module de FlexPWM0      */
/* Les arguments d'entrée sont = 0 --> désactivation de l'horloge. = 1 --> activation */
/*****************************************************************************/
void Run_PWM0_SubModule(uint8_t RunSub0, uint8_t RunSub1, uint8_t RunSub2, uint8_t RunSub3) {
	uint8_t RunSubModule;
	if ((RunSub0 != 0) && (RunSub0 != 1)) {
		RunSub0 = 0;
	}
	if ((RunSub1 != 0) && (RunSub1 != 1)) {
		RunSub1 = 0;
	}
	if ((RunSub2 != 0) && (RunSub2 != 1)) {
		RunSub2 = 0;
	}
	if ((RunSub3 != 0) && (RunSub3 != 1)) {
		RunSub3 = 0;
	}
	RunSubModule = RunSub0 | (RunSub1 << 1) | (RunSub2 << 2) | (RunSub3 << 3);
	FlexPWM_0.MCTRL.B.RUN = RunSubModule;
}

/*****************************************************************************/
/* Config_PWM0_SignalParams                                                  */
/* Description: configure les paramètres temporels du signal PWM émis par le sous-module
 * SubCh. La variable OutputNb indique s'il s'agit de la sortie A (0), B (1) ou X (2).
 * Period donne la période du signal PWM en nb de top de clock du sous-module (après préscaler du */
/* sous-module. Duty règle le rapport cyclique et DeadTime le dead time.
 * Le dead time n'est valable qu'en mode complémentaire.
 * Align indique l'alignemement de la PWM (center-aligned (0), edge-aligned (1), phase-shifted (2))
 * InitPhase indique le début de l'impulsion PWM par rapport au début du cycle PWM (exprimé en nb de clock)
 * InitPhase n'est pris en compte que dans l'alignement phase-shifted.
 * InitVal indique la valeur initiale de la sortie PWM (0 ou 1)
 */
/*****************************************************************************/
void Config_PWM0_SignalParams(uint8_t SubCh, uint8_t OutputNb, uint16_t Period, uint16_t Duty,
		uint16_t DeadTime, uint8_t Align, uint16_t InitPhase, uint8_t InitVal) {

	//la forme temporelle de PWMA et PWMB est définie par 7 paramètres, exprimé en entier signé sur 16 bit
	//qui sont comparés avec les valeurs d'un compteur cadencé par la clock du sous-module
	//INIT --> valeur initiale (= -period/2)
	//VAL0 --> valeur à la moitié de la période PWM (= 0)
	//VAL1 --> valeur finale (= +period/2)
	//VAL2 et VAL3 définissent les fronts montants et descendants de PWMA
	//VAL4 et VAL5 définissent les fronts montants et descendants de PWMB.
	//En mode complémentaire, selon la polarité, PWM4 et PWM 5 sont ignorés (polarité normale), ou
	//PWM2 et PWM3 sont ignorés (polarité inversée).

	uint16_t mid_period;

	//paramètres temporels indépendants de l'alignement et de la voie sélectionnée (hormis PWM_X dont
	//les paramètres temporels ne peuvent pas être contrôlés).
	mid_period = Period/2;
	FlexPWM_0.SUB[SubCh].INIT.R = -mid_period;
	FlexPWM_0.SUB[SubCh].VAL0.R = 0;
	FlexPWM_0.SUB[SubCh].VAL1.R = mid_period;

	if (OutputNb == OUTPUTA) {
		Config_PWM0_OutA_SignalParams(SubCh,Period,Duty,DeadTime,Align,InitPhase,InitVal);
	}
	else if (OutputNb == OUTPUTB) {
		Config_PWM0_OutB_SignalParams(SubCh,Period,Duty,DeadTime,Align,InitPhase,InitVal);
	}
	else if (OutputNb == OUTPUTX) {
		FlexPWM_0.SUB[SubCh].CTRL2.B.PWMX_INIT = InitVal; //valeur initiale de la sortie PWMX. Seul param réglable.
	}
}

/*****************************************************************************/
/* Config_PWM0_OutA_SignalParams                                                  */
/* Description: configure les paramètres temporels de la sortie A du sous-module SubCh.
 * Period donne la période du signal PWM en nb de top de clock du sous-module (après préscaler du */
/* sous-module. Duty règle le rapport cyclique et DeadTime le dead time.
 * Align indique l'alignemement de la PWM (center-aligned (0), edge-aligned (1), phase-shifted (2))
 * InitPhase indique le début de l'impulsion PWM par rapport au début du cycle PWM (exprimé en nb de clock)
 * InitPhase n'est pris en compte que dans l'alignement phase-shifted.
 * InitVal indique la valeur initiale de la sortie PWM (0 ou 1)
/*****************************************************************************/
void Config_PWM0_OutA_SignalParams(uint8_t SubCh, uint16_t Period, uint16_t Duty,
		uint16_t DeadTime, uint8_t Align, uint16_t InitPhase, uint8_t InitVal) {

	uint16_t mid_period;
	int16_t rising_edge;
	int16_t falling_edge;

	mid_period = Period/2;

	FlexPWM_0.SUB[SubCh].DTCNT0.R = DeadTime; // deadtime values (defined in terms of clock cycles)
	FlexPWM_0.SUB[SubCh].CTRL2.B.PWM23_INIT = InitVal; //valeur initiale de la sortie

	//on définit les instants des fronts montants (VAL2) et descendants (VAL3) de PWMA
	//valeurs signées !
	if (Align == CENTER_ALIGN) {
		rising_edge = -Duty/2;
		falling_edge = -rising_edge;

	}
	else if (Align == EDGE_ALIGN) {
		rising_edge = -mid_period;
		falling_edge = rising_edge+Duty;
	}
	else if (Align == PHASE_SHIFT_ALIGN) {
		rising_edge = -mid_period+InitPhase;
		falling_edge = rising_edge+Duty;
	}

	FlexPWM_0.SUB[SubCh].VAL2.R = rising_edge;
	FlexPWM_0.SUB[SubCh].VAL3.R = falling_edge;
}

/*****************************************************************************/
/* Config_PWM0_OutB_SignalParams                                                  */
/* Description: configure les paramètres temporels de la sortie B du sous-module SubCh.
 * Period donne la période du signal PWM en nb de top de clock du sous-module (après préscaler du */
/* sous-module. Duty règle le rapport cyclique et DeadTime le dead time.
 * Align indique l'alignemement de la PWM (center-aligned (0), edge-aligned (1), phase-shifted (2))
 * InitPhase indique le début de l'impulsion PWM par rapport au début du cycle PWM (exprimé en nb de clock)
 * InitPhase n'est pris en compte que dans l'alignement phase-shifted.
 * InitVal indique la valeur initiale de la sortie PWM (0 ou 1)
/*****************************************************************************/
void Config_PWM0_OutB_SignalParams(uint8_t SubCh, uint16_t Period, uint16_t Duty,
		uint16_t DeadTime, uint8_t Align, uint16_t InitPhase, uint8_t InitVal) {

	uint16_t mid_period;
	int16_t rising_edge;
	int16_t falling_edge;

	mid_period = Period/2;

	FlexPWM_0.SUB[SubCh].DTCNT1.R = DeadTime; // deadtime values (defined in terms of clock cycles)
	FlexPWM_0.SUB[SubCh].CTRL2.B.PWM45_INIT = InitVal; //valeur initiale de la sortie

	//on définit les instants des fronts montants (VAL4) et descendants (VAL5) de PWMB
	//valeurs signées !
	if (Align == CENTER_ALIGN) {
		rising_edge = -Duty/2;
		falling_edge = -rising_edge;

	}
	else if (Align == EDGE_ALIGN) {
		rising_edge = -mid_period;
		falling_edge = rising_edge+Duty;
	}
	else if (Align == PHASE_SHIFT_ALIGN) {
		rising_edge = -mid_period+InitPhase;
		falling_edge = rising_edge+Duty;
	}

	FlexPWM_0.SUB[SubCh].VAL4.R = rising_edge;
	FlexPWM_0.SUB[SubCh].VAL5.R = falling_edge;
}

/*****************************************************************************/
/* Disable_Fault_PWM0_All                                                  */
/* Description: desactive les fautes associées aux sorties A, B et X des 4
 * sous-modules (0, 1, 2, 3) du module PWM0. Dangereux du point de vue safety,
 * mais facilite le debug.
 */
/*****************************************************************************/
void Disable_Fault_PWM0_All(void) {
	FlexPWM_0.SUB[0].DISMAP.R   = 0x0000;	// disable fault pin condition
	FlexPWM_0.SUB[1].DISMAP.R   = 0x0000;	// disable fault pin condition
	FlexPWM_0.SUB[2].DISMAP.R   = 0x0000;	// disable fault pin condition
}

/*****************************************************************************/
/* Enable_Fault_PWM0_All                                                  */
/* Description: active les fautes associées aux sorties A, B et X des 4
 * sous-modules (0, 1, 2, 3) du module PWM0
 */
/*****************************************************************************/
void Enable_Fault_PWM0_All(void) {
	FlexPWM_0.SUB[0].DISMAP.R   = 0x0FFF;	// enable fault pin condition
	FlexPWM_0.SUB[1].DISMAP.R   = 0x0FFF;	// enable fault pin condition
	FlexPWM_0.SUB[2].DISMAP.R   = 0x0FFF;	// enable fault pin condition
}


/*****************************************************************************/
/* Activate_PWM0_Output                                                  */
/* Description: autorise les sorties A, B, X du sous-module SubCh
 * Si Activ_PWMA = 1, la sortie PWMA est activée. Pareil pour Activ_PWMB et Activ_PWMX.
 */
/*****************************************************************************/
void Activate_PWM0_Output(uint8_t SubCh, uint8_t Activ_PWMA, uint8_t Activ_PWMB, uint8_t Activ_PWMX) {
	uint8_t mask;

	switch(SubCh) {
		case 0 : mask = 0b0001; break;
		case 1 : mask = 0b0010; break;
		case 2 : mask = 0b0100; break;
		case 3 : mask = 0b1000; break;
	}

	//activation sortie PWMA
	if (Activ_PWMA == 0) {
		FlexPWM_0.OUTEN.B.PWMA_EN = FlexPWM_0.OUTEN.B.PWMA_EN | (0b0000 & mask);
	}
	else if (Activ_PWMA == 1) {
		FlexPWM_0.OUTEN.B.PWMA_EN = FlexPWM_0.OUTEN.B.PWMA_EN | (0b1111 & mask);
	}
	//activation sortie PWMB
	if (Activ_PWMB == 0) {
		FlexPWM_0.OUTEN.B.PWMB_EN = FlexPWM_0.OUTEN.B.PWMB_EN | (0b0000 & mask);
	}
	else if (Activ_PWMB == 1) {
		FlexPWM_0.OUTEN.B.PWMB_EN = FlexPWM_0.OUTEN.B.PWMB_EN | (0b1111 & mask);
	}
	//activation sortie PWMX
	if (Activ_PWMX == 0) {
		FlexPWM_0.OUTEN.B.PWMX_EN = FlexPWM_0.OUTEN.B.PWMX_EN | (0b0000 & mask);
	}
	else if (Activ_PWMX == 1) {
		FlexPWM_0.OUTEN.B.PWMX_EN = FlexPWM_0.OUTEN.B.PWMX_EN | (0b1111 & mask);
	}
}

/*****************************************************************************/
/* Enable_OutputTrigger                                                  */
/* Description: active la génération de trigger pour l'ADC sur VAL0 à 5
 * VAL0, VAL2, VAL4 peuvent générer OUT_TRIG0. VAL1, VAL3, VAL5 peuvent générer OUT_TRIG1.
 */
/*****************************************************************************/
void Enable_OutputTrigger(uint8_t SubCh, uint8_t EnVal0, uint8_t EnVal1, uint8_t EnVal2,
		uint8_t EnVal3, uint8_t EnVal4, uint8_t EnVal5) {
	uint8_t OutTrigEn;

	OutTrigEn = EnVal0 | (EnVal1 << 1) | (EnVal2 << 2) | (EnVal3 << 3) | (EnVal4 << 4) | (EnVal5 << 5) | (EnVal3 << 6);
	FlexPWM_0.SUB[SubCh].TCTRL.B.OUT_TRIG_EN = OutTrigEn;
}

/*****************************************************************************/
/* Disable_OutputTrigger                                                  */
/* Description: désactive la génération de trigger produit par le canal SubCh de FlexPW0.
 */
/*****************************************************************************/
void Disable_OutputTrigger(uint8_t SubCh) {
	uint8_t OutTrigEn;

	FlexPWM_0.SUB[SubCh].TCTRL.B.OUT_TRIG_EN = 0;
}

/*****************************************************************************/
/* Start_PWM0_Trigger                                                  */
/* Description: active les triggers des différents sous-modules de FlexPWM (requis
 * pour la synchro PWM --> ADC.
 */
/*****************************************************************************/
void Start_PWM0_Trigger(void) {
	Enable_OutputTrigger(0,OUT_TRIG_OFF,OUT_TRIG_ON,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF);
	Enable_OutputTrigger(1,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF);
	Enable_OutputTrigger(2,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF);
	LoadPWM0_Params(LOAD_SUB0_ON,LOAD_SUB1_OFF,LOAD_SUB2_OFF,LOAD_SUB3_OFF);
}

/*****************************************************************************/
/* Stop_PWM0_Trigger                                                  */
/* Description: arrête les triggers des différents sous-modules de FlexPWM (requis
 * pour la synchro PWM --> ADC.
 */
/*****************************************************************************/
void Stop_PWM0_Trigger(void) {
	Disable_OutputTrigger(0);
	Disable_OutputTrigger(1);
	Disable_OutputTrigger(2);
	LoadPWM0_Params(LOAD_SUB0_ON,LOAD_SUB1_OFF,LOAD_SUB2_OFF,LOAD_SUB3_OFF);
}




