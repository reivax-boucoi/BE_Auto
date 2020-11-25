/*
 * Config_eDMA.h
 *
 *  Created on: 17 août 2018
 *      Author: admin_aboyer
 *
 *  Configuration des opérations de DMA (pour le transfert des acquisitions par les ADC)
 */

#ifndef CONFIG_EDMA_H_
#define CONFIG_EDMA_H_

#define CTU0_FIFO0_DMARequest 6 //le FIFO0 de CTU0 est relié à l'entrée source n°6 du DMA_MUX
#define CTU0_FIFO1_DMARequest 7 //le FIFO1 de CTU0 est relié à l'entrée source n°7 du DMA_MUX
#define CTU0_FIFO2_DMARequest 8 //le FIFO2 de CTU0 est relié à l'entrée source n°8 du DMA_MUX
#define CTU0_FIFO3_DMARequest 9 //le FIFO3 de CTU0 est relié à l'entrée source n°9 du DMA_MUX

#define Adr_CTU0_FIFO0_Right 0xFBC10080  //FIFO 0 - FR0 (FIFO Right Aligned Data Register)
#define Adr_CTU0_FIFO1_Right 0xFBC10084  //FIFO 1 - FR1 (FIFO Right Aligned Data Register)
#define Adr_CTU0_FIFO2_Right 0xFBC10088  //FIFO 2 - FR2 (FIFO Right Aligned Data Register)
#define Adr_CTU0_FIFO3_Right 0xFBC1008C  //FIFO 3 - FR3 (FIFO Right Aligned Data Register)
#define Adr_CTU0_FIFO0_Left 0xFBC100A0  //FIFO 0 - FL0 (FIFO Left Aligned Data Register)
#define Adr_CTU0_FIFO1_Left 0xFBC100A4  //FIFO 1 - FL1 (FIFO Left Aligned Data Register)
#define Adr_CTU0_FIFO2_Left 0xFBC100A8  //FIFO 2 - FL2 (FIFO Left Aligned Data Register)
#define Adr_CTU0_FIFO3_Left 0xFBC100AC  //FIFO 3 - FL3 (FIFO Left Aligned Data Register)


void DMA_Init(void);
void Config_DMA_TCD(uint32_t ChNumber,uint32_t AddressSource, uint32_t AddressDestination, uint32_t MessageSize);
void ConfigDMA_MUX(uint8_t RequeteSource, uint8_t DMAChannel);
void init_edma_channel_arbitration(void);
void bridges_config(void);
void crossbar_config_DMA_highest(void);

#endif /* CONFIG_EDMA_H_ */
