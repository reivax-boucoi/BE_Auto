/*
 * Cmd_moteur.c
 *
 *  Created on: 20 nov. 2018
 *      Author: admin_aboyer
 *
 *  Fonctions associ�es au contr�le moteur, bas� sur une commande U/f.
 *  Les fonctions math�matiques sont issues de la SDK AMMCLIB. Le type FLT est utilis�.
 *  Dans le fichier SWLIBS_Config.h, donnez � SWLIBS_DEFAULT_IMPLEMENTATION la valeur
 *  SWLIBS_DEFAULT_IMPLEMENTATION_FLT, sinon erreur de build !
 */


#include "Cmd_moteur.h"
#include "gd3000.h"
#include "ConfigPIT.h"
#include "ClockConfig.h"
#include "SWLIBS_Typedefs.h"
#include "gflib.h"
#include "gmclib.h"
#include "mlib.h"
#include "pga411.h"
#include "ConfigGPIO.h"
#include "ConfigADC.h"
#include "Current_Sensor.h"
#include "User_Interface.h"
#include "modele_moteur_virtuel.h"


/*************************************************************************
 *variables de cmd_moteur.c :
 *************************************************************************/

//variables li�es � la s�quence de pilotage moteur
//la variable Mode_Sequence_Moteur indique si on se trouve en mode de fonctionnement normal ou de calibration.
//en mode de calibration, on fait des acquisitions de courant de IphA et IphB pour compenser l'offset. LA boucle
//slow loop n'est pas effectu�e. En mode normal, les boucles fast loop et slow loop sont effectu�es normalement.
uint8_t Mode_Sequence_Moteur;

//variables li�es au calcul de la commande de vitesse (rampe de vitesse)
extern tFloat Consigne_Vitesse; //la valeur finale de vitesse � atteindre
tFloat D_Vitesse; //la pente de la rampe --> changement de vitesse max pendant une p�riode
		//de boucle slow loop (exprim� en RPM)
tFloat Commande_Vitesse; //la commande de vitesse en cours

//variables li�es au contr�le de la boucle slow loop
uint32_t cptr_slow_loop; //compteur qui permet de d�terminer s'il faut ex�cuter la boucle slow loop lors
	//d'une ex�cution de la fast loop.
uint32_t nb_exec_FL_max_Exec_SL; //d�termine le nb max de boucle Fast Loop (FL) � ex�cuter pour ex�cuter
	//la boucle slow loop (SL)

//les variables li�es � la commande des 3 phases du moteur (tensions dans les rep�res abc et dq, et rapports cycliques)
//les commandes dans les rep�res DQ, ABC et traduction en rapports cycliques.
SWLIBS_2Syst_FLT Udq_Commande; //commande moteur dans le rep�re dq, exprim�e en V
SWLIBS_3Syst_FLT Uabc_Commande; //commande moteur dans le rep�re abc, exprim�e en V (uniquement pour le test, pour afficher
//sous Freemaster les tensions de commande en V).
uint16_t Cmd_DutyA, Cmd_DutyB, Cmd_DutyC; //rapports cycliques des commandes des 3 phases, � fournir directement
							//au FlexPWM (exprim�s en top d'horloge)
extern uint32_t PWMPeriod;  //en nb de clock submodule
extern uint32_t DutyDefaultGD3000; //rapport cyclique par d�faut (en nb de clock submodule)

//variables li�es aux courants de phase
extern SWLIBS_3Syst_FLT Iphase; //courant dans les 3 phases (en A)
extern SWLIBS_3Syst_FLT Offset_Mes_Iph_mV;  //�ventuel offset affectant la mesure du courant de phase (en mV)
extern SWLIBS_3Syst_FLT Offset_Mes_Iph_uV;
extern uint8_t Cptr_Mes_Cal;  //compteur pour la compensation de l'offset de mesure du courant
extern uint8_t Cal_Current_Done; //indique si la calibration de l'offset de la mesure du courant est achev�
uint32_t Delay_Trig_us; //d�lai en �s entre le trigger issu de la PWM (d�passement VAL1 pour d�clencher l'acquisition
//� partir du moment o� les 3 phases sont � 0) et la commande d'�chantillonnage envoy�e par le CTU � l'ADC.


