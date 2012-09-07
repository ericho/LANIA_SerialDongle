/**
 * @file protocol_parser.c 
 * @brief Protocol analizer, managment of FIFO lists. 
 * 
 * @author Erich Cordoba
 * @date 08/01/2012
 */ 
#include "SerialDongle.h"

#define MAX_COMMAND_LIST_SIZE 30

#define WAIT_FOR_CMD_STATE 0
#define CMD_DETECTED_STATE 1
#define ON_CMD_STATE 2

#define HEADER_FROM_COMPUTER 0x55
#define FOOTER_FROM_COMPUTER 0xAA
#define HEADER_TO_COMPUTER 0x98
#define HEADER_ERROR_TO_COMPUTER 0x98
#define FOOTER_TO_COMPUTER 0xCC


#define WRONG_HEADER		0x50
#define INCORRECT_MSG_SIZE	0x51
#define DUPLICATED_MSG_ID	0x52
#define WRONG_FOOTER		0x53
#define ACK_MSG				0x54


