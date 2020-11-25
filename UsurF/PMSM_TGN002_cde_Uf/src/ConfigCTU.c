/*
 * ConfigCTU.c
 *
 *  Created on: 16 ao�t 2018
 *      Author: admin_aboyer
 *      Configuration du bloc CTU0
 *
 *  Configuration du CTU0 (synchronisation FlexPWM --> ADC)
 */



#include "MPC5744P.h"
#include "ConfigCTU.h"

/*****************************************************************************/
/* Init_CTU0_GeneralControl                                                            */
/* Description: D�finit les contr�les g�n�raux du CTU0 (activation de la sortie)
 */
void Init_CTU0_GeneralControl(uint8_t OutputEnable) {
	CTU_0.CR.B.CTU_ODIS = OutputEnable; //CTU output enable

}


/*****************************************************************************/
/* Init_CTU0_TriggerGenerator_Trigger                                                            */
/* Description: Configure le trigger generator de CTU0.
 * Dans cette version, il est configur� en mode Trigger. Seule une source est s�lectionn�e.
 * On ne g�n�re qu'un seul trigger en sortie donn� par TriggerOutput.
 * TriggerInput donne le n� de l'entr�e de trigger (0 � 15).
 * RiseTrigger et FallTrigger indiquent si sensibles sur front montant ou descendant
 * ClkPres d�finit la valeur du prescaler pour l'horloge du compteur.
 * InitCptr et MaxCptr d�finissent les valeurs initiales et max du compteur
 * CompareVal d�finit la valeur qui sera compar�e avec celle du compteur. Lorsque leurs valeurs
 * sont �gales, un trigger est �mis en sortie du TriggerGenerator. On peut d�finir ainsi
 * un d�lai entre le trigger d'entr�e et le trigger de sortie.
 */
/*****************************************************************************/
void Init_CTU0_TriggerGenerator_Trigger(uint8_t TriggerInput, uint8_t RiseTrigger, uint8_t FallTrigger,
		uint8_t TriggerOutput,uint8_t ClkPres, uint16_t InitCptr, uint16_t MaxCptr, uint16_t CompareVal) {
	uint32_t TrigCmd;

	CTU_0.TGSCR.B.TGS_M = 0; //mode triggered
	CTU_0.TGSCR.B.PRES = ClkPres;
	TrigCmd = RiseTrigger  | (FallTrigger << 1);
	CTU_0.TGSISR.R = CTU_0.TGSISR.R | (TrigCmd << (2*TriggerInput));
	CTU_0.CR.B.TGSISR_RE = 1; //TGSISR est � double buffer. Le bit TGSISR_RE agit comme reload enable.

	CTU_0.TGSCRR.B.TGSCRV = InitCptr; //valeur initiale du compteur
	CTU_0.TGSCCR.B.TGSCCV = MaxCptr;  //valeur max du compteur. Une fois atteinte, il s'arr�te.
	CTU_0.TCR[TriggerOutput].B.TCRV = CompareVal; //valeur d�finissant le d�clenchement du trigger de sortie.
}

/*****************************************************************************/
/* Init_CTU0_TriggerGenerator_Trigger2                                                            */
/* Description: Configure le trigger generator de CTU0.
 * Dans cette version, il est configur� en mode Trigger. Seule une source est s�lectionn�e.
 * On ne g�n�re que deux trigger en sortie : OutTrig0 et OutTrig1.
 * TriggerInput donne le n� de l'entr�e de trigger (0 � 15).
 * RiseTrigger et FallTrigger indiquent si sensibles sur front montant ou descendant
 * ClkPres d�finit la valeur du prescaler pour l'horloge du compteur.
 * InitCptr et MaxCptr d�finissent les valeurs initiales et max du compteur
 * CompareVal0 et CompareVal1 d�finissent les valeurs qui seront compar�e avec celle du compteur.
 * Lorsque leurs valeurs sont �gales � CompareVal0/1, un trigger est �mis sur la sortie OutTrig0/1.
 * On peut d�finir ainsi  * un d�lai entre le trigger d'entr�e et le trigger de sortie.
 */