//variables li�es � la mesure de l'angle du rotor par le PGA411, ainsi qu'� l'extraction de la vitesse rotor
extern uint8_t Resolution_PGA411; //r�solution du PG411 pour la mesure de l'angle du rotor
tFloat angle_rotor_elec; //en degr�
tFloat angle_rotor_meca; //en degr� --> ce qui est mesur� par le r�solveur
tFloat velocity_rotor_elec; //en RPM
tFloat velocity_rotor_meca; //en RPM
uint32_t nb_exec_loop_max_speed;  //nombre d'acquisition de donn�es de position rotor pour estimer la vitesse
uint8_t cptr_speed;  //pour compter le nb de donn�es de position rotor que l'on a acquise.
//pour l'�valuation de la vitesse � partir des mesures d'angle rotor (par l'interface r�solveur) :
tFloat tps_integration_vitesse;  //le temps pendant lequel on fait le calcul d'int�gration de l'angle rotor parcouru pendant
	//la boucle d'�valuation de la vitesse.
tFloat increment_angle_rotor;  //suivi de l'angle parcouru pendant la boucle d'integration de la vitesse.
tFloat angle_rotor_meca_precedent; //valeur pr�c�demment mesur�e de l'angle (pour rep�rer un franchissement de 360�)
tFloat cte_calcul_RPM_from_deg_us;
tFloat cte_calcul_RPM_from_rad_us;


//variables li�es � l'estimation de l'angle �lectrique du rotor, requise pour la commande U/f.
//La commande U/f �tant en boucle ouverte, on n'utilise pas la mesure de l'angle rotor pour la commande. Sinon,
//risque d'instabilit�.
tFloat Angle_Rotor_Estime; //l'angle du rotor, estim� � partir de la commande de vitesse.
	//Cette variable est exprim� en radian, compris entre -pi et +pi.
	//attention : il s'agit d'un angle �lectrique ! Il est d�duit de Commande_Vitesse, qui est bien la
	//vitesse m�canique. Mais le nb de paires de poles est pris en compte dans le calcul de l'angle rotor.
tFloat const_calcul_RPM_Radian; //constante permettant l'estimation de l'angle rotor � partir de la vitesse.
			//Angle = Vitesse_RPM*pi/30*nb_paire_poles*T_FAST_LOOP


uint8_t Mode_Lecture_Courant_Angle; //= 0 --> angle rotor et courants de phase obtenus par mesure --> pour les essais sur
									//moteur.
									//=1 --> angle rotor et courants de phase obtenus par calcul (utilisation d'un mod�le
									//de moteur virtuel, donc ne convient pas pour des essais r�els).

/*****************************************************************************/
/* init_Cmd_Moteur_Variables                                                  */
/* Description: initialisation, � appeler au premier d�marrage, de tous les param�tres et signaux d'entr�e
 * de la commande du moteur, ainsi que des param�tres mesur�s (courants, angle et vitesse rotor).
 * La variable Moteur_Reel_Ou_Virtuel = MOTEUR_REEL ou MOTEUR_VIRTUEL, pour indiquer si on pilote r�ellement le moteur
 * avec l'utilisation de la vraie mesure de courant et d'angle, ou si on se base sur un mod�le virtuel (pas de pilotage
 * d'un vrai moteur, pas de capteur, d�di� uniquement � la validation du logiciel).
 */
