/*
 * gd3000.c
 *
 *  Created on: 1 oct. 2018
 *      Author: admin_aboyer
 *
 *  Ensemble des fonctions de configuration et d'utilisation du gate driver GD3000.
 */

#include "MPC5744P.h"
#include "gd3000.h"
#include "DSPIConfig.h"
#include "ConfigGPIO.h"
#include "ConfigPWM.h"
#include "ConfigPIT.h"
#include "eTimer.h"
#include "User_Interface.h"



GD3000_Status_struct GD3000_Status;
uint32_t GD_INT_pad;
uint32_t GD_OCOUT_pad;

//p�riode PWM --> �gale � la dur�e de la fast loop
uint32_t PWMPeriod;  //en nb de clock submodule
uint32_t DutyDefaultGD3000; //rapport cyclique par d�faut (en nb de clock submodule)


/*****************************************************************************/
/* Initialization and configuration of GD3000                                */
/*****************************************************************************/

void gd3000_Activ_Pad_Enable(void){
	Set_pad_out(Pad_Enable,Activ_Enable);
}

void gd3000_Inhib_Pad_Enable(void) {
	Set_pad_out(Pad_Enable,Inhib_Enable);
}

void gd3000_Activ_Pad_Reset(void) {
	Set_pad_out(Pad_Reset,Activ_Reset);
}

void gd3000_Inhib_Pad_Reset(void) {
	Set_pad_out(Pad_Reset,Inhib_Reset);
}

/*****************************************************************************/
/* gd3000_Reset                                                        */
/* Description: force le GD3000 � rester en reset, la pin enable est mis � 0. */
/*****************************************************************************/
void gd3000_Reset(void) {
	gd3000_Activ_Pad_Reset();
	gd3000_Inhib_Pad_Enable();
}

/*****************************************************************************/
/* gd3000_Config_GPIO                                                        */
/* Description: configure les GPIO utilis�es pour piloter le GD3000 : Enable, Reset,
 * PWM et SPI.
 */
/*****************************************************************************/
void gd3000_Config_GPIO(void) {
	Config_pad_out(Pad_Enable,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_OFF);
	Config_pad_out(Pad_Reset,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_ON);
	gd3000_Reset();

	//config entr�e INT (entr�e interruption externe, pour l'instant d�sactiv�)
	Config_pad_in(Pad_INT,PULL_OFF,PULL_DOWN,HYST_ON); //broche PA15
	//Config_Alter_Function_In(187,1); //on active la fonction External interrupt source 14 associ�e � PA15
	//Config_ERQ_Pad(14, ERQ_RISING_EVENT_ON, ERQ_RISING_EVENT_OFF,ERQ_FILTER_ON); //GD_INT passe � 1 lors
				//d'une d�faillance.

	//config entr�e OC_OUT (entr�e interruption externe, pour l'instant d�sactiv�)
	Config_pad_in(Pad_OCOUT,PULL_OFF,PULL_DOWN,HYST_ON); //broche PA14
	//Config_Alter_Function_In(186,1); //on active la fonction External interrupt source 13 associ�e � PA14
	//Config_ERQ_Pad(13, ERQ_RISING_EVENT_ON, ERQ_RISING_EVENT_OFF,ERQ_FILTER_ON); //GD_INT passe � 1 lors
					//d'une d�faillance.


	//config des entr�es A6, A7, A8, utilis�es comme canaux 2,3,4 du eTimer2.  Utilisable en mode
	//Input Capture pour contr�ler le rapport cyclique et la fr�quence des tensions de phase de
	//l'onduleur (v�rification non impl�ment�e dans ce projet).
	//A[6] used eTimer2.ch[2] input capture
	Config_pad_in(Pad_Lecture_VphA,PULL_OFF,PULL_DOWN,HYST_ON); //broche PA6
	Config_Alter_Function_In(73,1); //on active la fonction eTimer_2 Input Data Channel 2 associ�e � PA6

	//A[7] used eTimer2.ch[3] input capture
	Config_pad_in(Pad_Lecture_VphB,PULL_OFF,PULL_DOWN,HYST_ON); //broche PA7
	Config_Alter_Function_In(74,1); //on active la fonction eTimer_2 Input Data Channel 3 associ�e � PA7

	//A[8] used eTimer2.ch[4] input capture
	Config_pad_in(Pad_Lecture_VphC,PULL_OFF,PULL_DOWN,HYST_ON); //broche PA8
	Config_Alter_Function_In(75,1); //on active la fonction eTimer_2 Input Data Channel 4 associ�e � PA8


	//config pads DSPI0
	init_dspi0_ports();

	//config pads FlexPWM0 comme sorties. On configurera les sorties PWM apr�s l'initialisation du GD3000.
	//on les met en �tat hi-Z
	Config_pad_out(PA_LS,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_OFF);
	Config_pad_out(PA_HS,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_ON);
	Config_pad_out(PB_LS,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_OFF);
	Config_pad_out(PB_HS,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_ON);
	Config_pad_out(PC_LS,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_OFF);
	Config_pad_out(PC_HS,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_ON);
}

