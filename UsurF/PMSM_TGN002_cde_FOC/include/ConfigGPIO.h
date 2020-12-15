/*
 * ConfigGPIO.h
 *
 *  Created on: 4 juil. 2018
 *      Author: admin_aboyer
 *
 *  Configuration des GPIO (entrée, sortie, fonctions spéciales, type de pad ...) + lecture/écriture
 */

#ifndef CONFIGGPIO_H_
#define CONFIGGPIO_H_

/* GPIO PCRs */
/* voir tables 4.7 p 107 ou 4.16 p 151 MPC5744PRM.pdf */
#define  PA0  0
#define  PA1  1
#define  PA2  2
#define  PA3  3
#define  PA4  4
#define  PA5  5
#define  PA6  6
#define  PA7  7
#define  PA8  8
#define  PA9  9
#define  PA10  10
#define  PA11  11
#define  PA12  12
#define  PA13  13
#define  PA14  14
#define  PA15  15
#define  PB0  16
#define  PB1  17
#define  PB2  18
#define  PB3  19
#define  PB4  20
#define  PB5  21
#define  PB6  22
#define  PB7  23
#define  PB8  24
#define  PB9  25
#define  PB10  26
#define  PB11  27
#define  PB12  28
#define  PB13  29
#define  PB14  30
#define  PB15  31
#define  PC00  32
#define  PC01  33
#define  PC2  34
#define  PC3  35
#define  PC4  36
#define  PC5  37
#define  PC6  38
#define  PC7  39
#define  PC8  40
#define  PC9  41
#define  PC10  42
#define  PC11  43
#define  PC12  44
#define  PC13  45
#define  PC14  46
#define  PC15  47
#define  PD00  48
#define  PD1  49
#define  PD2  50
#define  PD3  51
#define  PD4  52
#define  PD5  53
#define  PD6  54
#define  PD7  55
#define  PD8  56
#define  PD9  57
#define  PD10  58
#define  PD11  59
#define  PD12  60
#define  PD13  61
#define  PD14  62
#define  PD15  63
#define  PE0  64
#define  PE1  65
#define  PE2  66
#define  PE3  67
#define  PE4  68
#define  PE5  69
#define  PE6  70
#define  PE7  71
#define  PE8  72
#define  PE9  73
#define  PE10  74
#define  PE11  75
#define  PE12  76
#define  PE13  77
#define  PE14  78
#define  PE15  79
#define  PF0  80
#define  PF1  81
#define  PF2  82
#define  PF3  83
#define  PF4  84
#define  PF5  85
#define  PF6  86
#define  PF7  87
#define  PF8  88
#define  PF9  89
#define  PF10  90
#define  PF11  91
#define  PF12  92
#define  PF13  93
#define  PF14  94
#define  PF15  95
#define  PG0  96
#define  PG1  97
#define  PG2  98
#define  PG3  99
#define  PG4  100
#define  PG5  101
#define  PG6  102
#define  PG7  103
#define  PG8  104
#define  PG9  105
#define  PG10  106
#define  PG11  107
#define  PG12  108
#define  PG13  109
#define  PG14  110
#define  PG15  111
#define  PH0  112
#define  PH1  113
#define  PH2  114
#define  PH3  115
#define  PH4  116
#define  PH5  117
#define  PH6  118
#define  PH7  119
#define  PH8  120
#define  PH9  121
#define  PH10  122
#define  PH11  123
#define  PH12  124
#define  PH13  125
#define  PH14  126
#define  PH15  127
#define  PI0  128
#define  PI1  129
#define  PI2  130
#define  PI3  131
#define  PI4  132
#define  PI5  133
#define  PI6  134
#define  PI7  135
#define  PI8  136
#define  PI9  137
#define  PI10  138
#define  PI11  139
#define  PI12  140
#define  PI13  141
#define  PI14  142
#define  PI15  143
#define  PJ0  144
#define  PJ1  145
#define  PJ2  146
#define  PJ3  147
#define  PJ4  148
#define  PJ5  149
#define  PJ6  150
#define  PJ7  151
#define  PJ8  152
#define  PJ9  153
#define  PJ10  154
#define  PJ11  155
#define  PJ12  156
#define  PJ13  157
#define  PJ14  158
#define  PJ15  159
#define  PK0  160
#define  PK1  161
#define  PK2  162
#define  PK3  163
#define  PK4  164
#define  PK5  165
#define  PK6  166
#define  PK7  167
#define  PK8  168
#define  PK9  169
#define  PK10  170
#define  PK11  171
#define  PK12  172
#define  PK13  173
#define  PK14  174
#define  PK15  175
#define  PL0  176
#define  PL1  177
#define  PL2  178
#define  PL3  179
#define  PL4  180
#define  PL5  181
#define  PL6  182
#define  PL7  183
#define  PL8  184
#define  PL9  185
#define  PL10  186
#define  PL11  187
#define  PL12  188
#define  PL13  189
#define  PL14  190
#define  PL15  191
#define  PM0  192
#define  PM1  193
#define  PM2  194
#define  PM3  195
#define  PM4  196
#define  PM5  197
#define  PM6  198
#define  PM7  199
#define  PM8  200
#define  PM9  201
#define  PM10  202
#define  PM11  203
#define  PM12  204
#define  PM13  205
#define  PM14  206
#define  PM15  207
#define  PN0  208
#define  PN1  209
#define  PN2  210
#define  PN3  211
#define  PN4  212
#define  PN5  213
#define  PN6  214
#define  PN7  215
#define  PN8  216
#define  PN9  217
#define  PN10  218
#define  PN11  219
#define  PN12  220
#define  PN13  221
#define  PN14  222
#define  PN15  223
#define  PO0  224
#define  PO1  225
#define  PO2  226
#define  PO3  227
#define  PO4  228
#define  PO5  229
#define  PO6  230
#define  PO7  231
#define  PO8  232
#define  PO9  233
#define  PO10  234
#define  PO11  235
#define  PO12  236
#define  PO13  237
#define  PO14  238
#define  PO15  239
#define  PP0  240
#define  PP1  241
#define  PP2  242
#define  PP3  243
#define  PP4  244
#define  PP5  245
#define  PP6  246
#define  PP7  247
#define  PP8  248
#define  PP9  249
#define  PP10  250
#define  PP11  251
#define  PP12  252
#define  PP13  253
#define  PP14  254
#define  PP15  255
#define  PQ0  256
#define  PQ1  257
#define  PQ2  258
#define  PQ3  259
#define  PQ4  260
#define  PQ5  261
#define  PQ6  262
#define  PQ7  263


