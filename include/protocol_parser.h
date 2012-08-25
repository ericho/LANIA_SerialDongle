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
#define FOOTER_TO_COMPUTER 0xCC




