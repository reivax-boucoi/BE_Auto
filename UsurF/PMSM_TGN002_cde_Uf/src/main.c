/*
 * main.c
 *
 *  Created on: 20 nov. 2018
 *  Updated on: 17 nov. 2020
 *      Author: admin_aboyer
 *
 *  Application : pilotage du moteur PMSM TG Drives par un commande U/f.
 *  Le moteur est commandé à l'aide du gate driver GD3000. Les courants de phase
 *  sont mesurés à l'aide de résistance de shunt de 0.01 ohm en pied de MOSFET.
 *  La vitesse et la position du rotor (non indispensable pour la commande U/f) sont
 *  mesurées à l'aide de l'interface résolveur PGA411.
 *  La fonction main assure l'initialisation des différents composants et des paramètres
 *  de la commande moteur. L'application présente 2 modes, commandés par le bouton SW1 de la
 *  carte DEVKIT-MPC5744P :
 *  > MODE_OFF : le moteur est à l'arrêt.
 *  > MODE_ON : le moteur tourne selon la consigne de vitesse définie par la rampe de vitesse
 *     utilisateur
 *
 *  Freemaster est utilisé pour lire en temps réel l'évolution des variables globales du programme.
 *
 *  Ce programme offre la possibilité de basculer sur un fonctionnement avec un modèle de moteur virtuel -->
 *  pas besoin de travailler avec le gate driver ou le PGA411. Ce modèle a uniquement comme vocation de vérifier
 *  que le code est valide logiciellement.
 *  Selon la loi U/F utilisée, le modèle du moteur peut converger vers une vitesse proche de la vitesse voulue, ou
 *  diverger.
 *  Pour passer en fonctionnement utilisation du modèle virtuel de moteur, il suffit de définir la valeur de la
 *  variable Mode_Lecture_Courant_Angle (dans init_Cmd_Moteur_Variables()), soit à MOTEUR_REEL, soit à MOTEUR_VIRTUEL.
 */

#include "MPC5744P.h"
#include "derivative.h" /* include peripheral declarations */
#include "typedefs.h"
#include "ClockConfig.h"
#include "ModeEntryAndConfig.h"
#include "ConfigGPIO.h"
#include "ConfigPIT.h"
#include "Cmd_moteur.h"
#include "SWLIBS_Typedefs.h"
#include "gd3000.h"
#include "pga411.h"
#include "ConfigADC.h"
#include "ConfigCTU.h"
#include "Current_Sensor.h"
#include "UARTConfig.h"
#include "freemaster.h"
#include "User_Interface.h"
#include "ConfigPWM.h"

//définition des deux modes de fonctionnement de l'application
#define MODE_OFF 0
#define MODE_ON 1

extern void xcptn_xmpl(void);

//variables globales
uint8_t appui_SW1 = 0;  //pour repérer un appui sur SW1 --> marche-arrêt moteur. Permet aussi d'identifier
		//s'il s'agit du premier démarrage.
uint8_t Premier_Demarrage = 1; //pour différencier le premier démarrage, des démarrages suivant des arrêts
tFloat MyVitesse_for_Consigne = 300; //vitesse finale de la rampe de vitesse (en RPM)
tFloat Consigne_Vitesse; //la consigne de vitesse, qui représente la vitesse à atteindre. Déclaré dans
   //Cmd_moteur.c comme variable externe
extern tFloat Commande_Vitesse; //la commande de vitesse en cours
uint8_t Mode; //mode de fonctionnement de l'application : 0 = commande PWM off, 1 = commande PWM on
extern uint8_t Mode_Sequence_Moteur;

extern uint32_t Delay_Trig_us; //délai en µs entre le trigger issu de la PWM (dépassement VAL1 pour déclencher l'acquisition
//à partir du moment où les 3 phases sont à 0) et la commande d'échantillonnage envoyée par le CTU à l'ADC.

//paramètres de l'interface du résolveur
uint8_t Resolution_PGA411; //résolution de la mesure de l'angle du rotor

//variable globale donnant l'état des différents composants
uint8_t ConfigGD3000_OK; //état général du GD3000
uint8_t Current_sensor_OK; //état général du capteur de courant (en fait, seuls ADC0 etADC1 sont testés)
uint8_t Config_PGA411_OK; //état général du PGA411

