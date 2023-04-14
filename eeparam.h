#ifndef EE_PARAM_H
#define EE_PARAM_H
//************************************************************************
#include <stdint.h>
//************************************************************************
#define EE_VERSION			3
//------------------------------------------------------------------------
// Structure des parametres
//------------------------------------------------------------------------
struct SParam
{
	//--------------------------------------------------------------------
	uint8_t		start;				// Début du paquet de paramètre = 0x55
	//--------------------------------------------------------------------
	uint16_t	version;
	//--------------------------------------------------------------------
	bool		optionAlarm;		// indique si un module alarm est connecté
	bool		optionDS18B20;		// indique si un capteur ds18b20 est connecté
	//--------------------------------------------------------------------
	uint16_t	tempoDS18B20;		// temps en sec entre chaque mesure
	//--------------------------------------------------------------------
	uint16_t	timeFullUp;			// Temps en ms d'ouverture complet
	uint16_t	timeFullDown;		// Temps en ms de fermeture complet
	uint16_t	timeMaxWaitIvr;		// temps en ms max entre la commande d'arrêt et la présence du courant
	uint16_t	timeStretchUp;		// temps en ms pour étirer le volet et voir les trou de lumière
	uint16_t	timeShowButee;		// temps en ms d'affiche de la butée
	uint16_t	timeIvrMargin;		// temps en ms de marge pour attendre la detection
	uint16_t	timeForceMargin;	// temps en ms de marge pour forcer la fermeture ou l'ouverture
	//--------------------------------------------------------------------
	uint16_t	nbMesIvrDetect;		// Nombre de points à détecter au dela du seuil pour valider l'état
	uint16_t	valIvrCurrentOff;	// Courant min en dessous duquel on considère être à l'arrêt
	uint16_t	valIvrCurrentOn;	// courant de seuil au dessus duquel on considére être en marche.
	//--------------------------------------------------------------------
	uint16_t	timeBuzzerAlarmOn;	// temps en ms du buzzer à ON
	uint16_t	timeBuzzerAlarmOff;	// temps en ms du buzzer à OFF
	uint16_t	timeBuzzerTestOn;	// temps en ms du buzzer à ON
	uint16_t	timeBuzzerTestOff;	// temps en ms du buzzer à OFF
	uint16_t	timeBuzzerCmdeOn;	// temps en ms du buzzer à ON
	uint16_t	timeBuzzerCmdeOff;	// temps en ms du buzzer à OFF
	uint16_t	timeBuzzerCmdeNum;	// Nombre de pulse buzzer
	//--------------------------------------------------------------------
	uint8_t		end;				// Fin du paquet de paramètre = 0xAA
	//---------------------------------------------------------------------
};
//------------------------------------------------------------------------
// Déclaration des fonctions
//------------------------------------------------------------------------
void	initParam();
bool	loadParam();
void	saveParam();
//------------------------------------------------------------------------
extern SParam param;
//************************************************************************
// Fin de listing
//************************************************************************
#endif
