/*
 * DSPIConfig.c
 *
 *  Created on: 21 août 2018
 *      Author: admin_aboyer
 *
 *  Configuration et utilisation des modules DSPI (Serial Peripheral Interface)
 */


#include "MPC5744P.h"
#include "DSPIConfig.h"
#include "ConfigGPIO.h"
#include "pga411.h"


/*****************************************************************************/
/* Halt_All_DSPI_modules                                                     */
/* Description: configure les pads d'IO associés à DSPI0                     */
/*****************************************************************************/
void Halt_All_DSPI_modules(void) {
	DSPI_Enter_Exit_HaltMode(Module_DSPI0,Stop_DSPI_Transfer);
	DSPI_Enter_Exit_HaltMode(Module_DSPI1,Stop_DSPI_Transfer);
	DSPI_Enter_Exit_HaltMode(Module_DSPI2,Stop_DSPI_Transfer);
	DSPI_Enter_Exit_HaltMode(Module_DSPI3,Stop_DSPI_Transfer);
}

/*****************************************************************************/
/* init_dspi0_ports                                                          */
/* Description: configure les pads d'IO associés à DSPI0                     */
/*****************************************************************************/
void init_dspi0_ports(void) {
	  /* Master - DSPI_0 */
	  SIUL2.MSCR[PC6].B.SSS = 1;                 /* Pad PC6: Source signal is DSPI_0 SOUT  */
	  SIUL2.MSCR[PC6].B.OBE = 1;                 /* Pad PC6: OBE=1. */
	  SIUL2.MSCR[PC6].B.SRC = 3;                 /* Pad PC6: Full strength slew rate */

	  SIUL2.MSCR[PC5].B.SSS = 1;                /* Pad PC5: Source signal is DSPI_0 CLK  */
	  SIUL2.MSCR[PC5].B.OBE = 1;                /* Pad PC5: OBE=1. */
	  SIUL2.MSCR[PC5].B.SRC = 3;                /* Pad PC5: Full strength slew rate */

	  SIUL2.MSCR[PC7].B.IBE = 1;                /* Pad PC7: Enable pad for input DSPI_0 SIN */
	  SIUL2.IMCR[41].B.SSS = 1;            		/* Pad PC7: connected to pad PC7 */

	  SIUL2.MSCR[PC4].B.SSS = 1;                 /* Pad PC4: Source signal is DSPI_0 CS0  */
	  SIUL2.MSCR[PC4].B.OBE = 1;                 /* Pad PC4: OBE=1. */
	  SIUL2.MSCR[PC4].B.SRC = 3;                 /* Pad PC4: Full strength slew rate */
}

/*****************************************************************************/
/* init_dspi1_ports                                                                 */
/* Description: configure les pads d'IO associés à DSPI1         */
/*****************************************************************************/
void init_dspi1_ports(void) {
	  /* Master - DSPI_1 */
	  SIUL2.MSCR[PA7].B.SSS = 1;                 /* Pad PA7: Source signal is DSPI_1 SOUT  */
	  SIUL2.MSCR[PA7].B.OBE = 1;                 /* Pad PA7: OBE=1. */
	  SIUL2.MSCR[PA7].B.SRC = 3;                 /* Pad PA7: Full strength slew rate */

	  SIUL2.MSCR[PA6].B.SSS = 1;                /* Pad PA6: Source signal is DSPI_1 CLK  */
	  SIUL2.MSCR[PA6].B.OBE = 1;                /* Pad PA6: OBE=1. */
	  SIUL2.MSCR[PA6].B.SRC = 3;                /* Pad PA6: Full strength slew rate */

	  SIUL2.MSCR[PA8].B.IBE = 1;                /* Pad PA8: Enable pad for input DSPI_1 SIN */
	  SIUL2.IMCR[44].B.SSS = 1;            		/* Pad PA8: connected to pad PA8 */

	  SIUL2.MSCR[PA5].B.SSS = 1;                 /* Pad PA5: Source signal is DSPI_1 CS0  */
	  SIUL2.MSCR[PA5].B.OBE = 1;                 /* Pad PA5: OBE=1. */
	  SIUL2.MSCR[PA5].B.SRC = 3;                 /* Pad PA5: Full strength slew rate */
}


