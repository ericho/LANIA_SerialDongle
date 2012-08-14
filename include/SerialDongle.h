
/****************************************************************
Includes  
****************************************************************/

#include <zdo.h> 
#include <configServer.h> 
#include <taskManager.h>
#include <aps.h> 
#include <mac.h> 
#include "leds.h"
#include <stdio.h>

/****************************************************************
General definitions
****************************************************************/
#define LANIA_ENDPOINT 1
#define LANIA_PROFILE_ID CCPU_TO_LE16(1)
#define LANIA_DEVICE_ID CCPU_TO_LE16(1)
#define LANIA_DEVICE_VERSION 1


typedef enum
  {
    INIT_STATE, 
    STARTING_NETWORK_STATE,
    IN_NETWORK_STATE,
    STOP_STATE
  }AppState_t;

typedef struct
{
  uint8_t lqi;
  int8_t rssi;
  ShortAddr_t shortAddr;
}laniaTest_t;

typedef struct
{
  uint8_t header[APS_ASDU_OFFSET];
  laniaTest_t data;
  uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET];
}laniaRequest_t;

void InitUsart(void);
void changeGlobalTask(void);
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);
void APS_DataIndCoord(APS_DataInd_t *InData);


extern SimpleDescriptor_t networkDescriptor;


void startVisualizer(void);
void showStartingNwk(void);
void showStartedNwk(void);
void showAirTxStart(void);
void showAirTxStop(void);
void showSerialTx(void);
void showAirRxStart(void);
void stopVisualizer(void);
void stopVisualizer(void);
