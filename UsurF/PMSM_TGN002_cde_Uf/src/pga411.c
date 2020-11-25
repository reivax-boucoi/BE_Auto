/*
 * pga411.c
 *
 *  Created on: 11 sept. 2018
 *      Author: admin_aboyer
 *
 *  Contient les fonctions permettant de communiquer avec le PGA411, afin de le
 *  configurer, le diagnostiquer par SPI, récupérer l'angle et la vitesse du rotor.
 */

#include "MPC5744P.h"
#include "DSPIConfig.h"
#include "ConfigGPIO.h"
#include "ConfigPIT.h"
#include "pga411.h"
#include "mlib.h"
#include "gflib.h"
#include "User_Interface.h"

PGA411_reg_t pga411_regs[PGA411_REG_COUNT] = {
	/* memory constants init */
	[DEV_OVUV1] = {.read_add = 0x53, .write_add = 0x87, .def_val = 0xBFC0},
	[DEV_OVUV2] = {.read_add = 0x6B, .write_add = 0x26, .def_val = 0x00C0},
	[DEV_OVUV3] = {.read_add = 0x65, .write_add = 0x17, .def_val = 0xFCFF},
	[DEV_OVUV4] = {.read_add = 0xEC, .write_add = 0x39, .def_val = 0xBFC0},
	[DEV_OVUV5] = {.read_add = 0x52, .write_add = 0x75, .def_val = 0x00C0},
	[DEV_OVUV6] = {.read_add = 0xE9, .write_add = 0x83, .def_val = 0xFCFF},
	[DEV_TLOOP_CFG] = {.read_add = 0xA6, .write_add = 0x42, .def_val = 0xBFC0},
	[DEV_AFE_CFG] = {.read_add = 0xC2, .write_add = 0x91, .def_val = 0x00C0},
	[DEV_PHASE_CFG] = {.read_add = 0x57, .write_add = 0x85, .def_val = 0xFCFF},
	[DEV_CONFIG1] = {.read_add = 0xBE, .write_add = 0xEB, .def_val = 0xFCFF},
	[DEV_CONTROL1] = {.read_add = 0x90, .write_add = 0x0D, .def_val = 0x0000},
	[DEV_CONTROL2] = {.read_add = 0x63, .write_add = 0x38, .def_val = 0x0000},
	[DEV_CONTROL3] = {.read_add = 0xDD, .write_add = 0xAE, .def_val = 0x0000},
	[DEV_STAT1] = {.read_add = 0x81, .write_add = 0x00, .def_val = 0x0000}, //read only register
	[DEV_STAT2] = {.read_add = 0x4D, .write_add = 0x00, .def_val = 0x0000}, //read only register
	[DEV_STAT3] = {.read_add = 0x84, .write_add = 0x00, .def_val = 0x0000}, //read only register
	[DEV_STAT4] = {.read_add = 0x1F, .write_add = 0x00, .def_val = 0x0000}, //read only register
	[DEV_STAT5] = {.read_add = 0x41, .write_add = 0x00, .def_val = 0x0000}, //read only register
	[DEV_STAT6] = {.read_add = 0x6F, .write_add = 0x00, .def_val = 0x0000}, //read only register
	[DEV_STAT7] = {.read_add = 0xE1, .write_add = 0x00, .def_val = 0x0000}, //read only register
	[DEV_CLCRC] = {.read_add = 0x4F, .write_add = 0xFC, .def_val = 0x0000},
	[DEV_CRC] = {.read_add = 0x0F, .write_add = 0xE7, .def_val = 0x0000},
	[CRCCALC] = {.read_add = 0xD9, .write_add = 0x00, .def_val = 0x0000},  //read only register
	[DEV_EE_CTRL1] = {.read_add = 0xE3, .write_add = 0x6E, .def_val = 0x0000},
	[DEV_CRC_CTRL1] = {.read_add = 0x7A, .write_add = 0xB6, .def_val = 0x0000},
	[DEV_EE_CTRL4] = {.read_add = 0xBA, .write_add = 0x56, .def_val = 0x0000},
	[DEV_UNLK_CTRL1] = {.read_add = 0x64, .write_add = 0x95, .def_val = 0x0000},
};

PGA411_status_reg_t PGA411_Status;

uint32_t cte_calcul_RPM_from_deg_us_F32;
tFloat cte_calcul_RPM_from_deg_us_FLT;



/*****************************************************************************/
/* Initialization and configuration                                          */
/*****************************************************************************/


/*****************************************************************************/
/* pga411_init_IO                                                            */
/* Description: configuration des pads d'I/O utilisés pour piloter le PGA411. */
/*****************************************************************************/
void pga411_init_IO(void) {
	//Config en sorties des pins de controle de NRST, FaultRes et BMODE0.
	//NRST et FaultRes sont mis à 1 initialement.
	Config_pad_out(PIN_NRST,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_ON);
	Config_pad_out(PIN_FaultRes,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,OUT_ON);
	Config_pad_out(PIN_BMode0,HALFDRV_SR_ON,OPENDRAIN_OFF,ANALOGPAD_OFF,PULL_OFF,PULL_DOWN,RESO_12BITS);

	//Config en entrée de la pin Fault (entrée interruption externe)
	Config_pad_in(PIN_Fault,PULL_OFF,PULL_DOWN,HYST_ON);
	//Config_Alter_Function_In(194,1); //on active la fonction External interrupt source 21 associée à PG8
	//Config_ERQ_Pad(21, ERQ_RISING_EVENT_ON, ERQ_RISING_EVENT_OFF,ERQ_FILTER_ON); //GD_INT passe à 1 lors
				//d'une défaillance.

	//config pads liés à DSPI2
	init_dspi2_ports();
}

/*****************************************************************************/
/* pga411_init_SPI1                                                          */
/* Description: initialisation du module DSPI2, utilisé pour communiquer avec */
/* le PGA411.  */
/*****************************************************************************/
void pga411_init_SPI(void) {
	DSPI_Enter_Exit_HaltMode(Module_DSPI2,Stop_DSPI_Transfer);
	init_dspi2();
	SetTiming_dspi2(DSPI_CTAR_NB);
	DSPI_Enter_Exit_HaltMode(Module_DSPI2,Start_DSPI_Transfer);
}

