#ifndef IHM_H
#define IHM_H
//************************************************************************
#include <stdint.h>
//************************************************************************
// Definiton Timer 0
//************************************************************************
#define TIMER2_PRESCALER		64
#define TIMER2_OCA_COUNT		0xA0
//************************************************************************
// Etiquettes pour la gestion du clavier
//************************************************************************
#define KEY_PIN_UP			PORTC1
#define KEY_PIN_OK			PORTC2
#define KEY_PIN_DOWN		PORTC3
//------------------------------------------------------------------------
#define KEY_TEMPO_MIN		20
#define KEY_TEMPO_SHORT		30
#define KEY_TEMPO_LONG		2000
#define KEY_TEMPO_MAX		2200
//------------------------------------------------------------------------
#define KEY_ST_NONE			0
#define KEY_ST_NEW			1
#define KEY_ST_SHORT		2
#define KEY_ST_LONG			3
//------------------------------------------------------------------------
#define KEY_NONE			0x00
#define KEY_UP				0x02
#define KEY_OK				0x04
#define KEY_DOWN			0x08
#define KEY_LONG   			0x40
#define KEY_MASK  			0x0E
//************************************************************************
// Etiquettes pour la gestion des leds
//************************************************************************
#define LED_GREEN				0
#define LED_BLUE				1
#define LED_RED					2
#define LED_NUM					3
//------------------------------------------------------------------------
#define LED_TEMPO_PWM			10		// R�glage luminosit� des LEDs
#define LED_TEMPO_BASE			50
//------------------------------------------------------------------------
#define LED_OFF					0
#define LED_BLINK				1
#define LED_ON					2
//************************************************************************
// Etiquettes pour la gestion mode setting
//************************************************************************
#define SETTING_PIN				PORTB3
//************************************************************************
// Etiquettes pour la gestion du buzzer
//************************************************************************
#define BUZZER_STD				PORTB0
//------------------------------------------------------------------------
#define BUZZER_TEMPO_KEY		10
//------------------------------------------------------------------------
#define BUZZER_OFF				0
#define BUZZER_ON				1
#define BUZZER_KEY				2
#define BUZZER_ALARM			3
#define BUZZER_ALARM_TEST		4
#define BUZZER_COMMAND			5
//************************************************************************
// Structure
//************************************************************************
struct SLed
{
	uint8_t state;
	uint8_t mask;
	uint8_t index;
	uint8_t cntOn;
	uint8_t cntTot;
};
//************************************************************************
// Fonctions et variables
//************************************************************************
void	initIHM(bool ParamLoaded);
void	setLedIHM(uint8_t iLed, uint8_t type, uint16_t cntOn = 500, uint16_t cntTot = 1000);
uint8_t	getLedIHM(uint8_t iLed);
void    cmdeBuzzerIHM(uint16_t timeOn, uint16_t timeOff, uint16_t pulseNum);
void	controlIHM();
void	callbackLuminosityIHM(void);
//------------------------------------------------------------------------
extern uint8_t stBuzzerStd;
//extern uint8_t lastPressedKey;
//extern bool modeSetting;
//************************************************************************
// Fin de listing
//************************************************************************
#endif
