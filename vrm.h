#ifndef VRM_H
#define VRM_H
//************************************************************************
#include <stdint.h>
//************************************************************************
// Etiquettes pour le Volet Roulant Motorisé
//************************************************************************
#define VRM_RELAY_1					PORTB3
#define VRM_RELAY_2					PORTB2
//------------------------------------------------------------------------
#define VRM_RELAY_STOP				0
#define VRM_RELAY_UP				1
#define VRM_RELAY_DOWN				2
//------------------------------------------------------------------------
#define VRM_DIR_UP					0
#define VRM_DIR_DOWN				1
//------------------------------------------------------------------------
#define VRM_POS_UNKNOWN				0
#define VRM_POS_FULL_CLOSED			1
#define VRM_POS_OPENED				2
#define VRM_POS_FULL_OPENED			3
//------------------------------------------------------------------------
#define VRM_POS_PHYS_MIN			0
#define VRM_POS_PHYS_MAX			1000
#define VRM_POS_PHYS_HYST			10
//------------------------------------------------------------------------
#define VRM_CMD_STOP				's'
#define VRM_CMD_FULL_UP				'u'
#define VRM_CMD_FULL_DOWN			'd'
#define VRM_CMD_FORCE_UP			'U'
#define VRM_CMD_FORCE_DOWN			'D'
#define VRM_CMD_VAL					'V'
//------------------------------------------------------------------------
#define VRM_ST_STOPPED				0
#define VRM_ST_CMDE_EXT				1
#define VRM_ST_CMDE_START_IVR		2
#define VRM_ST_WAIT_IVR				3
#define VRM_ST_BUTEE_IVR		    4
#define VRM_ST_MOVING_IVR			5
#define VRM_ST_CMDE_START_FORCE		6
#define VRM_ST_MOVING_FORCE			7
#define VRM_ST_CMDE_STOP			8
//------------------------------------------------------------------------
#define VRM_DETECT_FALLDOWN			0		// Detection du courant sur front descendant
#define VRM_DETECT_RISEUP			1		// Detection du courant sur front montant
//************************************************************************
// Fonctions et variables
//************************************************************************
extern uint8_t		stateVRM;
extern uint8_t		physicalStateVRM;
extern uint16_t		lastMovingTimeVRM;
extern uint16_t     targetPhysicalPositionVRM;
//------------------------------------------------------------------------
extern volatile uint16_t timerMsVRM;
//------------------------------------------------------------------------
void		initVRM();
bool		cmdeVoletVRM(uint8_t command, bool autostop, bool buzz = false, uint8_t target = 0, bool stretch = false);
void        setTimerMsValueVRM(uint16_t time, bool reset = false);
//uint16_t    getFullTimeToMoveVRM(uint16_t margin, bool force = false);
//uint8_t		getCommandRelayVRM();
uint16_t    getAbsolutePositionVRM();
uint16_t	getStretchPositionVRM();
void        setAbsolutePositionVRM(uint8_t state, uint16_t position);
void		controlVRM();
//************************************************************************
// Fin de listing
//************************************************************************
#endif