/*****************************************************************************/
/* pga411_reset                                                              */
/* Description: l'initialisation du PGA411 passe par un reset (application d'un pulse
 * '101' sur sa pin NRST. Cela force l'entrée en mode Diagnostic qui dure 5.2 ms environ.
 * A l'issue du Diagnostics, le PGA411 rentre en mode Normal. On doit impliquer une
 * impulsion '101' sur la pin FaultRes pour mettre à 0 la pin de Fault. A l'issue de
 * cette procédure, le composant démarre avec les configs par défaut (celle enregistrées en EEPROM).*/
/*****************************************************************************/
void pga411_reset(void) {
	//1. Transition High-Low-High sur pin Nreset pendant 500 µs.
	Set_pad_out(PIN_NRST,0);
	DelayTimerGeneral_us(500); //wait for 500 µs
	Set_pad_out(PIN_NRST,1);

	//2. attendre la fin du start-up time (LBIST + ABIST), soit environ 5.2 ms --> on attendra 10 ms
	DelayTimerGeneral_ms(10);

	//3. Entrée en mode Normal avec config par défaut.
	// On applique une impulsion à l'état bas sur FaultRes, pendant moins de 1 ms.
	Toggle_FaultRes_Pin();
}

/*****************************************************************************/
/* pga411_config                                                             */
/* Description: on configure les registres internes du PGA411. Il faut entrer en mode
 * Diagnostics pour envoyer les commandes de config par SPI. Il s'agit de réglages des blocs
 * analogiques et de diagnostics du circuit. Le réglage est custom.
 * A l'issue de la configuration, il faut revenir en mode Normal et appliquer une impulsion
 * '101' sur la pin FaultRes.
 */
/*****************************************************************************/
void pga411_config(void) {
	uint16_t data_read, data_write;

	//On entre dans le mode Diagnostic : écriture d'un '1' dans le bit SPIDIAG du registre DEV_CONTROL3.
	//Attention : lorsqu'on est en mode Diagnostics, il faut y rester moins de 150 ms, sinon une faute apparait.
	//Il est donc indispensable de ne pas faire de debug in-situ durant cette phase de debug.
   	pga411_Change_State(DIAG);

	//On déverrouille les registres : séquence d'écriture dans le registre DEV_UNLK_CTRL1.
	pga411_DeviceUnlock();


	//Envoi des commandes de configuration : à customiser selon les besoins
	data_read = pga411_ReadReg(DEV_AFE_CFG);
	//on impose un gain de 2.25 pour les amplis des voies COS et SIN
	//data_write = (data_read & 0xFFF0) | 0x000A;
	//on impose un gain de 3.5 pour les amplis des voies COS et SIN
	data_write = (data_read & 0xFFF0) | 0x000F;
	pga411_WriteReg(DEV_AFE_CFG,data_write);
	//on relit le contenu du registre DEV_AFE_CFG pour vérifier
	data_read = pga411_ReadReg(DEV_AFE_CFG);

	//on fixe la réf DVMSENL à 1.35 V
	data_read = pga411_ReadReg(DEV_OVUV2);
	data_write = (data_read & 0xFFF8) | 0x0007;
	pga411_WriteReg(DEV_OVUV2,data_write);
	data_read = pga411_ReadReg(DEV_OVUV2);

	//modif des param du tracking loop : pour améliorer la stabilité de la réponse :
	//on met Kpi au max, on enlève l'hystérésis et on diminue DKP au min (AMODE = LOW).
	//On a aussi augmenté le gain de l'AFE.
	data_read = pga411_ReadReg(DEV_TLOOP_CFG);
	data_write = (data_read & 0xFEC8) | 0x0007;
	pga411_WriteReg(DEV_TLOOP_CFG,data_write);
	data_read = pga411_ReadReg(DEV_TLOOP_CFG);

	//9. On retourne dans le mode Normal: écriture d'un '1' dans le bit DIAGEXIT du registre DEV_CONTROL1.
	pga411_Change_State(NORMAL);

	// On applique une impulsion à l'état bas sur FaultRes, pendant moins de 1 ms.
	Toggle_FaultRes_Pin();
}


/*****************************************************************************/
/* pga411_Config_Init                                                        */
/* Description: config et  initialisation du PGA411.                         */
/* La fonction renvoie une indication sur l'état de la configuration : correctement
 * effectuée ou non.
/*****************************************************************************/
uint8_t pga411_Config_Init(void) {
	uint8_t config_OK = 1;

	PGA411_Status.PGA411_Failed = PGA411_STATUS_OK;

	//initialisation des variables associées à l'utilisation du résolveur
	cte_calcul_RPM_from_deg_us_F32 = (uint32_t) (60000000/360);
	cte_calcul_RPM_from_deg_us_FLT = (tFloat) (60000000/360);

	pga411_init_IO();
	pga411_init_SPI();
	pga411_reset();
	pga411_config();
	Set_pad_out(PIN_FaultRes,0);

	//on vérifie qu'à l'issue de la configuration il n'y a pas eu de fautes (pas d'analyse fine des
	//registres de statut)
	PGA411_Status.PGA411_Failed = pga411_GetFault_status();
	config_OK != PGA411_Status.PGA411_Failed;
	config_OK = 0x01 & config_OK;
	return(config_OK);
}


/*****************************************************************************/
/* pga411_set_resolution                                                               */
/* Description: configure la résolution de la lecture d'angle et de vitesse. */
/* La fonction règle la variable resolution_pga411 et fixe l'état de la broche */
/* BMODE0. */
/*****************************************************************************/
uint8_t pga411_set_resolution(uint8_t Resolution) {
	uint8_t reso_configure = Resolution;
	Set_pad_out(PIN_BMode0,Resolution);
	return (reso_configure);
}



/*****************************************************************************/
/* PGA411_change_state                                                       */
/* Description: la fonction permet de changer l'état de fonctionnement
 * du PGA411. L'état désiré est indiqué par la variable state, pouvant prendre
 * comme valeur DIAG ou NORMAL.
 */
/*****************************************************************************/
void pga411_Change_State(uint16_t state) {
	uint16_t temp;
	/* Enter Diagnostic state */
	if (state == DIAG) {
		/* read content of DEV_CONTROL3 register */
		temp = pga411_ReadReg(DEV_CONTROL3);
		temp |= 0x0004; /* set bit SPIDIAG */
		pga411_WriteReg(DEV_CONTROL3,temp); /* finish R-M-W */
	}
	/* Go back to normal state */
	else {
		/* read content of DEV_CONTROL1 register */
		temp = pga411_ReadReg(DEV_CONTROL1);
		temp |= 0x0001; /* set bit DIAGEXIT */
		pga411_WriteReg(DEV_CONTROL1, temp); /* finish R-M-W */
	}
}