/*****************************************************************************/
void Init_CTU0_TriggerGenerator_Trigger2(uint8_t TriggerInput, uint8_t RiseTrigger, uint8_t FallTrigger,
		uint8_t OutTrig0, uint8_t OutTrig1,
		uint8_t ClkPres, uint16_t InitCptr, uint16_t MaxCptr, uint16_t CompareVal0, uint16_t CompareVal1) {
	uint32_t TrigCmd;

	CTU_0.TGSCR.B.TGS_M = 0; //mode triggered
	CTU_0.TGSCR.B.PRES = ClkPres;
	TrigCmd = RiseTrigger  | (FallTrigger << 1);
	CTU_0.TGSISR.R = CTU_0.TGSISR.R | (TrigCmd << (2*TriggerInput));
	CTU_0.CR.B.TGSISR_RE = 1; //TGSISR est � double buffer. Le bit TGSISR_RE agit comme reload enable.

	CTU_0.TGSCRR.B.TGSCRV = InitCptr; //valeur initiale du compteur
	CTU_0.TGSCCR.B.TGSCCV = MaxCptr;  //valeur max du compteur. Une fois atteinte, il s'arr�te.
	CTU_0.TCR[OutTrig0].B.TCRV = CompareVal0; //valeur d�finissant le d�clenchement du trigger de sortie OutTrig0.
	CTU_0.TCR[OutTrig1].B.TCRV = CompareVal1; //valeur d�finissant le d�clenchement du trigger de sortie OutTrig1.

}

/*****************************************************************************/
/* Init_CTU0_TriggerGenerator_Sequential                                                           */
/* Description: Configure le trigger generator de CTU0.
 * Dans cette version, il est configur� en mode Sequential pour 3 �v�nements d'entr�e.
 * Une seule source d�clenche le MRS,
 * les autres d�clenchent les aautres Event signal qui g�n�reront les triggers de sortie..
 * On g�n�re plusieurs triggers en sortie en fonction de ce qui est compl�t�. Cependant, on aura le
 * m�me d�lai entre chaque event signal et le trigger de sortie correspondant.
 * TriggerInput0,1,2 donne les n� des 3 entr�es de trigger qui vont d�clencher (0 � 15), TriggerInput0 est
 * celle qui va d�clencher le MRS.
 * les sorties correspondantes du bloc trigger generator subunit sont donn�es par TriggerOutput0/1/2.
 * RiseTrigger et FallTrigger indiquent si sensibles sur front montant ou descendant. Les 3 entr�es ont les m�mes carac.
 * ClkPres d�finit la valeur du prescaler pour l'horloge du compteur.
 * InitCptr et MaxCptr d�finissent les valeurs initiales et max du compteur
 * CompareVal d�finit la valeur commune pour chaque Event signal --> trigger de sortie,
 *  qui sera compar�e avec celle du compteur. Lorsque leurs valeurs
 * sont �gales, un trigger est �mis en sortie du TriggerGenerator. On peut d�finir ainsi
 * un d�lai entre un event signal d'entr�e et le trigger de sortie correspondant.
 */
