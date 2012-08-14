
/****************************************************************
Functions for Usart configuration and comunication
****************************************************************/

#include "UsartController.h"


/****************************************************************
Types
****************************************************************/

static HAL_UsartDescriptor_t usartDescriptor;
static uint8_t rxBuffer[USART_RX_BUFFER_LENGTH];
static uint8_t cmdState;
static uint16_t recvDataLen;

/****************************************************************
Prototypes
****************************************************************/


void InitUsart(void);
static void recvBytes(uint16_t readBytesLen);
static void writeBytes(void);
void stopUsart(void);
void sendDataUsart(void *data, uint8_t dataLength);
char str[100];
uint8_t *ptrDataRecv; 

/****************************************************************
Implementation
****************************************************************/

/****************************************************************
InitUsart
****************************************************************/

void InitUsart(void)
{
  // Define Usart Descriptor 
  usartDescriptor.tty = USART_CHANNEL;
  usartDescriptor.mode = USART_MODE_ASYNC;
  usartDescriptor.flowControl = USART_FLOW_CONTROL_NONE;
  usartDescriptor.baudrate = USART_BAUDRATE_38400;
  usartDescriptor.dataLength = USART_DATA8;
  usartDescriptor.parity = USART_PARITY_NONE;
  usartDescriptor.stopbits = USART_STOPBIT_1;
  usartDescriptor.rxBuffer = rxBuffer;
  usartDescriptor.rxBufferLength = USART_RX_BUFFER_LENGTH;
  usartDescriptor.txBuffer = NULL;
  usartDescriptor.txBufferLength = 0;
  usartDescriptor.rxCallback = recvBytes;
  usartDescriptor.txCallback = writeBytes;

  HAL_OpenUsart(&usartDescriptor);
  cmdState = WAIT_FOR_CMD_STATE;
}


// Close Usart 
void stopUsart(void){
  HAL_CloseUsart(&usartDescriptor);
}

// Callback Rx

static void recvBytes(uint16_t readBytesLen)
{
  
  if (cmdState == WAIT_FOR_CMD_STATE)
    {
      if (readBytesLen == 3 )
        {
          if (rxBuffer[readBytesLen-3] == HEADER_FROM_COMPUTER)
            {
              recvDataLen = (uint16_t) (rxBuffer[1] << 8) ;
              recvDataLen |= (uint8_t) rxBuffer[2];
              cmdState = CMD_DETECTED_STATE;
            }
          else
            {
              // Apply flush here
              ptrDataRecv =(uint8_t *) malloc(sizeof(uint8_t)*readBytesLen); // 3 bytes
              HAL_ReadUsart(&usartDescriptor, ptrDataRecv, sizeof(uint8_t)*readBytesLen);
              sprintf(str, "Entro\n");
              sendDataUsart(str, strlen(str));
              cmdState = WAIT_FOR_CMD_STATE;
            }
        }

    }
  else if (cmdState == CMD_DETECTED_STATE)
    {
      if (readBytesLen == recvDataLen)
        {
          if(rxBuffer[recvDataLen-1] == FOOTER_FROM_COMPUTER)
            {
              // Data is correct
              int tmp;
              ptrDataRecv = (uint8_t *) malloc(sizeof(uint8_t)*recvDataLen);
              tmp = HAL_ReadUsart(&usartDescriptor, ptrDataRecv, recvDataLen);
              sprintf(str, "Data : %x %x %x %x %x\n", ptrDataRecv[0], ptrDataRecv[1], ptrDataRecv[2], ptrDataRecv[3], ptrDataRecv[4]);
              sendDataUsart(str, strlen(str));
              cmdState = WAIT_FOR_CMD_STATE;
              recvDataLen = 0;
            }
          else
            {
              sprintf(str, "Data Error");
              sendDataUsart(str, strlen(str));
              flushRxBuffer(recvDataLen);
              cmdState = WAIT_FOR_CMD_STATE;
            }
        }
    }
}

// Callback Tx
// Conf of sent bytes was received. 
static void writeBytes(void)
{
  freeDataRecvBuffer();
  return;
}

void freeDataRecvBuffer(void)
{
  free(ptrDataRecv);
}

void flushRxBuffer(uint16_t dataLen)
{
  uint8_t *ptr = (uint8_t *) malloc(sizeof(uint8_t)*dataLen);
  HAL_ReadUsart(&usartDescriptor, ptr, dataLen);
  free(ptr);
}




void sendDataUsart(void *data, uint8_t dataLength)
{
  HAL_WriteUsart(&usartDescriptor, data, dataLength);
}

