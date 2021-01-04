/*
 * User_Interface.h
 *
 *  Created on: 2 oct. 2018
 *      Author: admin_aboyer
 *
 *  Gestion des boutons + afficheurs + I/O de test
 */


#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_

//d�finition des deux boutons utilisateurs : SW1 met en route le moteur ou l'�teint
#define User_SW1 PF12
#define User_SW2 PF13
//d�finition des LEDs pr�sentes sur la carte DEVKIT-MPC5744P
#define User_LED_Red PC11
#define User_LED_Green PC12
#define User_LED_Blue PC13
//d�finition de la sortie digitale de test PA14 (J6-06)
#define User_Test PA14

void init_User_Interface(void);

#endif /* USER_INTERFACE_H_ */