/*****************************************************************************/
void Init_CTU0_TriggerGenerator_Sequential(uint8_t TriggerInput0, uint8_t TriggerInput1, uint8_t TriggerInput2,
		uint8_t TriggerOutput0, uint8_t TriggerOutput1, uint8_t TriggerOutput2,uint8_t RiseTrigger, uint8_t FallTrigger,
		uint8_t ClkPres, uint16_t InitCptr, uint16_t MaxCptr, uint16_t CompareVal) {
	uint32_t TrigCmd;

	CTU_0.TGSCR.B.TGS_M = 1; //mode sequentiel
	//debug :
	//CTU_0.TGSCR.B.ET_TM = 1; //mode toggle

	CTU_0.TGSCR.B.PRES = ClkPres;
	CTU_0.TGSCR.B.MRS_SM = TriggerInput0; //s�lection de l'entr�e qui d�clenche le MRS (TriggerInput0)
	TrigCmd = RiseTrigger  | (FallTrigger << 1);
	CTU_0.TGSISR.R = CTU_0.TGSISR.R | (TrigCmd << (2*TriggerInput0)) | (TrigCmd << (2*TriggerInput1)) | (TrigCmd << (2*TriggerInput2));
	CTU_0.CR.B.TGSISR_RE = 1; //TGSISR est � double buffer. Le bit TGSISR_RE agit comme reload enable.


	//compl�ter les d�lais entre chacune des 3 entr�es de trigger (event signals) et les 3 sorties
	//on met le m�me d�lai !
	CTU_0.TGSCRR.B.TGSCRV = InitCptr; //valeur initiale du compteur
	CTU_0.TGSCCR.B.TGSCCV = MaxCptr;  //valeur max du compteur. Une fois atteinte, il s'arr�te.
	//ES0 --> Trigger output 0
	CTU_0.TCR[TriggerOutput0].B.TCRV = CompareVal; //valeur d�finissant le d�clenchement du trigger de sortie.
	//ES1 --> Trigger output 1
	CTU_0.TCR[TriggerOutput1].B.TCRV = CompareVal; //valeur d�finissant le d�clenchement du trigger de sortie.
	//ES2 --> Trigger output 2
	CTU_0.TCR[TriggerOutput2].B.TCRV = CompareVal; //valeur d�finissant le d�clenchement du trigger de sortie.
}

/*****************************************************************************/
/* Init_CTU0_TriggerScheduler                                                */
/* Description: Configure une des 8 voies du trigger scheduler subunit.      */
/* LE n� de l'entr�e est donn� par TrigIn. En_Out autorise les sorties de    */
/* trigger correspondantes. ADC_En autorise la sortie de commande de l'ADC.  */
/* ExtTrig_Out autorise la sortie Trigger externe. Les autres sorties (ETimer)*/
/* sont omises par souci de simplification de la fonction.                   */
/*****************************************************************************/
void Init_CTU0_TriggerScheduler(uint8_t TrigIn, uint8_t En_Out, uint8_t ADC_En, uint8_t ExtTrigOut_En) {
	if (TrigIn == 0) {
		CTU_0.THCR1.B.T0_E = En_Out;
		CTU_0.THCR1.B.T0_ADCE = ADC_En;
		CTU_0.THCR1.B.T0_ETE = ExtTrigOut_En;
	}
	if (TrigIn == 1) {
		CTU_0.THCR1.B.T1_E = En_Out;
		CTU_0.THCR1.B.T1_ADCE = ADC_En;
		CTU_0.THCR1.B.T1_ETE = ExtTrigOut_En;
	}
	if (TrigIn == 2) {
		CTU_0.THCR1.B.T2_E = En_Out;
		CTU_0.THCR1.B.T2_ADCE = ADC_En;
		CTU_0.THCR1.B.T2_ETE = ExtTrigOut_En;
	}
	if (TrigIn == 3) {
		CTU_0.THCR1.B.T3_E = En_Out;
		CTU_0.THCR1.B.T3_ADCE = ADC_En;
		CTU_0.THCR1.B.T3_ETE = ExtTrigOut_En;
	}
	if (TrigIn == 4) {
		CTU_0.THCR2.B.T4_E = En_Out;
		CTU_0.THCR2.B.T4_ADCE = ADC_En;
		CTU_0.THCR2.B.T4_ETE = ExtTrigOut_En;
	}
	if (TrigIn == 5) {
		CTU_0.THCR2.B.T5_E = En_Out;
		CTU_0.THCR2.B.T5_ADCE = ADC_En;
		CTU_0.THCR2.B.T5_ETE = ExtTrigOut_En;
	}
	if (TrigIn == 6) {
		CTU_0.THCR2.B.T6_E = En_Out;
		CTU_0.THCR2.B.T6_ADCE = ADC_En;
		CTU_0.THCR2.B.T6_ETE = ExtTrigOut_En;
	}
	if (TrigIn == 7) {
		CTU_0.THCR2.B.T7_E = En_Out;
		CTU_0.THCR2.B.T7_ADCE = ADC_En;
		CTU_0.THCR2.B.T7_ETE = ExtTrigOut_En;
	}
}