/*****************************************************************************/
/* Init_MCU                                                         */
/* Description: initialisation du microcontrôleur (clock and mode transition)	*/
void init_MCU_ClockAndMode(void) {
	/***************************************************************/
	/* Configuration of the peripheral, system clocks and modes    */
	/***************************************************************/
	/* System clock = 200 MHz, MOTC = 40 MHz, PBRIDGE_CLK = 50 MHz */
	/* MOTC clock est issu de PLL0                       */
	/****************************************************************/
	peri_clock_gating();     /* Config gating/enabling peri. clocks for modes*/
		                             /* Configuraiton occurs after mode transition! */
	DivideSystClock(3); //on divise par 4 la clock source PBRIDGEx_CLK utilisé par DSPI(200/4 = 50 MHz)
	ConfigMotCClock(PLL0AUX0CLK,4); //on fait tourner la clock MOTC à 40 MHz à partir de PLL0 --> division par 5
	ConfigADCClock(PLL0AUX0CLK,4); //on divise par 5 la clock source d'ADC_clk (40 MHz)

	//configuration of PLL0
	//PLL0 : clk_in = 40MHz (osc quartz)
	//PHI0 = 200 MHz et PHI1 = 50 MHz (pour réf PLL1)
	//Prediv = 2, MFD = 20, RFDPHI = 2, RFDPHI1 = 8
	ConfigPLL0(PLL0_SRC_XOSC,2,20,2,8);

	//configuration des modes de fonctionnement : ici, uniquement mode Run1.
	//si un oscillateur ou une PLL n'a pas démarré, on reste bloqué dans la fonction ModeConfig.
	ModeConfig(RUN1MODE,PLL0SYSCLK,IRC_ON,XOSC_ON,PLL0_ON,PLL1_OFF); //clock source = PLL0_PHI
	//autorisation des modes
	ModeEnable(RUN1_ON, RUN2_OFF, RUN3_OFF, HALT0_OFF, TEST_OFF);
	ModeTransition(RUN1MODE);
}


/*****************************************************************************/
/* init_MCU_periph                                                         */
/* Description: initialisation des périphériques du microcontrôleur
 * hormis ceux dédiés au pilotage de l'interface du résolveur (DSPI)
 */
void init_MCU_periph(void) {
	/***************************************************************/
	/* Configuration de l'UART et de Freemaster                    */
	/***************************************************************/
	/* configuration du LINFlexD_1 en UART --> utiliser pour la comm SCI */
	Config_LINFlex1_UART();
	/* FreeMASTER internal variables initialization */
	FMSTR_Init();
}


/*****************************************************************************/
/* init_ISR_appli                                                     */
/* Description: initialisation et config des routines d'interruption de l'application	     */
void init_ISR_appli(void) {
	/***************************************************************/
	/* Interruptions                                               */
	/***************************************************************/
	//Prise en compte de l'interruption lié à l'appui sur le bouton SW1 (ERQ30)
	//ajout de ERQ_SW1_isr dans la table des vecteurs d'interruption (n°246) --> intc_SW_mode_isr_vectors_MPC5744P.c
	INTC_0.PSR[246].R = 0x800B; //PRC_SELN0 = 1 --> IT sent to processor 0
								//PRIN = 10 --> priority level (up to 31 levels, 0 is the lowest)

	//Prise en compte de l'interruption lié à l'acquisition par ADC0 (IphA)
	//ajout de ADC0_EOC_isr dans la table des vecteurs d'interruption (n°496) --> intc_SW_mode_isr_vectors_MPC5744P.c
	INTC_0.PSR[496].R = 0x800C; //PRC_SELN0 = 1 --> IT sent to processor 0
								//PRIN = 11 --> priority level (up to 31 levels, 0 is the lowest)

	//Prise en compte des interruptions liées au LINFlex1
	//on donne le niveau de priorité 5 aux ISR liées à RX et TX LINFlex_1.
	INTC_0.PSR[380].R = 0x8005;
	INTC_0.PSR[381].R = 0x8005;

	//Prise en compte de l'interruption externe liée à la broche INT du GD3000 (ERQ14) et celle liée à
	//OC_OUT du GD3000 (ERQ13). Partage le même vecteur d'interruption
	//ajout de ERQ_GD_INT_OCOUT_isr dans la table des vecteurs d'interruption (n°244) --> intc_SW_mode_isr_vectors_MPC5744P.c
	//Pour l'instant, desactivé.
	//INTC_0.PSR[244].R = 0x800D; //PRC_SELN0 = 1 --> IT sent to processor 0
										//PRIN = 10 --> priority level (up to 31 levels, 0 is the lowest)

	//Prise en compte de l'interruption externe liée à la broche Fault du PGA411 (ERQ21)
	//ajout de ERQ_PGA411_Fault_isr dans la table des vecteurs d'interruption (n°245) --> intc_SW_mode_isr_vectors_MPC5744P.c
	INTC_0.PSR[245].R = 0x800D; //PRC_SELN0 = 1 --> IT sent to processor 0
										//PRIN = 10 --> priority level (up to 31 levels, 0 is the lowest)
}


