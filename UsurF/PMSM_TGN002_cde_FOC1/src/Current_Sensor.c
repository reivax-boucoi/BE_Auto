/*
 * Current_Sensor.c
 *
 *  Created on: 16 ao�t 2018
 *      Author: Alexandre Boyer
 *
 *  Fonctions li�es � la lecture des courants de phase (mise en oeuvre mat�rielle + calculs)
 */


#include "MPC5744P.h"
#include "Current_Sensor.h"
#include "ConfigGPIO.h"
#include "ConfigADC.h"
#include "ConfigCTU.h"
#include "ClockConfig.h"
#include "mlib.h"


SWLIBS_3Syst_FLT Iphase; //courant dans les 3 phases (en A)
SWLIBS_3Syst_FLT Offset_Mes_Iph_mV;  //�ventuel offset affectant la mesure du courant de phase (en mV)
SWLIBS_3Syst_FLT Offset_Mes_Iph_uV;  //�ventuel offset affectant la mesure du courant de phase (en �V)

uint8_t Cptr_Mes_Cal;  //compteur pour la compensation de l'offset de mesure du courant
uint8_t Cal_Current_Done; //indique si la calibration de l'offset de la mesure du courant est achev�e
uint8_t ADC_SensorA_Status; //indique le statut de l'ADC associ� au capteur de courant sur la phase A
uint8_t ADC_SensorB_Status; //indique le statut de l'ADC associ� au capteur de courant sur la phase B
uint8_t Current_Sensor_Status; //indique le statut (fonctionnel ou pas) de la lecture du courant

/*****************************************************************************/
/* Current_sensor_Config_Init                                                */
/* Description: initialisation et config des p�riph et variables li�es � la  */
/* lecture du courant. Sur la carte MOTORGD, la mesure est r�alis�e � l'aide */
/* de r�sistance de shunt sur les pieds des MOSFET Low Side.
 * Le d�lai entre le trigger issu de la PWM et le lancement
 * de l'acquisition est r�gl� par Delay_Trigger_us (en �s).
 * La fonction renvoie une indication sur l'�tat de la configuration : correctement
 * effectu�e ou non.
 */
/*****************************************************************************/
uint8_t Current_sensor_Config_Init(uint32_t Delay_Trigger_us) {
	Current_Sensor_Status = CURRENT_SENSOR_STATUS_OK;

	Iphase.fltArg1 = 0;
	Iphase.fltArg2 = 0;
	Iphase.fltArg3 = 0;

	Offset_Mes_Iph_mV.fltArg1 = 0;
	Offset_Mes_Iph_mV.fltArg2 = 0;
	Offset_Mes_Iph_mV.fltArg3 = 0;

	Offset_Mes_Iph_uV.fltArg1 = 0;
	Offset_Mes_Iph_uV.fltArg2 = 0;
	Offset_Mes_Iph_uV.fltArg3 = 0;

	Cal_Current_Done = CAL_NOT_DONE;

	//Initialisation des ADC (ceux pour les lectures des courants de phase A et B)
	ADC_SensorA_Status = Config_Init_ADC_IPhase(ADC_MOD_IphA,ADC_CH_IphA,Pad_IphA);
	ADC_SensorB_Status = Config_Init_ADC_IPhase(ADC_MOD_IphB,ADC_CH_IphB,Pad_IphB);
	Config_CTU0_IPhase_A_B(Delay_Trigger_us);

	Current_Sensor_Status = ADC_SensorA_Status & ADC_SensorB_Status;
	return(Current_Sensor_Status);
}

/*****************************************************************************/
/* Config_Init_ADC_IPhase                                                 */
/* Description: configuration et initialisation de l'ADC_Num[ADC_Ch_Num]     */
/* ADC_Num est le num�ro du bloc ADC et ADC_Ch_Num le num�ro du channel de
 * conversion utilis�. On configure aussi le pad physique associ�, donn� par
 * Pad_Mes.
 * LA fonction renvoie le r�sultat de processus de calibration de l'ADC. Pour l'instant,
 * m�me si la calibration de l'ADC dysfonctionne, les conversions analog-num restent configur�es
 * de mani�re nominale.
 */
