/****************************************************************
Includes  
****************************************************************/
#include "SerialDongle.h"


/****************************************************************
Prototypes
****************************************************************/


static APS_RegisterEndpointReq_t endPoint;
static ZDO_StartNetworkReq_t networkReq;
static ZDO_StartNetworkConf_t *confInfo;
static DeviceType_t deviceType;
SimpleDescriptor_t networkDescriptor;


static laniaTest_t  *laniaMessage;
static uint8_t lqiNode;
static int8_t rssiNode;

static NodeAddr_t childrenTable[6];
static ZDO_GetChildrenAddr_t children = 
    {
      .childrenCount = 6,
      .childrenTable = childrenTable,
    };


//static void recvBytes(uint16_t readBytesLen);
//static void writeBytes(void);
void endFunc(void);
void getRssiLqi();
static void APS_DataConf(APS_DataConf_t *confInfo);
void getFirstShortAddr(ZDO_GetLqiRssi_t* nodo);
/****************************************************************
Implementation
****************************************************************/

/****************************************************************
Main Task Handler
****************************************************************/

void APL_TaskHandler(void)
{
    switch(stateApp)
    {
        case INIT_STATE:
            init_usart();
            startVisualizer();
            init_lists();
            recv_activated = false;
            send_data_usart("Iniciando\n", strlen("Iniciando\n"));
            networkDescriptor.endpoint = LANIA_ENDPOINT;
            networkDescriptor.AppProfileId = LANIA_PROFILE_ID;
            networkDescriptor.AppDeviceId = LANIA_DEVICE_ID;
            networkDescriptor.AppDeviceVersion = LANIA_DEVICE_VERSION;
            showStartingNwk();

			message_params.profileId   = LANIA_PROFILE_ID;
			message_params.dstAddrMode = APS_SHORT_ADDRESS;
			//message_params.dstEndpoint = LANIA_ENDPOINT;
			message_params.dstEndpoint = APS_BROADCAST_ENDPOINT;
			message_params.clusterId   = CPU_TO_LE16(1);
			message_params.srcEndpoint = networkDescriptor.endpoint;
			message_params.txOptions.acknowledgedTransmission = 1;
			message_params.radius      = 0x0;
			message_params.APS_DataConf = APS_DataConf;
			
            recv_activated = false;
            // Saves descriptor on endPoint. 
            endPoint.simpleDescriptor = &networkDescriptor;
            endPoint.APS_DataInd = APS_DataIndCoord;
            APS_RegisterEndpointReq(&endPoint);

            // Saving devicetype 
            deviceType = DEVICE_TYPE_COORDINATOR;
            CS_WriteParameter(CS_DEVICE_TYPE_ID, &deviceType);

            // Starting network
            networkReq.ZDO_StartNetworkConf = ZDO_StartNetworkConf;
            ZDO_StartNetworkReq(&networkReq);
            
            break;

        case STARTING_NETWORK_STATE:
            break;

        case IN_NETWORK_STATE:

            if (confInfo->status == ZDO_SUCCESS_STATUS)
            {
                // Send usart data 
                // getChildrenAddr();          
                showStartedNwk();
                nwk_pan_id = confInfo->PANId;
            }
            else
            {
                // Send data 
                send_data_usart("Red error\n", 10);
            }
            stateApp = ANALIZE_MESSAGE_STATE;
            changeGlobalTask();
            break;

        case ANALIZE_MESSAGE_STATE:
            if (first_to_read != NULL && !executing_command)
            {
                stateApp = PROCESS_MESSAGE;
            }
			else if (first_output_list != NULL)
			{
				stateApp = SENDING_MESSAGE_STATE;
			}
            changeGlobalTask();
            break;
        case PROCESS_MESSAGE:
            BSP_ToggleLed(LED_GREEN);
            executing_command = true;
			move_first_to_execute();
			if (parse_command(first_execute_list))
			{
				move_executed_to_output();
            	if (first_output_list != NULL)
            	{
                	stateApp = SENDING_MESSAGE_STATE;
            	}
			}
			else
			{
				stateApp = ANALIZE_MESSAGE_STATE;
			}
			executing_command = false;
            changeGlobalTask();
            break;
        case SENDING_MESSAGE_STATE:
            BSP_ToggleLed(LED_RED);
            send_first_output();
            executing_command = false;
            stateApp = ANALIZE_MESSAGE_STATE;
            changeGlobalTask();
            //BSP_ToggleLed(LED_RED);
            break;
        case STOP_STATE:
            send_data_usart("FIN", strlen("FIN"));
            //BSP_OnLed(LED_RED);
            //endFunc();
            break;
    }
}



void changeGlobalTask(void)
{
    SYS_PostTask(APL_TASK_ID);
}

static void ZDO_StartNetworkConf(ZDO_StartNetworkConf_t *confirmInfo)
{
    stateApp = IN_NETWORK_STATE;
    //  sendDataUsart("Conf\n", 5);
    confInfo = confirmInfo;
    changeGlobalTask();
}