/*****************************************************************************/
/* init_dspi2_ports                                                                 */
/* Description: configure les pads d'IO associés à DSPI2         */
/*****************************************************************************/
void init_dspi2_ports(void) {
	  SIUL2.MSCR[PIN_DSPI2_SCK].B.SSS = 2;                /* Pad PA0: DSPI2_SCLK */
	  SIUL2.MSCR[PIN_DSPI2_SCK].B.OBE = 1;                /* Pad PA0: OBE=1. */
	  SIUL2.MSCR[PIN_DSPI2_SCK].B.SRC = 3;                /* Pad PA0: Full strength slew rate */

	  SIUL2.MSCR[PIN_DSPI2_SOUT].B.SSS = 2;                /* Pad PA1: DSPI_2 SOUT */
	  SIUL2.MSCR[PIN_DSPI2_SOUT].B.OBE = 1;                /* Pad PA1: OBE=1. */
	  SIUL2.MSCR[PIN_DSPI2_SOUT].B.SRC = 3;                /* Pad PA1: Full strength slew rate */

	  SIUL2.MSCR[PIN_DSPI2_SIN].B.IBE = 1;                /* Pad PA2: EDSPI_2 SIN */
	  SIUL2.IMCR[47].B.SSS = 2;            /* Pad PA2: connected to pad */

	  SIUL2.MSCR[PIN_DSPI2_NCS3].B.SSS = 2;                 /* Pad PA3: DSP2_ CS3  */
	  SIUL2.MSCR[PIN_DSPI2_NCS3].B.OBE = 1;                 /* Pad PA5: OBE=1. */
	  SIUL2.MSCR[PIN_DSPI2_NCS3].B.SRC = 3;                 /* Pad PA5: Full strength slew rate */
}

/*****************************************************************************/
/* init_dspi0                                                                 */
/* Description: configure les propriétés générale de DSPI0 (sauf propriétés temporelles)     */
/*****************************************************************************/
void init_dspi0(void) {
	/* Configure DSPI_1 as master, CS0 active at low state, stop transfers */
	SPI_0.MCR.B.HALT = Stop_DSPI_Transfer;
	SPI_0.MCR.B.MDIS = Enable_SPI_clock;
	SPI_0.MCR.B.MSTR = Master_Mode;
	SPI_0.MCR.B.PCSIS = Inactive_CS_High << (CS0_Asserted-1); //état actif = état bas -->
					//état inactif = état haut
	SPI_0.MCR.B.DIS_TXF = Enable_SPI_TX_FIFO;
	SPI_0.MCR.B.DIS_RXF = Enable_SPI_RX_FIFO;
	SPI_0.MCR.B.CONT_SCKE = 0; //Continuous SCK disabled.
}

/*****************************************************************************/
/* init_dspi1                                                                 */
/* Description: configure les propriétés générale de DSPI1 (sauf propriétés temporelles)     */
/*****************************************************************************/
void init_dspi1(void) {
	/* Configure DSPI_1 as master, CS0 active at low state, stop transfers */
	SPI_1.MCR.B.HALT = Stop_DSPI_Transfer;
	SPI_1.MCR.B.MDIS = Enable_SPI_clock;
	SPI_1.MCR.B.MSTR = Master_Mode;
	SPI_1.MCR.B.PCSIS = Inactive_CS_High << (CS0_Asserted-1); //état actif = état bas -->
					//état inactif = état haut
	SPI_1.MCR.B.DIS_TXF = Enable_SPI_TX_FIFO;
	SPI_1.MCR.B.DIS_RXF = Enable_SPI_RX_FIFO;
	SPI_1.MCR.B.CONT_SCKE = 0; //Continuous SCK disabled.
}

