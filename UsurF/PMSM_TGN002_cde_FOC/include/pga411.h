/*
 * pga411.h
 *
 *  Created on: 11 sept. 2018
 *      Author: admin_aboyer
 *
 *  Contient les fonctions permettant de communiquer avec le PGA411, afin de le
 *  configurer, le diagnostiquer par SPI, récupérer l'angle et la vitesse du rotor.
 */

#include "SWLIBS_Typedefs.h"

#ifndef PGA411_H_
#define PGA411_H_

enum {DIAG, NORMAL}; /* Operational states of PGA */
enum {READ, WRITE}; /* Enmus selects SPI operation */

/* PGA411 registers and their offsets */
#define PGA411_REG_COUNT 27
#define DEV_OVUV1 0x00
#define DEV_OVUV2 0x01
#define DEV_OVUV3 0x02
#define DEV_OVUV4 0x03
#define DEV_OVUV5 0x04
#define DEV_OVUV6 0x05
#define DEV_TLOOP_CFG 0x06
#define DEV_AFE_CFG 0x07
#define DEV_PHASE_CFG 0x08
#define DEV_CONFIG1 0x09
#define DEV_CONTROL1 0x0A
#define DEV_CONTROL2 0x0B
#define DEV_CONTROL3 0x0C
#define DEV_STAT1 0x0D
#define DEV_STAT2 0x0E
#define DEV_STAT3 0x0F
#define DEV_STAT4 0x10
#define DEV_STAT5 0x11
#define DEV_STAT6 0x12
#define DEV_STAT7 0x13
#define DEV_CLCRC 0x14
#define DEV_CRC 0x15
#define CRCCALC 0x16
#define DEV_EE_CTRL1 0x17
#define DEV_CRC_CTRL1 0x18
#define DEV_EE_CTRL4 0x19
#define DEV_UNLK_CTRL1 0x1A

#define CRC_READ_REG_DEV_STAT5 0x01
#define PI_FLT 3.14159265 //pi exprimé en flottant
#define TWO_PI_FLT 6.28318531 //2*pi exprimée en flottant
#define THREE_PI_FLT 9.424778 //3*pi exprimée en flottant
#define FOUR_PI_FLT 12.566371 //4*pi exprimée en flottant
#define FIVE_PI_FLT 15.707963 //5*pi exprimée en flottant
#define SIX_PI_FLT 18.849556 //6*pi exprimée en flottant
#define MINUS_PI_FLT -3.14159265 //-pi exprimé en flottant



//strcuture pour registre interne du PGA411 : pour chaque registre, on stocke l'adresse
//de lecture, d'écriture, la valeur par défaut et la valeur actuelle.
typedef struct {
	/* registers definition */
	const uint16_t read_add; /* SPI read address */
	const uint16_t write_add; /* SPI write address */
	const uint16_t def_val; /* default PGA value */
	uint16_t real_val; /* realtime register content */
} PGA411_reg_t;


//structure pour sauvegarder les trames SPI émises ou reçues
typedef union {
	/* outgoing data frame (from master to slave) */
	struct {
		uint32_t addr:8; /* register address, bits 24..31 */
		uint32_t dataout:16; /* data, MSB first, bits 8..23 */
		uint32_t reserved:2; /* reserved, always 0, bits 6..7 */
		uint32_t mcrc:6; /* master's CRC for data, bits 0..5 */
	};
	/* incomming data frame (to master from slave */
	struct {
		uint32_t ecbk:8; /* register address, bits 24..31 */
		uint32_t datain:16; /* data, MSB first, bits 8..23 */
		uint32_t stat:2; /* status of SPI communication, bits 6..7 */
		uint32_t scrc:6; /* slaves's CRC for data, bits 0..5 */
	};
	/* and finally the whole frame */
	struct {
		uint32_t frame;
	};
} pga411_spi_frame_t;

//Structure pour sauvegarder le contenu des 7 registres de statut du PGA411 (DEV_STATX, X = 1 à 7)
typedef struct {
	uint8_t PGA411_Failed;
	uint16_t dev_stat_1;
	uint16_t dev_stat_2;
	uint16_t dev_stat_3;
	uint16_t dev_stat_4;
	uint16_t dev_stat_5;
	uint16_t dev_stat_6;
	uint16_t dev_stat_7;
} PGA411_status_reg_t;



