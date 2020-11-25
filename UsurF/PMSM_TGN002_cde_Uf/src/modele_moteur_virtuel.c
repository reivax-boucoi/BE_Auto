/*
 * modele_moteur_virtuel.c
 *
 *  Created on: 14 sept 2020
 *      Author: admin_aboyer
 *
 *  Fonctions associées à l'utilisation d'un modèle de moteur "virtuel" pour valider le fonctionnement
 *  général du logiciel embarqué.
 */

#include "modele_moteur_virtuel.h"
#include "Cmd_moteur.h"
#include "Current_Sensor.h"
#include "pga411.h"
#include "SWLIBS_Typedefs.h"
#include "gflib.h"
#include "gmclib.h"
#include "mlib.h"

extern SWLIBS_3Syst_FLT Uabc_Commande; //commande moteur dans le repère abc, exprimée en V (uniquement pour le test, pour afficher
//sous Freemaster les tensions de commande en V).
extern SWLIBS_3Syst_FLT Iphase; //courant dans les 3 phases (en A)
extern tFloat angle_rotor_elec; //en degré
extern tFloat angle_rotor_meca; //en degré --> ce qui est mesuré par le résolveur

//paramètres pour le modèle virtuel du moteur (calcul du courant de phase et de la vitesse rotor)
SWLIBS_3Syst_FLT Iphase_A_k, Iphase_A_k_1;  //courants de phase sur A, B et C, exprimés en A
tFloat tau_moteur;
tFloat Ts_div_tau_moteur;
tFloat terme1_calc_vitesse, terme2_calc_vitesse;
tFloat Cn_1, Wn_1;
tFloat tau_elec;
tFloat Ts_div_tau_elec;
SWLIBS_3Syst_FLT Uabc_n_1;
tFloat terme1_calc_courant, terme2_calc_courant;

/*****************************************************************************/
/* init_modele_moteur_virtuel                                               */
/* Description: initialisation des variables associées au modèle de moteur virtuel
 */
/*****************************************************************************/
void init_modele_moteur_virtuel(void) {
	tFloat tt;

	Iphase_A_k.fltArg1 = 0;
	Iphase_A_k.fltArg2 = 0;
	Iphase_A_k.fltArg3 = 0;
	Iphase_A_k_1.fltArg1 = 0;
	Iphase_A_k_1.fltArg2 = 0;
	Iphase_A_k_1.fltArg3 = 0;

	tau_moteur = MLIB_Div_FLT(J_inertie,B_frott);
	Ts_div_tau_moteur = MLIB_Div_FLT(T_FAST_LOOP_S,tau_moteur);
	terme1_calc_vitesse = MLIB_Add_FLT((tFloat) (2),Ts_div_tau_moteur);
	terme2_calc_vitesse = terme1_calc_vitesse;
	terme1_calc_vitesse = MLIB_Mul_FLT(terme1_calc_vitesse,J_inertie);
	terme1_calc_vitesse = MLIB_Div_FLT(T_FAST_LOOP_S,terme1_calc_vitesse);

	tt = MLIB_Sub_FLT(Ts_div_tau_moteur, (tFloat) (2));
	terme2_calc_vitesse = MLIB_Div_FLT(tt,terme2_calc_vitesse);
	Cn_1 = (tFloat) (0);
	Wn_1 = (tFloat) (0);
	Uabc_n_1.fltArg1 = (tFloat) (0);
	Uabc_n_1.fltArg2 = (tFloat) (0);
	Uabc_n_1.fltArg2 = (tFloat) (0);

	tau_elec = MLIB_Div_FLT(L_moteur,R_moteur);
	Ts_div_tau_elec = MLIB_Div_FLT(T_FAST_LOOP_S,tau_elec);
	terme1_calc_courant = MLIB_Add_FLT((tFloat) (2),Ts_div_tau_elec);
	terme2_calc_courant = terme1_calc_courant;
	terme1_calc_courant = MLIB_Mul_FLT(terme1_calc_courant,L_moteur);
	terme1_calc_courant = MLIB_Div_FLT(T_FAST_LOOP_S,terme1_calc_courant);

	tt = MLIB_Sub_FLT(Ts_div_tau_elec, (tFloat) (2));
	terme2_calc_courant = MLIB_Div_FLT(tt,terme2_calc_courant);
}