/*****************************************************************************/
/* init_dspi2                                                                 */
/* Description: configure les propriétés générale de DSPI2 (sauf propriétés temporelles)     */
/*****************************************************************************/
void init_dspi2(void) {
	/* Configure DSPI_2 as master, CS3 active at low state, stop transfers */
	SPI_2.MCR.B.HALT = Stop_DSPI_Transfer;
	SPI_2.MCR.B.MDIS = Enable_SPI_clock;
	SPI_2.MCR.B.MSTR = Master_Mode;
	SPI_2.MCR.B.PCSIS = Inactive_CS_High*CS3_Asserted;  //état actif = état bas -->
	//état inactif = état haut. Ce champ est ignoré quand le module est esclave (CS actif à l'état bas).
	SPI_2.MCR.B.DIS_TXF = Enable_SPI_TX_FIFO;
	SPI_2.MCR.B.DIS_RXF = Enable_SPI_RX_FIFO;
	SPI_2.MCR.B.CONT_SCKE = 0; //Continuous SCK disabled.
}

/*****************************************************************************/
/* SetTiming_dspi0                                                                 */
/* Description: configure les paramètres temporelles associés au registre CTAR[CTAR_nb] de DSPI0     */
/* Le GD3000 impose que la donnée Sout change sur le front montant de SCK et soit échantillonné
 * sur front descendant de SCK --> CPOL = 0 et CPHA = 1
 */
/*****************************************************************************/
void SetTiming_dspi0(uint8_t CTAR_nb) {
	/*Configure Clock and transfer attributes */

	SPI_0.MODE.CTAR[CTAR_nb].B.FMSZ = 0x7; //frame size = 8 bits --> FMSZ = 8-1 = 7
	SPI_0.MODE.CTAR[CTAR_nb].B.CPOL = Inactive_SCLK_Low;
	SPI_0.MODE.CTAR[CTAR_nb].B.CPHA = 1; //0;
	SPI_0.MODE.CTAR[CTAR_nb].B.LSBFE = MSB_First;

	//config baud rate
	//Baud rate = Fsys/PBR*(1+DBR)/BR
	//With Fsys = 50 MHz and DBR = 0, PBR = 3 (0b01), BR = 128 (0b0111)
	// --> Baud Rate = 50M/3*(1+0)/128 = 130.21 kBds
	SPI_0.MODE.CTAR[CTAR_nb].B.DBR = Normal_DR;
	SPI_0.MODE.CTAR[CTAR_nb].B.PBR = 0b01; //(=Prescaler_Baud_Rate = 3), prescaler value for the baud rate
	SPI_0.MODE.CTAR[CTAR_nb].B.BR = 0b0111; //(128), baud rate scaler: SCK baud rate = Fsys/PBR*(1+DBR)/BR

	//Config delay between PCS and SCK signals
	//Delay between PCS and SCK signals (from assertion of the CS_x signal to the first SCK_x edge):
	//Tcsc = 1/Fsys*PCSSCK*CSSCK
	//With Fsys = 50M, PCSSCK = 1 (0b00) and CSSCK = 64 (0b0101)
	// --> Tcsc = 1/50M*1*64 = 1.28 µs
	SPI_0.MODE.CTAR[CTAR_nb].B.PCSSCK = 0b00; //(=1), prescaler value for delay between CS assertion and clk first edge
	SPI_0.MODE.CTAR[CTAR_nb].B.CSSCK = 0b0101; //(=64), scaler value for the PCS to SCK delay: Tcsc = 1/Fsys*PCSSCK*CSSCK

	//Config after SCK delay:
	//Delay between last edge of SCK and negation of CS (after SCK delay):
	//Tasc = 1/Fsys*PASC*ASC
	//With Fsys = 50 M, PASC = 1 (0b00) and ASC = 64 (0b0101)
	// --> Tasc = 1/50M*1*32 = .28 µs
	SPI_0.MODE.CTAR[CTAR_nb].B.PASC = 0b00; //(=1), prescaler value for delay between clk last edge and negation of CS
	SPI_0.MODE.CTAR[CTAR_nb].B.ASC = 0b0101; //(=64), scaler value for the After SCK Delay: Tasc = 1/Fsys*PASC*CASC

	//Config delay after transfer:
	//Delay between negation of the CSx signal for a frame and the assertion of the CSx signal
	//for the next frame (delay after transfer)
	//Tdt = 1/Fsys*PDT*DT
	//With Fsys = 50M, PDT = 1 (0b00) and DT = 32 (0b0101)
	// --> Tdt = 1/50M*1*64 = 1.28 µs
	SPI_0.MODE.CTAR[CTAR_nb].B.PDT = 0b00; //(=1), prescaler value for delay between negation of CS and assertion of CS at the next frame
	SPI_0.MODE.CTAR[CTAR_nb].B.DT = 0b0101; //(=64), Delay after Transfer Scaler: Tdt=1/Fsys*PDT*DT
}