/*****************************************************************************/
/* gd3000_Config_DSPI                                                        */
/* Description: initialisation et configuration du module DSPI utilis� pour communiquer
 * avec le MC34GD3000 (DSPI0).
 */
/*****************************************************************************/
void gd3000_Config_DSPI(void) {
	DSPI_Enter_Exit_HaltMode(GD3000_SPI_MODULE,Stop_DSPI_Transfer);
	init_dspi0();
	SetTiming_dspi0(CTAR_nb_GD3000);
	DSPI_Enter_Exit_HaltMode(GD3000_SPI_MODULE,Start_DSPI_Transfer);
}

/*****************************************************************************/
/* gd3000_Toggle_Pins_Init                                                        */
/* Description: une fois la config du GD3000 effectu�e, une proc�dure particuli�re
 * de commutation des broches de sortie (commande des MOSFET) est requise. Cette fonction
 * r�alise cette �tape, en respectant les timings d�finis par la datasheet du GD3000.
 */
/*****************************************************************************/
void gd3000_Toggle_Pins_Init(void) {
	//ENABLE1 and ENABLE2 are set high (LS outputs are now enabled)
	gd3000_Activ_Pad_Enable();
	DelayTimerGeneral_us(1); //wait at least 280 ns

	//PA_LS, PB_LS, and PC_LS are toggled high for about 1.0 us (HS outputs are enabled, but not latched)
	Set_pad_out(PA_HS,OUT_ON); //nPA_HS, nPB_HS, nPC_HS are High
	Set_pad_out(PB_HS,OUT_ON);
	Set_pad_out(PC_HS,OUT_ON);

	Set_pad_out(PA_LS,OUT_ON);
	Set_pad_out(PB_LS,OUT_ON);
	Set_pad_out(PC_LS,OUT_ON);
	DelayTimerGeneral_us(2);
	Set_pad_out(PA_LS,OUT_OFF);
	Set_pad_out(PB_LS,OUT_OFF);
	Set_pad_out(PC_LS,OUT_OFF);

	//Toggle nPA_HS, nPB_HS, and nPC_HS Low for deadtime plus at least 0.1 us (HS outputs are now latched and operational)
	Set_pad_out(PA_HS,OUT_OFF);
	Set_pad_out(PB_HS,OUT_OFF);
	Set_pad_out(PC_HS,OUT_OFF);
	DelayTimerGeneral_us(3);
	Set_pad_out(PA_HS,OUT_ON);
	Set_pad_out(PB_HS,OUT_ON);
	Set_pad_out(PC_HS,OUT_ON);
}


/*****************************************************************************/
/* gd3000_initialization_procedure                                           */
/* Description: lancement de la proc�dure d'initialisation du MC34GD3000.
 * Son initialisation requiert un envoi de commandes pr�cises par SPI et de signaux
 * sur les entr�es de commande PWM. La configuration du composant se fait pendant cette
 * phase d'initialisation. Toute erreur dans la liste des commandes peut conduire �
 * un dysfonctionnement du GD3000.
 * Remarque :
 * Le test donne des valeurs tr�s �tranges des registres de config (par exemple, les erreurs
 * ne semblent pas marqu�s, il y a pas mal d'erreurs indiqu�es par status reg 0, le deadtime est
 * diff�rent de 0. Pourtant, les sorties PWM commutent, avec le bon deadtime et le
 * bon rapport cyclique !
 */
