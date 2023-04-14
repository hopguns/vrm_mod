#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <ds18b20.h>
#include <portRS485.h>
#include <bootFunc.h>
#include "eeparam.h"
#include "homeBusFunc.h"
#include "vrm.h"
#include "Alarm.h"
#include "ihm.h"
#include "analog.h"
#include "timer.h"
//************************************************************************
// Main function
//************************************************************************
int main(void)
{
	bool paramLoaded;
	//--------------------------------------------------------------------
	paramLoaded = getResetMCUSR();
	wdt_reset(); // Reset du watchdog
	//--------------------------------------------------------------------
	paramLoaded = loadParam();
	initTimer();
	if (param.optionDS18B20) initDS18B20(param.tempoDS18B20);
	initADC();
	initVRM(); // à initialiser apres ADC !
	initAlarm(param.optionAlarm);  // à initialiser apres ADC !
	initIHM(paramLoaded);  // à initialiser apres ADC !
	initHomeBus(initFonctHomeBus, manageFonctHomeBus);
	//--------------------------------------------------------------------
	sei();
	//--------------------------------------------------------------------
	while(1)
	{
		//----------------------------------------------------------------
		controlADC();									// Mesures analogiques
		if (param.optionDS18B20) controlDS18B20();		// Mesures de temperature
		controlVRM();									// Gestion du volet roulant motorisé
		controlAlarm(param.optionAlarm);				// Gestion du module Alarme
		controlIHM();									// Gestion des interfaces
		controlProtocolHomeBus();						// Gestion du protocole de com RS485
		//----------------------------------------------------------------
		wdt_reset(); // Reset du watchdog
		//----------------------------------------------------------------
	}
	//--------------------------------------------------------------------
}
//************************************************************************
// Fin de listing
//************************************************************************