/*****************************************************************************/
/* pga411_DeviceUnlock                                                       */
/* Description: cette fonction envoie les commandes SPI permettant de déverrouiller
 * les registres DEV_CONTROL1 et DEV_CONTROL2 (device unlock procedure).
 */
/*****************************************************************************/
void pga411_DeviceUnlock(void) {
	pga411_WriteReg(DEV_UNLK_CTRL1,0x000F);
	pga411_WriteReg(DEV_UNLK_CTRL1,0x0055);
	pga411_WriteReg(DEV_UNLK_CTRL1,0x00AA);
	pga411_WriteReg(DEV_UNLK_CTRL1,0x00F0);
}



/*****************************************************************************/
/* Read/Write PGA411 internal registers by SPI                               */
/*****************************************************************************/

/*****************************************************************************/
/* pga411_ReadReg                                                               */
/* Description: la fonction lit le contenu d'un registre du PGA411. On spécifie son
 * adresse reg et il renvoie la donnée sur 16 bits. Il est nécessaire de faire deux écritures :
 * la première permet de transmettre au PGA411 l'adresse du registre à lire. La donnée ne peut
 * être lue qu'à la prochaine demande de lecture. C'est le rôle de la seconde demande.
 * La commande associée n'a pas d'importance --> on envoie une donnée appelée SPI_DUMMY.
 */
/*****************************************************************************/
uint16_t pga411_ReadReg(uint16_t reg) {
	/* first read returns whatever */
	pga411_XmitSPI(READ, reg, SPI_DUMMY);
	/* second read returns requested data */
	return (pga411_XmitSPI(READ, reg, SPI_DUMMY).datain);
}

/*****************************************************************************/
/* pga411_ReadReg_OneRead                                                   */
/* Description: la fonction lit le contenu d'un registre du PGA411. On spécifie son
 * adresse reg et il renvoie la donnée sur 16 bits. Contrairement à la fonction
 * pga411_ReadReg, on ne fait qu'une seule lecture. On part du principe qu'on a
 * réalisé au préalable une lecture qui a positionné la bonne adresse de lecture.
 * On spécifie aussi la même adresse de lecture, car il y a des chances que la
 * prochaine demande de lecture soit du même type
 */
/*****************************************************************************/
uint16_t pga411_ReadReg_OneRead(uint16_t reg) {
	return (pga411_XmitSPI(READ, reg, SPI_DUMMY).datain);
}

/*****************************************************************************/
/* pga411_ReadReg_CRC_connu                                                               */
/* Description: la fonction lit le contenu d'un registre du PGA411. Cependant, contrairement
 * à la fonction pga411_ReadReg qui calcule le champ CRC, ici on donne le CRC à l'avance.
 * On spécifie son adresse reg, la valeur du CRC et il renvoie la donnée sur 16 bits.
 * Il est nécessaire de faire deux écritures :
 * la première permet de transmettre au PGA411 l'adresse du registre à lire. La donnée ne peut
 * être lue qu'à la prochaine demande de lecture. C'est le rôle de la seconde demande.
 * La commande associée n'a pas d'importance --> on envoie une donnée appelée SPI_DUMMY.
 */
/*****************************************************************************/
uint16_t pga411_ReadReg_CRC_connu(uint16_t reg, uint32_t CRC_connu) {
	/* first read returns whatever */
	pga411_XmitSPI_no_CRC_compute(READ, reg, SPI_DUMMY, CRC_connu);
	/* second read returns requested data */
	return (pga411_XmitSPI_no_CRC_compute(READ, reg, SPI_DUMMY, CRC_connu).datain);
}

/*****************************************************************************/
/* pga411_ReadReg_CRC_connu_OneRead                                          */
/* Description: la fonction lit le contenu d'un registre du PGA411. Cependant, contrairement
 * à la fonction pga411_ReadReg qui calcule le champ CRC, ici on donne le CRC à l'avance.
 * On spécifie son adresse reg, la valeur du CRC et il renvoie la donnée sur 16 bits.
 * De plus, on ne fait qu'une seule lecture. On part du principe qu'on a
 * réalisé au préalable une lecture qui a positionné la bonne adresse de lecture.
 * On spécifie aussi la même adresse de lecture, car il y a des chances que la
 * prochaine demande de lecture soit du même type
 */
/*****************************************************************************/
uint16_t pga411_ReadReg_CRC_connu_OneRead(uint16_t reg, uint32_t CRC_connu) {
	return (pga411_XmitSPI_no_CRC_compute(READ, reg, SPI_DUMMY, CRC_connu).datain);
}

/* Write data to defined register */
/*****************************************************************************/
/* pga411_WriteReg                                                               */
/* Description: la fonction écrit une donnée (data) dans un registre du PGA411.
 * On spécifie son adresse reg.
 */
/*****************************************************************************/
void pga411_WriteReg(uint16_t reg, uint16_t data) {
	pga411_XmitSPI(WRITE, reg, data);
}


/*****************************************************************************/
/* pga411_XmitSPI                                                               */
/* Description: la fonction construit la trame à écrire dans le PGA411 (si dir = WRITE),
 * ou prépare la lecture d'un registre du PGA411 (dir = READ).
 * Selon le sens lecture/écriture, la fonction détermine la bonne adresse du registre.*
 * La fonction s'occupe aussi de calculer le CRC de la trame à transmettre.
 */
/*****************************************************************************/
pga411_spi_frame_t pga411_XmitSPI(uint16_t dir, uint16_t reg, uint16_t wdata) {
	pga411_spi_frame_t out, in;
	uint32_t error_data;

	/* do we read data ? */
	if (dir == READ) {
		out.addr = pga411_regs[reg].read_add;  /* read address */
	}
	/* or write data ? */
	else {
		out.addr = pga411_regs[reg].write_add;  /* write address */
	}
	/* compose the rest of the frame */
	out.dataout = wdata; /* real data (W) or dummy data (R) */
	out.reserved = 0x00; /* always zero */
	out.mcrc = pga411_Compute_CRC6(out.frame); /* calculate TX CRC */
	in.frame = Transmit_Read_DSPI_32bits(DSPI_PGA411,Master_Mode,DSPI_CTAR_NB,DSPI_PCS_NB,out.frame);
	/* check RX CRC */
	if (pga411_Compute_CRC6(in.frame) != in.scrc) {
		error_data = 1; /* if error -> terminate */
	}
	return(in);
}

