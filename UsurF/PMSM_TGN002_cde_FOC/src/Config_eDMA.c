/*
 * Config_eDMA.c
 *
 *  Created on: 17 août 2018
 *      Author: admin_aboyer
 *
 *  Configuration des opérations de DMA (pour le transfert des acquisitions par les ADC)
 */

#include "MPC5744P.h"
#include "Config_eDMA.h"

/*****************************************************************************/
/* DMA_Init                                                    */
/* Description: on garde les propriétés par défaut du DMA_engine.
 * On autorise les requêtes arrivant sur le canal DMA0.
 * On active aucune interruption liée à une erreur du DMA.
 */
/*****************************************************************************/
void DMA_Init(void) {
	DMA_0.ERQ.B.ERQ0 = 1; ///Enable eDMA request 0
	DMA_0.ERQ.B.ERQ1 = 1; ///Enable eDMA request 0
	DMA_0.EEI.R = 0x0000; /* Error Interrupt disabled for all channels */
}

/*****************************************************************************/
/* Config_DMA_TCD                                                      */
/* Description: fonction de configuration d'un canal DMA (registres TCD[ChNumber])
 * AddressSource et AddressDestination sont les adresses source et destination du message
 * à transférer par DMA. MessageSize est la taille du message en mot de 32 bits.
 */
/*****************************************************************************/
void Config_DMA_TCD(uint32_t ChNumber,uint32_t AddressSource, uint32_t AddressDestination,
		uint32_t MessageSize) {

	uint32_t NbOctets;  //nb d'octets à transférer

	NbOctets = 4*MessageSize;

	//paramètres des données sources
	DMA_0.TCD[ChNumber].SADDR.R = AddressSource;  //adresse du premier octet de la source
	DMA_0.TCD[ChNumber].ATTR.B.SMOD = 0; //pas de lecture circulaire des données
	DMA_0.TCD[ChNumber].ATTR.B.SSIZE = 0x2; //les données sont lues par paquet de 32 bits
	DMA_0.TCD[ChNumber].SOFF.R = 4; //0; // après chaque lecture d'une donnée, l'adresse de la source
	//augmente de 4 octets pour passer au prochain mot de 32 bits en mémoire

	//paramètres des données destination
	DMA_0.TCD[ChNumber].DADDR.R = AddressDestination;
	DMA_0.TCD[ChNumber].ATTR.B.DMOD = 0;
	DMA_0.TCD[ChNumber].ATTR.B.DSIZE = 0x2; /* 32-bit */
	DMA_0.TCD[ChNumber].DOFF.R = 4; //0;

	//minor loop configuration :
	//minor loop count (given in bytes)
	DMA_0.TCD[ChNumber].NBYTES.MLNO.R = NbOctets;
	DMA_0.TCD[ChNumber].SLAST.R = -NbOctets; //0; //-MessageSize; --> l'adresse source revient à sa valeur initiale à l'issue
	//de la dernière minor loop
	DMA_0.TCD[ChNumber].DLASTSGA.R = -NbOctets; //0; //-MessageSize; --> l'adresse destination revient à sa valeur initiale à l'issue
	//de la dernière minor loop
	//Le nombre de minor loop par major loop est donné par la valeur initiale du compteur CITER (Current
	//Iteration Counter), qui se décrémente à l'issue de chaque minor loop
	//(no channel linking -->ELINKNO is used)
	DMA_0.TCD[ChNumber].CITER.ELINKNO.B.ELINK = 0; //no channel linking after completion of one minor loop
	DMA_0.TCD[ChNumber].CITER.ELINKNO.B.CITER = 1; //1 iteration --> the major loop contains only one minor loop
	//BITER parameters must be identical to CITER parameters:
	DMA_0.TCD[ChNumber].BITER.ELINKNO.B.ELINK = 0;
	DMA_0.TCD[ChNumber].BITER.ELINKNO.B.BITER = 1;

	//major loop configuration
	DMA_0.TCD[ChNumber].CSR.B.MAJORELINK = 0; /* Disable channel linking */

	//TCD control and status
	DMA_0.TCD[ChNumber].CSR.B.BWC = 0; //no eDMA engine stall
	DMA_0.TCD[ChNumber].CSR.B.ESG = 0;  //disable scatter/gather process
	DMA_0.TCD[ChNumber].CSR.B.INTHALF = 0; //half-point interrupt is disabled.
	DMA_0.TCD[ChNumber].CSR.B.INTMAJOR = 1; //end-of-major loop interrupt is enabled.
	DMA_0.TCD[ChNumber].CSR.B.DREQ = 1; //The channel’s ERQ bit is reset when the major loop is completed.
		//Ainsi, lors du traitement de l'interruption liée à la fin de la major loop, on garantit qu'il n'y aura
		//pas de transfert DMA qui pourra écraser les données en cours de lecture. Il faudra réactiver les requêtes
		//à l'issue du traitement de l'interruption.
}

/*****************************************************************************/
/* ConfigDMA_MUX                                                      */
/* Description: configure le multiplexeur du DMA, pour relier une requête venant
 * d'un périphérique (RequeteSource) vers un canal de DMA (DMAChannel).
 */
/*****************************************************************************/
void ConfigDMA_MUX(uint8_t RequeteSource, uint8_t DMAChannel) {
	DMAMUX_0.CHCFG[DMAChannel].R = 0; //clear previous config.
	DMAMUX_0.CHCFG[DMAChannel].B.SOURCE = RequeteSource; //select MUX source for DMAChannel.
	DMAMUX_0.CHCFG[DMAChannel].B.TRIG = 0; //no periodical trigger
	DMAMUX_0.CHCFG[DMAChannel].B.ENBL = 1; //DMA channel enable
}

