#ifndef ALARM_H
#define ALARM_H
//************************************************************************
#include <stdint.h>
//************************************************************************
// Etiquettes pour le Volet Roulant Motorisé
//************************************************************************
#define ALARM_ST_UNKNOWN	0
#define ALARM_ST_NORMAL		1
#define ALARM_ST_TROUBLE	2
#define ALARM_ST_INTRUSION	3
#define ALARM_ST_MASK		4
#define ALARM_ST_CASE		5
//------------------------------------------------------------------------
#define ALARM_NB_MES_DETECT		10		// Nombre de points à détecter dans la plage pour valider l'état
//------------------------------------------------------------------------
#define ALARM_LEVEL0		0
#define ALARM_LEVEL1		1
#define ALARM_LEVEL2		2
#define ALARM_LEVEL3		3
#define ALARM_LEVEL4		4
#define ALARM_LEVEL5		5
//------------------------------------------------------------------------
// Voir feuille de calcul xls
//------------------------------------------------------------------------
#define ALARM_1_L1_L		(uint16_t)(128)
#define ALARM_1_L1_H		(uint16_t)(231)
#define ALARM_1_L2_L		(uint16_t)(547)
#define ALARM_1_L2_H		(uint16_t)(649)
#define ALARM_1_L3_L		(uint16_t)(973)
//------------------------------------------------------------------------
#define ALARM_2_L1_H		(uint16_t)(51)
#define ALARM_2_L2_L		(uint16_t)(348)
#define ALARM_2_L2_H		(uint16_t)(450)
#define ALARM_2_L3_L		(uint16_t)(973)
//------------------------------------------------------------------------
//#define ALARM_TEMPO_MS		30000	// Temps theorique max du mouvement du volet
//************************************************************************
// Fonctions et variables
//************************************************************************
void		initAlarm(uint8_t option);
void		callbackIn1Alarm(void);
void		callbackIn2Alarm(void);
uint8_t		getStateAlarm(void);
void		controlAlarm(uint8_t option);
//------------------------------------------------------------------------
extern uint8_t stateAlarm, stateIn1Alarm, stateIn2Alarm;
//************************************************************************
// Fin de listing
//************************************************************************
#endif