/*****************************************************************************/
/* pga411_XmitSPI_no_CRC                                                               */
/* Description: contrairement à la fonction pga411_XmitSPI, il n'y a pas de calcul
 * de CRC ici, qui prend du temps. Pour la transmission, on fournit le CRC à la fonction.
 * Il n'y a pas de vérif du CRC de la trame renvoyée par le PGA411.
 * la fonction construit la trame à écrire dans le PGA411 (si dir = WRITE),
 * ou prépare la lecture d'un registre du PGA411 (dir = READ).
 * Selon le sens lecture/écriture, la fonction détermine la bonne adresse du registre.*
 * La fonction s'occupe aussi de calculer le CRC de la trame à transmettre.
 */
/*****************************************************************************/
pga411_spi_frame_t pga411_XmitSPI_no_CRC_compute(uint16_t dir, uint16_t reg, uint16_t wdata, uint32_t CRC_Tx) {
	pga411_spi_frame_t out, in;

	/* do we read data ? */
	if (dir == READ) {
		out.addr = pga411_regs[reg].read_add;  /* read address */
	}
	/* or write data ? */
	else {
		out.addr = pga411_regs[reg].write_add;  /* write address */
	}
	/* compose the rest of the frame */
	out.dataout = wdata; /* real data (W) or dummy data (R) */
	out.reserved = 0x00; /* always zero */
	out.mcrc = CRC_Tx;
	in.frame = Transmit_Read_DSPI_32bits(DSPI_PGA411,Master_Mode,DSPI_CTAR_NB,DSPI_PCS_NB,out.frame);

	return(in);
}

/*****************************************************************************/
/* pga411_Compute_CRC6                                                       */
/* Description: cette fonction calcule le CRC6 selon la méthode définie
 * par TI dans le PGA411. Le CRC est calculé sur la donnée dataIn.
 * Attention : on envoie la trame complète 32b bits avec le champ CRC précédemment calculé comme donnée d'entrée.
 * Or, ce programme a été conçu pour une donnée sur 24 bits. Il faut donc supprimer les 8 bits de poids
 * faibles de dataIn, qui contient le CRC et revenir sur 24 bits, avant de lancer le calcul du CRC.
 */
/*****************************************************************************/
uint32_t pga411_Compute_CRC6(uint32_t dataIn) {
	uint32_t regdiv; //registre temporaire de calcul du CRC.
	uint32_t MSB = 0;
	uint32_t CRC = 0;
	int cptr_octets, cptr_bits;
	uint32_t mask_MSB = 0x80;
	uint32_t mask_CRC = (1 << CRC6_SIZE)-1;
	uint32_t data_decal = 0;
	uint32_t dataIn_3BYTES = (dataIn >> 8);

	//initialisation regdiv. Seed est aligné sur le MSB d'un octet de donnée
	regdiv = 0xFF & (CRC6_SEED << (8-CRC6_SIZE));

	for (cptr_octets = CRC6_DataByte;cptr_octets >= 1;cptr_octets--) {
		data_decal = 0xFF & (dataIn_3BYTES >> 8*(cptr_octets-1));
		regdiv = regdiv ^ data_decal;

		for (cptr_bits = 7;cptr_bits >= 0;cptr_bits--) {
			MSB = regdiv & mask_MSB;
			if (MSB == mask_MSB) { //if regdiv[MSB] == 1 :
				regdiv = (regdiv <<1) ^ (CRC6_POLY << 2); //le rôle du décalage à gauche du Diviseur n'est pas clair.
						//TI procède comme ça pour obtenir leur résultat. Pourtant, Diviseur est exprimé sur 7 bits !
			}
			else {
				regdiv = regdiv << 1;
			}
		}
	}

	CRC = (regdiv >> (8-CRC6_SIZE)) & mask_CRC;
	return(CRC);
}



/*****************************************************************************/
/* Read SPI status (failure diagnosis)                                       */
/*****************************************************************************/

/*****************************************************************************/
/* Toggle_FaultRes_Pin                                              */
/* Description: permet l'application d'une impulsation à l'état bas sur la broche
 * FaultRes pendant mois de 1 ms. Nécessaire avant de revenir en mode Normal
 */
/*****************************************************************************/
void Toggle_FaultRes_Pin(void) {
	Set_pad_out(PIN_FaultRes,0);
	DelayTimerGeneral_us(1000); //wait for 900 µs
	Set_pad_out(PIN_FaultRes,1);
}


/*****************************************************************************/
/* pga411_read_status_registers                                              */
/* Description: cette fonction lit et renvoie le contenu des 7 registres de
 * statut du PGA411 (DEV_STAT1 à 7). Le PGA411 n'a pas besoin d'être en mode
 * Diagnostics pour que ces registres soient lus.
 */
/*****************************************************************************/
PGA411_status_reg_t pga411_read_status_registers(void) {
	PGA411_Status.dev_stat_1 = pga411_ReadReg(DEV_STAT1);
	PGA411_Status.dev_stat_2 = pga411_ReadReg(DEV_STAT2);
	PGA411_Status.dev_stat_3 = pga411_ReadReg(DEV_STAT3);
	PGA411_Status.dev_stat_4 = pga411_ReadReg(DEV_STAT4);
	PGA411_Status.dev_stat_5 = pga411_ReadReg(DEV_STAT5);
	PGA411_Status.dev_stat_6 = pga411_ReadReg(DEV_STAT6);
	PGA411_Status.dev_stat_7 = pga411_ReadReg(DEV_STAT7);

	return(PGA411_Status);
}

/*****************************************************************************/
/* Sfault_status                                                    */
/* Description: récupère le bit Sfault, contenu dans le registre DEV_STAT4. Il
 * doit refléter le même état que la pin Fault.
 */
/*****************************************************************************/
uint8_t pga411_Sfault_status(void) {
	uint32_t Sfault = 0;
	uint16_t reg_stat4;

	reg_stat4 = pga411_ReadReg(DEV_STAT4);
	if ((reg_stat4 & 0x0008) == 0x0008) {
		Sfault = 1;
	}
	return(Sfault);
}

/*****************************************************************************/
/* Sfault_status                                                    */
/* Description: la fonction lit l'état de la broche Fault du PGA411.
 */
/*****************************************************************************/
uint8_t pga411_FaultPin_status(void) {
	uint8_t FaultPin_State;

	FaultPin_State = Read_pad_in(PIN_Fault);
	return(FaultPin_State);
}