/*****************************************************************************/
/* SetTiming_dspi1                                                                 */
/* Description: configure les paramètres temporelles associés au registre CTAR[CTAR_nb] de DSPI1     */
/*****************************************************************************/
void SetTiming_dspi1(uint8_t CTAR_nb) {
	/*Configure Clock and transfer attributes */

	SPI_1.MODE.CTAR[CTAR_nb].B.FMSZ = 0x7; //frame size = 8 bits --> FMSZ = 8-1 = 7
	SPI_1.MODE.CTAR[CTAR_nb].B.CPOL = Inactive_SCLK_Low;
	SPI_1.MODE.CTAR[CTAR_nb].B.CPHA = 0;
	SPI_1.MODE.CTAR[CTAR_nb].B.LSBFE = MSB_First;

	//config baud rate
	//Baud rate = Fsys/PBR*(1+DBR)/BR
	//With Fsys = 50 MHz and DBR = 0, PBR = 3 (0b01), BR = 128 (0b0111)
	// --> Baud Rate = 50M/3*(1+0)/128 = 130.21 kBds
	SPI_1.MODE.CTAR[CTAR_nb].B.DBR = Normal_DR;
	SPI_1.MODE.CTAR[CTAR_nb].B.PBR = 0b01; //(=Prescaler_Baud_Rate = 3), prescaler value for the baud rate
	SPI_1.MODE.CTAR[CTAR_nb].B.BR = 0b0111; //(128), baud rate scaler: SCK baud rate = Fsys/PBR*(1+DBR)/BR

	//Config delay between PCS and SCK signals
	//Delay between PCS and SCK signals (from assertion of the CS_x signal to the first SCK_x edge):
	//Tcsc = 1/Fsys*PCSSCK*CSSCK
	//With Fsys = 50M, PCSSCK = 1 (0b00) and CSSCK = 64 (0b0101)
	// --> Tcsc = 1/50M*1*64 = 1.28 µs
	SPI_1.MODE.CTAR[CTAR_nb].B.PCSSCK = 0b00; //(=1), prescaler value for delay between CS assertion and clk first edge
	SPI_1.MODE.CTAR[CTAR_nb].B.CSSCK = 0b0101; //(=64), scaler value for the PCS to SCK delay: Tcsc = 1/Fsys*PCSSCK*CSSCK

	//Config after SCK delay:
	//Delay between last edge of SCK and negation of CS (after SCK delay):
	//Tasc = 1/Fsys*PASC*ASC
	//With Fsys = 50 M, PASC = 1 (0b00) and ASC = 64 (0b0101)
	// --> Tasc = 1/50M*1*32 = .28 µs
	SPI_1.MODE.CTAR[CTAR_nb].B.PASC = 0b00; //(=1), prescaler value for delay between clk last edge and negation of CS
	SPI_1.MODE.CTAR[CTAR_nb].B.ASC = 0b0101; //(=64), scaler value for the After SCK Delay: Tasc = 1/Fsys*PASC*CASC

	//Config delay after transfer:
	//Delay between negation of the CSx signal for a frame and the assertion of the CSx signal
	//for the next frame (delay after transfer)
	//Tdt = 1/Fsys*PDT*DT
	//With Fsys = 50M, PDT = 1 (0b00) and DT = 32 (0b0101)
	// --> Tdt = 1/50M*1*64 = 1.28 µs
	SPI_1.MODE.CTAR[CTAR_nb].B.PDT = 0b00; //(=1), prescaler value for delay between negation of CS and assertion of CS at the next frame
	SPI_1.MODE.CTAR[CTAR_nb].B.DT = 0b0101; //(=64), Delay after Transfer Scaler: Tdt=1/Fsys*PDT*DT
}