/*****************************************************************************/
void gd3000_initialization_procedure(void) {
	uint8_t Read_Data;

	//RESET goes high (ENABLE1 and ENABLE2 remain low)
	gd3000_Inhib_Pad_Reset();

	DelayTimerGeneral_us(5000);

	//d�commenter la ligne pour contr�ler l'�tat initial des registres internes du GD3000
	//gd3000_Read_Status();

	//CLINT0 - on efface les flags d'interruptions
	Read_Data = gd3000_read_write_reg(GD3000_CLINT0_REG,0x0F);
	//CLINT1 - on efface les flags d'interruptions
	Read_Data = gd3000_read_write_reg(GD3000_CLINT1_REG,0x0F);

	//Mask0 - on masque toutes les interruptions
	Read_Data = gd3000_read_write_reg(GD3000_MASK0_REG,0x00);
	//Mask1 - on masque toutes les interruptions
	Read_Data = gd3000_read_write_reg(GD3000_MASK1_REG,0x00);

	//Deadtime command (zero deadtime command, deadtime g�r� par le module FlexPWM)
	Read_Data = gd3000_read_write_reg(GD3000_DEAD_REG,0x00);

	//Mode - Full-on mode actif (pas de controle de deadtime par le GD3000) + desat +
	//Lock bit --> plus de modif de la config possible avant un reset complet.
	Read_Data = gd3000_read_write_reg(GD3000_MODE_REG,0x02);

	//CLINT0 - on efface les flags d'interruptions
	Read_Data = gd3000_read_write_reg(GD3000_CLINT0_REG,0x0F);
	//CLINT1 - on efface les flags d'interruptions
	Read_Data = gd3000_read_write_reg(GD3000_CLINT1_REG,0x0F);

	//on envoie la commande Null
	Read_Data = gd3000_read_write_reg(GD3000_NULL_REG,0x00);
	//on contr�le l'�tat des registres de statut internes du GD3000
	gd3000_Read_Status();

	//proc�dure de commutation des broches de sortie (n�cessaire pour le faire d�marrer)
	gd3000_Toggle_Pins_Init();
}

/*****************************************************************************/
/* gd3000_Config_Init                                                         */
/* Description: appelle l'ensemble des fonctions n�cessaires � l'initialisation
 * des p�riph associ�s au pilotage du GD3000 (FlexPWM et SPI) et � sa configuration.
 * La fonction renvoie une indication sur l'�tat de la configuration : correctement
 * effectu�e ou non.
 */
/*****************************************************************************/
uint8_t gd3000_Config_Init(void) {

	//initialisation des registres de stockage des statuts du GD3000
	GD3000_Status.GD3000_Failed = GD3000_STATUS_OK;
	GD3000_Status.Status0 = 0;
	GD3000_Status.Status1 = 0;
	GD3000_Status.Status2 = 0;
	GD3000_Status.Status3 = 0;


	gd3000_Config_GPIO(); //toutes les I/O, m�me celles li�es � DSPI1 et FlexPWM0
	gd3000_Config_DSPI(); //config du module DSPI associ� � GD3000 (DSPI1)
	gd3000_initialization_procedure(); //proc�duer d'initialisation et configuration du MC34GD3000

	//initialisation de la commande PWM du GD3000 (quelque soit l'�tat du GD3000)
	Init_IO_PWM0(SUB0_ON,SUB1_ON,SUB2_ON);
	gd3000_Config_FlexPWM0_BrasABC();

	//initialisation des canaux du eTimer2 utilis�s pour le contr�le des rapports cycliques et fr�quences
	//des tensions de phase de l'onduleur (non impl�ment� dans ce projet)
	Config_eTimer2_InputCapture(eTimer2_VphA);
	Config_eTimer2_InputCapture(eTimer2_VphB);
	Config_eTimer2_InputCapture(eTimer2_VphC);
	//lancement du comptage des 3 canaux de eTimer2.
	Start_eTimer2_InputCapture_Count(eTimer2_VphA);
	Start_eTimer2_InputCapture_Count(eTimer2_VphB);
	Start_eTimer2_InputCapture_Count(eTimer2_VphC);

	//return(GD3000_Status.GD3000_Failed);
	return(1); //pour �viter les probl�mes de configuration et de lecture de statut du GD3000.
}


