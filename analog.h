#ifndef ANALOG_H
#define ANALOG_H
//************************************************************************
#include <stdint.h>
//************************************************************************
// Etiquettes pour les mesures Analogiques
//************************************************************************
#define ADC_FREE			0
#define ADC_ONGOING			1
#define ADC_FINISH			2
#define ADC_NB_POINT_MOY	4   // NB de point pour la moyenne glissante
#define ADC_NB_POINT_10MS	(10000 / 104)  // nb echantillion pour 10 ms
#define ADC_NB_POINT_20MS	(20000 / 104)  // nb echantillion pour 20 ms
//------------------------------------------------------------------------
#define ADC_NB_CH			5   // NB de channel
//------------------------------------------------------------------------
#define ADC_LUM_ID			0     // Mesure de la luminosité
#define ADC_AL2_ID			1     // Mesure de l'alarme 2
#define ADC_AL1_ID			2     // Mesure de l'alarme 1
#define ADC_IVR_ID			3     // Mesure du courant du volet roulant (resistance 3300 Ohm)
#define ADC_VIN_ID			4     // Mesure de la tension d'alimentation
//#define ADC_TEM_ID			5     // Mesure de la T° interne
//------------------------------------------------------------------------
// Generic Callback funtion
//------------------------------------------------------------------------
typedef void (*CallbackFunction) (void);
//************************************************************************
// Fonctions et variables
//************************************************************************
extern uint16_t   mesureADC[ADC_NB_CH];
//------------------------------------------------------------------------
void initADC();
void setCallbackADC(uint8_t channel, CallbackFunction function);
void controlADC();
//************************************************************************
// Fin de listing
//************************************************************************
#endif
