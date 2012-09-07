
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

#define APP_RX_BUFFER_SIZE 270

typedef enum
{
    INIT_STATE, 
    STARTING_NETWORK_STATE,
    IN_NETWORK_STATE,
    ANALIZE_MESSAGE_STATE,
    PROCESS_MESSAGE,      
    SENDING_MESSAGE_STATE,
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

typedef struct s_data_command
{
    uint8_t id_message;
    uint8_t command;
    uint8_t data_length;
    uint8_t *data;
    struct s_data_command *next_command;
}s_data_command;

void init_usart(void);
void changeGlobalTask(void);
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);
void APS_DataIndCoord(APS_DataInd_t *InData);

static AppState_t stateApp = INIT_STATE;
extern SimpleDescriptor_t networkDescriptor;

uint8_t offset_rx_buffer;

uint8_t app_rx_buffer[APP_RX_BUFFER_SIZE];
bool usart_tx_busy;
bool new_data_on_buffer;
char str[100];
uint8_t commands_on_recv_list;
bool executing_command;
uint8_t *ptr_output_data;
PanId_t nwk_pan_id;

s_data_command *first_to_read;
s_data_command *last_to_read;
s_data_command *data_command;
s_data_command *first_execute_list;
s_data_command *last_execute_list;
s_data_command *first_output_list;
s_data_command *last_output_list;
s_data_command *error_message;

void startVisualizer(void);
void showStartingNwk(void);
void showStartedNwk(void);
void showAirTxStart(void);
void showAirTxStop(void);
void showSerialTx(void);
void showAirRxStart(void);
void stopVisualizer(void);
void stopVisualizer(void);