static void APS_DataConf(APS_DataConf_t *confInfo)
{
	if(confInfo->status == APS_SUCCESS_STATUS)
	{
		move_executed_to_output();
		BSP_OnLed(LED_YELLOW);
	}
	else
	{
		move_executed_to_output();
		sprintf(str, "No %x", confInfo->status);
		send_data_usart(str, strlen(str));
	}
}

void APS_DataIndCoord(APS_DataInd_t *inData)
{
    if (recv_activated)
    {
        app_message_node_t *data_recv = (app_message_node_t *) inData->asdu;
        received_from_node = (s_data_command *) malloc(sizeof(s_data_command));
		received_from_node->id_message = 0;
		received_from_node->command = DATA_FROM_NODE;	
		received_from_node->data_length = 17;//sizeof(app_message_node_t);
		received_from_node->next_command = NULL;
		received_from_node->data = (uint8_t *) malloc(sizeof(uint8_t) * 17);
		serialize_message_node(received_from_node->data, data_recv);
		put_on_output_list(received_from_node);
    }
    else
    {
        BSP_ToggleLed(LED_RED);
    }
    //showAirRxStop();
}

void serialize_message_node(uint8_t *dest, app_message_node_t *orig)
{
	//dest = (uint8_t *) malloc(sizeof(uint8_t)*17);
	*(dest) = orig->tipo_tarjeta;
	*(dest + 1) = (uint8_t) (orig->shortAddr >> 8) & 0x00ff;
	*(dest + 2) = (uint8_t) (orig->shortAddr & 0x00ff);
	*(dest + 3) = orig->lqi;
	*(dest + 4) = (uint8_t) orig->rssi;
	*(dest + 5) = (uint8_t) (orig->sensor_humedad >> 8) & 0x00ff;
	*(dest + 6) = (uint8_t) (orig->sensor_humedad & 0x00ff);
	*(dest + 7) = (uint8_t) (orig->sensor_inclinacion >> 8) & 0x00ff;
	*(dest + 8) = (uint8_t) (orig->sensor_inclinacion & 0x00ff);
	*(dest + 9) = (uint8_t) (orig->sensor_precipitacion >> 8) & 0x00ff;
	*(dest + 10) = (uint8_t) (orig->sensor_precipitacion & 0x00ff);
	*(dest + 11) = (uint8_t) (orig->sensor_desplazamiento >> 8) & 0x00ff;
	*(dest + 12) = (uint8_t) (orig->sensor_desplazamiento & 0x00ff);
	*(dest + 13) = (uint8_t) (orig->bateria >> 8) & 0x00ff;
	*(dest + 14) = (uint8_t) (orig->bateria & 0x00ff);
	*(dest + 15) = (uint8_t) (orig->temperatura >> 8) & 0x00ff;
	*(dest + 16) = (uint8_t) (orig->temperatura & 0x00ff);
}

void endFunc(void)
{
    for(;;);
}


void getChildrenAddr()
{
    //      ZDO_GetChildrenAddr(&children);
    ZDO_GetNeibTable(neighborTable);

    uint16_t actualNumberChildren = neighborTable[0].shortAddr;
    char x[10];
    sprintf(x, "%x", actualNumberChildren);
    send_data_usart(&actualNumberChildren, 2);
}


void getRssiLqi(){
    ZDO_GetLqiRssi_t getrl;
    showAirTxStart();
    getFirstShortAddr(&getrl);
    ZDO_GetLqiRssi(&getrl);
    char x[50];
    sprintf(x, "SD Lqi, %d, Rssi, %d, Nd lqi, %d, Rssi, %d\n", getrl.lqi, getrl.rssi, laniaMessage->lqi, laniaMessage->rssi);
//    send_data_usart(x, strlen(x));
    showAirTxStop();
}

void getFirstShortAddr(ZDO_GetLqiRssi_t* nodo){
    ZDO_GetNeibTable(neighborTable);
    nodo->nodeAddr = neighborTable[0].shortAddr;
}


/*******************************************************************************
  Description: just a stub.
  Parameters: are not used.
  Returns: nothing.
*******************************************************************************/

void ZDO_MgmtNwkUpdateNotf(ZDO_MgmtNwkUpdateNotf_t *nwkParams) 
{
    nwkParams = nwkParams;  // Unused parameter warning prevention  
}

/*******************************************************************************
  Description: just a stub.
  Parameters: none.
  Returns: nothing.
*******************************************************************************/

void ZDO_WakeUpInd(void) 
{

}

#ifdef _BINDING_

/***********************************************************************************
  Stub for ZDO Binding Indication
  Parameters:
    bindInd - indication
  Return:
    none
 ***********************************************************************************/

void ZDO_BindIndication(ZDO_BindInd_t *bindInd) 
{
    (void)bindInd;
}

/***********************************************************************************

  Stub for ZDO Unbinding Indication
  Parameters:
    unbindInd - indication
  Return:
    none
 ***********************************************************************************/

void ZDO_UnbindIndication(ZDO_UnbindInd_t *unbindInd)
{
    (void)unbindInd;
}

#endif //_BINDING_


/****************************************************************
Main program
****************************************************************/
// Infinite loop
int main(void)
{
    SYS_SysInit();
    for(;;)
    {
        SYS_RunTask();
    }
}
