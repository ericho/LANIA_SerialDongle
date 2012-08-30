/**
 * @file UsartController.c 
 * @brief Implementation of USART relationed functions. Message interpreter and Protocol
 * 
 * @author Erich Cordoba
 * @date 08/01/2012
 */ 
#include "UsartController.h"


/****************************************************************
Types
****************************************************************/

static HAL_UsartDescriptor_t usart_descriptor;
static uint8_t rx_buffer[USART_RX_BUFFER_LENGTH];
uint8_t *ptr_data_recv;

/****************************************************************
Prototypes
****************************************************************/

void init_usart(void);
static void recv_bytes(uint16_t read_bytes_len);
static void write_bytes(void);
void stop_usart(void);
void send_data_usart(void *data, uint8_t data_length); 
uint8_t add_byte_to_buffer(uint8_t data);
void flush_app_buffer(void);


/****************************************************************
Implementation
****************************************************************/

/****************************************************************
InitUsart
****************************************************************/

void init_usart(void)
{
    // Define Usart Descriptor 
    usart_descriptor.tty = USART_CHANNEL;
    usart_descriptor.mode = USART_MODE_ASYNC;
    usart_descriptor.flowControl = USART_FLOW_CONTROL_NONE;
    usart_descriptor.baudrate = USART_BAUDRATE_38400;
    usart_descriptor.dataLength = USART_DATA8;
    usart_descriptor.parity = USART_PARITY_NONE;
    usart_descriptor.stopbits = USART_STOPBIT_1;
    usart_descriptor.rxBuffer = rx_buffer;
    usart_descriptor.rxBufferLength = sizeof(rx_buffer);
    usart_descriptor.txBuffer = NULL;
    usart_descriptor.txBufferLength = 0;
    usart_descriptor.rxCallback = recv_bytes;
    usart_descriptor.txCallback = write_bytes;

    HAL_OpenUsart(&usart_descriptor);
    offset_rx_buffer = 0    ;
    usart_tx_busy = false;
    new_data_on_buffer = false;
//    test = true;
}


// Close Usart 
void stop_usart(void){
  HAL_CloseUsart(&usart_descriptor);
}


/**
 * A callback for USART receive events
 * @param readBytesLen An integer with number of receive bytes
 */

static void recv_bytes (uint16_t read_bytes_len)
{
    uint8_t recv_data;
    if (read_bytes_len == 1)
    {
        if (HAL_ReadUsart(&usart_descriptor, &recv_data, read_bytes_len) == 1)
        {
            if (add_byte_to_buffer(recv_data) == APP_BUFFER_UPDATED)
            {
                new_data_on_buffer = true;
                analize_data_received();        
                return;
            }
            else
            {
                // Need to flush buffer and send an error message for max buffer size reached
                flush_app_buffer();
                // Push error 
                return;
            }
        }
    }
}

/**
 * Adds bytes into the appRxBuffer 
 * @param data The data to be added. 
 * @return APP_BUFFER_UPDATED if data was saved,
 * MAX_APP_BUFFER_SIZE_REACHED if appRxBuffer is full 
 */ 
// Adds a bytes into the app buffer 

uint8_t add_byte_to_buffer(uint8_t data)
{
    if(offset_rx_buffer < APP_RX_BUFFER_SIZE)
    {
        app_rx_buffer[offset_rx_buffer] = data;
        offset_rx_buffer++;
        return APP_BUFFER_UPDATED;
    }
    else
    {
        return MAX_APP_BUFFER_SIZE_REACHED;
    }
}
 
/**
 * Resets the app_rx_buffer by setting offset to zero
 */ 

void flush_app_buffer(void)
{
    offset_rx_buffer = 0;
}

// Callback Tx
// Conf of sent bytes was received. 
static void write_bytes(void)
{
  usart_tx_busy = false;
  return;
}


/**
 * Send data througth usart
 * @param data Void pointer, data_length of data to be send.
 */ 

void send_data_usart(void *data, uint8_t data_length)
{
    usart_tx_busy = true;
    HAL_WriteUsart(&usart_descriptor, data, data_length);
}




