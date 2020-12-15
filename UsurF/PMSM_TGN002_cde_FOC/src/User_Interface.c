/*
 * User_Interface.c
 *
 *  Created on: 2 oct. 2018
 *      Author: admin_aboyer
 *
 *  Gestion des boutons + afficheurs + I/O de test
 */

#include "MPC5744P.h"
#include "User_Interface.h"
#include "ConfigGPIO.h"

/*****************************************************************************/
/* init_User_Interface                                                       */
/* Description: initialisation des I/O liées à l'interface utilisateur :
 * Bouton SW1, LED_rouge et sortie User_Test.
 */
void init_User_Interface(void) {
	/***************************************************************/
	/* Initialisation des boutons SW1 et interruptions             */
	/* associées (vecteur 246)                                     */
	/***************************************************************/
	Config_pad_in(User_SW1,PULL_ON,PULL_DOWN,HYST_ON);
	Config_Alter_Function_In(203,1); //on active la fonction External interrupt source 30 associée à PF12
	Config_ERQ_Pad(30, ERQ_RISING_EVENT_ON, ERQ_RISING_EVENT_OFF,ERQ_FILTER_ON);

	//LED rouge du kit DEVKIT_MPC5744P indique un défaut de fonctionnement de l'application
	Config_pad_out(User_LED_Red,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,LED_OFF);
	//LED verte du kit DEVKIT_MPC5744P indique le mode de fonctionnement de l'application
	Config_pad_out(User_LED_Green,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,LED_OFF);
	//LED bleu du kit DEVKIT_MPC5744P : pour l'instant, aucun rôle spécifique
	Config_pad_out(User_LED_Blue,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,LED_OFF);

	//pin de test User_Test : faire commuter (0 - 1 - 0) pour mesurer le temps d'exécution
	//d'une partie du code.
	Config_pad_out(User_Test,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_OFF);
}
