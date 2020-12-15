/*
 * DSPIConfig.h
 *
 *  Created on: 21 août 2018
 *      Author: admin_aboyer
 *
 *
 *  Configuration et utilisation des modules DSPI (Serial Peripheral Interface)
 */

#ifndef DSPICONFIG_H_
#define DSPICONFIG_H_

#define Module_DSPI0 0
#define Module_DSPI1 1
#define Module_DSPI2 2
#define Module_DSPI3 3

#define Master_Mode 1
#define Slave_Mode 0
#define Inactive_CS_Low 0
#define Inactive_CS_High 1
#define Enable_SPI_clock 0
#define Disable_SPI_clock 1
#define Enable_SPI_TX_FIFO 0
#define Disable_SPI_TX_FIFO 1
#define Enable_SPI_RX_FIFO 0
#define Disable_SPI_RX_FIFO 1
#define Start_DSPI_Transfer 0
#define Stop_DSPI_Transfer 1

#define Normal_DR 0;
#define Double_DR 1;
#define Inactive_SCLK_Low 0
#define Inactive_SCLK_High 1
#define LSB_First 1
#define MSB_First 0

#define CS0_Asserted 1
#define CS1_Asserted 2
#define CS2_Asserted 4
#define CS3_Asserted 8
#define CS4_Asserted 16
#define CS5_Asserted 32
#define CS6_Asserted 64
#define CS7_Asserted 128

#define CS_Cont_OFF 0
#define CS_Cont_ON 1

void Halt_All_DSPI_modules(void);
void init_dspi0_ports(void);
void init_dspi1_ports(void);
void init_dspi2_ports(void);
void init_dspi0(void);
void init_dspi1(void);
void init_dspi2(void);
void SetTiming_dspi0(uint8_t CTAR_nb);
void SetTiming_dspi1(uint8_t CTAR_nb);
void SetTiming_dspi2(uint8_t CTAR_nb);
void DSPI_Enter_Exit_HaltMode(uint8_t Module_nb, uint8_t HaltEn);
void Transmit_DSPI(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb, uint16_t Message, uint8_t CS_Cont,uint32_t EOQ);
uint32_t Read_DSPI(uint8_t Module_nb);
void Transmit_DSPI_8bits(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,
uint8_t Message, uint8_t CS_Cont,uint32_t EOQ);
void Transmit_DSPI_32bits(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,uint32_t Message);
uint32_t Transmit_Read_DSPI(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,uint16_t Message, uint8_t CS_Cont,uint32_t EOQ);
uint32_t Transmit_Read_DSPI_8bits(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,
uint8_t Message, uint8_t CS_Cont,uint32_t EOQ);
uint32_t Transmit_Read_DSPI_32bits(uint8_t Module_nb, uint8_t MasterSlave, uint8_t CTAR_nb, uint8_t PCS_nb,uint32_t MessageTX);


#endif /* DSPICONFIG_H_ */