int main(void)
{
	/*Initialisation et configuration     */
	xcptn_xmpl ();              /* Configure and Enable Interrupts */
	init_MCU_ClockAndMode();
	init_MCU_periph();

	/***************************************************************/
	/* Initialisation des paramètres de la commande moteur         */
	/***************************************************************/
	init_Cmd_Moteur_Variables(MOTEUR_REEL); //MOTEUR_REEL ou MOTEUR_VIRTUEL

	/***************************************************************/
	/* Initialisation des composants extérieurs (gate driver, current*/
	/* sensor, interface résolveur.                                */
	/***************************************************************/
	//configuration du gate driver GD3000 (effet d'un dysfonctionnement observé en configuration
	//non pris en charge par ce programme : variable GD3000_Status)
	ConfigGD3000_OK = gd3000_Config_Init();
	//Configuration de la mesure de courant
	Current_sensor_OK = Current_sensor_Config_Init(Delay_Trig_us);
	//configuration de l'interface résolveur PGA411
	Config_PGA411_OK = pga411_Config_Init();
	//réglage de sa résolution
	Resolution_PGA411 = pga411_set_resolution(RESO_12BITS);

	//en cas d'erreur de configuration, on allume la LED rouge. Pour l'instant, on n'inhibe pas
	//l'allumage du moteur.
	if ((ConfigGD3000_OK & Current_sensor_OK & Config_PGA411_OK) == 0) {
		Set_pad_out(User_LED_Red,LED_ON);
	}
	else {
		Set_pad_out(User_LED_Red,LED_OFF);
	}

	init_User_Interface();
	init_ISR_appli();


	/* Loop forever */
	for(;;) {	   
	   	/* call function periodically*/
	   	FMSTR_Poll();
	}
}

/*****************************************************************************/
/* ERQ_SW1_isr                                                               */
/* Description: gestion de l'interruption liée à l'appui sur le boutons
 * user SW1 (même source d'interruption), qui définit les transitions entre les deux modes
 * de la machine à état de l'application.
 * L'appuis sur SW1 fait basculer du mode MODE_OFF au mode MODE_ON, et inversement.
 */
/*****************************************************************************/
void ERQ_SW1_isr(void) {
	//si appui sur SW1
	if (SIUL2.DISR0.B.EIF30 == 1) {
		//RAZ du flag
		SIUL2.DISR0.B.EIF30 = 1;

		if (Mode == MODE_OFF) {
			Mode = MODE_ON;
			Set_pad_out(User_LED_Green,LED_ON);
			//on donne la valeur à la consigne de vitesse
			Consigne_Vitesse = MyVitesse_for_Consigne;

			if (appui_SW1 == 0) {
				//on réalise la calibration des ADC (compensation des offsets liés à la lecture
				//du courant).
				Mode_Sequence_Moteur = MODE_FCT_CAL;

				//activation des horloges dans le module FlexPWM0
				gd3000_RunPWM_BrasABC();
				//lancement de la PWM :
				//Activation de la PWM avec un rapport cyclique de 50 % sur les 3 sorties
				//--> une tension moyenne nulle sur chacune des phases du moteur.
				gd3000_Update_DutyCycle_Cmd_PWM_ABC(500,500,500);
				DelayTimerGeneral_ms(10);
			}
			else {
				//lancement de la PWM :
				//Activation de la PWM avec un rapport cyclique de 50 % sur les 3 sorties
				//--> une tension moyenne nulle sur chacune des phases du moteur.
				gd3000_Update_DutyCycle_Cmd_PWM_ABC(500,500,500);
				DelayTimerGeneral_ms(10);

				//réactivation des sorties PWM du GD3000 après un arrêt
				gd3000_Inhib_Pad_Enable();
				gd3000_Toggle_Pins_Init();
				//relance de la synchro FlexPWM --> ADC
				Start_PWM0_Trigger();
			}
			appui_SW1 ++;
		}

		else if (Mode == MODE_ON) {
			Mode = MODE_OFF;
			Set_pad_out(User_LED_Green,LED_OFF);
			//arrêt du moteur immédiat :

			//raz des consignes et commande de vitesse :
			Consigne_Vitesse = 0;
			raz_Entree_Cmd();

			//mise à jour de la commande PWM pour assurer une tension de phase nulle entre les phases.
			gd3000_Update_DutyCycle_Cmd_PWM_ABC(900,900,900);
			//arrêt synchro PWM --> ADC (arrêt déclenchement fast loop)
			Stop_PWM0_Trigger();
			//desactivation des sorties PWM du GD3000.
			gd3000_Inhib_Pad_Enable();
		}
	}
}

