/*
 * UARTConfig.c
 *
 *  Created on: 22 août 2018
 *      Author: admin_aboyer
 *
 *  Initialisation de l'UART, transmission et réception.
 *  Dans ce BE, l'UART est utilisé pour les besoins de Freemaster.
 */

#include "MPC5744P.h"
#include "UARTConfig.h"
#include "ConfigGPIO.h"
#include "ClockConfig.h"



/*****************************************************************************/
/* Config_LINFlex                                                        */
/* Description: Configure le module LINFlex1, utilisé en UART.
/*****************************************************************************/
void Config_LINFlex1_UART(void) {
	/* Initialisation UART */
	InitPortLinFlex1();  //Initialisation des pads d'IO liées à LINFLEX1
	InitUARTLinFlex1();  //initialisation du LINFLex1 (mode UART, longueur mot, parité)
	SetBaudRateLinFlex(LINFlex1,LIN_CLK,UART_BAUD_RATE);
	/* Exit Initialization Mode */
	EnterExitInitModeLinFlex(LINFlex1,LINNormalMode);
}


/*****************************************************************************/
/* InitPortLinFlex1                                                                 */
/* Description: initialisation des pads d'IO associées à LINFLEX1
/*****************************************************************************/
void InitPortLinFlex1(void) {
	//pin TX
	SIUL2.MSCR[PD9].B.SSS = 2;
	SIUL2.MSCR[PD9].B.OBE = 1;
	SIUL2.MSCR[PD9].B.SRC = 3;  //Full drive-strength without slew rate control

	//pin RX
	SIUL2.MSCR[PD12].B.IBE = 1;
	SIUL2.IMCR[166].B.SSS = 2;
}

/*****************************************************************************/
/* InitUARTLinFlex1                                                                 */
/* Description: initialisation des paramètres l'UART associées à LINFLEX1
/*****************************************************************************/
void InitUARTLinFlex1(void) {
	LINFlexD_1.LINCR1.B.INIT = 1;     /* Enter Initialization Mode */
	LINFlexD_1.LINCR1.B.SLEEP = 0;    /* Exit Sleep Mode */

	/* Configuration for UART mode */
	LINFlexD_1.UARTCR.B.UART = 1;     /* UART Enable- Required before UART config.*/
	//word length = 8 bits (+1 parity bit if PCE = 1)
	LINFlexD_1.UARTCR.B.WL0 = 1;
	LINFlexD_1.UARTCR.B.WL1 = 0;
	//no parity check
	LINFlexD_1.UARTCR.B.PCE = 0;
	LINFlexD_1.UARTCR.B.PC0 = 0; //PC0 = PC1 = 0 --> parity sent is even
	LINFlexD_1.UARTCR.B.PC1 = 0;
	//enable transmitter and receiver
	LINFlexD_1.UARTCR.B.TxEn = 1;
	LINFlexD_1.UARTCR.B.RxEn = 1;
	//Buffer mode in reception and transmission
	LINFlexD_1.UARTCR.B.RFBM = 0;
	LINFlexD_1.UARTCR.B.TFBM = 0;
	//Nombre d'octets à transmettre en mode buffer
	LINFlexD_1.UARTCR.B.TDFL_TFC = 0; //1 octet
	//Nombre d'octets à recevoir en mode buffer
	LINFlexD_1.UARTCR.B.RDFL_RFC = 0; //1 octet

	//utile ?
	LINFlexD_1.UARTSR.B.SZF = 1;      /* Clear the Zero status bit */
	LINFlexD_1.UARTSR.B.DRFRFE = 1;   /* Clear DRFRFE flag */
}

/*****************************************************************************/
/* SetBaudRateLinFlex                                                                */
/* Description: Configuration du baud rate du module LINFLEX[Module_nb]
 * LIN_clk donne la fréquence LIN_clk en Hz (= à HALFSYS_clk).
 * BaudRate donne le baud rate attendu (en bauds).
 */
/*****************************************************************************/
void SetBaudRateLinFlex(uint8_t Module_nb, uint32_t LIN_clk, uint32_t BaudRate) {
	uint32_t LDIV; //rapport entre LIN_clk et le baud rate
	uint32_t LDIV_integer; //partie entière du facteur de division de LIN_clk (LDIV)
	uint32_t LDIV_fraction; //partie fractionnaire du facteur de division de LIN_clk (LDIV)

	//exemple: LIN_clk = 60 MHz et Baud rate = 9600 Bds
	//LDIV = 60M/(16*9600) = 390.625
	//Partie entière = 390 --> LINIBRR = 390
	//Partie fractionnaire = 0.625*16 = 10 --> LINFBRR = 10

	LDIV = LIN_clk/BaudRate;
	LDIV_integer = LDIV/16;
	LDIV_fraction = LDIV-16*LDIV_integer;

	if (Module_nb == LINFlex0) {
		LINFlexD_0.LINIBRR.R = LDIV_integer;
		LINFlexD_0.LINFBRR.R = LDIV_fraction;
	}
	else if (Module_nb == LINFlex1) {
		LINFlexD_1.LINIBRR.R = LDIV_integer;
		LINFlexD_1.LINFBRR.R = LDIV_fraction;
	}
}

/*****************************************************************************/
/* EnterExitInitModeLinFlex                                                                */
/* Description: Entrée ou sortie du mode initialisation du module LINFLEX[Module_nb]
 * En_InitMode indique si on autorise l'entrée en mode initialisation (pour la configuration)
 * ou la sortie de ce mode (donc entrée en mode normal).
 */
