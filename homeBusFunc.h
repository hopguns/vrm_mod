#ifndef HOMEBUS_FUNC_H
#define HOMEBUS_FUNC_H
//************************************************************************
#include "homeBus.h"
#include "homeBusVrm.h"
//************************************************************************
// defines
//************************************************************************
#define HOMEBUS_CMDE_DEFAULT                        0
#define HOMEBUS_CMDE_SET_MOD_ADDR                   1
//************************************************************************
// Functions and variables
//************************************************************************
void initFonctHomeBus(void);
bool manageFonctHomeBus(char *inMsg, char *outMsg, uint8_t *origAddr, uint8_t *destAddr, uint8_t *ctrlId);
//************************************************************************
// Fin de listing
//************************************************************************
#endif
