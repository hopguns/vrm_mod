#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "analog.h"
//************************************************************************
// Acquisition analogique
//************************************************************************
uint8_t statusADC;    // Status de l'algorythme
uint8_t indMoyADC;    // index du buffer Moyenne
uint8_t indCreteADC;  // index pour le calcul de la valeur crete
uint8_t channelADC;   // canal en cours de mesure
uint8_t nbPointADC;   // nombre de point de mesure
//------------------------------------------------------------------------
uint16_t moyMesADC, maxMesADC;
uint16_t bufMoyADC[ADC_NB_POINT_MOY];
uint16_t mesureADC[ADC_NB_CH];
//------------------------------------------------------------------------
const uint8_t   confChannelADC[ADC_NB_CH] =
{
	0x00,	// ADC0 - A0
	0x04,	// ADC4 - A4
	0x05,	// ADC5 - A5
	0x06,	// ADC6 - A6
	0x07,	// ADC7 - A7
	//0x08,   // Mesure de la T° interne
};
//------------------------------------------------------------------------
CallbackFunction callbackADC[ADC_NB_CH];
//************************************************************************
//initialise les variables et configuration
//************************************************************************
void initADC()
{
	//--------------------------------------------------------------------
	statusADC = ADC_FREE;
	nbPointADC = ADC_NB_POINT_MOY;
	channelADC = ADC_LUM_ID;
	//--------------------------------------------------------------------
	// Choix de la division du prescaler (ici, facteur 128)
	//--------------------------------------------------------------------
	ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
	//--------------------------------------------------------------------
	for (uint8_t i=0; i<ADC_NB_CH; i++)
	{
		mesureADC[i] = 0;
		callbackADC[i] = NULL;
	}
	//--------------------------------------------------------------------
	// Interruption à la fin de la conversion
	//--------------------------------------------------------------------
	ADCSRA |= ((1 << ADEN) | (1 << ADIE));
	//--------------------------------------------------------------------
}
//************************************************************************
// definit la fonction qui doit etre jouée après la mesure de la voie
//************************************************************************
void setCallbackADC(uint8_t channel, CallbackFunction function)
{
	//--------------------------------------------------------------------
	callbackADC[channel] = function;
	//--------------------------------------------------------------------
}
//************************************************************************
// Lit toutes les voies en boucle et met le resultat dans mesureADC
//************************************************************************
void controlADC()
{
	//--------------------------------------------------------------------
	if (statusADC == ADC_ONGOING) return;
	//--------------------------------------------------------------------
	mesureADC[channelADC] = maxMesADC / ADC_NB_POINT_MOY;
	//--------------------------------------------------------------------
	if (callbackADC[channelADC] != NULL)
	{
		callbackADC[channelADC]();
	}
	//--------------------------------------------------------------------
	channelADC++;
	if (channelADC == ADC_NB_CH)
	{
		channelADC = 0;
	}
	//--------------------------------------------------------------------
	indMoyADC = 0;
	indCreteADC = 0;
	moyMesADC = 0;
	maxMesADC = 0;
	for (uint8_t i=0; i<ADC_NB_POINT_MOY; i++) bufMoyADC[i] = 0;
	//--------------------------------------------------------------------
	if (channelADC == ADC_IVR_ID) nbPointADC = ADC_NB_POINT_20MS;
	else nbPointADC = ADC_NB_POINT_MOY;
	//--------------------------------------------------------------------
	ADMUX = 0x40 | confChannelADC[channelADC]; // Sélection de la pin AN
	ADCSRA |= ((1 << ADSC)); // On lance la prochaine conversion
	//--------------------------------------------------------------------
	statusADC = ADC_ONGOING;
	//--------------------------------------------------------------------
}
//************************************************************************
//
//************************************************************************
ISR(ADC_vect)
{
	//---------------------------------------------------------------------
	uint8_t i = ADCL; // A lire avant ADCH pour le mecanisme mock
	uint8_t j = ADCH;
	int result = (j << 8) | i;
	//---------------------------------------------------------------------
	indMoyADC = (indMoyADC + 1) % ADC_NB_POINT_MOY;
	//---------------------------------------------------------------------
	moyMesADC = moyMesADC + result  - bufMoyADC[indMoyADC];
	bufMoyADC[indMoyADC] = result;
	if (moyMesADC > maxMesADC) maxMesADC = moyMesADC;
	//---------------------------------------------------------------------
	indCreteADC++;
	if (indCreteADC < nbPointADC)
	{
		ADCSRA |= ((1 << ADSC)); // On lance la prochaine conversion
	}
	else
	{
		statusADC = ADC_FINISH;
	}
 	//---------------------------------------------------------------------
}
//************************************************************************
//
//************************************************************************

