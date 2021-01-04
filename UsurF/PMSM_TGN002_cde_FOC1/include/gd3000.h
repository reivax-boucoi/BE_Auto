/*
 * gd3000.h
 *
 *  Created on: 1 oct. 2018
 *      Author: admin_aboyer
 *
 *  Ensemble des fonctions de configuration et d'utilisation du gate driver GD3000.
 */

#include "typedefs.h"

#ifndef GD3000_H_
#define GD3000_H_

//structure contenant les registres de diagnostic du GD3000
typedef struct {
	uint8_t GD3000_Failed;
	uint32_t Status0;
	uint32_t Status1;
	uint32_t Status2;
	uint32_t Status3;
} GD3000_Status_struct;

#define Pad_Enable PA3 //sur la carte MOTORGD, les 2 broches Enable1 et Enable2 sont raccordées ensemble
#define Pad_Reset PA4
#define Pad_OCOUT PA14
#define Pad_INT PA15
#define PA_LS PA10
#define PA_HS PA11
#define PB_LS PD14
#define PB_HS PF0
#define PC_LS PA13
#define PC_HS PA12
#define Pad_IphA PB9
#define Pad_IphB PB10
#define Pad_IphC PB11
#define Pad_Lecture_VphA PA6
#define Pad_Lecture_VphB PA7
#define Pad_Lecture_VphC PA8

#define Inhib_Enable 0
#define Activ_Enable 1
#define Inhib_Reset 1 //reset est actif sur état bas
#define Activ_Reset 0

//paramètres pour la communication SPI
#define GD3000_SPI_MODULE Module_DSPI0
#define CTAR_nb_GD3000 0
#define PCS_nb_GD3000 0
#define PCS_asserted_GD3000 CS0_Asserted

//registres internes du GD3000
#define GD3000_NULL_REG 0x00
#define GD3000_MASK0_REG 0x20 //par défaut, toutes les interruptions sont inhibées
#define GD3000_MASK1_REG 0x30 //par défaut, toutes les interruptions sont inhibées
#define GD3000_MODE_REG 0x40 //par défaut, la config est celle par défaut (celle après reset)
#define GD3000_CLINT0_REG 0x6F //les 4 bits de poids faibles sont affectés à 1 pour remettre à zero les flags d'interruption
#define GD3000_CLINT1_REG 0x7F //les 4 bits de poids faibles sont affectés à 1 pour remettre à zero les flags d'interruption
#define GD3000_DEAD_REG 0x80 //les 4 bits de poids faibles sont affectés à 0 pour configuration zero dead time (par défaut)
#define GD3000_STATUS0_REG 0x00
#define GD3000_STATUS1_REG 0x01
#define GD3000_STATUS2_REG 0x02
#define GD3000_STATUS3_REG 0x03
#define GD3000_DUMMY_REG 0x00

//pour indiquer si le GD3000 est fonctionnel ou pas
#define GD3000_STATUS_FAULT 0
#define GD3000_STATUS_OK 1

//pour l'utilisation des canaux de eTimer2 (vérif fréquence et rapport cyclique des tensions de phase
//de l'onduleur).
#define eTimer2_VphA 2 //canal 2 de eTimer2
#define eTimer2_VphB 2 //canal 2 de eTimer2
#define eTimer2_VphC 2 //canal 2 de eTimer2

//Paramètres commande PWM : horloge MOTC = 40 MHz --> prescaler de 4 --> horloge module FlexPWM = 10 MHz
//Par ex, pour avoir une période PWM de 100µs, il suffit d'avoir 1000 clock periods de l'horloge du module.
//Pour avoir une période PWM de 100µs, il suffit d'avoir 1200 clock periods de l'horloge du module.
#define Prescaler_PWM 2 //prescaler = 2^2 = 4.
//valeur du deadtime par défaut, donné en nb de périodes d'horloge MOTC (fréq = 40 MHz --> période = 25 ns)
//deadtime = 50*25n = 1 µs. Même valeur pour fronts montants et descendants.
#define DeadTimeGD3000 100 //50
#define PWM_RLD_FREQ 0 //définit la période de rechargement des registres du module FlexPWM --> signal master
 //reload (MRS). La période de réchargement = PWM_RLD_FREQ+1 cycles PWM. En mettant 0, on recharge à chaque période
 //du signal PWM

//Initialization and configuration of GD3000
void gd3000_Activ_Pad_Enable(void);
void gd3000_Inhib_Pad_Enable(void);
void gd3000_Activ_Pad_Reset(void);
void gd3000_Inhib_Pad_Reset(void);
void gd3000_Reset(void);
void gd3000_Config_GPIO(void);
void gd3000_Config_DSPI(void);
void gd3000_Toggle_Pins_Init(void);
void gd3000_initialization_procedure(void);
uint8_t gd3000_Config_Init(void);

//Read/write of internal registers
void gd3000_write_reg(uint8_t my_register,uint8_t my_data);
uint8_t gd3000_read_reg(uint8_t my_register);
uint8_t gd3000_read_write_reg(uint8_t my_register,uint8_t my_data);
void gd3000_read_dummy(void);

//Configuration, activation, deactivation of PWM commands for GD3000
void gd3000_Config_FlexPWM0_BrasA(void);
void gd3000_Config_FlexPWM0_BrasABC(void);
void gd3000_Update_DutyCycle_Cmd_PWMA(uint16_t Duty0);
void gd3000_Update_DutyCycle_Cmd_PWM_ABC(uint16_t Duty0, uint16_t Duty1, uint16_t Duty2);
void gd3000_RunPWM_BrasA(void);
void gd3000_StopPWM_BrasA(void);
void gd3000_RunPWM_BrasABC(void);
void gd3000_StopPWM_BrasABC(void);
void gd3000_RunPWM_Triphase_Motor(void);
void gd3000_StopPWM_Triphase_Motor(void);

//Fault status and recovery, ISR management
void gd3000_Recovery_Fault(void);
void gd3000_Read_Status(void);
void ERQ_GD_INT_OCOUT_isr(void);

#endif /* GD3000_H_ */