/*****************************************************************************/
/* raz_variables_moteur_virtuel                                              */
/* Description: raz des paramètres du modèle du moteur qui stockent les valeurs
 * de la vitesse, du couple, de la tension de commande, du courant de phase.
 */
/*****************************************************************************/
void raz_variables_moteur_virtuel(void) {
	Wn_1 = 0;
	Cn_1 = 0;
	Iphase_A_k_1.fltArg1 = 0;
	Iphase_A_k_1.fltArg2 = 0;
	Iphase_A_k_1.fltArg3 = 0;
	Iphase_A_k.fltArg1 = 0;
	Iphase_A_k.fltArg2 = 0;
	Iphase_A_k.fltArg3 = 0;
	Uabc_n_1.fltArg1 = 0;
	Uabc_n_1.fltArg2 = 0;
	Uabc_n_1.fltArg3 = 0;
}

/*****************************************************************************/
/* Get_Phase_Current_Virtuel                                                */
/* Description: cette fonction n'est valide que si on utilise le modèle virtuel
 * du moteur. La fonction met à jour la variable globale Iphase (courant de phase)
 * à partir des tensions de phase (la variable Uabc_Commande doit être correctement mise
 * à jour !) et du modèle électrique du moteur.
 *
 * Attention : ce modèle n'a pas vocation à prédire exactement les performances du vériable
 * moteur utilisé, mais plutôt vérifier que la structure logicielle du code fonctionne et n'induit pas
 * de défauts de commande. Le tout peut être vérifier sans moteur et gate driver.
 */
/*****************************************************************************/
void Get_Phase_Current_Virtuel(void) {
	tFloat terme1, terme2;

	terme1 = MLIB_Add_FLT(Uabc_Commande.fltArg1,Uabc_n_1.fltArg1);
	terme1 = MLIB_Mul_FLT(terme1,terme1_calc_courant);
	terme2 = MLIB_Mul_FLT(Iphase_A_k_1.fltArg1,terme2_calc_courant);
	Iphase_A_k.fltArg1 = MLIB_Sub_FLT(terme2,terme1); //courant de phase en A (on inverse le signe pour tenir compte
	 //de la lecture du courant à travers le shunt)

	terme1 = MLIB_Add_FLT(Uabc_Commande.fltArg2,Uabc_n_1.fltArg2);
	terme1 = MLIB_Mul_FLT(terme1,terme1_calc_courant);
	terme2 = MLIB_Mul_FLT(Iphase_A_k_1.fltArg2,terme2_calc_courant);
	Iphase_A_k.fltArg2 = MLIB_Sub_FLT(terme2,terme1); //courant de phase en A (on inverse le signe pour tenir compte
	 //de la lecture du courant à travers le shunt)

	terme1 = MLIB_Add_FLT(Uabc_Commande.fltArg3,Uabc_n_1.fltArg3);
	terme1 = MLIB_Mul_FLT(terme1,terme1_calc_courant);
	terme2 = MLIB_Mul_FLT(Iphase_A_k_1.fltArg3,terme2_calc_courant);
	Iphase_A_k.fltArg3 = MLIB_Sub_FLT(terme2,terme1); //courant de phase en A (on inverse le signe pour tenir compte
	 //de la lecture du courant à travers le shunt)

	Iphase = Iphase_A_k;
	Iphase_A_k_1 = Iphase_A_k;
	Uabc_n_1 = Uabc_Commande;
}