/*****************************************************************************/
/* pga411_GetFault_status                                                    */
/* Description: la fonction renvoie l'état de faute du PGA411, à l'aide de 2 méthodes
 * devant normalement le même état : la lecture de la broche Fault et le bit SFault du
 * registre DEV_STAT4. Si les deux info sont contradictoires, on considère qu'il y
 * a une faute. Si la fonction renvoie un 1, c'est qu'il y a une faute.
 */
/*****************************************************************************/
uint8_t pga411_GetFault_status(void) {
	uint8_t FaultStatus1, FaultStatus2, FaultStatusVote;

	FaultStatus1 = pga411_Sfault_status();
	FaultStatus2 = pga411_FaultPin_status();
	FaultStatusVote = FaultStatus1 | FaultStatus2;

	return(FaultStatusVote);
}



/*****************************************************************************/
/* Read rotor angle position (read mechanical angle, compute electrical angle)*/
/*****************************************************************************/


/*****************************************************************************/
/* PGA411_Get_Angle_Degre_F32                                                       */
/* Description: cette fonction lit le contenu du registre interne DEV_STAT5 (angle_reg),
 * qui contient l'angle acquis par le PGA411, en un angle converti en degré et exprimé en entier sur 32 bits.
 * Le résultat dépend de la résolution sélectionné (10 ou 12 bits)
 */
/*****************************************************************************/
uint32_t pga411_Get_Angle_Degre_F32(uint8_t myResolution) {
	uint16_t angle_read;
	uint32_t myAngle;
	uint32_t Angle_degre = 0;

	//angle_read = pga411_ReadReg(DEV_STAT5);
	angle_read = pga411_ReadReg_CRC_connu_OneRead(DEV_STAT5,CRC_READ_REG_DEV_STAT5);

	if (myResolution == RESO_10BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x03FF & angle_read;
		//angle (°) = 360 * ORDx/2^reso
		myAngle = MLIB_Mul_F32(myAngle,360);
		Angle_degre = MLIB_Div_F32(myAngle,1024);
	}
	else if (myResolution == RESO_12BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x0FFF & angle_read;
		//angle (°) = 360 * ORDx/2^reso
		myAngle = MLIB_Mul_F32(myAngle,360);
		Angle_degre = MLIB_Div_F32(myAngle,4096);
	}
	return(Angle_degre);
}

/*****************************************************************************/
/* PGA411_Get_Angle_Degre_FLT                                                */
/* Description: cette fonction lit le contenu du registre interne DEV_STAT5 (angle_reg),
 * qui contient l'angle acquis par le PGA411, en un angle converti en degré et exprimé
 * en flottant sur 32 bits.Le résultat dépend de la résolution sélectionné (10 ou 12 bits)
 */
/*****************************************************************************/
tFloat pga411_Get_Angle_Degre_FLT(uint8_t myResolution) {
	uint16_t angle_read;
	uint32_t myAngle;
	tFloat myAngleFLT;
	tFloat Angle_degre = 0;

	//utiliser la ligne ci-dessous si on pointe au préalable sur le registre DEV_STAT5
	//angle_read = pga411_ReadReg_CRC_connu_OneRead(DEV_STAT5,CRC_READ_REG_DEV_STAT5);

	//utiliser la ligne ci-dessous si on ne pointe pas au préalable sur le registre DEV_STAT5
	angle_read = pga411_ReadReg_CRC_connu(DEV_STAT5,CRC_READ_REG_DEV_STAT5);

	if (myResolution == RESO_10BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x03FF & angle_read;
		//angle (°) = 360 * ORDx/2^reso
		myAngleFLT = MLIB_Mul_FLT((tFloat) (myAngle),360);
		Angle_degre = MLIB_Div_FLT(myAngleFLT,1024);
	}
	else if (myResolution == RESO_12BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x0FFF & angle_read;
		//angle (°) = 360 * ORDx/2^reso
		myAngleFLT = MLIB_Mul_FLT((tFloat) (myAngle),360);
		Angle_degre = MLIB_Div_FLT(myAngleFLT,4096);
	}
	return(Angle_degre);
}


/*****************************************************************************/
/* pga411_Compute_Angle_Elec_DEG_F32                                             */
/* Description: cette fonction convertit un angle mécanique (mesuré par le PGA411)
 * en un angle électrique. Ce dernier est NbPoles fois plus grand que l'angle mécanique.
 * Le résultat est exprimé en degré sur un entier de 32 bits.
 */
/*****************************************************************************/
uint32_t pga411_Compute_Angle_Elec_DEG_F32(uint32_t angle_meca, uint32_t NbPoles) {
	uint32_t angle_elec;

	angle_elec = MLIB_Mul_F32(angle_meca,NbPoles);
	return(angle_elec);
}


/*****************************************************************************/
/* pga411_Compute_Angle_Elec_DEG_FLT                                             */
/* Description: cette fonction convertit un angle mécanique (mesuré par le PGA411)
 * en un angle électrique. Ce dernier est NbPoles fois plus grand que l'angle mécanique.
 * Le résultat est exprimé en degré sur flottant sur 32 bits.
 */
/*****************************************************************************/
tFloat pga411_Compute_Angle_Elec_DEG_FLT(tFloat angle_meca, uint32_t NbPoles) {
	tFloat angle_elec;

	angle_elec = MLIB_Mul_FLT(angle_meca,(tFloat) (NbPoles));
	return(angle_elec);
}



/*****************************************************************************/
/* Read rotor speed (read mechanical speed, compute electrical speed)        */
/*****************************************************************************/

/*****************************************************************************/
/* pga411_Get_Speed_RPM_F32                                                  */
/* Description: cette fonction lit le contenu du registre interne DEV_STAT6,
 * qui contient la vitesse acquise par le PGA411, en un angle exprimé en RPM et exprimé sur un entier 32 bits.
 * Le résultat dépend de la résolution sélectionné (10 ou 12 bits)
 */
