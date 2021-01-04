/*
 * Current_Sensor.h
 *
 *  Created on: 16 août 2018
 *      Author: Alexandre Boyer
 *
 *  Fonctions liées à la lecture des courants de phase (mise en oeuvre matérielle + calculs)
 */

#include "SWLIBS_Typedefs.h"
#include "typedefs.h"

#ifndef CURRENT_SENSOR_H_
#define CURRENT_SENSOR_H_


#define PHASE_A 0
#define PHASE_B 1
#define PHASE_C 2

#define Pad_IphA PB9
#define Pad_IphB PB10
#define Pad_IphC PB11

//constantes liées aux ADC utilisées pour les lectures des courants
#define ADC_MOD_IphA 0 //ADC0 est utilisé pour la lecture de IphA
#define ADC_MOD_IphB 1 //ADC1 est utilisé pour la lecture de IphB
#define ADC_CH_IphA 11 //ADC0_AN11 est utilisé pour la lecture de IphA
#define ADC_CH_IphB 12 //ADC1_AN12 est utilisé pour la lecture de IphB

//constantes liées au CTU utilisé (déclenchement acquisition par les ADC) : ici, on utilise seulement CTU0
//définition des entrées de trigger du bloc Trigger Generator Subunit
#define Trigger_Input_MRS 0 //entrée qui va produire le MRS du CTU en mode séquentiel : le master reload
			//de FlexPWM0
#define Trigger_Input_ChA 5 //entrée utilisée = OUT_TRIG1 du sous-module 0 de FlexPWM0 pour la lecture
          //de IphA
#define Trigger_Input_ChB 6 //entrée utilisée = OUT_TRIG1 du sous-module 1 de FlexPWM0 pour la lecture
          //de IphB
//définition des sorties de trigger du bloc Trigger Generator Subunit (et donc entrées du bloc scheduler subunit
#define Trigger_Output_MRS 0 //sortie trigger lié au MRS du CTU en mode séquentiel : le master reload
			//de FlexPWM0
#define Trigger_Output_ChA 1 //sortie trigger lié au OUT_TRIG1 du sous-module 0 de FlexPWM0 pour la lecture
          //de IphA
#define Trigger_Output_ChB 2 //sortie trigger lié au OUT_TRIG1 du sous-module 1 de FlexPWM0 pour la lecture
          //de IphB

#define CTUCptrPrescaler 0b11 //valeur du prescaler pour l'horloge du compteur (0b11 --> 4) --> 10 MHz
#define CTUInitCptr 0 //valeur initiale du compteur
#define CTUMaxCptr 990 //valeur max du compteur (doit être supérieure à CTUTrigDelay, mais inférieure
                //à la période de rafraichissement de la PWM (Tpwm = 100 µs --> 1000 CTU clock)
#define CTUTrigDelay 50	//clock compteur = 100 ns --> pour un délai de 5 µs, il faut 50 périodes d'horloge.


//constantes liées à l'utilisation de la DMA pour la récupération des résultats de conversion des ADC
#define FIFO_ConvResult_IphA 0 //n° du FIFO dans lequel on stocke les résultats de conversion de IphA.
#define FIFO_ConvResult_IphB 1 //n° du FIFO dans lequel on stocke les résultats de conversion de IphB.

#define DMA_CH_Number_IphA 0 //on utilise le canal DMA 0 pour gérer les requêtes du FIFO0 de CTU0 (conversion de IphA)
#define DMA_CH_Number_IphB 1 //on utilise le canal DMA 1 pour gérer les requêtes du FIFO0 de CTU0 (conversion de IphA)

#define Adr_DMA_IphA 0x50800000 //là où on récupère le résultat de conversion de IphA, récupéré par DMA
#define Adr_DMA_IphB 0x50800010 //là où on récupère le résultat de conversion de IphB, récupéré par DMA

//paramètres pour les conversions tensions mesurées --> courants
#define U_PLEINE_ECH_mV 3300 //tension de pleine échelle des ADC, exprimée en mV
#define ADC_NB_BIT 12//nb de bits de l'ADC
#define ADC_RESOLUTION_uV 806 //résolution de l'ADC en uV = 1000*U_PLEINE_ECH_mV/2^ADC_NB_BIT = 805.6 µV
#define OFFSET_Iph_MEAS_uV 1650000 //offset pour la lecture des courants de phase, exprimé en mV
#define OFFSET_Iph_MEAS_mV 1650 //offset pour la lecture des courants de phase, exprimé en mV
#define GAIN_Iph_MEAS_VV 8 //gain en V/V pour la lecture des courants de phase
#define GAIN_Iph_MEAS_mVuV 8000 //gain en mV/µV pour la lecture des courants de phase
#define GAIN_RES_Iph_MEAS_mVuV 80 //gain + effet résistance de shunt en mV/µV pour la lecture des courants de phase
#define GAIN_RES_Iph_MEAS_VuV 80000 //gain + effet résistance de shunt en V/µV pour la lecture des courants de phase
//paramètres liées à l'acquisition des courants de phase
#define RSHUNT_Iph 10 //en milliohms

//paramètres de calibration des mesures de courant
#define NB_MAX_MES_CAL 20 //nb de mesures max pour réaliser la calibration en offset
#define CAL_NOT_DONE 0
#define CAL_DONE 1

#define CURRENT_SENSOR_STATUS_FAULT 0
#define CURRENT_SENSOR_STATUS_OK 1


uint8_t Current_sensor_Config_Init(uint32_t Delay_Trigger_us);
uint8_t Config_Init_ADC_IPhase(uint8_t ADC_Num, uint8_t ADC_Ch_Num, uint8_t Pad_Mes);
void Config_CTU0_IPhaseA(uint32_t Delay_Trigger_us);
void Config_CTU0_IPhase_A_B(uint32_t Delay_Trigger_us);
uint8_t Get_Iphase_MotorGD_FLT(uint8_t Phase_Nb, uint8_t ADC_Num, uint8_t ADC_Ch_Num);
void Deduce_Iphase_C_FLT(void);
uint8_t Get_ThreePhase_Current_MotorGD_FLT(void);
void Perform_Current_Meas_Calibration(void);
tFloat Compensate_Iphase_MotorGD_FLT(tFloat Meas_Iph,tFloat offset_meas_uV,tFloat gain_meas_V_uV);



#endif /* CURRENT_SENSOR_H_ */