//void Get_Phase_Current_Virtuel(void) {
//		tFloat Rphase;
//
//		Rphase = MLIB_Mul_FLT(-1,R_moteur);
//		Iphase.fltArg1 = MLIB_Div_FLT(Uabc_Commande.fltArg1,Rphase);
//		Iphase.fltArg2 = MLIB_Div_FLT(Uabc_Commande.fltArg2,Rphase);
//		Iphase.fltArg3 = MLIB_Div_FLT(Uabc_Commande.fltArg3,Rphase);
//}

/*****************************************************************************/
/* Get_Angle_Rotor_Virtuel                                                */
/* Description: cette fonction n'est valide que si on utilise le modèle virtuel
 * du moteur. La fonction imite donc la lecture de l'angle par l'interface résolveur.
 * Elle met à jour les variables angle_rotor_meca et angle_rotor_elec.
 * Pour y parvenir, la vitesse rotor électrique est calculée à partir du couple théorique déduit du courant
 * de phase. La vitesse rotor méca n'est pas calculée, car elle déduite des lectures d'angle rotor électrique
 * les angles rotor méca et électrique théoriques à partir du couple théorique appliquée sur le rotor.
 * Cette fonction ne calcule pas la vitesse rotor !
 *
 * Attention : ce modèle n'a pas vocation à prédire exactement les performances du vériable
 * moteur utilisé, mais plutôt vérifier que la structure logicielle du code fonctionne et n'induit pas
 * de défauts de commande. Le tout peut être vérifier sans moteur et gate driver.
 */
void Get_Angle_Rotor_Virtuel(void) {
	tFloat Cn, terme1, terme2;
	SWLIBS_2Syst_FLT Idq_meas;
	SWLIBS_2Syst_FLT CosSinOut,Ialpha_beta_meas,DQ_Out;
	tFloat dAngle;
	tFloat Cres;
	tFloat V_rotor_elec;


	GFLIB_SinCos_FLT(angle_rotor_elec,&CosSinOut,GFLIB_SINCOS_DEFAULT_FLT);
	//on passe les courants de phase du repère abc au repère alpha-beta (exprimé en A)
	GMCLIB_Clark_FLT(&Ialpha_beta_meas,&Iphase);
	//on passe les courants de phase du repère alpha-beta au repère dq (exprimé en A)
	GMCLIB_Park_FLT(&Idq_meas,&CosSinOut,&Ialpha_beta_meas);


	//calcul du couple théorique (à max d'efficacité)
	Cn = MLIB_Mul_FLT(TROIS_DEMI_KI,Idq_meas.fltArg2);
	Cn = MLIB_Mul_FLT(Cn,(tFloat) (-1));  //pour compenser le fait que le courant mesuré est l'opposé du courant de phase

	Cres = MLIB_Mul_FLT(0.00023,Wn_1);
	Cn = MLIB_Sub_FLT(Cn,Cres);

	terme1 = MLIB_Add_FLT(Cn,Cn_1);
	terme1 = MLIB_Mul_FLT(terme1,terme1_calc_vitesse);
	terme2 = MLIB_Mul_FLT(Wn_1,terme2_calc_vitesse);
	V_rotor_elec = MLIB_Sub_FLT(terme1,terme2); //vitesse électrique du rotor en rad/s
	Cn_1 = Cn;
	Wn_1 = V_rotor_elec;

	dAngle = MLIB_Mul_FLT(T_FAST_LOOP_S,V_rotor_elec);
	angle_rotor_elec = MLIB_Add_FLT(angle_rotor_elec,dAngle);
	angle_rotor_elec = Adapt_Angle_minus_pi_plus_pi_FLT(angle_rotor_elec);

	dAngle = MLIB_Div_FLT(dAngle,PAIRE_POLE);
	angle_rotor_meca = MLIB_Add_FLT(angle_rotor_meca,dAngle);
	angle_rotor_meca = Adapt_Angle_minus_pi_plus_pi_FLT(angle_rotor_meca);
}