/*****************************************************************************/
int32_t pga411_Get_Speed_RPM_F32(uint8_t myResolution) {
	uint16_t speed_read;
	int16_t speed_sign;
	int32_t Speed_RPM = 0;

	speed_read = pga411_ReadReg(DEV_STAT6);

	if (myResolution == RESO_10BITS) {
		//on teste le signe à l'aide du bit n°9
		if ((speed_read & 0x0200) == 0x0200) {
			//la vitesse est un nombre négatif
			speed_sign = ((~speed_read) +1) & 0x03FF;
			Speed_RPM = MLIB_Mul_F32(-1,speed_sign);
		}
		else {
			//la vitesse est un nombre positif
			Speed_RPM = speed_read & 0x03FF;
		}
	   	//velocity (RPM) = 60 * Fclk(Hz)*(ORDx+1)/2^(2*reso+1), Fclk = 20MHz
		//60*Fclk(Hz)/2^21 = 572.2 --> arrondi à 572
		Speed_RPM = MLIB_Mul_F32(Speed_RPM,572);
	}
	else if (myResolution == RESO_12BITS) {
		//on teste le signe à l'aide du bit n°11
		if ((speed_read & 0x0800) == 0x0800) {
			//la vitesse est un nombre négatif
			speed_sign = ((~speed_read) +1) & 0x0FFF;
			Speed_RPM = MLIB_Mul_F32(-1,speed_sign);
		}
		else {
			//la vitesse est un nombre positif
			Speed_RPM = speed_read & 0x0FFF;
		}
	   	//velocity (RPM) = 60 * Fclk(Hz)*(ORDx+1)/2^(2*reso+1), Fclk = 20MHz
		//60*Fclk(Hz)/2^25 = 35.8 --> arrondi à 36
		Speed_RPM = MLIB_Mul_F32(Speed_RPM,36);
	}
	return(Speed_RPM);
}

/*****************************************************************************/
/* pga411_Get_Speed_RPM_FLT                                                  */
/* Description: cette fonction lit le contenu du registre interne DEV_STAT6,
 * qui contient la vitesse acquise par le PGA411, en un angle exprimé en RPM et exprimé sur un flottant 32 bits.
 * Le résultat dépend de la résolution sélectionné (10 ou 12 bits)
 */
/*****************************************************************************/
tFloat pga411_Get_Speed_RPM_FLT(uint8_t myResolution) {
	uint16_t speed_read;
	int16_t speed_sign;
	tFloat Speed_RPM = 0;

	speed_read = pga411_ReadReg(DEV_STAT6);

	if (myResolution == RESO_10BITS) {
		//on teste le signe à l'aide du bit n°9
		if ((speed_read & 0x0200) == 0x0200) {
			//la vitesse est un nombre négatif
			speed_sign = ((~speed_read) +1) & 0x03FF;
			Speed_RPM = MLIB_Mul_FLT(-1,(tFloat) (speed_sign));
		}
		else {
			//la vitesse est un nombre positif
			Speed_RPM = (tFloat) (speed_read & 0x03FF);
		}
	   	//velocity (RPM) = 60 * Fclk(Hz)*(ORDx+1)/2^(2*reso+1), Fclk = 20MHz
		//60*Fclk(Hz)/2^21 = 572.2 --> arrondi à 572
		Speed_RPM = MLIB_Mul_FLT(Speed_RPM,572);
	}
	else if (myResolution == RESO_12BITS) {
		//on teste le signe à l'aide du bit n°11
		if ((speed_read & 0x0800) == 0x0800) {
			//la vitesse est un nombre négatif
			speed_sign = ((~speed_read) +1) & 0x0FFF;
			Speed_RPM = MLIB_Mul_FLT(-1,(tFloat) (speed_sign));
		}
		else {
			//la vitesse est un nombre positif
			Speed_RPM = speed_read & 0x0FFF;
		}
	   	//velocity (RPM) = 60 * Fclk(Hz)*(ORDx+1)/2^(2*reso+1), Fclk = 20MHz
		//60*Fclk(Hz)/2^25 = 35.8 --> arrondi à 36
		Speed_RPM = MLIB_Mul_FLT(Speed_RPM,36);
	}
	return(Speed_RPM);
}

/*****************************************************************************/
/* Convert_SPEED_DEG_US_TO_RPM_F32                                           */
/* Description: conversion d'une vitesse exprimée en °/µs en une vitesse exprimée en RPM.
 * Les données sont au format entier signé 32 bits
 */
/*****************************************************************************/
int32_t Convert_SPEED_DEG_US_TO_RPM_F32(int32_t Speed_deg_us) {
	int32_t speed_RPM;

	speed_RPM = MLIB_Mul_F32(Speed_deg_us,cte_calcul_RPM_from_deg_us_F32);
	return(speed_RPM);
}

/*****************************************************************************/
/* Convert_SPEED_DEG_US_TO_RPM_FLT                                           */
/* Description: conversion d'une vitesse exprimée en °/µs en une vitesse exprimée en RPM.
 * Les données sont au format entier signé 32 bits
 */
/*****************************************************************************/
tFloat Convert_SPEED_DEG_US_TO_RPM_FLT(tFloat Speed_deg_us) {
	tFloat speed_RPM;

	speed_RPM = MLIB_Mul_FLT(Speed_deg_us,cte_calcul_RPM_from_deg_us_FLT);
	return(speed_RPM);
}


/*****************************************************************************/
/* Convert_AngleElec_Rad_FLT_slow                                                       */
/* Description: cette fonction convertit la valeur de l'angle lu depuis le résolveur (angle_read) sur 16 bits,
 * qui contient l'angle acquis par le PGA411, en un angle électrique convertit en radian, compris entre -pi et +pi.
 * Le résultat dépend de la résolution sélectionné (10 ou 12 bits). On rappelle que l'angle électrique
 * est paire_poles fois plus grand que l'angle mécanique.
 * Remarque : si l'angle d'entrée est compris entre 0 et 180°, la sortie doit être comprise entre 0 et +pi.
 * Si si l'angle d'entrée est compris entre 180° et 360°, la sortie doit être comprise entre -pi et 0.
 *
 * Cette fonction est qualifiée de "slow" car elle est lente, en raison des comparaisons entre float faites
 * directement dans les boucles if. La durée d'exécution de cette fonction varie entre 12 et 32 µs selon la valeur
 * de l'angle ! La fonction Convert_AngleElec_Rad_FLT est à privilégier. Son temps d'exécution ne dépasse pas
 * 12 µs.
 */