/*****************************************************************************/
/* SetTiming_dspi2                                                                */
/* Description: configure les paramètres temporelles associés au registre CTAR[CTAR_nb] de DSPI2    */
/*****************************************************************************/
void SetTiming_dspi2(uint8_t CTAR_nb) {
	/*Configure Clock and transfer attributes */
	//Baud rate = Fsys/PBR*(1+DBR)/BR
	//With Fsys = 50 MHz and DBR = 0, PBR = 2 (0b00), BR = 8 (0b0011)
	// --> Baud Rate = 50M/2*(1+0)/8 = 3.125 MBds (baud period = 320 ns)
	//
	//Delay between PCS and SCK signals (from assertion of the CS_x signal to the first SCK_x edge):
	//Tcsc = 1/Fsys*PCSSCK*CSSCK
	//With Fsys = 50M, PCSSCK = 1 (0b00) and CSSCK = 32 (0b0100)
	// --> Tcsc = 1/50M*1*32 = 0.64 µs
	//
	//Delay between last edge of SCK and negation of CS (after SCK delay):
	//Tasc = 1/Fsys*PASC*ASC
	//With Fsys = 50 M, PASC = 1 (0b00) and ASC = 32 (0b0100)
	// --> Tasc = 1/50M*1*32 = 0.64 µs
	//
	//Delay between negation of the CSx signal for a frame and the assertion of the CSx signal
	//for the next frame (delay after transfer)
	//Tdt = 1/Fsys*PDT*DT
	//With Fsys = 50 M, PDT = 1 (0b00) and DT = 32 (0b0100)
	// --> Tdt = 1/50M*1*32 = 0.64 µs


	SPI_2.MODE.CTAR[CTAR_nb].B.FMSZ = 0xF; //frame size = 16 bits --> FMSZ = 16-1 = 15
	SPI_2.MODE.CTAR[CTAR_nb].B.CPOL = Inactive_SCLK_Low;
	SPI_2.MODE.CTAR[CTAR_nb].B.CPHA = 1; //SPI data is clocked in on the falling edge
	SPI_2.MODE.CTAR[CTAR_nb].B.LSBFE = MSB_First;

	//config baud rate
	SPI_2.MODE.CTAR[CTAR_nb].B.DBR = Normal_DR;
	SPI_2.MODE.CTAR[CTAR_nb].B.PBR = 0b00; //(=Prescaler_Baud_Rate = 2), prescaler value for the baud rate
	SPI_2.MODE.CTAR[CTAR_nb].B.BR = 0b0011; //(64), baud rate scaler: SCK baud rate = Fsys/PBR*(1+DBR)/BR

	//Config delay between PCS and SCK signals
	SPI_2.MODE.CTAR[CTAR_nb].B.PCSSCK = 0b00; //(=1), prescaler value for delay between CS assertion and clk first edge
	SPI_2.MODE.CTAR[CTAR_nb].B.CSSCK = 0b0100; //(=32), scaler value for the PCS to SCK delay: Tcsc = 1/Fsys*PCSSCK*CSSCK

	//Config after SCK delay:
	SPI_2.MODE.CTAR[CTAR_nb].B.PASC = 0b00; //(=1), prescaler value for delay between clk last edge and negation of CS
	SPI_2.MODE.CTAR[CTAR_nb].B.ASC = 0b0100; //(=32), scaler value for the After SCK Delay: Tasc = 1/Fsys*PASC*CASC

	//Config delay after transfer:
	SPI_2.MODE.CTAR[CTAR_nb].B.PDT = 0b00; //(=1), prescaler value for delay between negation of CS and assertion of CS at the next frame
	SPI_2.MODE.CTAR[CTAR_nb].B.DT = 0b0100; //(=32), Delay after Transfer Scaler: Tdt=1/Fsys*PDT*DT
}

/*****************************************************************************/
/* DSPI_Enter_Exit_HaltMode                                                                */
/* Description: configure l'entrée ou la sortie du mode halt pour le module DSPI*/
/* indiqué par Module_nb. */
/* HaltEn : si = 1 --> mode halt --> aucun transfert. */
 /* Sinon  --> start transfer.*/
/*****************************************************************************/
void DSPI_Enter_Exit_HaltMode(uint8_t Module_nb, uint8_t HaltEn) {
	if (Module_nb ==  Module_DSPI0) {
		SPI_0.MCR.B.HALT = HaltEn;
	}
	if (Module_nb ==  Module_DSPI1) {
		SPI_1.MCR.B.HALT = HaltEn;
	}
	if (Module_nb ==  Module_DSPI2) {
		SPI_2.MCR.B.HALT = HaltEn;
	}
	if (Module_nb ==  Module_DSPI3) {
		SPI_3.MCR.B.HALT = HaltEn;
	}
}