/*****************************************************************************/
void init_Cmd_Moteur_Variables(uint8_t Moteur_Reel_Ou_Virtuel) {
	Consigne_Vitesse = 0;
	Commande_Vitesse = 0;
	D_Vitesse = MLIB_Mul_FLT(ACCELERATION_RAMPE_VITESSE,T_SLOW_LOOP_MS);
	Udq_Commande.fltArg1 = 0;
	Udq_Commande.fltArg2 = 0;
	Uabc_Commande.fltArg1 = 0;
	Uabc_Commande.fltArg2 = 0;
	Uabc_Commande.fltArg3 = 0;
	cptr_slow_loop = 0;
	Angle_Rotor_Estime = 0;
	nb_exec_FL_max_Exec_SL = T_SLOW_LOOP_MS*1000/T_FAST_LOOP_US;
	const_calcul_RPM_Radian = (tFloat) (0.1047*PAIRE_POLE*T_FAST_LOOP_US);
	Mode_Sequence_Moteur = MODE_FCT_CAL;
	Cptr_Mes_Cal = 0;

	//param�tres pour la PWM
	//initialisation de la commande PWM du GD3000
	PWMPeriod = FLEXPWM_CLK_MHz*T_FAST_LOOP_US;
	DutyDefaultGD3000 = PWMPeriod/2;

	//param�tres pour la mesure de courant
	Delay_Trig_us = 1; //d�lai entre trigger PWM et acquisition du courant low side par l'ADC (en �s)

	//initialisation des param�tres de mesure de l'angle et de la vitesse
	increment_angle_rotor = 0;
	angle_rotor_meca_precedent = 0;
	angle_rotor_elec = 0;
	angle_rotor_meca = 0;
	velocity_rotor_elec = 0;
	velocity_rotor_meca = 0;
	tps_integration_vitesse = (nb_exec_FL_max_Exec_SL-1)*T_FAST_LOOP_US; //exprim� en �s
	cte_calcul_RPM_from_deg_us = (tFloat) (60000000/360);
	cte_calcul_RPM_from_rad_us = (tFloat) (60000000/TWO_PI_FLT);

	//pour indiquer si on se base sur des mesures r�elles des courants de phase et de l'angle rotor --> essai sur moteur
	//r�el, ou si on se base sur un mod�le de moteur virtuel.
	//Mode_Lecture_Courant_Angle = MOTEUR_REEL ou MOTEUR_VIRTUEL
	Mode_Lecture_Courant_Angle = Moteur_Reel_Ou_Virtuel;

	//initialisation des variables et param�tres du mod�le de moteur virtuel
	if (Mode_Lecture_Courant_Angle == MOTEUR_VIRTUEL) {
		init_modele_moteur_virtuel();
	}
}

/*****************************************************************************/
/* raz_Entree_Cmd                                                  */
/* Description: remise � z�ro des commandes requises pour le contr�le de type U/f
 * --> uniquement la commande de vitesse. On met � 0 aussi les param�tres du mod�le du moteur
 * dans le cas o� l'on est mode simulation.
 * A utiliser apr�s un arr�t du moteur.
 */
/*****************************************************************************/
void raz_Entree_Cmd(void) {
	//raz de la rampe de vitesse
	Commande_Vitesse = 0;
	
	//raz des param�tres du mod�le du moteur qui stockent les valeurs de la vitesse, du couple, de la tension
	//de commande, du courant de phase � l'it�ration Fast Loop pr�c�dente.
	if (Mode_Lecture_Courant_Angle == MOTEUR_VIRTUEL) {
		raz_variables_moteur_virtuel();
	}
}


/*****************************************************************************/
/* Exec_Fast_Loop                                                  */
/* Description: cette fonction g�re l'ex�cution de la boucle dite Fast Loop.
 * Cette boucle calcule les tensions de phase (Ua,Ub,Uc) � partir des
 * consignes (Ud,Uq).
 * Les tensions Ud,Uq sont exprim�es en V.
 * Elle renvoie les rapports cycliques au format entier 16 bits.                                              */