/*****************************************************************************/
tFloat Convert_AngleElec_Rad_FLT_slow(uint16_t angle_read, uint32_t myResolution, uint16_t paire_poles) {
	uint32_t myAngle;
	tFloat Angle_rad = 0;

	if (myResolution == RESO_10BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x03FF & angle_read;
		//passage en single
		Angle_rad = (tFloat) (myAngle);
		//angle (°) = 360 * ORDx/2^reso, angle compris entre 0 et 360°
		//angle (rad) = (2*pi) * ORDx/2^reso, angle compris entre 0 et 2*pi
		Angle_rad = MLIB_Div_FLT(Angle_rad,1024);
		Angle_rad = MLIB_Mul_FLT(Angle_rad,TWO_PI_FLT);
		//puis en angle électrique --> l'angle peut être compris entre 0 et 6*pi
		Angle_rad = MLIB_Mul_FLT(Angle_rad,(tFloat) (paire_poles));
	}
	else if (myResolution == RESO_12BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x0FFF & angle_read;
		//passage en single
		Angle_rad = (tFloat) (myAngle);
		//angle (°) = 360 * ORDx/2^reso, angle compris entre 0 et 360°
		//angle (rad) = (2*pi) * ORDx/2^reso, angle compris entre 0 et 2*pi
		Angle_rad = MLIB_Div_FLT(Angle_rad,4096);
		Angle_rad = MLIB_Mul_FLT(Angle_rad,TWO_PI_FLT);
		//, puis en angle électrique
		Angle_rad = MLIB_Mul_FLT(Angle_rad,(tFloat) (paire_poles));
	}

	//on ramène l'angle électrique entre 0 et 360°. On rappelle que le PGA411 ne renvoie que
	//des angles compris entre 0 et 360°.
	//on ramène l'angle entre -pi et +pi.
	if (Angle_rad <= PI_FLT) {
		//on ne fait rien
	}
	else if (Angle_rad <= TWO_PI_FLT) {
		//on le ramène entre -pi et 0.
		//si angle entrée = +pi --> +pi-2pi = -pi
		//si angle entrée = +2pi --> +2pi-2pi = 0
		Angle_rad = MLIB_Sub_FLT(Angle_rad,TWO_PI_FLT);
	}
	else if (Angle_rad <= THREE_PI_FLT) {
		//on le ramène entre 0 et +pi.
		//si angle entrée = +2pi --> +2pi-2pi = 0
		//si angle entrée = +3pi --> +3pi-2pi = +pi
		Angle_rad = MLIB_Sub_FLT(Angle_rad,TWO_PI_FLT);
	}
	else if (Angle_rad <= FOUR_PI_FLT) {
		//on le ramène entre -pi et 0.
		//si angle entrée = +3pi --> +3pi-4pi = -pi
		//si angle entrée = +4pi --> +4pi-4pi = 0
		Angle_rad = MLIB_Sub_FLT(Angle_rad,FOUR_PI_FLT);
	}
	else if (Angle_rad <= FIVE_PI_FLT) {
		//on le ramène entre 0 et +pi.
		//si angle entrée = +4pi --> +4pi-4pi = 0
		//si angle entrée = +5pi --> +5pi-4pi = +pi
		Angle_rad = MLIB_Sub_FLT(Angle_rad,FOUR_PI_FLT);
	}
	else if (Angle_rad <= SIX_PI_FLT) {
		//on le ramène entre -pi et 0.
		//si angle entrée = +5pi --> +5pi-6pi = -pi
		//si angle entrée = +5pi --> +6pi-6pi = 0
		Angle_rad = MLIB_Sub_FLT(Angle_rad,SIX_PI_FLT);
	}

	return(Angle_rad);
}

/*****************************************************************************/
/* Convert_AngleElec_Rad_FLT                                                       */
/* Description: cette fonction convertit la valeur de l'angle lu depuis le résolveur (angle_read) sur 16 bits,
 * qui contient l'angle acquis par le PGA411, en un angle électrique convertit en radian, compris entre -pi et +pi.
 * Le résultat dépend de la résolution sélectionné (10 ou 12 bits). On rappelle que l'angle électrique
 * est paire_poles fois plus grand que l'angle mécanique.
 * Remarque : si l'angle d'entrée est compris entre 0 et 180°, la sortie doit être comprise entre 0 et +pi.
 * Si si l'angle d'entrée est compris entre 180° et 360°, la sortie doit être comprise entre -pi et 0.
 */
/*****************************************************************************/
tFloat Convert_AngleElec_Rad_FLT(uint16_t angle_read, uint32_t myResolution, uint16_t paire_poles) {
	uint32_t myAngle;
	tFloat Angle_rad = 0;
	tFloat d_Angle, Sign;

	if (myResolution == RESO_10BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x03FF & angle_read;
		//passage en single
		Angle_rad = (tFloat) (myAngle);
		//angle (°) = 360 * ORDx/2^reso, angle compris entre 0 et 360°
		//angle (rad) = (2*pi) * ORDx/2^reso, angle compris entre 0 et 2*pi
		Angle_rad = MLIB_Div_FLT(Angle_rad,1024);
		Angle_rad = MLIB_Mul_FLT(Angle_rad,TWO_PI_FLT);
		//puis en angle électrique --> l'angle peut être compris entre 0 et 6*pi
		Angle_rad = MLIB_Mul_FLT(Angle_rad,(tFloat) (paire_poles));
	}
	else if (myResolution == RESO_12BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x0FFF & angle_read;
		//passage en single
		Angle_rad = (tFloat) (myAngle);
		//angle (°) = 360 * ORDx/2^reso, angle compris entre 0 et 360°
		//angle (rad) = (2*pi) * ORDx/2^reso, angle compris entre 0 et 2*pi
		Angle_rad = MLIB_Div_FLT(Angle_rad,4096);
		Angle_rad = MLIB_Mul_FLT(Angle_rad,TWO_PI_FLT);
		//, puis en angle électrique
		Angle_rad = MLIB_Mul_FLT(Angle_rad,(tFloat) (paire_poles));
	}

	d_Angle = MLIB_Sub_FLT(Angle_rad,PI_FLT);
	Sign = GFLIB_Sign_FLT(d_Angle);
	if (Sign == (tFloat)(-1)) {
		//rien, Angle_rad est compris entre 0 et pi.
	}
	else {
		d_Angle = MLIB_Sub_FLT(Angle_rad,TWO_PI_FLT);
		Sign = GFLIB_Sign_FLT(d_Angle);
		if (Sign == (tFloat) (-1)) {
			//on le ramène entre -pi et 0.
			//si angle entrée = +pi --> +pi-2pi = -pi
			//si angle entrée = +2pi --> +2pi-2pi = 0
			Angle_rad = MLIB_Sub_FLT(Angle_rad,TWO_PI_FLT);
		}
		else {
			d_Angle = MLIB_Sub_FLT(Angle_rad,THREE_PI_FLT);
			Sign = GFLIB_Sign_FLT(d_Angle);
			if (Sign == (tFloat) (-1)) {
				//on le ramène entre 0 et +pi.
				//si angle entrée = +2pi --> +2pi-2pi = 0
				//si angle entrée = +3pi --> +3pi-2pi = +pi
				Angle_rad = MLIB_Sub_FLT(Angle_rad,TWO_PI_FLT);
			}

			else {
				d_Angle = MLIB_Sub_FLT(Angle_rad,FOUR_PI_FLT);
				Sign = GFLIB_Sign_FLT(d_Angle);
				if (Sign == (tFloat) (-1)) {
					//on le ramène entre -pi et 0.
					//si angle entrée = +3pi --> +3pi-4pi = -pi
					//si angle entrée = +4pi --> +4pi-4pi = 0
					Angle_rad = MLIB_Sub_FLT(Angle_rad,FOUR_PI_FLT);
				}

				else {
					d_Angle = MLIB_Sub_FLT(Angle_rad,FIVE_PI_FLT);
					Sign = GFLIB_Sign_FLT(d_Angle);
					if (Sign == (tFloat) (-1)) {
						//on le ramène entre 0 et +pi.
						//si angle entrée = +4pi --> +4pi-4pi = 0
						//si angle entrée = +5pi --> +5pi-4pi = +pi
						Angle_rad = MLIB_Sub_FLT(Angle_rad,FOUR_PI_FLT);
					}

					else {
						d_Angle = MLIB_Sub_FLT(Angle_rad,SIX_PI_FLT);
						Sign = GFLIB_Sign_FLT(d_Angle);
						if (Sign == (tFloat) (-1)) {
							//on le ramène entre -pi et 0.
							//si angle entrée = +5pi --> +5pi-6pi = -pi
							//si angle entrée = +5pi --> +6pi-6pi = 0
							Angle_rad = MLIB_Sub_FLT(Angle_rad,SIX_PI_FLT);
						}
					}
				}
			}
		}
	}

	return(Angle_rad);
}