/*****************************************************************************/
/* Transmit_DSPI                                                               */
/* Description: lancement d'uen transmission par le module DSPI[Module_nb].
 * MasterSlave indique si le module est en mode maitre ou esclave.
 * CTAR_nb indique quelle configuration temporelle définie dans un registre CTAR[CTAR_nb]
 * est utilisée. PCS_nb indique quel signal PCS est utilisé pour le transfert.
 * Message est le message à transmettre (sur 16 bits).
 * En mode slave, les infos CTAR_nb et PCS_nb sont ignorées
 * Le signal PCS est continu selon l'entrée CS_Cont (bit CONT).
 * On transmet des messages continuellement
 * (pas de queue) (EOQ = 0). On ne remet pas à 0 le compteur de transfert (CTCNT = 0).
 */
/*****************************************************************************/
void Transmit_DSPI(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,
		uint16_t Message, uint8_t CS_Cont,uint32_t EOQ) {
	uint32_t PUSHR = 0;
	uint32_t Asserted_CS = 0;
	//uint32_t EOQ = 0;

	Asserted_CS = PCS_nb;
	if (MasterSlave == Master_Mode) {
		PUSHR = PUSHR | (CS_Cont << 31) | (EOQ << 27) | (CTAR_nb << 28) | (Asserted_CS << 16);
	}
	PUSHR = PUSHR | (Message);

	if (Module_nb ==  Module_DSPI0) {
		if (MasterSlave == Master_Mode) {
			SPI_0.PUSHR.PUSHR.R = PUSHR;
			//while (SPI_0.SR.B.TCF != 1){}
		}
		else if (MasterSlave == Slave_Mode) {
			SPI_0.PUSHR.PUSHR_SLAVE.R = PUSHR;
		}
	}
	else if (Module_nb ==  Module_DSPI1) {
		if (MasterSlave == Master_Mode) {
			SPI_1.PUSHR.PUSHR.R = PUSHR;
			//while (SPI_1.SR.B.TCF != 1){}
		}
		else if (MasterSlave == Slave_Mode) {
			SPI_1.PUSHR.PUSHR_SLAVE.R = PUSHR;
		}
	}
	else if (Module_nb ==  Module_DSPI2) {
		if (MasterSlave == Master_Mode) {
			SPI_2.PUSHR.PUSHR.R = PUSHR;
			//while (SPI_2.SR.B.TCF != 1){}
		}
		else if (MasterSlave == Slave_Mode) {
			SPI_2.PUSHR.PUSHR_SLAVE.R = PUSHR;
		}
	}
	else if (Module_nb ==  Module_DSPI3) {
		if (MasterSlave == Master_Mode) {
			SPI_3.PUSHR.PUSHR.R = PUSHR;
			//while (SPI_3.SR.B.TCF != 1){}
		}
		else if (MasterSlave == Slave_Mode) {
			SPI_3.PUSHR.PUSHR_SLAVE.R = PUSHR;
		}
	}
}

/*****************************************************************************/
/* Transmit_DSPI_8bits
 * Vérifier que la config de taille du message est bien de 8 bits           */
/* Description: lancement d'uen transmission par le module DSPI[Module_nb].
 * MasterSlave indique si le module est en mode maitre ou esclave.
 * CTAR_nb indique quelle configuration temporelle définie dans un registre CTAR[CTAR_nb]
 * est utilisée. PCS_nb indique quel signal PCS est utilisé pour le transfert.
 * Message est le message à transmettre (sur 8 bits).
 * En mode slave, les infos CTAR_nb et PCS_nb sont ignorées
 * Le signal PCS est continu selon l'entrée CS_Cont (bit CONT).
 * On transmet des messages continuellement
 * (pas de queue) (EOQ = 0). On ne remet pas à 0 le compteur de transfert (CTCNT = 0).
 */
