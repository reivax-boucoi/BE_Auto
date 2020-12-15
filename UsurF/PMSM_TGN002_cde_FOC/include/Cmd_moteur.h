/*
 * Cmd_moteur.h
 *
 *  Created on: 2 oct. 2018
 *      Author: admin_aboyer
 *
 *  Fonctions associées au contrôle moteur, basé sur une commande U/f.
 */

#include "SWLIBS_Typedefs.h"
#include "typedefs.h"

#ifndef CMD_MOTEUR_H_
#define CMD_MOTEUR_H_



//les limites de vitesse, de tension, de rapport cyclique : les tensions sont exprimées en mV, les vitesses
//en RPM et les rapports cycliques en partie pour mille ! Les angles sont exprimés en microradian.
//Les accélérations sont exprimées en RPM/millisecondes.
#define VITESSE_MAX 4000 //en RPM
#define UCC_MAX_mV 18000 //en mV
#define UCC_MAX_V 18 //en V
#define UPHASE_MAX_mV 9000 //en mV
#define UPHASE_MAX_V 9 //en V
#define DUTY_CYCLE_MAX 1000 //en partie pour mille
#define IMES_MAX 21 //courant max mesurable sur la carte MOTORGD en A (sous alim 3.3 V)
#define PI_MICRORAD 3141590 //pi exprimé en µradian
#define TWO_PI_MICRORAD 6283180 //2*pi exprimé en µradian
#define PI_FLT 3.14159265 //pi exprimé en flottant
#define TWO_PI_FLT 6.28318531 //2*pi exprimée en flottant
#define THREE_PI_FLT 9.424778 //3*pi exprimée en flottant
#define FOUR_PI_FLT 12.566371 //4*pi exprimée en flottant
#define FIVE_PI_FLT 15.707963 //5*pi exprimée en flottant
#define SIX_PI_FLT 18.849556 //6*pi exprimée en flottant
#define MINUS_PI_FLT -3.14159265 //-pi exprimé en flottant

//paramètres du moteur
#define PAIRE_POLE 3 //nb de paires de pôles

//paramètres de la commande moteur
#define T_SLOW_LOOP_MS 1.2 //période de la boucle dite slow loop (en ms)
#define T_FAST_LOOP_US 120 //période de la boucle dite fast loop (en µs)
#define T_SLOW_LOOP_S 0.0012 //période de la boucle dite slow loop (en s)
#define T_FAST_LOOP_S 0.00012  //période de la boucle dite fast loop (en s)
#define ACCELERATION_RAMPE_VITESSE 0.25 //accélération (ou décelération) max exprimée en RPM/ms
     //ex. --> 3 RPM/ms = 3000 RPM/sec. Ce chiffre est toujours positif.

//mode de fonctionnement du moteur
#define MODE_FCT_NORM 0 //mode de fonctionnement normal, exécution normal des boucles slow et fast loop
#define MODE_FCT_CAL 1 //mode de calibration des offset de tension des mesures analogiques. Mode par défaut
			//au démarrage.

//paramètres pour la lecture du courant et de la vitesse
#define d_angle_FST_LOOP_max 10 //incrément d'angle max entre 2 itération de FAST LOOP (=120µs)
#define d_angle_rad_FST_LOOP_max 0.1745329 //incrément d'angle max entre 2 itération de FAST LOOP (=100µs)
//Vitesse max pour TG Drives = 12000 RPM, soit un angle de 12000/60(RPS)*120us*360 = 8.64°. On
//arrondit à 10. Ca va nous aider à détecter un franchissement par le rotor de 0° - 360°

//pour indiquer si on travaille avec un vrai moteur (utilisation du GD3000 pilotant un véritable PMSM) ou avec un modèle
//mathématique du comportement électromécanique du moteur (pour vérifier le bon comportement logiciel du programme)
#define MOTEUR_REEL 0
#define MOTEUR_VIRTUEL 1

void init_Cmd_Moteur_Variables(uint8_t Moteur_Reel_Ou_Virtuel);
void raz_Entree_Cmd(void);
SWLIBS_3Syst_F16 Exec_Fast_Loop(SWLIBS_2Syst_FLT Udq_ref);
tFloat User_Speed_Ramp(tFloat Previous_Speed_RPM,tFloat Speed_Consigne_RPM,tFloat delta_Speed_RPM);
SWLIBS_2Syst_FLT Exec_Slow_Loop(void);
tFloat U_F_LUT(tFloat Vitesse_in);
tFloat Estimate_Rotor_Angle(tFloat Vitesse_RPM,tFloat Angle_rotor);
void Convert_CmdDuty_F16_INT16(SWLIBS_3Syst_F16 Cmd_Duty_F16);


#endif /* CMD_MOTEUR_H_ */