/*****************************************************************************/
/* Convert_AngleMeca_Rad_FLT                                                       */
/* Description: cette fonction convertit la valeur de l'angle lu depuis le résolveur (angle_read) sur 16 bits,
 * qui contient l'angle acquis par le PGA411, en un angle mécanique convertit en radian, compris entre -pi et +pi.
 * Le résultat dépend de la résolution sélectionné (10 ou 12 bits).
 * Remarque : si l'angle d'entrée est compris entre 0 et 180°, la sortie doit être comprise entre 0 et +pi.
 * Si l'angle d'entrée est compris entre 180° et 360°, la sortie doit être comprise entre -pi et 0.
 */
/*****************************************************************************/
tFloat Convert_AngleMeca_Rad_FLT(uint16_t angle_read, uint32_t myResolution) {
	uint32_t myAngle;
	tFloat Angle_rad = 0;
	tFloat d_Angle, Sign;

	if (myResolution == RESO_10BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x03FF & angle_read;
		//passage en single
		Angle_rad = (tFloat) (myAngle);
		//angle (°) = 360 * ORDx/2^reso, angle compris entre 0 et 360°
		//angle (rad) = (2*pi) * ORDx/2^reso, angle compris entre 0 et 2*pi
		Angle_rad = MLIB_Div_FLT(Angle_rad,1024);
		Angle_rad = MLIB_Mul_FLT(Angle_rad,TWO_PI_FLT);
	}
	else if (myResolution == RESO_12BITS) {
		//masque pour ne garder que les bits de données du registre
		myAngle = 0x0FFF & angle_read;
		//passage en single
		Angle_rad = (tFloat) (myAngle);
		//angle (°) = 360 * ORDx/2^reso, angle compris entre 0 et 360°
		//angle (rad) = (2*pi) * ORDx/2^reso, angle compris entre 0 et 2*pi
		Angle_rad = MLIB_Div_FLT(Angle_rad,4096);
		Angle_rad = MLIB_Mul_FLT(Angle_rad,TWO_PI_FLT);
	}

	d_Angle = MLIB_Sub_FLT(Angle_rad,PI_FLT);
	Sign = GFLIB_Sign_FLT(d_Angle);
	if (Sign == (tFloat)(-1)) {
		//rien, Angle_rad est compris entre 0 et pi.
	}
	else {
		d_Angle = MLIB_Sub_FLT(Angle_rad,TWO_PI_FLT);
		Sign = GFLIB_Sign_FLT(d_Angle);
		if (Sign == (tFloat) (-1)) {
			//on le ramène entre -pi et 0.
			//si angle entrée = +pi --> +pi-2pi = -pi
			//si angle entrée = +2pi --> +2pi-2pi = 0
			Angle_rad = MLIB_Sub_FLT(Angle_rad,TWO_PI_FLT);
		}
	}

	return(Angle_rad);
}


/*****************************************************************************/
/* Adapt_Angle_minus_pi_plus_pi_FLT                                          */
/* Description: la fonction retourne un angle exprimé en radian compris entre -pi et +pi.
 * L'angle passé en entrée est aussi exprimé en radian.
 */
/*****************************************************************************/
tFloat Adapt_Angle_minus_pi_plus_pi_FLT(tFloat inAngle) {
	tFloat outAngle = 0;

	if (inAngle < MINUS_PI_FLT) {
		outAngle = MLIB_Add_FLT(inAngle,TWO_PI_FLT);
	}
	else if (inAngle > PI_FLT) {
		outAngle = MLIB_Sub_FLT(inAngle,TWO_PI_FLT);
	}
	else {
		outAngle = inAngle;
	}
	return(outAngle);
}


/*****************************************************************************/
/* Gestion des interruptions                                      */
/*****************************************************************************/

/*****************************************************************************/
/* ERQ_PGA411_Fault_isr                                                   */
/* Description: gestion de l'interruption associée à un passage à 1 de la broche
 * Fault (entrée ERQ21).
 */
/*****************************************************************************/
void ERQ_PGA411_Fault_isr(void) {
	if (SIUL2.DISR0.B.EIF21 == 1) {
		//RAZ du flag
		SIUL2.DISR0.B.EIF21 = 1;

		//pour l'instant, le traitement de la détection d'une faute se limite à une commutation
		//de la broche FaultRes, pour mettre à 0 la broche Fault (valide uniquement si toutes les
		//conditions de faute ont été résolues).
		Toggle_FaultRes_Pin();
	}
	Set_pad_out(User_LED_Green,LED_OFF);
	Set_pad_out(User_LED_Red,LED_ON);
}
