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
static uint8_t app_rx_buffer[APP_RX_BUFFER_SIZE];
uint8_t offset_rx_buffer;
static uint8_t cmd_state;
static uint16_t recv_data_len;
char str[100];
uint8_t *ptr_data_recv;
uint8_t commands_on_list;
s_data_command *first_to_read;
s_data_command *last_to_read;
s_data_command *data_command;
static bool usart_tx_busy;

/****************************************************************
Prototypes
****************************************************************/

void init_usart(void);
static void recv_bytes(uint16_t read_bytes_len);
static void write_bytes(void);
void stop_usart(void);
void send_data_usart(void *data, uint8_t data_length); 
void free_data_recv_buffer(void);
uint8_t add_byte_to_buffer(uint8_t data);
void flush_app_buffer(void);
void analize_data_received(void);
void put_on_list(s_data_command *cmd);
void print_entire_list(void);

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
    cmd_state = WAIT_FOR_CMD_STATE;
    offset_rx_buffer = 0;

    // Init dynamic lists
    first_to_read = NULL;
    last_to_read = NULL;
    commands_on_list = 0;

    usart_tx_busy = false;
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
                // Analize recv data 
                analize_data_received();
                return;
            }
            else
            {
                // Need to flush buffer and send an error message for max buffer size reached
                flush_app_buffer();
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

void analize_data_received(void)
{
    if (offset_rx_buffer >= 3)
    {
        if (cmd_state == WAIT_FOR_CMD_STATE)
        {
            if (app_rx_buffer[0] == HEADER_FROM_COMPUTER)
            {
                // Obtain the lenght of data received. 
                recv_data_len = (uint16_t) (app_rx_buffer[1] << 8);
                recv_data_len |= (uint8_t) app_rx_buffer[2];
                if (recv_data_len >= 6 && recv_data_len <= 261)
                {
                    cmd_state = CMD_DETECTED_STATE;
                }
                else
                {
                    // Wrong length
                    flush_app_buffer();
                    cmd_state = WAIT_FOR_CMD_STATE;
                }
                
                
            }
            else
            {
                // Invalid header. Flush buffer, pull error message.
                flush_app_buffer();
                cmd_state = WAIT_FOR_CMD_STATE;
            }
        }
        else if (cmd_state == CMD_DETECTED_STATE)
        {
            if (offset_rx_buffer == recv_data_len)
            {
                // Checking for end of data.
                if (app_rx_buffer[offset_rx_buffer-1] == FOOTER_FROM_COMPUTER)
                {
                    // Extract received data. 
                    data_command = (s_data_command *) malloc(sizeof(s_data_command));
                    data_command->id_message = app_rx_buffer[3];
                    data_command->command = app_rx_buffer[4];
                    data_command->data_length = offset_rx_buffer - 6;
                    data_command->next_command = NULL;
                    if (data_command->data_length > 0)
                    {
                        data_command->data = (uint8_t *) malloc(data_command->data_length);
                        for (int i = 0; i < data_command->data_length; i++)
                        {
                            *(data_command->data + i) = app_rx_buffer[i + 5];
                        }
                        
                    }
                    else
                    {
                        data_command->data = NULL;
                    }
                    sprintf(str, "%d", offset_rx_buffer);
                    send_data_usart(str, strlen(str));
                    flush_app_buffer();
                    cmd_state = WAIT_FOR_CMD_STATE;
                    put_on_list(data_command);
                    
                    // Only for testing
                    if (data_command->command == 0x99)
                    {
                        print_entire_list();    
                    }
                    // End test
                    
                }
                else
                {
                    // Invalid footer. Flush buffer. Pull error message.
                    flush_app_buffer();
                    cmd_state = WAIT_FOR_CMD_STATE;
                }
            
            }
        }
    }
}

void print_entire_list(void)
{
    
    if (first_to_read != NULL && commands_on_list > 0)
    {
        s_data_command *ptr;
        ptr = first_to_read;
        while(ptr->next_command != NULL)
        {
                sprintf(str, "%d", ptr->command);
                usart_tx_busy = true;
                send_data_usart(str, strlen(str));
                ptr = ptr->next_command;
        }
    }
}

void put_on_list(s_data_command *cmd)
{
    if (commands_on_list <= MAX_COMMAND_LIST_SIZE)
    {
        if (first_to_read == NULL && commands_on_list == 0)
        {
            // The list it is empty.
            first_to_read = cmd;
            last_to_read = cmd;
            commands_on_list++;
        }
        else
        {
            last_to_read->next_command = cmd;
            last_to_read = cmd;
            commands_on_list++;
        }
    }
    else
    {
        // The list is full, push error 
        
    }
}


// Callback Tx
// Conf of sent bytes was received. 
static void write_bytes(void)
{
  usart_tx_busy = false;
  free_data_recv_buffer();
  return;
}

void free_data_recv_buffer(void)
{
  free(ptr_data_recv);
}


/**
 * Flush the rx_buffer
 */

void flush_rx_buffer(uint16_t data_len)
{
  uint8_t *ptr = (uint8_t *) malloc(sizeof(uint8_t)*data_len);
  HAL_ReadUsart(&usart_descriptor, ptr, data_len);
  free(ptr);
}


/**
 * Send data througth usart
 * @param data Void pointer, data_length of data to be send.
 */ 

void send_data_usart(void *data, uint8_t data_length)
{
  HAL_WriteUsart(&usart_descriptor, data, data_length);
}

