#ifndef HOMEBUS_VRM_H
#define HOMEBUS_VRM_H
//------------------------------------------------------------------------
#define HOMEBUS_TYPE_VRM                            'A'
//------------------------------------------------------------------------
#define HOMEBUS_EVENT_VRM_NONE                      0
#define HOMEBUS_EVENT_VRM_KEYS_MASK                 0x0F    // Au dessous de cette valeur, il s'agit de keys
//------------------------------------------------------------------------
#define HOMEBUS_EVENT_VRM_CMD_STOP                  0x10
#define HOMEBUS_EVENT_VRM_CMD_FULL_UP               0x11
#define HOMEBUS_EVENT_VRM_CMD_FULL_DOWN             0x12
#define HOMEBUS_EVENT_VRM_CMD_FORCE_UP              0x13
#define HOMEBUS_EVENT_VRM_CMD_FORCE_DOWN            0x14
#define HOMEBUS_EVENT_VRM_CMD_VAL_UP                0x15
#define HOMEBUS_EVENT_VRM_CMD_VAL_DOWN              0x16
#define HOMEBUS_EVENT_VRM_EXT_CMDE                  0x17
#define HOMEBUS_EVENT_VRM_BUTEE                     0x18
#define HOMEBUS_EVENT_VRM_TOO_LONG                  0x19
#define HOMEBUS_EVENT_VRM_STOP_RELAY                0x1A
//------------------------------------------------------------------------
#define HOMEBUS_EVENT_VRM_EEPROM_INIT               0x20
//------------------------------------------------------------------------
// Commandes communes à tous les types de module
//------------------------------------------------------------------------
#define HOMEBUS_CMDE_START_COMMON                   20
//------------------------------------------------------------------------
#define HOMEBUS_CMDE_RESET_CPU						(HOMEBUS_CMDE_START_COMMON + 0)
#define HOMEBUS_CMDE_GET_MODULE_TYPE				(HOMEBUS_CMDE_START_COMMON + 1)
#define HOMEBUS_CMDE_SET_MODULE_OPTIONS				(HOMEBUS_CMDE_START_COMMON + 2)
#define HOMEBUS_CMDE_GET_MODULE_OPTIONS				(HOMEBUS_CMDE_START_COMMON + 3)
#define HOMEBUS_CMDE_GET_TEMPERATURE				(HOMEBUS_CMDE_START_COMMON + 4)
#define HOMEBUS_CMDE_GET_ROM_DS18B20				(HOMEBUS_CMDE_START_COMMON + 5)
#define HOMEBUS_CMDE_SAVE_PARAMS					(HOMEBUS_CMDE_START_COMMON + 6)
#define HOMEBUS_CMDE_SAVE_CMDE_BUZZER				(HOMEBUS_CMDE_START_COMMON + 7)
//------------------------------------------------------------------------
// Commandes spécifiques à ce type de module
//------------------------------------------------------------------------
#define HOMEBUS_CMDE_START_SPECIFIC                 60
//------------------------------------------------------------------------
#define HOMEBUS_CMDE_VRM_GET_ANALOG_MESURES			(HOMEBUS_CMDE_START_SPECIFIC + 0)
#define HOMEBUS_CMDE_VRM_GET_PARAM_TIME_VOLET		(HOMEBUS_CMDE_START_SPECIFIC + 1)
#define HOMEBUS_CMDE_VRM_SET_PARAM_TIME_VOLET		(HOMEBUS_CMDE_START_SPECIFIC + 2)
#define HOMEBUS_CMDE_VRM_GET_PARAM_TIME_BUZZER		(HOMEBUS_CMDE_START_SPECIFIC + 3)
#define	HOMEBUS_CMDE_VRM_SET_PARAM_TIME_BUZZER		(HOMEBUS_CMDE_START_SPECIFIC + 4)
#define HOMEBUS_CMDE_VRM_GET_STATE_VOLET			(HOMEBUS_CMDE_START_SPECIFIC + 5)
#define HOMEBUS_CMDE_VRM_EXEC_CMDE_VOLET			(HOMEBUS_CMDE_START_SPECIFIC + 6)
#define HOMEBUS_CMDE_VRM_INIT_STATE     			(HOMEBUS_CMDE_START_SPECIFIC + 7)
//------------------------------------------------------------------------
#if ((HOMEBUS_CMDE_START_COMMON < 20) || (HOMEBUS_CMDE_START_SPECIFIC < 20))
Attention !!!
#endif
//------------------------------------------------------------------------
bool    addEventHomeBus(uint8_t event);
//------------------------------------------------------------------------
// Fin de listing
//------------------------------------------------------------------------
#endif