/*****************************************************************************/
/* Read/Write of internal registers of GD3000                                */
/*****************************************************************************/

void gd3000_write_reg(uint8_t my_register,uint8_t my_data){
	uint8_t Transmit_Data;
	uint8_t Read_Data;

	Transmit_Data = (my_register & 0xF0) | (my_data & 0x0F);
	Read_Data = Transmit_Read_DSPI_8bits(GD3000_SPI_MODULE,Master_Mode,CTAR_nb_GD3000,PCS_asserted_GD3000,Transmit_Data,CS_Cont_OFF,0);
}

uint8_t gd3000_read_reg(uint8_t my_register) {
	uint8_t Read_Data;

	Read_Data = Transmit_Read_DSPI_8bits(GD3000_SPI_MODULE,Master_Mode,CTAR_nb_GD3000,PCS_asserted_GD3000,my_register,CS_Cont_OFF,0);
	return(Read_Data);
}

uint8_t gd3000_read_write_reg(uint8_t my_register,uint8_t my_data) {
	uint8_t Transmit_Data;
	uint8_t Read_Data;

	Transmit_Data = (my_register & 0xF0) | (my_data & 0x0F);
	Read_Data = Transmit_Read_DSPI_8bits(GD3000_SPI_MODULE,Master_Mode,CTAR_nb_GD3000,PCS_asserted_GD3000,Transmit_Data,CS_Cont_OFF,0);
	return(Read_Data);
}


void gd3000_read_dummy(void) {
	uint8_t Read_Data;
	Read_Data = Transmit_Read_DSPI_8bits(GD3000_SPI_MODULE,Master_Mode,CTAR_nb_GD3000,PCS_asserted_GD3000,GD3000_DUMMY_REG,CS_Cont_OFF,0);

}


/*****************************************************************************/
/* Configuration, activation, deactivation of PWM commands for GD3000        */
/*****************************************************************************/


/*****************************************************************************/
/* gd3000_Config_FlexPWM0_BrasA                                              */
/* Description: initialisation et configuration du FlexPWM0 qui pilote le
 * driver de grille MC34GD300. Cas o� seul le bras A est activ�.
 * Attention, la fonction n'active pas les sorties PWM.
 */