/*****************************************************************************/
void Transmit_DSPI_8bits(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,
		uint8_t Message, uint8_t CS_Cont,uint32_t EOQ) {
	uint32_t PUSHR = 0;
	uint32_t Asserted_CS = 0;
	//uint32_t EOQ = 0;

	Asserted_CS = PCS_nb;
	if (MasterSlave == Master_Mode) {
		PUSHR = PUSHR | (CS_Cont << 31) | (EOQ << 27) | (CTAR_nb << 28) | (Asserted_CS << 16);
	}
	PUSHR = PUSHR | (Message);

	if (Module_nb ==  Module_DSPI0) {
		if (MasterSlave == Master_Mode) {
			SPI_0.PUSHR.PUSHR.R = PUSHR;
			//while (SPI_0.SR.B.TCF != 1){}
		}
		else if (MasterSlave == Slave_Mode) {
			SPI_0.PUSHR.PUSHR_SLAVE.R = PUSHR;
		}
	}
	else if (Module_nb ==  Module_DSPI1) {
		if (MasterSlave == Master_Mode) {
			SPI_1.PUSHR.PUSHR.R = PUSHR;
			//while (SPI_1.SR.B.TCF != 1){}
		}
		else if (MasterSlave == Slave_Mode) {
			SPI_1.PUSHR.PUSHR_SLAVE.R = PUSHR;
		}
	}
	else if (Module_nb ==  Module_DSPI2) {
		if (MasterSlave == Master_Mode) {
			SPI_2.PUSHR.PUSHR.R = PUSHR;
			//while (SPI_2.SR.B.TCF != 1){}
		}
		else if (MasterSlave == Slave_Mode) {
			SPI_2.PUSHR.PUSHR_SLAVE.R = PUSHR;
		}
	}
	else if (Module_nb ==  Module_DSPI3) {
		if (MasterSlave == Master_Mode) {
			SPI_3.PUSHR.PUSHR.R = PUSHR;
			//while (SPI_3.SR.B.TCF != 1){}
		}
		else if (MasterSlave == Slave_Mode) {
			SPI_3.PUSHR.PUSHR_SLAVE.R = PUSHR;
		}
	}
}

/*****************************************************************************/
/* Transmit_DSPI_DeuxTrames                                                              */
/* Description: lancement d'uen transmission par le module DSPI[Module_nb] de deux trames
 * de 16 bits, sans retour à l'état inactif de CS[PCS_nb] entre les 2 transmissions.
 * MasterSlave indique si le module est en mode maitre ou esclave.
 * CTAR_nb indique quelle configuration temporelle définie dans un registre CTAR[CTAR_nb]
 * est utilisée. PCS_nb indique quel signal PCS est utilisé pour le transfert.
 * Message1 et Message2 sont les 2 parties du message de 32 bits à transmettre.
 * En mode slave, les infos CTAR_nb et PCS_nb sont ignorées
 * Le signal PCS est continu selon l'entrée CS_Cont (bit CONT).
 */
/*****************************************************************************/
void Transmit_DSPI_32bits(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,uint32_t Message) {
	uint16_t Message1, Message2;

	//extraction des 2 mots de 16 bits du message. On transmet d'abord le MSB.
	Message2 = (uint16_t)(0x0000FFFF & Message);
	Message1 = (uint16_t)(0x0000FFFF & (Message >> 16));


	Transmit_DSPI(Module_nb,MasterSlave,CTAR_nb,PCS_nb,Message1,CS_Cont_ON,0);
	Transmit_DSPI(Module_nb,MasterSlave,CTAR_nb,PCS_nb,Message2,CS_Cont_OFF,0);
}


/*****************************************************************************/
/* Read_DSPI                                                               */
/* Description: lecture des données reçues par le module DSPI[Module_nb].
 * La fonction renvoie les données stockées dans le registre POPR.
 * ATTENTION : en phase de debug sur puce, toute lecture par l'outil de debug de POPR le videra.
 * Cela peut conduire à un bug de fonctionnement de l'application car, si la lecture de POPR par
 * le programme du MCU est faite après celle de l'outil de debug, POPR renverra 0 au lieu
 * de la valeur reçue sur le bus SPI !!!
 */