/*****************************************************************************/
void EnterExitInitModeLinFlex(uint8_t Module_nb, uint32_t En_InitMode) {
	//attention, l'entrée en mode normal se fait non seulement si le bit INIT est mis à '0', mais
	//aussi si le module est sorti du mode SLEEP (bit SLEEP à 0).

	if (Module_nb == LINFlex0) {
		LINFlexD_0.LINCR1.B.INIT = En_InitMode;
	}
	else if (Module_nb == LINFlex1) {
		LINFlexD_1.LINCR1.B.INIT = En_InitMode;
	}
}


/*****************************************************************************/
/* TransmitData                                                               */
/* Description: Transmission du message pointé par pBuf (pointeur sur char).
 * La longueur du message est indiquée par MessageSize (longueur en nombre de caractères).
 * Le module qui doit transmettre est indiqué par Module_nb.
 * Comme on transmet des char, on transmet des mots de 8 bits --> dans l'initialisation, configurer
 * la longueur des mots à 8 bits. Le caractère à transmettre est placé dans le registre BDR0.
 */
/*****************************************************************************/
void TransmitData(uint8_t Module_nb, const char *pBuf, uint32_t MessageSize) {
	uint8_t	j = 0;

    for (j=0; j< MessageSize; j++) {  // Loop for character string
    	if (Module_nb == LINFlex0) {
    		//write character to transmit buffer
    		LINFlexD_0.BDRL.B.DATA0 = *(pBuf+j);
    		// Wait for data transmission completed flag
    	    while (1 != LINFlexD_0.UARTSR.B.DTFTFF) {}
    	    // clear the DTF flag and not the other flags
    	    LINFlexD_0.UARTSR.R = 0x0002;
    	}
    	else if (Module_nb == LINFlex1) {
    		//write character to transmit buffer
    		LINFlexD_1.BDRL.B.DATA0 = *(pBuf+j);
    		// Wait for data transmission completed flag
    	    while (1 != LINFlexD_1.UARTSR.B.DTFTFF) {}
    	    // clear the DTF flag and not the other flags
    	    LINFlexD_1.UARTSR.R = 0x0002;
    	}
    }
}

/*****************************************************************************/
/* receiveData                                                               */
/* Description: Réception d'un message par le module LINFlex[Module_nb].
 * Le message reçu est pointé par (pointeur sur char).
 */
/*****************************************************************************/
void ReceiveData(uint8_t Module_nb, char *pBuf) {
    int32_t rx_data;

	if (Module_nb == LINFlex0) {
    	/* wait for DRF */
		while (1 != LINFlexD_0.UARTSR.B.DRFRFE) {}  /* Wait for data reception completed flag */
		/* wait for RMB */
		while (1 != LINFlexD_0.UARTSR.B.RMB) {}  /* Wait for Release Message Buffer */
		/* get the data */
		rx_data = LINFlexD_0.BDRM.R; // read whole register due to erratum e4897PS
		*pBuf = (uint8_t)rx_data;
		/* clear the DRF and RMB flags by writing 1 to them */
		LINFlexD_0.UARTSR.R = 0x0204;
	}
	else if (Module_nb == LINFlex1) {
    	/* wait for DRF */
		while (1 != LINFlexD_1.UARTSR.B.DRFRFE) {}  /* Wait for data reception completed flag */
		/* wait for RMB */
		while (1 != LINFlexD_1.UARTSR.B.RMB) {}  /* Wait for Release Message Buffer */
		/* get the data */
		rx_data = LINFlexD_1.BDRM.R; // read whole register due to erratum e4897PS
		*pBuf = (uint8_t)rx_data;
		/* clear the DRF and RMB flags by writing 1 to them */
		LINFlexD_1.UARTSR.R = 0x0204;
	}
}

/*****************************************************************************/
/* HexToASCII_Word                                                               */
/* Description: Convertit un entier non signé 32 bits (input) en un symbole HEX
 * du type '0x12341BCD'. Pour cela, l'entier est convertit en un tableau de 8
 * caractères ASCII.
 */
/*****************************************************************************/
void HexToASCII_Word(unsigned int input,uint8_t* buffer) {
	uint8_t i = 0;
	unsigned int number = input;
	unsigned int temp = 0;

	/* Table of ASCII characters */
	/*
	 * Number_decimal	Number_HEX	Character
	048     30   0
	049     31   1
	050     32   2
	051		33	 3
	052     34   4
	053     35   5
	054     36   6
	055     37   7
	056     38   8
	057     39   9
	058     3A   :
	059     3B   ;
	060     3C   <
	061     3D   =
	062     3E   >
	063     3F   ?
	064     40   @
	065     41   A
	066     42   B
	067     43   C
	068     44   D
	069     45   E
	070     46   F
	         */

	for(i=0;i<8;i++){
		temp = number & 0x0000000F; //Mask least significant nibble of word
		/* Convert to ASCII. Store in buffer backwards because LSB gets stored first */
		if(temp > 9) {
			*(buffer+8-(i+1)) = temp + '0' + 7; //'A' does not come immediately after '9' in ASCII table. Additional offset
		}
		else {
			*(buffer+8-(i+1)) = temp + '0'; //Otherwise store '0' to '9'
		}

		number = number >> 4; //Get next nibble
	}
}