/*****************************************************************************/
/* init_edma_channel_arbitration                                                      */
/* Description: configure les priorités des différents canaux de DMA : les valeurs
 * par défaut sont réécrites dans les registres DCHPRI[x]. Aucune préemption ou suspension
 * autorisée. Ce sont aussi les valeurs par défaut.
 */
/*****************************************************************************/
void init_edma_channel_arbitration(void) { /* Use default fixed arbitration */

  DMA_0.CR.R = 0x0000E400; /* Fixed priority arbitration for groups, channels */

  DMA_0.DCHPRI[0].R  = 0x00; /* Grp 0 chan 00, no suspension, no premption */
  DMA_0.DCHPRI[1].R  = 0x01; /* Grp 0 chan 01, no suspension, no premption */
  DMA_0.DCHPRI[2].R  = 0x02; /* Grp 0 chan 02, no suspension, no premption */
  DMA_0.DCHPRI[3].R  = 0x03; /* Grp 0 chan 03, no suspension, no premption */
  DMA_0.DCHPRI[4].R  = 0x04; /* Grp 0 chan 04, no suspension, no premption */
  DMA_0.DCHPRI[5].R  = 0x05; /* Grp 0 chan 05, no suspension, no premption */
  DMA_0.DCHPRI[6].R  = 0x06; /* Grp 0 chan 06, no suspension, no premption */
  DMA_0.DCHPRI[7].R  = 0x07; /* Grp 0 chan 07, no suspension, no premption */
  DMA_0.DCHPRI[8].R  = 0x08; /* Grp 0 chan 08, no suspension, no premption */
  DMA_0.DCHPRI[9].R  = 0x09; /* Grp 0 chan 09, no suspension, no premption */
  DMA_0.DCHPRI[10].R = 0x0A; /* Grp 0 chan 10, no suspension, no premption */
  DMA_0.DCHPRI[11].R = 0x0B; /* Grp 0 chan 11, no suspension, no premption */
  DMA_0.DCHPRI[12].R = 0x0C; /* Grp 0 chan 12, no suspension, no premption */
  DMA_0.DCHPRI[13].R = 0x0D; /* Grp 0 chan 13, no suspension, no premption */
  DMA_0.DCHPRI[14].R = 0x0E; /* Grp 0 chan 14, no suspension, no premption */
  DMA_0.DCHPRI[15].R = 0x0F; /* Grp 0 chan 15, no suspension, no premption */

  DMA_0.DCHPRI[16].R = 0x10; /* Grp 1 chan 00, no suspension, no premption */
  DMA_0.DCHPRI[17].R = 0x11; /* Grp 1 chan 01, no suspension, no premption */
  DMA_0.DCHPRI[18].R = 0x12; /* Grp 1 chan 02, no suspension, no premption */
  DMA_0.DCHPRI[19].R = 0x13; /* Grp 1 chan 03, no suspension, no premption */
  DMA_0.DCHPRI[20].R = 0x14; /* Grp 1 chan 04, no suspension, no premption */
  DMA_0.DCHPRI[21].R = 0x15; /* Grp 1 chan 05, no suspension, no premption */
  DMA_0.DCHPRI[22].R = 0x16; /* Grp 1 chan 06, no suspension, no premption */
  DMA_0.DCHPRI[23].R = 0x17; /* Grp 1 chan 07, no suspension, no premption */
  DMA_0.DCHPRI[24].R = 0x18; /* Grp 1 chan 08, no suspension, no premption */
  DMA_0.DCHPRI[25].R = 0x19; /* Grp 1 chan 09, no suspension, no premption */
  DMA_0.DCHPRI[26].R = 0x1A; /* Grp 1 chan 10, no suspension, no premption */
  DMA_0.DCHPRI[27].R = 0x1B; /* Grp 1 chan 11, no suspension, no premption */
  DMA_0.DCHPRI[28].R = 0x1C; /* Grp 1 chan 12, no suspension, no premption */
  DMA_0.DCHPRI[29].R = 0x1D; /* Grp 1 chan 13, no suspension, no premption */
  DMA_0.DCHPRI[30].R = 0x1E; /* Grp 1 chan 14, no suspension, no premption */
  DMA_0.DCHPRI[31].R = 0x1F; /* Grp 1 chan 15, no suspension, no premption */
}

/*****************************************************************************/
/* bridges_config                                                            */
/* Description: Configures bridges to provide desired RW and user/supervisor */
/*              access and priorites by crossbar masters to crossbar slaves. */
/*****************************************************************************/
void bridges_config (void) {
  AIPS_0.MPRA.R |= 0x77777770;       /* All masters have RW & user level access */
  AIPS_1.MPRA.R |= 0x77777770;       /* All masters have RW & user level access */
}

/*****************************************************************************/
/* crossbar_config_DMA_highest                                               */
/* Description: Configures desired crossbar priroities for crossbar slaves   */
/*              to give to crossbar masters.                                 */
/*****************************************************************************/
void crossbar_config_DMA_highest (void) {
  XBAR_0.PORT[4].PRS.R = 0x03000021; /* PBridge 0:    gives highest priority to DMA */
  XBAR_0.PORT[5].PRS.R = 0x03000021; /* PBridge 1:    gives highest priority to DMA */
  XBAR_0.PORT[0].PRS.R = 0x03000021; /* PFLASH CTRL:  gives highest priority to DMA */
}

