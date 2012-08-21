#include <types.h>
#include <usart.h>
#include <rs232Controller.h>
#include "SerialDongle.h"

#define USART_CHANNEL USART_CHANNEL_0
#define USART_RX_BUFFER_LENGTH 5
#define RX_BUFFER_SIZE 5
#define APP_RX_BUFFER_SIZE 270
#define MAX_COMMAND_LIST_SIZE 30

#define WAIT_FOR_CMD_STATE 0
#define CMD_DETECTED_STATE 1
#define ON_CMD_STATE 2

#define HEADER_FROM_COMPUTER 0x55
#define FOOTER_FROM_COMPUTER 0xAA
#define HEADER_TO_COMPUTER 0x98
#define FOOTER_TO_COMPUTER 0xCC


#define APP_BUFFER_UPDATED 0
#define MAX_APP_BUFFER_SIZE_REACHED 1

typedef struct s_data_command
{
    uint8_t id_message;
    uint8_t command;
    uint8_t data_length;
    uint8_t *data;
    struct s_data_command *next_command;
}s_data_command;