/*****************************************************************************/
SWLIBS_3Syst_F16 Exec_Fast_Loop(SWLIBS_2Syst_FLT Udq_ref) {
	SWLIBS_3Syst_F16 Uabc_Duty_F16;
	SWLIBS_2Syst_FLT CosSinOut, AlphaBetaOut,DQ_Out;
	SWLIBS_3Syst_FLT PWM_abc_Out;

	DQ_Out.fltArg1 = Udq_ref.fltArg1;
	DQ_Out.fltArg2 = Udq_ref.fltArg2;

	Angle_Rotor_Estime = Estimate_Rotor_Angle(Commande_Vitesse,Angle_Rotor_Estime);

	//calcul du cos et du sin de l'angle rotor, n�cessaire au calcul de la transfo inverse de Park.
	//GFLIB_SINCOS_DEFAULT_FLT contient l'ensemble des coefs de de Taylor pour �valuer les fonctions
	//cos et sin par un polynome �quivalent. Voir p 440 (partie 5.33.3.5 doc AMMCLIB) pour un exemple
	//d'utilisation de cette fonction.
	GFLIB_SinCos_FLT(Angle_Rotor_Estime,&CosSinOut,GFLIB_SINCOS_DEFAULT_FLT);

	//fonction Park inverse pour exprimer les consignes de tension Ud,Uq dans le rep�re alpha_beta.
	GMCLIB_ParkInv_FLT(&AlphaBetaOut,&CosSinOut,&DQ_Out);

	//mapping des tensions Ualpha,Ubeta dans le rep�re abc en utilisant la standard space vector modulation
	//la sortie donne directement un rapport cyclique (valeur comprise entre 0 et 1, � condition que l'on ait
	//normalis� les tensions d'entr�es par rapport � la tension de phase max)
	AlphaBetaOut.fltArg1 = MLIB_Div_FLT(AlphaBetaOut.fltArg1,UPHASE_MAX_V);
	AlphaBetaOut.fltArg2 = MLIB_Div_FLT(AlphaBetaOut.fltArg2,UPHASE_MAX_V);

	//ClarkInv fournit un r�sultat compris entre -1 et +1.
	//Il faut donc le ramener entre 0 et 1 pour �tre compatible avec les valeurs de rapport
	//cycliques acceptables
	GMCLIB_ClarkInv_FLT(&PWM_abc_Out,&AlphaBetaOut);
	PWM_abc_Out.fltArg1 = MLIB_Add_FLT(PWM_abc_Out.fltArg1,1);
	PWM_abc_Out.fltArg2 = MLIB_Add_FLT(PWM_abc_Out.fltArg2,1);
	PWM_abc_Out.fltArg3 = MLIB_Add_FLT(PWM_abc_Out.fltArg3,1);
	PWM_abc_Out.fltArg1 = MLIB_Div_FLT(PWM_abc_Out.fltArg1,2);
	PWM_abc_Out.fltArg2 = MLIB_Div_FLT(PWM_abc_Out.fltArg2,2);
	PWM_abc_Out.fltArg3 = MLIB_Div_FLT(PWM_abc_Out.fltArg3,2);

	//Uabc_commande n'est utile qu'� des fins de debug (et pour l'utilisation du mod�le de moteur virtuel).
	Uabc_Commande.fltArg1 = MLIB_Mul_FLT(PWM_abc_Out.fltArg1,UCC_MAX_V);
	Uabc_Commande.fltArg2 = MLIB_Mul_FLT(PWM_abc_Out.fltArg2,UCC_MAX_V);
	Uabc_Commande.fltArg3 = MLIB_Mul_FLT(PWM_abc_Out.fltArg3,UCC_MAX_V);
	Uabc_Commande.fltArg1 = MLIB_Sub_FLT(Uabc_Commande.fltArg1,UPHASE_MAX_V);
	Uabc_Commande.fltArg2 = MLIB_Sub_FLT(Uabc_Commande.fltArg2,UPHASE_MAX_V);
	Uabc_Commande.fltArg3 = MLIB_Sub_FLT(Uabc_Commande.fltArg3,UPHASE_MAX_V);


	//conversion au format F16
	Uabc_Duty_F16.f16Arg1 = MLIB_ConvertPU_F16FLT(PWM_abc_Out.fltArg1);
	Uabc_Duty_F16.f16Arg2 = MLIB_ConvertPU_F16FLT(PWM_abc_Out.fltArg2);
	Uabc_Duty_F16.f16Arg3 = MLIB_ConvertPU_F16FLT(PWM_abc_Out.fltArg3);

	return(Uabc_Duty_F16);
}


/*****************************************************************************/
/* User_Speed_Ramp                                                  */
/* Description: cette fonction calcule le prochain point de vitesse de la rampe d�finissant
 * la consigne de vitesse pour le moteur. LA variable d'entr�e Previous_Speed_RPM d�finit la
 * vitesse en cours (en RPM). Speed_Consigne_RPM d�finit  * la vitesse finale � atteindre
 * (en RPM). delta_Speed d�finit la pente de la rampe de vitesse (en RPM/appel de cette fonction).
 * La fonction renvoie la valeur de la future vitesse de consigne. */