/*****************************************************************************/
uint32_t Read_DSPI(uint8_t Module_nb) {
	uint32_t ReceivedData = 0;

	if (Module_nb ==  Module_DSPI0) {
		while (SPI_0.SR.B.RFDF != 1){}  /* Wait for Receive FIFO Drain Flag = 1 */
		ReceivedData = SPI_0.POPR.R;    /* Read data received by slave SPI */
		SPI_0.SR.R = 0x80020000;        /* Clear TCF, RDRF flags by writing 1 to them */
	}
	else if (Module_nb ==  Module_DSPI1) {
		while (SPI_1.SR.B.RFDF != 1){}  /* Wait for Receive FIFO Drain Flag = 1 */
		ReceivedData = SPI_1.POPR.R;    /* Read data received by slave SPI */
		SPI_1.SR.R = 0x80020000;        /* Clear TCF, RDRF flags by writing 1 to them */
	}
	else if (Module_nb ==  Module_DSPI2) {
		while (SPI_2.SR.B.RFDF != 1){}  /* Wait for Receive FIFO Drain Flag = 1 */
		ReceivedData = SPI_2.POPR.R;    /* Read data received by slave SPI */
		SPI_2.SR.R = 0x80020000;        /* Clear TCF, RDRF flags by writing 1 to them */
	}
	else if (Module_nb ==  Module_DSPI3) {
		while (SPI_3.SR.B.RFDF != 1){}  /* Wait for Receive FIFO Drain Flag = 1 */
		ReceivedData = SPI_3.POPR.R;    /* Read data received by slave SPI */
		SPI_3.SR.R = 0x80020000;        /* Clear TCF, RDRF flags by writing 1 to them */
	}

	return(ReceivedData);
}


/*****************************************************************************/
/* Transmit_Read_8bits_DSPI                                                             */
/* Description: la fonction fait transmettre par le maitre une trame 8 bits et récupère une
 * trame sur 16 bits (réponse de l'esclave).
 */
/*****************************************************************************/
uint32_t Transmit_Read_DSPI_8bits(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,
		uint8_t Message, uint8_t CS_Cont,uint32_t EOQ) {
	uint32_t ReadData;

	Transmit_DSPI_8bits(Module_nb,MasterSlave,CTAR_nb,PCS_nb,Message,CS_Cont,EOQ);
	ReadData = Read_DSPI(Module_nb);
	return(ReadData);
}


/*****************************************************************************/
/* Transmit_Read_DSPI                                                             */
/* Description: la fonction fait transmettre par le maitre une trame 16 bits et récupère une
 * trame sur 16 bits (réponse de l'esclave).
 */
/*****************************************************************************/
uint32_t Transmit_Read_DSPI(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,
		uint16_t Message, uint8_t CS_Cont,uint32_t EOQ) {
	uint32_t ReadData;

	Transmit_DSPI(Module_nb,MasterSlave,CTAR_nb,PCS_nb,Message,CS_Cont,EOQ);
	ReadData = Read_DSPI(Module_nb);
	return(ReadData);
}


/*****************************************************************************/
/* Transmit_Read_DSPI_32bits                                                             */
/* Description: la fonction fait transmettre par le maitre une trame 32 bits et récupère une
 * trame sur 32 bits.
 */
/*****************************************************************************/
uint32_t Transmit_Read_DSPI_32bits(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,uint32_t MessageTX) {
	uint16_t MessageTX1, MessageTX2;
	uint16_t MessageRX1, MessageRX2;
	uint32_t MessageRX = 0;

	//extraction des 2 mots de 16 bits du message. On transmet d'abord le MSB.
	MessageTX2 = (uint16_t)(0x0000FFFF & MessageTX); //LSB
	MessageTX1 = (uint16_t)(0x0000FFFF & (MessageTX >> 16)); //MSB

   	Transmit_DSPI(Module_nb,MasterSlave,CTAR_nb,PCS_nb,MessageTX1,CS_Cont_ON,0);
   	Transmit_DSPI(Module_nb,MasterSlave,CTAR_nb,PCS_nb,MessageTX2,CS_Cont_OFF,0);

   	//read received data through SPI
   	MessageRX1 = Read_DSPI(Module_nb);
   	MessageRX2 = Read_DSPI(Module_nb);

	MessageRX = (MessageRX1 << 16) | MessageRX2;

	return(MessageRX);
}