//autorisation d'une ex�cution en mode streaming ou parall�le des listes d'ADC, selon la variable Mode
void CTU0_Stream_or_Parallel_ADC_List(uint8_t Mode) {
	if (Mode == MODE_ADC_STREAM) {
		CTU_0.LISTCSR.B.PAR_LIST = MODE_ADC_STREAM;
	}
	else if (Mode == MODE_ADC_PARALLEL) {
		CTU_0.LISTCSR.B.PAR_LIST = MODE_ADC_PARALLEL;
	}

}

/*****************************************************************************/
/* Init_CTU0_TriggerScheduler_ADC_SingleConv                                 */
/* Description: Configure la liste de commande ADC, qu'on consid�re en mode
 * single conversion. On ne consid�re qu'une seule commande --> une  seule conversion.
 * Trig_Index : indique quelle est la source de Trigger qui a lanc� la commande de l'ADC
 * ADCPort : si 0 --> ADC port A (ADC0). Si 1 --> ADC port B (ADC1)
 * ADC_ch : num�ro du canal de l'ADC
 * List_Cmd_Adr : indique l'adresse de la premi�re commande dans la liste de commande
 * FIFO_dest : n� du FIFO de destination (0 � 7).
 * On ne sert pas du DMA ou des interruptions sur le FIFO --> pas de config du DMA et
 * du seuil de d�bordement du FIFO.
 */
/*****************************************************************************/
void Init_CTU0_TriggerScheduler_ADC_SingleConv(uint8_t Trig_Index, uint8_t ADCPort, uint8_t ADC_ch,
		uint8_t List_Cmd_Adr, uint8_t FIFO_dest) {

	//configuration du registre de controle de liste de commande d'ADC : on indique l'adresse de la prem�re commande
	//d'une s�quence de commande.
	if (Trig_Index == 0) {
		CTU_0.CLCR1.B.T0_INDEX = List_Cmd_Adr;
	}
	if (Trig_Index == 1) {
		CTU_0.CLCR1.B.T1_INDEX = List_Cmd_Adr;
	}
	if (Trig_Index == 2) {
		CTU_0.CLCR1.B.T2_INDEX = List_Cmd_Adr;
	}
	if (Trig_Index == 3) {
		CTU_0.CLCR1.B.T3_INDEX = List_Cmd_Adr;
	}
	if (Trig_Index == 4) {
		CTU_0.CLCR2.B.T4_INDEX = List_Cmd_Adr;
	}
	if (Trig_Index == 5) {
		CTU_0.CLCR2.B.T5_INDEX = List_Cmd_Adr;
	}
	if (Trig_Index == 6) {
		CTU_0.CLCR2.B.T6_INDEX = List_Cmd_Adr;
	}
	if (Trig_Index == 7) {
		CTU_0.CLCR2.B.T7_INDEX = List_Cmd_Adr;
	}

	//configuration des commandes d'ici : ici, il n'y a qu'une seule commande
	CTU_0.CLR[List_Cmd_Adr].A.B.SU = ADCPort;
	CTU_0.CLR[List_Cmd_Adr].A.B.CH = ADC_ch;
	CTU_0.CLR[List_Cmd_Adr].A.B.CMS = 0; //doit �tre = 0 en mode single conversion
	CTU_0.CLR[List_Cmd_Adr].A.B.ST0 = 0; //doit �tre = 0 en mode single conversion
	CTU_0.CLR[List_Cmd_Adr].A.B.LC = 0;
	CTU_0.CLR[List_Cmd_Adr].A.B.FIFO = FIFO_dest;

	//une seule commande � effectuer donc la prochaine commande est la derni�re commande de la liste
	//donc bit LC = 1
	CTU_0.CLR[List_Cmd_Adr+1].A.R = 0x4000;
}