/*****************************************************************************/
tFloat User_Speed_Ramp(tFloat Previous_Speed_RPM,tFloat Speed_Consigne_RPM,tFloat delta_Speed_RPM) {
	tFloat Speed_Output;

	//Tant que la commande de vitesse n'est pas �gale � la consigne, on cherche � l'augmenter
	//(ou la diminuer) avec une acc�l�ration donn�e par ACCELERATION_RAMPE_VITESSE (en RPM).

	if (Previous_Speed_RPM <= Speed_Consigne_RPM) { //on doit acc�l�rer
		if ((MLIB_Sub_FLT(Speed_Consigne_RPM ,Previous_Speed_RPM)) < delta_Speed_RPM) {
			Speed_Output = Speed_Consigne_RPM;
		}
		else {
			Speed_Output = MLIB_Add_FLT(Previous_Speed_RPM,delta_Speed_RPM);
		}
	}
	else if (Previous_Speed_RPM > Speed_Consigne_RPM) { //on doit d�c�l�rer
		if ((MLIB_Sub_FLT(Previous_Speed_RPM,Speed_Consigne_RPM)) < delta_Speed_RPM) {
			Speed_Output = Speed_Consigne_RPM;
		}
		else {
			Speed_Output = MLIB_Sub_FLT(Previous_Speed_RPM,delta_Speed_RPM);
		}
	}
	return(Speed_Output);
}

/*****************************************************************************/
/* Exec_Slow_Loop                                                  */
/* Description: cette fonction g�re l'ex�cution de la boucle dite Slow Loop.
 * Cette boucle g�re la mise � g�re de la commande de vitesse, d�termin�e � partir de la
 * consigne de vitesse, puis calcule les r�f�rences de tension Ud et Uq � appliquer
 * au moteur.
 * Les tensions de phase (Ud,Uq) repr�sentent des tensions exprim�es en V.  */
/*****************************************************************************/
SWLIBS_2Syst_FLT Exec_Slow_Loop(void) {
	SWLIBS_2Syst_FLT Udq_calc;

	Udq_calc.fltArg1 = 0; //le flux est fix� � 0 !
	Udq_calc.fltArg2 = 0;

	//mise � jour de la commande de vitesse � partir de la consigne de vitesse courante.
	Commande_Vitesse = User_Speed_Ramp(Commande_Vitesse,Consigne_Vitesse,D_Vitesse);

	//calcul de la commande en tension Uq, � partir d'une look-up table
	//Set_pad_out(User_Test,OUT_ON);
	Udq_calc.fltArg2 = U_F_LUT(Commande_Vitesse);
	//Set_pad_out(User_Test,OUT_OFF);

	return(Udq_calc);
}

/*****************************************************************************/
/* U_F_LUT                                                  */
/* Description: cette fonction d�termine la tension Uq (en V) � appliquer au moteur pour
 * une commande de vitesse donn�e (Vitesse_in en RPM). S'apparente � une LUT.
 */
/*****************************************************************************/
tFloat U_F_LUT(tFloat Vitesse_in) {
	tFloat Uq_ref = 0;

	//Look-up table � cr�er !
	//Pour l'instant, la fonction renvoie une commande de r�f�rence nulle.

	return(Uq_ref);
}


/*****************************************************************************/
/*Estimate_Rotor_Angle                                                       */
/* Description: cette fonction d�termine l'angle du rotor � partir de la vitesse du rotor
 * exprim�e en RPM (Vitesse_RPM) et de la valeur pr�c�dente de l'angle rotor Angle_Rotor exprim� en rad.
 * Comme la commande est de type U/f, on ne lit pas l'angle � partir du r�solveur.
 * L'angle est compris entre [-pi et +pi] et exprim� en radians.                                        */
/*****************************************************************************/
tFloat Estimate_Rotor_Angle(tFloat Vitesse_RPM, tFloat Angle_rotor) {
	tFloat Angle_incr_urad, Angle_rotor_init_urad, Angle_rotor_fin_urad; //angles exprim�s en �radian.
	tFloat Angle_rotor_fin_rad; //exprim� en radian

	//Angle  (T0+dt) = Angle (T0) + vitesse_RPM * pi/30 * PP * TFST_LOOP, o� PP est le
	//nb de paires de p�les, T_FST_LOOP = dt = p�riode de la boucle Fast Loop. pi/30 = 0.1047.
	//on calcule d'abord l'incr�ment d'angle rotor produit par la vitesse du rotor pendant la dur�e T_FAST_LOOP
	Angle_incr_urad = MLIB_Mul_FLT(const_calcul_RPM_Radian,Vitesse_RPM); //comme T_FAST_LOOP_US est exprim�
		//en �s, Angl_incr est exprim�e en �rad.

	Angle_rotor_init_urad = MLIB_Mul_FLT(Angle_rotor,1000000);
	Angle_rotor_fin_urad = MLIB_Add_FLT(Angle_incr_urad,Angle_rotor_init_urad);

	if (Angle_rotor_fin_urad > PI_MICRORAD) {
		Angle_rotor_fin_urad = MLIB_Sub_FLT(Angle_rotor_fin_urad,TWO_PI_MICRORAD);
	}
	else if (Angle_rotor_fin_urad < -PI_MICRORAD) {
		Angle_rotor_fin_urad = MLIB_Add_FLT(Angle_rotor_fin_urad,TWO_PI_MICRORAD);
	}
	Angle_rotor_fin_rad = MLIB_Div_FLT(Angle_rotor_fin_urad,1000000);

	return(Angle_rotor_fin_rad);
}