//options drive (half or full) + slew rate control (on ou off)
#define HALFDRV_SR_ON 0
#define FULLDRV_SR_ON 1
#define HALFDRV_SR_OFF 2
#define FULLDRV_SR_OFF 3
//options sortie open drain
#define OPENDRAIN_OFF 0
#define OPENDRAIN_ON 1
//(dés)activation buffer de sortie
#define OUTBUFF_OFF 0
#define OUTBUFF_ON 1
//(dés)activation buffer d'entrée
#define INBUFF_OFF 0
#define INBUFF_ON 1
//(dés)activation de l'hystérésis d'entrée d'un pad
#define HYST_OFF 0
#define HYST_ON 1
//(dés)activation pad analogique
#define ANALOGPAD_OFF 0
#define ANALOGPAD_ON 1
//option pull-up/down
#define PULL_OFF 0
#define PULL_ON 1
#define PULL_DOWN 0
#define PULL_UP 1
//états des pads de sortie et des LEDS connectées sur PC11,12 et 13
#define OUT_OFF 0
#define OUT_ON 1
#define LED_OFF 1
#define LED_ON 0
//paramètres pour les pads d'interruptions ext ERQ
#define ERQ_RISING_EVENT_OFF 0
#define ERQ_RISING_EVENT_ON 1
#define ERQ_FALLING_EVENT_OFF 0
#define ERQ_FALLING_EVENT_ON 1
#define ERQ_FILTER_OFF 0
#define ERQ_FILTER_ON 1


void Set_pad_out(uint8_t padNb, uint8_t State);
void Set_PORTA(uint16_t OutWord);
uint8_t Read_pad_in(uint8_t padNb);
void Config_pad_out(uint8_t padNb, uint8_t SlewRateAndDrive, uint8_t OpenDrain, uint8_t Analog, uint8_t PullEn, uint8_t PullSel, uint8_t OutState);
void Config_pad_in(uint8_t padNb, uint8_t PullEn, uint8_t PullSel, uint8_t Hyst_Sel);
void Config_analog_pad(uint8_t padNb);
void Config_Alter_Function_Out(uint8_t padNb, uint8_t FunctionNb);
void Config_Alter_Function_In(uint8_t IMCRNb, uint8_t FunctionNb);
void Config_ERQ_Pad(uint8_t IRQ_pin_Nb, uint8_t RisingEvent, uint8_t FallingEvent, uint8_t FilterEnable);


#endif /* CONFIGGPIO_H_ */
