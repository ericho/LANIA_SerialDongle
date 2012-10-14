
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

// Struct of data to be sent to nodes

typedef struct
{
	uint8_t command;
	uint8_t op_mode;
	uint8_t samples;
	uint8_t time;
}app_command_node_t;

typedef struct
{
	uint8_t header[APS_ASDU_OFFSET];
	app_command_node_t command;
	uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET];
}app_request_node_t;

// Struct of data received from nodes

typedef struct
{
    uint8_t nodeType;
    uint8_t tipo_tarjeta;
    ExtAddr_t   extAddr;
    ShortAddr_t shortAddr;
    uint8_t     lqi;
    int8_t      rssi;
    uint16_t    sensor_humedad;
    uint16_t    sensor_inclinacion;
    uint16_t    sensor_precipitacion;
    uint16_t    sensor_desplazamiento;
    uint16_t    bateria;
    uint16_t    temperatura;
} app_message_node_t;

void init_usart(void);
void changeGlobalTask(void);
static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo);
void APS_DataIndCoord(APS_DataInd_t *InData);

static AppState_t stateApp = INIT_STATE;
extern SimpleDescriptor_t networkDescriptor;


uint8_t offset_rx_buffer;
ZDO_Neib_t neighborTable[CS_NEIB_TABLE_SIZE];
uint8_t app_rx_buffer[APP_RX_BUFFER_SIZE];
bool usart_tx_busy;
bool new_data_on_buffer;
char str[100];
uint8_t commands_on_recv_list;
bool executing_command;
uint8_t *ptr_output_data;
bool recv_activated;
PanId_t nwk_pan_id;
APS_DataReq_t message_params;

s_data_command *first_to_read;
s_data_command *last_to_read;
s_data_command *data_command;
s_data_command *first_execute_list;
s_data_command *last_execute_list;
s_data_command *first_output_list;
s_data_command *last_output_list;
s_data_command *error_message;
s_data_command *received_from_node;

app_message_node_t *data_from_node;
uint8_t *serialize_from_node;

void startVisualizer(void);
void showStartingNwk(void);
void showStartedNwk(void);
void showAirTxStart(void);
void showAirTxStop(void);
void showSerialTx(void);
void showAirRxStart(void);
void stopVisualizer(void);
void stopVisualizer(void);

#define DATA_FROM_NODE 0x07