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

static ZDO_Neib_t neighborTable[CS_NEIB_TABLE_SIZE];
//static void recvBytes(uint16_t readBytesLen);
//static void writeBytes(void);
void endFunc(void);
void getRssiLqi();
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
            send_data_usart("Iniciando\n", strlen("Iniciando\n"));
            networkDescriptor.endpoint = LANIA_ENDPOINT;
            networkDescriptor.AppProfileId = LANIA_PROFILE_ID;
            networkDescriptor.AppDeviceId = LANIA_DEVICE_ID;
            networkDescriptor.AppDeviceVersion = LANIA_DEVICE_VERSION;
            showStartingNwk();

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
            changeGlobalTask();
            break;
        case PROCESS_MESSAGE:
            BSP_ToggleLed(LED_GREEN);
            executing_command = true;
            move_first_to_execute();
            // Execute command !!
			move_executed_to_output();
            if (first_output_list != NULL)
            {
                stateApp = SENDING_MESSAGE_STATE;
            }
            else
                stateApp = ANALIZE_MESSAGE_STATE;
			BSP_ToggleLed(LED_YELLOW);
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

void APS_DataIndCoord(APS_DataInd_t *inData)
{
    // send data throught usart
    //  sendDataUsart("Datos\n", 6);
    //  getChildrenAddr();
    showAirRxStart();
    laniaMessage = (laniaTest_t  *)inData->asdu;
    getRssiLqi();
    showAirRxStop();
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
    send_data_usart(x, strlen(x));
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
