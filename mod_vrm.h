#ifndef DEFINES_VRM_H
#define DEFINES_VRM_H
//************************************************************************
//#include "defines.h"
//************************************************************************
#define MSG_GET_GENERAL_STATUS_VRM			(0 + MSG_BASE_LEN)
#define MSG_SAVE_ALL_PARAM_VRM				(1 + MSG_BASE_LEN)
#define MSG_LOAD_ALL_PARAM_VRM				(2 + MSG_BASE_LEN)
#define MSG_EXEC_CMDE_VOLET_VRM				(3 + MSG_BASE_LEN)
#define MSG_GET_STATE_VOLET_VRM				(4 + MSG_BASE_LEN)
#define MSG_GET_ANALOG_MESURES_VRM			(5 + MSG_BASE_LEN)
#define MSG_GET_PARAM_TIME_VOLET_VRM		(6 + MSG_BASE_LEN)
#define MSG_SET_PARAM_TIME_VOLET_VRM		(7 + MSG_BASE_LEN)
#define MSG_GET_PARAM_TIME_BUZZER_VRM		(8 + MSG_BASE_LEN)
#define MSG_SET_PARAM_TIME_BUZZER_VRM		(9 + MSG_BASE_LEN)
#define MSG_VRM_LEN							(10 + MSG_BASE_LEN)
//------------------------------------------------------------------------
#define EVENT_VRM_MOVE_START			(0 + EVENT_BASE_LEN)
#define EVENT_VRM_MOVE_STOP				(1 + EVENT_BASE_LEN)
#define EVENT_VRM_MOVE_TIMEOUT			(2 + EVENT_BASE_LEN)
#define EVENT_VRM_MOVE_SUSPEND			(3 + EVENT_BASE_LEN)
//------------------------------------------------------------------------
#define VRM_ST_STOPPED					0
#define VRM_ST_CMDE_EXT					1
#define VRM_ST_CMDE_START_IVR			2
#define VRM_ST_WAIT_IVR					3
#define VRM_ST_MOVING_IVR				4
#define VRM_ST_CMDE_START_FORCE			5
#define VRM_ST_MOVING_FORCE				6
#define VRM_ST_CMDE_BUTEE				7
#define VRM_ST_CMDE_STOP				8
#define VRM_ST_CMDE_SUSPEND				9
//------------------------------------------------------------------------
// #define VRM_MOVE_UNKNOWN				0
// #define VRM_MOVE_EXTREMITY				0x01
// #define VRM_MOVE_FULL_UP				0x02
// #define VRM_MOVE_FULL_DOWN				0x04
// #define VRM_MOVE_KNOWN					(VRM_MOVE_EXTREMITY | VRM_MOVE_FULL_UP | VRM_MOVE_FULL_DOWN)
//************************************************************************
// Fin de listing
//************************************************************************
#endif