/*****************************************************************************/
uint8_t Config_Init_ADC_IPhase(uint8_t ADC_Num, uint8_t ADC_Ch_Num, uint8_t Pad_Mes) {
	uint8_t ADC_Cal_OK;

	//Config pads entr�e analogique utilis� pour la mesure
	Config_analog_pad(Pad_Mes);

	//configuration de l'ADC (uniquement en mode power down)
	ADC_Enter_PWDN(ADC_Num);
	ADC_DisableAllChannel_NormalConv(ADC_Num); //on desactive les conversions sur toutes les entr�es d'ADC_Num
	ADC_Cal_OK = ADC_Calibration(ADC_Num);
	//conversion one shot, lanc�e par le CTU
	ADC_init(ADC_Num,RIGHT_ALIGNED,ONE_SHOT,EXT_TRIG_DIS,TRIG_EDGE_RISE,CTU_TRIG_EN,HALF_CLK_ADC);
	ADC_ConvTime_config(ADC_Num,8);
	ADC_SelectChannel_NormalConv(ADC_Num,ADC_Ch_Num); //s�lection de l'entr�e analogique d'ADC_Num[ADC_Ch_Num]
	//configuration des interruptions li�es � ADC_Num et de son channel de conversion ADC_Ch_Num
	//Une interruption est g�n�r�e � l'issue d'une acquisition d�clench�e par CTU
	ADC_ISRGlobalEnable(ADC_Num,EOC_OFF,ECH_OFF,JEOC_OFF,JECH_OFF,ECTU_ON);
	ADC_ChannelMask_ISR(ADC_Num,ADC_Ch_Num,1);

	//activation presampling (optionnel)
//	ADC_0.PSCR.B.PRECONV = 0;
//	ADC_0.PSCR.B.PREVAL0 = 0;
//	ADC_0.PSR0.B.PRES11 = 1;

	//sortie du mode power down
	ADC_Exit_PWDN(ADC_MOD_IphA);

	return(ADC_Cal_OK);
}

/*****************************************************************************/
/* Config_CUT_IPhase_A                                              */
/* Description: configuration et initialisation du CTU en charge de synchroniser */
/* les acquisitions de IphA sur la commande PWM de FlexPWM0. Le CTU0 est utilis� � cet effet.
 * Le d�lai entre le trigger issu de la PWM et le lancement
 * de l'acquisition est r�gl� par Delay_Trigger_us (en �s).
 */
/*****************************************************************************/
void Config_CTU0_IPhaseA(uint32_t Delay_Trigger_us) {

	uint32_t Delay_Trig_NbClocks;

	/* Initialization of CTU                                  */
	//configuration du trigger generator (clock prescaler, entr�e de trigger, param compteur) pour les lectures
	//de IphA
	//d�clenchement sur front montant de Out_Trig0. Utilisation de MOTC divis�e par 4 comme horloge du compteur
	// --> �a fait 10 MHz pour l'horloge CTU. Il faut 5 �s entre la desactivation du high side et la stabilisation du courant.

	Delay_Trig_NbClocks = Delay_Trigger_us*F_CTU_MHz;

	Init_CTU0_TriggerGenerator_Trigger(Trigger_Input_ChA,1,0,Trigger_Output_ChA,
			CTUCptrPrescaler,CTUInitCptr,CTUMaxCptr,Delay_Trig_NbClocks);


	//configuration du trigger scheduler :
	//Bug : �a fonctionne, � condition d'activer les sorties Out_Trig pour les voies Trigger_Output_ChA/B.
	//Sinon, les conversions ADC ne sont pas lanc�es.
	Init_CTU0_TriggerScheduler(Trigger_Output_ChA,TRIG_EN,TRIG_EN,TRIG_EN); //sortie li� � l'entr�e MRS : on fait
			//commuter la sortie de trigger pour des raisons de debug et on active la commande ADC


	 //configuration de la liste de commande.
	//CTU0_Stream_or_Parallel_ADC_List(MODE_ADC_PARALLEL);
	CTU0_Stream_or_Parallel_ADC_List(MODE_ADC_STREAM);
	Init_CTU0_TriggerScheduler_ADC_SingleConv(Trigger_Output_ChA,ADC_MOD_IphA,ADC_CH_IphA,1,FIFO_ConvResult_IphA);

	//on autorise la g�n�ration du trigger externe associ� au Trigger 0 (broche C14) pour des besoins
	//de test (� d�commenter si n�cessaire)
	//Config_pad_out(PC14,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,0);
	//Config_Alter_Function_Out(PC14,2);

	//Activation de la sortie du CTU (activ�e par d�faut)
	Init_CTU0_GeneralControl(CTUOUT_ON);
	//Genetal Reload enable : GRE doit �tre mis � 1 pour indiquer que tous les registrers sont pr�ts � �tre mis
	//� jour au prochain MRS.
	CTU0_General_Reload_Enable();
}

