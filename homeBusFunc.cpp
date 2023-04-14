#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <portRS485.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <mystrfunc.h>
#include <compileTime.h>
#include <ds18b20.h>
#include <BootFunc.h>
#include "analog.h"
#include "eeparam.h"
#include "homeBusFunc.h"
#include "ihm.h"
#include "vrm.h"
#include "alarm.h"
//************************************************************************
// Variables globales
//************************************************************************
#define HOMEBUS_NB_EVENTS      10
uint8_t listEvents[HOMEBUS_NB_EVENTS];
uint8_t firstEvent, lastEvent;
//************************************************************************
// Ajoute un event à la liste des events à communiquer au master
//************************************************************************
bool addEventHomeBus(uint8_t event)
{
    //--------------------------------------------------------------------
/* 	if ((firstEvent == HOMEBUS_NB_EVENTS) || (lastEvent == HOMEBUS_NB_EVENTS))
    {
        firstEvent = lastEvent = 0;
    }
    else
    {
        uint8_t next = (lastEvent + 1) % HOMEBUS_NB_EVENTS;
        if (next == firstEvent) return false;
        lastEvent = next;
    }
    listEvents[lastEvent] = event;
 */    return true;
    //--------------------------------------------------------------------
}
//************************************************************************
// Prend le 1er event de la liste des events à communiquer au master
//************************************************************************
uint8_t getEventHomeBus()
{
    //--------------------------------------------------------------------
/* 	if ((firstEvent == HOMEBUS_NB_EVENTS) || (lastEvent == HOMEBUS_NB_EVENTS))
    {
        return HOMEBUS_EVENT_VRM_NONE;
    }
    //--------------------------------------------------------------------
    uint8_t event = listEvents[firstEvent];
    listEvents[firstEvent] = 0;
    if (firstEvent == lastEvent)
    {
        firstEvent = lastEvent = HOMEBUS_NB_EVENTS;
    }
    else
    {
        firstEvent = (firstEvent + 1) % HOMEBUS_NB_EVENTS;
    }
    return event;
  */   //--------------------------------------------------------------------
  return 0;
}
//************************************************************************
// Fonction D'initialisation du protocol
//************************************************************************
void initFonctHomeBus(void)
{
	//--------------------------------------------------------------------
	setFilterAddressRS485(moduleAddressHB);
    //firstEvent = lastEvent = HOMEBUS_NB_EVENTS;
	//--------------------------------------------------------------------
}
//************************************************************************
// Fonction principale de communication
//************************************************************************
bool manageFonctHomeBus(char *inMsg, char *outMsg, uint8_t *origAddr, uint8_t *destAddr, uint8_t *ctrlId)
{
	//--------------------------------------------------------------------
	*(outMsg++) = inMsg[0];
	*(outMsg++) = inMsg[1];
	outMsg = to2ha(outMsg, stateAlarm);
	outMsg = to2ha(outMsg, getEventHomeBus());
	//--------------------------------------------------------------------
	if (*ctrlId == HOMEBUS_CTRL_BOOTLOADER)
	{
		*ctrlId = HOMEBUS_CTRL_NORMAL; // On indique au master qu'on est en mode normal
		strcpy(outMsg, "E");
		return true;
	}
	//--------------------------------------------------------------------
	uint8_t cmde;
	inMsg = from2ha(inMsg, &cmde); // Commande a executer
	masterAlarmHB = from1ha(*(inMsg++));
	//--------------------------------------------------------------------
	switch(cmde)
	{
		//----------------------------------------------------------------
		// Commande similaire au Bootloader
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_DEFAULT:
		*(outMsg++) = 'S';
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_SET_MOD_ADDR:
		{
			setAddressFromUniqueID(&inMsg);
			setFilterAddressRS485(moduleAddressHB);
			*(outMsg++) = 'S';
			*(outMsg++) = '\0';
		}
		return true;
		//----------------------------------------------------------------
		//----------------------------------------------------------------
		// Commandes communes à tous les types de module
		//----------------------------------------------------------------
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_RESET_CPU:
		*(outMsg++) = 'S';
		*(outMsg++) = '\0';
		sendDirectHomeBus(); // Envoie de la réponse en direct avant de faire le reset
		cli();
		eeprom_update_byte(EEPROM_VAR_AUTO_RESET, BOOT_FLAG_AUTO_RESET);
		while (1); // Bloque l'execussion pour déclencher le watchdog
		return false;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_GET_MODULE_TYPE:
		*(outMsg++) = 'S';
		*(outMsg++) = HOMEBUS_TYPE_VRM;
		*(outMsg++) = '-';
		*(outMsg++) = 'H';
		outMsg = to2da(outMsg, HOMEBUS_VERSION);
		*(outMsg++) = '-';
		outMsg = to4da(outMsg, __TIME_YEARS__);
		outMsg = to2da(outMsg, __TIME_MONTH__);
		outMsg = to2da(outMsg, __TIME_DAYS__);
		*(outMsg++) = '-';
		outMsg = to2da(outMsg, __TIME_HOURS__);
		outMsg = to2da(outMsg, __TIME_MINUTES__);
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_SET_MODULE_OPTIONS:
		if (strlen(inMsg) != 6)
		{
			*(outMsg++) = 'E';
			*(outMsg++) = '\0';
			return true;
		}
		param.optionAlarm = (*(inMsg++) == 'A');
		param.optionDS18B20 = (*(inMsg++) == 'T');
		inMsg = from4ha(inMsg, &param.tempoDS18B20);
		*(outMsg++) = 'S';
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_GET_MODULE_OPTIONS:
		*(outMsg++) = 'S';
		if (param.optionAlarm) *(outMsg++) = 'A'; else *(outMsg++) = 'a';
		if (param.optionDS18B20) *(outMsg++) = 'T'; else *(outMsg++) = 't';
		outMsg = to4ha(outMsg, param.tempoDS18B20);
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_GET_TEMPERATURE:
		{
			uint8_t result; float temp;
			result = getTempDS18B20(&temp);
			*(outMsg++) = 'S';
			if (result) *(outMsg++) = 'N'; else *(outMsg++) = 'O'; // New or Old
			sprintf((char *)outMsg, "%d", (int16_t)(temp * 100));
		}
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_GET_ROM_DS18B20:
		{
			uint8_t * rom = getRomDS18B20();
			*(outMsg++) = 'S';
			for(uint8_t i=0; i<DS18B20_ROM_DATA_LEN; i++)
			{
				outMsg = to2ha(outMsg, rom[i]);
			}
			*(outMsg++) = '\0';
		}
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_SAVE_PARAMS:
		*(outMsg++) = 'S';
		*(outMsg++) = '\0';
		saveParam();
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_SAVE_CMDE_BUZZER:
		{
            *(outMsg++) = 'S';
		    *(outMsg++) = '\0';
            uint16_t timeOn, timeOff, pulseNum;
		    inMsg = from4ha(inMsg, &timeOn);
		    inMsg = from4ha(inMsg, &timeOff);
		    inMsg = from4ha(inMsg, &pulseNum);
            cmdeBuzzerIHM(timeOn, timeOff, pulseNum);
        }
		return true;
		//----------------------------------------------------------------
		//----------------------------------------------------------------
		// Commandes spécifiques à ce type de module
		//----------------------------------------------------------------
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_VRM_GET_ANALOG_MESURES:
		*(outMsg++) = 'S';
		for(uint8_t i=0; i<ADC_NB_CH; i++)
		{
			outMsg = to4ha(outMsg, mesureADC[i]);
		}
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_VRM_GET_PARAM_TIME_VOLET:
		*(outMsg++) = 'S';
		outMsg = to4ha(outMsg, param.timeFullUp);
		outMsg = to4ha(outMsg, param.timeFullDown);
		outMsg = to4ha(outMsg, param.timeMaxWaitIvr);
		outMsg = to4ha(outMsg, param.timeStretchUp);
		outMsg = to4ha(outMsg, param.timeShowButee);
		outMsg = to4ha(outMsg, param.timeIvrMargin);
		outMsg = to4ha(outMsg, param.timeForceMargin);
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_VRM_SET_PARAM_TIME_VOLET:
		if (strlen(inMsg) != (4 * 7))
		{
			*(outMsg++) = 'E';
			*(outMsg++) = '\0';
			return true;
		}
		inMsg = from4ha(inMsg, &param.timeFullUp);
		inMsg = from4ha(inMsg, &param.timeFullDown);
		inMsg = from4ha(inMsg, &param.timeMaxWaitIvr);
		inMsg = from4ha(inMsg, &param.timeStretchUp);
		inMsg = from4ha(inMsg, &param.timeShowButee);
		inMsg = from4ha(inMsg, &param.timeIvrMargin);
		inMsg = from4ha(inMsg, &param.timeForceMargin);
		*(outMsg++) = 'S';
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_VRM_GET_PARAM_TIME_BUZZER:
		*(outMsg++) = 'S';
		outMsg = to4ha(outMsg, param.timeBuzzerAlarmOn);
		outMsg = to4ha(outMsg, param.timeBuzzerAlarmOff);
		outMsg = to4ha(outMsg, param.timeBuzzerTestOn);
		outMsg = to4ha(outMsg, param.timeBuzzerTestOff);
		outMsg = to4ha(outMsg, param.timeBuzzerCmdeOn);
		outMsg = to4ha(outMsg, param.timeBuzzerCmdeOff);
		outMsg = to4ha(outMsg, param.timeBuzzerCmdeNum);
 		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_VRM_SET_PARAM_TIME_BUZZER:
		if (strlen(inMsg) != (4 * 7))
		{
			*(outMsg++) = 'E';
			*(outMsg++) = '\0';
			return true;
		}
		inMsg = from4ha(inMsg, &param.timeBuzzerAlarmOn);
		inMsg = from4ha(inMsg, &param.timeBuzzerAlarmOff);
		inMsg = from4ha(inMsg, &param.timeBuzzerTestOn);
		inMsg = from4ha(inMsg, &param.timeBuzzerTestOff);
		inMsg = from4ha(inMsg, &param.timeBuzzerCmdeOn);
		inMsg = from4ha(inMsg, &param.timeBuzzerCmdeOff);
		inMsg = from4ha(inMsg, &param.timeBuzzerCmdeNum);
		*(outMsg++) = 'S';
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_VRM_GET_STATE_VOLET:
		*(outMsg++) = 'S';
		outMsg = to2ha(outMsg, stateVRM);
		outMsg = to2ha(outMsg, physicalStateVRM);
		outMsg = to4ha(outMsg, lastMovingTimeVRM);
		outMsg = to4ha(outMsg, targetPhysicalPositionVRM);
		outMsg = to4ha(outMsg, getAbsolutePositionVRM());
        outMsg = to4ha(outMsg, getStretchPositionVRM());
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		// CSBVV
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_VRM_EXEC_CMDE_VOLET:
		if (strlen(inMsg) != 6)
		{
			*(outMsg++) = 'E';
			*(outMsg++) = '\0';
			return true;
		}
		else
		{
			uint8_t cmde = (*(inMsg++));
			bool stop = (*(inMsg++) == 'S');
			bool buzz = (*(inMsg++) == 'B');
			bool stretch = (*(inMsg++) == 'S');
			uint8_t value; inMsg = from2ha(inMsg, &value);
			*(outMsg++) = 'S';
			outMsg = to2ha(outMsg, cmdeVoletVRM(cmde, stop, buzz, value, stretch));
			*(outMsg++) = '\0';
		}
		return true;
		//----------------------------------------------------------------
		case HOMEBUS_CMDE_VRM_INIT_STATE:
		uint8_t state;
		uint16_t position;
		inMsg = from2ha(inMsg, &state);
		inMsg = from4ha(inMsg, &position);
        setAbsolutePositionVRM(state, position);
		*(outMsg++) = 'S';
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
		default:
		*(outMsg++) = 'X';
		*(outMsg++) = '\0';
		return true;
		//----------------------------------------------------------------
	}
	//--------------------------------------------------------------------
}
//************************************************************************
// Fin de listing
//************************************************************************