/*****************************************************************************/
/* Convert_CmdDuty_F16_INT16                                                 */
/* Description: la fonction convertit les rapports cycliques des 3 commandes PWM
 * d�finies au format F16 (compris entre -1 et + 1, en des rapports cycliques au
 *  format int16, compatibles avec les registres du p�riph FlexPWM.
 * Leurs valeurs sont comprises entre 0 et PeriodPWM (= 1000). Les r�sultats de
 * sortie sont les variables globales Cmd_DutyA, Cmd_DutyB et Cmd_DutyC. */
/*****************************************************************************/
void Convert_CmdDuty_F16_INT16(SWLIBS_3Syst_F16 Cmd_Duty_F16) {
	uint32_t cmd_dutyA_32, cmd_dutyB_32, cmd_dutyC_32;

	//mise � jour des rapports cycliques envoy�s au FlexPWM. Il faut d'abord passer du format Q1.15 au
	//format uint16_t et mettre le chiffre entre 0 et 1000.
	cmd_dutyA_32 = PWMPeriod*Cmd_Duty_F16.f16Arg1;
	cmd_dutyB_32 = PWMPeriod*Cmd_Duty_F16.f16Arg2;
	cmd_dutyC_32 = PWMPeriod*Cmd_Duty_F16.f16Arg3;

	cmd_dutyA_32 = cmd_dutyA_32/0x7FFF;
	cmd_dutyB_32 = cmd_dutyB_32/0x7FFF;
	cmd_dutyC_32 = cmd_dutyC_32/0x7FFF;

	Cmd_DutyA = (uint16_t) (cmd_dutyA_32);
	Cmd_DutyB = (uint16_t) (cmd_dutyB_32);
	Cmd_DutyC = (uint16_t) (cmd_dutyC_32);
}



/*****************************************************************************/
/* ADC0_EOC_FAST_LOOP_isr                                                 */
/* Description: gestion de l'interruption li�e � une nouvelle acquisition de IphA, qui cadence
 * l'ex�cution de la boucle Fast Loop. Dans cette boucle, l'acquisition des courants et
 * la mise � jour des rapports cycliques pour la commance sont effectu�es.
 * On lance aussi la boucle dite Slow_loop de mise � jour de la vitesse de consigne
 * cptr_slow_loop < nb_exec_FL_max_Exec_SL.*/
