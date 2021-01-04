/*
 * modele_moteur_virtuel.h
 *
 *  Created on: 14 sept 2020
 *      Author: admin_aboyer
 *
 *  Fonctions associ�es � l'utilisation d'un mod�le de moteur "virtuel" pour valider le fonctionnement
 *  g�n�ral du logiciel embarqu�.
 */

#ifndef MODELE_MOTEUR_VIRTUEL_H_
#define MODELE_MOTEUR_VIRTUEL_H_

//param�tres du moteur
#define PAIRE_POLE 3 //nb de paires de p�les

//param�tres pour mod�liser le moteur
#define J_inertie 0.00007 //kg.m�
#define B_frott 0.000229 //N.m/rad/s
#define TROIS_DEMI_KI 0.06 //3/2 * constante de couple du moteur
#define L_moteur 0.00021 //H
#define R_moteur 0.215 //ohm

void init_modele_moteur_virtuel(void);
void raz_variables_moteur_virtuel(void);
void Get_Phase_Current_Virtuel(void);
void Get_Angle_Rotor_Virtuel(void);

#endif /* MODELE_MOTEUR_VIRTUEL_H_ */