/*****************************************************************************/
/* Config_CUT_IPhase_A_B                                              */
/* Description: configuration et initialisation du CTU en charge de synchroniser */
/* les acquisitions de IphA et IphB sur la commande PWM de FlexPWM0.
 * Le CTU0 est utilis� � cet effet.
 * Le d�lai entre le trigger issu de la PWM et le lancement
 * de l'acquisition est r�gl� par Delay_Trigger_us (en �s).
 */
/*****************************************************************************/
void Config_CTU0_IPhase_A_B(uint32_t Delay_Trigger_us) {

	uint32_t Delay_Trig_NbClocks;

	/* Initialization of CTU                                  */
	//configuration du trigger generator (clock prescaler, entr�e de trigger, param compteur) pour les lectures
	//de IphA et IphB
	//d�clenchement sur front montant de Out_Trig0. Utilisation de MOTC divis�e par 4 comme horloge du compteur
	// --> �a fait 10 MHz pour l'horloge CTU. Il faut 5 �s entre la desactivation du high side et la stabilisation du courant.

	Delay_Trig_NbClocks = Delay_Trigger_us*F_CTU_MHz;

	Init_CTU0_TriggerGenerator_Trigger2(Trigger_Input_ChA,1,0,Trigger_Output_ChA,Trigger_Output_ChB,
				CTUCptrPrescaler,CTUInitCptr,CTUMaxCptr,Delay_Trig_NbClocks,Delay_Trig_NbClocks);


	//configuration du trigger scheduler :
	//Bug : �a fonctionne, � condition d'activer les sorties Out_Trig pour les voies Trigger_Output_ChA/B.
	//Sinon, les conversions ADC ne sont pas lanc�es.
	Init_CTU0_TriggerScheduler(Trigger_Output_ChA,TRIG_EN,TRIG_EN,TRIG_EN); //sortie li� � l'entr�e MRS : on fait
			//commuter la sortie de trigger pour des raisons de debug et on active la commande ADC
	Init_CTU0_TriggerScheduler(Trigger_Output_ChB,TRIG_EN,TRIG_EN,TRIG_EN); //sortie li� � l'entr�e MRS : on fait
				//commuter la sortie de trigger pour des raisons de debug et on active la commande ADC

	 //configuration de la liste de commande :
	//deux commandes doivent s'ex�cuter ind�pendamment, � partir de 2 triggers diff�rents :
		// > l'acquisition de IphA sur ADC0_AN11, lanc� par VAL3 sur OUT0
		// > l'acquisition de IphB sur ADC1_AN12, lanc� par VAL3 sur OUT1
		//on utilise deux listes de commandes d'ADC tournant en parall�le, chacune ne contenant qu'une seule commande
		//en mode single conversion.
	CTU0_Stream_or_Parallel_ADC_List(MODE_ADC_PARALLEL);
	Init_CTU0_TriggerScheduler_ADC_SingleConv(Trigger_Output_ChA,ADC_MOD_IphA,ADC_CH_IphA,1,FIFO_ConvResult_IphA);
	Init_CTU0_TriggerScheduler_ADC_SingleConv(Trigger_Output_ChB,ADC_MOD_IphB,ADC_CH_IphB,5,FIFO_ConvResult_IphB);

	//on autorise la g�n�ration du trigger externe associ� au Trigger 0 (broche C14) pour des besoins
	//de test (� d�commenter si n�cessaire)
	Config_pad_out(PC14,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,0);
	Config_Alter_Function_Out(PC14,2);

	//Activation de la sortie du CTU (activ�e par d�faut)
	Init_CTU0_GeneralControl(CTUOUT_ON);
	//Genetal Reload enable : GRE doit �tre mis � 1 pour indiquer que tous les registrers sont pr�ts � �tre mis
	//� jour au prochain MRS.
	CTU0_General_Reload_Enable();
}


/*****************************************************************************/
/* Get_Iphase_MotorGD                                                 */
/* Description: la fonction r�cup�re la valeur lue par l'ADC rep�r� par ADC_Num[ADC_Ch_Num]
 * li�e au courant passant par la phase Phase_Nb (Iabc_struct). L'effet du gain et de l'offset
 * de la chaine d'acquisition est compens�, la valeur du courant est exprim�e en A.
 * La fonction renvoie le statut du r�sultat de conversion (valide ou non).
 */
