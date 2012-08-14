#include <types.h>
#include <usart.h>
#include <rs232Controller.h>
#include "SerialDongle.h"

#define USART_CHANNEL USART_CHANNEL_0
#define USART_RX_BUFFER_LENGTH 200
#define RX_BUFFER_SIZE 100

#define WAIT_FOR_CMD_STATE 0
#define CMD_DETECTED_STATE 1
#define ON_CMD_STATE 2

#define HEADER_FROM_COMPUTER 0x55
#define FOOTER_FROM_COMPUTER 0xAA
#define HEADER_TO_COMPUTER 0x98
#define FOOTER_TO_COMPUTER 0xCC