/*****************************************************************************/
void gd3000_Config_FlexPWM0_BrasA(void) {
	//config des horloges des sous-modules 0 de PWM0. Clock source = horloge Periph = MotC
	//sa fr�quence = 80 MHz. On redivise par 8 pour avoir une horloge du module de 10 MHz. Ainsi, une p�riode PWM
	//de 10 kHz est compt� par 1000 p�riodes d'horloge du module PWM.
	InitPWM0_Sub_ClkConfig(0,CLKSOURCE_PERIPH,Prescaler_PWM);

	//Initialisation du mode de fonctionnement des sous-modules 0 de PWM0
	//Les  sorties sont en mode compl�mentaires
	InitPWM0_Sub_OpMode(0,COMPL_OUTPUT,POLAR_NORMAL,PWM_INACTIV_DBG,INITSEL_LOCALSYNC,RELOAD_LOCAL,PWM_RLD_FREQ,FULL_RLD);

	//polarit� des sorties A et B des sous-modules 0 (polarit� invers�e pour les sorties A, car la commande
	//des high side est en logique inverse : un �tat 0 active le high side, tandis qu'un 1 active le high side.
	//Comme les sorties A et B sont en mode compl, sans inversion de polar de A, quand B = 1 --> low side est on,
	//alors A = 0 --> high side passant ! En inversant la polarit� de A, quand B = 1 --> low side est on,
	//alors A = 1 --> high side bloqu� !
	Config_PWM_Output(0,OUTPUTA,POLAR_INV,TRISTATE_FS);
	Config_PWM_Output(0,OUTPUTB,POLAR_NORMAL,TRISTATE_FS);

	//initialisation des param�tres temporels des sorties des sous-modules 0. Les sorties A et B
	//sont compl�mentaires. Elles sont align�es sur le front montant de PWMA (comme elle est invers�e, le cycle
	//PWM d�marre sur front descendant de /PWM_HS).
	Config_PWM0_SignalParams(0,OUTPUTA,PWMPeriod,DutyDefaultGD3000,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(0,OUTPUTB,PWMPeriod,DutyDefaultGD3000,DeadTimeGD3000,CENTER_ALIGN,0,0);

	//autorisation des triggers en sortie des sous-modules 02. Comme la sortie PWM est
	//align�e sur leurs centres, on active le trigger au moment o� l'on sait qu'elles sont toutes � l'�tat0.
	//Cela arrive lorsque le compteur interne atteint la valeur VAL1.
	//tant qu'on utilise pas l'ADC et le CTU, on ne g�n�re pas de trigger.
	Enable_OutputTrigger(0,OUT_TRIG_OFF,OUT_TRIG_ON,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF);

	//disable fault protection pins
	Disable_Fault_PWM0_All();

	//chargement des param�tres PWM dans les sous-modules 0
	LoadPWM0_Params(LOAD_SUB0_ON,LOAD_SUB1_OFF,LOAD_SUB2_OFF,LOAD_SUB3_OFF);
	//activation des sorties A et B des sous-modules 0 (attention, on n'a pas encore activ�
	//les horloges dans les sous-modules, donc les sorties PWM ne commutent pas encore.)
	Activate_PWM0_Output(0,OUTA_ON,OUTB_ON,OUTX_OFF);
}



/*****************************************************************************/
/* gd3000_Config_FlexPWM0_BrasABC                                              */
/* Description: initialisation et configuration du FlexPWM0 qui pilote le
 * driver de grille MC34GD300. Cas o� seul le bras A est activ�.
 * Attention, la fonction n'active pas les sorties PWM.
 */
/*****************************************************************************/
void gd3000_Config_FlexPWM0_BrasABC(void) {
	//config des horloges des sous-modules 0, 1 et 2 de PWM0. Clock source = horloge Periph = MotC
	//sa fr�quence = 80 MHz. On redivise par 8 pour avoir une horloge du module de 10 MHz. Ainsi, une p�riode PWM
	//de 10 kHz est compt� par 1000 p�riodes d'horloge du module PWM.
	InitPWM0_Sub_ClkConfig(0,CLKSOURCE_PERIPH,Prescaler_PWM);
	InitPWM0_Sub_ClkConfig(1,CLKSOURCE_PERIPH,Prescaler_PWM);
	InitPWM0_Sub_ClkConfig(2,CLKSOURCE_PERIPH,Prescaler_PWM);

	//Initialisation du mode de fonctionnement des sous-modules 0, 1 et 2 de PWM0
	//Les 3 sorties sont en mode compl�mentaires
	//Le sous-module 0 initialise le compteur des 2 autres sous-modules et g�re leur rechagement
	InitPWM0_Sub_OpMode(0,COMPL_OUTPUT,POLAR_NORMAL,PWM_INACTIV_DBG,INITSEL_LOCALSYNC,RELOAD_LOCAL,PWM_RLD_FREQ,FULL_RLD);
	InitPWM0_Sub_OpMode(1,COMPL_OUTPUT,POLAR_NORMAL,PWM_INACTIV_DBG,INITSEL_MSTSYNC,RELOAD_MSTRLD,PWM_RLD_FREQ,FULL_RLD);
	InitPWM0_Sub_OpMode(2,COMPL_OUTPUT,POLAR_NORMAL,PWM_INACTIV_DBG,INITSEL_MSTSYNC,RELOAD_MSTRLD,PWM_RLD_FREQ,FULL_RLD);

	//polarit� des sorties A et B des sous-modules 0, 1 et 2 (polarit� invers�e pour les sorties A, car la commande
	//des high side est en logique inverse : un �tat 0 active le high side, tandis qu'un 1 active le high side.
	//Comme les sorties A et B sont en mode compl, sans inversion de polar de A, quand B = 1 --> low side est on,
	//alors A = 0 --> high side passant ! En inversant la polarit� de A, quand B = 1 --> low side est on,
	//alors A = 1 --> high side bloqu� !
	Config_PWM_Output(0,OUTPUTA,POLAR_INV,TRISTATE_FS);
	Config_PWM_Output(0,OUTPUTB,POLAR_NORMAL,TRISTATE_FS);
	Config_PWM_Output(1,OUTPUTA,POLAR_INV,TRISTATE_FS);
	Config_PWM_Output(1,OUTPUTB,POLAR_NORMAL,TRISTATE_FS);
	Config_PWM_Output(2,OUTPUTA,POLAR_INV,TRISTATE_FS);
	Config_PWM_Output(2,OUTPUTB,POLAR_NORMAL,TRISTATE_FS);

	//initialisation des param�tres temporels des sorties des sous-modules 0, 1 et 2. Les sorties A et B
	//sont compl�mentaires. Elles sont align�es sur le front montant de PWMA (comme elle est invers�e, le cycle
	//PWM d�marre sur front descendant de /PWM_HS).
	Config_PWM0_SignalParams(0,OUTPUTA,PWMPeriod,DutyDefaultGD3000,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(0,OUTPUTB,PWMPeriod,DutyDefaultGD3000,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(1,OUTPUTA,PWMPeriod,DutyDefaultGD3000,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(1,OUTPUTB,PWMPeriod,DutyDefaultGD3000,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(2,OUTPUTA,PWMPeriod,DutyDefaultGD3000,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(2,OUTPUTB,PWMPeriod,DutyDefaultGD3000,DeadTimeGD3000,CENTER_ALIGN,0,0);

	//autorisation des triggers en sortie des sous-modules 0, 1 et 2. Comme les 3 sorties PWM sont
	//align�es sur leurs centres, on active le trigger au moment o� l'on sait qu'elles sont toutes les phases
	//sont � l'�tat0, donc quand les commandes PWM sont � l'�tat 1.
	//Cela arrive lorsque le compteur interne atteint la valeur VAL1 (attention : VAl1,3,5 g�n�re Out_Trig1 et non
	//Out_Trig0.
	//tant qu'on utilise pas l'ADC et le CTU, on ne g�n�re pas de trigger.
	Enable_OutputTrigger(0,OUT_TRIG_OFF,OUT_TRIG_ON,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF);
	Enable_OutputTrigger(1,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF);
	Enable_OutputTrigger(2,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF,OUT_TRIG_OFF);

	//disable fault protection pins
	Disable_Fault_PWM0_All();
	//chargement des param�tres PWM dans les sous-modules 0, 1 et 2
	LoadPWM0_Params(LOAD_SUB0_ON,LOAD_SUB1_ON,LOAD_SUB2_ON,LOAD_SUB3_OFF);
	//activation des sorties A et B des sous-modules 0, 1 et 2 (attention, on n'a pas encore activ�
	//les horloges dans les sous-modules, donc les sorties PWM ne commutent pas encore.)
	Activate_PWM0_Output(0,OUTA_ON,OUTB_ON,OUTX_ON);
	Activate_PWM0_Output(1,OUTA_ON,OUTB_ON,OUTX_ON);
	Activate_PWM0_Output(2,OUTA_ON,OUTB_ON,OUTX_ON);
}

/*****************************************************************************/
/* gd3000_Update_DutyCycle_Cmd_PWMA                                                  */
/* Description: met � jour le rapport cyclique de la commande PWM (assur� par*/
/* FlexPWM0 du soumodule 0 (commande du bras A)                     		 */
/*****************************************************************************/
void gd3000_Update_DutyCycle_Cmd_PWMA(uint16_t Duty0) {
	Config_PWM0_SignalParams(0,OUTPUTA,PWMPeriod,Duty0,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(0,OUTPUTB,PWMPeriod,Duty0,DeadTimeGD3000,CENTER_ALIGN,0,0);

	//chargement des param�tres PWM dans les sous-modules 0, 1 et 2
	LoadPWM0_Params(LOAD_SUB0_ON,LOAD_SUB1_OFF,LOAD_SUB2_OFF,LOAD_SUB3_OFF);
}

/*****************************************************************************/
/* gd3000_Update_DutyCycle_Cmd_PWM                                                  */
/* Description: met � jour le rapport cyclique de la commande PWM (assur� par*/
/* FlexPWM0.																 */
/*****************************************************************************/
void gd3000_Update_DutyCycle_Cmd_PWM_ABC(uint16_t Duty0, uint16_t Duty1, uint16_t Duty2) {
	Config_PWM0_SignalParams(0,OUTPUTA,PWMPeriod,Duty0,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(0,OUTPUTB,PWMPeriod,Duty0,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(1,OUTPUTA,PWMPeriod,Duty1,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(1,OUTPUTB,PWMPeriod,Duty1,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(2,OUTPUTA,PWMPeriod,Duty2,DeadTimeGD3000,CENTER_ALIGN,0,0);
	Config_PWM0_SignalParams(2,OUTPUTB,PWMPeriod,Duty2,DeadTimeGD3000,CENTER_ALIGN,0,0);

	//chargement des param�tres PWM dans les sous-modules 0, 1 et 2
	LoadPWM0_Params(LOAD_SUB0_ON,LOAD_SUB1_ON,LOAD_SUB2_ON,LOAD_SUB3_OFF);
}


/*****************************************************************************/
/* gd3000_RunPWM_BrasA                                                         */
/* Description: lance les signaux PWM pour piloter le bras A (Module 0 de FlexPWM0).
 * Dans la pratique, �a active l'horloge dans les sous-modules 0.
 */
/*****************************************************************************/
void gd3000_RunPWM_BrasA(void) {
	//activation de l'horloge dans les sous-modules 0 de FlexPWM0
	Run_PWM0_SubModule(SUB0_ON,SUB1_OFF,SUB2_OFF,SUB3_OFF);

	//on colle les �tats des 2 autres bras � 0
	Set_pad_out(PB_LS,OUT_ON);
	Set_pad_out(PC_LS,OUT_ON);
	Set_pad_out(PB_HS,OUT_ON);
	Set_pad_out(PC_HS,OUT_ON);
}

/*****************************************************************************/
/* gd3000_StopPWM_BrasA                                                         */
/* Description: arr�te les signaux PWM pour piloter le bras A (Module 0 de FlexPWM0).
 * Dans la pratique, �a d�sactive l'horloge dans les sous-modules 0.
 */
/*****************************************************************************/
void gd3000_StopPWM_BrasA(void) {
	//activation de l'horloge dans les sous-modules 0 de FlexPWM0
	Run_PWM0_SubModule(SUB0_OFF,SUB1_OFF,SUB2_OFF,SUB3_OFF);

	//on colle les �tats des 2 autres bras � 0
	Set_pad_out(PB_LS,OUT_ON);
	Set_pad_out(PC_LS,OUT_ON);
	Set_pad_out(PB_HS,OUT_ON);
	Set_pad_out(PC_HS,OUT_ON);
}


/*****************************************************************************/
/* gd3000_RunPWM_BrasABC                                                         */
/* Description: lance les signaux PWM pour piloter le bras A, B et C (Module 0 de FlexPWM0).
 * Dans la pratique, �a active l'horloge dans les sous-modules 0, 1 et 2.
 */
/*****************************************************************************/
void gd3000_RunPWM_BrasABC(void) {
	//activation de l'horloge dans les sous-modules 0 de FlexPWM0
	Run_PWM0_SubModule(SUB0_ON,SUB1_ON,SUB2_ON,SUB3_OFF);
}

/*****************************************************************************/
/* gd3000_StopPWM_BrasABC                                                         */
/* Description: arr�te les signaux PWM pour piloter le bras A, B et C (Module 0 de FlexPWM0).
 * Dans la pratique, �a desactive l'horloge dans les sous-modules 0, 1 et 2.
 */
/*****************************************************************************/
void gd3000_StopPWM_BrasABC(void) {
	//activation de l'horloge dans les sous-modules 0 de FlexPWM0
	Run_PWM0_SubModule(SUB0_OFF,SUB1_OFF,SUB2_OFF,SUB3_OFF);

}

/*****************************************************************************/
/* gd3000_RunPWM_Triphase_Motor                                                         */
/* Description: lance les signaux PWM sur les sorties 0, 1 et 2 du module PWM0.
 * Dans la pratique, �a active l'horloge dans les sous-modules 0, 1 et 2.
 */
/*****************************************************************************/
void gd3000_RunPWM_Triphase_Motor(void) {
	//activation de l'horloge dans les sous-modules 0, 1 et 2 de FlexPWM0
	Run_PWM0_SubModule(SUB0_ON,SUB1_ON,SUB2_ON,SUB3_OFF);
}

/*****************************************************************************/
/* gd3000_StopPWM_Triphase_Motor                                                         */
/* Description: arr�te les signaux PWM sur les sorties 0, 1 et 2 du module PWM0.
 * Dans la pratique, �a d�sactive l'horloge dans les sous-modules 0, 1 et 2.
 */
/*****************************************************************************/
void gd3000_StopPWM_Triphase_Motor(void) {
	//activation de l'horloge dans les sous-modules 0, 1 et 2 de FlexPWM0
	Run_PWM0_SubModule(SUB0_OFF,SUB1_OFF,SUB2_OFF,SUB3_OFF);
}



/*****************************************************************************/
/* Fault status and recovery, ISR management                                 */
/*****************************************************************************/


/*****************************************************************************/
/* gd3000_Recovery_Fault                                                    */
/* Description: cette fonction r�initialise le composant apr�s la d�tection
 * d'une faute (effacement des flags d'interruption + enable + toggle output)
 */
/*****************************************************************************/
void gd3000_Recovery_Fault(void) {
	uint8_t Read_Data;

	//on peut contr�ler l'�tat des registres de statut internes en mode debug
	gd3000_Read_Status();

	//ENABLE1 and ENABLE2 are set low
	gd3000_Inhib_Pad_Enable();
	DelayTimerGeneral_us(1);

	//clear interrupt flags (CLINT0 and CLINT1)
	Read_Data = gd3000_read_write_reg(GD3000_CLINT0_REG,0x0F);
	Read_Data = gd3000_read_write_reg(GD3000_CLINT1_REG,0x0F);

	//on recontr�le l'�tat des registres de statut internes
	gd3000_Read_Status();

	//proc�dure de commutation des broches de sortie
	gd3000_Toggle_Pins_Init();
}


/*****************************************************************************/
/* gd3000_Read_Status                                                    */
/* Description: cette fonction relit l'�tat des registres de statut internes du
 * GD3000 et met � jour les variable GD3000_Status, GD_INT_pad et GD_OCOUT_pad.
 * Pour l'instant, la fonction ne renvoie rien selon l'�tat des registres.
 * Cette fonction n'est utile que pour le debug in-situ.
 */
/*****************************************************************************/
void gd3000_Read_Status(void) {
	uint8_t Read_Data;


	//Read Status register 0
	GD3000_Status.Status0 = gd3000_read_reg(GD3000_STATUS0_REG);

	//Read Status register 1
	GD3000_Status.Status1 = gd3000_read_reg(GD3000_STATUS1_REG);

	//Read Status register 2
	GD3000_Status.Status2 = gd3000_read_reg(GD3000_STATUS2_REG);

	//Read Status register 3 (v�rif deadtime)
	GD3000_Status.Status3 = gd3000_read_reg(GD3000_STATUS3_REG);

	//on envoie la commande Null
	Read_Data = gd3000_read_write_reg(GD3000_NULL_REG,0x00);

	//aucune v�rification des r�sultats du contenu des registres de statuts du GD3000.

	//on lit l'�tat des broches INT et OC_OUT
	GD_INT_pad = Read_pad_in(Pad_INT);
	GD_OCOUT_pad = Read_pad_in(Pad_OCOUT);
	if ((GD_INT_pad == 1) | (GD_OCOUT_pad == 1)) {
		GD3000_Status.GD3000_Failed = GD3000_STATUS_FAULT;
	}
}

/*****************************************************************************/
/* ERQ_GD_INT_isr                                                   */
/* Description: gestion de l'interruption associ�e � un passage � 1 de la broche
 * GD_INT (entr�e ERQ14) ou de la broche OC_OUT (entr�e ERQ13).
 */
/*****************************************************************************/
void ERQ_GD_INT_OCOUT_isr(void) {
	if (SIUL2.DISR0.B.EIF14 == 1) {
		//RAZ du flag
		SIUL2.DISR0.B.EIF14 = 1;

		//lecture des registres de statut du GD3000
		gd3000_Read_Status();
		//pour l'instant, aucune action corrective

	}
	else if (SIUL2.DISR0.B.EIF13 == 1) {
		//RAZ du flag
		SIUL2.DISR0.B.EIF13 = 1;

		//lecture des registres de statut du GD3000
		gd3000_Read_Status();
		//pour l'instant, aucune action corrective
	}
	Set_pad_out(User_LED_Green,LED_OFF);
	Set_pad_out(User_LED_Red,LED_ON);
}