/*****************************************************************************/
uint8_t Get_Iphase_MotorGD_FLT(uint8_t Phase_Nb, uint8_t ADC_Num, uint8_t ADC_Ch_Num) {
	tFloat Iphase_not_scaled;
	ADC_Result_struct mes_Courant;

	//r�cup�ration par lecture des registres de conversion des ADC, sans passer par interruptions, uniquement
	//par v�rification du bit EOCTU du registre ISR
	//if (ADC_0.ISR.B.EOCTU == 1) {
	if (ADC_Get_EOCTU(ADC_Num) == 1) {
			ADC_Clear_EOCTU(ADC_Num); //clear bit
			mes_Courant = ADC_ReadChannel(ADC_Num,ADC_Ch_Num);
			if (mes_Courant.ValidResult == 1) {
				Iphase_not_scaled = (tFloat) (0xFFF &  mes_Courant.ConvResult);
			}
	}
	else {
		Iphase_not_scaled = 0;
		mes_Courant.ValidResult = 0;
	}


	if (mes_Courant.ValidResult == 1) {
		if (Phase_Nb == PHASE_A) {
			Iphase.fltArg1 = Compensate_Iphase_MotorGD_FLT(Iphase_not_scaled,Offset_Mes_Iph_uV.fltArg1,GAIN_RES_Iph_MEAS_VuV);
		}
		else if (Phase_Nb == PHASE_B) {
			Iphase.fltArg2 = Compensate_Iphase_MotorGD_FLT(Iphase_not_scaled,Offset_Mes_Iph_uV.fltArg2,GAIN_RES_Iph_MEAS_VuV);
		}
		else {
			Iphase.fltArg3 = Compensate_Iphase_MotorGD_FLT(Iphase_not_scaled,Offset_Mes_Iph_uV.fltArg3,GAIN_RES_Iph_MEAS_VuV);
		}
	}
	else {
		//si le r�sultat de la conversion est erron�, on ne met pas � jour la valeur du courant sur la phase mesur�e.
	}
	return(mes_Courant.ValidResult);
}

/*****************************************************************************
/* Deduce_Iphase_C_FLT                                                      */
/* Description : la fonction calcule le courant de phase IC en fonction de IA et IB
 * (on suppose un syst�me triphas� �quilibr�)*/
/*****************************************************************************/
void Deduce_Iphase_C_FLT(void) {
	Iphase.fltArg3 = MLIB_Add_FLT(Iphase.fltArg1,Iphase.fltArg2);
	Iphase.fltArg3 = MLIB_Neg_FLT(Iphase.fltArg3);
}

/*****************************************************************************
/* Get_ThreePhase_MotorGD_FLT                                                      */
/* Description: : la fonction r�alise l'ensemble des op�rations permettant de mettre
 * � jour les valeurs des 3 courants de phase, � partir des lectures des courants traversant
 * les r�sistances de shunt sur les bras A et B (celui sur le bras C est d�duit des 2
 * autres mesures. LA fonction met � jour les valeurs de la variable globale Iphase.
 * Pour l'instant, en cas de renvoi d'une conversion d'un ADC erron�e, le calcul du
 * courant de la phase C n'est pas effectu�e.
 * La fonction indique si la mesure des courants de phase s'est correctement d�roul�e.
/*****************************************************************************/
uint8_t Get_ThreePhase_Current_MotorGD_FLT(void) {
	uint8_t ADC_phaseA_convOK, ADC_phaseB_convOK, Get_Current_OK;

	//r�cup�ration des courants de phase : IphA et IphB sont lus par les ADC, IphC est d�duit des
	//courants IphA et IphB.
	ADC_phaseA_convOK = Get_Iphase_MotorGD_FLT(PHASE_A,ADC_MOD_IphA,ADC_CH_IphA);
	ADC_Clear_All_Flags(ADC_MOD_IphA);
	ADC_phaseB_convOK = Get_Iphase_MotorGD_FLT(PHASE_B,ADC_MOD_IphB,ADC_CH_IphB);
	ADC_Clear_All_Flags(ADC_MOD_IphB);
	Get_Current_OK = ADC_phaseA_convOK & ADC_phaseB_convOK;
	//d�duction du courant de phase C
	if (Get_Current_OK == 1) {
		Deduce_Iphase_C_FLT();
	}

	return(Get_Current_OK);
}


/*****************************************************************************
/* Perform_Current_Meas_Calibration                                                       */
/* Description: : on compense les offsets des mesures de courant des phases A et B.
 * On effectue un nb NB_MAX_MES_CAL de mesures pour d�terminer l'offset en entr�e des
 * points de mesure. LA commande PWM est choisie pour ne pas faire circuler de courant
 * dans les r�sistances de shunt.
 * A l'issue des NB_MAS_MES_CAL, on entrera en mode de fonctionnement normal.*/