/*****************************************************************************/
/* Init_CTU0_TriggerScheduler_ADC_DualConv                                                            */
/* Description: Configure le trigger scheduler pour que la destination du trigger soit l'ADC
 * On ne consid�re ici qu'un seul trigger re�u sur l'entr�e Trigger0.
 * ADC command list : on consid�re seulement un mode dual conversion,
 * ADC_ChA --> port A = ADC0, on indique la voie analogique � convertir
 * ADC_ChB --> port B = ADC1, on indique la voie analogique � convertir
 * FIFO_dest : n� du FIFO de destination (0 � 7).
 * On ne sert pas du DMA ou des interruptions sur le FIFO --> pas de config du DMA et
 * du seuil de d�bordement du FIFO.
 */
/*****************************************************************************/
void Init_CTU0_TriggerScheduler_ADC_DualConv(uint8_t ADC_chA, uint8_t ADC_chB, uint8_t FIFO_dest) {
	CTU_0.THCR1.B.T0_ADCE = 1; //enable trigger0 ADC command
	CTU_0.THCR1.B.T0_E = 1; //enable trigger0

	//d�finition de la liste de commande associ�e � trigger 0
	//comme il n'y a qu'une seule commande, on utilise la commande d'adresse 0
	CTU_0.CLCR1.B.T0_INDEX = 0;
	//CTU_0.CLR[0].A.B.SU = ADCPort;
	CTU_0.CLR[0].B.B.CH_A = ADC_chA;
	CTU_0.CLR[0].B.B.CH_B = ADC_chB;
	CTU_0.CLR[0].B.B.CMS = 1; //doit �tre = 1 en mode dual conversion
	CTU_0.CLR[0].B.B.ST0 = 0; //doit �tre = 0
	CTU_0.CLR[0].B.B.LC = 0;
	CTU_0.CLR[0].B.B.FIFO = FIFO_dest;

	//une seule commande � effectuer donc la prochaine commande est la derni�re commande de la liste
	//donc bit LC = 1 et bit CMS aussi pour indiquer dual conversion mode.
	CTU_0.CLR[1].B.R = 0x6000;
}

/*****************************************************************************/
/* CTU_0_ConfigDMA_ADC                                                          */
/* Description: Configure les acc�s par DMA aux r�sultats de conversion d'un ADC
 * d�clench� par le CTU0.
 * Fifo_Nb : indique quel FIFO contient les r�sultats de conversion. Thres indique
 * � partir de quel seuil de remplissage du FIFO on d�clenche le transfert des donn�es
 * par DMA.
 */
/*****************************************************************************/
void CTU0_ConfigDMA_ADC(uint8_t Fifo_Nb, uint8_t Thres) {

	//configuration du seuil d'overflow pour les FIFO et autorisation des requ�tes DMA
	if (Fifo_Nb == 0) {
		CTU_0.FTH.B.TH0 = Thres;
		CTU_0.FDCR.B.DE0 = 1;
	}
	if (Fifo_Nb == 1) {
		CTU_0.FTH.B.TH1 = Thres;
		CTU_0.FDCR.B.DE1 = 1;
	}
	if (Fifo_Nb == 2) {
		CTU_0.FTH.B.TH2 = Thres;
		CTU_0.FDCR.B.DE2 = 1;
	}
	if (Fifo_Nb == 3) {
		CTU_0.FTH.B.TH3 = Thres;
		CTU_0.FDCR.B.DE3 = 1;
	}
}

/*****************************************************************************/
/* CTU0_General_Reload_Enable                                                          */
/* Description: Active le rechargement g�n�ral des registres de contr�le de CTU0
 */
/*****************************************************************************/
void CTU0_General_Reload_Enable(void) {
	//General Reload enable : GRE doit �tre mis � 1 pour indiquer que tous les registrers sont pr�ts � �tre mis
	//� jour au prochain MRS.
	CTU_0.CR.B.GRE = 1;
}


void CTU0_General_Reload_Disable(void) {
	CTU_0.CR.B.GRE = 0;
}
