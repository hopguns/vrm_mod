#include <avr/io.h>
#include "vrm.h"
#include "ihm.h"
#include "timer.h"
#include "analog.h"
#include "eeparam.h"
#include "homeBusFunc.h"
//************************************************************************
// Gestion du VRM (Volet Roulant Motorisé)
//************************************************************************
//#define VRM_DEBUG
//------------------------------------------------------------------------
volatile uint16_t	timerMsVRM;
uint16_t	startValTimerMsVRM;
//------------------------------------------------------------------------
uint16_t	currentMovingTimeVRM;
uint16_t	lastMovingTimeVRM;
uint16_t	lastPhysicalPositionVRM;
uint16_t	targetPhysicalPositionVRM;
//------------------------------------------------------------------------
uint16_t	seuilDetectVRM;
uint8_t		indexDetectVRM;
uint8_t		stateDetectVRM;
//------------------------------------------------------------------------
uint8_t		directionVRM;
uint8_t		stateVRM;
bool		suspendMovingVRM;
bool		modeStretchUpVRM; // si true, considère le temps de l'étirement pour le %
uint8_t		physicalStateVRM;
//************************************************************************
// Commande des relais de commande du VRM
//************************************************************************
void cmdeRelayVRM(char cmd)
{
	//--------------------------------------------------------------------
	switch(cmd)
	{
		//----------------------------------------------------------------
		case VRM_RELAY_STOP:
			PORTB &= ~((1 << VRM_RELAY_2) | (1 << VRM_RELAY_1)); // Rel1 et Rel2 ouverts
		break;
		//----------------------------------------------------------------
		case VRM_RELAY_UP:
			PORTB |= ((1 << VRM_RELAY_1)); // Rel1 fermé
			PORTB &= ~((1 << VRM_RELAY_2)); // Rel2 ouvert
		break;
		//----------------------------------------------------------------
		case VRM_RELAY_DOWN:
			PORTB |= ((1 << VRM_RELAY_2) | (1 << VRM_RELAY_1)); // Rel1 et Rel2 fermés
		break;
		//----------------------------------------------------------------
		default:
			PORTB &= ~((1 << VRM_RELAY_2) | (1 << VRM_RELAY_1)); // Rel1 et Rel2 ouverts
		break;
		//----------------------------------------------------------------
	}
	//--------------------------------------------------------------------
}
//************************************************************************
// Lit l'état des relais de commande du VRM
//************************************************************************
uint8_t getStateRelayVRM()
{
	//--------------------------------------------------------------------
	if ((PINB & VRM_RELAY_1) == 0)
	{
		if ((PINB & VRM_RELAY_2) == 0) return VRM_RELAY_STOP;
		else return VRM_RELAY_UP;
	}
	else
	{
		if ((PINB & VRM_RELAY_2) == 0) return VRM_RELAY_STOP;
		else return VRM_RELAY_DOWN;
	}
	//--------------------------------------------------------------------
}
//************************************************************************
// convertit une position de absolute à stretch
//************************************************************************
uint16_t convertAbsoluteToStretchPositionVRM(uint16_t absolute)
{
	//--------------------------------------------------------------------
    // Calcul du % correspondant à la position étirée
	//--------------------------------------------------------------------
    uint16_t seuil = (uint16_t)(((uint32_t)param.timeStretchUp * VRM_POS_PHYS_MAX) / param.timeFullUp);
    if (VRM_POS_PHYS_MAX >= seuil) return absolute;
	//--------------------------------------------------------------------
    if (absolute < seuil) return VRM_POS_PHYS_MIN; // Tant que le temps est en dessous du seuil, la position est à 0
	//--------------------------------------------------------------------
    return (uint16_t)((uint32_t)(absolute - seuil) * VRM_POS_PHYS_MAX / (VRM_POS_PHYS_MAX - seuil));  // Absolute => Stretch   
	//--------------------------------------------------------------------
}
//************************************************************************
// convertit une position stretch à absolute
//************************************************************************
uint16_t convertStretchToAbsolutePositionVRM(uint16_t stretch)
{
	//--------------------------------------------------------------------
    // Calcul du % correspondant à la position étirée
	//--------------------------------------------------------------------
    uint16_t seuil = (uint16_t)(((uint32_t)param.timeStretchUp * VRM_POS_PHYS_MAX) / param.timeFullUp);
    if (VRM_POS_PHYS_MAX >= seuil) return stretch;
	//--------------------------------------------------------------------
    // La posiiton stretch à zéro correspond au seuil
	//--------------------------------------------------------------------
    return (uint32_t)((uint32_t)stretch *  (VRM_POS_PHYS_MAX - seuil) / VRM_POS_PHYS_MAX + seuil); // Stretch => Absolute
	//--------------------------------------------------------------------
}
//************************************************************************
// Traite la commande du VRM
//************************************************************************
bool cmdeVoletVRM(uint8_t command, bool autostop, bool buzz /*= false*/, uint8_t target /*= 0*/, bool stretch /*= false*/)
{
	//--------------------------------------------------------------------
	if (stateVRM != VRM_ST_STOPPED)
	{
		if (autostop)
		{
			suspendMovingVRM = true;
			if (buzz) stBuzzerStd = BUZZER_COMMAND ;
			return true;
		}
		else return false;
	}
	//--------------------------------------------------------------------
	uint16_t value = target * 10;
    if (stretch) value = convertStretchToAbsolutePositionVRM(value);
	//--------------------------------------------------------------------
	switch(command)
	{
		//----------------------------------------------------------------
		case VRM_CMD_STOP:
		stateVRM = VRM_ST_CMDE_STOP;
		setLedIHM(LED_RED, LED_OFF);// En cas d'erreur avant, reinit de la led
        addEventHomeBus(HOMEBUS_EVENT_VRM_CMD_STOP);
		break;
		//----------------------------------------------------------------
		case VRM_CMD_FULL_UP:
		directionVRM = VRM_DIR_UP;
		stateVRM = VRM_ST_CMDE_START_IVR;
		targetPhysicalPositionVRM = VRM_POS_PHYS_MAX;
        addEventHomeBus(HOMEBUS_EVENT_VRM_CMD_FULL_UP);
		break;
		//----------------------------------------------------------------
		case VRM_CMD_FULL_DOWN:
		directionVRM = VRM_DIR_DOWN;
		stateVRM = VRM_ST_CMDE_START_IVR;
		targetPhysicalPositionVRM = VRM_POS_PHYS_MIN;
        addEventHomeBus(HOMEBUS_EVENT_VRM_CMD_FULL_DOWN);
		break;
		//----------------------------------------------------------------
		case VRM_CMD_FORCE_UP:
		directionVRM = VRM_DIR_UP;
		stateVRM = VRM_ST_CMDE_START_FORCE;
		targetPhysicalPositionVRM = VRM_POS_PHYS_MAX;
        addEventHomeBus(HOMEBUS_EVENT_VRM_CMD_FORCE_UP);
		break;
		//----------------------------------------------------------------
		case VRM_CMD_FORCE_DOWN:
		directionVRM = VRM_DIR_DOWN;
		stateVRM = VRM_ST_CMDE_START_FORCE;
		targetPhysicalPositionVRM = VRM_POS_PHYS_MIN;
        addEventHomeBus(HOMEBUS_EVENT_VRM_CMD_FULL_DOWN);
		break;
		//----------------------------------------------------------------
		case VRM_CMD_VAL:
		if (value >= (lastPhysicalPositionVRM + VRM_POS_PHYS_HYST))
		{
			directionVRM = VRM_DIR_UP;
			stateVRM = VRM_ST_CMDE_START_IVR;
			targetPhysicalPositionVRM = value;
            addEventHomeBus(HOMEBUS_EVENT_VRM_CMD_VAL_UP);
		}
		else
		{
			if ((value + VRM_POS_PHYS_HYST) <= lastPhysicalPositionVRM)
			{
				directionVRM = VRM_DIR_DOWN;
				stateVRM = VRM_ST_CMDE_START_IVR;
				targetPhysicalPositionVRM = value;
                addEventHomeBus(HOMEBUS_EVENT_VRM_CMD_VAL_DOWN);
			}
			else
			{
				return false;
			}
		}
		break;
		//----------------------------------------------------------------
		default:
		return false;
		break;
		//----------------------------------------------------------------
	}
	//--------------------------------------------------------------------
	if (buzz) cmdeBuzzerIHM(param.timeBuzzerCmdeOn, param.timeBuzzerCmdeOff, param.timeBuzzerCmdeNum);
	//--------------------------------------------------------------------
	return true;
	//--------------------------------------------------------------------
}
//************************************************************************
// Definit le sens de detection du courant et le nombre de mesures
//************************************************************************
void setIvrDectectDirVRM(uint8_t state)
{
	//--------------------------------------------------------------------
	stateDetectVRM = state;
	indexDetectVRM = param.nbMesIvrDetect;
	if (stateDetectVRM == VRM_DETECT_RISEUP) seuilDetectVRM = param.valIvrCurrentOn;
	else seuilDetectVRM = param.valIvrCurrentOff;
 	//--------------------------------------------------------------------
}
//************************************************************************
// Calcul le temps (max) de mouvement du volet
//************************************************************************
uint16_t getFullTimeToMoveVRM(uint16_t margin, bool force)
{
	//--------------------------------------------------------------------
	if (directionVRM == VRM_DIR_UP)
	{
		if ((physicalStateVRM == VRM_POS_UNKNOWN) || force)
		{
			return margin + param.timeFullUp;
		}
		if (targetPhysicalPositionVRM > lastPhysicalPositionVRM)
		{
			return (uint16_t)(((uint32_t)param.timeFullUp * (targetPhysicalPositionVRM - lastPhysicalPositionVRM)) / VRM_POS_PHYS_MAX);
		}
		return (uint16_t)(((uint32_t)param.timeFullUp * VRM_POS_PHYS_HYST) / VRM_POS_PHYS_MAX);
	}
	//--------------------------------------------------------------------
	if ((physicalStateVRM == VRM_POS_UNKNOWN) || force)
	{
		return margin + param.timeFullDown;
	}
	if (targetPhysicalPositionVRM < lastPhysicalPositionVRM)
	{
		return (uint16_t)(((uint32_t)param.timeFullDown * (lastPhysicalPositionVRM - targetPhysicalPositionVRM)) / VRM_POS_PHYS_MAX);
	}
	return (uint16_t)(((uint32_t)param.timeFullDown * VRM_POS_PHYS_HYST) / VRM_POS_PHYS_MAX);
	//--------------------------------------------------------------------
}
//************************************************************************
// Calcul si le courant à franchi le seuil sur un nombre de mesure
//************************************************************************
void callbackMesureIvrVRM(void)
{
	//--------------------------------------------------------------------
	if (((mesureADC[ADC_IVR_ID] > seuilDetectVRM) && (stateDetectVRM == VRM_DETECT_RISEUP)) ||
	((mesureADC[ADC_IVR_ID] < seuilDetectVRM) && (stateDetectVRM == VRM_DETECT_FALLDOWN)))
	{
		if (indexDetectVRM > 0) indexDetectVRM--;
	}
	else
	{
		indexDetectVRM = param.nbMesIvrDetect;
	}
 	//--------------------------------------------------------------------
}
//************************************************************************
// 
//************************************************************************
uint16_t getCurrentMovingTimeVRM()
{
	//--------------------------------------------------------------------
	if (stateVRM == VRM_ST_STOPPED)
	{
		return lastMovingTimeVRM;
	}
	return currentMovingTimeVRM + (startValTimerMsVRM - timerMsVRM);
 	//--------------------------------------------------------------------
}
//************************************************************************
// position absolue en pour mille (selon de temps de mouvement)
//************************************************************************
uint16_t getAbsolutePositionVRM()
{
	uint16_t value;
	//--------------------------------------------------------------------
	if (stateVRM == VRM_ST_STOPPED)
	{
		if (physicalStateVRM == VRM_POS_FULL_CLOSED) return VRM_POS_PHYS_MIN;
		if (physicalStateVRM == VRM_POS_FULL_OPENED) return VRM_POS_PHYS_MAX;
		if (physicalStateVRM == VRM_POS_OPENED)
        {
            return lastPhysicalPositionVRM;
        }
		return VRM_POS_PHYS_MAX / 2; // Par défaut on dit que c'est ouvert à moitier
	}
 	//--------------------------------------------------------------------
	if (directionVRM == VRM_DIR_UP)
 	{
		value = lastPhysicalPositionVRM + (uint16_t)((VRM_POS_PHYS_MAX * (uint32_t)getCurrentMovingTimeVRM()) / param.timeFullUp);
		if (value < VRM_POS_PHYS_MAX)
        {
            return value;
        }
		return VRM_POS_PHYS_MAX;
 	}
 	//--------------------------------------------------------------------
	value = (uint16_t)((VRM_POS_PHYS_MAX * (uint32_t)getCurrentMovingTimeVRM()) / param.timeFullDown);
	if (lastPhysicalPositionVRM > value)
    {
        value = lastPhysicalPositionVRM - value;
        return value;
    }
	return VRM_POS_PHYS_MIN;
 	//--------------------------------------------------------------------
}
//************************************************************************
// position en pour mille selon le mode (Stretch ou Absolute)
//************************************************************************
uint16_t getStretchPositionVRM()
{
 	//--------------------------------------------------------------------
    if (modeStretchUpVRM) return convertAbsoluteToStretchPositionVRM(getAbsolutePositionVRM());
    return getAbsolutePositionVRM();
 	//--------------------------------------------------------------------
}
//************************************************************************
// position en pour mille
//************************************************************************
void setAbsolutePositionVRM(uint8_t state, uint16_t position)
{
	physicalStateVRM = state;
	//--------------------------------------------------------------------
	if (physicalStateVRM == VRM_POS_FULL_CLOSED) lastPhysicalPositionVRM = VRM_POS_PHYS_MIN;
	if (physicalStateVRM == VRM_POS_FULL_OPENED) lastPhysicalPositionVRM = VRM_POS_PHYS_MAX;
	if (physicalStateVRM == VRM_POS_OPENED) lastPhysicalPositionVRM = position;
 	//--------------------------------------------------------------------
}
//************************************************************************
// 
//************************************************************************
void setTimerMsValueVRM(uint16_t time, bool reset)
{
	//--------------------------------------------------------------------
	if (reset) currentMovingTimeVRM = 0;
	else currentMovingTimeVRM = getCurrentMovingTimeVRM();
	//--------------------------------------------------------------------
	startValTimerMsVRM = timerMsVRM = time;
 	//--------------------------------------------------------------------
}
//************************************************************************
// Initialisation des relais pour le VRM
//************************************************************************
void initVRM()
{
    //--------------------------------------------------------------------
    stateVRM = VRM_ST_STOPPED;
    directionVRM = VRM_DIR_DOWN;
    physicalStateVRM = VRM_POS_UNKNOWN;
	suspendMovingVRM = false;
    modeStretchUpVRM = false;
    //--------------------------------------------------------------------
    currentMovingTimeVRM = 0;
	startValTimerMsVRM = 0;
	lastMovingTimeVRM = 0;
    timerMsVRM = 0;
	//--------------------------------------------------------------------
	targetPhysicalPositionVRM = VRM_POS_PHYS_MIN;
	lastPhysicalPositionVRM = VRM_POS_PHYS_MIN;
	//--------------------------------------------------------------------
	setIvrDectectDirVRM(VRM_DETECT_RISEUP);
	//--------------------------------------------------------------------
	// Pin de controle du Relai 1 = PB3 
	// Pin de controle du Relai 2 = PB2
	//--------------------------------------------------------------------
	DDRB |= ((1 << VRM_RELAY_1) | (1 << VRM_RELAY_2));
	PORTB &= ~((1 << VRM_RELAY_1) | (1 << VRM_RELAY_2)); // Rel1 et Rel2 ouverts
	//--------------------------------------------------------------------
    setCallbackADC(ADC_IVR_ID, callbackMesureIvrVRM);
	//--------------------------------------------------------------------
}
//************************************************************************
// Controle du processus du VRM
//************************************************************************
void controlVRM()
{
	//---------------------------------------------------------------------
	switch(stateVRM)
	{
		//-------------------------------------------------------------------
		// Etat à l'arret. Monitoring du courant (inter à bascule forcé ou panne)
		//-------------------------------------------------------------------
		case VRM_ST_STOPPED:
		if (indexDetectVRM == 0)
		{
			setLedIHM(LED_GREEN, LED_ON);
			setLedIHM(LED_RED, LED_BLINK);
			setIvrDectectDirVRM(VRM_DETECT_FALLDOWN);
			stateVRM = VRM_ST_CMDE_EXT;
            addEventHomeBus(HOMEBUS_EVENT_VRM_EXT_CMDE);
		}
		break;
		//-------------------------------------------------------------------
		// Etat Commande Ext. Attend le retour à zéro du courant ...
		//-------------------------------------------------------------------
		case VRM_ST_CMDE_EXT:
		if (indexDetectVRM == 0)
		{
			setLedIHM(LED_GREEN, LED_OFF);
			setLedIHM(LED_RED, LED_OFF);
			setIvrDectectDirVRM(VRM_DETECT_RISEUP);
			stateVRM = VRM_ST_STOPPED;
		}
		break;
		//-------------------------------------------------------------------
		// Etat démarrage monitoré IVR. 
		//-------------------------------------------------------------------
		case VRM_ST_CMDE_START_IVR:
		if (directionVRM == VRM_DIR_UP)
		{
			cmdeRelayVRM(VRM_RELAY_UP);
		}
		else
		{
			cmdeRelayVRM(VRM_RELAY_DOWN);
		}
		setTimerMsValueVRM(param.timeMaxWaitIvr, true);
		setIvrDectectDirVRM(VRM_DETECT_RISEUP);
		stateVRM = VRM_ST_WAIT_IVR;
		break;
		//-------------------------------------------------------------------
		// Etat Attente présence courant IVR. 
		//-------------------------------------------------------------------
		case VRM_ST_WAIT_IVR:
		if (indexDetectVRM == 0)
		{
			// Le volet est alimenté et donc en mouvement
			setLedIHM(LED_GREEN, LED_BLINK);
			setIvrDectectDirVRM(VRM_DETECT_FALLDOWN);
			setTimerMsValueVRM(getFullTimeToMoveVRM(param.timeIvrMargin, false), true);
			stateVRM = VRM_ST_MOVING_IVR;
			break;
		}
		else if (timerMsVRM == 0)
		{
			// l'alimentation du volet n'a pas été détéctée
			setLedIHM(LED_GREEN, LED_BLINK, 150, 300);
			setTimerMsValueVRM(param.timeShowButee);
			stateVRM = VRM_ST_BUTEE_IVR;
            addEventHomeBus(HOMEBUS_EVENT_VRM_BUTEE);
		}
		break;
		//-------------------------------------------------------------------
		// Etat butée en monitoring (pas de courant donc VRM en butée)
		//-------------------------------------------------------------------
		case VRM_ST_BUTEE_IVR:
		if (timerMsVRM == 0)
		{
			stateVRM = VRM_ST_CMDE_STOP;
		}
		break;
		//-------------------------------------------------------------------
		// Etat mouvement VRM avec monotoring du courant IVR
		//-------------------------------------------------------------------
		case VRM_ST_MOVING_IVR:
		if (indexDetectVRM == 0)
		{
			// Le volet n'est plus alimenté et donc arreté theoriquement en butée
			if (directionVRM == VRM_DIR_UP)
			{
				physicalStateVRM = VRM_POS_FULL_OPENED;
				lastPhysicalPositionVRM = VRM_POS_PHYS_MAX;
			}
			else
			{
				physicalStateVRM = VRM_POS_FULL_CLOSED;
				lastPhysicalPositionVRM = VRM_POS_PHYS_MIN;
			}
			setTimerMsValueVRM(0); // Permet de mettre à jour currentMovingTimeVRM sans attendre
			lastMovingTimeVRM = getCurrentMovingTimeVRM(); //avant stateVRM = VRM_ST_STOPPED; !
			stateVRM = VRM_ST_CMDE_STOP;
			break; // On n'execute pas les cas suivants
		}
		if (timerMsVRM == 0) 
		{
			if ((targetPhysicalPositionVRM == VRM_POS_PHYS_MIN) || (targetPhysicalPositionVRM == VRM_POS_PHYS_MAX))
			{
				// Le volet est toujours alimenté aprés la durée max !!!
				setLedIHM(LED_RED, LED_BLINK);
				stateVRM = VRM_ST_CMDE_STOP;
				physicalStateVRM = VRM_POS_UNKNOWN;
				lastPhysicalPositionVRM = VRM_POS_PHYS_MAX / 2;
                addEventHomeBus(HOMEBUS_EVENT_VRM_TOO_LONG);
			}
			else
			{
				// On arrive au temps de mouvement spécifié
				setTimerMsValueVRM(0); // Permet de mettre à jour currentMovingTimeVRM sans attendre
				lastMovingTimeVRM = getCurrentMovingTimeVRM(); //avant stateVRM = VRM_ST_STOPPED; !
				lastPhysicalPositionVRM = getAbsolutePositionVRM();
				physicalStateVRM = VRM_POS_OPENED;
				stateVRM = VRM_ST_CMDE_STOP;
			}
			break; // On n'execute pas les cas suivants
		}
		if (suspendMovingVRM)
		{
			suspendMovingVRM = false;
			setTimerMsValueVRM(0); // Permet de mettre à jour currentMovingTimeVRM sans attendre
			lastMovingTimeVRM = getCurrentMovingTimeVRM(); //avant stateVRM = VRM_ST_STOPPED; !
			lastPhysicalPositionVRM = getAbsolutePositionVRM();
			physicalStateVRM = VRM_POS_OPENED;
			stateVRM = VRM_ST_CMDE_STOP;
		}
		break;
		//-------------------------------------------------------------------
		// Etat démarrage forcé (sans monitoring)
		//-------------------------------------------------------------------
		case VRM_ST_CMDE_START_FORCE:
		if (directionVRM == VRM_DIR_UP)
		{
			cmdeRelayVRM(VRM_RELAY_UP);
		}
		else
		{
			cmdeRelayVRM(VRM_RELAY_DOWN);
		}
		setLedIHM(LED_GREEN, LED_ON);
		setTimerMsValueVRM(getFullTimeToMoveVRM(param.timeForceMargin, true), true);
		stateVRM = VRM_ST_MOVING_FORCE;
		physicalStateVRM = VRM_POS_UNKNOWN;
		break;
		//-------------------------------------------------------------------
		// Etat mouvement du VRM forcé (sans monitoring)
		//-------------------------------------------------------------------
		case VRM_ST_MOVING_FORCE:
		if (timerMsVRM == 0)
		{
			stateVRM = VRM_ST_CMDE_STOP;
		}
		if (suspendMovingVRM)
		{
			suspendMovingVRM = false;
			stateVRM = VRM_ST_CMDE_STOP;
		}
		break;
		//-------------------------------------------------------------------
		// Etat de commande d'arrêt du VRM. Memorisation des positions
		//-------------------------------------------------------------------
		case VRM_ST_CMDE_STOP:
		cmdeRelayVRM(VRM_RELAY_STOP);
		stateVRM = VRM_ST_STOPPED;
		setLedIHM(LED_GREEN, LED_OFF);
		setIvrDectectDirVRM(VRM_DETECT_RISEUP);
        addEventHomeBus(HOMEBUS_EVENT_VRM_STOP_RELAY);
		break;
		//-------------------------------------------------------------------
	}
	//---------------------------------------------------------------------
}
//************************************************************************
// Fin de listing
//************************************************************************
