#include "alarm.h"
#include "analog.h"
//************************************************************************
// Gestion du VRM (Volet Roulant Motorisé)
//************************************************************************
//#define VRM_DEBUG
//------------------------------------------------------------------------
unsigned long timeAlarm;
unsigned int nbDetectIn1Alarm, nbDetectIn2Alarm;
uint8_t stateAlarm, stateIn1Alarm, stateIn2Alarm;
//************************************************************************
// Initialisation des relais pour le VRM
//************************************************************************
void initAlarm(uint8_t option)
{
    //--------------------------------------------------------------------
    timeAlarm = 0;
    nbDetectIn1Alarm = ALARM_NB_MES_DETECT;
    nbDetectIn2Alarm = ALARM_NB_MES_DETECT;
    stateIn1Alarm = ALARM_LEVEL0;
    stateIn2Alarm = ALARM_LEVEL0;
	stateAlarm = ALARM_ST_NORMAL;
	//--------------------------------------------------------------------
    if (option)
	{
	    stateAlarm = ALARM_ST_UNKNOWN;
		setCallbackADC(ADC_AL1_ID, callbackIn1Alarm);
		setCallbackADC(ADC_AL2_ID, callbackIn2Alarm);
	}
    //--------------------------------------------------------------------
}
//************************************************************************
// Calcul le niveau de l'entrée alarme 1
//************************************************************************
void callbackIn1Alarm(void)
{
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL1_ID] < ALARM_1_L1_L)
	{
		if (stateIn1Alarm != ALARM_LEVEL0) nbDetectIn1Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn1Alarm > 0) nbDetectIn1Alarm--;
		stateIn1Alarm = ALARM_LEVEL0;
		return;
	}
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL1_ID] < ALARM_1_L1_H)
	{
		if (stateIn1Alarm != ALARM_LEVEL1) nbDetectIn1Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn1Alarm > 0) nbDetectIn1Alarm--;
		stateIn1Alarm = ALARM_LEVEL1;
		return;
	}
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL1_ID] < ALARM_1_L2_L)
	{
		if (stateIn1Alarm != ALARM_LEVEL2) nbDetectIn1Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn1Alarm > 0) nbDetectIn1Alarm--;
		stateIn1Alarm = ALARM_LEVEL2;
		return;
	}
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL1_ID] < ALARM_1_L2_H)
	{
		if (stateIn1Alarm != ALARM_LEVEL3) nbDetectIn1Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn1Alarm > 0) nbDetectIn1Alarm--;
		stateIn1Alarm = ALARM_LEVEL3;
		return;
	}
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL1_ID] < ALARM_1_L3_L)
	{
		if (stateIn1Alarm != ALARM_LEVEL4) nbDetectIn1Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn1Alarm > 0) nbDetectIn1Alarm--;
		stateIn1Alarm = ALARM_LEVEL4;
		return;
	}
	//--------------------------------------------------------------------
	if (stateIn1Alarm != ALARM_LEVEL5) nbDetectIn1Alarm = ALARM_NB_MES_DETECT;
	if (nbDetectIn1Alarm > 0) nbDetectIn1Alarm--;
	stateIn1Alarm = ALARM_LEVEL5;
	//--------------------------------------------------------------------
}
//************************************************************************
// Calcul le niveau de l'entrée alarme 2
//************************************************************************
void callbackIn2Alarm(void)
{
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL2_ID] < ALARM_2_L1_H)
	{
		if (stateIn2Alarm != ALARM_LEVEL0) nbDetectIn2Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn2Alarm > 0) nbDetectIn2Alarm--;
		stateIn2Alarm = ALARM_LEVEL0;
		return;
	}
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL2_ID] < ALARM_2_L2_L)
	{
		if (stateIn2Alarm != ALARM_LEVEL1) nbDetectIn2Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn2Alarm > 0) nbDetectIn2Alarm--;
		stateIn2Alarm = ALARM_LEVEL1;
		return;
	}
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL2_ID] < ALARM_2_L2_H)
	{
		if (stateIn2Alarm != ALARM_LEVEL2) nbDetectIn2Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn2Alarm > 0) nbDetectIn2Alarm--;
		stateIn2Alarm = ALARM_LEVEL2;
		return;
	}
	//--------------------------------------------------------------------
	if (mesureADC[ADC_AL2_ID] < ALARM_2_L3_L)
	{
		if (stateIn2Alarm != ALARM_LEVEL3) nbDetectIn2Alarm = ALARM_NB_MES_DETECT;
		if (nbDetectIn2Alarm > 0) nbDetectIn2Alarm--;
		stateIn2Alarm = ALARM_LEVEL3;
		return;
	}
	//--------------------------------------------------------------------
	if (stateIn2Alarm != ALARM_LEVEL4) nbDetectIn2Alarm = ALARM_NB_MES_DETECT;
	if (nbDetectIn2Alarm > 0) nbDetectIn2Alarm--;
	stateIn2Alarm = ALARM_LEVEL4;
	//--------------------------------------------------------------------
}
//************************************************************************
// Controle du processus du VRM
//************************************************************************
uint8_t getStateAlarm(void)
{
	//--------------------------------------------------------------------
	return stateAlarm;
	//--------------------------------------------------------------------
}
//************************************************************************
// Controle du processus du VRM
//************************************************************************
void controlAlarm(uint8_t option)
{
	//--------------------------------------------------------------------
	if ((nbDetectIn1Alarm == 0) && (nbDetectIn2Alarm == 0) && option)
	{
		switch(stateIn1Alarm)
		{
			//------------------------------------------------------------
			case ALARM_LEVEL0:
				stateAlarm = ALARM_ST_UNKNOWN;
			break;
			//------------------------------------------------------------
			case ALARM_LEVEL1: // Niveau 1
				if (stateIn2Alarm == ALARM_LEVEL0) stateAlarm = ALARM_ST_NORMAL;
				else if (stateIn2Alarm == ALARM_LEVEL2) stateAlarm = ALARM_ST_TROUBLE;
				else stateAlarm = ALARM_ST_UNKNOWN;
			break;
			//------------------------------------------------------------
			case ALARM_LEVEL2:
				stateAlarm = ALARM_ST_UNKNOWN;
			break;
			//------------------------------------------------------------
			case ALARM_LEVEL3: // Niveau 2
				if (stateIn2Alarm == ALARM_LEVEL0) stateAlarm = ALARM_ST_INTRUSION;
				else if (stateIn2Alarm == ALARM_LEVEL2) stateAlarm = ALARM_ST_MASK;
				else stateAlarm = ALARM_ST_UNKNOWN;
			break;
			//------------------------------------------------------------
			case ALARM_LEVEL4:
				stateAlarm = ALARM_ST_UNKNOWN;
			break;
			//------------------------------------------------------------
			case ALARM_LEVEL5: // Niveau 3
				if (stateIn2Alarm == ALARM_LEVEL0) stateAlarm = ALARM_ST_CASE;
				else stateAlarm = ALARM_ST_UNKNOWN;
			break;
			//------------------------------------------------------------
			default:
				stateAlarm = ALARM_ST_UNKNOWN;
			break;
			//------------------------------------------------------------
		}
	}
	//--------------------------------------------------------------------
}
//************************************************************************
// Fin de listing
//************************************************************************