/*****************************************************************************/
void Perform_Current_Meas_Calibration(void) {
	ADC_Result_struct mes_CourantA, mes_CourantB;
	SWLIBS_3Syst_FLT Iphase_not_scaled_FLT;

	if (Cptr_Mes_Cal < NB_MAX_MES_CAL) {
		//mesure courant de phase � courant nul, si les nouvelles donn�es sont dispo
		if ((ADC_0.ISR.B.EOCTU == 1) && (ADC_1.ISR.B.EOCTU == 1)) {
			mes_CourantA = ADC_ReadChannel(ADC_MOD_IphA,ADC_CH_IphA);
			Iphase_not_scaled_FLT.fltArg1 = (tFloat) (0xFFF & mes_CourantA.ConvResult); //donn�es de conversion align�es � droite sur 12 bits
			mes_CourantB = ADC_ReadChannel(ADC_MOD_IphB,ADC_CH_IphB);
			Iphase_not_scaled_FLT.fltArg2 = (tFloat) (0xFFF & mes_CourantB.ConvResult); //donn�es de conversion align�es � droite sur 12 bits
			if ((mes_CourantA.ValidResult == 1) && (mes_CourantB.ValidResult == 1)) {
				//on fera la mise � l'�chelle � l'issue du calcul
				Offset_Mes_Iph_mV.fltArg1 = MLIB_Add_FLT(Offset_Mes_Iph_mV.fltArg1,Iphase_not_scaled_FLT.fltArg1);
				Offset_Mes_Iph_mV.fltArg2 = MLIB_Add_FLT(Offset_Mes_Iph_mV.fltArg2,Iphase_not_scaled_FLT.fltArg2);
				Cptr_Mes_Cal++;
			}
			ADC_0.ISR.B.EOCTU = 1; //clear bit
			ADC_1.ISR.B.EOCTU = 1; //clear bit
		}
	}
	else {
		//calcul des offset
		Offset_Mes_Iph_mV.fltArg1 = MLIB_Div_FLT(Offset_Mes_Iph_mV.fltArg1,NB_MAX_MES_CAL);
		Offset_Mes_Iph_mV.fltArg2 = MLIB_Div_FLT(Offset_Mes_Iph_mV.fltArg2,NB_MAX_MES_CAL);

		//calcul des valeurs d'offset en mV et �V
		Offset_Mes_Iph_uV.fltArg1 = MLIB_Mul_FLT(Offset_Mes_Iph_mV.fltArg1,ADC_RESOLUTION_uV); //r�sultat en uV
		Offset_Mes_Iph_uV.fltArg2 = MLIB_Mul_FLT(Offset_Mes_Iph_mV.fltArg2,ADC_RESOLUTION_uV); //r�sultat en uV
		Offset_Mes_Iph_mV.fltArg1 = MLIB_Div_FLT(Offset_Mes_Iph_uV.fltArg1,1000); //r�sultat en mV
		Offset_Mes_Iph_mV.fltArg2 = MLIB_Div_FLT(Offset_Mes_Iph_uV.fltArg2,1000);  //r�sultat en mV

		Cptr_Mes_Cal =0;
		Cal_Current_Done = CAL_DONE;
	}
}

/*****************************************************************************/
/* Compensate_Iphase_MotorGD                                                */
/* Description: la fonction corrige le courant mesur� sur une phase (r�sultat renvoy� en A) */
/* Elle compense l'effet de l'offset et du gain de mesure du courant sur une phase.                                                                     */
/****************************************************************************/
tFloat Compensate_Iphase_MotorGD_FLT(tFloat Meas_Iph,tFloat offset_meas_uV,tFloat gain_meas_V_uV) {
	tFloat Umeas_uV;  //tensions acquises par l'ADC aux bornes des r�sistances de shunt pour la
				//lecture de IphA (exprim� en uV)
	tFloat Imeas_extract_A;

	Umeas_uV = MLIB_Mul_FLT(Meas_Iph,ADC_RESOLUTION_uV); //r�sultat en uV

	//compensation de l'offset
	Umeas_uV = MLIB_Sub_FLT(Umeas_uV,offset_meas_uV);

	//compensation du gain, passage en mV puis conversion en A.
	Imeas_extract_A = MLIB_Div_FLT(Umeas_uV,gain_meas_V_uV);

	return(Imeas_extract_A);
}