/*****************************************************************************/
void ADC0_EOC_FAST_LOOP_isr(void) {
	SWLIBS_3Syst_F16 Cmd_Duty_F16;
	uint16_t angle_read;
	tFloat d_angle;
	uint8_t PhaseCurrentRead_OK;

	Set_pad_out(User_Test,OUT_ON);

	//FONCTIONNEMENT NORMAL : ex�cution normal du pilotage du moteur (ex�cution normale des slow et fast loop).
	if (Mode_Sequence_Moteur == MODE_FCT_NORM) {

		//angle rotor et courants de phase obtenus par mesure
		if (Mode_Lecture_Courant_Angle == MOTEUR_REEL) {
			//r�cup�ration des courants de phase par la lecture des tensions aux bornes des r�sistances de shunt
			//pour l'instant, la fonction ignore le cas d'une lecture erron�e des courants de phase.
			PhaseCurrentRead_OK = Get_ThreePhase_Current_MotorGD_FLT();

			//r�cup�ration de l'angle rotor
			//lecture de l'angle "m�canique" = position du rotor. Si on ex�cute la boucle slow loop,
			//on positionne la prochaine adresse � lire sur DEV_STAT6 (lecture de vitesse). Sinon,
			//sur DEV_STAT5 (lecture de la position)
			angle_read = pga411_ReadReg_CRC_connu_OneRead(DEV_STAT5,CRC_READ_REG_DEV_STAT5);

			//conversion de l'angle m�canique en angle "�lectrique", exprim� en radian compris entre -pi et +pi
			//pour �tre compatible avec les fonctions d'AMMCLIB
			angle_rotor_elec = Convert_AngleElec_Rad_FLT(angle_read,Resolution_PGA411,PAIRE_POLE);
			//angle_rotor_meca = Convert_AngleElec_Rad_FLT(angle_read,Resolution_PGA411,1);
			//il ne faut pas diviser par 3 l'angle rotor �lectrique pour obtenir l'angle rotor m�canique.
			//Cela a pour cons�quence de diviser par 3 la valeur de la vitesse m�canique.
			angle_rotor_meca = Convert_AngleMeca_Rad_FLT(angle_read,Resolution_PGA411);
		 }
		//angle rotor et courants de phase obtenus � l'aide d'un mod�le de moteur virtuel (pour des essais de validation
		//du code uniquement)
		else {
			Get_Phase_Current_Virtuel();
			Get_Angle_Rotor_Virtuel();
		}

		//ex�cution boucle fast loop
		Cmd_Duty_F16 = Exec_Fast_Loop(Udq_Commande);

		//conversion des rapports cycliques calcul�s par la fonction Exec_Fast_Loop (type F16) --> format
		//compatible avec le p�riph�rique FlexPWM.
		Convert_CmdDuty_F16_INT16(Cmd_Duty_F16);
		//mise � jour des rapports cycliques des 3 commandes PWM
		gd3000_Update_DutyCycle_Cmd_PWM_ABC(Cmd_DutyA,Cmd_DutyB,Cmd_DutyC);

		//�x�cution  de la boucle slow loop au bout de nb_exec_FL_max_Exec_SL ex�cutions de la Fast Loop pour
		// mettre � jour la commmande Udq et mise � jour de la commande de vitesse.
		//Entre temps, on acquiert la variation d'angle rotor pour d�duire la vitesse du rotor.
		if (cptr_slow_loop < nb_exec_FL_max_Exec_SL) {

			//int�gration de l'angle rotor m�canique parcouru depuis la derni�re ex�cution de la slow loop :
			if (cptr_slow_loop == 0) {
				angle_rotor_meca_precedent = angle_rotor_meca;
			}
			else {
				//ATTENTION AU PASSAGE 360 --> 0 ou l inverse !!
				d_angle = MLIB_Sub_FLT(angle_rotor_meca,angle_rotor_meca_precedent);

				if (d_angle < (-d_angle_rad_FST_LOOP_max)) {
					//d_angle = 360 + angle-angle_precedent
					d_angle = MLIB_Add_FLT((tFloat) (TWO_PI_FLT),d_angle);
				}
				else if (d_angle > d_angle_rad_FST_LOOP_max) {
					//d_angle = angle-angle_precedent-360
					d_angle = MLIB_Sub_FLT(d_angle,(tFloat) (TWO_PI_FLT));
				}

				increment_angle_rotor = MLIB_Add_FLT(increment_angle_rotor,d_angle);
				angle_rotor_meca_precedent = angle_rotor_meca;
			}

			cptr_slow_loop++;
		}
		else {
			cptr_slow_loop = 0;

			velocity_rotor_meca = MLIB_Div_FLT(increment_angle_rotor,tps_integration_vitesse); //vitesse en rad/us
			velocity_rotor_meca = MLIB_Mul_FLT(velocity_rotor_meca,cte_calcul_RPM_from_rad_us); //conversion en RPM
			increment_angle_rotor = (tFloat) (0);

			Udq_Commande = Exec_Slow_Loop();
		}
	}

	//MODE CALIBRATION INITIALE : on compense les offsets des mesures de courant de phase. On effectue un nb
	//NB_MAX_MES_CAL de mesures pour d�terminer l'offset en entr�e des points de mesure.
	else if (Mode_Sequence_Moteur == MODE_FCT_CAL) {
		Perform_Current_Meas_Calibration();
		if (Cal_Current_Done == CAL_DONE) {
			Mode_Sequence_Moteur = MODE_FCT_NORM; //passage en fonctionnement normal
		}
	}

	//RAZ du flag ADC0
	ADC_Clear_All_Flags(ADC0);
	Set_pad_out(User_Test,OUT_OFF);
}
