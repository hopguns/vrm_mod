#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "vrm.h"
#include "ihm.h"
#include "analog.h"
#include "eeparam.h"
#include "homeBusFunc.h"
//************************************************************************
#ifndef _AVR_IOM328P_H_
Attention sur la definition des registres
#endif
//************************************************************************
// Gestion du VRM (Volet Roulant Motorisé)
//************************************************************************
uint8_t pressedKey, currentKey;
uint8_t stateKey;
uint16_t counterKey;
//------------------------------------------------------------------------
uint8_t counterLed, pwmLed, cmdeLed;
uint16_t tempoLed;
SLed leds[LED_NUM];
uint8_t allLedMask;
//------------------------------------------------------------------------
uint16_t tempoBuzzer;
uint8_t stBuzzerStd, stBuzzerkey, indexBuzzer;
//------------------------------------------------------------------------
uint16_t timeBuzzerCmdeOn;	// temps en ms du buzzer à ON
uint16_t timeBuzzerCmdeOff;	// temps en ms du buzzer à OFF
uint16_t timeBuzzerCmdeNum;	// Nombre de pulse buzzer
//************************************************************************
// Initialisation des relais pour le VRM
//************************************************************************
void initIHM(bool ParamLoaded)
{
	//--------------------------------------------------------------------
	#if (TIMER2_PRESCALER == 64)
	TCCR2B |= ((1 << CS22));
	#else
	Erreur: modifier la config du registre
	#endif
	//--------------------------------------------------------------------
	TCNT2 = 0; // initialize counter
    TIMSK2 |= ((1 << OCIE2A) | (1 << TOIE2));
	OCR2A = TIMER2_OCA_COUNT;
	//--------------------------------------------------------------------
	// Inititialisation des keys
	//--------------------------------------------------------------------
	DDRC &= ~((1 << KEY_PIN_UP) | (1 << KEY_PIN_OK) | (1 << KEY_PIN_DOWN)); // Entrées
	//--------------------------------------------------------------------
	counterKey = 0;
	stateKey = KEY_ST_NONE;
	pressedKey = 0;
	currentKey = 0;
	//--------------------------------------------------------------------
	// Inititialisation des Leds
	//--------------------------------------------------------------------
	counterLed = 0;
	pwmLed = LED_TEMPO_PWM;
	cmdeLed = 0;
	tempoLed = 0;
    timeBuzzerCmdeOn = 0;
    timeBuzzerCmdeOff = 0;
    timeBuzzerCmdeNum = 0;
	//--------------------------------------------------------------------
	leds[LED_GREEN].mask = 1 << PORTD5;
	leds[LED_BLUE].mask = 1 << PORTD6;
	leds[LED_RED].mask = 1 << PORTD7;
	//--------------------------------------------------------------------
	setLedIHM(LED_GREEN, LED_OFF);
	setLedIHM(LED_BLUE, LED_OFF);
	setLedIHM(LED_RED, LED_OFF);
	//--------------------------------------------------------------------
	allLedMask = leds[LED_GREEN].mask | leds[LED_BLUE].mask | leds[LED_RED].mask;
	DDRD |= allLedMask; // Sorties
	//--------------------------------------------------------------------
	// Inititialisation du buzzer
	//--------------------------------------------------------------------
	DDRB |= ((1 << BUZZER_STD)); // Sorties
	tempoBuzzer = 0;
	stBuzzerStd = stBuzzerkey = indexBuzzer = 0;
	//--------------------------------------------------------------------
	setCallbackADC(ADC_LUM_ID, callbackLuminosityIHM);
	//--------------------------------------------------------------------
	uint8_t nbInit = 7;
	if (ParamLoaded) nbInit = 1;
	//--------------------------------------------------------------------	
	while (nbInit > 0)
	{
		PORTD |= allLedMask; // Allumées
		//PORTB |= ((1 << BUZZER_STD)); // Allumé
		for (volatile uint16_t i = 0; i < 5000; i++);
		PORTD &= ~allLedMask; // Eteintes
		//PORTB &= ~((1 << BUZZER_STD)); // Eteint
		for (volatile uint16_t i = 0; i < 10000; i++);
		nbInit--;
	}
	//--------------------------------------------------------------------
}
//************************************************************************
// Interruption périodique
//************************************************************************
ISR(TIMER2_OVF_vect)
{
	//--------------------------------------------------------------------
	uint8_t val = ~PINC & KEY_MASK;
	//--------------------------------------------------------------------
	if (val == 0)
	{
		if (stateKey > KEY_ST_NEW)
		{
			if (pressedKey == 0)
			{
				pressedKey = currentKey;
				if (stateKey == KEY_ST_LONG) pressedKey = pressedKey | KEY_LONG;
			}
		}
		currentKey = 0;
		counterKey = 0;
		stateKey = KEY_ST_NONE;
	}
	else
	{
		if (currentKey == val)
		{
			counterKey++;
			if (counterKey < KEY_TEMPO_MIN) {stateKey = KEY_ST_NONE;}
			else if (counterKey < KEY_TEMPO_SHORT) {stateKey = KEY_ST_NEW; stBuzzerkey = BUZZER_KEY;}
			else  if (counterKey < KEY_TEMPO_LONG) {stateKey = KEY_ST_SHORT;}
			else  if (counterKey < KEY_TEMPO_MAX) {stateKey = KEY_ST_LONG; stBuzzerkey = BUZZER_KEY;}
			else  {stateKey = KEY_ST_LONG; stBuzzerkey = BUZZER_OFF;}
		}
		else
		{
			currentKey = val;
			counterKey = 0;
		}
	}
	//--------------------------------------------------------------------
}
//************************************************************************
// Interruption périodique
//************************************************************************
ISR(TIMER2_COMPA_vect)
{
	//--------------------------------------------------------------------
	if (counterLed < LED_TEMPO_PWM) counterLed++;
	else counterLed = 0;
	//--------------------------------------------------------------------
	if (counterLed < pwmLed) PORTD |= cmdeLed;
	else PORTD &= ~allLedMask;
	//--------------------------------------------------------------------
	if (tempoLed > 0) tempoLed--;
	//--------------------------------------------------------------------
	if (tempoBuzzer > 0) tempoBuzzer--;
	//--------------------------------------------------------------------
}
//************************************************************************
// Calcul si le courant à franchi le seuil sur un nombre de mesure
//************************************************************************
void callbackLuminosityIHM(void)
{
	//--------------------------------------------------------------------
	if (mesureADC[ADC_LUM_ID] < 150)
	{
		pwmLed = 3;	
	}
	else
	{
		pwmLed = mesureADC[ADC_LUM_ID] / 25;
		if (pwmLed >= LED_TEMPO_PWM) pwmLed = LED_TEMPO_PWM;
	}
	//--------------------------------------------------------------------
}
//************************************************************************
//
//************************************************************************
void setLedIHM(uint8_t iLed, uint8_t type, uint16_t cntOn/*= 500*/, uint16_t cntTot/*= 1000*/)
{
	//--------------------------------------------------------------------
	leds[iLed].index = 0;
	leds[iLed].state = type;
	leds[iLed].cntOn = (uint8_t)(cntOn / LED_TEMPO_BASE);
	leds[iLed].cntTot = (uint8_t)(cntTot / LED_TEMPO_BASE);
	//--------------------------------------------------------------------
}
//************************************************************************
//
//************************************************************************
uint8_t getLedIHM(uint8_t iLed)
{
	//--------------------------------------------------------------------
	return leds[iLed].state;
	//--------------------------------------------------------------------
}
//************************************************************************
//
//************************************************************************
void cmdeBuzzerIHM(uint16_t timeOn, uint16_t timeOff, uint16_t pulseNum)
{
	//--------------------------------------------------------------------
    timeBuzzerCmdeOn = timeOn;
    timeBuzzerCmdeOff = timeOff;
    timeBuzzerCmdeNum = pulseNum;
	stBuzzerStd = BUZZER_COMMAND ;
	//--------------------------------------------------------------------
}
//************************************************************************
// 
//************************************************************************
void controlIHM()
{
	//--------------------------------------------------------------------
	// Gestion du clavier
	//--------------------------------------------------------------------
	if (pressedKey > 0)
	{	
		switch(pressedKey)
		{
			case KEY_UP:
				cmdeVoletVRM(VRM_CMD_FULL_UP, true);
			break;
			case KEY_OK:
				cmdeVoletVRM(VRM_CMD_STOP, true);
			break;
			case KEY_DOWN:
				cmdeVoletVRM(VRM_CMD_FULL_DOWN, true);
			break;
			case KEY_UP | KEY_LONG:
				cmdeVoletVRM(VRM_CMD_FORCE_UP, true);
			break;
			case KEY_OK | KEY_LONG:
			break;
			case KEY_DOWN | KEY_LONG:
				cmdeVoletVRM(VRM_CMD_FORCE_DOWN, true);
			break;
		}
        addEventHomeBus(pressedKey >> 1);
	}
	//--------------------------------------------------------------------
	// Gestion des Leds
	//--------------------------------------------------------------------
	if (tempoLed == 0)
	{
		tempoLed = LED_TEMPO_BASE;
		//----------------------------------------------------------------
		for(int8_t i=0; i<LED_NUM; i++)
		{
			if (leds[i].state == LED_OFF) cmdeLed &= ~leds[i].mask;
			//------------------------------------------------------------
			if (leds[i].state == LED_ON) cmdeLed |= leds[i].mask;
			//------------------------------------------------------------
			if (leds[i].state == LED_BLINK)
			{
				if (leds[i].index < leds[i].cntOn) cmdeLed |= leds[i].mask;
				else cmdeLed &= ~leds[i].mask;
				if (leds[i].index < leds[i].cntTot) leds[i].index++;
				else leds[i].index = 0;
			}
		}
	}
	//--------------------------------------------------------------------
	// Gestion du buzzer
	//--------------------------------------------------------------------
	if (tempoBuzzer == 0)
	{	
		if (stBuzzerkey == BUZZER_KEY)
		{
			tempoBuzzer = BUZZER_TEMPO_KEY;
			PORTB |= (1 << BUZZER_STD);
			stBuzzerkey = BUZZER_OFF;
		}
		else
		{
			switch(stBuzzerStd)
			{
				//------------------------------------------------------------
				case BUZZER_OFF:
					PORTB &= ~(1 << BUZZER_STD); // Buzzer off
				break;
				//------------------------------------------------------------
				case BUZZER_ON:
					PORTB |= (1 << BUZZER_STD); // Buzzer on
				break;
				//------------------------------------------------------------
				case BUZZER_ALARM:
				switch(indexBuzzer)
				{
					case 0:
					indexBuzzer = 1;
					tempoBuzzer = param.timeBuzzerAlarmOn;
					PORTB |= (1 << BUZZER_STD); // Buzzer on
					break;
					case 1:
					indexBuzzer = 0;
					tempoBuzzer = param.timeBuzzerAlarmOff;
					PORTB &= ~(1 << BUZZER_STD); // Buzzer off
					break;
				}
				break;
				//------------------------------------------------------------
				case BUZZER_ALARM_TEST:
				switch(indexBuzzer)
				{
					case 0:
					indexBuzzer = 1;
					tempoBuzzer = param.timeBuzzerTestOn;
					PORTB |= (1 << BUZZER_STD); // Buzzer on
					break;
					case 1:
					indexBuzzer = 2;
					tempoBuzzer = param.timeBuzzerTestOff;
					PORTB &= ~(1 << BUZZER_STD); // Buzzer off
					break;
					case 2:
					indexBuzzer = 0;
					stBuzzerStd = BUZZER_OFF ;
					break;
				}
				break;
				//------------------------------------------------------------
				case BUZZER_COMMAND:
				if ((indexBuzzer%2)==0)
				{
					tempoBuzzer = timeBuzzerCmdeOn;
					PORTB |= (1 << BUZZER_STD); // Buzzer on
				}
				else
				{
					tempoBuzzer = timeBuzzerCmdeOff;
					PORTB &= ~(1 << BUZZER_STD); // Buzzer off
				}
				if (indexBuzzer < timeBuzzerCmdeNum) indexBuzzer++;
				else
				{
					indexBuzzer = 0;
					stBuzzerStd = BUZZER_OFF ;
				}
				break;
				//------------------------------------------------------------
			}
		}
		//----------------------------------------------------------------
	}
	//--------------------------------------------------------------------
}
//************************************************************************
// Fin de listing
//************************************************************************