//pads utilisés pour piloter le PGA411
#define PIN_NRST PE4
#define PIN_FaultRes PD8
#define PIN_Fault PG8
#define PIN_BMode0 PG9
#define PIN_DSPI2_NCS3 PG10
#define PIN_DSPI2_SCK PA0
#define PIN_DSPI2_SIN PA2
#define PIN_DSPI2_SOUT PA1


//paramètres pour le calcul du CRC6
#define CRC6_SEED 0x3F
#define CRC6_POLY 0x5B //poly générateur = x^6+x^4+x^3+x+1
#define CRC6_SIZE 6 //la taille du CRC est de 6 bits
#define CRC6_DataBit 24 //taille en bits du message à coder (24 bits formés de 8 d'adresse + 16 bits de données)
#define CRC6_DataByte 3 //taille en octets du message à coder (3 octets)

//paramètres du module DSPI utilisé (DSPI2)
#define DSPI_PGA411 Module_DSPI2
#define DSPI_CTAR_NB 0
#define DSPI_PCS_NB CS3_Asserted
#define SPI_DUMMY 0x00F0

//paramètres pour la gestion de la résolution des mesures d'angle et de vitesse
#define RESO_10BITS 0
#define RESO_12BITS 1

//pour indiquer si le PGA411 est fonctionnel ou pas
#define PGA411_STATUS_FAULT 0
#define PGA411_STATUS_OK 1

//Initialization and configuration
void pga411_init_IO(void);
void pga411_init_SPI(void);
void pga411_reset(void);
void pga411_config(void);
uint8_t pga411_set_resolution(uint8_t Resolution);
uint8_t pga411_Config_Init(void);
void pga411_Change_State(uint16_t state);
void pga411_DeviceUnlock(void);

//Read/write of internal registers
uint16_t pga411_ReadReg(uint16_t reg);
uint16_t pga411_ReadReg_OneRead(uint16_t reg);
uint16_t pga411_ReadReg_CRC_connu(uint16_t reg, uint32_t CRC_connu);
uint16_t pga411_ReadReg_CRC_connu_OneRead(uint16_t reg, uint32_t CRC_connu);
void pga411_WriteReg(uint16_t reg, uint16_t data);
pga411_spi_frame_t pga411_XmitSPI(uint16_t dir, uint16_t reg, uint16_t wdata);
pga411_spi_frame_t pga411_XmitSPI_no_CRC_compute(uint16_t dir, uint16_t reg, uint16_t wdata, uint32_t CRC_Tx);
uint32_t pga411_Compute_CRC6(uint32_t dataIn);

//Obtain PGA411 device status (failure diagnosis)
void Toggle_FaultRes_Pin(void);
PGA411_status_reg_t pga411_read_status_registers(void);
uint8_t pga411_Sfault_status(void);
uint8_t pga411_FaultPin_status(void);
uint8_t pga411_GetFault_status(void);

//Read rotor angle position
uint32_t pga411_Get_Angle_Degre_F32(uint8_t myResolution);
tFloat pga411_Get_Angle_Degre_FLT(uint8_t myResolution);
uint32_t pga411_Compute_Angle_Elec_DEG_F32(uint32_t angle_meca, uint32_t NbPoles);
tFloat pga411_Compute_Angle_Elec_DEG_FLT(tFloat angle_meca, uint32_t NbPoles);

//read rotor speed
int32_t pga411_Get_Speed_RPM_F32(uint8_t myResolution);
tFloat pga411_Get_Speed_RPM_FLT(uint8_t myResolution);
int32_t Convert_SPEED_DEG_US_TO_RPM_F32(int32_t Speed_deg_us);
tFloat Convert_SPEED_DEG_US_TO_RPM_FLT(tFloat Speed_deg_us);
tFloat Convert_AngleElec_Rad_FLT_slow(uint16_t angle_read, uint32_t myResolution, uint16_t paire_poles);
tFloat Convert_AngleElec_Rad_FLT(uint16_t angle_read, uint32_t myResolution, uint16_t paire_poles);
tFloat Convert_AngleMeca_Rad_FLT(uint16_t angle_read, uint32_t myResolution);
tFloat Adapt_Angle_minus_pi_plus_pi_FLT(tFloat inAngle);

//gestion des interruptions
void ERQ_PGA411_Fault_isr(void);

#endif /* PGA411_H_ */
